module t(input[5:0] a,output[1:0] y);wire u,v;
 \$lut #(.WIDTH(4),.LUT(16'heb07)) root(.A(a[3:0]),.Y(u));
\$lut #(.WIDTH(4),.LUT(16'h14f8)) opposite(.A(a[3:0]),.Y(v));
 \$lut #(.WIDTH(4),.LUT(16'hb4af)) c0(.A({a[5],a[1],a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(4),.LUT(16'h55ae)) c1(.A({a[5],a[1],a[0],v}),.Y(y[1]));
endmodule
