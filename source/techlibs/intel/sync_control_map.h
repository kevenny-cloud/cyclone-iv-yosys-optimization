// Exact dedicated-control mapping for groups sharing a clock/reset/enable.
// No design identifiers or measured answers enter the candidate selection.
struct SyncControlMapPass:Pass {
    SyncControlMapPass():Pass("map_sync_controls","share dedicated Cyclone IV synchronous register controls"){}
    void help()override{
        log("    map_sync_controls [-min-group N] [selection]\n");
        log("Map fully selected groups of binary fine SDFF/SDFFE/SDFFCE cells to\n");
        log("dedicated SCLR/SLOAD controls, preserving both priorities and all\n");
        log("clock/reset/enable polarities. Default minimum group size is two.\n");
        log("Keep objects, ambiguous Q drivers and unknown controls are skipped.\n");
        log("Wire initialization is transferred to the physical power_up parameter.\n");
        log("A one-million bit-work budget bounds preflight and grouping.\n");
        log("Direct primitive construction avoids reparsing templates. LAB fitting is a\n");
        log("physical constraint, not proved by the primitive resource counter.\n");
    }
    bool synchronous_type(Cell*c)const{
        auto name=c->type.str();size_t n=0;
        if(name.compare(0,8,"$_SDFFE_")==0)n=8;
        else if(name.compare(0,9,"$_SDFFCE_")==0)n=9;
        else if(name.compare(0,7,"$_SDFF_")==0)n=7;
        else return false;
        if(name.size()!=n+(n==7?4:5)||name.back()!='_')return false;
        return (name[n]=='P'||name[n]=='N')&&(name[n+1]=='P'||name[n+1]=='N')&&
            (name[n+2]=='0'||name[n+2]=='1')&&(n==7||name[n+3]=='P'||name[n+3]=='N');
    }
    void execute(std::vector<std::string>args,Design*design)override{
        int minimum=2;size_t argidx=1;
        if(argidx<args.size()&&args[argidx]=="-min-group"){
            if(++argidx>=args.size())log_cmd_error("Missing group size.\n");
            auto value=args[argidx++];
            if(value.empty()||value.size()>4||value.find_first_not_of("0123456789")!=std::string::npos)log_cmd_error("Invalid group size.\n");
            minimum=std::stoi(value);if(minimum<1||minimum>1024)log_cmd_error("Group size out of range.\n");
        }
        extra_args(args,argidx,design);log_header(design,"Executing MAP_SYNC_CONTROLS pass.\n");
        struct Stop{};int64_t remaining=1000000;
        auto charge=[&](int64_t n){if(n>remaining)throw Stop{};remaining-=n;};
        RTLIL::Selection eligible(false,false,design);std::unique_ptr<CellTypes> types;
        int candidates=0,mapped=0,groups=0,singletons=0;bool exhausted=false;
        for(auto module:design->selected_modules()){
            if(!design->selected_whole_module(module)||module->has_processes()||module->get_blackbox_attribute()||module->get_bool_attribute(ID::keep))continue;
            try{
                std::vector<Cell*>possible;
                for(auto cell:module->cells()){
                    charge(1);
                    if(!synchronous_type(cell)||cell->get_bool_attribute(ID::keep)||!cell->parameters.empty())continue;
                    bool no_enable=cell->type.str().compare(0,7,"$_SDFF_")==0;
                    bool valid=cell->connections().size()==(no_enable?4:5);
                    for(auto port:{ID::D,ID::C,ID::R,ID::Q})
                        if(!cell->hasPort(port)||cell->getPort(port).size()!=1)valid=false;
                    if(!no_enable&&(!cell->hasPort(ID::E)||cell->getPort(ID::E).size()!=1))valid=false;
                    if(valid)possible.push_back(cell);
                }
                candidates+=GetSize(possible);
                if(GetSize(possible)<minimum){singletons+=GetSize(possible);continue;}
                for(auto wire:module->wires())charge(1+int64_t(wire->width)*3);
                for(auto &conn:module->connections())charge(1+int64_t(conn.first.size())+conn.second.size());
                for(auto cell:module->cells())for(auto &port:cell->connections())charge(1+int64_t(port.second.size())*3);
                SigMap sm(module);pool<SigBit>protected_bits;dict<SigBit,int>drivers;
                if(!types){types.reset(new CellTypes());types->setup();}
                for(auto wire:module->wires())for(auto bit:sm(SigSpec(wire))){
                    if(wire->get_bool_attribute(ID::keep))protected_bits.insert(bit);
                    if(wire->port_input&&bit.wire)++drivers[bit];
                }
                for(auto cell:module->cells()){
                    bool known=types->cell_known(cell->type);
                    for(auto &port:cell->connections())if(!known||types->cell_output(cell->type,port.first))
                        for(auto bit:sm(port.second)){charge(1);if(bit.wire)++drivers[bit];}
                }
                using Key=std::tuple<IdString,SigBit,SigBit,SigBit>;
                std::map<Key,std::vector<Cell*>>buckets;
                for(auto cell:possible){
                    charge(8);bool protect=false;
                    for(auto &port:cell->connections())for(auto bit:sm(port.second))
                        if(protected_bits.count(bit)||(!bit.wire&&bit.data!=State::S0&&bit.data!=State::S1))protect=true;
                    auto q=sm(cell->getPort(ID::Q))[0];
                    if(!q.wire||!drivers.count(q)||drivers.at(q)!=1)protect=true;
                    if(protect)continue;
                    auto enable=cell->hasPort(ID::E)?sm(cell->getPort(ID::E))[0]:SigBit(State::S1);
                    auto key=std::make_tuple(cell->type,sm(cell->getPort(ID::C))[0],sm(cell->getPort(ID::R))[0],enable);
                    buckets[key].push_back(cell);
                }
                // A completed module plan is committed to the selection only
                // after every budgeted read. No graph is half transformed.
                // Bare SDFFs do not remove an enable mux and can expose
                // additional shared data cones to the LUT mapper. Include them
                // only to complete an enabled synchronous reset domain; mapping
                // only part of that domain can lose existing sharing instead.
                using Domain=std::tuple<SigBit,SigBit,char,char,char>;
                auto domain=[&](Cell*cell) {
                    auto type=cell->type.str();size_t pos=type.compare(0,7,"$_SDFF_")==0?7:type.compare(0,8,"$_SDFFE_")==0?8:9;
                    return Domain(sm(cell->getPort(ID::C))[0],sm(cell->getPort(ID::R))[0],type[pos],type[pos+1],type[pos+2]);
                };
                std::set<Domain> enabled_domains;
                for(auto &group:buckets)if(GetSize(group.second)>=minimum&&group.second[0]->hasPort(ID::E))
                    enabled_domains.insert(domain(group.second[0]));
                for(auto &group:buckets){
                    bool bare=!group.second[0]->hasPort(ID::E);
                    if(GetSize(group.second)<minimum||(bare&&minimum>1&&!enabled_domains.count(domain(group.second[0])))){
                        singletons+=GetSize(group.second);continue;
                    }
                    ++groups;
                    for(auto cell:group.second){eligible.select(module,cell);++mapped;}
                }
            }catch(Stop&){exhausted=true;break;}
        }
        if(mapped){
            // All work has been precharged. Each selected cell becomes exactly
            // one physical FF, with shared scalar inversions and enable gates.
            for (auto module:design->selected_modules()) {
                std::vector<Cell*> cells;
                for(auto cell:module->cells()) if(eligible.selected_member(module->name,cell->name)) cells.push_back(cell);
                if(cells.empty()) continue;
                SigMap sm(module); FfInitVals inits(&sm,module);
                dict<SigBit,SigBit> inverses;
                std::map<std::pair<SigBit,SigBit>,SigBit> enables;
                pool<SigBit> transferred;
                auto invert=[&](SigBit x) {
                    x=sm(x);
                    if(!x.wire) return SigBit(x.data==State::S0?State::S1:State::S0);
                    if(inverses.count(x)) return inverses.at(x);
                    SigBit y(module->addWire(NEW_ID));
                    module->addNotGate(NEW_ID,x,y); inverses[x]=y;return y;
                };
                auto either=[&](SigBit x,SigBit y) {
                    if(x==SigBit(State::S1)||y==SigBit(State::S1))return SigBit(State::S1);
                    if(x==SigBit(State::S0)||x==y)return y;
                    if(y==SigBit(State::S0))return x;
                    if(y<x)std::swap(x,y);
                    auto key=std::make_pair(x,y);
                    if(enables.count(key))return enables.at(key);
                    SigBit z(module->addWire(NEW_ID));module->addOrGate(NEW_ID,x,y,z);
                    enables[key]=z;return z;
                };
                for(auto cell:cells) {
                    auto name=cell->type.str();bool no_enable=name.compare(0,7,"$_SDFF_")==0;
                    bool reset_priority=name.compare(0,8,"$_SDFFE_")==0;
                    size_t pos=no_enable?7:reset_priority?8:9;
                    auto q=cell->getPort(ID::Q)[0],d=cell->getPort(ID::D)[0];
                    auto clock=sm(cell->getPort(ID::C))[0],reset=sm(cell->getPort(ID::R))[0];
                    auto enable=no_enable?SigBit(State::S1):sm(cell->getPort(ID::E))[0];
                    bool high=name[pos+2]=='1';State initial=inits(q);
                    if(name[pos]=='N')clock=invert(clock);
                    if(name[pos+1]=='N')reset=invert(reset);
                    if(!no_enable&&name[pos+3]=='N')enable=invert(enable);
                    if(reset_priority)enable=either(enable,reset);
                    transferred.insert(sm(q));
                    cell->type=IdString("\\dffeas");cell->connections_.clear();cell->parameters.clear();
                    cell->setParam(IdString("\\power_up"),Const(initial==State::S1?"high":"low"));
                    cell->setParam(IdString("\\is_wysiwyg"),Const("TRUE"));
                    for(auto item:std::vector<std::pair<const char*,SigBit>>{
                        {"d",d},{"q",q},{"clk",clock},{"ena",enable},
                        {"clrn",SigBit(State::S1)},{"prn",SigBit(State::S1)},
                        {"aload",SigBit(State::S0)},{"asdata",SigBit(high?State::S1:State::S0)},
                        {"sclr",high?SigBit(State::S0):reset},{"sload",high?reset:SigBit(State::S0)}})
                        cell->setPort(IdString(std::string("\\")+item.first),item.second);
                }
                // Clear every initialized alias of a transferred Q, including
                // duplicate identical attributes, without touching other bits.
                for(auto wire:module->wires())if(wire->attributes.count(ID::init)) {
                    Const value=wire->attributes.at(ID::init);bool changed=false;
                    for(int i=0;i<GetSize(value)&&i<wire->width;++i)
                        if(transferred.count(sm(SigBit(wire,i)))){value.set(i,State::Sx);changed=true;}
                    if(changed){if(value.is_fully_undef())wire->attributes.erase(ID::init);else wire->attributes[ID::init]=value;}
                }
            }
        }
        design->scratchpad_set_bool("map_sync_controls.changed",mapped!=0);
        log("SYNC_CONTROL_MAP: candidates=%d groups=%d mapped_ff=%d singleton_skipped=%d work=%lld exhausted=%d\n",candidates,groups,mapped,singletons,(long long)(1000000-remaining),int(exhausted));
    }
} SyncControlMapPass;
