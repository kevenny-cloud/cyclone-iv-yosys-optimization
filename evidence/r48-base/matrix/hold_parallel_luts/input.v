module top(input [3:0] a,output [5:0] y);
\$lut #(.WIDTH(4),.LUT(16'h6996)) l0(.A(a),.Y(y[0]));
\$lut #(.WIDTH(4),.LUT(16'h9669)) l1(.A(a),.Y(y[1]));
\$lut #(.WIDTH(4),.LUT(16'h8421)) l2(.A(a),.Y(y[2]));
\$lut #(.WIDTH(4),.LUT(16'h1248)) l3(.A(a),.Y(y[3]));
assign y[4]=a[0];assign y[5]=a[1];endmodule
