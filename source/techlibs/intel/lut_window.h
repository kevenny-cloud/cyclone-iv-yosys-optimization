// Generic, bounded cut rewriting after LUT4 mapping. No testcase recognition.
// Register/memory/blackbox pins are boundaries; each edit is Boolean-exact,
// removes more physical LUTs than it adds, and preserves a depth upper bound.
struct WindowBudget {
    int64_t used=0, limit=3000000;
    struct Stop {};
    void charge(int64_t n=1) { if(n>limit-used) throw Stop{}; used+=n; }
};

struct LutWindowWorker {
    using Truth = std::bitset<128>;
    struct Node {
        Cell *cell; std::vector<int> pins; int y,depth=1;
        uint16_t table; bool active=true,good=true;
    };
    struct Plan {
        std::vector<int> leaves,dead,bound,free,shared;
        uint16_t first=0,second=0;
        int cost=0,depth=0,gain=0;
    };
    Module *m; CellTypes &types; WindowBudget &budget; SigMap sm;
    dict<SigBit,int> ids;
    std::vector<SigBit> signals;
    std::vector<Node> nodes;
    std::vector<int> producer,driver_count;
    std::vector<std::vector<int>> users;
    std::vector<bool> external,protected_bits,primary;
    std::vector<int> required;std::map<int,int> endpoint_limit;
    int roots=0,cuts_examined=0,edits=0,saved=0,decomposed=0;

    LutWindowWorker(Module *module,CellTypes &ct,WindowBudget &b):m(module),types(ct),budget(b),sm(module) {
        id(SigBit(State::S0));id(SigBit(State::S1));
    }
    int id(SigBit bit) {
        bit=sm(bit);auto it=ids.find(bit);if(it!=ids.end())return it->second;
        int i=GetSize(signals);ids[bit]=i;signals.push_back(bit);
        producer.push_back(-1);driver_count.push_back(0);users.emplace_back();
        external.push_back(false);protected_bits.push_back(false);primary.push_back(false);return i;
    }
    int depth(int bit)const {int n=producer[bit];return n>=0&&nodes[n].active&&nodes[n].good?nodes[n].depth:0;}
    bool expandable(int bit)const {
        int n=producer[bit];return n>=0&&nodes[n].active&&nodes[n].good&&driver_count[bit]==1;
    }
    void build() {
        for(auto w:m->wires())for(auto bit:SigSpec(w)) {
            budget.charge();int b=id(bit);
            if(w->port_input){primary[b]=true;driver_count[b]++;}
            if(w->port_output)external[b]=true;
            if(w->get_bool_attribute(ID::keep)||w->attributes.count(ID::init))protected_bits[b]=true;
        }
        std::vector<Cell*> ordered;
        for(auto c:m->cells())ordered.push_back(c);
        std::sort(ordered.begin(),ordered.end(),[](Cell*a,Cell*b){return a->name.str()<b->name.str();});
        for(auto c:ordered) {
            budget.charge();
            bool lut=c->type==ID($lut)&&!c->get_bool_attribute(ID::keep)&&
                c->hasPort(ID::A)&&c->hasPort(ID::Y)&&GetSize(c->getPort(ID::Y))==1&&
                GetSize(c->getPort(ID::A))>=1&&GetSize(c->getPort(ID::A))<=4&&
                c->hasParam(ID::WIDTH)&&c->getParam(ID::WIDTH).is_fully_def()&&
                c->getParam(ID::WIDTH).as_int()==GetSize(c->getPort(ID::A))&&
                c->hasParam(ID::LUT)&&c->getParam(ID::LUT).is_fully_def()&&
                GetSize(c->getParam(ID::LUT))==(1<<GetSize(c->getPort(ID::A)));
            if(lut) {
                Node n{c,{},id(c->getPort(ID::Y)[0]),1,uint16_t(c->getParam(ID::LUT).as_int()),true,true};
                for(auto bit:c->getPort(ID::A)){budget.charge();int b=id(bit);n.pins.push_back(b);if(b>1&&!signals[b].wire)n.good=false;}
                n.good=n.good&&signals[n.y].wire&&!protected_bits[n.y]&&!primary[n.y];
                int index=GetSize(nodes);nodes.push_back(n);driver_count[n.y]++;
                if(producer[n.y]==-1)producer[n.y]=index;
                for(int b:n.pins)users[b].push_back(index);
            }else{
                bool known=types.cell_known(c->type);
                for(auto &p:c->connections())for(auto bit:p.second) {
                    budget.charge();int b=id(bit);
                    if(!known||types.cell_output(c->type,p.first))driver_count[b]++;
                    // Preserve all non-LUT connectivity, including unknown ports.
                    if(!known||types.cell_input(c->type,p.first))external[b]=true;
                }
            }
        }
        for(auto &n:nodes)if(driver_count[n.y]!=1)n.good=false;
    }
    std::vector<int> topological() {
        std::vector<std::vector<int>> children(nodes.size());
        std::vector<int> pending(nodes.size(),0),order;
        for(int i=0;i<GetSize(nodes);i++)if(nodes[i].good) {
            std::set<int> deps;
            for(int b:nodes[i].pins)if(expandable(b))deps.insert(producer[b]);
            pending[i]=GetSize(deps);for(int d:deps)children[d].push_back(i);
            if(deps.empty())order.push_back(i);
        }
        for(int k=0;k<GetSize(order);k++) {
            budget.charge();int n=order[k];
            for(int b:nodes[n].pins)nodes[n].depth=std::max(nodes[n].depth,depth(b)+1);
            for(int c:children[n])if(!--pending[c])order.push_back(c);
        }
        // Cyclic or ambiguous portions remain untouched; their outputs are cuts.
        for(int i=0;i<GetSize(nodes);i++)if(pending[i])nodes[i].good=false;
        return order;
    }
    bool cone(int root,const std::vector<int>&leaves,std::vector<int>&region) {
        std::set<int> boundary(leaves.begin(),leaves.end()),seen,visiting;
        std::function<bool(int)> visit=[&](int n){
            budget.charge();if(seen.count(n))return true;
            if(visiting.count(n)||seen.size()+visiting.size()>=48)return false;
            if(!nodes[n].good||!nodes[n].active)return false;
            visiting.insert(n);
            for(int b:nodes[n].pins) {
                if(b<2||boundary.count(b))continue;
                if(!expandable(b)||!visit(producer[b]))return false;
            }
            visiting.erase(n);seen.insert(n);region.push_back(n);return true;
        };
        return visit(root);
    }
    std::vector<int> removable(int root,const std::vector<int>&region) {
        std::set<int> dead={root};
        for(auto it=region.rbegin();it!=region.rend();++it) {
            budget.charge();int n=*it;if(n==root)continue;
            int y=nodes[n].y;
            if(external[y]||protected_bits[y]||primary[y])continue;
            bool outside=false;
            for(int user:users[y])if(nodes[user].active&&!dead.count(user)){outside=true;break;}
            if(!outside)dead.insert(n);
        }
        return std::vector<int>(dead.begin(),dead.end());
    }
    Truth truth(const std::vector<int>&leaves,const std::vector<int>&region,int root) {
        int rows=1<<GetSize(leaves);log_assert(rows<=128);Truth mask;
        for(int r=0;r<rows;r++)mask.set(r);
        std::map<int,Truth> tables;tables[0]=Truth();tables[1]=mask;
        for(int j=0;j<GetSize(leaves);j++) {
            Truth table;for(int r=0;r<rows;r++)table.set(r,(r>>j)&1);
            tables[leaves[j]]=table;
        }
        for(int n:region) {
            auto &node=nodes[n];Truth table;
            budget.charge((1<<GetSize(node.pins))*GetSize(node.pins)*2);
            for(int k=0;k<(1<<GetSize(node.pins));k++)if(node.table&(1<<k)) {
                Truth term=mask;
                for(int j=0;j<GetSize(node.pins);j++) {
                    auto pos=tables.find(node.pins[j]);log_assert(pos!=tables.end());
                    term&=(k&(1<<j))?pos->second:(pos->second^mask);
                }
                table|=term;
            }
            tables[node.y]=table;
        }
        return tables.at(nodes[root].y);
    }
    static int expand_address(int address,const std::vector<int>&positions) {
        int result=0;for(int i=0;i<GetSize(positions);i++)result|=((address>>i)&1)<<positions[i];return result;
    }
    void minimize(std::vector<int>&leaves,Truth &table) {
        int n=GetSize(leaves),rows=1<<n;std::vector<int> positions,kept;
        for(int j=0;j<n;j++) {
            bool depends=false;
            for(int r=0;r<rows;r++)if(!(r&(1<<j))) {
                budget.charge();if(table[r]!=table[r|(1<<j)]){depends=true;break;}
            }
            if(depends){positions.push_back(j);kept.push_back(leaves[j]);}
        }
        Truth compact;
        for(int r=0;r<(1<<GetSize(positions));r++)compact.set(r,table[expand_address(r,positions)]);
        leaves=kept;table=compact;
    }
    bool synthesize(std::vector<int> leaves,Truth table,int old_depth,int dead_count,Plan &plan) {
        minimize(leaves,table);int n=GetSize(leaves);
        if(n<=4) {
            int d=1;for(int b:leaves)d=std::max(d,depth(b)+1);
            if(d>old_depth)return false;
            plan.leaves=leaves;plan.cost=1;plan.depth=d;plan.first=uint16_t(table.to_ullong());return true;
        }
        if(dead_count<3)return false;
        // Ashenhurst-style two-LUT decomposition: a Boolean bound function
        // exists exactly when the cofactor columns have at most two classes.
        for(int subset=1;subset<(1<<n);subset++) {
            int size=__builtin_popcount(unsigned(subset));
            if(size<2||size>4||n-size+1>4)continue;
            budget.charge();std::vector<int> bound,free;
            for(int j=0;j<n;j++)((subset&(1<<j))?bound:free).push_back(j);
            int low_depth=1;for(int j:bound)low_depth=std::max(low_depth,depth(leaves[j])+1);
            int high_depth=low_depth+1;for(int j:free)high_depth=std::max(high_depth,depth(leaves[j])+1);
            if(high_depth>old_depth)continue;
            int column0=-1,column1=-1;uint16_t first=0,second=0;bool good=true;
            for(int b=0;b<(1<<size)&&good;b++) {
                int column=0;
                for(int f=0;f<(1<<GetSize(free));f++) {
                    budget.charge();int addr=expand_address(b,bound)|expand_address(f,free);
                    column|=int(table[addr])<<f;
                }
                if(column0<0)column0=column;
                if(column!=column0){if(column1<0)column1=column;if(column!=column1)good=false;else first|=1<<b;}
            }
            if(!good)continue;
            if(column1<0)column1=column0;
            second=uint16_t(column0|(column1<<(1<<GetSize(free))));
            for(int r=0;r<(1<<n);r++) {
                budget.charge();int b=0,f=0;
                for(int j=0;j<GetSize(bound);j++)b|=((r>>bound[j])&1)<<j;
                for(int j=0;j<GetSize(free);j++)f|=((r>>free[j])&1)<<j;
                int address=f|(((first>>b)&1)<<GetSize(free));
                if(((second>>address)&1)!=int(table[r]))log_error("LUT window decomposition truth mismatch.\n");
            }
            plan.leaves=leaves;plan.bound=bound;plan.free=free;plan.first=first;plan.second=second;
            plan.cost=2;plan.depth=high_depth;return true;
        }
        return synthesize_shared(leaves,table,old_depth,dead_count,plan);
    }
    void rewrite(int root,const Plan &plan) {
        budget.charge(GetSize(plan.dead)*5+GetSize(plan.leaves)*4+10);
        for(int n:plan.dead)for(int b:nodes[n].pins)users[b].erase(std::remove(users[b].begin(),users[b].end(),n),users[b].end());
        for(int n:plan.dead)if(n!=root) {
            producer[nodes[n].y]=-1;driver_count[nodes[n].y]=0;nodes[n].active=false;m->remove(nodes[n].cell);
        }
        std::vector<int> pins;uint16_t table=plan.first;
        if(plan.cost==1)pins=plan.leaves;
        else {
            SigSpec low_pins;std::vector<int> low;
            for(int j:plan.bound){low.push_back(plan.leaves[j]);low_pins.append(signals[plan.leaves[j]]);}
            auto wire=m->addWire(NEW_ID);int out=id(SigBit(wire));
            auto cell=m->addLut(NEW_ID,low_pins,SigSpec(wire),Const(int(plan.first),1<<GetSize(low)));
            int index=GetSize(nodes);Node node{cell,low,out,1,plan.first,true,true};
            for(int b:low){node.depth=std::max(node.depth,depth(b)+1);users[b].push_back(index);}
            nodes.push_back(node);required.push_back(1000000000);producer[out]=index;driver_count[out]=1;
            for(int j:plan.free) pins.push_back(plan.leaves[j]);
            for(int j:plan.shared) pins.push_back(plan.leaves[j]);
            pins.push_back(out);table=plan.second;decomposed++;
        }
        if(pins.empty()){pins.push_back(0);table=table?3:0;}
        SigSpec inputs;for(int b:pins){inputs.append(signals[b]);users[b].push_back(root);}
        nodes[root].cell->setPort(ID::A,inputs);
        nodes[root].cell->setParam(ID::WIDTH,GetSize(pins));
        nodes[root].cell->setParam(ID::LUT,Const(int(table),1<<GetSize(pins)));
        nodes[root].pins=pins;nodes[root].table=table;nodes[root].depth=plan.depth;
        edits++;saved+=plan.gain;propagate_required(root);
    }
    // Merge equal/complement LUT functions only when the discarded output has
    // no external consumer. Complement phase is folded into every LUT user.
    void merge_phases(const std::vector<int>&order) {
        using Key=std::pair<std::vector<int>,uint16_t>;
        std::map<Key,std::pair<int,bool>> index;
        auto phase_order=order;
        std::stable_sort(phase_order.begin(),phase_order.end(),[&](int a,int b){return external[nodes[a].y]>external[nodes[b].y];});
        for(int root:phase_order) {
            budget.charge();auto &n=nodes[root];if(!n.active||!n.good)continue;
            auto pins=n.pins;pins.erase(std::remove_if(pins.begin(),pins.end(),[](int b){return b<2;}),pins.end());
            std::sort(pins.begin(),pins.end());pins.erase(std::unique(pins.begin(),pins.end()),pins.end());
            if(pins.empty())continue;
            uint16_t table=0;
            for(int row=0;row<(1<<GetSize(pins));++row) {
                budget.charge(1+GetSize(n.pins));int a=0;
                for(int k=0;k<GetSize(n.pins);++k) {
                    int b=n.pins[k];int value=b<2?b:(row>>int(std::find(pins.begin(),pins.end(),b)-pins.begin()))&1;
                    a|=value<<k;
                }
                table|=((n.table>>a)&1)<<row;
            }
            bool phase=table&1;uint16_t mask=uint16_t((uint32_t(1)<<(1<<GetSize(pins)))-1);
            Key key={pins,uint16_t(phase?table^mask:table)};auto found=index.find(key);
            if(found==index.end()){index[key]={root,phase};continue;}
            auto &keep=nodes[found->second.first];
            if(!keep.active||!keep.good){index[key]={root,phase};continue;}
            // Earlier substitutions may have changed a previously indexed
            // consumer's pins. Recompute before trusting any cached entry.
            auto kp=keep.pins;kp.erase(std::remove_if(kp.begin(),kp.end(),[](int b){return b<2;}),kp.end());
            std::sort(kp.begin(),kp.end());kp.erase(std::unique(kp.begin(),kp.end()),kp.end());
            uint16_t kt=0;
            for(int row=0;row<(1<<GetSize(kp));++row) {
                budget.charge(1+GetSize(keep.pins));int a=0;
                for(int k=0;k<GetSize(keep.pins);++k) {
                    int b=keep.pins[k];int v=b<2?b:(row>>int(std::find(kp.begin(),kp.end(),b)-kp.begin()))&1;a|=v<<k;
                }
                kt|=((keep.table>>a)&1)<<row;
            }
            bool keep_phase=kt&1;uint16_t km=uint16_t((uint32_t(1)<<(1<<GetSize(kp)))-1);
            if(Key(kp,uint16_t(keep_phase?kt^km:kt))!=key){index[key]={root,phase};continue;}
            if(external[n.y]||protected_bits[n.y]||primary[n.y])continue;
            bool invert=phase!=keep_phase;
            struct Update{int node;std::vector<int>pins;uint16_t table;};
            std::vector<Update>updates;std::set<int>fanout;
            for(int u:users[n.y])if(nodes[u].active)fanout.insert(u);
            bool valid=true;
            for(int u:fanout) {
                budget.charge();auto &c=nodes[u];if(u==root||u==found->second.first||!c.good){valid=false;break;}
                std::vector<int> next;
                for(int b:c.pins)if(b>1)next.push_back(b==n.y?keep.y:b);
                std::sort(next.begin(),next.end());next.erase(std::unique(next.begin(),next.end()),next.end());
                log_assert(!next.empty()&&next.size()<=4);uint16_t truth=0;
                for(int row=0;row<(1<<GetSize(next));++row) {
                    budget.charge(1+GetSize(c.pins));int a=0;
                    for(int k=0;k<GetSize(c.pins);++k) {
                        int b=c.pins[k];bool neg=b==n.y;if(neg)b=keep.y;
                        int value=b<2?b:(row>>int(std::find(next.begin(),next.end(),b)-next.begin()))&1;
                        if(neg&&invert)value^=1;a|=value<<k;
                    }
                    truth|=((c.table>>a)&1)<<row;
                }
                updates.push_back({u,next,truth});
            }
            if(!valid)continue;
            int64_t mutation=GetSize(n.pins)+1;
            for(auto &u:updates){mutation+=GetSize(u.pins)+1;for(int b:nodes[u.node].pins)mutation+=GetSize(users[b])+1;}
            for(int b:n.pins)mutation+=GetSize(users[b])+1;
            budget.charge(mutation);
            for(auto &u:updates) {
                auto &c=nodes[u.node];
                for(int b:c.pins){auto &v=users[b];v.erase(std::remove(v.begin(),v.end(),u.node),v.end());}
                SigSpec inputs;for(int b:u.pins){inputs.append(signals[b]);users[b].push_back(u.node);}
                c.cell->setPort(ID::A,inputs);c.cell->setParam(ID::WIDTH,GetSize(u.pins));
                c.cell->setParam(ID::LUT,Const(int(u.table),1<<GetSize(u.pins)));c.pins=u.pins;c.table=u.table;
            }
            for(int b:n.pins){auto &v=users[b];v.erase(std::remove(v.begin(),v.end(),root),v.end());}
            n.active=false;producer[n.y]=-1;driver_count[n.y]=0;m->remove(n.cell);n.cell=nullptr;++edits;++saved;
        }
    }
    // Inline one shared LUT into ALL its consumers as a single transaction.
    // Each consumer must still fit a LUT4; shared node must have no other use.
    // Per-root MFFC accounting cannot see this strictly positive group gain.
    void inline_shared(const std::vector<int>&order) {
        for(int root:order) {
            budget.charge();auto &n=nodes[root];
            if(!n.active||!n.good||external[n.y]||protected_bits[n.y]||primary[n.y])continue;
            std::set<int> fanout;
            for(int u:users[n.y])if(nodes[u].active)fanout.insert(u);
            if(fanout.size()<2)continue;
            struct Update {int node;std::vector<int> pins;uint16_t table;};
            std::vector<Update> updates;bool valid=true;
            for(int u:fanout) {
                budget.charge();auto &c=nodes[u];
                if(u==root||!c.good){valid=false;break;}
                std::vector<int> pins;
                for(int b:c.pins)if(b!=n.y&&b>1)pins.push_back(b);
                for(int b:n.pins)if(b>1)pins.push_back(b);
                std::sort(pins.begin(),pins.end());pins.erase(std::unique(pins.begin(),pins.end()),pins.end());
                if(pins.empty()||pins.size()>4||std::find(pins.begin(),pins.end(),n.y)!=pins.end()){valid=false;break;}
                uint16_t table=0;
                auto value=[&](int b,int row){if(b<2)return b;auto it=std::find(pins.begin(),pins.end(),b);log_assert(it!=pins.end());return (row>>int(it-pins.begin()))&1;};
                for(int row=0;row<(1<<GetSize(pins));++row) {
                    budget.charge(1+GetSize(n.pins)+GetSize(c.pins));
                    int a=0;for(int k=0;k<GetSize(n.pins);++k)a|=value(n.pins[k],row)<<k;
                    int intermediate=(n.table>>a)&1;
                    int b=0;for(int k=0;k<GetSize(c.pins);++k)b|=(c.pins[k]==n.y?intermediate:value(c.pins[k],row))<<k;
                    table|=((c.table>>b)&1)<<row;
                }
                updates.push_back({u,pins,table});
            }
            if(!valid)continue;
            // Pay all mutation work before changing any cell, including wide fanout.
            int64_t mutation=GetSize(n.pins)+1;
            for(auto &u:updates) {
                mutation+=GetSize(u.pins)+1;
                for(int b:nodes[u.node].pins)mutation+=GetSize(users[b])+1;
            }
            for(int b:n.pins)mutation+=GetSize(users[b])+1;
            budget.charge(mutation);
            for(auto &u:updates) {
                auto &c=nodes[u.node];
                for(int b:c.pins){auto &v=users[b];v.erase(std::remove(v.begin(),v.end(),u.node),v.end());}
                SigSpec pins;for(int b:u.pins){pins.append(signals[b]);users[b].push_back(u.node);}
                c.cell->setPort(ID::A,pins);c.cell->setParam(ID::WIDTH,GetSize(u.pins));
                c.cell->setParam(ID::LUT,Const(int(u.table),1<<GetSize(u.pins)));
                c.pins=u.pins;c.table=u.table;
                // Existing depth remains a valid upper bound after bypassing n.
            }
            for(int b:n.pins){auto &v=users[b];v.erase(std::remove(v.begin(),v.end(),root),v.end());}
            n.active=false;producer[n.y]=-1;driver_count[n.y]=0;m->remove(n.cell);n.cell=nullptr;
            ++edits;++saved;
        }
    }
    #include "lut_shared.h"
    void initialize_required(const std::vector<int>&order) {
        required.assign(nodes.size(),1000000000);int maximum=0;
        for(int n:order)if(nodes[n].active&&nodes[n].good&&external[nodes[n].y])maximum=std::max(maximum,nodes[n].depth);
        for(int n:order)if(nodes[n].active&&nodes[n].good&&external[nodes[n].y]) {
            required[n]=maximum;endpoint_limit[n]=maximum;
        }
        for(auto it=order.rbegin();it!=order.rend();++it) {
            int n=*it;if(!nodes[n].active||!nodes[n].good||required[n]==1000000000)continue;
            for(int b:nodes[n].pins)if(expandable(b))required[producer[b]]=std::min(required[producer[b]],required[n]-1);
        }
        for(int n:order)if(required[n]==1000000000)required[n]=nodes[n].depth;
    }
    void propagate_required(int root) {
        std::vector<int> pending={root};
        for(int k=0;k<GetSize(pending);++k) {
            int n=pending[k];
            for(int b:nodes[n].pins)if(expandable(b)) {
                budget.charge();int p=producer[b];int r=required[n]-1;
                if(r<required[p]){required[p]=r;pending.push_back(p);}
            }
        }
    }
    void verify_endpoint_depth() {
        if(endpoint_limit.empty())return;
        std::vector<int> counts(nodes.size(),0),levels(nodes.size(),1),order;
        std::vector<std::vector<int>> children(nodes.size());int expected=0;
        for(int n=0;n<GetSize(nodes);++n)if(nodes[n].active&&nodes[n].good) {
            expected++;std::set<int> deps;
            for(int b:nodes[n].pins)if(expandable(b))deps.insert(producer[b]);
            counts[n]=GetSize(deps);for(int d:deps)children[d].push_back(n);
            if(deps.empty())order.push_back(n);
        }
        for(int k=0;k<GetSize(order);++k) {
            int n=order[k];for(int c:children[n]) {levels[c]=std::max(levels[c],levels[n]+1);if(!--counts[c])order.push_back(c);}
        }
        if(GetSize(order)!=expected)log_error("Native LUT rewrite introduced a dependency cycle.\n");
        for(auto &p:endpoint_limit)if(!nodes[p.first].active||levels[p.first]>p.second)
            log_error("Native LUT rewrite exceeded endpoint depth bound.\n");
    }
    void run() {
        build();auto order=topological();merge_phases(order);inline_shared(order);order=topological();initialize_required(order);
        for(auto it=order.rbegin();it!=order.rend();++it) {
            int root=*it;if(!nodes[root].active||!nodes[root].good)continue;
            budget.charge();roots++;
            std::vector<int> initial;
            for(int b:nodes[root].pins)if(b>1)initial.push_back(b);
            std::sort(initial.begin(),initial.end());initial.erase(std::unique(initial.begin(),initial.end()),initial.end());
            std::vector<std::vector<int>> cuts={initial};std::set<std::vector<int>> seen={initial};Plan best;
            for(int k=0;k<GetSize(cuts)&&k<64;k++) {
                auto leaves=cuts[k];budget.charge(GetSize(leaves)+1);cuts_examined++;
                std::vector<int> region;
                if(cone(root,leaves,region)) {
                    auto dead=removable(root,region);
                    if(GetSize(dead)>=2) {
                        Truth table=truth(leaves,region,root);Plan candidate;
                        if(synthesize(leaves,table,required[root],GetSize(dead),candidate)) {
                            candidate.dead=dead;candidate.gain=GetSize(dead)-candidate.cost;
                            if(candidate.gain>best.gain||(candidate.gain>0&&candidate.gain==best.gain&&candidate.depth<best.depth))best=candidate;
                        }
                    }
                }
                if(GetSize(cuts)>=64)continue;
                for(int leaf:leaves)if(expandable(leaf)&&producer[leaf]!=root) {
                    std::vector<int> expanded;
                    for(int b:leaves)if(b!=leaf)expanded.push_back(b);
                    for(int b:nodes[producer[leaf]].pins)if(b>1)expanded.push_back(b);
                    std::sort(expanded.begin(),expanded.end());expanded.erase(std::unique(expanded.begin(),expanded.end()),expanded.end());
                    budget.charge(GetSize(expanded)+1);
                    if(GetSize(expanded)<=7&&seen.insert(expanded).second)cuts.push_back(expanded);
                    if(GetSize(cuts)>=64)break;
                }
            }
            if(best.gain>0)rewrite(root,best);
        }
    }
};

struct LutWindowPass:Pass {
    LutWindowPass():Pass("lut_window","bounded exact local LUT4 cut rewriting"){}
    void help()override {
        log("    lut_window [-work-limit N] [selection]\n");
        log("Known Boolean LUT4s; <=7 cut leaves, <=48 internal LUTs, <=64 cuts/root.\n");
        log("Exact one/two-LUT replacements; strict savings and unchanged global LUT level bound.\n");
        log("State, memory, unknown cells, keep/init and ambiguous drivers are boundaries.\n");
        log("Search budgets retain earlier proved edits and leave remaining roots unchanged.\n");
    }
    void execute(std::vector<std::string>args,Design*design)override {
        WindowBudget budget;size_t argidx=1;
        if(argidx<args.size()&&args[argidx]=="-work-limit") {
            if(++argidx>=args.size())log_cmd_error("Missing work limit.\n");
            auto value=args[argidx++];
            if(value.empty()||value.size()>8||value.find_first_not_of("0123456789")!=std::string::npos)log_cmd_error("Invalid work limit.\n");
            budget.limit=std::stoll(value);if(budget.limit<1||budget.limit>80000000)log_cmd_error("Work limit out of range.\n");
        }
        extra_args(args,argidx,design);log_header(design,"Executing LUT_WINDOW exact cut pass.\n");
        CellTypes types(design);int roots=0,cuts=0,edits=0,saved=0,decomposed=0;bool stopped=false;
        for(auto module:design->selected_modules()) {
            if(!design->selected_whole_module(module)||module->get_blackbox_attribute()||module->has_processes()||module->get_bool_attribute(ID::keep))continue;
            // Charge and bound graph size before constructing SigMap or indices.
            // This also bounds large no-match inputs without whole-island rejection.
            int candidates=0;
            try {
                for(auto c:module->cells()) {
                    budget.charge();if(c->type==ID($lut))candidates++;
                    for(auto &p:c->connections())budget.charge(int64_t(GetSize(p.second))+1);
                }
                if(candidates<2)continue;
                for(auto w:module->wires())budget.charge(int64_t(w->width)+1);
                for(auto &c:module->connections())budget.charge(int64_t(GetSize(c.first))+GetSize(c.second)+1);
            }catch(WindowBudget::Stop&){stopped=true;break;}
            LutWindowWorker worker(module,types,budget);
            try{worker.run();}catch(WindowBudget::Stop&){stopped=true;}
            worker.verify_endpoint_depth();
            roots+=worker.roots;cuts+=worker.cuts_examined;edits+=worker.edits;saved+=worker.saved;decomposed+=worker.decomposed;
            if(stopped)break;
        }
        // Normalize only supported Boolean LUT pin permutations, preserving output.
        // Bounded memoization avoids repeated factorial work for equal functions.
        std::map<std::pair<int,int>,std::pair<int,std::vector<int>>> canonical;
        int normalized=0,visited=0;
        for(auto module:design->selected_modules()) {
            if(!design->selected_whole_module(module)||module->get_blackbox_attribute()||module->has_processes()||module->get_bool_attribute(ID::keep))continue;
            for(auto cell:module->selected_cells()) {
                if(++visited>16384)break;
                if(cell->type!=ID($lut)||cell->get_bool_attribute(ID::keep)||cell->attributes.count(ID::init))continue;
                auto pins=cell->getPort(ID::A);int n=GetSize(pins);
                if(n<1||n>4)continue;
                bool protected_output=false;
                for(auto bit:cell->getPort(ID::Y))if(bit.wire&&(bit.wire->get_bool_attribute(ID::keep)||bit.wire->attributes.count(ID::init)))protected_output=true;
                if(protected_output)continue;
                auto truth=cell->getParam(ID::LUT);
                if(GetSize(truth)!=(1<<n)||!truth.is_fully_def())continue;
                int t=truth.as_int();auto key=std::make_pair(n,t);auto it=canonical.find(key);
                if(it==canonical.end()) {
                    if(GetSize(canonical)>=512)continue;
                    std::vector<int> perm;for(int j=0;j<n;++j)perm.push_back(j);
                    auto best=std::make_pair(t,perm);
                    do {
                        int q=0;
                        for(int a=0;a<(1<<n);++a) {
                            int old=0;for(int j=0;j<n;++j)old|=((a>>j)&1)<<perm[j];
                            q|=((t>>old)&1)<<a;
                        }
                        if(q<best.first)best=std::make_pair(q,perm);
                    }while(std::next_permutation(perm.begin(),perm.end()));
                    it=canonical.emplace(key,best).first;
                }
                auto &best=it->second;if(best.first==t)continue;
                SigSpec reordered;for(int j:best.second)reordered.append(pins[j]);
                cell->setPort(ID::A,reordered);cell->setParam(ID::LUT,Const(best.first,1<<n));++normalized;
            }
            if(visited>16384)break;
        }
        log("  LUT_WINDOW pin permutations: normalized=%d functions=%d visited=%d\n",normalized,GetSize(canonical),visited);
        design->scratchpad_set_bool("lut_window.changed",edits!=0||normalized!=0);
        log("  LUT_WINDOW: roots=%d cuts=%d edits=%d saved=%d two_lut=%d work=%lld exhausted=%d\n",roots,cuts,edits,saved,decomposed,(long long)budget.used,stopped);
    }
} LutWindowPass;
