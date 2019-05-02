module top(x0, x1, x2, x3, x4, x5, y0);
  input x0, x1, x2, x3, x4, x5;
  output y0;
  wire n7, n8, n9, n10, n11, n12, n13;
  assign n7 = (x0 & x4) | (x0 & ~x5) | (x4 & ~x5);
  assign n8 = (~x1 & x4) | (~x1 & x5) | (x4 & x5);
  assign n9 = n7 & ~n8;
  assign n10 = (x3 & ~x4) | (x3 & x5) | (~x4 & x5);
  assign n11 = (x2 & x4) | (x2 & x5) | (x4 & x5);
  assign n12 = n10 & ~n11;
  assign n13 = (n9 & n10) | (n9 & ~n12) | (n10 & ~n12);
  assign y0 = n13;
endmodule
