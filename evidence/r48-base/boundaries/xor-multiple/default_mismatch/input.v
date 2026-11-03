module t(input [1:0] s0,s1,input [3:0] a,b,c,d,k,e,f,g,h,j,output reg [3:0] y0,y1);
always @* case(s0) 0:y0=a^k;1:y0=b^k;2:y0=c^k;default:y0=d^k;endcase
always @* case(s1) 0:y1=e^j;1:y1=f^j;2:y1=g^j;default:y1=h|j;endcase
endmodule
