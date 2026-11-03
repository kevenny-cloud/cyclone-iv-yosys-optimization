module t(input[1:0] sel,input[3:0] d,output[0:0] y);
wire[1:0] s=sel;
reg[0:0] z;always @* case(s)
2'd0: z=d[0 +: 1];
2'd1: z=d[1 +: 1];
2'd2: z=d[2 +: 1];
default:z=d[3 +: 1];
endcase
assign y=z;endmodule
