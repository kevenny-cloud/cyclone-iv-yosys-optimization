module t(input[5:0] a,output[2:0] y);wire u,v;
 \$lut #(.WIDTH(2),.LUT(4'h7)) root(.A(a[1:0]),.Y(u));
\$lut #(.WIDTH(2),.LUT(4'h8)) opposite(.A(a[1:0]),.Y(v));
 \$lut #(.WIDTH(2),.LUT(4'h2)) c0(.A({a[5],u}),.Y(y[0]));
 \$lut #(.WIDTH(2),.LUT(4'h8)) c1(.A({a[5],v}),.Y(y[1]));
 \$lut #(.WIDTH(2),.LUT(4'h6)) c2(.A({a[5],u}),.Y(y[2]));
endmodule
