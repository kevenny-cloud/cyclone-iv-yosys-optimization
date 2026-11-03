module t(input[3:0] sel,input[79:0] d,output[4:0] y);
wire[3:0] s=sel;
reg[4:0] z;always @* case(s)
4'd0: z=d[0 +: 5];
4'd1: z=d[5 +: 5];
4'd2: z=d[10 +: 5];
4'd3: z=d[15 +: 5];
4'd4: z=d[20 +: 5];
4'd5: z=d[25 +: 5];
4'd6: z=d[30 +: 5];
4'd7: z=d[35 +: 5];
4'd8: z=d[40 +: 5];
4'd9: z=d[45 +: 5];
4'd10: z=d[50 +: 5];
4'd11: z=d[55 +: 5];
4'd12: z=d[60 +: 5];
4'd13: z=d[65 +: 5];
4'd14: z=d[70 +: 5];
default:z=d[75 +: 5];
endcase
assign y=z;endmodule
