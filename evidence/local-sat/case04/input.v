module t(input[5:0] a,output tap,output[5:0] y);wire u,v;
 \$lut #(.WIDTH(3),.LUT(8'h52)) root(.A(a[2:0]),.Y(u));
\$lut #(.WIDTH(3),.LUT(8'had)) opposite(.A(a[2:0]),.Y(v));
 \$lut #(.WIDTH(3),.LUT(8'he6)) c0(.A({a[1],a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(3),.LUT(8'h8e)) c1(.A({a[1],a[0],v}),.Y(y[1]));
 \$lut #(.WIDTH(3),.LUT(8'h7d)) c2(.A({a[1],a[0],u}),.Y(y[2]));
 \$lut #(.WIDTH(3),.LUT(8'h39)) c3(.A({a[1],a[0],v}),.Y(y[3]));
 \$lut #(.WIDTH(3),.LUT(8'h36)) c4(.A({a[1],a[0],u}),.Y(y[4]));
 \$lut #(.WIDTH(3),.LUT(8'he4)) c5(.A({a[1],a[0],v}),.Y(y[5]));
assign tap=u;
endmodule
