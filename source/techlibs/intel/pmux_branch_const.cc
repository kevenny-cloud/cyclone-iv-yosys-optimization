// R1: substitute facts established by a PMUX lane's equality condition.
// Designed against Yosys 0.69; build with the matching yosys-config.
#include "kernel/yosys.h"
#include "kernel/sigtools.h"
#include "kernel/celltypes.h"

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

// Structural safety limits, deliberately independent of names and options.
// These thresholds have not been calibrated with production measurements.
struct BranchBudgetExceeded {};
struct BranchBudget {
	static constexpr int64_t module_limit = 8000000, design_limit = 32000000;
	static constexpr int64_t module_candidates = 4096, design_candidates = 16384;
	int64_t &design_work, &design_count;
	int64_t work = 0, count = 0;
	BranchBudget(int64_t &w, int64_t &c) : design_work(w), design_count(c) {}
	void spend(int64_t n) {
		if (n < 0) throw BranchBudgetExceeded();
		if (n > design_limit - design_work) {
			design_work = design_limit;
			throw BranchBudgetExceeded();
		}
		if (n > module_limit - work)
			throw BranchBudgetExceeded();
		work += n; design_work += n;
	}
	void candidate() {
		if (count >= module_candidates || design_count >= design_candidates)
			throw BranchBudgetExceeded();
		++count; ++design_count;
	}
};

// Read sizes before constructing SigMap or any per-bit driver/fact objects.
// Charge traversal as well as bits, including aliases and opaque cell ports.
static void branch_graph_preflight(RTLIL::Module *module, BranchBudget &budget)
{
	for (auto wire : module->wires())
		budget.spend(1 + int64_t(wire->width) * 3);
	for (const auto &conn : module->connections())
		budget.spend(1 + (int64_t(conn.first.size()) + conn.second.size()) * 3);
	for (auto cell : module->cells()) {
		budget.spend(1);
		for (const auto &conn : cell->connections())
			budget.spend(1 + int64_t(conn.second.size()) * 3);
	}
}

static void branch_register_types(RTLIL::Design *design, RTLIL::Module *module, CellTypes &types, BranchBudget &budget)
{
	for (auto cell : module->cells()) {
		budget.spend(1);
		if (!types.cell_known(cell->type)) {
			auto target = design->module(cell->type);
			if (target) {
				budget.spend(1 + int64_t(target->ports.size()));
				types.setup_module(target);
			}
		}
	}
}

struct DriverInfo {
	int count = 0;
	RTLIL::Cell *cell = nullptr;
	RTLIL::IdString port;
};

struct BranchConstWorker {
	RTLIL::Module *module;
	SigMap sigmap;
	dict<RTLIL::SigBit, DriverInfo> drivers;
	pool<RTLIL::SigBit> protected_bits;
	dict<RTLIL::SigBit, dict<RTLIL::SigBit, RTLIL::State>> facts_cache;
	int changed_cells = 0, replaced_bits = 0;
	bool dry_run, verbose;
	BranchBudget &budget;

	BranchConstWorker(RTLIL::Module *module, const CellTypes &types, bool dry_run, bool verbose, BranchBudget &budget)
		: module(module), sigmap(module), dry_run(dry_run), verbose(verbose), budget(budget)
	{
		// Scan ALL cells/ports, even when only one PMUX is selected for editing.
		for (auto wire : module->wires()) {
			for (auto bit : sigmap(RTLIL::SigSpec(wire))) {
				if (wire->get_bool_attribute(ID::keep))
					protected_bits.insert(bit);
				if (wire->port_input)
					add_driver(bit, nullptr, RTLIL::IdString());
			}
		}
		for (auto cell : module->cells()) {
			for (const auto &conn : cell->connections()) {
				bool output = types.cell_output(cell->type, conn.first);
				bool unknown = !types.cell_known(cell->type) ||
					(!output && !types.cell_input(cell->type, conn.first));
				// An opaque port might drive the net; do not assume it is an input.
				if (output || unknown)
					for (auto bit : sigmap(conn.second))
						add_driver(bit, cell, conn.first);
			}
		}
	}

	void add_driver(RTLIL::SigBit bit, RTLIL::Cell *cell, RTLIL::IdString port)
	{
		if (!bit.wire)
			return;
		auto &info = drivers[bit];
		if (info.count == 0) {
			info.cell = cell;
			info.port = port;
		}
		// Only uniqueness matters; saturating avoids integer overflow.
		if (info.count < 2) ++info.count;
	}

	bool protected_cell(RTLIL::Cell *cell)
	{
		if (cell->get_bool_attribute(ID::keep))
			return true;
		for (const auto &conn : cell->connections()) {
			budget.spend(1 + int64_t(conn.second.size()));
			for (auto bit : sigmap(conn.second))
				if (protected_bits.count(bit))
					return true;
		}
		return false;
	}

	bool param_matches(RTLIL::Cell *cell, RTLIL::IdString param, int value)
	{
		if (!cell->hasParam(param))
			return false;
		const auto &p = cell->getParam(param);
		budget.spend(1 + int64_t(p.size()));
		return p.is_fully_def() && p.try_as_int() == value;
	}

	bool known_constant(const RTLIL::SigSpec &signal)
	{
		budget.spend(1 + int64_t(signal.size()));
		for (auto bit : signal)
			if (bit.wire || (bit.data != RTLIL::State::S0 && bit.data != RTLIL::State::S1))
				return false;
		return signal.size() > 0;
	}

	const dict<RTLIL::SigBit, RTLIL::State> &facts(RTLIL::SigBit select)
	{
		budget.spend(1);
		if (facts_cache.count(select))
			return facts_cache.at(select);
		auto &result = facts_cache[select];
		auto it = drivers.find(select);
		if (it == drivers.end() || it->second.count != 1 || it->second.cell == nullptr || it->second.port != ID::Y)
			return result;
		auto cmp = it->second.cell;
		if (cmp->type != ID($eq) || protected_cell(cmp) ||
			!cmp->hasPort(ID::A) || !cmp->hasPort(ID::B) || !cmp->hasPort(ID::Y))
			return result;
		budget.spend(1 + int64_t(cmp->getPort(ID::A).size()) + cmp->getPort(ID::B).size() + cmp->getPort(ID::Y).size());
		auto a = sigmap(cmp->getPort(ID::A));
		auto b = sigmap(cmp->getPort(ID::B));
		auto y = sigmap(cmp->getPort(ID::Y));
		if (a.empty() || a.size() != b.size() || y.size() != 1 || y[0] != select ||
			!param_matches(cmp, ID::A_WIDTH, a.size()) || !param_matches(cmp, ID::B_WIDTH, b.size()) ||
			!param_matches(cmp, ID::Y_WIDTH, 1))
			return result;
		// Equal widths avoid implicit sign/zero extension. Signedness does not
		// change bitwise equality for equal-width operands.
		if (known_constant(a))
			std::swap(a, b);
		if (!known_constant(b))
			return result;
		budget.spend(int64_t(a.size()) * 2); // Fact visits and cache storage.
		for (int i = 0; i < a.size(); ++i) {
			if (!a[i].wire || (result.count(a[i]) && result.at(a[i]) != b[i].data)) {
				result.clear(); // Mixed constant operands or contradictory aliases.
				return result;
			}
			result[a[i]] = b[i].data;
		}
		return result;
	}

	void run(const std::vector<RTLIL::Cell *> &candidates)
	{
		for (auto cell : candidates) {
			try {
			budget.spend(1);
			if (cell->type != ID($pmux) || protected_cell(cell) ||
				!cell->hasPort(ID::A) || !cell->hasPort(ID::B) || !cell->hasPort(ID::S) || !cell->hasPort(ID::Y))
				continue;
			budget.spend(1 + int64_t(cell->getPort(ID::A).size()) +
				int64_t(cell->getPort(ID::B).size()) * 4 + cell->getPort(ID::S).size());
			auto a = cell->getPort(ID::A);
			auto b = cell->getPort(ID::B); // Preserve original aliases on untouched bits.
			auto canonical_b = sigmap(b);
			auto s = sigmap(cell->getPort(ID::S));
			int width = a.size();
			if (width <= 0 || s.empty() || cell->getPort(ID::Y).size() != width ||
				int64_t(width) * s.size() != b.size() ||
				!param_matches(cell, ID::WIDTH, width) || !param_matches(cell, ID::S_WIDTH, s.size()))
				continue;
			int changes = 0;
			for (int lane = 0; lane < s.size(); ++lane) {
				const auto &known = facts(s[lane]);
				for (int k = 0; k < width; ++k) {
					int pos = lane * width + k;
					auto fact = known.find(canonical_b[pos]);
					if (fact != known.end()) {
						b[pos] = RTLIL::SigBit(fact->second);
						++changes;
					}
				}
			}
			if (changes) {
				if (verbose)
					log("  %s.%s: %s %d branch data bits.\n", log_id(module), log_id(cell),
						dry_run ? "would replace" : "replaced", changes);
				if (!dry_run)
					cell->setPort(ID::B, b);
				++changed_cells;
				replaced_bits += changes;
			}
			} catch (const BranchBudgetExceeded &) {
				// All edits above use a local B; setPort occurs only after completion.
				log("  R1 %s.%s: bit-work/candidate budget reached; current and remaining candidates left unchanged.\n",
					log_id(module), log_id(cell));
				break;
			}
		}
	}
};

struct PmuxBranchConstPass : public Pass {
	PmuxBranchConstPass() : Pass("pmux_branch_const", "R1: propagate equality facts inside PMUX branches") {}

	void help() override
	{
		log("\n    pmux_branch_const [-dry-run] [-verbose] [selection]\n\n");
		log("For each selected $pmux, inspect equality-driven select bits. If a lane\n");
		log("is selected by X == CONST, replace occurrences of X bits in that lane's\n");
		log("B data with the corresponding constant bits. The A default is preserved.\n\n");
		log("Run after proc (normally proc; opt_clean). Only equal-width $eq with a\n");
		log("fully defined binary constant and uniquely driven select is supported.\n");
		log("Aliases are canonicalized. keep modules/cells/connected wires, modules\n");
		log("with remaining processes, X/Z constants and ambiguous drivers are skipped.\n\n");
		log("  -dry-run    Report opportunities without changing the netlist.\n");
		log("  -verbose    Report each changed PMUX and its replaced bit count.\n\n");
		log("Safety budgets include candidate collection, graph bits and fact caches:\n");
		log("8,000,000 work units/4,096 candidates per module; 32,000,000/16,384\n");
		log("per pass invocation. On exhaustion, the current candidate is unchanged.\n\n");
		log("No branch exclusivity is invented: existing $pmux semantics apply.\n");
		log("No timing, target-area, or arbitrary four-state equivalence is promised.\n\n");
	}

	void execute(std::vector<std::string> args, RTLIL::Design *design) override
	{
		bool dry_run = false, verbose = false;
		size_t argidx = 1;
		for (; argidx < args.size(); ++argidx) {
			if (args[argidx] == "-dry-run") dry_run = true;
			else if (args[argidx] == "-verbose") verbose = true;
			else break;
		}
		extra_args(args, argidx, design);
		log_header(design, "Executing PMUX_BRANCH_CONST pass (R1 branch facts).\n");
		// Register only types actually used by admitted modules. setup(design)
		// would eagerly traverse unrelated modules even with no PMUX candidates.
		CellTypes types;
		types.setup();
		int64_t cells = 0, bits = 0, design_work = 0, design_count = 0;
		for (auto module : design->modules()) {
			if (design_work >= BranchBudget::design_limit || design_count >= BranchBudget::design_candidates) {
				log("  R1 design budget reached; remaining modules left unchanged.\n");
				break;
			}
			++design_work; // Bound module-only designs, including skipped modules.
			if (!design->selected(module)) continue;
			if (module->get_bool_attribute(ID::keep) || module->get_blackbox_attribute())
				continue;
			if (module->has_processes()) {
				log("  Skipping %s: run proc first.\n", log_id(module));
				continue;
			}
			BranchBudget budget(design_work, design_count);
			try {
				std::vector<RTLIL::Cell *> candidates;
				for (auto cell : module->cells()) {
					budget.spend(1);
					if (cell->type != ID($pmux) || !module->selected(cell) || cell->get_bool_attribute(ID::keep))
						continue;
					if (!cell->hasPort(ID::A) || !cell->hasPort(ID::B) || !cell->hasPort(ID::S) || !cell->hasPort(ID::Y))
						continue;
					int64_t width = cell->getPort(ID::A).size(), lanes = cell->getPort(ID::S).size();
					if (width <= 0 || lanes <= 0 || width * lanes != cell->getPort(ID::B).size() ||
						cell->getPort(ID::Y).size() != width) continue;
					budget.candidate();
					budget.spend(1 + width + int64_t(cell->getPort(ID::B).size()) + lanes);
					candidates.push_back(cell);
				}
				if (candidates.empty()) continue;
				branch_graph_preflight(module, budget);
				branch_register_types(design, module, types, budget);
				BranchConstWorker worker(module, types, dry_run, verbose, budget);
				worker.run(candidates);
				cells += worker.changed_cells;
				bits += worker.replaced_bits;
			} catch (const BranchBudgetExceeded &) {
				log("  R1 %s: preflight bit-work/candidate budget reached; module left unchanged before driver graph construction.\n", log_id(module));
			}
		}
		design->scratchpad_set_bool("pmux_branch_const.matches", bits != 0);
		if (bits && !dry_run)
			design->scratchpad_set_bool("opt.did_something", true);
		log("R1 summary: matched_cells=%lld replaced_bits=%lld dry_run=%d\n", (long long)cells, (long long)bits, int(dry_run));
	}
} PmuxBranchConstPass;

PRIVATE_NAMESPACE_END
