module t(input [1:0] sel,input [3:0] a,b,c,d,e,f,g,h,output reg [3:0] y);always @* case(sel) 0:y=a+b;1:y=c+d;2:y=e+f;default:y=g+h;endcase endmodule
