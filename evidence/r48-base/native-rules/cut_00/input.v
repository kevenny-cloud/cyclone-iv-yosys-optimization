module t(input[6:0] a,output y);wire u,v;
  \$lut #(.WIDTH(3),.LUT(8'h6c)) l(.A(a[2:0]),.Y(u));
  \$lut #(.WIDTH(4),.LUT(16'h9600)) m(.A({a[5],a[4],a[3],u}),.Y(v));
  \$lut #(.WIDTH(3),.LUT(8'h1f)) h(.A({a[1],a[0],v}),.Y(y));endmodule
  