module t(input [1:0] sel,input [2:0] a,b,c,d,k, input clk,output reg [2:0] y);
(* init=3'b101 *) wire [2:0] ay=y;
always @* case(sel)
0:y=a^k;1:y=b^k;2:y=c^k;default:y=d^k;
endcase
endmodule
