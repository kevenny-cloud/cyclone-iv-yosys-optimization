module top(input [4:0] a,output y,z);
wire u,v,w,alias_u,alias_v;
\$lut #(.WIDTH(3),.LUT(8'h96)) l0(.A(a[2:0]),.Y(u));
assign alias_u=u;
\$lut #(.WIDTH(3),.LUT(8'he8)) l1(.A({a[3:2],alias_u}),.Y(v));
assign alias_v=v;
\$lut #(.WIDTH(3),.LUT(8'h96)) l2(.A({a[4],alias_v,a[0]}),.Y(w));
assign y=w;assign z=alias_u;
endmodule
