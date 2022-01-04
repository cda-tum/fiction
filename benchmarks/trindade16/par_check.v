module top(a, b, c, p, parity_check);
input a, b, c, p;
output parity_check;
wire n1, n2, w1, w2, w3, w4, w5, w6, w7, w8, w9, w10, w11, w12;

assign w1 = ~b;
assign w2 = a & w1;
assign w3 = ~a;
assign w4 = w3 & b;
assign n1 = w2 | w4;

assign w5 = ~p;
assign w6 = w5 & c;
assign w7 = ~a;
assign w8 = w7 & p;
assign n2 = w6 | w8;

assign w9 = ~n2;
assign w10 = n1 & w9;
assign w11 = ~n1;
assign w12 = w11 & n2;
assign parity_check = w10 | w12;

endmodule
