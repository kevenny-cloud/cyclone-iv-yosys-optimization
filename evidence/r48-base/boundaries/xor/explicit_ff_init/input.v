module t(input [1:0] sel,input [2:0] a,b,c,d,k, input clk,output reg [2:0] y);
reg [2:0] q=3'b101; always @(posedge clk) q<=k;
always @* case(sel)
0:y=a^q;1:y=b^q;2:y=c^q;default:y=d^q;
endcase
endmodule
