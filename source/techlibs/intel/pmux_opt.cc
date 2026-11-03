// Built-in contest entry. R3b selects a mapping policy for a pure priority chain.
#include "kernel/yosys.h"
#include "kernel/sigtools.h"
#include "kernel/celltypes.h"
#include "kernel/ffinit.h"
#include "kernel/ff.h"

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

#include "structural_share.h"
#include "sync_control_map.h"

#include "cycloneiv_powerup.h"
#include "pmux_xor_factor.h"

struct PmuxOptPass : Pass {
    PmuxOptPass() : Pass("pmux_opt", "bounded PMUX, ROM and priority-chain optimization") {}
    void help() override {
        log("\n    pmux_opt [-prepare|-apply|-cycloneiv-powerup] [selection]\n\n");
        log("-prepare only records validated matches; -apply reruns the accepted rules.\n");
        log("synth_intel invokes cycloneiv_powerup between those phases when required.\n");
        log("R1/R2 rewrite PMUX structures; ROMs use post-ABC LUT resubstitution.\n");
        log("R3b recognizes a pure 2..16-level, 1..4096-bit priority mux chain\n");
        log("with independent input controls/data and zero default, and requests\n");
        log("direct ABC9 cut mapping in synth_intel. The chain itself is unchanged.\n");
        log("Kept objects, shared internals, aliases and mixed logic are skipped.\n\n");
    }
    void execute(std::vector<std::string> args, Design *design) override {
        bool cycloneiv_powerup = false, prepare_only = false, apply_only = false;
        size_t argidx = 1;
        if (argidx < args.size() && args[argidx] == "-cycloneiv-powerup") {
            cycloneiv_powerup = true;
            argidx++;
        }
        if (argidx < args.size() && args[argidx] == "-prepare") {
            prepare_only = true; argidx++;
        } else if (argidx < args.size() && args[argidx] == "-apply") {
            apply_only = true; argidx++;
        }
        extra_args(args, argidx, design);
        log_header(design, "Executing PMUX_OPT built-in pass.\n");
        bool branch_match = false, tree_match = false, has_rom = false;
        if (apply_only) {
            if (!design->scratchpad_get_bool("pmux_opt.prepared"))
                log_cmd_error("pmux_opt -apply requires a preceding -prepare.\n");
            branch_match = design->scratchpad_get_bool("pmux_opt.branch_match");
            tree_match = design->scratchpad_get_bool("pmux_opt.tree_match");
            has_rom = design->scratchpad_get_bool("pmux_opt.has_rom");
            design->scratchpad_set_bool("pmux_opt.prepared", false);
        } else {
            design->scratchpad_set_bool("pmux_opt.branch_match", false);
            design->scratchpad_set_bool("pmux_opt.tree_match", false);
            design->scratchpad_set_bool("pmux_opt.has_rom", false);
            design->scratchpad_set_bool("pmux_opt.prepared", false);
            design->scratchpad_set_bool("pmux_opt.legacy_rewrite", false);
            design->scratchpad_set_bool("pmux_opt.priority_chain", false);
            // Necessary structural conditions only: avoid constructing expensive
            // driver maps for rules that cannot match. No testcase names are used.
            bool has_pmux = false, has_eq = false, has_complete_size = false;
            for (auto m : design->selected_modules()) {
                if (m->has_processes() || m->get_bool_attribute(ID::keep) || m->get_blackbox_attribute()) continue;
                for (auto &entry : m->memories)
                    if (m->selected(entry.second)) has_rom = true;
                for (auto c : m->cells()) {
                    if (c->type == ID($eq)) has_eq = true;
                    if (c->type != ID($pmux) || !m->selected(c)) continue;
                    has_pmux = true;
                    if (c->hasPort(ID::S)) {
                        int n = GetSize(c->getPort(ID::S));
                        if (n >= 2 && n <= 32 && (n & (n - 1)) == 0) has_complete_size = true;
                    }
                }
            }
            // Probe actual opportunities before target-specific initialization.
            // A PMUX with no accepted R1/R2 plan must keep the baseline state flow.
            if (has_pmux && has_eq) {
                Pass::call(design, "pmux_branch_const -dry-run");
                branch_match=design->scratchpad_get_bool("pmux_branch_const.matches");
            }
            if (has_complete_size) {
                Pass::call(design, "pmux_decode_tree -dry-run -vector -leaf 3 -keep-decode -rank 1");
                tree_match=design->scratchpad_get_bool("pmux_decode_tree.matches");
            }
    
            design->scratchpad_set_bool("pmux_opt.branch_match", branch_match);
            design->scratchpad_set_bool("pmux_opt.tree_match", tree_match);
            design->scratchpad_set_bool("pmux_opt.has_rom", has_rom);
            design->scratchpad_set_bool("pmux_opt.prepared", prepare_only);
            if (prepare_only) return;
        }
        if ((branch_match || tree_match) && cycloneiv_powerup && design->full_selection())
            Pass::call(design, "cycloneiv_powerup");
        if (branch_match) Pass::call(design, "pmux_branch_const");
        if (tree_match) Pass::call(design, "pmux_decode_tree -vector -leaf 3 -keep-decode -rank 1");
        design->scratchpad_set_bool("pmux_opt.legacy_rewrite", branch_match || tree_match);
        if (has_rom) Pass::call(design, "rom_share -leaf 4");
    }
} PmuxOptPass;

PRIVATE_NAMESPACE_END
