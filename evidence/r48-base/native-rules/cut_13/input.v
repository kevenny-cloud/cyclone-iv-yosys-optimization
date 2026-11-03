module t(input[6:0] a,output y);wire u,v;
  \$lut #(.WIDTH(4),.LUT(16'h9d01)) l(.A(a[3:0]),.Y(u));
  \$lut #(.WIDTH(1),.LUT(2'b10)) m(.A(u),.Y(v));
  \$lut #(.WIDTH(4),.LUT(16'h866e)) h(.A({a[6],a[5],a[4],v}),.Y(y));endmodule
  