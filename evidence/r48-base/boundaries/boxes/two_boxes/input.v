(* abc9_box, blackbox *) module box1(input i,output o);specify(i=>o)=1;endspecify endmodule
module top(input[5:0] a,output [1:0] y);wire z,t;box1 u(.i(a[0]^a[1]),.o(z));box1 v(.i(a[2]&z),.o(t));assign y={t^(a[3]&a[4]),z^a[5]};endmodule
