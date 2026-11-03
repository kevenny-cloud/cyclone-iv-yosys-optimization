module unit(input clk,rst,en,input[2:0] sel,input[2:0] a,b,c,d,output reg[2:0] y,output[2:0] tap);
reg[2:0] u,v;
always @* case(sel) 0:u=a;1:u=b;2:u=c;3:u=d;4:u=a^3'd5;default:u=b^d;endcase
always @* case(sel[1:0]) 0:v=u^c;1:v=a&d;2:v=u|b;default:v=c^3'd5;endcase
assign tap=v^u;
initial y=3'd5;always @(posedge clk)if(rst)y<=3'd5;else if(en)y<=v;
endmodule
