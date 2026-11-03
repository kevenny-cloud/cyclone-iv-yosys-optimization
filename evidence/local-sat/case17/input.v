module t(input[5:0] a,output[3:0] y);wire u,v;
 \$lut #(.WIDTH(4),.LUT(16'hc566)) root(.A(a[3:0]),.Y(u));
\$lut #(.WIDTH(4),.LUT(16'h3a99)) opposite(.A(a[3:0]),.Y(v));
 \$lut #(.WIDTH(4),.LUT(16'h3d3f)) c0(.A({a[2],a[1],a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(4),.LUT(16'hbe21)) c1(.A({a[2],a[1],a[0],v}),.Y(y[1]));
 \$lut #(.WIDTH(4),.LUT(16'hfe11)) c2(.A({a[2],a[1],a[0],u}),.Y(y[2]));
 \$lut #(.WIDTH(4),.LUT(16'h6d84)) c3(.A({a[2],a[1],a[0],v}),.Y(y[3]));
endmodule
