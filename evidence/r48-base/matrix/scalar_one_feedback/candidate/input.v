module top(input clk,rst,en,input [1:0] s,input a,b,output reg q=1'b1,output reg r=1'b0);
reg t;
always @* case(s) 0:t=a;1:t=s[0]^b;2:t=q^r;default:t=b;endcase
always @(posedge clk) if(rst)q<=1'b1;else if(en)q<=t^r;
always @(posedge clk) if(en)r<=t^q;
endmodule
