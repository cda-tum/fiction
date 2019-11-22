module hs(in0, in1, diff, bout);
input in0, in1;
output diff, bout;

wire w1, w2, w3, w4;

assign w1 = ~in0;
assign bout = w1 & in1;

assign w2 = in0 & in1;
assign w3 = in0 | in1;
assign w4 = ~w2;
assign diff = w3 | w4;

endmodule
