(* abc9_box, blackbox *)module bv #(parameter P=1)(input[1:0] a,b,output[1:0] y,z);
specify(a *> y)=P;(b *> y)=P;(a *> z)=P;(b *> z)=P;endspecify endmodule
module top(input[7:0] a,output[3:0] y);wire[1:0] u,v;bv #(.P(2)) box(.a(a[1:0]^a[3:2]),.b(a[5:4]),.y(u),.z(v));assign y={u^a[7:6],v^a[3:2]};endmodule
