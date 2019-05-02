module top(x0, x1, x2, y0);
  input x0, x1, x2;
  output y0;
  wire n4;
  assign n4 = (x0 & x1) | (x0 & ~x2) | (x1 & ~x2);
  assign y0 = n4;
endmodule
