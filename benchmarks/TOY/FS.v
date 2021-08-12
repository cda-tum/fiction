module top(x, y, bin, diff, bout);
input x, y, bin;
output diff, bout;

wire w1, w2, w3;

assign w1 = x ^ y;
assign w2 = ~x & y;
assign w3 = bin & ~w1;

assign diff = w1 ^ bin;
assign bout = w3 | w2;

endmodule
