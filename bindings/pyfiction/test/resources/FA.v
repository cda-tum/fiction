module top(x, y, cin, s, cout);
input x, y, cin;
output s, cout;

wire w1, w2, w3;

assign w1 = x & y;
assign w2 = x ^ y;
assign w3 = cin & w2;
assign cout = w1 | w3;
assign s = cin ^ w2;

endmodule
