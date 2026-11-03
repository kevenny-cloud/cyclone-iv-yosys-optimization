// Factor fully defined one/two-input enables into literal ENA/SLOAD/D/ASDATA.
// Every accepted plan is checked over all independent data/state assignments.
struct FfEnableFoldPass : Pass {
    struct Limit { int64_t left=1000000; struct Stop{};
        void charge(int64_t n=1){if(n>left)throw Stop{};left-=n;} };
    struct EnablePlan { bool computed=false,valid=false;std::array<int,4> ports{}; };
    static bool literal(int role,bool a,bool b,bool d,bool q){
        return role==0?false:role==1?true:role==2?a:role==3?b:role==4?d:q;
    }
    static EnablePlan find_plan(int width,int mask,Limit&budget){
        EnablePlan result;result.computed=true;
        if(mask==0||mask==((1<<(1<<width))-1)){
            result.valid=true;result.ports={mask==0?0:1,0,4,5};return result;
        }
        for(int ena:{2,3,1,0})for(int load:{0,2,3})for(int data:{4,5})for(int alternate:{5,4}){
            bool valid=true;
            for(int row=0;row<16;++row){
                budget.charge();bool a=row&1,b=row&2,d=row&4,q=row&8;
                bool enabled=(mask>>(int(a)+(width==2?2*int(b):0)))&1;
                bool before=enabled?d:q;
                bool after=literal(ena,a,b,d,q)?(literal(load,a,b,d,q)?literal(alternate,a,b,d,q):literal(data,a,b,d,q)):q;
                if(before!=after){valid=false;break;}
            }
            if(valid){result.valid=true;result.ports={ena,load,data,alternate};return result;}
        }
        return result;
    }
    FfEnableFoldPass():Pass("ff_enable_fold","factor exclusive FF enables into unused synchronous controls"){}
    void help()override{
        log("    ff_enable_fold [selection]\n");
        log("Remove a one/two-input binary enable LUT used exclusively by compatible\n");
        log("dffeas ENA ports. Requires idle SCLR/SLOAD/ALOAD and inactive PRN.\n");
        log("Preserves clocks, async clear, data, state and power-up values.\n");
    }
    void execute(std::vector<std::string>args,Design*design)override{
        extra_args(args,1,design);log_header(design,"Executing FF_ENABLE_FOLD pass.\n");
        Limit budget;int removed=0,registers=0;bool exhausted=false;
        std::array<EnablePlan,32>cache;
        for(auto module:design->selected_modules()){
            if(!design->selected_whole_module(module)||module->has_processes()||module->get_blackbox_attribute()||module->get_bool_attribute(ID::keep))continue;
            try{
                bool has_ff=false;for(auto c:module->cells()){budget.charge();if(c->type==IdString("\\dffeas"))has_ff=true;}
                if(!has_ff)continue;
                for(auto w:module->wires())budget.charge(1+int64_t(w->width)*2);
                for(auto&c:module->connections())budget.charge(1+int64_t(c.first.size())+c.second.size());
                for(auto c:module->cells())for(auto&p:c->connections())budget.charge(1+int64_t(p.second.size())*2);
                SigMap sm(module);pool<SigBit>protected_bits,keep_init,primary;
                dict<SigBit,std::vector<Cell*>>drivers;
                dict<SigBit,std::vector<std::pair<Cell*,IdString>>>users;
                for(auto w:module->wires())for(auto b:sm(SigSpec(w))){
                    if(w->get_bool_attribute(ID::keep)||w->attributes.count(ID::init)||w->port_output)protected_bits.insert(b);
                    if(w->get_bool_attribute(ID::keep)||w->attributes.count(ID::init))keep_init.insert(b);
                    if(w->port_input)primary.insert(b);
                }
                for(auto c:module->cells())for(auto&p:c->connections())for(auto b:sm(p.second)){
                    budget.charge();if(!c->known()||c->output(p.first))drivers[b].push_back(c);
                    if(!c->known()||c->input(p.first))users[b].push_back({c,p.first});
                }
                struct Plan {Cell*gate;std::vector<Cell*>ffs;std::array<SigBit,2>pins;EnablePlan controls;int width,mask;};
                std::vector<Plan>plans;
                for(auto gate:module->cells()){
                    budget.charge();if(gate->get_bool_attribute(ID::keep)||gate->connections().size()!=2||
                        !gate->hasPort(ID::A)||!gate->hasPort(ID::Y)||gate->getPort(ID::Y).size()!=1)continue;
                    int width=gate->getPort(ID::A).size(),mask=1;
                    if(gate->type==ID($not)){if(width!=1)continue;}
                    else if(gate->type==ID($lut)){
                        if(width<1||width>2||!gate->hasParam(ID::WIDTH)||!gate->getParam(ID::WIDTH).is_fully_def()||
                            gate->getParam(ID::WIDTH).as_int()!=width||!gate->hasParam(ID::LUT)||
                            !gate->getParam(ID::LUT).is_fully_def()||gate->getParam(ID::LUT).size()!=(1<<width))continue;
                        mask=gate->getParam(ID::LUT).as_int();if(width==1&&mask!=1)continue;
                    }else continue;
                    auto input=sm(gate->getPort(ID::A));auto y=sm(gate->getPort(ID::Y))[0];
                    if(width==1&&!input[0].wire)continue;
                    if(!y.wire||primary.count(y)||protected_bits.count(y)||drivers[y].size()!=1||users[y].empty())continue;
                    bool good_inputs=true;
                    for(auto bit:input)if((!bit.wire&&bit.data!=State::S0&&bit.data!=State::S1)||bit==y||keep_init.count(bit))good_inputs=false;
                    if(!good_inputs)continue;
                    bool valid=true;pool<Cell*>seen;std::vector<Cell*>batch;
                    for(auto user:users[y]){
                        budget.charge(12);auto ff=user.first;
                        if(user.second!=IdString("\\ena")||ff->type!=IdString("\\dffeas")||ff->get_bool_attribute(ID::keep)||ff->connections().size()!=10){valid=false;break;}
                        for(auto name:{"d","q","clk","clrn","prn","ena","asdata","aload","sclr","sload"}){
                            auto id=IdString(std::string("\\")+name);
                            if(!ff->hasPort(id)||ff->getPort(id).size()!=1){valid=false;break;}
                        }
                        if(!valid)break;
                        auto port=[&](const char*n){return sm(ff->getPort(IdString(std::string("\\")+n)))[0];};
                        for(auto&p:ff->connections())for(auto b:sm(p.second)){
                            if(!b.wire&&b.data!=State::S0&&b.data!=State::S1)valid=false;
                            if(keep_init.count(b))valid=false;
                        }
                        auto q=port("q");
                        if(!q.wire||primary.count(q)||drivers[q].size()!=1||port("prn")!=State::S1||port("aload")!=State::S0||
                            port("sclr")!=State::S0||port("sload")!=State::S0)valid=false;
                        if(!valid)break;
                        if(!seen.count(ff)){seen.insert(ff);batch.push_back(ff);}
                    }
                    if(valid&&!batch.empty()){
                        int key=(width==1?0:16)+mask;
                        if(!cache[key].computed)cache[key]=find_plan(width,mask,budget);
                        if(cache[key].valid)plans.push_back({gate,batch,{input[0],width==2?input[1]:SigBit(State::S0)},cache[key],width,mask});
                    }
                }
                int count=0;for(auto&p:plans)count+=GetSize(p.ffs);
                budget.charge(int64_t(count)*64+int64_t(plans.size())*8);
                for(auto&p:plans){
                    for(auto ff:p.ffs){
                        // Exhaust the independent local transition relation.
                        for(int row=0;row<32;++row){bool a=row&1,b=row&2,d=row&4,q=row&8,clrn=row&16;
                            bool enabled=(p.mask>>(int(a)+(p.width==2?2*int(b):0)))&1;
                            auto v=[&](int role){return literal(role,a,b,d,q);};
                            bool before=clrn?(enabled?d:q):false;
                            bool after=clrn?(v(p.controls.ports[0])?(v(p.controls.ports[1])?v(p.controls.ports[3]):v(p.controls.ports[2])):q):false;
                            log_assert(before==after);
                        }
                        std::array<SigBit,6>bits={SigBit(State::S0),SigBit(State::S1),p.pins[0],p.pins[1],
                            sm(ff->getPort(IdString("\\d")))[0],sm(ff->getPort(IdString("\\q")))[0]};
                        ff->setPort(IdString("\\ena"),bits[p.controls.ports[0]]);
                        ff->setPort(IdString("\\sload"),bits[p.controls.ports[1]]);
                        ff->setPort(IdString("\\d"),bits[p.controls.ports[2]]);
                        ff->setPort(IdString("\\asdata"),bits[p.controls.ports[3]]);++registers;
                    }
                    module->remove(p.gate);++removed;
                }
            }catch(Limit::Stop&){exhausted=true;break;}
        }
        design->scratchpad_set_bool("ff_enable_fold.changed",removed!=0);
        log("FF_ENABLE_FOLD: removed=%d registers=%d work=%lld exhausted=%d\n",removed,registers,(long long)(1000000-budget.left),int(exhausted));
    }
} FfEnableFoldPass;
