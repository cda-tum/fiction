module top(a, b, c, parity);
input a, b, c;
output parity;
wire w1, w2, w3, w4, w5, w6, w7, w8, n1;

assign w1 = ~b;
assign w2 = c & w1;
assign w3 = ~c;
assign w4 = w3 & b;
assign n1 = w2 | w4;

assign w5 = ~n1;
assign w6 = a & w5;
assign w7 = ~a;
assign w8 = w7 & n1;
assign parity = w6 | w8;

endmodule
