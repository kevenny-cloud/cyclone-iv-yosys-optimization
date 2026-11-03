module renamed_unit(input clk,rst,en,input[2:0] sel,input[0:0] a,b,c,d,output[0:0] y,output[0:0] tap);
reg[0:0] internal_u,v;
always @* case(sel) 0:internal_u=a;1:internal_u=b;2:internal_u=c;3:internal_u=d;4:internal_u=a^1'd0;default:internal_u=b^d;endcase
always @* case(sel[1:0]) 0:v=internal_u^c;1:v=a&d;2:v=internal_u|b;default:v=c^1'd0;endcase
assign tap=internal_u;
assign y=v;
endmodule
