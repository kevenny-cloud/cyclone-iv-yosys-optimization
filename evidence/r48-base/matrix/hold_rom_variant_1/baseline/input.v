module top(input clk,we, input [4:0] addr, input [23:0] din, output [23:0] y);
reg [23:0] rom_0 [0:31];
initial $readmemh("rom_0.hex", rom_0);
assign y[0 +: 24] = rom_0[addr[0 +: 5]];
endmodule
