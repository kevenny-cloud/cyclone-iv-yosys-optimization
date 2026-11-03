module t(input[5:0] a,output[4:0] y);wire u,v;
 \$lut #(.WIDTH(3),.LUT(8'hc4)) root(.A(a[2:0]),.Y(u));
\$lut #(.WIDTH(3),.LUT(8'h3b)) opposite(.A(a[2:0]),.Y(v));
 \$lut #(.WIDTH(3),.LUT(8'h47)) c0(.A({a[5],a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(3),.LUT(8'hf6)) c1(.A({a[5],a[0],v}),.Y(y[1]));
 \$lut #(.WIDTH(3),.LUT(8'h43)) c2(.A({a[5],a[0],u}),.Y(y[2]));
 \$lut #(.WIDTH(3),.LUT(8'hf4)) c3(.A({a[5],a[0],v}),.Y(y[3]));
 \$lut #(.WIDTH(3),.LUT(8'h97)) c4(.A({a[5],a[0],u}),.Y(y[4]));
endmodule
