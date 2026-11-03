module fresh_06(input clk,en,input[2:0] addr,output[2:0] y);
reg[2:0] data,q;initial q='d0;
always @* begin case(addr) 3'd0: data=3'h1;3'd1: data=3'h6;3'd2: data=3'h2;3'd3: data=3'h7;3'd4: data=3'h3;3'd5: data=3'h0;3'd6: data=3'h4;3'd7: data=3'h1; endcase end
always @(posedge clk)if(en)q<=data;assign y=q;endmodule
