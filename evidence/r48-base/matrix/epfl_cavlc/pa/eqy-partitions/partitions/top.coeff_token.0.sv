module miter (
  input  [  0:0] \__pi_ctable[0] ,
  input  [  0:0] \__pi_ctable[1] ,
  input  [  0:0] \__pi_ctable[2] ,
  input  [  0:0] \__pi_totalcoeffs[0] ,
  input  [  0:0] \__pi_totalcoeffs[1] ,
  input  [  0:0] \__pi_totalcoeffs[2] ,
  input  [  0:0] \__pi_totalcoeffs[3] ,
  input  [  0:0] \__pi_totalcoeffs[4] ,
  input  [  0:0] \__pi_trailingones[0] ,
  input  [  0:0] \__pi_trailingones[1] ,
`ifdef DIRECT_CROSS_POINTS
`else
`endif
  output [  0:0] \__po_coeff_token[0]__gold ,
  output [  0:0] \__po_coeff_token[1]__gold ,
  output [  0:0] \__po_coeff_token[2]__gold ,
  output [  0:0] \__po_coeff_token[3]__gold ,
  output [  0:0] \__po_coeff_token[4]__gold ,
  output [  0:0] \__po_coeff_token[5]__gold ,
  output [  0:0] \__po_ctoken_len[0]__gold ,
  output [  0:0] \__po_ctoken_len[1]__gold ,
  output [  0:0] \__po_ctoken_len[2]__gold ,
  output [  0:0] \__po_ctoken_len[3]__gold ,
  output [  0:0] \__po_ctoken_len[4]__gold ,
  output [  0:0] \__po_coeff_token[0]__gate ,
  output [  0:0] \__po_coeff_token[1]__gate ,
  output [  0:0] \__po_coeff_token[2]__gate ,
  output [  0:0] \__po_coeff_token[3]__gate ,
  output [  0:0] \__po_coeff_token[4]__gate ,
  output [  0:0] \__po_coeff_token[5]__gate ,
  output [  0:0] \__po_ctoken_len[0]__gate ,
  output [  0:0] \__po_ctoken_len[1]__gate ,
  output [  0:0] \__po_ctoken_len[2]__gate ,
  output [  0:0] \__po_ctoken_len[3]__gate ,
  output [  0:0] \__po_ctoken_len[4]__gate
);
  \gold.top.coeff_token.0 gold (
    .\__pi_ctable[0] (\__pi_ctable[0] ),
    .\__pi_ctable[1] (\__pi_ctable[1] ),
    .\__pi_ctable[2] (\__pi_ctable[2] ),
    .\__pi_totalcoeffs[0] (\__pi_totalcoeffs[0] ),
    .\__pi_totalcoeffs[1] (\__pi_totalcoeffs[1] ),
    .\__pi_totalcoeffs[2] (\__pi_totalcoeffs[2] ),
    .\__pi_totalcoeffs[3] (\__pi_totalcoeffs[3] ),
    .\__pi_totalcoeffs[4] (\__pi_totalcoeffs[4] ),
    .\__pi_trailingones[0] (\__pi_trailingones[0] ),
    .\__pi_trailingones[1] (\__pi_trailingones[1] ),
`ifdef DIRECT_CROSS_POINTS
`else
`endif
    .\__po_coeff_token[0] (\__po_coeff_token[0]__gold ),
    .\__po_coeff_token[1] (\__po_coeff_token[1]__gold ),
    .\__po_coeff_token[2] (\__po_coeff_token[2]__gold ),
    .\__po_coeff_token[3] (\__po_coeff_token[3]__gold ),
    .\__po_coeff_token[4] (\__po_coeff_token[4]__gold ),
    .\__po_coeff_token[5] (\__po_coeff_token[5]__gold ),
    .\__po_ctoken_len[0] (\__po_ctoken_len[0]__gold ),
    .\__po_ctoken_len[1] (\__po_ctoken_len[1]__gold ),
    .\__po_ctoken_len[2] (\__po_ctoken_len[2]__gold ),
    .\__po_ctoken_len[3] (\__po_ctoken_len[3]__gold ),
    .\__po_ctoken_len[4] (\__po_ctoken_len[4]__gold )
  );
  \gate.top.coeff_token.0 gate (
    .\__pi_ctable[0] (\__pi_ctable[0] ),
    .\__pi_ctable[1] (\__pi_ctable[1] ),
    .\__pi_ctable[2] (\__pi_ctable[2] ),
    .\__pi_totalcoeffs[0] (\__pi_totalcoeffs[0] ),
    .\__pi_totalcoeffs[1] (\__pi_totalcoeffs[1] ),
    .\__pi_totalcoeffs[2] (\__pi_totalcoeffs[2] ),
    .\__pi_totalcoeffs[3] (\__pi_totalcoeffs[3] ),
    .\__pi_totalcoeffs[4] (\__pi_totalcoeffs[4] ),
    .\__pi_trailingones[0] (\__pi_trailingones[0] ),
    .\__pi_trailingones[1] (\__pi_trailingones[1] ),
`ifdef DIRECT_CROSS_POINTS
`else
`endif
    .\__po_coeff_token[0] (\__po_coeff_token[0]__gate ),
    .\__po_coeff_token[1] (\__po_coeff_token[1]__gate ),
    .\__po_coeff_token[2] (\__po_coeff_token[2]__gate ),
    .\__po_coeff_token[3] (\__po_coeff_token[3]__gate ),
    .\__po_coeff_token[4] (\__po_coeff_token[4]__gate ),
    .\__po_coeff_token[5] (\__po_coeff_token[5]__gate ),
    .\__po_ctoken_len[0] (\__po_ctoken_len[0]__gate ),
    .\__po_ctoken_len[1] (\__po_ctoken_len[1]__gate ),
    .\__po_ctoken_len[2] (\__po_ctoken_len[2]__gate ),
    .\__po_ctoken_len[3] (\__po_ctoken_len[3]__gate ),
    .\__po_ctoken_len[4] (\__po_ctoken_len[4]__gate )
  );
`ifdef ASSUME_DEFINED_INPUTS
  miter_def_prop #(1, "assume") \__pi_ctable[0]__assume (\__pi_ctable[0] );
  miter_def_prop #(1, "assume") \__pi_ctable[1]__assume (\__pi_ctable[1] );
  miter_def_prop #(1, "assume") \__pi_ctable[2]__assume (\__pi_ctable[2] );
  miter_def_prop #(1, "assume") \__pi_totalcoeffs[0]__assume (\__pi_totalcoeffs[0] );
  miter_def_prop #(1, "assume") \__pi_totalcoeffs[1]__assume (\__pi_totalcoeffs[1] );
  miter_def_prop #(1, "assume") \__pi_totalcoeffs[2]__assume (\__pi_totalcoeffs[2] );
  miter_def_prop #(1, "assume") \__pi_totalcoeffs[3]__assume (\__pi_totalcoeffs[3] );
  miter_def_prop #(1, "assume") \__pi_totalcoeffs[4]__assume (\__pi_totalcoeffs[4] );
  miter_def_prop #(1, "assume") \__pi_trailingones[0]__assume (\__pi_trailingones[0] );
  miter_def_prop #(1, "assume") \__pi_trailingones[1]__assume (\__pi_trailingones[1] );
`endif
`ifndef DIRECT_CROSS_POINTS
`endif
`ifdef CHECK_MATCH_POINTS
`endif
`ifdef CHECK_OUTPUTS
  miter_cmp_prop #(1, "assert") \__po_coeff_token[0]__assert (\__po_coeff_token[0]__gold , \__po_coeff_token[0]__gate );
  miter_cmp_prop #(1, "assert") \__po_coeff_token[1]__assert (\__po_coeff_token[1]__gold , \__po_coeff_token[1]__gate );
  miter_cmp_prop #(1, "assert") \__po_coeff_token[2]__assert (\__po_coeff_token[2]__gold , \__po_coeff_token[2]__gate );
  miter_cmp_prop #(1, "assert") \__po_coeff_token[3]__assert (\__po_coeff_token[3]__gold , \__po_coeff_token[3]__gate );
  miter_cmp_prop #(1, "assert") \__po_coeff_token[4]__assert (\__po_coeff_token[4]__gold , \__po_coeff_token[4]__gate );
  miter_cmp_prop #(1, "assert") \__po_coeff_token[5]__assert (\__po_coeff_token[5]__gold , \__po_coeff_token[5]__gate );
  miter_cmp_prop #(1, "assert") \__po_ctoken_len[0]__assert (\__po_ctoken_len[0]__gold , \__po_ctoken_len[0]__gate );
  miter_cmp_prop #(1, "assert") \__po_ctoken_len[1]__assert (\__po_ctoken_len[1]__gold , \__po_ctoken_len[1]__gate );
  miter_cmp_prop #(1, "assert") \__po_ctoken_len[2]__assert (\__po_ctoken_len[2]__gold , \__po_ctoken_len[2]__gate );
  miter_cmp_prop #(1, "assert") \__po_ctoken_len[3]__assert (\__po_ctoken_len[3]__gold , \__po_ctoken_len[3]__gate );
  miter_cmp_prop #(1, "assert") \__po_ctoken_len[4]__assert (\__po_ctoken_len[4]__gold , \__po_ctoken_len[4]__gate );
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
  miter_def_prop #(1, "cover") \__po_coeff_token[0]__gold_cover (\__po_coeff_token[0]__gold );
  miter_def_prop #(1, "cover") \__po_coeff_token[1]__gold_cover (\__po_coeff_token[1]__gold );
  miter_def_prop #(1, "cover") \__po_coeff_token[2]__gold_cover (\__po_coeff_token[2]__gold );
  miter_def_prop #(1, "cover") \__po_coeff_token[3]__gold_cover (\__po_coeff_token[3]__gold );
  miter_def_prop #(1, "cover") \__po_coeff_token[4]__gold_cover (\__po_coeff_token[4]__gold );
  miter_def_prop #(1, "cover") \__po_coeff_token[5]__gold_cover (\__po_coeff_token[5]__gold );
  miter_def_prop #(1, "cover") \__po_ctoken_len[0]__gold_cover (\__po_ctoken_len[0]__gold );
  miter_def_prop #(1, "cover") \__po_ctoken_len[1]__gold_cover (\__po_ctoken_len[1]__gold );
  miter_def_prop #(1, "cover") \__po_ctoken_len[2]__gold_cover (\__po_ctoken_len[2]__gold );
  miter_def_prop #(1, "cover") \__po_ctoken_len[3]__gold_cover (\__po_ctoken_len[3]__gold );
  miter_def_prop #(1, "cover") \__po_ctoken_len[4]__gold_cover (\__po_ctoken_len[4]__gold );
`endif
`ifdef COVER_DEF_GATE_OUTPUTS
  miter_def_prop #(1, "cover") \__po_coeff_token[0]__gate_cover (\__po_coeff_token[0]__gate );
  miter_def_prop #(1, "cover") \__po_coeff_token[1]__gate_cover (\__po_coeff_token[1]__gate );
  miter_def_prop #(1, "cover") \__po_coeff_token[2]__gate_cover (\__po_coeff_token[2]__gate );
  miter_def_prop #(1, "cover") \__po_coeff_token[3]__gate_cover (\__po_coeff_token[3]__gate );
  miter_def_prop #(1, "cover") \__po_coeff_token[4]__gate_cover (\__po_coeff_token[4]__gate );
  miter_def_prop #(1, "cover") \__po_coeff_token[5]__gate_cover (\__po_coeff_token[5]__gate );
  miter_def_prop #(1, "cover") \__po_ctoken_len[0]__gate_cover (\__po_ctoken_len[0]__gate );
  miter_def_prop #(1, "cover") \__po_ctoken_len[1]__gate_cover (\__po_ctoken_len[1]__gate );
  miter_def_prop #(1, "cover") \__po_ctoken_len[2]__gate_cover (\__po_ctoken_len[2]__gate );
  miter_def_prop #(1, "cover") \__po_ctoken_len[3]__gate_cover (\__po_ctoken_len[3]__gate );
  miter_def_prop #(1, "cover") \__po_ctoken_len[4]__gate_cover (\__po_ctoken_len[4]__gate );
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
module \gold.top.coeff_token.0 (
  input  [  0:0] \__pi_ctable[0] ,
  input  [  0:0] \__pi_ctable[1] ,
  input  [  0:0] \__pi_ctable[2] ,
  input  [  0:0] \__pi_totalcoeffs[0] ,
  input  [  0:0] \__pi_totalcoeffs[1] ,
  input  [  0:0] \__pi_totalcoeffs[2] ,
  input  [  0:0] \__pi_totalcoeffs[3] ,
  input  [  0:0] \__pi_totalcoeffs[4] ,
  input  [  0:0] \__pi_trailingones[0] ,
  input  [  0:0] \__pi_trailingones[1] ,
  output [  0:0] \__po_coeff_token[0] ,
  output [  0:0] \__po_coeff_token[1] ,
  output [  0:0] \__po_coeff_token[2] ,
  output [  0:0] \__po_coeff_token[3] ,
  output [  0:0] \__po_coeff_token[4] ,
  output [  0:0] \__po_coeff_token[5] ,
  output [  0:0] \__po_ctoken_len[0] ,
  output [  0:0] \__po_ctoken_len[1] ,
  output [  0:0] \__po_ctoken_len[2] ,
  output [  0:0] \__po_ctoken_len[3] ,
  output [  0:0] \__po_ctoken_len[4]
);
endmodule
module \gate.top.coeff_token.0 (
  input  [  0:0] \__pi_ctable[0] ,
  input  [  0:0] \__pi_ctable[1] ,
  input  [  0:0] \__pi_ctable[2] ,
  input  [  0:0] \__pi_totalcoeffs[0] ,
  input  [  0:0] \__pi_totalcoeffs[1] ,
  input  [  0:0] \__pi_totalcoeffs[2] ,
  input  [  0:0] \__pi_totalcoeffs[3] ,
  input  [  0:0] \__pi_totalcoeffs[4] ,
  input  [  0:0] \__pi_trailingones[0] ,
  input  [  0:0] \__pi_trailingones[1] ,
  output [  0:0] \__po_coeff_token[0] ,
  output [  0:0] \__po_coeff_token[1] ,
  output [  0:0] \__po_coeff_token[2] ,
  output [  0:0] \__po_coeff_token[3] ,
  output [  0:0] \__po_coeff_token[4] ,
  output [  0:0] \__po_coeff_token[5] ,
  output [  0:0] \__po_ctoken_len[0] ,
  output [  0:0] \__po_ctoken_len[1] ,
  output [  0:0] \__po_ctoken_len[2] ,
  output [  0:0] \__po_ctoken_len[3] ,
  output [  0:0] \__po_ctoken_len[4]
);
endmodule
