module miter (
  input  [ 31:0] \__pi_data__31_0 ,
  input  [  7:0] \__pi_data__47_40 ,
  input  [ 39:0] \__pi_data__95_56 ,
  input  [  7:0] \__pi_data__111_104 ,
  input  [ 39:0] \__pi_data__159_120 ,
  input  [  7:0] \__pi_data__175_168 ,
  input  [ 39:0] \__pi_data__223_184 ,
  input  [  7:0] \__pi_data__239_232 ,
  input  [ 39:0] \__pi_data__287_248 ,
  input  [  7:0] \__pi_data__303_296 ,
  input  [ 39:0] \__pi_data__351_312 ,
  input  [  7:0] \__pi_data__367_360 ,
  input  [ 39:0] \__pi_data__415_376 ,
  input  [  7:0] \__pi_data__431_424 ,
  input  [ 39:0] \__pi_data__479_440 ,
  input  [  7:0] \__pi_data__495_488 ,
  input  [ 39:0] \__pi_data__543_504 ,
  input  [  7:0] \__pi_data__559_552 ,
  input  [ 39:0] \__pi_data__607_568 ,
  input  [  7:0] \__pi_data__623_616 ,
  input  [ 39:0] \__pi_data__671_632 ,
  input  [  7:0] \__pi_data__687_680 ,
  input  [ 39:0] \__pi_data__735_696 ,
  input  [  7:0] \__pi_data__751_744 ,
  input  [ 39:0] \__pi_data__799_760 ,
  input  [  7:0] \__pi_data__815_808 ,
  input  [ 39:0] \__pi_data__863_824 ,
  input  [  7:0] \__pi_data__879_872 ,
  input  [ 39:0] \__pi_data__927_888 ,
  input  [ 74:0] \__pi_sel ,
`ifdef DIRECT_CROSS_POINTS
`else
`endif
  output [ 15:0] \__po_mixed__gold ,
  output [463:0] \__po_y__gold ,
  output [ 15:0] \__po_mixed__gate ,
  output [463:0] \__po_y__gate
);
  \gold.top.mixed gold (
    .\__pi_data__31_0 (\__pi_data__31_0 ),
    .\__pi_data__47_40 (\__pi_data__47_40 ),
    .\__pi_data__95_56 (\__pi_data__95_56 ),
    .\__pi_data__111_104 (\__pi_data__111_104 ),
    .\__pi_data__159_120 (\__pi_data__159_120 ),
    .\__pi_data__175_168 (\__pi_data__175_168 ),
    .\__pi_data__223_184 (\__pi_data__223_184 ),
    .\__pi_data__239_232 (\__pi_data__239_232 ),
    .\__pi_data__287_248 (\__pi_data__287_248 ),
    .\__pi_data__303_296 (\__pi_data__303_296 ),
    .\__pi_data__351_312 (\__pi_data__351_312 ),
    .\__pi_data__367_360 (\__pi_data__367_360 ),
    .\__pi_data__415_376 (\__pi_data__415_376 ),
    .\__pi_data__431_424 (\__pi_data__431_424 ),
    .\__pi_data__479_440 (\__pi_data__479_440 ),
    .\__pi_data__495_488 (\__pi_data__495_488 ),
    .\__pi_data__543_504 (\__pi_data__543_504 ),
    .\__pi_data__559_552 (\__pi_data__559_552 ),
    .\__pi_data__607_568 (\__pi_data__607_568 ),
    .\__pi_data__623_616 (\__pi_data__623_616 ),
    .\__pi_data__671_632 (\__pi_data__671_632 ),
    .\__pi_data__687_680 (\__pi_data__687_680 ),
    .\__pi_data__735_696 (\__pi_data__735_696 ),
    .\__pi_data__751_744 (\__pi_data__751_744 ),
    .\__pi_data__799_760 (\__pi_data__799_760 ),
    .\__pi_data__815_808 (\__pi_data__815_808 ),
    .\__pi_data__863_824 (\__pi_data__863_824 ),
    .\__pi_data__879_872 (\__pi_data__879_872 ),
    .\__pi_data__927_888 (\__pi_data__927_888 ),
    .\__pi_sel (\__pi_sel ),
`ifdef DIRECT_CROSS_POINTS
`else
`endif
    .\__po_mixed (\__po_mixed__gold ),
    .\__po_y (\__po_y__gold )
  );
  \gate.top.mixed gate (
    .\__pi_data__31_0 (\__pi_data__31_0 ),
    .\__pi_data__47_40 (\__pi_data__47_40 ),
    .\__pi_data__95_56 (\__pi_data__95_56 ),
    .\__pi_data__111_104 (\__pi_data__111_104 ),
    .\__pi_data__159_120 (\__pi_data__159_120 ),
    .\__pi_data__175_168 (\__pi_data__175_168 ),
    .\__pi_data__223_184 (\__pi_data__223_184 ),
    .\__pi_data__239_232 (\__pi_data__239_232 ),
    .\__pi_data__287_248 (\__pi_data__287_248 ),
    .\__pi_data__303_296 (\__pi_data__303_296 ),
    .\__pi_data__351_312 (\__pi_data__351_312 ),
    .\__pi_data__367_360 (\__pi_data__367_360 ),
    .\__pi_data__415_376 (\__pi_data__415_376 ),
    .\__pi_data__431_424 (\__pi_data__431_424 ),
    .\__pi_data__479_440 (\__pi_data__479_440 ),
    .\__pi_data__495_488 (\__pi_data__495_488 ),
    .\__pi_data__543_504 (\__pi_data__543_504 ),
    .\__pi_data__559_552 (\__pi_data__559_552 ),
    .\__pi_data__607_568 (\__pi_data__607_568 ),
    .\__pi_data__623_616 (\__pi_data__623_616 ),
    .\__pi_data__671_632 (\__pi_data__671_632 ),
    .\__pi_data__687_680 (\__pi_data__687_680 ),
    .\__pi_data__735_696 (\__pi_data__735_696 ),
    .\__pi_data__751_744 (\__pi_data__751_744 ),
    .\__pi_data__799_760 (\__pi_data__799_760 ),
    .\__pi_data__815_808 (\__pi_data__815_808 ),
    .\__pi_data__863_824 (\__pi_data__863_824 ),
    .\__pi_data__879_872 (\__pi_data__879_872 ),
    .\__pi_data__927_888 (\__pi_data__927_888 ),
    .\__pi_sel (\__pi_sel ),
`ifdef DIRECT_CROSS_POINTS
`else
`endif
    .\__po_mixed (\__po_mixed__gate ),
    .\__po_y (\__po_y__gate )
  );
`ifdef ASSUME_DEFINED_INPUTS
  miter_def_prop #(32, "assume") \__pi_data__31_0__assume (\__pi_data__31_0 );
  miter_def_prop #(8, "assume") \__pi_data__47_40__assume (\__pi_data__47_40 );
  miter_def_prop #(40, "assume") \__pi_data__95_56__assume (\__pi_data__95_56 );
  miter_def_prop #(8, "assume") \__pi_data__111_104__assume (\__pi_data__111_104 );
  miter_def_prop #(40, "assume") \__pi_data__159_120__assume (\__pi_data__159_120 );
  miter_def_prop #(8, "assume") \__pi_data__175_168__assume (\__pi_data__175_168 );
  miter_def_prop #(40, "assume") \__pi_data__223_184__assume (\__pi_data__223_184 );
  miter_def_prop #(8, "assume") \__pi_data__239_232__assume (\__pi_data__239_232 );
  miter_def_prop #(40, "assume") \__pi_data__287_248__assume (\__pi_data__287_248 );
  miter_def_prop #(8, "assume") \__pi_data__303_296__assume (\__pi_data__303_296 );
  miter_def_prop #(40, "assume") \__pi_data__351_312__assume (\__pi_data__351_312 );
  miter_def_prop #(8, "assume") \__pi_data__367_360__assume (\__pi_data__367_360 );
  miter_def_prop #(40, "assume") \__pi_data__415_376__assume (\__pi_data__415_376 );
  miter_def_prop #(8, "assume") \__pi_data__431_424__assume (\__pi_data__431_424 );
  miter_def_prop #(40, "assume") \__pi_data__479_440__assume (\__pi_data__479_440 );
  miter_def_prop #(8, "assume") \__pi_data__495_488__assume (\__pi_data__495_488 );
  miter_def_prop #(40, "assume") \__pi_data__543_504__assume (\__pi_data__543_504 );
  miter_def_prop #(8, "assume") \__pi_data__559_552__assume (\__pi_data__559_552 );
  miter_def_prop #(40, "assume") \__pi_data__607_568__assume (\__pi_data__607_568 );
  miter_def_prop #(8, "assume") \__pi_data__623_616__assume (\__pi_data__623_616 );
  miter_def_prop #(40, "assume") \__pi_data__671_632__assume (\__pi_data__671_632 );
  miter_def_prop #(8, "assume") \__pi_data__687_680__assume (\__pi_data__687_680 );
  miter_def_prop #(40, "assume") \__pi_data__735_696__assume (\__pi_data__735_696 );
  miter_def_prop #(8, "assume") \__pi_data__751_744__assume (\__pi_data__751_744 );
  miter_def_prop #(40, "assume") \__pi_data__799_760__assume (\__pi_data__799_760 );
  miter_def_prop #(8, "assume") \__pi_data__815_808__assume (\__pi_data__815_808 );
  miter_def_prop #(40, "assume") \__pi_data__863_824__assume (\__pi_data__863_824 );
  miter_def_prop #(8, "assume") \__pi_data__879_872__assume (\__pi_data__879_872 );
  miter_def_prop #(40, "assume") \__pi_data__927_888__assume (\__pi_data__927_888 );
  miter_def_prop #(75, "assume") \__pi_sel__assume (\__pi_sel );
`endif
`ifndef DIRECT_CROSS_POINTS
`endif
`ifdef CHECK_MATCH_POINTS
`endif
`ifdef CHECK_OUTPUTS
  miter_cmp_prop #(16, "assert") \__po_mixed__assert (\__po_mixed__gold , \__po_mixed__gate );
  miter_cmp_prop #(464, "assert") \__po_y__assert (\__po_y__gold , \__po_y__gate );
`endif
`ifdef COVER_DEF_CROSS_POINTS
  `ifdef DIRECT_CROSS_POINTS
  `else
  `endif
`endif
`ifdef COVER_DEF_GOLD_MATCH_POINTS
`endif
`ifdef COVER_DEF_GATE_MATCH_POINTS
`endif
`ifdef COVER_DEF_GOLD_OUTPUTS
  miter_def_prop #(16, "cover") \__po_mixed__gold_cover (\__po_mixed__gold );
  miter_def_prop #(464, "cover") \__po_y__gold_cover (\__po_y__gold );
`endif
`ifdef COVER_DEF_GATE_OUTPUTS
  miter_def_prop #(16, "cover") \__po_mixed__gate_cover (\__po_mixed__gate );
  miter_def_prop #(464, "cover") \__po_y__gate_cover (\__po_y__gate );
`endif
endmodule
module miter_cmp_prop #(parameter WIDTH=1, parameter TYPE="assert") (input [WIDTH-1:0] in_gold, in_gate);
  reg okay;
  integer i;
  always @* begin
    okay = 1;
    for (i = 0; i < WIDTH; i = i+1)
      okay = okay && (in_gold[i] === 1'bx || in_gold[i] === in_gate[i]);
  end
  generate
    if (TYPE == "assert") always @* assert(okay);
    if (TYPE == "assume") always @* assume(okay);
    if (TYPE == "cover")  always @* cover(okay);
  endgenerate
endmodule
module miter_def_prop #(parameter WIDTH=1, parameter TYPE="assert") (input [WIDTH-1:0] in);
  wire okay = ^in !== 1'bx;
  generate
    if (TYPE == "assert") always @* assert(okay);
    if (TYPE == "assume") always @* assume(okay);
    if (TYPE == "cover")  always @* cover(okay);
  endgenerate
endmodule
module \gold.top.mixed (
  input  [ 31:0] \__pi_data__31_0 ,
  input  [  7:0] \__pi_data__47_40 ,
  input  [ 39:0] \__pi_data__95_56 ,
  input  [  7:0] \__pi_data__111_104 ,
  input  [ 39:0] \__pi_data__159_120 ,
  input  [  7:0] \__pi_data__175_168 ,
  input  [ 39:0] \__pi_data__223_184 ,
  input  [  7:0] \__pi_data__239_232 ,
  input  [ 39:0] \__pi_data__287_248 ,
  input  [  7:0] \__pi_data__303_296 ,
  input  [ 39:0] \__pi_data__351_312 ,
  input  [  7:0] \__pi_data__367_360 ,
  input  [ 39:0] \__pi_data__415_376 ,
  input  [  7:0] \__pi_data__431_424 ,
  input  [ 39:0] \__pi_data__479_440 ,
  input  [  7:0] \__pi_data__495_488 ,
  input  [ 39:0] \__pi_data__543_504 ,
  input  [  7:0] \__pi_data__559_552 ,
  input  [ 39:0] \__pi_data__607_568 ,
  input  [  7:0] \__pi_data__623_616 ,
  input  [ 39:0] \__pi_data__671_632 ,
  input  [  7:0] \__pi_data__687_680 ,
  input  [ 39:0] \__pi_data__735_696 ,
  input  [  7:0] \__pi_data__751_744 ,
  input  [ 39:0] \__pi_data__799_760 ,
  input  [  7:0] \__pi_data__815_808 ,
  input  [ 39:0] \__pi_data__863_824 ,
  input  [  7:0] \__pi_data__879_872 ,
  input  [ 39:0] \__pi_data__927_888 ,
  input  [ 74:0] \__pi_sel ,
  output [ 15:0] \__po_mixed ,
  output [463:0] \__po_y
);
endmodule
module \gate.top.mixed (
  input  [ 31:0] \__pi_data__31_0 ,
  input  [  7:0] \__pi_data__47_40 ,
  input  [ 39:0] \__pi_data__95_56 ,
  input  [  7:0] \__pi_data__111_104 ,
  input  [ 39:0] \__pi_data__159_120 ,
  input  [  7:0] \__pi_data__175_168 ,
  input  [ 39:0] \__pi_data__223_184 ,
  input  [  7:0] \__pi_data__239_232 ,
  input  [ 39:0] \__pi_data__287_248 ,
  input  [  7:0] \__pi_data__303_296 ,
  input  [ 39:0] \__pi_data__351_312 ,
  input  [  7:0] \__pi_data__367_360 ,
  input  [ 39:0] \__pi_data__415_376 ,
  input  [  7:0] \__pi_data__431_424 ,
  input  [ 39:0] \__pi_data__479_440 ,
  input  [  7:0] \__pi_data__495_488 ,
  input  [ 39:0] \__pi_data__543_504 ,
  input  [  7:0] \__pi_data__559_552 ,
  input  [ 39:0] \__pi_data__607_568 ,
  input  [  7:0] \__pi_data__623_616 ,
  input  [ 39:0] \__pi_data__671_632 ,
  input  [  7:0] \__pi_data__687_680 ,
  input  [ 39:0] \__pi_data__735_696 ,
  input  [  7:0] \__pi_data__751_744 ,
  input  [ 39:0] \__pi_data__799_760 ,
  input  [  7:0] \__pi_data__815_808 ,
  input  [ 39:0] \__pi_data__863_824 ,
  input  [  7:0] \__pi_data__879_872 ,
  input  [ 39:0] \__pi_data__927_888 ,
  input  [ 74:0] \__pi_sel ,
  output [ 15:0] \__po_mixed ,
  output [463:0] \__po_y
);
endmodule
