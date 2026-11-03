module t(input[5:0] a,output[4:0] y);wire u,v;
(* keep *) \$lut #(.WIDTH(3),.LUT(8'h19)) root(.A(a[2:0]),.Y(u));
\$lut #(.WIDTH(3),.LUT(8'he6)) opposite(.A(a[2:0]),.Y(v));
 \$lut #(.WIDTH(3),.LUT(8'h93)) c0(.A({a[1],a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(3),.LUT(8'h91)) c1(.A({a[1],a[0],v}),.Y(y[1]));
 \$lut #(.WIDTH(3),.LUT(8'hfb)) c2(.A({a[1],a[0],u}),.Y(y[2]));
 \$lut #(.WIDTH(3),.LUT(8'h4a)) c3(.A({a[1],a[0],v}),.Y(y[3]));
 \$lut #(.WIDTH(3),.LUT(8'h79)) c4(.A({a[1],a[0],u}),.Y(y[4]));
endmodule
