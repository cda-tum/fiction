module top(x0, x1, y0);
  input x0, x1;
  output y0;
  wire n3, n4, n5;
  assign n3 = x0 | x1;
  assign n4 = (x0 & x1) | (x0 & ~n3) | (x1 & ~n3);
  assign n5 = n3 & ~n4;
  assign y0 = n5;
endmodule
