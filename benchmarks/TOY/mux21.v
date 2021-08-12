module top(in0, in1, in2, out);
input in0, in1, in2;
output out;

wire w1, w2, w3;

assign w1 = ~in2;
assign w2 = in0 & w1;
assign w3 = in1 & in2;
assign out = w2 | w3;

endmodule
