module t(input[5:0] a,output[1:0] y);wire u,v;
 \$lut #(.WIDTH(2),.LUT(4'h9)) root(.A(a[1:0]),.Y(u));
\$lut #(.WIDTH(2),.LUT(4'h6)) opposite(.A(a[1:0]),.Y(v));
 \$lut #(.WIDTH(2),.LUT(4'hf)) c0(.A({a[5],u}),.Y(y[0]));
 \$lut #(.WIDTH(2),.LUT(4'he)) c1(.A({a[5],v}),.Y(y[1]));
endmodule
