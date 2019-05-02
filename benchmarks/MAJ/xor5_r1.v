module top(x0, x1, x2, x3, x4, y0);
  input x0, x1, x2, x3, x4;
  output y0;
  wire n6, n7, n8, n9, n10, n11;
  assign n6 = (~x0 & x1) | (~x0 & x2) | (x1 & x2);
  assign n7 = (x1 & x2) | (x1 & ~n6) | (x2 & ~n6);
  assign n8 = (x0 & n6) | (x0 & ~n7) | (n6 & ~n7);
  assign n9 = (~x3 & x4) | (~x3 & n8) | (x4 & n8);
  assign n10 = (x4 & n8) | (x4 & ~n9) | (n8 & ~n9);
  assign n11 = (x3 & n9) | (x3 & ~n10) | (n9 & ~n10);
  assign y0 = n11;
endmodule
