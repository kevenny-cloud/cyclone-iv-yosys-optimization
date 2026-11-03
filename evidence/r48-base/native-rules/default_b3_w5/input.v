module t(input[2:0] sel,input[39:0] d,output[4:0] y);
wire[2:0] s=sel;
reg[4:0] z;always @* case(s)
3'd0: z=d[0 +: 5];
3'd1: z=d[5 +: 5];
3'd2: z=d[10 +: 5];
3'd3: z=d[15 +: 5];
3'd4: z=d[20 +: 5];
3'd5: z=d[25 +: 5];
3'd6: z=d[30 +: 5];
default:z=d[35 +: 5];
endcase
assign y=z;endmodule
