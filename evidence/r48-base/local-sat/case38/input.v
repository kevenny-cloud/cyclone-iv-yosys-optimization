module t(input[5:0] a,output[4:0] y);wire u,v;
 \$lut #(.WIDTH(4),.LUT(16'hc60b)) root(.A(a[3:0]),.Y(u));
\$lut #(.WIDTH(4),.LUT(16'h39f4)) opposite(.A(a[3:0]),.Y(v));
 \$lut #(.WIDTH(4),.LUT(16'hacc9)) c0(.A({a[2],a[1],a[0],u}),.Y(y[0]));
 \$lut #(.WIDTH(4),.LUT(16'h943e)) c1(.A({a[2],a[1],a[0],v}),.Y(y[1]));
 \$lut #(.WIDTH(4),.LUT(16'hc167)) c2(.A({a[2],a[1],a[0],u}),.Y(y[2]));
 \$lut #(.WIDTH(4),.LUT(16'h2771)) c3(.A({a[2],a[1],a[0],v}),.Y(y[3]));
 \$lut #(.WIDTH(4),.LUT(16'h9ec9)) c4(.A({a[2],a[1],a[0],u}),.Y(y[4]));
endmodule
