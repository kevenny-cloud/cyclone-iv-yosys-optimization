module top(input [11:0] s, input [71:0] d, output [5:0] y);
wire [5:0] v0 = 6'b0;
wire [5:0] v1 = s[0] ? d[0 +: 6] : v0;
wire [5:0] v2 = s[1] ? d[6 +: 6] : v1;
wire [5:0] v3 = s[2] ? d[12 +: 6] : v2;
wire [5:0] v4 = s[3] ? d[18 +: 6] : v3;
wire [5:0] v5 = s[4] ? d[24 +: 6] : v4;
wire [5:0] v6 = s[5] ? d[30 +: 6] : v5;
wire [5:0] v7 = s[6] ? d[36 +: 6] : v6;
wire [5:0] v8 = s[7] ? d[42 +: 6] : v7;
wire [5:0] v9 = s[8] ? d[48 +: 6] : v8;
wire [5:0] v10 = s[9] ? d[54 +: 6] : v9;
wire [5:0] v11 = s[10] ? d[60 +: 6] : v10;
wire [5:0] v12 = s[11] ? d[66 +: 6] : v11;
assign y = v12;
endmodule
