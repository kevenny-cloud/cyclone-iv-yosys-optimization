module miter (
  input  [  0:0] \__pi_op_ext[0] ,
  input  [  0:0] \__pi_op_ext[1] ,
  input  [  0:0] \__pi_opcode[0] ,
  input  [  0:0] \__pi_opcode[1] ,
  input  [  0:0] \__pi_opcode[2] ,
  input  [  0:0] \__pi_opcode[3] ,
  input  [  0:0] \__pi_opcode[4] ,
`ifdef DIRECT_CROSS_POINTS
`else
`endif
  output [  0:0] \__po_Cin__gold ,
  output [  0:0] \__po_alu_op[0]__gold ,
  output [  0:0] \__po_alu_op[1]__gold ,
  output [  0:0] \__po_alu_op[2]__gold ,
  output [  0:0] \__po_alu_op_ext[0]__gold ,
  output [  0:0] \__po_alu_op_ext[1]__gold ,
  output [  0:0] \__po_alu_op_ext[2]__gold ,
  output [  0:0] \__po_alu_op_ext[3]__gold ,
  output [  0:0] \__po_beqz__gold ,
  output [  0:0] \__po_bgez__gold ,
  output [  0:0] \__po_bltz__gold ,
  output [  0:0] \__po_bnez__gold ,
  output [  0:0] \__po_halt__gold ,
  output [  0:0] \__po_invA__gold ,
  output [  0:0] \__po_invB__gold ,
  output [  0:0] \__po_jump__gold ,
  output [  0:0] \__po_mem_write__gold ,
  output [  0:0] \__po_reg_write__gold ,
  output [  0:0] \__po_sel_alu_opB[0]__gold ,
  output [  0:0] \__po_sel_alu_opB[1]__gold ,
  output [  0:0] \__po_sel_pc_opA__gold ,
  output [  0:0] \__po_sel_pc_opB__gold ,
  output [  0:0] \__po_sel_reg_dst[0]__gold ,
  output [  0:0] \__po_sel_reg_dst[1]__gold ,
  output [  0:0] \__po_sel_wb__gold ,
  output [  0:0] \__po_sign__gold ,
  output [  0:0] \__po_Cin__gate ,
  output [  0:0] \__po_alu_op[0]__gate ,
  output [  0:0] \__po_alu_op[1]__gate ,
  output [  0:0] \__po_alu_op[2]__gate ,
  output [  0:0] \__po_alu_op_ext[0]__gate ,
  output [  0:0] \__po_alu_op_ext[1]__gate ,
  output [  0:0] \__po_alu_op_ext[2]__gate ,
  output [  0:0] \__po_alu_op_ext[3]__gate ,
  output [  0:0] \__po_beqz__gate ,
  output [  0:0] \__po_bgez__gate ,
  output [  0:0] \__po_bltz__gate ,
  output [  0:0] \__po_bnez__gate ,
  output [  0:0] \__po_halt__gate ,
  output [  0:0] \__po_invA__gate ,
  output [  0:0] \__po_invB__gate ,
  output [  0:0] \__po_jump__gate ,
  output [  0:0] \__po_mem_write__gate ,
  output [  0:0] \__po_reg_write__gate ,
  output [  0:0] \__po_sel_alu_opB[0]__gate ,
  output [  0:0] \__po_sel_alu_opB[1]__gate ,
  output [  0:0] \__po_sel_pc_opA__gate ,
  output [  0:0] \__po_sel_pc_opB__gate ,
  output [  0:0] \__po_sel_reg_dst[0]__gate ,
  output [  0:0] \__po_sel_reg_dst[1]__gate ,
  output [  0:0] \__po_sel_wb__gate ,
  output [  0:0] \__po_sign__gate
);
  \gold.top gold (
    .\__pi_op_ext[0] (\__pi_op_ext[0] ),
    .\__pi_op_ext[1] (\__pi_op_ext[1] ),
    .\__pi_opcode[0] (\__pi_opcode[0] ),
    .\__pi_opcode[1] (\__pi_opcode[1] ),
    .\__pi_opcode[2] (\__pi_opcode[2] ),
    .\__pi_opcode[3] (\__pi_opcode[3] ),
    .\__pi_opcode[4] (\__pi_opcode[4] ),
`ifdef DIRECT_CROSS_POINTS
`else
`endif
    .\__po_Cin (\__po_Cin__gold ),
    .\__po_alu_op[0] (\__po_alu_op[0]__gold ),
    .\__po_alu_op[1] (\__po_alu_op[1]__gold ),
    .\__po_alu_op[2] (\__po_alu_op[2]__gold ),
    .\__po_alu_op_ext[0] (\__po_alu_op_ext[0]__gold ),
    .\__po_alu_op_ext[1] (\__po_alu_op_ext[1]__gold ),
    .\__po_alu_op_ext[2] (\__po_alu_op_ext[2]__gold ),
    .\__po_alu_op_ext[3] (\__po_alu_op_ext[3]__gold ),
    .\__po_beqz (\__po_beqz__gold ),
    .\__po_bgez (\__po_bgez__gold ),
    .\__po_bltz (\__po_bltz__gold ),
    .\__po_bnez (\__po_bnez__gold ),
    .\__po_halt (\__po_halt__gold ),
    .\__po_invA (\__po_invA__gold ),
    .\__po_invB (\__po_invB__gold ),
    .\__po_jump (\__po_jump__gold ),
    .\__po_mem_write (\__po_mem_write__gold ),
    .\__po_reg_write (\__po_reg_write__gold ),
    .\__po_sel_alu_opB[0] (\__po_sel_alu_opB[0]__gold ),
    .\__po_sel_alu_opB[1] (\__po_sel_alu_opB[1]__gold ),
    .\__po_sel_pc_opA (\__po_sel_pc_opA__gold ),
    .\__po_sel_pc_opB (\__po_sel_pc_opB__gold ),
    .\__po_sel_reg_dst[0] (\__po_sel_reg_dst[0]__gold ),
    .\__po_sel_reg_dst[1] (\__po_sel_reg_dst[1]__gold ),
    .\__po_sel_wb (\__po_sel_wb__gold ),
    .\__po_sign (\__po_sign__gold )
  );
  \gate.top gate (
    .\__pi_op_ext[0] (\__pi_op_ext[0] ),
    .\__pi_op_ext[1] (\__pi_op_ext[1] ),
    .\__pi_opcode[0] (\__pi_opcode[0] ),
    .\__pi_opcode[1] (\__pi_opcode[1] ),
    .\__pi_opcode[2] (\__pi_opcode[2] ),
    .\__pi_opcode[3] (\__pi_opcode[3] ),
    .\__pi_opcode[4] (\__pi_opcode[4] ),
`ifdef DIRECT_CROSS_POINTS
`else
`endif
    .\__po_Cin (\__po_Cin__gate ),
    .\__po_alu_op[0] (\__po_alu_op[0]__gate ),
    .\__po_alu_op[1] (\__po_alu_op[1]__gate ),
    .\__po_alu_op[2] (\__po_alu_op[2]__gate ),
    .\__po_alu_op_ext[0] (\__po_alu_op_ext[0]__gate ),
    .\__po_alu_op_ext[1] (\__po_alu_op_ext[1]__gate ),
    .\__po_alu_op_ext[2] (\__po_alu_op_ext[2]__gate ),
    .\__po_alu_op_ext[3] (\__po_alu_op_ext[3]__gate ),
    .\__po_beqz (\__po_beqz__gate ),
    .\__po_bgez (\__po_bgez__gate ),
    .\__po_bltz (\__po_bltz__gate ),
    .\__po_bnez (\__po_bnez__gate ),
    .\__po_halt (\__po_halt__gate ),
    .\__po_invA (\__po_invA__gate ),
    .\__po_invB (\__po_invB__gate ),
    .\__po_jump (\__po_jump__gate ),
    .\__po_mem_write (\__po_mem_write__gate ),
    .\__po_reg_write (\__po_reg_write__gate ),
    .\__po_sel_alu_opB[0] (\__po_sel_alu_opB[0]__gate ),
    .\__po_sel_alu_opB[1] (\__po_sel_alu_opB[1]__gate ),
    .\__po_sel_pc_opA (\__po_sel_pc_opA__gate ),
    .\__po_sel_pc_opB (\__po_sel_pc_opB__gate ),
    .\__po_sel_reg_dst[0] (\__po_sel_reg_dst[0]__gate ),
    .\__po_sel_reg_dst[1] (\__po_sel_reg_dst[1]__gate ),
    .\__po_sel_wb (\__po_sel_wb__gate ),
    .\__po_sign (\__po_sign__gate )
  );
`ifdef ASSUME_DEFINED_INPUTS
  miter_def_prop #(1, "assume") \__pi_op_ext[0]__assume (\__pi_op_ext[0] );
  miter_def_prop #(1, "assume") \__pi_op_ext[1]__assume (\__pi_op_ext[1] );
  miter_def_prop #(1, "assume") \__pi_opcode[0]__assume (\__pi_opcode[0] );
  miter_def_prop #(1, "assume") \__pi_opcode[1]__assume (\__pi_opcode[1] );
  miter_def_prop #(1, "assume") \__pi_opcode[2]__assume (\__pi_opcode[2] );
  miter_def_prop #(1, "assume") \__pi_opcode[3]__assume (\__pi_opcode[3] );
  miter_def_prop #(1, "assume") \__pi_opcode[4]__assume (\__pi_opcode[4] );
`endif
`ifndef DIRECT_CROSS_POINTS
`endif
`ifdef CHECK_MATCH_POINTS
`endif
`ifdef CHECK_OUTPUTS
  miter_cmp_prop #(1, "assert") \__po_Cin__assert (\__po_Cin__gold , \__po_Cin__gate );
  miter_cmp_prop #(1, "assert") \__po_alu_op[0]__assert (\__po_alu_op[0]__gold , \__po_alu_op[0]__gate );
  miter_cmp_prop #(1, "assert") \__po_alu_op[1]__assert (\__po_alu_op[1]__gold , \__po_alu_op[1]__gate );
  miter_cmp_prop #(1, "assert") \__po_alu_op[2]__assert (\__po_alu_op[2]__gold , \__po_alu_op[2]__gate );
  miter_cmp_prop #(1, "assert") \__po_alu_op_ext[0]__assert (\__po_alu_op_ext[0]__gold , \__po_alu_op_ext[0]__gate );
  miter_cmp_prop #(1, "assert") \__po_alu_op_ext[1]__assert (\__po_alu_op_ext[1]__gold , \__po_alu_op_ext[1]__gate );
  miter_cmp_prop #(1, "assert") \__po_alu_op_ext[2]__assert (\__po_alu_op_ext[2]__gold , \__po_alu_op_ext[2]__gate );
  miter_cmp_prop #(1, "assert") \__po_alu_op_ext[3]__assert (\__po_alu_op_ext[3]__gold , \__po_alu_op_ext[3]__gate );
  miter_cmp_prop #(1, "assert") \__po_beqz__assert (\__po_beqz__gold , \__po_beqz__gate );
  miter_cmp_prop #(1, "assert") \__po_bgez__assert (\__po_bgez__gold , \__po_bgez__gate );
  miter_cmp_prop #(1, "assert") \__po_bltz__assert (\__po_bltz__gold , \__po_bltz__gate );
  miter_cmp_prop #(1, "assert") \__po_bnez__assert (\__po_bnez__gold , \__po_bnez__gate );
  miter_cmp_prop #(1, "assert") \__po_halt__assert (\__po_halt__gold , \__po_halt__gate );
  miter_cmp_prop #(1, "assert") \__po_invA__assert (\__po_invA__gold , \__po_invA__gate );
  miter_cmp_prop #(1, "assert") \__po_invB__assert (\__po_invB__gold , \__po_invB__gate );
  miter_cmp_prop #(1, "assert") \__po_jump__assert (\__po_jump__gold , \__po_jump__gate );
  miter_cmp_prop #(1, "assert") \__po_mem_write__assert (\__po_mem_write__gold , \__po_mem_write__gate );
  miter_cmp_prop #(1, "assert") \__po_reg_write__assert (\__po_reg_write__gold , \__po_reg_write__gate );
  miter_cmp_prop #(1, "assert") \__po_sel_alu_opB[0]__assert (\__po_sel_alu_opB[0]__gold , \__po_sel_alu_opB[0]__gate );
  miter_cmp_prop #(1, "assert") \__po_sel_alu_opB[1]__assert (\__po_sel_alu_opB[1]__gold , \__po_sel_alu_opB[1]__gate );
  miter_cmp_prop #(1, "assert") \__po_sel_pc_opA__assert (\__po_sel_pc_opA__gold , \__po_sel_pc_opA__gate );
  miter_cmp_prop #(1, "assert") \__po_sel_pc_opB__assert (\__po_sel_pc_opB__gold , \__po_sel_pc_opB__gate );
  miter_cmp_prop #(1, "assert") \__po_sel_reg_dst[0]__assert (\__po_sel_reg_dst[0]__gold , \__po_sel_reg_dst[0]__gate );
  miter_cmp_prop #(1, "assert") \__po_sel_reg_dst[1]__assert (\__po_sel_reg_dst[1]__gold , \__po_sel_reg_dst[1]__gate );
  miter_cmp_prop #(1, "assert") \__po_sel_wb__assert (\__po_sel_wb__gold , \__po_sel_wb__gate );
  miter_cmp_prop #(1, "assert") \__po_sign__assert (\__po_sign__gold , \__po_sign__gate );
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
  miter_def_prop #(1, "cover") \__po_Cin__gold_cover (\__po_Cin__gold );
  miter_def_prop #(1, "cover") \__po_alu_op[0]__gold_cover (\__po_alu_op[0]__gold );
  miter_def_prop #(1, "cover") \__po_alu_op[1]__gold_cover (\__po_alu_op[1]__gold );
  miter_def_prop #(1, "cover") \__po_alu_op[2]__gold_cover (\__po_alu_op[2]__gold );
  miter_def_prop #(1, "cover") \__po_alu_op_ext[0]__gold_cover (\__po_alu_op_ext[0]__gold );
  miter_def_prop #(1, "cover") \__po_alu_op_ext[1]__gold_cover (\__po_alu_op_ext[1]__gold );
  miter_def_prop #(1, "cover") \__po_alu_op_ext[2]__gold_cover (\__po_alu_op_ext[2]__gold );
  miter_def_prop #(1, "cover") \__po_alu_op_ext[3]__gold_cover (\__po_alu_op_ext[3]__gold );
  miter_def_prop #(1, "cover") \__po_beqz__gold_cover (\__po_beqz__gold );
  miter_def_prop #(1, "cover") \__po_bgez__gold_cover (\__po_bgez__gold );
  miter_def_prop #(1, "cover") \__po_bltz__gold_cover (\__po_bltz__gold );
  miter_def_prop #(1, "cover") \__po_bnez__gold_cover (\__po_bnez__gold );
  miter_def_prop #(1, "cover") \__po_halt__gold_cover (\__po_halt__gold );
  miter_def_prop #(1, "cover") \__po_invA__gold_cover (\__po_invA__gold );
  miter_def_prop #(1, "cover") \__po_invB__gold_cover (\__po_invB__gold );
  miter_def_prop #(1, "cover") \__po_jump__gold_cover (\__po_jump__gold );
  miter_def_prop #(1, "cover") \__po_mem_write__gold_cover (\__po_mem_write__gold );
  miter_def_prop #(1, "cover") \__po_reg_write__gold_cover (\__po_reg_write__gold );
  miter_def_prop #(1, "cover") \__po_sel_alu_opB[0]__gold_cover (\__po_sel_alu_opB[0]__gold );
  miter_def_prop #(1, "cover") \__po_sel_alu_opB[1]__gold_cover (\__po_sel_alu_opB[1]__gold );
  miter_def_prop #(1, "cover") \__po_sel_pc_opA__gold_cover (\__po_sel_pc_opA__gold );
  miter_def_prop #(1, "cover") \__po_sel_pc_opB__gold_cover (\__po_sel_pc_opB__gold );
  miter_def_prop #(1, "cover") \__po_sel_reg_dst[0]__gold_cover (\__po_sel_reg_dst[0]__gold );
  miter_def_prop #(1, "cover") \__po_sel_reg_dst[1]__gold_cover (\__po_sel_reg_dst[1]__gold );
  miter_def_prop #(1, "cover") \__po_sel_wb__gold_cover (\__po_sel_wb__gold );
  miter_def_prop #(1, "cover") \__po_sign__gold_cover (\__po_sign__gold );
`endif
`ifdef COVER_DEF_GATE_OUTPUTS
  miter_def_prop #(1, "cover") \__po_Cin__gate_cover (\__po_Cin__gate );
  miter_def_prop #(1, "cover") \__po_alu_op[0]__gate_cover (\__po_alu_op[0]__gate );
  miter_def_prop #(1, "cover") \__po_alu_op[1]__gate_cover (\__po_alu_op[1]__gate );
  miter_def_prop #(1, "cover") \__po_alu_op[2]__gate_cover (\__po_alu_op[2]__gate );
  miter_def_prop #(1, "cover") \__po_alu_op_ext[0]__gate_cover (\__po_alu_op_ext[0]__gate );
  miter_def_prop #(1, "cover") \__po_alu_op_ext[1]__gate_cover (\__po_alu_op_ext[1]__gate );
  miter_def_prop #(1, "cover") \__po_alu_op_ext[2]__gate_cover (\__po_alu_op_ext[2]__gate );
  miter_def_prop #(1, "cover") \__po_alu_op_ext[3]__gate_cover (\__po_alu_op_ext[3]__gate );
  miter_def_prop #(1, "cover") \__po_beqz__gate_cover (\__po_beqz__gate );
  miter_def_prop #(1, "cover") \__po_bgez__gate_cover (\__po_bgez__gate );
  miter_def_prop #(1, "cover") \__po_bltz__gate_cover (\__po_bltz__gate );
  miter_def_prop #(1, "cover") \__po_bnez__gate_cover (\__po_bnez__gate );
  miter_def_prop #(1, "cover") \__po_halt__gate_cover (\__po_halt__gate );
  miter_def_prop #(1, "cover") \__po_invA__gate_cover (\__po_invA__gate );
  miter_def_prop #(1, "cover") \__po_invB__gate_cover (\__po_invB__gate );
  miter_def_prop #(1, "cover") \__po_jump__gate_cover (\__po_jump__gate );
  miter_def_prop #(1, "cover") \__po_mem_write__gate_cover (\__po_mem_write__gate );
  miter_def_prop #(1, "cover") \__po_reg_write__gate_cover (\__po_reg_write__gate );
  miter_def_prop #(1, "cover") \__po_sel_alu_opB[0]__gate_cover (\__po_sel_alu_opB[0]__gate );
  miter_def_prop #(1, "cover") \__po_sel_alu_opB[1]__gate_cover (\__po_sel_alu_opB[1]__gate );
  miter_def_prop #(1, "cover") \__po_sel_pc_opA__gate_cover (\__po_sel_pc_opA__gate );
  miter_def_prop #(1, "cover") \__po_sel_pc_opB__gate_cover (\__po_sel_pc_opB__gate );
  miter_def_prop #(1, "cover") \__po_sel_reg_dst[0]__gate_cover (\__po_sel_reg_dst[0]__gate );
  miter_def_prop #(1, "cover") \__po_sel_reg_dst[1]__gate_cover (\__po_sel_reg_dst[1]__gate );
  miter_def_prop #(1, "cover") \__po_sel_wb__gate_cover (\__po_sel_wb__gate );
  miter_def_prop #(1, "cover") \__po_sign__gate_cover (\__po_sign__gate );
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
  input  [  0:0] \__pi_op_ext[0] ,
  input  [  0:0] \__pi_op_ext[1] ,
  input  [  0:0] \__pi_opcode[0] ,
  input  [  0:0] \__pi_opcode[1] ,
  input  [  0:0] \__pi_opcode[2] ,
  input  [  0:0] \__pi_opcode[3] ,
  input  [  0:0] \__pi_opcode[4] ,
  output [  0:0] \__po_Cin ,
  output [  0:0] \__po_alu_op[0] ,
  output [  0:0] \__po_alu_op[1] ,
  output [  0:0] \__po_alu_op[2] ,
  output [  0:0] \__po_alu_op_ext[0] ,
  output [  0:0] \__po_alu_op_ext[1] ,
  output [  0:0] \__po_alu_op_ext[2] ,
  output [  0:0] \__po_alu_op_ext[3] ,
  output [  0:0] \__po_beqz ,
  output [  0:0] \__po_bgez ,
  output [  0:0] \__po_bltz ,
  output [  0:0] \__po_bnez ,
  output [  0:0] \__po_halt ,
  output [  0:0] \__po_invA ,
  output [  0:0] \__po_invB ,
  output [  0:0] \__po_jump ,
  output [  0:0] \__po_mem_write ,
  output [  0:0] \__po_reg_write ,
  output [  0:0] \__po_sel_alu_opB[0] ,
  output [  0:0] \__po_sel_alu_opB[1] ,
  output [  0:0] \__po_sel_pc_opA ,
  output [  0:0] \__po_sel_pc_opB ,
  output [  0:0] \__po_sel_reg_dst[0] ,
  output [  0:0] \__po_sel_reg_dst[1] ,
  output [  0:0] \__po_sel_wb ,
  output [  0:0] \__po_sign
);
endmodule
module \gate.top (
  input  [  0:0] \__pi_op_ext[0] ,
  input  [  0:0] \__pi_op_ext[1] ,
  input  [  0:0] \__pi_opcode[0] ,
  input  [  0:0] \__pi_opcode[1] ,
  input  [  0:0] \__pi_opcode[2] ,
  input  [  0:0] \__pi_opcode[3] ,
  input  [  0:0] \__pi_opcode[4] ,
  output [  0:0] \__po_Cin ,
  output [  0:0] \__po_alu_op[0] ,
  output [  0:0] \__po_alu_op[1] ,
  output [  0:0] \__po_alu_op[2] ,
  output [  0:0] \__po_alu_op_ext[0] ,
  output [  0:0] \__po_alu_op_ext[1] ,
  output [  0:0] \__po_alu_op_ext[2] ,
  output [  0:0] \__po_alu_op_ext[3] ,
  output [  0:0] \__po_beqz ,
  output [  0:0] \__po_bgez ,
  output [  0:0] \__po_bltz ,
  output [  0:0] \__po_bnez ,
  output [  0:0] \__po_halt ,
  output [  0:0] \__po_invA ,
  output [  0:0] \__po_invB ,
  output [  0:0] \__po_jump ,
  output [  0:0] \__po_mem_write ,
  output [  0:0] \__po_reg_write ,
  output [  0:0] \__po_sel_alu_opB[0] ,
  output [  0:0] \__po_sel_alu_opB[1] ,
  output [  0:0] \__po_sel_pc_opA ,
  output [  0:0] \__po_sel_pc_opB ,
  output [  0:0] \__po_sel_reg_dst[0] ,
  output [  0:0] \__po_sel_reg_dst[1] ,
  output [  0:0] \__po_sel_wb ,
  output [  0:0] \__po_sign
);
endmodule
