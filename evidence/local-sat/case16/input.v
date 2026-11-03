module t(input[5:0] a,output[2:0] y);wire u,v;
 \$lut #(.WIDTH(3),.LUT(8'hef)) root(.A(a[2:0]),.Y(u));
\$lut #(.WIDTH(3),.LUT(8'h10)) opposite(.A(a[2:0]),.Y(v));
 \$lut #(.WIDTH(3),.LUT(8'h96)) c0(.A({a[1],a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(3),.LUT(8'ha5)) c1(.A({a[1],a[0],v}),.Y(y[1]));
 \$lut #(.WIDTH(3),.LUT(8'hca)) c2(.A({a[1],a[0],u}),.Y(y[2]));
endmodule
