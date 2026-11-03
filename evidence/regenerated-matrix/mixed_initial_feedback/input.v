module top(input clk,rst,en, input [1:0] sel,
input [7:0] d, output [23:0] y);
reg [7:0] q0 = 8'b0;
reg [7:0] q1 = 8'b10100101;
reg [7:0] qu;
reg [7:0] t;
always @* begin
  case(sel)
    0: t=d;
    1: t={d[5:0],sel};
    2: t=~d;
    default: t=q0 ^ q1 ^ qu;
  endcase
end
always @(posedge clk or posedge rst)
  if(rst) q0 <= 0; else if(en) q0 <= t;
always @(posedge clk)
  if(rst) q1 <= {8{1'b1}}; else q1 <= t ^ q0;
always @(posedge clk) if(en) qu <= t ^ q1;
assign y={qu,q1,q0};
endmodule
