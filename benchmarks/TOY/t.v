module top(pi0, pi1, pi2, pi3, pi4, po0, po1);
  input  pi0, pi1, pi2, pi3, pi4;
  output po0, po1;
  wire n9, n10, n11, n13;
  assign n9 = pi2 & pi3;
  assign n10 = ~n9 & pi1;
  assign n11 = pi0 & pi2;
  assign po0 = n10 | n11;
  assign n13 = ~pi1 & ~pi4;
  assign po1 = ~n9 & ~n13;
endmodule


