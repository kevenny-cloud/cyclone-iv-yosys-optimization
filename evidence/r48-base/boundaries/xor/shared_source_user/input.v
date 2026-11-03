module t(input [1:0] sel,input [2:0] a,b,c,d,k, input clk,output [2:0] extra,output reg [2:0] y);
assign extra=a^k;
always @* case(sel)
0:y=extra;1:y=b^k;2:y=c^k;default:y=d^k;
endcase
endmodule
