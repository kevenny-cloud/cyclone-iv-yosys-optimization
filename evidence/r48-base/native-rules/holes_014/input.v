module t(input[2:0] sel,input[23:0] d,output[2:0] y);
wire[2:0] s=sel;
reg[2:0] z;always @* case(s)
3'd0: z=d[0 +: 3];
3'd1: z=d[3 +: 3];
3'd4: z=d[12 +: 3];
default:z=d[21 +: 3];
endcase
assign y=z;endmodule
