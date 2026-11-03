module t(input[5:0] a,output[1:0] y);wire u,v;
 \$lut #(.WIDTH(3),.LUT(8'h67)) root(.A(a[2:0]),.Y(u));
\$lut #(.WIDTH(3),.LUT(8'h98)) opposite(.A(a[2:0]),.Y(v));
(* keep *) \$lut #(.WIDTH(3),.LUT(8'h9f)) c0(.A({a[1],a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(3),.LUT(8'h63)) c1(.A({a[1],a[0],v}),.Y(y[1]));
endmodule
