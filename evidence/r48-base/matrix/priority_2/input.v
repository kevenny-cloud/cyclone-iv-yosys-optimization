module top(input [1:0] s, input [31:0] d, output [15:0] y);
wire [15:0] v0 = 16'b0;
wire [15:0] v1 = s[0] ? d[0 +: 16] : v0;
wire [15:0] v2 = s[1] ? d[16 +: 16] : v1;
assign y = v2;
endmodule
