module top(x, y, cin, s, cout);
input x, y, cin;
output s, cout;

wire w1, w11, w12, w2, w3;

assign w1 = x & y;
assign w11 = ~x & ~y;
assign w12 = x & y;
assign w2 = ~w11 & ~w12;
assign w3 = cin & w2;
assign cout = w1 | w3;
assign s1 = ~cin & ~w2;
assign s2 = cin & w2;
assign s = ~s1 & ~s2;

endmodule
