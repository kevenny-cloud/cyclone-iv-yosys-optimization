
module cycloneiv_lcell_comb #(parameter lut_mask=16'hffff, parameter sum_lutc_input="datac", parameter dont_touch="off", parameter lpm_type="cycloneiv_lcell_comb")
 (input dataa,datab,datac,datad,cin,output combout,cout);
 wire _TECHMAP_FAIL_ = sum_lutc_input != "datac";
 \$lut #(.WIDTH(4),.LUT(lut_mask)) comb (.A({datad,datac,datab,dataa}),.Y(combout));
 \$lut #(.WIDTH(3),.LUT(lut_mask[7:0])) carry (.A({cin,datab,dataa}),.Y(cout));
endmodule
module dffeas #(parameter power_up="low", parameter is_wysiwyg="TRUE")
 (input d,clk,clrn,prn,ena,asdata,aload,sclr,sload,output q);
 // Restricted model: prn=1, aload=0; ENA precedes SCLR, then SLOAD.
 // Based on the documented Intel register-control order; no async load/preset claim.
 wire _TECHMAP_FAIL_ = power_up != "low" && power_up != "high";
 (* init = (power_up == "high" ? 1'b1 : 1'b0) *) wire state_q;
 \$adffe #(.WIDTH(1),.CLK_POLARITY(1),.ARST_POLARITY(0),.ARST_VALUE(1'b0),.EN_POLARITY(1))
 ff (.CLK(clk),.D(sclr ? 1'b0 : (sload ? asdata : d)),.Q(state_q),.ARST(clrn),.EN(ena));
 assign q = state_q;
endmodule
