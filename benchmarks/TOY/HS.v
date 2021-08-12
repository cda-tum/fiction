module top(x, y, diff, bout);
input x, y;
output diff, bout;

assign diff = x ^ y;
assign bout = ~x & y;

endmodule
