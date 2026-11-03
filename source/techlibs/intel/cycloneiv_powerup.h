// Target initialization policy is a separate pass. PMUX planning only tells
// synth_intel whether its existing rewrite path requires that policy.
// Reference dfflegalize targets async clear-to-zero hardware. A logical
// async-set bit is complemented during legalization, so default-low physical
// power-up means logical one for that bit. Never equate logical default zero
// with physical default low; preserve all explicit init values and polarities.
static void resolve_unspecified_powerup(Design *design)
{
    int changed = 0, skipped = 0;
    for (auto module : design->selected_modules()) {
        if (module->get_bool_attribute(ID::keep) || module->get_blackbox_attribute()) continue;
        SigMap sigmap(module);
        FfInitVals initvals(&sigmap, module);
        for (auto cell : module->selected_cells()) {
            if (!cell->is_builtin_ff() || cell->get_bool_attribute(ID::keep)) continue;
            FfData ff(&initvals, cell);
            if (!ff.has_clk) continue;
            // Async load/set-reset need a separate legalization contract.
            if (ff.has_aload || ff.has_sr) { skipped += ff.width; continue; }
            for (int i = 0; i < ff.width; ++i) {
                auto bit=ff.sig_q[i];
                if (!bit.wire || !sigmap(bit).wire || bit.wire->get_bool_attribute(ID::keep)) continue;
                if (ff.val_init[i] != State::Sx) continue;
                State value = ff.has_arst ? ff.val_arst[i] : State::S0;
                if (value != State::S0 && value != State::S1) { ++skipped; continue; }
                initvals.set_init(bit, value);
                ++changed;
            }
        }
    }
    log("Cyclone IV power-up: resolved %d unspecified FF bits using canonical async-reset polarity; skipped %d unsupported bits; explicit initial states preserved.\n", changed, skipped);
}

struct CycloneivPowerupPass : Pass {
    CycloneivPowerupPass() : Pass("cycloneiv_powerup", "resolve unspecified Cyclone IV FF initial states") {}
    void help() override {
        log("\n    cycloneiv_powerup [selection]\n\n");
        log("Resolve unspecified clocked FF initial states using canonical async-reset\n");
        log("polarity. Preserve explicit initial states and kept objects.\n");
        log("Only a full design selection is supported; other selections are skipped.\n");
        log("synth_intel invokes this policy only for its accepted R1/R2 rewrite path.\n");
    }
    void execute(std::vector<std::string> args, Design *design) override {
        extra_args(args, 1, design);
        log_header(design, "Executing CYCLONEIV_POWERUP policy pass.\n");
        if (!design->full_selection()) {
            log("Cyclone IV power-up policy skipped: full design selection required.\n");
            return;
        }
        resolve_unspecified_powerup(design);
    }
} CycloneivPowerupPass;

