module top(input clk,rst,en, input [1:0] sel,
input [4:0] d, output [14:0] y);
reg [4:0] q0 = 5'b0;
reg [4:0] q1 = {5{1'b1}};
reg [4:0] qu;
reg [4:0] t;
always @* begin
  case(sel)
    0: t=d;
    1: t={d[2:0],sel};
    2: t=~d;
    default: t=q0 ^ q1 ^ qu;
  endcase
end
always @(posedge clk or posedge rst)
  if(rst) q0 <= 0; else if(en) q0 <= t;
always @(posedge clk)
  if(rst) q1 <= {5{1'b1}}; else q1 <= t ^ q0;
always @(posedge clk) if(en) qu <= t ^ q1;
assign y={qu,q1,q0};
endmodule
