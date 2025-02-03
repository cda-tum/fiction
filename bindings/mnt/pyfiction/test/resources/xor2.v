module top(in0, in1, out);
input a, b;
output out;

wire w1, w2, w3;

assign w1 = a & b;
assign w2 = a | b;
assign w3 = ~w1;
assign out = w2 & w3;

endmodule
