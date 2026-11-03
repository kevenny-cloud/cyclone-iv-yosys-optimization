// Bounded, Boolean, post-ABC LUT4 resubstitution. Every accepted edit removes
// at least two LUTs, adds one, and is checked over every boundary assignment.
#include "kernel/yosys.h"
#include "kernel/sigtools.h"
#include "kernel/celltypes.h"
#include <algorithm>
#include <array>
#include <bitset>
#include <deque>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <vector>

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

#include "lut_window.h"
#include "lut_ff_load.h"
#include "lut_correlation.h"
#include "ff_enable_fold.h"

struct ResubBudget {
    int64_t used = 0, limit = 80000000, island_stop = 80000000;
    int64_t island_override = 0;
    int max_island_edits = 0; // zero selects the structural output-scaled default
    struct Exhausted { bool global; };
    void charge(int64_t n = 1) {
        if (n > limit - used) throw Exhausted{true};
        if (n > island_stop - used) throw Exhausted{false};
        used += n;
    }
};

struct ResubNode {
    Cell *cell;
    std::vector<int> pins;
    int y;
    uint16_t table;
    bool good;
};

struct Divisor { int bit; int count = 0; };

struct LutResubWorker {
    Module *module;
    CellTypes &types;
    ResubBudget &budget;
    int edits = 0, removed = 0;
    std::set<std::vector<uint64_t>> infeasible;
    LutResubWorker(Module *m, CellTypes &t, ResubBudget &b) : module(m), types(t), budget(b) {}

    bool step() {
        // Bound the inputs to SigMap and connectivity construction before
        // allocating them. This also counts negative/no-match designs.
        int64_t extent = 0;
        for (auto w : module->wires()) extent += int64_t(w->width) + 1;
        for (auto &c : module->connections()) extent += int64_t(c.first.size()) + c.second.size() + 1;
        for (auto c : module->cells()) {
            extent++;
            for (auto &p : c->connections()) extent += int64_t(p.second.size()) + 1;
        }
        budget.charge(extent);
        if (extent > 2000000) return false;
        SigMap sigmap(module);
        dict<SigBit,int> ids;
        std::vector<SigBit> signals = {SigBit(State::S0), SigBit(State::S1)};
        ids[signals[0]] = 0; ids[signals[1]] = 1;
        auto id = [&](SigBit bit) {
            bit = sigmap(bit);
            auto it = ids.find(bit);
            if (it != ids.end()) return it->second;
            int index = GetSize(signals); ids[bit] = index; signals.push_back(bit); return index;
        };
        std::vector<Wire*> wires;
        for (auto w : module->wires()) wires.push_back(w);
        std::sort(wires.begin(), wires.end(), [](Wire *a, Wire *b){return a->name.str() < b->name.str();});
        std::set<int> protected_bits, ports, input_ports;
        for (auto w : wires) for (int j = 0; j < w->width; j++) {
            int b = id(SigBit(w,j));
            if (w->get_bool_attribute(ID::keep) || w->attributes.count(ID::init)) protected_bits.insert(b);
            if (w->port_input || w->port_output) ports.insert(b);
            if (w->port_input) input_ports.insert(b);
        }
        std::vector<Cell*> ordered;
        for (auto c : module->cells()) ordered.push_back(c);
        std::sort(ordered.begin(),ordered.end(),[](Cell*a,Cell*b){return a->name.str()<b->name.str();});
        std::map<int,std::set<int>> drivers, users;
        for (int b : input_ports) drivers[b].insert(-1);
        std::vector<ResubNode> nodes;
        for (int i = 0; i < GetSize(ordered); i++) {
            Cell *c = ordered[i];
            ResubNode n{c,{},-1,0,false};
            bool known = types.cell_known(c->type);
            for (auto &p : c->connections()) for (auto bit : p.second) {
                int b = id(bit);
                if (!known || types.cell_output(c->type,p.first)) drivers[b].insert(i);
                if (!known || types.cell_input(c->type,p.first)) users[b].insert(i);
            }
            if (c->type == ID($lut) && !c->get_bool_attribute(ID::keep) &&
                c->hasPort(ID::A) && c->hasPort(ID::Y) && c->getPort(ID::Y).size() == 1 &&
                c->getPort(ID::A).size() >= 1 && c->getPort(ID::A).size() <= 4 &&
                c->hasParam(ID::LUT) && c->getParam(ID::LUT).is_fully_def() &&
                c->getParam(ID::LUT).size() == (1 << c->getPort(ID::A).size())) {
                n.y = id(c->getPort(ID::Y)[0]); n.good = signals[n.y].wire != nullptr;
                n.table = c->getParam(ID::LUT).as_int();
                for (auto bit : c->getPort(ID::A)) {
                    int b = id(bit); n.pins.push_back(b);
                    if (b > 1 && !signals[b].wire) n.good = false;
                }
            }
            nodes.push_back(n);
        }
        int count = GetSize(nodes);
        std::vector<std::set<int>> deps(count), children(count), support(count);
        std::vector<int> pending(count,0), topo;
        std::set<int> ready;
        for (auto &n : nodes) if (n.good && (protected_bits.count(n.y) || drivers[n.y].size()!=1)) n.good=false;
        for (int i=0;i<count;i++) if(nodes[i].good) {
            for(int b:nodes[i].pins) if(drivers[b].size()==1) {
                int d=*drivers[b].begin();
                if(d>=0 && nodes[d].good) deps[i].insert(d);
            }
            pending[i]=GetSize(deps[i]);
            for(int d:deps[i]) children[d].insert(i);
            if(!pending[i]) ready.insert(i);
        }
        while(!ready.empty()) {
            int n=*ready.begin();ready.erase(ready.begin());topo.push_back(n);
            for(int b:nodes[n].pins) if(b>1) {
                int d=drivers[b].size()==1?*drivers[b].begin():-1;
                if(d>=0 && nodes[d].good && !support[d].empty()) support[n].insert(support[d].begin(),support[d].end());
                else support[n].insert(b);
            }
            budget.charge(GetSize(support[n])+1);
            if(support[n].size()>6) support[n]={nodes[n].y};
            for(int c:children[n]) if(!--pending[c]) ready.insert(c);
        }
        for(auto ri=topo.rbegin();ri!=topo.rend();++ri) {
            int root=*ri, ry=nodes[root].y;
            if(support[root].count(ry) || support[root].size()>6) continue;
            std::set<int> dead={root};
            std::vector<int> queue(deps[root].begin(),deps[root].end());
            while(!queue.empty()) {
                int n=queue.back();queue.pop_back();budget.charge();
                if(dead.count(n) || ports.count(nodes[n].y) || support[root].count(nodes[n].y)) continue;
                bool external=false;
                for(int u:users[nodes[n].y]) if(!dead.count(u)) external=true;
                if(!external) {dead.insert(n);queue.insert(queue.end(),deps[n].begin(),deps[n].end());}
            }
            if(dead.size()<2) continue;
            std::vector<int> boundary(support[root].begin(),support[root].end());
            int rows=1<<boundary.size();
            uint64_t mask=rows==64?~uint64_t(0):((uint64_t(1)<<rows)-1);
            std::vector<uint64_t> tables(signals.size(),0);
            std::vector<bool> available(signals.size(),false);
            available[0]=available[1]=true;tables[1]=mask;
            for(int j=0;j<GetSize(boundary);j++) {
                int b=boundary[j];available[b]=true;
                for(int k=0;k<rows;k++) tables[b]|=uint64_t((k>>j)&1)<<k;
            }
            std::set<int> direct_nodes;
            if (boundary.size() <= 4) {
                std::vector<int> visit = {root};
                while (!visit.empty()) {
                    int n = visit.back(); visit.pop_back(); budget.charge();
                    if (support[root].count(nodes[n].y) || !direct_nodes.insert(n).second) continue;
                    visit.insert(visit.end(), deps[n].begin(), deps[n].end());
                }
            }
            std::vector<int> region;
            for(int n:topo) {
                if (boundary.size() <= 4 && !direct_nodes.count(n)) continue;
                budget.charge();
                if(support[root].count(nodes[n].y)) continue;
                bool all=true;for(int b:nodes[n].pins) if(!available[b]) all=false;
                if(!all) continue;
                const auto &pins=nodes[n].pins;
                budget.charge((1<<pins.size())*std::max(1,GetSize(pins)));
                uint64_t result=0;
                for(int k=0;k<(1<<pins.size());k++) if(nodes[n].table&(1<<k)) {
                    uint64_t term=mask;
                    for(int j=0;j<GetSize(pins);j++) term&=(k&(1<<j))?tables[pins[j]]:(tables[pins[j]]^mask);
                    result|=term;
                }
                tables[nodes[n].y]=result;available[nodes[n].y]=true;region.push_back(n);
            }
            if(!available[ry]) continue;
            uint64_t target=tables[ry];
            std::vector<int> chosen;
            if (boundary.size() <= 4) {
                // Boundary truth rows already prove an exact single-LUT4
                // implementation; no divisor/set-cover search is necessary.
                chosen = boundary;
            } else {
            std::set<int> descendants={root};queue={root};
            while(!queue.empty()) {
                int n=queue.back();queue.pop_back();
                for(int c:children[n]) {budget.charge();if(descendants.insert(c).second) queue.push_back(c);}
            }
            std::vector<int> divbits=boundary;
            for(int n:region) if(!dead.count(n) && !descendants.count(n)) divbits.push_back(nodes[n].y);
            std::set<uint64_t> unique;
            std::vector<int> filtered;
            for(int b:divbits) {budget.charge();if(unique.insert(tables[b]).second) filtered.push_back(b);}
            std::vector<std::pair<int,int>> pairs;
            for(int a=0;a<rows;a++) if(!(target>>a&1))
                for(int b=0;b<rows;b++) if(target>>b&1) pairs.emplace_back(a,b);
            budget.charge(GetSize(pairs));
            std::vector<Divisor> divisors;
            for(int b:filtered) {
                Divisor d;d.bit=b;
                budget.charge(4);
                // Count separated opposite-output pairs by their four Boolean
                // classes, avoiding a scan of every pair for every divisor.
                uint64_t t=tables[b],nt=t^mask,nf=target^mask;
                d.count=__builtin_popcountll(t&target)*__builtin_popcountll(nt&nf)
                       +__builtin_popcountll(nt&target)*__builtin_popcountll(t&nf);
                if(d.count) divisors.push_back(d);
            }
            std::sort(divisors.begin(),divisors.end(),[](const Divisor&a,const Divisor&b){return a.count!=b.count?a.count>b.count:a.bit<b.bit;});
            if(divisors.size()>40) divisors.resize(40);
            // Dual form: each opposite-output row pair requires choosing at
            // least one divisor that separates it. Branch on the tightest
            // requirement, instead of enumerating every 4-divisor tuple.
            std::vector<uint64_t> row_codes(rows,0);
            budget.charge(rows*GetSize(divisors));
            for(int r=0;r<rows;r++) for(int j=0;j<GetSize(divisors);j++)
                row_codes[r]|=((tables[divisors[j].bit]>>r)&1)<<j;
            std::set<uint64_t> unique_requirements;
            for(int p=0;p<GetSize(pairs);p++) {
                budget.charge();
                uint64_t requirement=row_codes[pairs[p].first]^row_codes[pairs[p].second];
                unique_requirements.insert(requirement);
            }
            std::vector<uint64_t> requirements(unique_requirements.begin(),unique_requirements.end());
            budget.charge(GetSize(requirements));
            if(infeasible.count(requirements)) continue;
            std::vector<int> trial;
            std::function<bool(const std::vector<uint64_t>&,uint64_t,int)> search=
                [&](const std::vector<uint64_t>&remaining,uint64_t available,int left) {
                if(remaining.empty()) {chosen=trial;return true;}
                if(!left) return false;
                uint64_t smallest=0,common=available;int mincount=65;
                for(uint64_t r:remaining) {
                    budget.charge();uint64_t choices=r&available;
                    if(!choices) return false;
                    common&=choices;
                    int count=__builtin_popcountll(choices);
                    if(count<mincount) {mincount=count;smallest=choices;}
                }
                if(common) {trial.push_back(divisors[__builtin_ctzll(common)].bit);chosen=trial;trial.pop_back();return true;}
                if(left==1) return false;
                while(smallest) {
                    int j=__builtin_ctzll(smallest);uint64_t bit=uint64_t(1)<<j;
                    smallest&=~bit;available&=~bit;
                    std::vector<uint64_t> next;
                    for(uint64_t r:remaining) {budget.charge();if(!(r&bit)) next.push_back(r);}
                    trial.push_back(divisors[j].bit);
                    if(search(next,available,left-1)) return true;
                    trial.pop_back();
                }
                return false;
            };
            bool found=pairs.empty();
            // Every solution costs exactly one physical LUT4, independent of
            // whether it uses 1, 2, 3 or 4 inputs. Search the feasible bound once.
            if(!found) found=search(requirements,(uint64_t(1)<<divisors.size())-1,4);
            if(!found) {
                if(infeasible.size()<4096) infeasible.insert(requirements);
                continue;
            }
            }
            uint16_t table=0;std::map<int,int> seen;
            for(int k=0;k<rows;k++) {
                budget.charge(GetSize(chosen)+1);
                int addr=0;for(int j=0;j<GetSize(chosen);j++) addr|=((tables[chosen[j]]>>k)&1)<<j;
                int value=(target>>k)&1;
                if(seen.count(addr) && seen.at(addr)!=value) log_error("LUT resub conflicting truth rows.\n");
                seen[addr]=value;table|=value<<addr;
            }
            if(chosen.empty()) {chosen={0};table=target?3:0;}
            for(int n:dead) for(int b:chosen)
                if(nodes[n].y==b) log_error("LUT resub attempted to delete a candidate input.\n");
            for(int k=0;k<rows;k++) {
                budget.charge(GetSize(chosen)+1);
                int addr=0;for(int j=0;j<GetSize(chosen);j++) addr|=((tables[chosen[j]]>>k)&1)<<j;
                if(((table>>addr)&1)!=((target>>k)&1)) log_error("LUT resub candidate truth mismatch.\n");
            }
            budget.charge(GetSize(dead)+GetSize(chosen)+1);
            IdString name=nodes[root].cell->name;
            auto attributes=nodes[root].cell->attributes;
            SigSpec pins;for(int b:chosen) pins.append(signals[b]);
            SigBit output=signals[ry];
            for(int n:dead) module->remove(nodes[n].cell);
            module->addLut(name,pins,output,Const(int(table),1<<chosen.size()))->attributes=attributes;
            edits++;removed+=GetSize(dead)-1;
            return true;
        }
        return false;
    }
};

// LUT islands that share a nonconstant signal are optimized together. Their
// scratch modules contain only those LUTs and explicit boundary ports, so a
// change in one ROM does not rebuild connectivity for every other ROM.
static void optimize_islands(Module *m,CellTypes &types,ResubBudget &budget,int &total_edits,int &total_saved)
{
    int64_t extent=0;
    for(auto w:m->wires()) extent+=int64_t(w->width)+1;
    for(auto &c:m->connections()) extent+=int64_t(c.first.size())+c.second.size()+1;
    for(auto c:m->cells()) {extent++;for(auto &p:c->connections()) extent+=int64_t(p.second.size())+1;}
    budget.charge(extent);
    if(extent>2000000) return;
    SigMap sigmap(m);
    std::vector<Cell*> luts;
    std::set<SigBit> observed,external_driver,protected_bits;
    for(auto w:m->wires()) for(auto bit:SigSpec(w)) {
        bit=sigmap(bit);
        if(w->port_output) observed.insert(bit);
        if(w->port_input) external_driver.insert(bit);
        if(w->get_bool_attribute(ID::keep)||w->attributes.count(ID::init)) protected_bits.insert(bit);
    }
    for(auto c:m->cells()) {
        if(c->type==ID($lut)) luts.push_back(c);
        else for(auto &p:c->connections()) for(auto bit:p.second) {
            bit=sigmap(bit);observed.insert(bit);
            if(!types.cell_known(c->type)||types.cell_output(c->type,p.first)) external_driver.insert(bit);
        }
    }
    // Removing >=2 LUTs requires a LUT-to-LUT edge. Shared primary inputs
    // alone cannot supply such a cone. Canonicalize aliases before testing;
    // this is only a necessary condition, never a functional simplification.
    if (luts.size() < 2) return;
    pool<SigBit> lut_outputs;
    for (auto c : luts) if (c->hasPort(ID::Y))
        for (auto bit : sigmap(c->getPort(ID::Y)))
            if (bit.wire) lut_outputs.insert(bit);
    bool cascade = false;
    for (auto c : luts) if (c->hasPort(ID::A)) {
        for (auto bit : sigmap(c->getPort(ID::A))) {
            budget.charge();
            if (bit.wire && lut_outputs.count(bit)) { cascade = true; break; }
        }
        if (cascade) break;
    }
    if (!cascade) return;
    std::sort(luts.begin(),luts.end(),[](Cell*a,Cell*b){return a->name.str()<b->name.str();});
    std::vector<int> parent(luts.size());for(int i=0;i<GetSize(parent);i++)parent[i]=i;
    std::function<int(int)> find=[&](int i){return parent[i]==i?i:parent[i]=find(parent[i]);};
    dict<SigBit,int> first;
    for(int i=0;i<GetSize(luts);i++) for(auto &p:luts[i]->connections()) for(auto bit:p.second) {
        budget.charge();bit=sigmap(bit);if(!bit.wire)continue;
        auto it=first.find(bit);
        if(it==first.end()) first[bit]=i;
        else {int a=find(i),b=find(it->second);if(a!=b)parent[std::max(a,b)]=std::min(a,b);}
    }
    std::map<int,std::vector<Cell*>> groups;
    for(int i=0;i<GetSize(luts);i++)groups[find(i)].push_back(luts[i]);
    int edits=0;
    for(auto &entry:groups) {
        auto &group=entry.second;
        // A local optimizer must not repeatedly index an entire arithmetic
        // datapath. Large connected islands keep ABC's verified mapping.
        if(group.size()<2 || group.size()>512 || edits>=64)continue;
        std::set<SigBit> used,driven;
        for(auto c:group) for(auto &p:c->connections()) for(auto bit:p.second) {
            budget.charge();bit=sigmap(bit);if(!bit.wire)continue;
            used.insert(bit);if(p.first==ID::Y)driven.insert(bit);
        }
        std::vector<SigBit> ordered(used.begin(),used.end());
        std::sort(ordered.begin(),ordered.end(),[](SigBit a,SigBit b){
            return a.wire->name!=b.wire->name?a.wire->name.str()<b.wire->name.str():a.offset<b.offset;
        });
        std::unique_ptr<Module> scratch(new Module);
        scratch->name=ID($resub_island);
        dict<SigBit,SigBit> forward,reverse;
        for(int i=0;i<GetSize(ordered);i++) {
            SigBit bit=ordered[i];auto w=scratch->addWire(stringf("\\b%08d",i));
            w->port_input=!driven.count(bit)||external_driver.count(bit);
            w->port_output=observed.count(bit);
            if(protected_bits.count(bit))w->set_bool_attribute(ID::keep);
            forward[bit]=SigBit(w);reverse[SigBit(w)]=bit;
        }
        for(auto c:group) {
            auto clone=scratch->addCell(c->name,c->type);
            clone->parameters=c->parameters;clone->attributes=c->attributes;
            for(auto &p:c->connections()) {
                SigSpec pins;for(auto bit:p.second) {bit=sigmap(bit);pins.append(bit.wire?forward.at(bit):bit);}
                clone->setPort(p.first,pins);
            }
        }
        LutResubWorker worker(scratch.get(),types,budget);
        // Reserve commit work before searching. step() mutates its scratch
        // graph only after all truth checks and charged work have completed.
        // A failed later search therefore leaves earlier exact edits valid.
        budget.charge(GetSize(group));
        int output_count = 0;
        for (auto bit : driven) if (observed.count(bit)) ++output_count;
        // More independently observed functions offer more useful rewrites.
        // Retain a small floor, then scale effort with observable outputs.
        int max_edits = budget.max_island_edits ? budget.max_island_edits :
            std::min(64, std::max(4, (output_count + 2) / 3));
        int64_t allowance = budget.island_override ? budget.island_override :
            std::min<int64_t>(1000000, std::max<int64_t>(
                65536 + int64_t(group.size()) * 8192, int64_t(output_count) * 32768));
        budget.island_stop = std::min(budget.limit, budget.used + allowance);
        bool global_stop = false;
        try {
            for (int i=0; i<max_edits && edits+worker.edits<64; ++i)
                if (!worker.step()) break;
        } catch (const ResubBudget::Exhausted &e) {
            global_stop = e.global;
            log("  LUT_RESUB island budget exit: retained_edits=%d saved=%d global=%d\n",
                worker.edits, worker.removed, int(global_stop));
        }
        budget.island_stop = budget.limit;
        if(!worker.edits) {
            if(global_stop) throw ResubBudget::Exhausted{true};
            continue;
        }
        for(auto original:group) {
            auto updated=scratch->cell(original->name);
            if(!updated) {m->remove(original);continue;}
            original->parameters=updated->parameters;original->attributes=updated->attributes;
            for(auto &p:updated->connections()) {
                SigSpec pins;for(auto bit:p.second)pins.append(bit.wire?reverse.at(bit):bit);
                original->setPort(p.first,pins);
            }
        }
        edits+=worker.edits;total_edits+=worker.edits;total_saved+=worker.removed;
        if(global_stop) throw ResubBudget::Exhausted{true};
    }
}

struct LutResubPass : Pass {
    LutResubPass() : Pass("lut_resub_bounded","bounded post-ABC LUT4 resubstitution") {}
    void help() override {
        log("    lut_resub_bounded [-work-limit N] [-island-work-limit N] [-max-island-edits N] [selection]\n");
        log("Known Boolean LUT4 functions; <=6 boundary inputs, <=40 divisors,\n");
        log("Output-scaled island edits (>=4, <=64); <=64/module; 80M design work.\n");
        log("Completed exact edits survive later search budget exhaustion.\n");
    }
    void execute(std::vector<std::string> args,Design*design) override {
        ResubBudget budget;
        size_t argidx = 1;
        while (argidx < args.size()) {
            auto option = args[argidx];
            if (option != "-work-limit" && option != "-island-work-limit" && option != "-max-island-edits") break;
            if (++argidx >= args.size()) log_cmd_error("Missing budget value.\n");
            auto value = args[argidx++];
            if (value.empty() || value.find_first_not_of("0123456789") != std::string::npos || value.size() > 8)
                log_cmd_error("Budget must be a positive integer.\n");
            int64_t n = std::stoll(value);
            if (n < 1 || n > 80000000 || (option == "-max-island-edits" && n > 64))
                log_cmd_error("Budget value out of range.\n");
            if (option == "-work-limit") budget.limit = n;
            else if (option == "-island-work-limit") budget.island_override = n;
            else budget.max_island_edits = int(n);
        }
        budget.island_stop = budget.limit;
        extra_args(args,argidx,design);
        log_header(design,"Executing LUT_RESUB_BOUNDED pass.\n");
        CellTypes types(design);
        int edits=0,removed=0;bool exhausted=false;
        for(auto module:design->selected_modules()) {
            if(!design->selected_whole_module(module) || module->get_bool_attribute(ID::keep) ||
               module->get_blackbox_attribute() || module->has_processes()) continue;
            try {optimize_islands(module,types,budget,edits,removed);}
            catch(ResubBudget::Exhausted&) {exhausted=true;}
            if(exhausted) break;
        }
        design->scratchpad_set_bool("lut_resub_bounded.changed", edits != 0);
        log("  LUT_RESUB: edits=%d saved=%d work=%lld exhausted=%d\n",edits,removed,(long long)budget.used,exhausted);
    }
} LutResubPass;
PRIVATE_NAMESPACE_END
