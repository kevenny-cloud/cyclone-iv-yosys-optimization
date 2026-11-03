module renamed_unit(input clk,rst,en,input[2:0] sel,input[2:0] a,b,c,d,output reg[2:0] y,output[2:0] tap);
reg[2:0] internal_u,v;
always @* case(sel) 0:internal_u=a;1:internal_u=b;2:internal_u=c;3:internal_u=d;4:internal_u=a^3'd0;default:internal_u=b^d;endcase
always @* case(sel[1:0]) 0:v=internal_u^c;1:v=a&d;2:v=internal_u|b;default:v=c^3'd0;endcase
assign tap=v^internal_u;
initial y=3'd0;always @(posedge clk)if(rst)y<=3'd0;else if(en)y<=v;
endmodule
