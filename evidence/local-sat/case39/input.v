module t(input[5:0] a,output[5:0] y);wire u,v;
 \$lut #(.WIDTH(2),.LUT(4'h8)) root(.A(a[1:0]),.Y(u));
\$lut #(.WIDTH(2),.LUT(4'h7)) opposite(.A(a[1:0]),.Y(v));
 \$lut #(.WIDTH(2),.LUT(4'h9)) c0(.A({a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(2),.LUT(4'h2)) c1(.A({a[0],v}),.Y(y[1]));
 \$lut #(.WIDTH(2),.LUT(4'hc)) c2(.A({a[0],u}),.Y(y[2]));
 \$lut #(.WIDTH(2),.LUT(4'h2)) c3(.A({a[0],v}),.Y(y[3]));
 \$lut #(.WIDTH(2),.LUT(4'h8)) c4(.A({a[0],u}),.Y(y[4]));
 \$lut #(.WIDTH(2),.LUT(4'hb)) c5(.A({a[0],v}),.Y(y[5]));
endmodule
