module top(pi0, pi1, pi2, pi3, pi4, pi5, pi6, pi7, po0);
  input  pi0, pi1, pi2, pi3, pi4, pi5, pi6, pi7;
  output po0;
  wire n11, n121, n122, n123, n12, n13, n14, n15;
  assign n11 = pi6 & pi7;
  assign n121 = ~pi4 & ~pi5;
  assign n122 = ~n11 | n121;
  assign n123 = ~pi4 | ~pi5;
  assign n12 = n122 & n123;
  assign n13 = n12 & pi3;
  assign n14 = pi1 & ~pi2;
  assign n15 = n14 & ~pi0;
  assign po0 = n13 | ~n15;
endmodule


