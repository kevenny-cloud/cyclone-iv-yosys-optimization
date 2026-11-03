module fresh_09(input ca,cb,rst,en,input[3:0] s,input[4:0] a,b,c,d,output[4:0] y);
reg[4:0] qa,qb;reg[4:0] x;initial qa=0;initial qb=1;
always @* case(s[1:0])0:x=a^qb;1:x=b+qa;2:x=c-qb;default:x=d;endcase
always @(posedge ca)if(rst)qa<=0;else if(en)qa<=x;
always @(negedge cb)if(rst)qb<=0;else if(en)qb<=qa^a;
assign y=qa^qb;endmodule
