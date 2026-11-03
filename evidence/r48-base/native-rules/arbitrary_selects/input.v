module t(input[2:0] s,input[11:0] d,output[2:0] y);
\$pmux #(.WIDTH(3),.S_WIDTH(3)) p(.A(d[2:0]),.B(d[11:3]),.S(s),.Y(y));endmodule
