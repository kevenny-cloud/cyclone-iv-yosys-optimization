// Exact reachable-input simplification for terminal FF-data LUTs.
// Bounded upstream LUT cones expose correlation hidden at a cut boundary.
struct LutCorrelationPass : Pass {
    struct Limit { int64_t left=1000000; struct Stop{};
        void charge(int64_t n=1){if(n>left)throw Stop{};left-=n;} };
    struct Skip {};
    LutCorrelationPass():Pass("lut_correlation","simplify correlated inputs of terminal register-data LUTs"){}
    void help() override {
        log("    lut_correlation [selection]\n");
        log("Exact binary reachability over at most 10 external inputs and 24\n");
        log("upstream LUTs, stopping at state/unknown/kept/ambiguous boundaries.\n");
        log("Only reduces existing FF-data LUT support, never adds a cell.\n");
    }
    void execute(std::vector<std::string>args,Design*design)override{
        extra_args(args,1,design);log_header(design,"Executing LUT_CORRELATION pass.\n");
        Limit budget;int scanned=0,edits=0;bool exhausted=false;
        for(auto module:design->selected_modules()){
            if(!design->selected_whole_module(module)||module->has_processes()||
                module->get_blackbox_attribute()||module->get_bool_attribute(ID::keep))continue;
            try{
                std::vector<Cell*>ffs;
                for(auto c:module->cells()){budget.charge();if(c->type==IdString("\\dffeas")&&!c->get_bool_attribute(ID::keep))ffs.push_back(c);}
                if(ffs.empty())continue;
                for(auto w:module->wires())budget.charge(1+int64_t(w->width)*2);
                for(auto&c:module->connections())budget.charge(1+int64_t(c.first.size())+c.second.size());
                for(auto c:module->cells())for(auto&p:c->connections())budget.charge(1+int64_t(p.second.size())*2);
                SigMap sm(module);dict<SigBit,std::vector<Cell*>>drivers;pool<SigBit>protected_bits,keep_init,primary;
                for(auto w:module->wires())for(auto b:sm(SigSpec(w))){
                    if(w->get_bool_attribute(ID::keep)||w->attributes.count(ID::init)||w->port_output)protected_bits.insert(b);
                    if(w->get_bool_attribute(ID::keep)||w->attributes.count(ID::init))keep_init.insert(b);
                    if(w->port_input)primary.insert(b);
                }
                for(auto c:module->cells())for(auto&p:c->connections())
                    if(!c->known()||c->output(p.first))for(auto b:sm(p.second)){budget.charge();drivers[b].push_back(c);}
                auto eligible=[&](Cell*c){
                    if(c->type!=ID($lut)||c->get_bool_attribute(ID::keep)||c->connections().size()!=2||
                        !c->hasPort(ID::A)||!c->hasPort(ID::Y)||c->getPort(ID::Y).size()!=1||
                        !c->hasParam(ID::WIDTH)||!c->hasParam(ID::LUT))return false;
                    int n=c->getPort(ID::A).size();
                    return n>=1&&n<=4&&c->getParam(ID::WIDTH).is_fully_def()&&c->getParam(ID::WIDTH).as_int()==n&&
                        c->getParam(ID::LUT).is_fully_def()&&c->getParam(ID::LUT).size()==(1<<n);
                };
                pool<Cell*>visited;
                for(auto ff:ffs){
                    budget.charge();
                    if(!ff->hasPort(IdString("\\d"))||ff->getPort(IdString("\\d")).size()!=1)continue;
                    auto rootbit=sm(ff->getPort(IdString("\\d")))[0];
                    if(!rootbit.wire||primary.count(rootbit)||protected_bits.count(rootbit)||drivers[rootbit].size()!=1)continue;
                    auto root=drivers[rootbit][0];if(!eligible(root)||visited.count(root))continue;
                    visited.insert(root);++scanned;
                    auto pins=sm(root->getPort(ID::A));int width=GetSize(pins);
                    if(width<2)continue;
                    try{
                        std::vector<SigBit>boundary;std::vector<Cell*>order;
                        pool<SigBit>done,active;dict<SigBit,unsigned>support;
                        std::function<void(SigBit,int)>visit=[&](SigBit b,int depth){
                            budget.charge();
                            if(!b.wire){if(b.data!=State::S0&&b.data!=State::S1)throw Skip{};return;}
                            if(done.count(b))return;
                            if(active.count(b)||b==rootbit)throw Skip{};
                            // Recursive insertion into drivers may rehash its
                            // storage. Keep a value copy across recursive visits.
                            auto ds=drivers[b];
                            if(!primary.count(b)&&!keep_init.count(b)&&ds.size()==1&&eligible(ds[0])){
                                if(depth>=8||order.size()>=24)throw Skip{};
                                active.insert(b);
                                for(auto p:sm(ds[0]->getPort(ID::A)))visit(p,depth+1);
                                active.erase(b);order.push_back(ds[0]);
                                if(order.size()>24)throw Skip{};
                                unsigned deps=0;for(auto p:sm(ds[0]->getPort(ID::A)))if(p.wire)deps|=support.at(p);
                                support[b]=deps;
                            }else{
                                boundary.push_back(b);if(boundary.size()>10)throw Skip{};
                                support[b]=1u<<(boundary.size()-1);
                            }
                            done.insert(b);
                        };
                        for(auto b:pins){if(keep_init.count(b))throw Skip{};visit(b,0);}
                        if(order.empty())continue;
                        // Independent structural supports cannot expose the
                        // shared-input correlation targeted here. Skip their
                        // exponential truth-table evaluation conservatively.
                        unsigned seen_support=0;bool overlap=false;
                        for(auto b:pins)if(b.wire){unsigned deps=support.at(b);overlap|=(seen_support&deps)!=0;seen_support|=deps;}
                        if(!overlap)continue;
                        const auto original=root->getParam(ID::LUT);
                        // Compile canonical signals to small integer slots once.
                        // The hot truth-table loop needs no SigMap, hash-table,
                        // parameter lookup or allocation per boundary assignment.
                        struct Flat {std::array<int,4> pins;int width,output;unsigned mask;};
                        dict<SigBit,int>slots;std::vector<Flat>flat;
                        for(int i=0;i<GetSize(boundary);++i)slots[boundary[i]]=i;
                        auto index=[&](SigBit b){
                            if(!b.wire)return b.data==State::S1?-2:-1;
                            log_assert(slots.count(b));return slots.at(b);
                        };
                        int next_slot=GetSize(boundary);
                        for(auto cell:order){
                            auto ps=sm(cell->getPort(ID::A));Flat node{};node.width=GetSize(ps);
                            budget.charge(2+node.width);
                            for(int i=0;i<node.width;++i)node.pins[i]=index(ps[i]);
                            auto y=sm(cell->getPort(ID::Y))[0];log_assert(!slots.count(y));
                            node.output=next_slot++;slots[y]=node.output;node.mask=cell->getParam(ID::LUT).as_int();flat.push_back(node);
                        }
                        log_assert(next_slot<=34);
                        std::array<int,4>root_pins;for(int i=0;i<width;++i)root_pins[i]=index(pins[i]);
                        unsigned reachable=0;std::vector<unsigned>addresses;addresses.reserve(1u<<boundary.size());
                        std::array<bool,34>values{};
                        auto bit=[&](int slot){return slot<0?slot==-2:values[slot];};
                        const unsigned all_patterns=(1u<<(1<<width))-1u;
                        for(unsigned row=0;row<(1u<<boundary.size());++row){
                            for(int i=0;i<GetSize(boundary);++i)values[i]=(row>>i)&1;
                            for(auto&node:flat){
                                budget.charge(2+node.width);unsigned addr=0;
                                for(int i=0;i<node.width;++i)addr|=unsigned(bit(node.pins[i]))<<i;
                                values[node.output]=(node.mask>>addr)&1;
                            }
                            budget.charge(width);unsigned addr=0;
                            for(int i=0;i<width;++i)addr|=unsigned(bit(root_pins[i]))<<i;
                            addresses.push_back(addr);reachable|=1u<<addr;
                            // Every cut-input pattern already occurs: further
                            // enumeration cannot expose a usable correlation.
                            if(reachable==all_patterns)break;
                        }
                        // Without correlation there is no reduced support to find.
                        if(reachable==all_patterns)continue;
                        auto mux_score=[&](unsigned table,int n){
                            auto matches=[&](int selector,int side,int literal){
                                for(int row=0;row<(1<<n);++row){
                                    budget.charge();if(selector>=0&&((row>>selector)&1)!=side)continue;
                                    bool value=literal<0?literal==-2:bool((row>>literal)&1);
                                    if(value!=bool((table>>row)&1))return false;
                                }return true;
                            };
                            for(int literal=-2;literal<n;++literal)if(matches(-1,0,literal))return 0;
                            for(int s=0;s<n;++s){bool arms[2]={false,false};
                                for(int side=0;side<2;++side)for(int literal=-2;literal<n;++literal)
                                    if(matches(s,side,literal)){arms[side]=true;break;}
                                if(arms[0]&&arms[1])return 1;
                            }return 2;
                        };
                        unsigned chosen=0,newtable=0;int chosen_size=-1,best_score=3;
                        for(int count=0;count<width&&chosen_size<0;++count)for(unsigned subset=0;subset<(1u<<width);++subset){
                            if(__builtin_popcount(subset)!=count)continue;
                            int output[16];std::fill(output,output+16,-1);bool valid=true;unsigned table=0;
                            for(unsigned addr=0;addr<(1u<<width);++addr)if((reachable>>addr)&1){
                                budget.charge(width+1);unsigned projected=0,pos=0;
                                for(int i=0;i<width;++i)if((subset>>i)&1)projected|=((addr>>i)&1)<<pos++;
                                int value=original[addr]==State::S1;
                                if(output[projected]>=0&&output[projected]!=value){valid=false;break;}
                                output[projected]=value;if(value)table|=1u<<projected;
                            }
                            if(valid){
                                unsigned defined=0;for(int i=0;i<(1<<count);++i)if(output[i]>=0)defined|=1u<<i;
                                unsigned all=(1u<<(1<<count))-1u;
                                // Both uniform completions preserve every reachable
                                // row. Prefer one representable by existing FF mux
                                // inputs, rather than arbitrarily introducing XOR.
                                for(unsigned completed:{table,table|(all^defined)}){
                                    int score=mux_score(completed,count);
                                    if(chosen_size<0||score<best_score){
                                        chosen=subset;newtable=completed;chosen_size=count;best_score=score;
                                    }
                                }
                            }
                        }
                        if(chosen_size<0)continue;
                        // Check every original cone input assignment, including
                        // assignments merged by the reachable-address projection.
                        for(auto addr:addresses){
                            budget.charge(width+1);unsigned projected=0,pos=0;
                            for(int i=0;i<width;++i)if((chosen>>i)&1)projected|=((addr>>i)&1)<<pos++;
                            log_assert((original[addr]==State::S1)==bool((newtable>>projected)&1));
                        }
                        SigSpec reduced;for(int i=0;i<width;++i)if((chosen>>i)&1)reduced.append(pins[i]);
                        if(chosen_size==0){reduced.append(State::S0);chosen_size=1;newtable=(newtable&1)?3:0;}
                        budget.charge(8);
                        root->setPort(ID::A,reduced);root->setParam(ID::WIDTH,chosen_size);
                        root->setParam(ID::LUT,Const(newtable,1<<chosen_size));++edits;
                    }catch(Skip&){}
                }
            }catch(Limit::Stop&){exhausted=true;break;}
        }
        design->scratchpad_set_bool("lut_correlation.changed",edits!=0);
        log("LUT_CORRELATION: scanned=%d reduced=%d work=%lld exhausted=%d\n",scanned,edits,(long long)(1000000-budget.left),int(exhausted));
    }
} LutCorrelationPass;
