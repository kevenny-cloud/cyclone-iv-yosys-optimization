module fresh_10(input clk,en,input[1:0] s,input[6:0] a,b,c,d,output[6:0] y,tap);
(* keep *) reg[6:0] x;reg[6:0] q;initial q=0;
always @* case(s)0:x=a+b;1:x=b+c;2:x=c+d;default:x=d+a;endcase
always @(posedge clk)if(en)q<=x;assign tap=x;assign y=q;endmodule
