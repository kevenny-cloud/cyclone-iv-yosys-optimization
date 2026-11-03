module t(input[5:0] a,output[4:0] y);wire u,v;
 \$lut #(.WIDTH(4),.LUT(16'h7e6d)) root(.A(a[3:0]),.Y(u));
\$lut #(.WIDTH(4),.LUT(16'h8192)) opposite(.A(a[3:0]),.Y(v));
 \$lut #(.WIDTH(4),.LUT(16'h3447)) c0(.A({a[2],a[1],a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(4),.LUT(16'h3969)) c1(.A({a[2],a[1],a[0],v}),.Y(y[1]));
 \$lut #(.WIDTH(4),.LUT(16'h57a)) c2(.A({a[2],a[1],a[0],u}),.Y(y[2]));
 \$lut #(.WIDTH(4),.LUT(16'h491a)) c3(.A({a[2],a[1],a[0],v}),.Y(y[3]));
 \$lut #(.WIDTH(4),.LUT(16'h6f96)) c4(.A({a[2],a[1],a[0],u}),.Y(y[4]));
endmodule
