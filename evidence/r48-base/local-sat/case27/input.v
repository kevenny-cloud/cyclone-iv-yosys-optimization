module t(input[5:0] a,output[3:0] y);wire u,v;
 \$lut #(.WIDTH(2),.LUT(4'h3)) root(.A(a[1:0]),.Y(u));
\$lut #(.WIDTH(2),.LUT(4'hc)) opposite(.A(a[1:0]),.Y(v));
 \$lut #(.WIDTH(2),.LUT(4'h7)) c0(.A({a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(2),.LUT(4'hb)) c1(.A({a[0],v}),.Y(y[1]));
 \$lut #(.WIDTH(2),.LUT(4'h6)) c2(.A({a[0],u}),.Y(y[2]));
 \$lut #(.WIDTH(2),.LUT(4'hc)) c3(.A({a[0],v}),.Y(y[3]));
endmodule
