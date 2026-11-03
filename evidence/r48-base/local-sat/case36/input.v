module t(input[5:0] a,output[2:0] y);wire u,v;
 \$lut #(.WIDTH(2),.LUT(4'hc)) root(.A(a[1:0]),.Y(u));
\$lut #(.WIDTH(2),.LUT(4'h3)) opposite(.A(a[1:0]),.Y(v));
 \$lut #(.WIDTH(2),.LUT(4'h7)) c0(.A({a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(2),.LUT(4'h3)) c1(.A({a[0],v}),.Y(y[1]));
 \$lut #(.WIDTH(2),.LUT(4'h2)) c2(.A({a[0],u}),.Y(y[2]));
endmodule
