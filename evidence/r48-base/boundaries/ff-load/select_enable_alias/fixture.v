module t(input wire clk,a,b,s,ena,clrn,  output wire q);
wire d; 
 \$_MUX_ mux (.A(a),.B(b),.S(ena),.Y(d));
dffeas ff (.clk(clk),.d(d),.q(q),.clrn(clrn),.prn(1'b1),.ena(ena),.asdata(1'b0),.aload(1'b0),.sclr(1'b0),.sload(1'b0));

endmodule
