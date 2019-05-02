module top(x0, x1, x2, y0, y1);
  input x0, x1, x2;
  output y0, y1;
  wire n4, n5, n6, n7, n8, n9, n10;
  assign n4 = x1 & x2;
  assign n5 = (x1 & x2) | (x1 & ~n4) | (x2 & ~n4);
  assign n6 = ~n4 & n5;
  assign n7 = x0 & n6;
  assign n8 = x0 | n6;
  assign n9 = ~n7 & n8;
  assign n10 = n4 | n7;
  assign y0 = n9;
  assign y1 = n10;
endmodule
