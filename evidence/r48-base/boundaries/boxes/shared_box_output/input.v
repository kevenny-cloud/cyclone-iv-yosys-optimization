(* abc9_box, blackbox *) module box1(input i,output o);specify(i=>o)=1;endspecify endmodule
module top(input[3:0] a,output[1:0] y);wire z;box1 u(.i(a[0]^a[1]),.o(z));assign y={z&a[2],z^a[3]};endmodule
