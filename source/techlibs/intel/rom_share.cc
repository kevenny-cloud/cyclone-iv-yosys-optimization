// R4b: bounded shared decision diagrams for complete, constant, async ROMs.
#include "kernel/yosys.h"
#include "kernel/sigtools.h"
#include "kernel/celltypes.h"
#include <algorithm>
#include <numeric>
#include <map>
#include <set>
#include <tuple>

#ifdef near
#undef near
#endif

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

struct RomDriver { int count = 0; Cell *cell = nullptr; IdString port; };
struct RomTable { RTLIL::Memory *mem; Cell *read, *init; SigSpec address, output; Const data; };

// Conservative engineering caps, not measured runtime or memory guarantees.
// Owned by execute(), so repeated passes and different designs start fresh.
struct RomBudget {
    static constexpr int64_t module_candidates = 4096, design_candidates = 16384;
    static constexpr int64_t module_signals = 2000000, design_signals = 8000000;
    static constexpr int64_t design_search = 8000000;
    int64_t candidates = 0, signals = 0, search = 0;
    bool search_exhausted = false;
};

static bool rom_candidate(Module *m, RTLIL::Memory *mem)
{
    return m->selected(mem) && !mem->get_bool_attribute(ID::keep) && mem->start_offset == 0 &&
        mem->width >= 1 && mem->width <= 256 && mem->size >= 2 && mem->size <= 32 &&
        !(mem->size & (mem->size - 1));
}

// Bound all bulk SigMap/driver/index inputs before constructing the worker.
// Include records as well as bits to cover empty wires/ports/connections.
static bool rom_scope(Module *m, RomBudget &budget, int64_t &signals)
{
    signals = 0;
    auto charge = [&](int64_t amount) {
        if (amount > RomBudget::module_signals - signals ||
            amount > RomBudget::design_signals - budget.signals - signals) return false;
        signals += amount;
        return true;
    };
    for (auto w : m->wires())
        if (!charge(int64_t(w->width) + 1)) return false;
    for (auto &conn : m->connections())
        if (!charge(int64_t(conn.first.size()) + conn.second.size() + 1)) return false;
    for (auto c : m->cells()) {
        if (!charge(1)) return false;
        if (c->hasParam(ID::MEMID) && !charge(c->getParam(ID::MEMID).size())) return false;
        for (auto &p : c->connections())
            if (!charge(int64_t(p.second.size()) + 1)) return false;
    }
    return true;
}

struct RomWorker {
    Module *m;
    SigMap sm;
    dict<SigBit, RomDriver> drivers;
    pool<SigBit> kept;
    dict<std::string, std::vector<Cell *>> memory_users;
    RomBudget &budget;
    int leaf, rank, matched = 0, nodes = 0;
    bool dry, verbose;
    std::string forced_order;
    int64_t work = 0;
    static constexpr int64_t limit = 2000000;
    RomWorker(Module *m, const CellTypes &types, RomBudget &budget, int leaf, int rank, bool dry, bool verbose, std::string order)
        : m(m), sm(m), budget(budget), leaf(leaf), rank(rank), dry(dry), verbose(verbose), forced_order(order) {
        for (auto w : m->wires()) for (auto b : sm(SigSpec(w))) {
            if (w->get_bool_attribute(ID::keep)) kept.insert(b);
            if (w->port_input) add_driver(b, nullptr, IdString());
        }
        for (auto c : m->cells()) {
            // Index every MEMID user, including unknown cell types: identify()
            // must still reject writes, duplicates and unknown memory users.
            if (c->hasParam(ID::MEMID)) memory_users[c->getParam(ID::MEMID).decode_string()].push_back(c);
            for (auto &p : c->connections())
                if (types.cell_output(c->type, p.first) || !types.cell_known(c->type) || !types.cell_input(c->type, p.first))
                    for (auto b : sm(p.second)) add_driver(b, c, p.first);
        }
    }
    void add_driver(SigBit b, Cell *c, IdString port) {
        if (!b.wire) return;
        auto &d = drivers[b];
        if (d.count++ == 0) { d.cell = c; d.port = port; }
    }
    bool param(Cell *c, IdString p, int n) {
        return c->hasParam(p) && c->getParam(p).is_fully_def() && c->getParam(p).try_as_int() == n;
    }
    bool protected_cell(Cell *c) {
        if (c->get_bool_attribute(ID::keep)) return true;
        for (auto &p : c->connections()) for (auto b : sm(p.second)) if (kept.count(b)) return true;
        return false;
    }
    bool identify(RTLIL::Memory *mem, RomTable &t) {
        if (!rom_candidate(m, mem)) return false;
        Cell *rd = nullptr, *init = nullptr;
        auto users = memory_users.find(mem->name.str());
        if (users == memory_users.end()) return false;
        for (auto c : users->second) {
            if (!m->selected(c) || protected_cell(c)) return false;
            if (c->type == ID($memrd_v2) && !rd) rd = c;
            else if (c->type == ID($meminit) && !init) init = c;
            else return false; // No writes, multiple reads/inits, or unknown memory users.
        }
        if (!rd || !init) return false;
        for (auto port : {ID::ADDR, ID::DATA, ID::EN, ID::ARST, ID::SRST}) if (!rd->hasPort(port)) return false;
        for (auto port : {ID::ADDR, ID::DATA}) if (!init->hasPort(port)) return false;
        auto address = sm(rd->getPort(ID::ADDR)), output = sm(rd->getPort(ID::DATA));
        int n = address.size();
        if (n < 1 || n > 5 || (1 << n) != mem->size || output.size() != mem->width ||
            !param(rd, ID::ABITS, n) || !param(rd, ID::WIDTH, mem->width) || !param(rd, ID::CLK_ENABLE, 0) ||
            sm(rd->getPort(ID::EN)) != SigSpec(State::S1) || sm(rd->getPort(ID::ARST)) != SigSpec(State::S0) ||
            sm(rd->getPort(ID::SRST)) != SigSpec(State::S0) ||
            !param(init, ID::WIDTH, mem->width) || !param(init, ID::WORDS, mem->size)) return false;
        auto start = sm(init->getPort(ID::ADDR)), data = sm(init->getPort(ID::DATA));
        if (start.empty() || !start.is_fully_const() || !start.as_const().is_fully_zero() ||
            !param(init, ID::ABITS, start.size()) || !data.is_fully_const() ||
            !data.as_const().is_fully_def() || data.size() != mem->size * mem->width) return false;
        pool<SigBit> seen;
        for (auto b : address) {
            if (!b.wire || seen.count(b) || !drivers.count(b) || drivers.at(b).count != 1) return false;
            seen.insert(b);
        }
        seen.clear();
        for (auto b : output) {
            if (!b.wire || seen.count(b) || !drivers.count(b)) return false;
            const auto &d = drivers.at(b);
            if (d.count != 1 || d.cell != rd || d.port != ID::DATA) return false;
            seen.insert(b);
        }
        t = {mem, rd, init, address, output, data.as_const()};
        return true;
    }
    struct Plan {
        struct Node { std::vector<int> inputs; uint64_t truth; int lo = -1, hi = -1, pivot = -1; };
        std::vector<Node> dag;
        std::vector<int> roots, order;
        std::map<std::pair<std::vector<int>, uint64_t>, int> cache;
        std::set<int> inverses;
        int n, leaf;
        int64_t *work;
        RomBudget *budget;
        enum class Exhausted { Module, Design };
        void charge(int amount) {
            if (amount > RomWorker::limit - *work) throw Exhausted::Module;
            if (amount > RomBudget::design_search - budget->search) {
                budget->search_exhausted = true;
                throw Exhausted::Design;
            }
            *work += amount;
            budget->search += amount;
        }
        // References are (node ID << 1) | inverted; zero/one are refs 0/1.
        int build(std::vector<int> inputs, uint64_t truth) {
            charge(1 << GetSize(inputs));
            for (int j = GetSize(inputs) - 1; j >= 0; --j) {
                bool depends = false;
                for (int k = 0; k < (1 << GetSize(inputs)); ++k)
                    if (!(k & (1 << j)) && ((truth >> k) & 1) != ((truth >> (k | (1 << j))) & 1)) { depends = true; break; }
                if (!depends) {
                    uint64_t reduced = 0; int pos = 0;
                    for (int k = 0; k < (1 << GetSize(inputs)); ++k) if (!(k & (1 << j)))
                        reduced |= ((truth >> k) & 1) << pos++;
                    inputs.erase(inputs.begin() + j); truth = reduced;
                }
            }
            if (inputs.empty()) return int(truth & 1);
            int invert = int(truth & 1);
            if (invert) truth ^= (uint64_t(1) << (1 << GetSize(inputs))) - 1;
            int result;
            if (GetSize(inputs) == 1) result = 2 * (1 + inputs[0]);
            else {
                auto key = std::make_pair(inputs, truth);
                auto it = cache.find(key);
                if (it != cache.end()) result = it->second;
                else {
                    Node node{inputs, truth};
                    if (GetSize(inputs) > leaf) {
                        for (int p : order) if (std::find(inputs.begin(), inputs.end(), p) != inputs.end()) { node.pivot = p; break; }
                        int j = int(std::find(inputs.begin(), inputs.end(), node.pivot) - inputs.begin());
                        std::vector<int> remaining = inputs; remaining.erase(remaining.begin() + j);
                        uint64_t low = 0, high = 0; int l = 0, h = 0;
                        for (int k = 0; k < (1 << GetSize(inputs)); ++k)
                            if (k & (1 << j)) high |= ((truth >> k) & 1) << h++;
                            else low |= ((truth >> k) & 1) << l++;
                        node.lo = build(remaining, low); node.hi = build(remaining, high);
                    }
                    result = 2 * (1 + n + GetSize(dag)); dag.push_back(node); cache[key] = result;
                }
            }
            if (invert) { inverses.insert(result); result ^= 1; }
            return result;
        }
    };
    Plan plan(const RomTable &t, const std::vector<int> &order) {
        Plan p; p.n = t.address.size(); p.leaf = leaf; p.order = order; p.work = &work; p.budget = &budget;
        std::vector<int> inputs(p.n); std::iota(inputs.begin(), inputs.end(), 0);
        for (int bit = 0; bit < t.mem->width; ++bit) {
            uint64_t truth = 0;
            for (int k = 0; k < t.mem->size; ++k) if (t.data[k * t.mem->width + bit] == State::S1) truth |= uint64_t(1) << k;
            p.roots.push_back(p.build(inputs, truth));
        }
        return p;
    }
    void emit(const RomTable &t, const Plan &p) {
        std::vector<SigBit> signals = {SigBit(State::S0)};
        for (auto b : t.address) signals.push_back(b);
        std::map<int, SigBit> inverse;
        auto signal = [&](int ref) {
            if (ref <= 1) return SigBit(ref ? State::S1 : State::S0);
            auto b = signals.at(ref / 2);
            if (!(ref & 1)) return b;
            if (!inverse.count(ref)) {
                auto y = SigBit(m->addWire(NEW_ID)); m->addNot(NEW_ID, b, y); inverse[ref] = y;
            }
            return inverse.at(ref);
        };
        for (const auto &node : p.dag) {
            auto y = SigBit(m->addWire(NEW_ID));
            if (node.pivot >= 0) m->addMux(NEW_ID, signal(node.lo), signal(node.hi), t.address[node.pivot], y);
            else {
                SigSpec address; for (int j : node.inputs) address.append(t.address[j]);
                Const truth(State::S0, 1 << GetSize(node.inputs));
                for (int j = 0; j < truth.size(); ++j) if ((node.truth >> j) & 1) truth.set(j, State::S1);
                m->addLut(NEW_ID, address, y, truth);
            }
            signals.push_back(y);
        }
        SigSpec output; for (int ref : p.roots) output.append(signal(ref));
        m->connect(t.output, output);
        m->remove(t.read); m->remove(t.init);
        m->memories.erase(t.mem->name); delete t.mem;
    }
    void run(const std::vector<IdString> &names) {
        for (auto name : names) {
            RomTable t; if (!identify(m->memories.at(name), t)) continue;
            std::vector<int> order(t.address.size()); std::iota(order.begin(), order.end(), 0);
            bool fixed = !forced_order.empty();
            if (fixed) {
                if (GetSize(forced_order) != GetSize(order)) continue;
                std::vector<int> requested; for (char c : forced_order) requested.push_back(c - '0');
                auto sorted = requested; std::sort(sorted.begin(), sorted.end());
                if (sorted != order) continue;
                order = requested;
            }
            std::vector<Plan> best; int trials = 0; std::set<int> first_pivots;
            try {
                do {
                    // At most one split above a direct LUT leaf: only its first
                    // pivot matters. All suffix permutations emit the same DAG.
                    // Keep explicit rank/order semantics unchanged for callers.
                    if (!fixed && rank == 0 && leaf >= GetSize(order)-1 &&
                        !first_pivots.insert(order[0]).second) continue;
                    best.push_back(plan(t, order)); ++trials;
                    std::sort(best.begin(), best.end(), [](const Plan &a, const Plan &b) {
                        return std::make_tuple(a.dag.size(), a.inverses.size(), a.order) <
                               std::make_tuple(b.dag.size(), b.inverses.size(), b.order);
                    });
                    if (GetSize(best) > rank + 1) best.pop_back();
                } while (!fixed && std::next_permutation(order.begin(), order.end()));
            } catch (Plan::Exhausted exhausted) {
                log("  R4b %s search limit reached in %s.%s (module_work=%lld design_work=%lld); current and later ROMs left unchanged.\n",
                    exhausted == Plan::Exhausted::Module ? "module" : "design", log_id(m), log_id(name),
                    (long long)work, (long long)budget.search);
                break;
            }
            if (rank >= GetSize(best)) continue;
            auto &p = best[rank];
            if (verbose) {
                log("  %s.%s: %dx%d ROM, leaf=%d rank=%d nodes=%d inverses=%d trials=%d order=",
                    log_id(m), log_id(name), t.mem->size, t.mem->width, leaf, rank, GetSize(p.dag), GetSize(p.inverses), trials);
                for (int j : p.order) log("%d", j);
                log("\n");
            }
            ++matched; nodes += GetSize(p.dag);
            if (!dry) emit(t, p);
        }
    }
};

struct RomSharePass : Pass {
    RomSharePass() : Pass("rom_share", "R4b: share bounded constant-ROM decision diagrams") {}
    void help() override {
        log("\n    rom_share [-leaf 2|3|4] [-rank 0..7] [-order DIGITS]\n");
        log("              [-dry-run] [-verbose] [selection]\n\n");
        log("Reconstruct complete 2..32-row, 1..256-bit constant ROMs after proc.\n");
        log("Only one async $memrd_v2 and one complete $meminit are accepted.\n");
        log("No writes, unknown table bits, resets/enables, ambiguous drivers or keep\n");
        log("objects are changed. Select the memory, read and init cells together.\n");
        log("Default leaf=3 rank=0; max 120 orders and 2,000,000 row visits/module.\n");
        log("Resource caps: candidates 4096/module, 16384/pass; signal units\n");
        log("2,000,000/module, 8,000,000/pass; search 8,000,000 row visits/pass.\n");
        log("Limits skip unprocessed ROMs; plans are emitted only after full search.\n");
        log("Measure mapped area and run EQY before using a generated candidate.\n\n");
    }
    void execute(std::vector<std::string> args, Design *design) override {
        int leaf = 3, rank = 0; bool dry = false, verbose = false; std::string order;
        size_t i = 1;
        for (; i < args.size(); ++i) {
            if (args[i] == "-dry-run") dry = true;
            else if (args[i] == "-verbose") verbose = true;
            else if (args[i] == "-leaf" && i + 1 < args.size()) {
                auto v = args[++i]; if (v != "2" && v != "3" && v != "4") log_cmd_error("-leaf must be 2, 3 or 4.\n"); leaf = std::stoi(v);
            } else if (args[i] == "-rank" && i + 1 < args.size()) {
                auto v = args[++i]; if (v.size() != 1 || v[0] < '0' || v[0] > '7') log_cmd_error("-rank must be 0..7.\n"); rank = v[0] - '0';
            } else if (args[i] == "-order" && i + 1 < args.size()) order = args[++i];
            else break;
        }
        extra_args(args, i, design); log_header(design, "Executing ROM_SHARE pass.\n");
        CellTypes types(design); int matched = 0, nodes = 0;
        RomBudget budget;
        int skipped_scope = 0, skipped_candidates = 0, skipped_search = 0, no_candidates = 0;
        for (auto m : design->selected_modules()) {
            if (m->has_processes() || m->get_bool_attribute(ID::keep) || m->get_blackbox_attribute()) continue;
            if (budget.search_exhausted || budget.search >= RomBudget::design_search) { ++skipped_search; continue; }
            std::vector<IdString> names;
            bool candidate_limit = false;
            for (auto &item : m->memories) if (rom_candidate(m, item.second)) {
                if (int64_t(names.size()) >= RomBudget::module_candidates ||
                    int64_t(names.size()) >= RomBudget::design_candidates - budget.candidates) {
                    candidate_limit = true;
                    break;
                }
                names.push_back(item.first);
            }
            // Count candidates inspected in rejected modules too, so many
            // over-limit modules cannot each get a fresh design allowance.
            budget.candidates += names.size();
            if (candidate_limit) {
                ++skipped_candidates;
                log("  R4b candidate limit: module %s left unchanged (module_cap=4096 design_remaining=%lld).\n",
                    log_id(m), (long long)(RomBudget::design_candidates - budget.candidates));
                continue;
            }
            if (names.empty()) { ++no_candidates; continue; }
            // Charge preflight scans even if this module is later rejected.
            int64_t signal_units = 0;
            bool scope_ok = rom_scope(m, budget, signal_units);
            budget.signals += signal_units;
            if (!scope_ok) {
                ++skipped_scope;
                log("  R4b signal limit: module %s left unchanged (scanned_units=%lld module_cap=2000000 design_remaining=%lld).\n",
                    log_id(m), (long long)signal_units, (long long)(RomBudget::design_signals - budget.signals));
                continue;
            }
            std::sort(names.begin(), names.end());
            RomWorker worker(m, types, budget, leaf, rank, dry, verbose, order); worker.run(names);
            matched += worker.matched; nodes += worker.nodes;
        }
        if (matched && !dry) design->scratchpad_set_bool("opt.did_something", true);
        log("R4b summary: matched_roms=%d new_nodes=%d dry_run=%d\n", matched, nodes, int(dry));
        log("R4b resources: candidates=%lld signal_units=%lld search_work=%lld skipped_candidates=%d skipped_signal=%d skipped_search=%d no_candidates=%d\n",
            (long long)budget.candidates, (long long)budget.signals, (long long)budget.search,
            skipped_candidates, skipped_scope, skipped_search, no_candidates);
    }
} RomSharePass;

PRIVATE_NAMESPACE_END
