module miter (
  input  [ 31:0] \__pi_data__31_0 ,
  input  [  7:0] \__pi_data__47_40 ,
  input  [ 39:0] \__pi_data__95_56 ,
  input  [  7:0] \__pi_data__111_104 ,
  input  [ 39:0] \__pi_data__159_120 ,
  input  [  7:0] \__pi_data__175_168 ,
  input  [  7:0] \__pi_data__191_184 ,
  input  [  5:0] \__pi_sel ,
`ifdef DIRECT_CROSS_POINTS
`else
`endif
  output [ 15:0] \__po_mixed__gold ,
  output [ 95:0] \__po_y__gold ,
  output [ 15:0] \__po_mixed__gate ,
  output [ 95:0] \__po_y__gate
);
  \gold.top.mixed gold (
    .\__pi_data__31_0 (\__pi_data__31_0 ),
    .\__pi_data__47_40 (\__pi_data__47_40 ),
    .\__pi_data__95_56 (\__pi_data__95_56 ),
    .\__pi_data__111_104 (\__pi_data__111_104 ),
    .\__pi_data__159_120 (\__pi_data__159_120 ),
    .\__pi_data__175_168 (\__pi_data__175_168 ),
    .\__pi_data__191_184 (\__pi_data__191_184 ),
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
    .\__pi_data__191_184 (\__pi_data__191_184 ),
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
  miter_def_prop #(8, "assume") \__pi_data__191_184__assume (\__pi_data__191_184 );
  miter_def_prop #(6, "assume") \__pi_sel__assume (\__pi_sel );
`endif
`ifndef DIRECT_CROSS_POINTS
`endif
`ifdef CHECK_MATCH_POINTS
`endif
`ifdef CHECK_OUTPUTS
  miter_cmp_prop #(16, "assert") \__po_mixed__assert (\__po_mixed__gold , \__po_mixed__gate );
  miter_cmp_prop #(96, "assert") \__po_y__assert (\__po_y__gold , \__po_y__gate );
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
  miter_def_prop #(96, "cover") \__po_y__gold_cover (\__po_y__gold );
`endif
`ifdef COVER_DEF_GATE_OUTPUTS
  miter_def_prop #(16, "cover") \__po_mixed__gate_cover (\__po_mixed__gate );
  miter_def_prop #(96, "cover") \__po_y__gate_cover (\__po_y__gate );
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
  input  [  7:0] \__pi_data__191_184 ,
  input  [  5:0] \__pi_sel ,
  output [ 15:0] \__po_mixed ,
  output [ 95:0] \__po_y
);
endmodule
module \gate.top.mixed (
  input  [ 31:0] \__pi_data__31_0 ,
  input  [  7:0] \__pi_data__47_40 ,
  input  [ 39:0] \__pi_data__95_56 ,
  input  [  7:0] \__pi_data__111_104 ,
  input  [ 39:0] \__pi_data__159_120 ,
  input  [  7:0] \__pi_data__175_168 ,
  input  [  7:0] \__pi_data__191_184 ,
  input  [  5:0] \__pi_sel ,
  output [ 15:0] \__po_mixed ,
  output [ 95:0] \__po_y
);
endmodule
