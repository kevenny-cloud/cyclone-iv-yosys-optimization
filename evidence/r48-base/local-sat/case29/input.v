module t(input[5:0] a,output[5:0] y);wire u,v;
 \$lut #(.WIDTH(4),.LUT(16'h73e3)) root(.A(a[3:0]),.Y(u));
\$lut #(.WIDTH(4),.LUT(16'h8c1c)) opposite(.A(a[3:0]),.Y(v));
 \$lut #(.WIDTH(4),.LUT(16'he4a6)) c0(.A({a[2],a[1],a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(4),.LUT(16'h946e)) c1(.A({a[2],a[1],a[0],v}),.Y(y[1]));
 \$lut #(.WIDTH(4),.LUT(16'h8832)) c2(.A({a[2],a[1],a[0],u}),.Y(y[2]));
 \$lut #(.WIDTH(4),.LUT(16'h969c)) c3(.A({a[2],a[1],a[0],v}),.Y(y[3]));
 \$lut #(.WIDTH(4),.LUT(16'h10bd)) c4(.A({a[2],a[1],a[0],u}),.Y(y[4]));
 \$lut #(.WIDTH(4),.LUT(16'h7384)) c5(.A({a[2],a[1],a[0],v}),.Y(y[5]));
endmodule
