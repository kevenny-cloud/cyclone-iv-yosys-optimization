module t(input[5:0] a,output[3:0] y);wire u,v;
 \$lut #(.WIDTH(3),.LUT(8'h43)) root(.A(a[2:0]),.Y(u));
\$lut #(.WIDTH(3),.LUT(8'hbc)) opposite(.A(a[2:0]),.Y(v));
 \$lut #(.WIDTH(3),.LUT(8'h3e)) c0(.A({a[1],a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(3),.LUT(8'h8f)) c1(.A({a[1],a[0],v}),.Y(y[1]));
 \$lut #(.WIDTH(3),.LUT(8'h73)) c2(.A({a[1],a[0],u}),.Y(y[2]));
 \$lut #(.WIDTH(3),.LUT(8'h11)) c3(.A({a[1],a[0],v}),.Y(y[3]));
endmodule
