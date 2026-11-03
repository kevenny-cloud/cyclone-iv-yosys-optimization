module t(input[5:0] a,output tap,output[5:0] y);wire u,v;
 \$lut #(.WIDTH(4),.LUT(16'he7ad)) root(.A(a[3:0]),.Y(u));
\$lut #(.WIDTH(4),.LUT(16'h1852)) opposite(.A(a[3:0]),.Y(v));
 \$lut #(.WIDTH(4),.LUT(16'hc00d)) c0(.A({a[5],a[1],a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(4),.LUT(16'hfa5b)) c1(.A({a[5],a[1],a[0],v}),.Y(y[1]));
 \$lut #(.WIDTH(4),.LUT(16'h1b5b)) c2(.A({a[5],a[1],a[0],u}),.Y(y[2]));
 \$lut #(.WIDTH(4),.LUT(16'h87c9)) c3(.A({a[5],a[1],a[0],v}),.Y(y[3]));
 \$lut #(.WIDTH(4),.LUT(16'h598e)) c4(.A({a[5],a[1],a[0],u}),.Y(y[4]));
 \$lut #(.WIDTH(4),.LUT(16'h137e)) c5(.A({a[5],a[1],a[0],v}),.Y(y[5]));
assign tap=u;
endmodule
