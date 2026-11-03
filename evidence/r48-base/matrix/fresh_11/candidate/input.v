module fresh_11(input clk,we,input[2:0] wa,ra,input[3:0] d,output[3:0] y);
reg[3:0] mem[0:7];always @(posedge clk)if(we)mem[wa]<=d;assign y=mem[ra];endmodule
