module top(x0, x1, x2, x3, x4, y0, y1);
  input x0, x1, x2, x3, x4;
  output y0, y1;
  wire n6, n7, n8, n9, n10;
  assign n6 = x2 & x3;
  assign n7 = x1 & ~n6;
  assign n8 = x0 & x2;
  assign n9 = n7 | n8;
  assign n10 = (x4 & ~n6) | (x4 & n7) | (~n6 & n7);
  assign y0 = n9;
  assign y1 = n10;
endmodule
