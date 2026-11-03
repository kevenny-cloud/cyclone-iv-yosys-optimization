// Conservative selection wrapper around the upstream common-operand optimizer.
// No testcase identifiers, parameters, or measured answers enter selection.
struct StructuralSharePass : Pass {
    StructuralSharePass() : Pass("structural_share", "bounded common-operand sharing with protected boundaries") {}
    void help() override {
        log("\n    structural_share [selection]\n\n");
        log("Run upstream opt_share on fully selected, process-free modules only.\n");
        log("Keep/init aliases, kept cells and blackboxes are protected boundaries.\n");
        log("Operators with fewer than four output bits are left to local LUT cuts.\n");
        log("Preflight is limited to 1,000,000 wire/connection bits per invocation.\n");
        log("This preserves Boolean functionality; mapped-area improvement remains\n");
        log("a heuristic and is checked in the independent acceptance suite.\n\n");
    }
    void execute(std::vector<std::string> args, Design *design) override {
        extra_args(args, 1, design);
        log_header(design, "Executing STRUCTURAL_SHARE pass.\n");
        int64_t remaining = 1000000;
        RTLIL::Selection safe(false, false, design);
        int considered=0, excluded=0, modules=0;
        bool budget_exhausted=false;
        for (auto module : design->selected_modules()) {
            if (!design->selected_whole_module(module) || module->has_processes() ||
                module->get_blackbox_attribute() || module->get_bool_attribute(ID::keep)) continue;
            int ops=0, muxes=0;
            bool exhausted=false;
            auto charge=[&](int64_t n) {
                if (n > remaining) { exhausted=true; budget_exhausted=true; return; }
                remaining-=n;
            };
            for (auto cell : module->cells()) {
                charge(1);
                if (exhausted) break;
                if (cell->type.in(ID($mux), ID($pmux))) ++muxes;
                if (cell->type.in(ID($add),ID($sub),ID($and),ID($or),ID($xor),ID($xnor),ID($mul),ID($shl),ID($shr),ID($sshl),ID($sshr)) &&
                    cell->hasPort(ID::Y) && cell->getPort(ID::Y).size()>=4) ++ops;
            }
            if (exhausted) break;
            if (ops<2 || !muxes) continue;
            for (auto wire : module->wires()) { charge(1+int64_t(wire->width)*2); if (exhausted) break; }
            if (exhausted) break;
            for (auto &conn : module->connections()) {
                charge(1+int64_t(conn.first.size())+conn.second.size());
                if (exhausted) break;
            }
            if (exhausted) break;
            for (auto cell : module->cells()) {
                for (auto &port : cell->connections()) { charge(1+int64_t(port.second.size())*2); if (exhausted) break; }
                if (exhausted) break;
            }
            if (exhausted) break;
            SigMap sm(module);
            pool<SigBit> protected_bits;
            for (auto wire : module->wires())
                if (wire->get_bool_attribute(ID::keep) || wire->attributes.count(ID::init))
                    for (auto bit : sm(SigSpec(wire))) if (bit.wire) protected_bits.insert(bit);
            int selected=0;
            for (auto cell : module->cells()) {
                bool protect=cell->get_bool_attribute(ID::keep) || (cell->hasPort(ID::Y) && cell->getPort(ID::Y).size()<4);
                for (auto &port : cell->connections())
                    for (auto bit : sm(port.second)) if (protected_bits.count(bit)) protect=true;
                ++considered;
                if (protect) { ++excluded; continue; }
                safe.select(module,cell);
                ++selected;
            }
            if (selected) ++modules;
        }
        if (modules) {
            design->selection_stack.push_back(safe);
            try { Pass::call(design,"opt_share"); }
            catch (...) { design->selection_stack.pop_back(); throw; }
            design->selection_stack.pop_back();
        }
        log("STRUCTURAL_SHARE: modules=%d considered=%d protected=%d work=%lld exhausted=%d\n",
            modules,considered,excluded,(long long)(1000000-remaining),int(budget_exhausted));
    }
} StructuralSharePass;
