module t(input[3:0] a,output[11:0] y);
(* keep *) \$lut #(.WIDTH(4),.LUT(16'h53a9)) c0(.A(a),.Y(y[0]));
 \$lut #(.WIDTH(4),.LUT(16'h5416)) c1(.A(a),.Y(y[1]));
 \$lut #(.WIDTH(4),.LUT(16'h5483)) c2(.A(a),.Y(y[2]));
 \$lut #(.WIDTH(4),.LUT(16'h54f0)) c3(.A(a),.Y(y[3]));
 \$lut #(.WIDTH(4),.LUT(16'h555d)) c4(.A(a),.Y(y[4]));
 \$lut #(.WIDTH(4),.LUT(16'h55ca)) c5(.A(a),.Y(y[5]));
 \$lut #(.WIDTH(4),.LUT(16'h5637)) c6(.A(a),.Y(y[6]));
 \$lut #(.WIDTH(4),.LUT(16'h56a4)) c7(.A(a),.Y(y[7]));
 \$lut #(.WIDTH(4),.LUT(16'h5711)) c8(.A(a),.Y(y[8]));
 \$lut #(.WIDTH(4),.LUT(16'h577e)) c9(.A(a),.Y(y[9]));
 \$lut #(.WIDTH(4),.LUT(16'h57eb)) c10(.A(a),.Y(y[10]));
 \$lut #(.WIDTH(4),.LUT(16'h5858)) c11(.A(a),.Y(y[11]));
endmodule
