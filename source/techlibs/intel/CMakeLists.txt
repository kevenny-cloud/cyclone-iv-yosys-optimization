yosys_pass(synth_intel
	synth_intel.cc
	REQUIRES
		pmux_opt
		abc
		abc9
		autoname
		blackbox
		check
		clean
		deminout
		dfflegalize
		flatten
		fsm
		hierarchy
		iopadmap
		memory
		memory_bram
		memory_map
		opt
		opt_clean
		opt_expr
		peepopt
		proc
		read_verilog
		setundef
		stat
		synth
		techmap
		tribuf
		wreduce
		write_blif
		write_verilog
	DATA_DIR
		intel
	DATA_FILES
		common/m9k_bb.v
		common/altpll_bb.v
		common/brams_m9k.txt
		common/brams_map_m9k.v
		common/ff_map.v
		common/ff_map_sync.v

		max10/cells_sim.v
		max10/cells_map.v
		max10/dsp_map.v

		cyclone10lp/cells_sim.v
		cyclone10lp/cells_map.v

		cycloneiv/cells_sim.v
		cycloneiv/cells_map.v

		cycloneive/cells_sim.v
		cycloneive/cells_map.v
)


yosys_pass(pmux_opt pmux_opt.cc pmux_branch_const.cc pmux_decode_tree.cc rom_share.cc lut_resub.cc REQUIRES zinit techmap opt_share)
