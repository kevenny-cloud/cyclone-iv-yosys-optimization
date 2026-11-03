module t(input[5:0] a,output[3:0] y);wire u,v;
 \$lut #(.WIDTH(4),.LUT(16'hdad1)) root(.A(a[3:0]),.Y(u));
\$lut #(.WIDTH(4),.LUT(16'h252e)) opposite(.A(a[3:0]),.Y(v));
 \$lut #(.WIDTH(4),.LUT(16'h61d2)) c0(.A({a[2],a[1],a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(4),.LUT(16'hc3df)) c1(.A({a[2],a[1],a[0],v}),.Y(y[1]));
 \$lut #(.WIDTH(4),.LUT(16'hb464)) c2(.A({a[2],a[1],a[0],u}),.Y(y[2]));
 \$lut #(.WIDTH(4),.LUT(16'hae1d)) c3(.A({a[2],a[1],a[0],v}),.Y(y[3]));
endmodule
