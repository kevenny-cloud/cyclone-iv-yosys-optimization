module t(input [1:0] sel,input [2:0] a,b,c,d,k, input clk,output reg [2:0] y);
wire [2:0] alias_k=k;
always @* case(sel)
0:y=a^alias_k;1:y=b^alias_k;2:y=c^alias_k;default:y=d^alias_k;
endcase
endmodule
