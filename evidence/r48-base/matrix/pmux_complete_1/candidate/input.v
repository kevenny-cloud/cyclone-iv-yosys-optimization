module top(input [1:0] sel, input [127:0] data, output [63:0] y);
wire [1:0] s0;
\$eq #(.A_WIDTH(1),.B_WIDTH(1),.Y_WIDTH(1),.A_SIGNED(0),.B_SIGNED(0)) eq_0_0 (.A(sel[0 +: 1]),.B(1'd0),.Y(s0[0]));
\$eq #(.A_WIDTH(1),.B_WIDTH(1),.Y_WIDTH(1),.A_SIGNED(0),.B_SIGNED(0)) eq_0_1 (.A(sel[0 +: 1]),.B(1'd1),.Y(s0[1]));
\$pmux #(.WIDTH(16),.S_WIDTH(2)) p0 (.A(16'b0),.B({{data[24 +: 8],data[16 +: 8]},{data[8 +: 8],data[0 +: 8]}}),.S(s0),.Y(y[0 +: 16]));
wire [1:0] s1;
\$eq #(.A_WIDTH(1),.B_WIDTH(1),.Y_WIDTH(1),.A_SIGNED(0),.B_SIGNED(0)) eq_1_0 (.A(sel[0 +: 1]),.B(1'd0),.Y(s1[0]));
\$eq #(.A_WIDTH(1),.B_WIDTH(1),.Y_WIDTH(1),.A_SIGNED(0),.B_SIGNED(0)) eq_1_1 (.A(sel[0 +: 1]),.B(1'd1),.Y(s1[1]));
\$pmux #(.WIDTH(16),.S_WIDTH(2)) p1 (.A(16'b0),.B({{data[56 +: 8],data[16 +: 8]},{data[40 +: 8],data[0 +: 8]}}),.S(s1),.Y(y[16 +: 16]));
wire [1:0] s2;
\$eq #(.A_WIDTH(1),.B_WIDTH(1),.Y_WIDTH(1),.A_SIGNED(0),.B_SIGNED(0)) eq_2_0 (.A(sel[1 +: 1]),.B(1'd0),.Y(s2[0]));
\$eq #(.A_WIDTH(1),.B_WIDTH(1),.Y_WIDTH(1),.A_SIGNED(0),.B_SIGNED(0)) eq_2_1 (.A(sel[1 +: 1]),.B(1'd1),.Y(s2[1]));
\$pmux #(.WIDTH(16),.S_WIDTH(2)) p2 (.A(16'b0),.B({{data[88 +: 8],data[80 +: 8]},{data[72 +: 8],data[64 +: 8]}}),.S(s2),.Y(y[32 +: 16]));
wire [1:0] s3;
\$eq #(.A_WIDTH(1),.B_WIDTH(1),.Y_WIDTH(1),.A_SIGNED(0),.B_SIGNED(0)) eq_3_0 (.A(sel[1 +: 1]),.B(1'd0),.Y(s3[0]));
\$eq #(.A_WIDTH(1),.B_WIDTH(1),.Y_WIDTH(1),.A_SIGNED(0),.B_SIGNED(0)) eq_3_1 (.A(sel[1 +: 1]),.B(1'd1),.Y(s3[1]));
\$pmux #(.WIDTH(16),.S_WIDTH(2)) p3 (.A(16'b0),.B({{data[120 +: 8],data[80 +: 8]},{data[104 +: 8],data[64 +: 8]}}),.S(s3),.Y(y[48 +: 16]));
endmodule
