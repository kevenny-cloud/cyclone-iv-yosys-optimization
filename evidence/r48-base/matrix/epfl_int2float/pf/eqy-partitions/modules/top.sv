module miter (
  input  [  0:0] \__pi_B[0] ,
  input  [  0:0] \__pi_B[10] ,
  input  [  0:0] \__pi_B[1] ,
  input  [  0:0] \__pi_B[2] ,
  input  [  0:0] \__pi_B[3] ,
  input  [  0:0] \__pi_B[4] ,
  input  [  0:0] \__pi_B[5] ,
  input  [  0:0] \__pi_B[6] ,
  input  [  0:0] \__pi_B[7] ,
  input  [  0:0] \__pi_B[8] ,
  input  [  0:0] \__pi_B[9] ,
`ifdef DIRECT_CROSS_POINTS
`else
`endif
  output [  0:0] \__po_E[0]__gold ,
  output [  0:0] \__po_E[1]__gold ,
  output [  0:0] \__po_E[2]__gold ,
  output [  0:0] \__po_M[0]__gold ,
  output [  0:0] \__po_M[1]__gold ,
  output [  0:0] \__po_M[2]__gold ,
  output [  0:0] \__po_M[3]__gold ,
  output [  0:0] \__po_E[0]__gate ,
  output [  0:0] \__po_E[1]__gate ,
  output [  0:0] \__po_E[2]__gate ,
  output [  0:0] \__po_M[0]__gate ,
  output [  0:0] \__po_M[1]__gate ,
  output [  0:0] \__po_M[2]__gate ,
  output [  0:0] \__po_M[3]__gate
);
  \gold.top gold (
    .\__pi_B[0] (\__pi_B[0] ),
    .\__pi_B[10] (\__pi_B[10] ),
    .\__pi_B[1] (\__pi_B[1] ),
    .\__pi_B[2] (\__pi_B[2] ),
    .\__pi_B[3] (\__pi_B[3] ),
    .\__pi_B[4] (\__pi_B[4] ),
    .\__pi_B[5] (\__pi_B[5] ),
    .\__pi_B[6] (\__pi_B[6] ),
    .\__pi_B[7] (\__pi_B[7] ),
    .\__pi_B[8] (\__pi_B[8] ),
    .\__pi_B[9] (\__pi_B[9] ),
`ifdef DIRECT_CROSS_POINTS
`else
`endif
    .\__po_E[0] (\__po_E[0]__gold ),
    .\__po_E[1] (\__po_E[1]__gold ),
    .\__po_E[2] (\__po_E[2]__gold ),
    .\__po_M[0] (\__po_M[0]__gold ),
    .\__po_M[1] (\__po_M[1]__gold ),
    .\__po_M[2] (\__po_M[2]__gold ),
    .\__po_M[3] (\__po_M[3]__gold )
  );
  \gate.top gate (
    .\__pi_B[0] (\__pi_B[0] ),
    .\__pi_B[10] (\__pi_B[10] ),
    .\__pi_B[1] (\__pi_B[1] ),
    .\__pi_B[2] (\__pi_B[2] ),
    .\__pi_B[3] (\__pi_B[3] ),
    .\__pi_B[4] (\__pi_B[4] ),
    .\__pi_B[5] (\__pi_B[5] ),
    .\__pi_B[6] (\__pi_B[6] ),
    .\__pi_B[7] (\__pi_B[7] ),
    .\__pi_B[8] (\__pi_B[8] ),
    .\__pi_B[9] (\__pi_B[9] ),
`ifdef DIRECT_CROSS_POINTS
`else
`endif
    .\__po_E[0] (\__po_E[0]__gate ),
    .\__po_E[1] (\__po_E[1]__gate ),
    .\__po_E[2] (\__po_E[2]__gate ),
    .\__po_M[0] (\__po_M[0]__gate ),
    .\__po_M[1] (\__po_M[1]__gate ),
    .\__po_M[2] (\__po_M[2]__gate ),
    .\__po_M[3] (\__po_M[3]__gate )
  );
`ifdef ASSUME_DEFINED_INPUTS
  miter_def_prop #(1, "assume") \__pi_B[0]__assume (\__pi_B[0] );
  miter_def_prop #(1, "assume") \__pi_B[10]__assume (\__pi_B[10] );
  miter_def_prop #(1, "assume") \__pi_B[1]__assume (\__pi_B[1] );
  miter_def_prop #(1, "assume") \__pi_B[2]__assume (\__pi_B[2] );
  miter_def_prop #(1, "assume") \__pi_B[3]__assume (\__pi_B[3] );
  miter_def_prop #(1, "assume") \__pi_B[4]__assume (\__pi_B[4] );
  miter_def_prop #(1, "assume") \__pi_B[5]__assume (\__pi_B[5] );
  miter_def_prop #(1, "assume") \__pi_B[6]__assume (\__pi_B[6] );
  miter_def_prop #(1, "assume") \__pi_B[7]__assume (\__pi_B[7] );
  miter_def_prop #(1, "assume") \__pi_B[8]__assume (\__pi_B[8] );
  miter_def_prop #(1, "assume") \__pi_B[9]__assume (\__pi_B[9] );
`endif
`ifndef DIRECT_CROSS_POINTS
`endif
`ifdef CHECK_MATCH_POINTS
`endif
`ifdef CHECK_OUTPUTS
  miter_cmp_prop #(1, "assert") \__po_E[0]__assert (\__po_E[0]__gold , \__po_E[0]__gate );
  miter_cmp_prop #(1, "assert") \__po_E[1]__assert (\__po_E[1]__gold , \__po_E[1]__gate );
  miter_cmp_prop #(1, "assert") \__po_E[2]__assert (\__po_E[2]__gold , \__po_E[2]__gate );
  miter_cmp_prop #(1, "assert") \__po_M[0]__assert (\__po_M[0]__gold , \__po_M[0]__gate );
  miter_cmp_prop #(1, "assert") \__po_M[1]__assert (\__po_M[1]__gold , \__po_M[1]__gate );
  miter_cmp_prop #(1, "assert") \__po_M[2]__assert (\__po_M[2]__gold , \__po_M[2]__gate );
  miter_cmp_prop #(1, "assert") \__po_M[3]__assert (\__po_M[3]__gold , \__po_M[3]__gate );
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
  miter_def_prop #(1, "cover") \__po_E[0]__gold_cover (\__po_E[0]__gold );
  miter_def_prop #(1, "cover") \__po_E[1]__gold_cover (\__po_E[1]__gold );
  miter_def_prop #(1, "cover") \__po_E[2]__gold_cover (\__po_E[2]__gold );
  miter_def_prop #(1, "cover") \__po_M[0]__gold_cover (\__po_M[0]__gold );
  miter_def_prop #(1, "cover") \__po_M[1]__gold_cover (\__po_M[1]__gold );
  miter_def_prop #(1, "cover") \__po_M[2]__gold_cover (\__po_M[2]__gold );
  miter_def_prop #(1, "cover") \__po_M[3]__gold_cover (\__po_M[3]__gold );
`endif
`ifdef COVER_DEF_GATE_OUTPUTS
  miter_def_prop #(1, "cover") \__po_E[0]__gate_cover (\__po_E[0]__gate );
  miter_def_prop #(1, "cover") \__po_E[1]__gate_cover (\__po_E[1]__gate );
  miter_def_prop #(1, "cover") \__po_E[2]__gate_cover (\__po_E[2]__gate );
  miter_def_prop #(1, "cover") \__po_M[0]__gate_cover (\__po_M[0]__gate );
  miter_def_prop #(1, "cover") \__po_M[1]__gate_cover (\__po_M[1]__gate );
  miter_def_prop #(1, "cover") \__po_M[2]__gate_cover (\__po_M[2]__gate );
  miter_def_prop #(1, "cover") \__po_M[3]__gate_cover (\__po_M[3]__gate );
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
module \gold.top (
  input  [  0:0] \__pi_B[0] ,
  input  [  0:0] \__pi_B[10] ,
  input  [  0:0] \__pi_B[1] ,
  input  [  0:0] \__pi_B[2] ,
  input  [  0:0] \__pi_B[3] ,
  input  [  0:0] \__pi_B[4] ,
  input  [  0:0] \__pi_B[5] ,
  input  [  0:0] \__pi_B[6] ,
  input  [  0:0] \__pi_B[7] ,
  input  [  0:0] \__pi_B[8] ,
  input  [  0:0] \__pi_B[9] ,
  output [  0:0] \__po_E[0] ,
  output [  0:0] \__po_E[1] ,
  output [  0:0] \__po_E[2] ,
  output [  0:0] \__po_M[0] ,
  output [  0:0] \__po_M[1] ,
  output [  0:0] \__po_M[2] ,
  output [  0:0] \__po_M[3]
);
endmodule
module \gate.top (
  input  [  0:0] \__pi_B[0] ,
  input  [  0:0] \__pi_B[10] ,
  input  [  0:0] \__pi_B[1] ,
  input  [  0:0] \__pi_B[2] ,
  input  [  0:0] \__pi_B[3] ,
  input  [  0:0] \__pi_B[4] ,
  input  [  0:0] \__pi_B[5] ,
  input  [  0:0] \__pi_B[6] ,
  input  [  0:0] \__pi_B[7] ,
  input  [  0:0] \__pi_B[8] ,
  input  [  0:0] \__pi_B[9] ,
  output [  0:0] \__po_E[0] ,
  output [  0:0] \__po_E[1] ,
  output [  0:0] \__po_E[2] ,
  output [  0:0] \__po_M[0] ,
  output [  0:0] \__po_M[1] ,
  output [  0:0] \__po_M[2] ,
  output [  0:0] \__po_M[3]
);
endmodule
