module top(input [6:0] s, input [62:0] d, output [8:0] y);
wire [8:0] v0 = 9'b0;
wire [8:0] v1 = s[0] ? d[0 +: 9] : v0;
wire [8:0] v2 = s[1] ? d[9 +: 9] : v1;
wire [8:0] v3 = s[2] ? d[18 +: 9] : v2;
wire [8:0] v4 = s[3] ? d[27 +: 9] : v3;
wire [8:0] v5 = s[4] ? d[36 +: 9] : v4;
wire [8:0] v6 = s[5] ? d[45 +: 9] : v5;
wire [8:0] v7 = s[6] ? d[54 +: 9] : v6;
assign y = v7;
endmodule
