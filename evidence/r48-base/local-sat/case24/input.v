module t(input[5:0] a,output[5:0] y);wire u,v;
 \$lut #(.WIDTH(2),.LUT(4'h0)) root(.A(a[1:0]),.Y(u));
\$lut #(.WIDTH(2),.LUT(4'hf)) opposite(.A(a[1:0]),.Y(v));
 \$lut #(.WIDTH(2),.LUT(4'hd)) c0(.A({a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(2),.LUT(4'he)) c1(.A({a[0],v}),.Y(y[1]));
 \$lut #(.WIDTH(2),.LUT(4'h1)) c2(.A({a[0],u}),.Y(y[2]));
 \$lut #(.WIDTH(2),.LUT(4'hc)) c3(.A({a[0],v}),.Y(y[3]));
 \$lut #(.WIDTH(2),.LUT(4'h9)) c4(.A({a[0],u}),.Y(y[4]));
 \$lut #(.WIDTH(2),.LUT(4'ha)) c5(.A({a[0],v}),.Y(y[5]));
endmodule
