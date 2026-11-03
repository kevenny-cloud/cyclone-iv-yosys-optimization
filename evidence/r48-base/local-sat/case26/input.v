module t(input[5:0] a,output[2:0] y);wire u,v;
 \$lut #(.WIDTH(4),.LUT(16'hf772)) root(.A(a[3:0]),.Y(u));
\$lut #(.WIDTH(4),.LUT(16'h88d)) opposite(.A(a[3:0]),.Y(v));
 \$lut #(.WIDTH(4),.LUT(16'ha29f)) c0(.A({a[2],a[1],a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(4),.LUT(16'h5082)) c1(.A({a[2],a[1],a[0],v}),.Y(y[1]));
 \$lut #(.WIDTH(4),.LUT(16'h1565)) c2(.A({a[2],a[1],a[0],u}),.Y(y[2]));
endmodule
