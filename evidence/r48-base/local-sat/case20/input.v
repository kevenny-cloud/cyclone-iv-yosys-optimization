module t(input[5:0] a,output[1:0] y);wire u,v;
 \$lut #(.WIDTH(4),.LUT(16'ha44d)) root(.A(a[3:0]),.Y(u));
\$lut #(.WIDTH(4),.LUT(16'h5bb2)) opposite(.A(a[3:0]),.Y(v));
 \$lut #(.WIDTH(4),.LUT(16'h109b)) c0(.A({a[2],a[1],a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(4),.LUT(16'h1613)) c1(.A({a[2],a[1],a[0],v}),.Y(y[1]));
endmodule
