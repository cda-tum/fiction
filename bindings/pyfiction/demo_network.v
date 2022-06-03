module top(x1, x2, x3, x4, f1, f2);
input x1, x2, x3, x4;
output f1, f2;

wire w1, w2;

assign w1 = x1 & x2;
assign w2 = x3 & x4;
assign f1 = w1;
assign f2 = w2;

endmodule
