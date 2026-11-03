module leaf(input clk,rst,en,input[2:0] sel,input[0:0] a,b,c,d,output[0:0] y,output[0:0] tap);
reg[0:0] u,v;
always @* case(sel) 0:u=a;1:u=b;2:u=c;3:u=d;4:u=a^1'd0;default:u=b^d;endcase
always @* case(sel[1:0]) 0:v=u^c;1:v=a&d;2:v=u|b;default:v=c^1'd0;endcase
assign tap=u;
assign y=v;
endmodule
module unit(input clk,rst,en,input[2:0] sel,input[0:0] a,b,c,d,output[0:0] y,output[0:0] tap);
wire[0:0] y0,y1,t0,t1;
leaf left(.clk(clk),.rst(rst),.en(en),.sel(sel),.a(a),.b(b),.c(c),.d(d),.y(y0),.tap(t0));
leaf right(.clk(clk),.rst(rst),.en(en),.sel(sel),.a(d),.b(c),.c(b),.d(a),.y(y1),.tap(t1));
assign y=y0^y1;assign tap=t0^t1;endmodule
