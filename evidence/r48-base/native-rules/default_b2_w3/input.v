module t(input[1:0] sel,input[11:0] d,output[2:0] y);
wire[1:0] s=sel;
reg[2:0] z;always @* case(s)
2'd0: z=d[0 +: 3];
2'd1: z=d[3 +: 3];
2'd2: z=d[6 +: 3];
default:z=d[9 +: 3];
endcase
assign y=z;endmodule
