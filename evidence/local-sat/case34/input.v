module t(input[5:0] a,output[5:0] y);wire u,v;
 \$lut #(.WIDTH(3),.LUT(8'h73)) root(.A(a[2:0]),.Y(u));
\$lut #(.WIDTH(3),.LUT(8'h8c)) opposite(.A(a[2:0]),.Y(v));
 \$lut #(.WIDTH(3),.LUT(8'hda)) c0(.A({a[1],a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(3),.LUT(8'h6a)) c1(.A({a[1],a[0],v}),.Y(y[1]));
 \$lut #(.WIDTH(3),.LUT(8'h33)) c2(.A({a[1],a[0],u}),.Y(y[2]));
 \$lut #(.WIDTH(3),.LUT(8'h55)) c3(.A({a[1],a[0],v}),.Y(y[3]));
 \$lut #(.WIDTH(3),.LUT(8'h9b)) c4(.A({a[1],a[0],u}),.Y(y[4]));
 \$lut #(.WIDTH(3),.LUT(8'h12)) c5(.A({a[1],a[0],v}),.Y(y[5]));
endmodule
