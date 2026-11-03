(* abc9_box, blackbox *) module box1(input i,output o);specify(i=>o)=1;endspecify endmodule
module top(input[3:0] a,output y);wire z;box1 u(.i(a[0]),.o(z));assign y=a[1]^(a[2]&a[3]);endmodule
