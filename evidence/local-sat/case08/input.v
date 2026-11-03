module t(input[5:0] a,output[4:0] y);wire u,v;
(* keep *) \$lut #(.WIDTH(4),.LUT(16'h1445)) root(.A(a[3:0]),.Y(u));
\$lut #(.WIDTH(4),.LUT(16'hebba)) opposite(.A(a[3:0]),.Y(v));
 \$lut #(.WIDTH(4),.LUT(16'h53e5)) c0(.A({a[2],a[1],a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(4),.LUT(16'hdcde)) c1(.A({a[2],a[1],a[0],v}),.Y(y[1]));
 \$lut #(.WIDTH(4),.LUT(16'h3cdb)) c2(.A({a[2],a[1],a[0],u}),.Y(y[2]));
 \$lut #(.WIDTH(4),.LUT(16'h6fea)) c3(.A({a[2],a[1],a[0],v}),.Y(y[3]));
 \$lut #(.WIDTH(4),.LUT(16'hcf25)) c4(.A({a[2],a[1],a[0],u}),.Y(y[4]));
endmodule
