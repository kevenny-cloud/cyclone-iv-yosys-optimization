module t(input[1:0] s,t,r,input[35:0] d,output[5:0] y);
reg[2:0] u;always @* case(s)
2'd0:u=d[0 +: 3];
2'd1:u=d[3 +: 3];
2'd2:u=d[6 +: 3];
default:u=d[9 +: 3];endcase
reg[2:0] v;always @* case(t)
2'd0:v=u;
2'd1:v=d[12 +: 3];
2'd2:v=d[15 +: 3];
default:v=d[18 +: 3];endcase
reg[2:0] w;always @* case(r)
2'd0:w=u;
2'd1:w=d[21 +: 3];
2'd2:w=d[24 +: 3];
default:w=d[27 +: 3];endcase
assign y={v,w};endmodule
