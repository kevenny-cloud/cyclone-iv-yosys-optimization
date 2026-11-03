module fresh_07(input clk,en,input[3:0] addr,output[4:0] y);
reg[4:0] data,q;initial q='d0;
always @* begin case(addr) 4'd0: data=5'h9;4'd1: data=5'h16;4'd2: data=5'ha;4'd3: data=5'h17;4'd4: data=5'hb;4'd5: data=5'h18;4'd6: data=5'hc;4'd7: data=5'h19;4'd8: data=5'hd;4'd9: data=5'h1a;4'd10: data=5'he;4'd11: data=5'h1b;4'd12: data=5'hf;4'd13: data=5'h1c;4'd14: data=5'h10;4'd15: data=5'h1d; endcase end
always @(posedge clk)if(en)q<=data;assign y=q;endmodule
