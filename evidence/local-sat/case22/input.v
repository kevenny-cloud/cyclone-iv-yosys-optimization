module t(input[5:0] a,output[3:0] y);wire u,v;
 \$lut #(.WIDTH(3),.LUT(8'hc9)) root(.A(a[2:0]),.Y(u));
\$lut #(.WIDTH(3),.LUT(8'h36)) opposite(.A(a[2:0]),.Y(v));
 \$lut #(.WIDTH(3),.LUT(8'h4c)) c0(.A({a[1],a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(3),.LUT(8'h49)) c1(.A({a[1],a[0],v}),.Y(y[1]));
 \$lut #(.WIDTH(3),.LUT(8'hcd)) c2(.A({a[1],a[0],u}),.Y(y[2]));
 \$lut #(.WIDTH(3),.LUT(8'hfa)) c3(.A({a[1],a[0],v}),.Y(y[3]));
endmodule
