module fs(in0, in1, bin, diff, bout);
input in0, in1, bin;
output diff, bout;

assign x1 = in0 & in1;
assign x2 = in0 | in1;
assign x3 = ~x1;
assign w1 = x2 & x3;

assign x4 = in0 & in1;
assign x5 = in0 & in1;
assign x6 = ~x4;
assign diff = x5 & x6;

assign w2 = ~in0;
assign w3 = w2 & in1;
assign w4 = ~w1;
assign w5 = bin & w4;

assign bout = w3 | w5;

endmodule
