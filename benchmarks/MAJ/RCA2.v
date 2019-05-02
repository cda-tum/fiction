module top(x0, x1, x2, x3, x4, y0, y1, y2);
  input x0, x1, x2, x3, x4;
  output y0, y1, y2;
  wire n6, n7, n8, n9, n10, n11, n12, n13, n14, n15, n16;
  assign n6 = x0 | x2;
  assign n7 = (x0 & x2) | (x0 & ~n6) | (x2 & ~n6);
  assign n8 = n6 & ~n7;
  assign n9 = x4 & n8;
  assign n10 = (x4 & n8) | (x4 & ~n9) | (n8 & ~n9);
  assign n11 = ~n9 & n10;
  assign n12 = x1 & n9;
  assign n13 = x1 & x3;
  assign n14 = (~x1 & n9) | (~x1 & n13) | (n9 & n13);
  assign n15 = (x1 & ~n12) | (x1 & n14) | (~n12 & n14);
  assign n16 = (x1 & x4) | (x1 & n13) | (x4 & n13);
  assign y0 = n11;
  assign y1 = n15;
  assign y2 = n16;
endmodule
