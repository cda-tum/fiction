module top(x0, x1, x2, y0);
  input x0, x1, x2;
  output y0;
  wire n4, n5, n6;
  assign n4 = x0 | x2;
  assign n5 = ~x1 & x2;
  assign n6 = n4 & ~n5;
  assign y0 = n6;
endmodule
