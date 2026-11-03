module t(input[3:0] sel,input[15:0] d,output[0:0] y);
wire[3:0] s=sel;
reg[0:0] z;always @* case(s)
4'd0: z=d[0 +: 1];
4'd1: z=d[1 +: 1];
4'd2: z=d[2 +: 1];
4'd3: z=d[3 +: 1];
4'd4: z=d[4 +: 1];
4'd5: z=d[5 +: 1];
4'd6: z=d[6 +: 1];
4'd7: z=d[7 +: 1];
4'd8: z=d[8 +: 1];
4'd9: z=d[9 +: 1];
4'd10: z=d[10 +: 1];
4'd11: z=d[11 +: 1];
4'd12: z=d[12 +: 1];
4'd13: z=d[13 +: 1];
4'd14: z=d[14 +: 1];
default:z=d[15 +: 1];
endcase
assign y=z;endmodule
