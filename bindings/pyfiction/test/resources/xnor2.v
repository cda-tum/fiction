module top(a, b, out);
input a, b;
output out;
wire w1, w2, w3, w4, w5;

assign w1 = ~b;
assign w2 = a & w1;
assign w3 = ~a;
assign w4 = w3 & b;
assign w5 = w2 | w4;
assign out = ~w5;

endmodule
