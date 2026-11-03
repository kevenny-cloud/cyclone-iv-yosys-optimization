module t(input[5:0] a,output[5:0] y);wire u,v;
 \$lut #(.WIDTH(3),.LUT(8'hbd)) root(.A(a[2:0]),.Y(u));
\$lut #(.WIDTH(3),.LUT(8'h42)) opposite(.A(a[2:0]),.Y(v));
 \$lut #(.WIDTH(3),.LUT(8'hcd)) c0(.A({a[1],a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(3),.LUT(8'hf)) c1(.A({a[1],a[0],v}),.Y(y[1]));
 \$lut #(.WIDTH(3),.LUT(8'h37)) c2(.A({a[1],a[0],u}),.Y(y[2]));
 \$lut #(.WIDTH(3),.LUT(8'h97)) c3(.A({a[1],a[0],v}),.Y(y[3]));
 \$lut #(.WIDTH(3),.LUT(8'h3a)) c4(.A({a[1],a[0],u}),.Y(y[4]));
 \$lut #(.WIDTH(3),.LUT(8'hee)) c5(.A({a[1],a[0],v}),.Y(y[5]));
endmodule
