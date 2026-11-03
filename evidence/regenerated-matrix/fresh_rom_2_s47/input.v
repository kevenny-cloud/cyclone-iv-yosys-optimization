module top(input clk,we, input [9:0] addr, input [23:0] din, output [47:0] y);
reg [23:0] rom_0 [0:31];
initial $readmemh("rom_0.hex", rom_0);
assign y[0 +: 24] = rom_0[addr[0 +: 5]];
reg [23:0] rom_1 [0:31];
initial $readmemh("rom_1.hex", rom_1);
assign y[24 +: 24] = rom_1[addr[5 +: 5]];
endmodule
