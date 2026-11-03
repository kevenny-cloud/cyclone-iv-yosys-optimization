// Exact local absorption into unused Cyclone IV synchronous FF controls.
// This pass never adds a logic cell or a register and never uses design names.
struct LutFfLoadPass : Pass {
    struct Limit { int64_t left=1000000; struct Stop{};
        void charge(int64_t n=1){if(n>left)throw Stop{};left-=n;} };
    struct Plan {Cell *ff,*lut;SigBit data,load,alternate,clear;int controls;};
    LutFfLoadPass():Pass("lut_ff_load","absorb exclusive terminal LUTs into synchronous register controls"){}
    void help() override {
        log("    lut_ff_load [-premap] [-min-group N] [selection]\n");
        log("Binary LUT1--4 or scalar MUX driving only one dffeas D input. Uses free\n");
        log("SLOAD/ASDATA and optionally SCLR, preserving ENA/reset priority and\n");
        log("power-up state. Each edit removes a physical LUT/NOT and adds none.\n");
        log("Default minimum identical control group is one. Keep/init, unknown,\n");
        log("ambiguous drivers and partial module selections are boundaries.\n");
        log("Logical resource count only; physical LAB packing/timing is separate.\n");
    }
    bool binary(SigBit b) const {return b.wire||b.data==State::S0||b.data==State::S1;}
    void execute(std::vector<std::string>args,Design*design)override{
        int minimum=1;size_t argidx=1;bool premap=false;
        if(argidx<args.size()&&args[argidx]=="-premap"){premap=true;++argidx;}
        if(argidx<args.size()&&args[argidx]=="-min-group"){
            if(++argidx>=args.size())log_cmd_error("Missing group size.\n");
            auto text=args[argidx++];
            if(text.empty()||text.size()>4||text.find_first_not_of("0123456789")!=std::string::npos)log_cmd_error("Invalid group size.\n");
            minimum=std::stoi(text);if(minimum<1||minimum>1024)log_cmd_error("Group size out of range.\n");
        }
        extra_args(args,argidx,design);log_header(design,"Executing LUT_FF_LOAD exact absorption pass.\n");
        Limit budget;int scanned=0,candidates=0,edits=0;bool exhausted=false;
        for(auto module:design->selected_modules()){
            if(!design->selected_whole_module(module)||module->has_processes()||
                module->get_blackbox_attribute()||module->get_bool_attribute(ID::keep))continue;
            try{
                std::vector<Cell*>ffs;
                for(auto cell:module->cells()){
                    budget.charge();
                    if(cell->type==IdString("\\dffeas")&&!cell->get_bool_attribute(ID::keep))ffs.push_back(cell);
                }
                if(ffs.empty())continue;
                scanned+=GetSize(ffs);
                for(auto w:module->wires())budget.charge(1+int64_t(w->width)*3);
                for(auto &c:module->connections())budget.charge(1+int64_t(c.first.size())+c.second.size());
                for(auto c:module->cells())for(auto&p:c->connections())budget.charge(1+int64_t(p.second.size())*3);
                SigMap sm(module);pool<SigBit>protected_bits,keep_init,primary;
                dict<SigBit,std::vector<Cell*>>drivers;
                dict<SigBit,std::vector<std::pair<Cell*,IdString>>>users;
                for(auto w:module->wires())for(auto b:sm(SigSpec(w))){
                    if(w->get_bool_attribute(ID::keep)||w->attributes.count(ID::init)||w->port_output)protected_bits.insert(b);
                    if(w->get_bool_attribute(ID::keep)||w->attributes.count(ID::init))keep_init.insert(b);
                    if(w->port_input)primary.insert(b);
                }
                for(auto c:module->cells()){
                    bool known=c->known();
                    for(auto&p:c->connections())for(auto b:sm(p.second)){
                        budget.charge();
                        if(!known||c->output(p.first))drivers[b].push_back(c);
                        if(!known||c->input(p.first))users[b].push_back({c,p.first});
                    }
                }
                std::vector<Plan>possible;
                const SigBit zero(State::S0),one(State::S1);
                for(auto ff:ffs){
                    budget.charge(12);bool valid=ff->connections().size()==10;
                    for(auto name:{"d","q","clk","clrn","prn","ena","asdata","aload","sclr","sload"})
                        if(!ff->hasPort(IdString(std::string("\\")+name))||ff->getPort(IdString(std::string("\\")+name)).size()!=1)valid=false;
                    if(!valid)continue;
                    auto port=[&](const char *name){return sm(ff->getPort(IdString(std::string("\\")+name)))[0];};
                    if(port("sload")!=zero||port("aload")!=zero||port("prn")!=one)continue;
                    // A top-level FF Q is an ordinary observable register output,
                    // and is preserved; only keep/init there is a boundary.
                    auto q=port("q");
                    if(!q.wire||primary.count(q)||drivers[q].size()!=1)continue;
                    // Re-evaluate protection excluding ordinary output Q.
                    valid=true;
                    for(auto&p:ff->connections())for(auto b:sm(p.second)){
                        if(!binary(b))valid=false;
                        // Reading an ordinary observable input does not modify
                        // that signal. Keep/init contracts still block rewriting.
                        if(p.first!=IdString("\\q")&&keep_init.count(b))valid=false;
                    }
                    if(keep_init.count(q))valid=false;
                    if(!valid||port("ena")==zero||port("clrn")==zero||port("sclr")==one)continue;
                    auto output=port("d");
                    if(!output.wire||protected_bits.count(output)||primary.count(output)||drivers[output].size()!=1||
                        users[output].size()!=1||users[output][0]!=std::make_pair(ff,IdString("\\d")))continue;
                    auto lut=drivers[output][0];
                    if(lut->get_bool_attribute(ID::keep)||!lut->hasPort(ID::Y)||lut->getPort(ID::Y).size()!=1)continue;
                    // Before mapping, do not split a small primary-input function
                    // that ABC9 can cover with one LUT. Such a split can expose
                    // several separately mapped cofactors despite deleting a gate.
                    if(premap) {
                        std::set<SigBit> leaves,visiting,done;int internal=0;
                        std::function<bool(SigBit)> small_primary=[&](SigBit b) {
                            budget.charge();
                            if(!b.wire)return b.data==State::S0||b.data==State::S1;
                            if(keep_init.count(b))return false;
                            if(primary.count(b)){leaves.insert(b);return leaves.size()<=4;}
                            if(done.count(b))return true;
                            if(visiting.count(b)||internal>=64||drivers[b].size()!=1)return false;
                            auto c=drivers[b][0];
                            if(c->get_bool_attribute(ID::keep)||!c->type.in(ID($_MUX_),ID($_AND_),ID($_OR_),ID($_XOR_),ID($_XNOR_),ID($_NAND_),ID($_NOR_),ID($_NOT_),ID($lut)))return false;
                            if(!c->hasPort(ID::Y)||sm(c->getPort(ID::Y))!=SigSpec(b))return false;
                            visiting.insert(b);++internal;
                            for(auto &p:c->connections())if(p.first!=ID::Y)for(auto x:sm(p.second))if(!small_primary(x))return false;
                            visiting.erase(b);done.insert(b);return true;
                        };
                        if(small_primary(output)&&internal>=2)continue;
                    }
                    SigSpec pins;Const mask;int width=0;
                    if(lut->type==ID($_MUX_)) {
                        if(lut->connections().size()!=4||!lut->hasPort(ID::A)||!lut->hasPort(ID::B)||!lut->hasPort(ID::S)||
                            GetSize(lut->getPort(ID::A))!=1||GetSize(lut->getPort(ID::B))!=1||GetSize(lut->getPort(ID::S))!=1)continue;
                        pins.append(sm(lut->getPort(ID::A)));pins.append(sm(lut->getPort(ID::B)));pins.append(sm(lut->getPort(ID::S)));
                        width=3;mask=Const(0xca,8); // A,B,S: S ? B : A
                    } else if(lut->type.in(ID($_AND_),ID($_OR_),ID($_NOT_))) {
                        const bool unary=lut->type==ID($_NOT_);
                        if(lut->connections().size()!=(unary?2:3)||!lut->hasPort(ID::A)||GetSize(lut->getPort(ID::A))!=1||
                            (!unary&&(!lut->hasPort(ID::B)||GetSize(lut->getPort(ID::B))!=1)))continue;
                        pins.append(sm(lut->getPort(ID::A)));
                        if(!unary)pins.append(sm(lut->getPort(ID::B)));
                        width=unary?1:2;
                        mask=Const(unary?1:lut->type==ID($_AND_)?8:14,1<<width);
                    } else if(lut->type==ID($lut)) {
                        if(lut->connections().size()!=2||!lut->hasPort(ID::A)||!lut->hasParam(ID::WIDTH)||!lut->hasParam(ID::LUT))continue;
                        pins=sm(lut->getPort(ID::A));width=GetSize(pins);mask=lut->getParam(ID::LUT);
                        if(width<1||width>4||!lut->getParam(ID::WIDTH).is_fully_def()||
                            lut->getParam(ID::WIDTH).as_int()!=width||!mask.is_fully_def()||mask.size()!=(1<<width))continue;
                    } else continue;
                    // Unary constants and buffers are already free in cells_map.
                    if(width==1&&(mask.as_int()!=1||!pins[0].wire))continue;
                    std::vector<SigBit>vars;
                    for(auto b:pins){
                        if(!binary(b)||keep_init.count(b))valid=false;
                        if(b.wire&&std::find(vars.begin(),vars.end(),b)==vars.end())vars.push_back(b);
                    }
                    if(!valid)continue;
                    // Compile canonical signals once; row loops only inspect
                    // integer slots. -1/-2 are constants, -3 is an independent
                    // control outside the LUT's variables (unrestricted care).
                    auto slot=[&](SigBit b){
                        if(!b.wire)return b.data==State::S1?-2:-1;
                        auto it=std::find(vars.begin(),vars.end(),b);
                        return it==vars.end()?-3:int(it-vars.begin());
                    };
                    auto value=[&](int index,int row){
                        log_assert(index!=-3);
                        return index<0?index==-2:bool((row>>index)&1);
                    };
                    auto fixed=[&](int index,bool desired,int row){return index==-3||value(index,row)==desired;};
                    std::array<int,4>pin_slots;
                    for(int i=0;i<width;++i)pin_slots[i]=slot(pins[i]);
                    const auto old_ena=port("ena"),old_clrn=port("clrn"),old_sclr=port("sclr");
                    const int ena_slot=slot(old_ena),clrn_slot=slot(old_clrn),sclr_slot=slot(old_sclr);
                    std::vector<int>care;std::vector<bool>truth(1<<GetSize(vars));
                    for(int row=0;row<GetSize(truth);++row){
                        budget.charge(2+width);
                        int address=0;for(int i=0;i<width;++i)address|=int(value(pin_slots[i],row))<<i;
                        truth[row]=mask[address]==State::S1;
                        if(fixed(ena_slot,true,row)&&fixed(clrn_slot,true,row)&&fixed(sclr_slot,false,row))care.push_back(row);
                    }
                    if(care.empty())continue;
                    std::vector<SigBit>literals={zero,one};literals.insert(literals.end(),vars.begin(),vars.end());
                    auto literal=[&](const std::vector<int>&rows,SigBit &result){
                        if(rows.empty()){result=zero;return true;}
                        for(auto b:literals){
                            bool match=true;const int literal_slot=slot(b);
                            for(int row:rows){budget.charge();if(value(literal_slot,row)!=truth[row]){match=false;break;}}
                            if(match){result=b;return true;}
                        }
                        return false;
                    };
                    SigBit direct;
                    if(literal(care,direct)){
                        possible.push_back({ff,lut,direct,zero,port("asdata"),port("sclr"),0});++candidates;continue;
                    }
                    auto try_mux=[&](SigBit clear,bool new_clear){
                        std::vector<int>rows;const int clear_slot=slot(clear);
                        for(int row:care){
                            budget.charge();
                            if(new_clear&&value(clear_slot,row)){if(truth[row])return;continue;}
                            rows.push_back(row);
                        }
                        for(auto select:vars){
                            std::vector<int>low,high;const int select_slot=slot(select);
                            for(int row:rows)(value(select_slot,row)?high:low).push_back(row);
                            SigBit d,alternate;
                            if(literal(low,d)&&literal(high,alternate))
                                possible.push_back({ff,lut,d,select,alternate,clear,new_clear?2:1});
                        }
                    };
                    auto old_size=possible.size();
                    try_mux(port("sclr"),false);
                    if(port("sclr")==zero)for(auto clear:vars)try_mux(clear,true);
                    if(possible.size()!=old_size)++candidates;
                }
                // Prefer fewer new control signals, then larger sharing groups.
                using Key=std::tuple<SigBit,SigBit,SigBit,SigBit,SigBit,int>;
                std::map<Key,std::vector<Plan>>groups;
                for(auto&p:possible){
                    auto port=[&](const char*n){return sm(p.ff->getPort(IdString(std::string("\\")+n)))[0];};
                    groups[Key(port("clk"),port("clrn"),port("ena"),p.clear,p.load,p.controls)].push_back(p);
                }
                std::vector<std::pair<Key,std::vector<Plan>>>ordered(groups.begin(),groups.end());
                std::stable_sort(ordered.begin(),ordered.end(),[](const auto&a,const auto&b){
                    if(std::get<5>(a.first)!=std::get<5>(b.first))return std::get<5>(a.first)<std::get<5>(b.first);
                    if(a.second.size()!=b.second.size())return a.second.size()>b.second.size();
                    return a.first<b.first;
                });
                pool<Cell*>selected;std::vector<Plan>accepted;
                for(auto&group:ordered){
                    pool<Cell*>unique;std::vector<Plan>batch;
                    for(auto&p:group.second)if(!selected.count(p.ff)&&!unique.count(p.ff)){
                        unique.insert(p.ff);batch.push_back(p);
                    }
                    if(!batch.empty()&&(batch[0].controls==0||GetSize(batch)>=minimum)){
                        for(auto&p:batch){accepted.push_back(p);selected.insert(p.ff);}
                    }
                }
                // Independently enumerate the complete old/new binary transition
                // relation, including both prior states and reset/enable aliases.
                // This check uses the original LUT mask, not cofactor care rows.
                for(auto&p:accepted){
                    std::vector<SigBit>inputs;
                    auto port=[&](const char*n){return sm(p.ff->getPort(IdString(std::string("\\")+n)))[0];};
                    SigSpec pins;Const mask;
                    if(p.lut->type==ID($_MUX_)) {
                        pins.append(sm(p.lut->getPort(ID::A)));pins.append(sm(p.lut->getPort(ID::B)));pins.append(sm(p.lut->getPort(ID::S)));
                        mask=Const(0xca,8);
                    } else if(p.lut->type.in(ID($_AND_),ID($_OR_),ID($_NOT_))) {
                        const bool unary=p.lut->type==ID($_NOT_);
                        pins.append(sm(p.lut->getPort(ID::A)));
                        if(!unary)pins.append(sm(p.lut->getPort(ID::B)));
                        mask=Const(unary?1:p.lut->type==ID($_AND_)?8:14,unary?2:4);
                    } else {pins=sm(p.lut->getPort(ID::A));mask=p.lut->getParam(ID::LUT);}
                    const auto old_clrn=port("clrn"),old_ena=port("ena"),old_sclr=port("sclr");
                    for(auto b:pins)if(b.wire&&std::find(inputs.begin(),inputs.end(),b)==inputs.end())inputs.push_back(b);
                    for(auto b:{old_clrn,old_ena,old_sclr,p.data,p.load,p.alternate,p.clear})
                        if(b.wire&&std::find(inputs.begin(),inputs.end(),b)==inputs.end())inputs.push_back(b);
                    log_assert(inputs.size()<=8);
                    auto slot=[&](SigBit b){
                        if(!b.wire)return b.data==State::S1?-2:-1;
                        auto it=std::find(inputs.begin(),inputs.end(),b);log_assert(it!=inputs.end());
                        return int(it-inputs.begin());
                    };
                    auto bit=[&](int index,int row){return index<0?index==-2:bool((row>>index)&1);};
                    std::array<int,4>pin_slots;
                    for(int i=0;i<GetSize(pins);++i)pin_slots[i]=slot(pins[i]);
                    const int clrn_slot=slot(old_clrn),ena_slot=slot(old_ena),sclr_slot=slot(old_sclr);
                    const int data_slot=slot(p.data),load_slot=slot(p.load),alternate_slot=slot(p.alternate),clear_slot=slot(p.clear);
                    for(int row=0;row<(1<<GetSize(inputs));++row){
                        budget.charge(20+GetSize(pins));
                        int address=0;for(int i=0;i<GetSize(pins);++i)address|=int(bit(pin_slots[i],row))<<i;
                        for(bool state:{false,true}){
                            bool before=!bit(clrn_slot,row)?false:!bit(ena_slot,row)?state:
                                bit(sclr_slot,row)?false:mask[address]==State::S1;
                            bool after=!bit(clrn_slot,row)?false:!bit(ena_slot,row)?state:
                                bit(clear_slot,row)?false:bit(load_slot,row)?bit(alternate_slot,row):bit(data_slot,row);
                            log_assert(before==after);
                        }
                    }
                }
                budget.charge(int64_t(accepted.size())*16);
                for(auto&p:accepted){
                    // No new cells, inversion, state or initial-value edits.
                    p.ff->setPort(IdString("\\d"),p.data);
                    p.ff->setPort(IdString("\\sload"),p.load);
                    p.ff->setPort(IdString("\\asdata"),p.alternate);
                    p.ff->setPort(IdString("\\sclr"),p.clear);
                    module->remove(p.lut);++edits;
                }
            }catch(Limit::Stop&){exhausted=true;break;}
        }
        design->scratchpad_set_bool("lut_ff_load.changed",edits!=0);
        log("LUT_FF_LOAD: scanned_ff=%d candidates=%d edits=%d saved_luts=%d work=%lld exhausted=%d\n",
            scanned,candidates,edits,edits,(long long)(1000000-budget.left),int(exhausted));
    }
} LutFfLoadPass;
