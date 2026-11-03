module t(input[2:0] sel,input[7:0] d,output[0:0] y);
wire[2:0] s=sel;
reg[0:0] z;always @* case(s)
3'd0: z=d[0 +: 1];
3'd1: z=d[1 +: 1];
3'd2: z=d[2 +: 1];
3'd3: z=d[3 +: 1];
3'd4: z=d[4 +: 1];
3'd5: z=d[5 +: 1];
3'd6: z=d[6 +: 1];
default:z=d[7 +: 1];
endcase
assign y=z;endmodule
