module top(x, y, s, cout);
input x, y;
output s, cout;

assign cout = x & y;
assign s = x ^ y;

endmodule
