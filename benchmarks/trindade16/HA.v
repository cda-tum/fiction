module top(x, y, s, cout);
input x, y;
output s, cout;
wire w1, w2;

assign cout = x & y;

assign w1 = ~x & y;
assign w2 = x & ~y;
assign s = w1 | w2;

endmodule
