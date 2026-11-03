// PMUX-local distribution of a common XOR operand. No generic arithmetic scan.
struct PmuxXorFactorPass : public Pass {
 PmuxXorFactorPass():Pass("pmux_xor_factor","factor a common XOR operand only across proven exclusive PMUX lanes"){}
 void execute(std::vector<std::string> args, Design *design) override {
  extra_args(args,1,design); log_header(design,"Executing PMUX_XOR_FACTOR pass.\n");
  int changed=0,removed=0;
  CellTypes types;types.setup();
  for(auto m:design->selected_modules()) {
   if(m->has_processes()||m->get_bool_attribute(ID::keep)||m->get_blackbox_attribute())continue;
   for (;;) {
   bool round_changed=false;
   SigMap sm(m);dict<SigBit,std::vector<std::pair<Cell*,IdString>>> drivers;dict<SigBit,int> users;pool<SigBit> kept,init;
   for(auto w:m->wires())for(auto b:sm(SigSpec(w))) {if(w->get_bool_attribute(ID::keep))kept.insert(b);if(w->attributes.count(ID::init))init.insert(b);if(w->port_output)users[b]++;if(w->port_input)drivers[b].push_back({nullptr,IdString()});}
   for(auto c:m->cells())for(auto &p:c->connections())for(auto b:sm(p.second)) {
    if(types.cell_output(c->type,p.first))drivers[b].push_back({c,p.first});
    if(types.cell_input(c->type,p.first))users[b]++;
   }
   auto protected_cell=[&](Cell*c) {if(c->get_bool_attribute(ID::keep))return true;for(auto&p:c->connections())for(auto b:sm(p.second)){if(kept.count(b))return true;if(init.count(b)) {if(types.cell_output(c->type,p.first))return true;auto ds=drivers[b];if(GetSize(ds)!=1||!ds[0].first||ds[0].second!=ID::Q||!ds[0].first->type.in(ID($dff),ID($adff),ID($dffe),ID($adffe),ID($sdff),ID($sdffe),ID($sdffce),ID($ff)))return true;}}return false;};
   auto scalar_param=[](Cell*c,IdString p,int n){return c->hasParam(p)&&c->getParam(p).is_fully_def()&&c->getParam(p).try_as_int()==n;};
   for(auto mux:m->selected_cells()) {
    if(mux->type!=ID($pmux)||protected_cell(mux))continue;
    bool valid=true;for(auto p:{ID::A,ID::B,ID::S,ID::Y})if(!mux->hasPort(p))valid=false;if(!valid)continue;
    auto a=sm(mux->getPort(ID::A)),b=sm(mux->getPort(ID::B)),sel=sm(mux->getPort(ID::S)),y=sm(mux->getPort(ID::Y));int w=GetSize(y),n=GetSize(sel);
    if(w<1||w>4096||n<2||n>32||GetSize(a)!=w||GetSize(b)!=w*n||!scalar_param(mux,ID::WIDTH,w)||!scalar_param(mux,ID::S_WIDTH,n))continue;
    for(auto bit:y)if(GetSize(drivers[bit])!=1||drivers[bit][0].first!=mux||init.count(bit))valid=false;if(!valid)continue;
    // Distinct equalities over exactly one canonical control vector prove exclusivity.
    SigSpec control;pool<int> addresses;
    for(auto bit:sel) {
     if(GetSize(drivers[bit])!=1){valid=false;break;}auto d=drivers[bit][0];auto c=d.first;
     if(!c||d.second!=ID::Y||!c->type.in(ID($eq),ID($logic_not))||protected_cell(c)||!c->hasPort(ID::A)||!scalar_param(c,ID::Y_WIDTH,1)){valid=false;break;}
     auto ca=sm(c->getPort(ID::A));int value=0;
     if(c->type==ID($eq)){if(!c->hasPort(ID::B)){valid=false;break;}auto cb=sm(c->getPort(ID::B));if(ca.is_fully_const())std::swap(ca,cb);if(!cb.is_fully_const()||!cb.as_const().is_fully_def()||GetSize(cb)>5||GetSize(ca)!=GetSize(cb)){valid=false;break;}value=cb.as_const().as_int();}
     if(GetSize(ca)<1||GetSize(ca)>5||ca.is_fully_const()||addresses.count(value)){valid=false;break;}
     pool<SigBit> bits;for(auto z:ca)if(!z.wire||bits.count(z))valid=false;else bits.insert(z);
     if(control.empty())control=ca;else if(control!=ca)valid=false;addresses.insert(value);
    }
    if(!valid)continue;
    std::vector<Cell*> ops;std::vector<SigSpec> left,right;pool<Cell*> unique;
    for(int k=-1;k<n;k++) {
     auto lane=k<0?a:b.extract(k*w,w);auto ds=drivers[lane[0]];if(GetSize(ds)!=1||!ds[0].first||ds[0].second!=ID::Y){valid=false;break;}auto op=ds[0].first;
     if(!op->type.in(ID($xor),ID($add),ID($sub))||protected_cell(op)||!op->hasPort(ID::A)||!op->hasPort(ID::B)||sm(op->getPort(ID::Y))!=lane||!scalar_param(op,ID::A_WIDTH,w)||!scalar_param(op,ID::B_WIDTH,w)||!scalar_param(op,ID::Y_WIDTH,w)||!scalar_param(op,ID::A_SIGNED,0)||!scalar_param(op,ID::B_SIGNED,0)){valid=false;break;}
     for(auto bit:lane)if(GetSize(drivers[bit])!=1||drivers[bit][0].first!=op||users[bit]!=1)valid=false;
     if(unique.count(op)||(!ops.empty()&&ops[0]->type!=op->type)){valid=false;break;}unique.insert(op);ops.push_back(op);left.push_back(sm(op->getPort(ID::A)));right.push_back(sm(op->getPort(ID::B)));
    }
    if(!valid)continue;
    if (ops[0]->type.in(ID($add),ID($sub))) {
     // Uniform unsigned arithmetic over mutually exclusive branches. Move
     // the operation past the selector; never cross a state element.
     SigSpec lb,rb;for(int k=1;k<GetSize(ops);++k){lb.append(left[k]);rb.append(right[k]);}
     auto lm=m->addWire(NEW_ID,w),rm=m->addWire(NEW_ID,w);
     auto lc=m->addCell(NEW_ID,ID($pmux));lc->parameters=mux->parameters;
     lc->setPort(ID::A,left[0]);lc->setPort(ID::B,lb);lc->setPort(ID::S,sel);lc->setPort(ID::Y,lm);
     mux->setPort(ID::A,right[0]);mux->setPort(ID::B,rb);mux->setPort(ID::Y,rm);
     auto shared=m->addCell(NEW_ID,ops[0]->type);shared->parameters=ops[0]->parameters;
     shared->setPort(ID::A,lm);shared->setPort(ID::B,rm);shared->setPort(ID::Y,y);
     for(auto old:ops)m->remove(old);changed++;removed+=n*w;round_changed=true;
     break;
    }
    SigSpec common;for(auto candidate:{left[0],right[0]}){bool all=true;for(int k=0;k<GetSize(ops);k++)if(left[k]!=candidate&&right[k]!=candidate)all=false;if(all){common=candidate;break;}}
    if(common.empty())continue;
    std::vector<SigSpec> varying;for(int k=0;k<GetSize(ops);k++)varying.push_back(left[k]==common?right[k]:left[k]);
    SigSpec nb;for(int k=1;k<GetSize(varying);k++)nb.append(varying[k]);auto tmp=m->addWire(NEW_ID,w);mux->setPort(ID::A,varying[0]);mux->setPort(ID::B,nb);mux->setPort(ID::Y,SigSpec(tmp));auto op=m->addCell(NEW_ID,ID($xor));op->parameters=ops[0]->parameters;op->setPort(ID::A,SigSpec(tmp));op->setPort(ID::B,common);op->setPort(ID::Y,y);
    for(auto old:ops)m->remove(old);changed++;removed+=n*w;round_changed=true;
    // Rebuild maps after each successful rewrite; never use stale connectivity.
    break;
   }
   if (!round_changed) break;
   }
  }
  log("PMUX_XOR_FACTOR: matched=%d gross_removed_xor_bits=%d\n",changed,removed);
 }
} PmuxXorFactorPass;
