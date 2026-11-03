// R2: reconstruct uniquely decoded PMUX tables as shared mux trees.
// The historical complete-table mode is unchanged by default. -partial is an
// explicit experimental extension which fills missing binary selector values
// with the original PMUX A port, never with an invented don't-care value.
// -force is an offline exploration option, not an area-safety guarantee.
// Implements matrix/logic-sharing ideas, not the reference parser's text edits.
#include "kernel/yosys.h"
#include "kernel/sigtools.h"
#include "kernel/celltypes.h"
#include <algorithm>
#include <numeric>
#include <map>
#include <tuple>

#ifdef near
#undef near
#endif

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

// Structural limits, not measured performance thresholds. The historical
// 2,000,000 search limit below remains independent and unchanged.
struct TreeBudgetExceeded {};
struct TreeBudget {
    static constexpr int64_t module_limit = 8000000, design_limit = 32000000;
    static constexpr int64_t module_candidates = 4096, design_candidates = 16384;
    int64_t &design_work, &design_count;
    int64_t work = 0, count = 0;
    TreeBudget(int64_t &w, int64_t &c) : design_work(w), design_count(c) {}
    void spend(int64_t n) {
        if (n < 0) throw TreeBudgetExceeded();
        if (n > design_limit - design_work) {
            design_work = design_limit;
            throw TreeBudgetExceeded();
        }
        if (n > module_limit - work)
            throw TreeBudgetExceeded();
        work += n; design_work += n;
    }
    void candidate() {
        if (count >= module_candidates || design_count >= design_candidates)
            throw TreeBudgetExceeded();
        ++count; ++design_count;
    }
};

static void tree_graph_preflight(Module *m, TreeBudget &budget) {
    for (auto w : m->wires()) budget.spend(1 + int64_t(w->width) * 3);
    for (const auto &p : m->connections())
        budget.spend(1 + (int64_t(p.first.size()) + p.second.size()) * 3);
    for (auto c : m->cells()) {
        budget.spend(1);
        for (const auto &p : c->connections()) budget.spend(1 + int64_t(p.second.size()) * 3);
    }
}

static void tree_register_types(Design *design, Module *m, CellTypes &types, TreeBudget &budget) {
    for (auto c : m->cells()) {
        budget.spend(1);
        if (!types.cell_known(c->type)) {
            auto target = design->module(c->type);
            if (target) {
                budget.spend(1 + int64_t(target->ports.size()));
                types.setup_module(target);
            }
        }
    }
}

struct Driver { int count = 0; Cell *cell = nullptr; IdString port; };
struct Table { Cell *cell; SigSpec control; std::vector<SigSpec> rows; };

struct TreeWorker {
    Module *module;
    SigMap sm;
    dict<SigBit, Driver> drivers;
    dict<SigBit,pool<Cell*>> consumers;pool<SigBit> external_outputs;pool<Cell*> blocked_region;
    pool<SigBit> kept_bits;
    bool dry, verbose, scalar, lut4, near, keep_decode, partial, force;
    std::string forced_order;
    int leaf, rank, changed = 0, nodes = 0;
    int64_t work = 0;
    static constexpr int64_t work_limit = 2000000;
    TreeBudget &budget;

    TreeWorker(Module *m, const CellTypes &types, bool dry, bool verbose, bool scalar, int leaf,
               bool lut4, std::string forced_order, bool near, bool keep_decode, int rank, bool partial, bool force, TreeBudget &budget)
        : module(m), sm(m), dry(dry), verbose(verbose), scalar(scalar), lut4(lut4),
          near(near), keep_decode(keep_decode), partial(partial), force(force), forced_order(forced_order), leaf(leaf), rank(rank), budget(budget) {
        for (auto w : m->wires()) for (auto b : sm(SigSpec(w))) {
            if (w->get_bool_attribute(ID::keep) || (partial && w->attributes.count(ID::init))) kept_bits.insert(b);
            if (w->port_input) add_driver(b, nullptr, IdString());
            if(w->port_output)external_outputs.insert(b);
        }
        for (auto c : m->cells()) for (const auto &p : c->connections()) {
            if (types.cell_output(c->type, p.first) || !types.cell_known(c->type) ||
                !types.cell_input(c->type, p.first))
                for (auto b : sm(p.second)) add_driver(b, c, p.first);
            if(partial&&!leaf&&(!types.cell_known(c->type)||types.cell_input(c->type,p.first)))
                for(auto b:sm(p.second))if(b.wire)consumers[b].insert(c);
        }
        if(partial&&!leaf) {
            pool<Cell*> muxes;dict<Cell*,pool<Cell*>> adjacency;
            for(auto c:m->cells())if(c->type==ID($pmux))muxes.insert(c);
            for(auto c:muxes)if(c->hasPort(ID::Y))for(auto bit:sm(c->getPort(ID::Y))) {
                budget.spend(1+GetSize(consumers[bit]));
                if(GetSize(consumers[bit])+int(external_outputs.count(bit))>1)blocked_region.insert(c);
                for(auto user:consumers[bit])if(muxes.count(user)) {adjacency[c].insert(user);adjacency[user].insert(c);}
            }
            std::vector<Cell*> pending;for(auto c:blocked_region)pending.push_back(c);
            for(int k=0;k<GetSize(pending);k++)for(auto c:adjacency[pending[k]]) {
                budget.spend(1);if(blocked_region.insert(c).second)pending.push_back(c);
            }
        }
    }
    void add_driver(SigBit b, Cell *c, IdString port) {
        if (!b.wire) return;
        auto &d = drivers[b];
        if (d.count == 0) { d.cell = c; d.port = port; }
        if (d.count < 2) ++d.count;
    }
    bool protected_cell(Cell *c) {
        if (c->get_bool_attribute(ID::keep)) return true;
        for (const auto &p : c->connections()) {
            budget.spend(1 + int64_t(p.second.size()));
            for (auto b : sm(p.second)) if (kept_bits.count(b)) return true;
        }
        return false;
    }
    bool param(Cell *c, IdString p, int n) {
        if (!c->hasParam(p)) return false;
        const auto &value = c->getParam(p);
        budget.spend(1 + int64_t(value.size()));
        return value.is_fully_def() && value.try_as_int() == n;
    }
    bool binary(SigSpec s) {
        budget.spend(1 + int64_t(s.size()));
        return !s.empty() && s.is_fully_const() && s.as_const().is_fully_def();
    }
    bool decode(SigBit s, SigSpec &control, int &value) {
        budget.spend(1);
        if (!drivers.count(s)) return false;
        const auto &d = drivers.at(s);
        if (d.count != 1 || !d.cell || d.port != ID::Y) return false;
        auto c = d.cell;
        if (!c->type.in(ID($eq), ID($logic_not)) || protected_cell(c) ||
            !c->hasPort(ID::A) || !c->hasPort(ID::Y) ||
            sm(c->getPort(ID::Y)) != SigSpec(s) || !param(c, ID::Y_WIDTH, 1)) return false;
        budget.spend(1 + int64_t(c->getPort(ID::A).size()));
        control = sm(c->getPort(ID::A));
        if (!param(c, ID::A_WIDTH, control.size())) return false;
        value = 0;
        if (c->type == ID($eq)) {
            if (!c->hasPort(ID::B)) return false;
            budget.spend(1 + int64_t(c->getPort(ID::B).size()));
            auto b = sm(c->getPort(ID::B));
            if (b.size() != control.size() || !param(c, ID::B_WIDTH, b.size())) return false;
            if (binary(control)) std::swap(control, b);
            if (!binary(b) || b.size() > 5) return false;
            value = b.as_const().as_int();
        }
        if (control.size() < 1 || control.size() > 5) return false;
        pool<SigBit> seen;
        for (auto b : control) {
            if (!b.wire || seen.count(b)) return false;
            seen.insert(b);
        }
        return true;
    }
    bool identify(Cell *c, Table &t) {
        if (c->type != ID($pmux) || protected_cell(c)) return false;
        for (auto p : {ID::A, ID::B, ID::S, ID::Y}) if (!c->hasPort(p)) return false;
        budget.spend(1 + int64_t(c->getPort(ID::A).size()) +
            int64_t(c->getPort(ID::B).size()) * 2 + c->getPort(ID::S).size() + c->getPort(ID::Y).size());
        auto a = sm(c->getPort(ID::A)), b = sm(c->getPort(ID::B));
        auto s = sm(c->getPort(ID::S)), y = sm(c->getPort(ID::Y));
        int width = a.size();
        if (width < 1 || width > 4096 || s.size() < 2 || s.size() > 32 ||
            y.size() != width || b.size() != width * s.size() ||
            !param(c, ID::WIDTH, width) || !param(c, ID::S_WIDTH, s.size())) return false;
        // Output aliases/drivers are checked before replacing its driver.
        pool<SigBit> outputs;
        for (auto bit : y) {
            if (!bit.wire || outputs.count(bit) || !drivers.count(bit)) return false;
            auto d = drivers.at(bit);
            if (d.count != 1 || d.cell != c || d.port != ID::Y) return false;
            outputs.insert(bit);
        }
        // The local table cost does not model shared downstream cones.
        // Retain their original one-hot representation conservatively.
        if(partial&&!leaf&&blocked_region.count(c))return false;
        t.cell = c;
        pool<int> seen;
        for (int i = 0; i < s.size(); ++i) {
            SigSpec control; int value;
            if (!decode(s[i], control, value)) return false;
            if (i == 0) {
                t.control = control;
                int domain = 1 << control.size();
                if ((!partial && s.size() != domain) || s.size() > domain) return false;
                // Mutual exclusion is proven below by equal canonical control
                // signals and distinct fully defined equality constants. This
                // is not a conversion of arbitrary priority/select signals.
                if (partial) {
                    budget.spend(1 + int64_t(domain) * width);
                    t.rows.assign(domain, a);
                } else t.rows.resize(domain);
            }
            if (control != t.control || seen.count(value)) return false;
            seen.insert(value);
            t.rows[value] = b.extract(i * width, width);
        }
        if(partial)for(const auto &row:t.rows)for(auto bit:row) {
            budget.spend(1);
            if(!bit.wire&&bit.data!=State::S0&&bit.data!=State::S1)return false;
        }
        return true;
    }

    // A symbolic DAG is built before any mutation. Negative refs are existing
    // SigSpecs; nonnegative refs are nodes. Identical subfunctions share nodes.
    struct Plan {
        struct Node { SigSpec control; std::vector<int> rows; int width; int override_index = -1; };
        bool near = false;
        int64_t *work = nullptr;
        TreeBudget *budget = nullptr;
        std::vector<Node> dag;
        std::vector<SigSpec> terminals;
        std::map<SigSpec, int> term_ids;
        std::map<std::tuple<SigSpec, std::vector<int>, int>, int> cache;
        std::vector<std::vector<int>> roots;
        int64_t cost = 0;
        int terminal(SigSpec s) {
            // Map comparisons/hash-equivalent key inspection scale with bus
            // width; pay before lookups and before allocating terminal keys.
            budget->spend(1 + int64_t(s.size()) * (2 + key_depth(term_ids.size())));
            auto it = term_ids.find(s);
            if (it != term_ids.end()) return it->second;
            int id = -1 - GetSize(terminals);
            terminals.push_back(s); term_ids[s] = id; return id;
        }
        int node(SigSpec c, const std::vector<int> &rows, int width) {
            budget->spend(1 + (int64_t(c.size()) + GetSize(rows)) * (2 + key_depth(cache.size())));
            if (std::all_of(rows.begin(), rows.end(), [&](int r) { return r == rows[0]; })) return rows[0];
            auto key = std::make_tuple(c, rows, width);
            auto it = cache.find(key);
            if (it != cache.end()) return it->second;
            if (near && c.size() >= 2) {
                for (const auto &item : cache) {
                    budget->spend(1 + int64_t(c.size()) + GetSize(rows));
                    *work += 1 + GetSize(rows);
                    if (*work > TreeWorker::work_limit) throw 1;
                    const auto &k = item.first;
                    if (std::get<0>(k) != c || std::get<2>(k) != width ||
                        GetSize(std::get<1>(k)) != GetSize(rows) || dag[item.second].override_index >= 0) continue;
                    int different = 0, index = -1;
                    for (int j = 0; j < GetSize(rows); ++j)
                        if (rows[j] != std::get<1>(k)[j]) { ++different; index = j; }
                    if (different != 1) continue;
                    int id = GetSize(dag);
                    dag.push_back({c, {item.second, rows[index]}, width, index});
                    cache[key] = id; cost += width + 1; return id;
                }
            }
            int id = GetSize(dag); dag.push_back({c, rows, width}); cache[key] = id;
            cost += int64_t(width) * (GetSize(rows) - 1);
            return id;
        }
        static int key_depth(size_t size) {
            int depth = 1;
            while (size) { ++depth; size >>= 1; }
            return depth;
        }
    };
    int tree(Plan &p, const SigSpec &control, const std::vector<int> &order,
             const std::vector<SigSpec> &data, int level, int width) {
        work += GetSize(data);
        if (work > work_limit) throw 1;
        budget.spend(1 + int64_t(GetSize(data)) * (int64_t(width) + 2));
        if (std::all_of(data.begin(), data.end(), [&](SigSpec v) { return v == data[0]; })) return p.terminal(data[0]);
        int rem = GetSize(order) - level;
        if (leaf && rem <= leaf) {
            SigSpec c; for (int k = level; k < GetSize(order); ++k) c.append(control[order[k]]);
            std::vector<int> rows;
            for (const auto &v : data) rows.push_back(p.terminal(v));
            return p.node(c, rows, width);
        }
        std::vector<SigSpec> lo, hi;
        // Data index bits are in the remaining order, with the current split LSB.
        for (int k = 0; k < GetSize(data); ++k) (k & 1 ? hi : lo).push_back(data[k]);
        int a = tree(p, control, order, lo, level + 1, width);
        int b = tree(p, control, order, hi, level + 1, width);
        return p.node(SigSpec(control[order[level]]), {a, b}, width);
    }
    Plan make_plan(const std::vector<Table> &tables, const std::vector<int> &order) {
        Plan p; p.near = near; p.work = &work; p.budget = &budget;
        for (const auto &t : tables) {
            int width = t.rows[0].size();
            std::vector<int> roots;
            for (int bit = 0; bit < (scalar ? width : 1); ++bit) {
                budget.spend(1 + int64_t(GetSize(t.rows)) *
                    (int64_t(GetSize(order)) + (scalar ? 1 : width)));
                std::vector<SigSpec> data;
                for (int k = 0; k < GetSize(t.rows); ++k) {
                    int address = 0;
                    for (int j = 0; j < GetSize(order); ++j) address |= ((k >> j) & 1) << order[j];
                    data.push_back(scalar ? t.rows[address].extract(bit, 1) : t.rows[address]);
                }
                roots.push_back(tree(p, t.control, order, data, 0, scalar ? 1 : width));
            }
            p.roots.push_back(roots);
        }
        return p;
    }
    // Reserve all materialization work before the first netlist mutation.
    // A group is the transaction because its tables share one symbolic DAG.
    void reserve_emit(const std::vector<Table> &tables, const Plan &p) {
        for (const auto &n : p.dag) {
            // Covers wires, row assembly, mux ports and LUT truth tables.
            // <=5 decode bits, <=3 leaf bits, so 64 per row is conservative.
            budget.spend(1 + int64_t(n.width) * (int64_t(GetSize(n.rows)) + 1) * 64);
        }
        for (const auto &t : tables) budget.spend(1 + int64_t(t.cell->getPort(ID::Y).size()) * 2);
    }
    void emit(const std::vector<Table> &tables, const Plan &p) {
        std::vector<SigSpec> outputs;
        auto ref = [&](int i) { return i < 0 ? p.terminals.at(-1-i) : outputs.at(i); };
        std::map<std::pair<SigSpec, int>, SigBit> decoders;
        for (const auto &n : p.dag) {
            SigSpec y = module->addWire(NEW_ID, n.width);
            if (n.override_index >= 0) {
                auto key = std::make_pair(n.control, n.override_index);
                if (!decoders.count(key)) {
                    SigBit s = decode_wire();
                    module->addEq(NEW_ID, n.control, Const(n.override_index, n.control.size()), s);
                    decoders[key] = s;
                }
                module->addMux(NEW_ID, ref(n.rows[0]), ref(n.rows[1]), decoders[key], y);
            } else if (n.control.size() == 1)
                module->addMux(NEW_ID, ref(n.rows[0]), ref(n.rows[1]), n.control, y);
            else if (lut4) {
                // Explicit 4-input LUT cuts retain shared decoded enables across
                // wide data paths. Boolean equivalence is checked with EQY.
                std::vector<SigBit> sels;
                for (int k = 0; k < GetSize(n.rows); ++k) {
                    auto key = std::make_pair(n.control, k);
                    if (!decoders.count(key)) {
                        SigBit s = decode_wire();
                        Const truth(State::S0, 1 << n.control.size()); truth.set(k, State::S1);
                        make_lut(n.control, truth, s);
                        decoders[key] = s;
                    }
                    sels.push_back(decoders[key]);
                }
                for (int bit = 0; bit < n.width; ++bit) {
                    std::vector<SigBit> terms;
                    for (int k = 0; k < GetSize(n.rows); k += 2) {
                        SigSpec inputs; inputs.append(sels[k]); inputs.append(ref(n.rows[k])[bit]);
                        inputs.append(sels[k+1]); inputs.append(ref(n.rows[k+1])[bit]);
                        SigBit term = module->addWire(NEW_ID);
                        Const truth(State::S0, 16);
                        for (int v = 0; v < 16; ++v)
                            if (((v & 3) == 3) || ((v & 12) == 12)) truth.set(v, State::S1);
                        make_lut(inputs, truth, term); terms.push_back(term);
                    }
                    SigSpec inputs; for (auto t : terms) inputs.append(t);
                    Const truth(State::S1, 1 << inputs.size()); truth.set(0, State::S0);
                    make_lut(inputs, truth, y[bit]);
                }
            } else {
                SigSpec sels, data;
                for (int k = 0; k < GetSize(n.rows); ++k) {
                    auto key = std::make_pair(n.control, k);
                    if (!decoders.count(key)) {
                        SigBit s = decode_wire();
                        module->addEq(NEW_ID, n.control, Const(k, n.control.size()), s);
                        decoders[key] = s;
                    }
                    sels.append(decoders[key]); data.append(ref(n.rows[k]));
                }
                module->addPmux(NEW_ID, SigSpec(State::Sx, n.width), data, sels, y);
            }
            outputs.push_back(y);
        }
        for (int i = 0; i < GetSize(tables); ++i) {
            SigSpec result; for (int r : p.roots[i]) result.append(ref(r));
            module->connect(tables[i].cell->getPort(ID::Y), result);
            module->remove(tables[i].cell);
        }
    }
    SigBit decode_wire() {
        auto w = module->addWire(NEW_ID);
        if (keep_decode) w->set_bool_attribute(ID::keep);
        return SigBit(w);
    }
    void make_lut(SigSpec a, Const truth, SigBit y) {
        auto c = module->addCell(NEW_ID, ID($lut));
        c->setParam(ID::WIDTH, a.size()); c->setParam(ID::LUT, truth);
        c->setPort(ID::A, a); c->setPort(ID::Y, y);
    }

    bool all_outputs_fit_lut4(const std::vector<Table> &tables) {
        for (const auto &t : tables) {
            int width=t.rows[0].size(), count=GetSize(t.rows);
            for (int bit=0; bit<width; ++bit) {
                pool<SigBit> support;
                for (const auto &row : t.rows) {
                    budget.spend(1);
                    auto value=row[bit];
                    if (value.wire) support.insert(value);
                    else if (value.data != State::S0 && value.data != State::S1) return false;
                    if (GetSize(support)>4) return false;
                }
                for (int control=0; control<GetSize(t.control); ++control) {
                    bool depends=false;
                    for (int row=0; row<count; ++row) if (!(row & (1<<control))) {
                        budget.spend(2);
                        if (t.rows[row][bit] != t.rows[row | (1<<control)][bit]) {depends=true;break;}
                    }
                    if (depends) support.insert(t.control[control]);
                    if (GetSize(support)>4) return false;
                }
            }
        }
        return true;
    }

    void run(std::vector<Cell *> selected) {
        std::map<SigSpec, std::vector<Table>> groups;
        std::sort(selected.begin(), selected.end(), [](Cell *a, Cell *b) { return a->name < b->name; });
        try {
            for (auto c : selected) { Table t; if (identify(c, t)) groups[t.control].push_back(t); }
        } catch (const TreeBudgetExceeded &) {
            log("  R2 %s: candidate collection bit-work budget reached; module left unchanged.\n", log_id(module));
            return;
        }
        for (auto &g : groups) {
            try {
                if (!force && all_outputs_fit_lut4(g.second)) {
                    log("  R2 direct-LUT4 support guard: retaining %d complete tables.\n", GetSize(g.second));
                    continue;
                }
            } catch (const TreeBudgetExceeded &) {
                log("  R2 support budget reached; current and remaining groups retained.\n");
                break;
            }

            std::vector<int> order(g.first.size()); std::iota(order.begin(), order.end(), 0);
            if (!forced_order.empty()) {
                if (forced_order.size() != order.size()) continue;
                pool<int> seen;
                for (int j = 0; j < GetSize(order); ++j) {
                    int v = forced_order[j] - '0';
                    if (v < 0 || v >= GetSize(order) || seen.count(v))
                        log_cmd_error("-order must be a permutation for its control width.\n");
                    seen.insert(v); order[j] = v;
                }
            }
            Plan best; std::vector<int> best_order; int trials = 0;
            std::vector<std::pair<std::vector<int>, Plan>> ranked;
            int64_t old_cost = 0;
            for (const auto &t : g.second) {
                int explicit_rows = GetSize(t.cell->getPort(ID::S));
                int old_rows = partial && explicit_rows < GetSize(t.rows) ? explicit_rows + 1 : GetSize(t.rows);
                old_cost += int64_t(t.rows[0].size()) * (old_rows-1);
            }
            try {
                do {
                    auto p = make_plan(g.second, order); ++trials;
                    ranked.emplace_back(order, std::move(p));
                    std::sort(ranked.begin(), ranked.end(), [](const auto &a, const auto &b) {
                        if (a.second.cost != b.second.cost) return a.second.cost < b.second.cost;
                        return a.first < b.first;
                    });
                    if (GetSize(ranked) > rank + 1) ranked.pop_back();
                } while (forced_order.empty() && std::next_permutation(order.begin(), order.end()));
            } catch (int) {
                log("  R2 %s: 2000000 search work limit reached; current and remaining groups left unchanged.\n", log_id(module));
                break;
            } catch (const TreeBudgetExceeded &) {
                log("  R2 %s: bit-work budget reached during planning; current and remaining groups left unchanged.\n", log_id(module));
                break;
            }
            if (rank >= GetSize(ranked)) continue;
            best_order = ranked[rank].first; best = std::move(ranked[rank].second);
            // Binary decoded PMUX normalization may keep the data-mux cost
            // equal while eliminating redundant one-hot decode structure.
            // Do not use unknown default/row bits as invented don't-cares.
            bool normalized_binary = partial && !leaf && !near && !lut4 && best.cost == old_cost;
            if (!force && (best.cost>old_cost || (best.cost==old_cost&&!normalized_binary))) continue;
            try { reserve_emit(g.second, best); }
            catch (const TreeBudgetExceeded &) {
                log("  R2 %s: bit-work budget reached before emission; current and remaining groups left unchanged.\n", log_id(module));
                break;
            }
            if (verbose) {
                log("  %s: tables=%d scalar=%d leaf=%d cost=%lld -> %lld trials=%d order=",
                    log_id(module), GetSize(g.second), int(scalar), leaf,
                    (long long)old_cost, (long long)best.cost, trials);
                for (int v : best_order) log("%d,", v);
                log("\n");
            }
            changed += GetSize(g.second); nodes += GetSize(best.dag);
            if (!dry) emit(g.second, best);
        }
    }
};

struct DecodeTreePass : Pass {
    DecodeTreePass() : Pass("pmux_decode_tree", "R2: share complete decoded PMUX tables") {}
    void help() override {
        log("\n    pmux_decode_tree [-dry-run] [-verbose] [-vector] [-leaf 0|2|3]\n");
        log("                     [-lut4] [-near] [-keep-decode] [-rank 0..7]\n");
        log("                     [-order DIGITS] [-partial [-force]] [selection]\n\n");
        log("-partial also accepts incomplete equality domains, filling every missing\n");
        log("Partial tables reject X/Z row bits. Binary leaf0 normalization retains\n");
        log("whole shared PMUX regions and may accept equal symbolic mux cost.\n");
        log("binary address with the original default A input. No initial-state\n");
        log("normalization is performed by this pass. Keep and init wire attributes\n");
        log("are boundaries in partial mode. Full four-state X/Z equivalence is not\n");
        log("claimed: equalities and a binary mux tree can propagate unknown control\n");
        log("bits differently. Prove in the declared defined-input formal model.\n");
        log("-force requires -partial and disables structural-cost/support acceptance\n");
        log("guards for OFFLINE MEASUREMENTS ONLY. Mapped area can increase.\n\n");
        log("Recognize complete 1..5-bit equality decode domains and share mux subtrees.\n");
        log("The default uses scalar sharing; -vector retains whole data buses.\n");
        log("-leaf keeps 2/3-bit subfunctions as PMUX leaves. Orders are searched with\n");
        log("-lut4 emits shared decoded AND/OR leaves as explicit <=4-input LUTs.\n");
        log("-near shares leaf tables differing in one row, with an exact override.\n");
        log("-keep-decode preserves only newly added shared decoder outputs for mapping.\n");
        log("-order fixes a root-to-leaf permutation (e.g. 0123) for exploration.\n");
        log("-rank chooses one of the top 8 structural orders for measured evaluation.\n");
        log("Search retains a 2,000,000 row-visit budget per module.\n");
        log("Collection, graph bits, bus comparisons and emission also have safety\n");
        log("budgets: 8,000,000 work units/4,096 candidates per module and\n");
        log("32,000,000/16,384 per pass invocation. Exhausted groups stay unchanged.\n");
        log("Without -partial, default/partial tables; and always ambiguous\n");
        log("drivers and keep objects are skipped. Run after proc; opt_clean.\n");
        log("Structural cost is a heuristic: measure mapped area and prove equivalence\n");
        log("before accepting a candidate. No arbitrary four-state equivalence claim.\n\n");
    }
    void execute(std::vector<std::string> args, Design *design) override {
        bool dry = false, verbose = false, scalar = true, lut4 = false, near = false, keep_decode = false, partial = false, force = false; int leaf = 0, rank = 0;
        std::string forced_order;
        size_t i = 1;
        for (; i < args.size(); ++i) {
            if (args[i] == "-dry-run") dry = true;
            else if (args[i] == "-verbose") verbose = true;
            else if (args[i] == "-vector") scalar = false;
            else if (args[i] == "-lut4") lut4 = true;
            else if (args[i] == "-near") near = true;
            else if (args[i] == "-keep-decode") keep_decode = true;
            else if (args[i] == "-partial") partial = true;
            else if (args[i] == "-force") force = true;
            else if (args[i] == "-order" && i + 1 < args.size()) forced_order = args[++i];
            else if (args[i] == "-rank" && i + 1 < args.size()) {
                auto v = args[++i];
                if (v.size() != 1 || v[0] < '0' || v[0] > '7') log_cmd_error("-rank must be 0..7.\n");
                rank = v[0] - '0';
            }
            else if (args[i] == "-leaf" && i + 1 < args.size()) {
                auto v = args[++i];
                if (v != "0" && v != "2" && v != "3") log_cmd_error("-leaf must be 0, 2 or 3.\n");
                leaf = std::stoi(v);
            } else break;
        }
        extra_args(args, i, design);
        if (force && !partial) log_cmd_error("-force is an offline option and requires -partial.\n");
        log_header(design, "Executing PMUX_DECODE_TREE pass.\n");
        if (force) log("EXPERIMENTAL -force: structural-cost and direct-LUT4 acceptance guards DISABLED; mapped area may increase.\n");
        CellTypes types; types.setup();
        int64_t changed = 0, nodes = 0, design_work = 0, design_count = 0;
        for (auto m : design->modules()) {
            if (design_work >= TreeBudget::design_limit || design_count >= TreeBudget::design_candidates) {
                log("  R2 design budget reached; remaining modules left unchanged.\n");
                break;
            }
            ++design_work;
            if (!design->selected(m)) continue;
            if (m->has_processes() || m->get_bool_attribute(ID::keep) || m->get_blackbox_attribute()) continue;
            TreeBudget budget(design_work, design_count);
            try {
                std::vector<Cell *> candidates;
                for (auto c : m->cells()) {
                    budget.spend(1);
                    if (c->type != ID($pmux) || !m->selected(c) || c->get_bool_attribute(ID::keep)) continue;
                    if (!c->hasPort(ID::A) || !c->hasPort(ID::B) || !c->hasPort(ID::S) || !c->hasPort(ID::Y)) continue;
                    int64_t width = c->getPort(ID::A).size(), lanes = c->getPort(ID::S).size();
                    if (width < 1 || width > 4096 || lanes < 2 || lanes > 32 ||
                        width * lanes != c->getPort(ID::B).size() || c->getPort(ID::Y).size() != width) continue;
                    budget.candidate();
                    budget.spend(1 + width + int64_t(c->getPort(ID::B).size()) + lanes);
                    candidates.push_back(c);
                }
                if (candidates.empty()) continue;
                tree_graph_preflight(m, budget);
                tree_register_types(design, m, types, budget);
                TreeWorker w(m, types, dry, verbose, scalar, leaf, lut4, forced_order, near, keep_decode, rank, partial, force, budget);
                w.run(std::move(candidates));
                changed += w.changed; nodes += w.nodes;
            } catch (const TreeBudgetExceeded &) {
                log("  R2 %s: preflight bit-work/candidate budget reached; module left unchanged before table emission.\n", log_id(m));
            }
        }
        design->scratchpad_set_bool("pmux_decode_tree.matches", changed != 0);
        if (changed && !dry) design->scratchpad_set_bool("opt.did_something", true);
        log("R2 summary: matched_cells=%lld new_nodes=%lld dry_run=%d partial=%d force=%d\n", (long long)changed, (long long)nodes, int(dry), int(partial), int(force));
    }
} DecodeTreePass;

PRIVATE_NAMESPACE_END
