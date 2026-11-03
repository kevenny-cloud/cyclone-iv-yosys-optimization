module fresh_08(input clk,en,input[4:0] addr,output[6:0] y);
reg[6:0] data,q;initial q='d0;
always @* begin case(addr) 5'd0: data=7'h9;5'd1: data=7'h16;5'd2: data=7'h2a;5'd3: data=7'h37;5'd4: data=7'h4b;5'd5: data=7'h58;5'd6: data=7'h6c;5'd7: data=7'h79;5'd8: data=7'hd;5'd9: data=7'h1a;5'd10: data=7'h2e;5'd11: data=7'h3b;5'd12: data=7'h4f;5'd13: data=7'h5c;5'd14: data=7'h70;5'd15: data=7'h7d;5'd16: data=7'h11;5'd17: data=7'h1e;5'd18: data=7'h32;5'd19: data=7'h3f;5'd20: data=7'h53;5'd21: data=7'h60;5'd22: data=7'h74;5'd23: data=7'h1;5'd24: data=7'h15;5'd25: data=7'h22;5'd26: data=7'h36;5'd27: data=7'h43;5'd28: data=7'h57;5'd29: data=7'h64;5'd30: data=7'h78;5'd31: data=7'h5; endcase end
always @(posedge clk)if(en)q<=data;assign y=q;endmodule
