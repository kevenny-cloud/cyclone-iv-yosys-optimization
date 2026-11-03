module t(input[5:0] a,output[2:0] y);wire u,v;
 \$lut #(.WIDTH(3),.LUT(8'h2e)) root(.A(a[2:0]),.Y(u));
\$lut #(.WIDTH(3),.LUT(8'hd1)) opposite(.A(a[2:0]),.Y(v));
 \$lut #(.WIDTH(3),.LUT(8'h24)) c0(.A({a[1],a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(3),.LUT(8'h8a)) c1(.A({a[1],a[0],v}),.Y(y[1]));
 \$lut #(.WIDTH(3),.LUT(8'h30)) c2(.A({a[1],a[0],u}),.Y(y[2]));
endmodule
