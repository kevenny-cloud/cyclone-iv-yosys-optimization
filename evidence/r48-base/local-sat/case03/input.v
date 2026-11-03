module t(input[5:0] a,output[4:0] y);wire u,v;
(* keep *) \$lut #(.WIDTH(2),.LUT(4'he)) root(.A(a[1:0]),.Y(u));
\$lut #(.WIDTH(2),.LUT(4'h1)) opposite(.A(a[1:0]),.Y(v));
 \$lut #(.WIDTH(2),.LUT(4'he)) c0(.A({a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(2),.LUT(4'h2)) c1(.A({a[0],v}),.Y(y[1]));
 \$lut #(.WIDTH(2),.LUT(4'hf)) c2(.A({a[0],u}),.Y(y[2]));
 \$lut #(.WIDTH(2),.LUT(4'h1)) c3(.A({a[0],v}),.Y(y[3]));
 \$lut #(.WIDTH(2),.LUT(4'h7)) c4(.A({a[0],u}),.Y(y[4]));
endmodule
