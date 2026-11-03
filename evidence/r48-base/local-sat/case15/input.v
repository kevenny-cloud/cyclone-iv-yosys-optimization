module t(input[5:0] a,output[1:0] y);wire u,v;
 \$lut #(.WIDTH(2),.LUT(4'ha)) root(.A(a[1:0]),.Y(u));
\$lut #(.WIDTH(2),.LUT(4'h5)) opposite(.A(a[1:0]),.Y(v));
(* keep *) \$lut #(.WIDTH(2),.LUT(4'h4)) c0(.A({a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(2),.LUT(4'h4)) c1(.A({a[0],v}),.Y(y[1]));
endmodule
