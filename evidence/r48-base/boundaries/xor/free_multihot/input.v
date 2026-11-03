module t(input [2:0] s,input [2:0] a,b,c,d,k,output [2:0] y);
wire [2:0] x0=a^k,x1=b^k,x2=c^k,x3=d^k;
\$pmux #(.WIDTH(3),.S_WIDTH(3)) p(.A(x0),.B({x3,x2,x1}),.S(s),.Y(y));
endmodule
