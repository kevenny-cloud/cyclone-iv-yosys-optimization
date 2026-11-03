module leaf(input [1:0] s,input [7:0] a,b,c,d,output reg[7:0] y);
always @* case(s) 0:y=a;1:y=b;2:y=c;3:y=d;endcase
endmodule
module top(input clk,rst,en,input [1:0] s,input [7:0] a,b,c,d,output reg[7:0] y);
wire [7:0] t;leaf u(s,a,b,c,d,t);
always @(posedge clk or posedge rst) if(rst)y<=0;else if(en)y<=t;
endmodule
