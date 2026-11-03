module fresh_05(input clk,rn,rst,en,input[3:0] s,input[2:0] a,b,c,d,output[2:0] y);
reg[2:0] q;reg[2:0] x,z;initial q='d0;
always @* case(s[1:0]) 0:x=a^q;1:x=b+q;2:x=c-q;default:x=d;endcase
always @* case(s[3:2]) 0:z=x;1:z=b^a;2:z=c+q;default:z=q;endcase
always @(posedge clk or negedge rn) begin if(!rn) q<='d0; else if(en)q<=z;end
assign y=q;endmodule
