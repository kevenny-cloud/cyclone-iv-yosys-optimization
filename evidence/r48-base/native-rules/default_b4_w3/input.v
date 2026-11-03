module t(input[3:0] sel,input[47:0] d,output[2:0] y);
wire[3:0] s=sel;
reg[2:0] z;always @* case(s)
4'd0: z=d[0 +: 3];
4'd1: z=d[3 +: 3];
4'd2: z=d[6 +: 3];
4'd3: z=d[9 +: 3];
4'd4: z=d[12 +: 3];
4'd5: z=d[15 +: 3];
4'd6: z=d[18 +: 3];
4'd7: z=d[21 +: 3];
4'd8: z=d[24 +: 3];
4'd9: z=d[27 +: 3];
4'd10: z=d[30 +: 3];
4'd11: z=d[33 +: 3];
4'd12: z=d[36 +: 3];
4'd13: z=d[39 +: 3];
4'd14: z=d[42 +: 3];
default:z=d[45 +: 3];
endcase
assign y=z;endmodule
