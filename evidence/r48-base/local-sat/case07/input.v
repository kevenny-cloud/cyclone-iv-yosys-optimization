module t(input[5:0] a,output[3:0] y);wire u,v;
 \$lut #(.WIDTH(3),.LUT(8'he5)) root(.A(a[2:0]),.Y(u));
\$lut #(.WIDTH(3),.LUT(8'h1a)) opposite(.A(a[2:0]),.Y(v));
 \$lut #(.WIDTH(3),.LUT(8'hb8)) c0(.A({a[5],a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(3),.LUT(8'h6b)) c1(.A({a[5],a[0],v}),.Y(y[1]));
 \$lut #(.WIDTH(3),.LUT(8'h63)) c2(.A({a[5],a[0],u}),.Y(y[2]));
 \$lut #(.WIDTH(3),.LUT(8'hc)) c3(.A({a[5],a[0],v}),.Y(y[3]));
endmodule
