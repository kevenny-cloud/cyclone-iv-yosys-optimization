module t(input[5:0] a,output tap,output[5:0] y);wire u,v;
 \$lut #(.WIDTH(2),.LUT(4'h3)) root(.A(a[1:0]),.Y(u));
\$lut #(.WIDTH(2),.LUT(4'hc)) opposite(.A(a[1:0]),.Y(v));
 \$lut #(.WIDTH(2),.LUT(4'hc)) c0(.A({a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(2),.LUT(4'h3)) c1(.A({a[0],v}),.Y(y[1]));
 \$lut #(.WIDTH(2),.LUT(4'hd)) c2(.A({a[0],u}),.Y(y[2]));
 \$lut #(.WIDTH(2),.LUT(4'ha)) c3(.A({a[0],v}),.Y(y[3]));
 \$lut #(.WIDTH(2),.LUT(4'ha)) c4(.A({a[0],u}),.Y(y[4]));
 \$lut #(.WIDTH(2),.LUT(4'hd)) c5(.A({a[0],v}),.Y(y[5]));
assign tap=u;
endmodule
