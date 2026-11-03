module t(input[5:0] a,output[1:0] y);wire u,v;
 \$lut #(.WIDTH(3),.LUT(8'he0)) root(.A(a[2:0]),.Y(u));
\$lut #(.WIDTH(3),.LUT(8'h1f)) opposite(.A(a[2:0]),.Y(v));
 \$lut #(.WIDTH(3),.LUT(8'hdb)) c0(.A({a[1],a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(3),.LUT(8'hf4)) c1(.A({a[1],a[0],v}),.Y(y[1]));
endmodule
