module unit(input clk,rst,en,input[2:0] sel,input[6:0] a,b,c,d,output[6:0] y,output[6:0] tap);
reg[6:0] u,v;
always @* case(sel) 0:u=a;1:u=b;2:u=c;3:u=d;4:u=a^7'd0;default:u=b^d;endcase
always @* case(sel[1:0]) 0:v=u^c;1:v=a&d;2:v=u|b;default:v=c^7'd0;endcase
assign tap=u;
assign y=v;
endmodule
