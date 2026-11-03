module t(input[1:0] sel,input[19:0] d,output[4:0] y);
wire[1:0] s=sel;
reg[4:0] z;always @* case(s)
2'd0: z=d[0 +: 5];
2'd1: z=d[5 +: 5];
2'd2: z=d[10 +: 5];
default:z=d[15 +: 5];
endcase
assign y=z;endmodule
