module startup_policy(input clk,rst,en,input [1:0] sel,input a,b,c,d,output reg [1:0] y,output reg q,output reg sampled);
always @* case(sel) 2'd0:y={a,sel[0]}; 2'd1:y={b,sel[1]}; default:y={c,d}; endcase
always @(negedge clk or posedge rst) if(rst) q<=1'b1; else if(en) q<=a^b;
always @(posedge clk) if(en) sampled<=q;
endmodule
