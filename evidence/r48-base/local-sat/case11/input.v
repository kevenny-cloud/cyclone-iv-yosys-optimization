module t(input[5:0] a,output[2:0] y);wire u,v;
 \$lut #(.WIDTH(4),.LUT(16'hfd52)) root(.A(a[3:0]),.Y(u));
\$lut #(.WIDTH(4),.LUT(16'h2ad)) opposite(.A(a[3:0]),.Y(v));
 \$lut #(.WIDTH(4),.LUT(16'hcb6d)) c0(.A({a[2],a[1],a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(4),.LUT(16'hd37c)) c1(.A({a[2],a[1],a[0],v}),.Y(y[1]));
 \$lut #(.WIDTH(4),.LUT(16'h2c0b)) c2(.A({a[2],a[1],a[0],u}),.Y(y[2]));
endmodule
