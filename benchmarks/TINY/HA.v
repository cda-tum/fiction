module ha(in0, in1, sum, cout);
input in0, in1;
output sum, cout;

wire w1, w2, w3, w4;

assign cout = in0 & in1;
assign w1 = ~in1;
assign w2 = in0 & w1;
assign w3 = ~in0;
assign w4 = w3 & in1;
assign sum = w2 | w4;

endmodule
