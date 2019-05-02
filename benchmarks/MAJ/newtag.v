module top(x0, x1, x2, x3, x4, x5, x6, x7, y0);
  input x0, x1, x2, x3, x4, x5, x6, x7;
  output y0;
  wire n9, n10, n11, n12, n13, n14;
  assign n9 = (~x3 & x6) | (~x3 & x7) | (x6 & x7);
  assign n10 = (x4 & x5) | (x4 & n9) | (x5 & n9);
  assign n11 = x3 & ~n10;
  assign n12 = x1 & ~x2;
  assign n13 = ~x0 & n12;
  assign n14 = ~n11 & n13;
  assign y0 = ~n14;
endmodule
