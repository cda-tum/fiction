module top (pi0, pi1, pi2, pi3, pi4, po0);
  input  pi0, pi1, pi2, pi3, pi4;
  output po0;
  wire n7, n8, n9, n10, n11, n12, n13, n14, n15, n16, n17;
  assign n7 = ~pi3 & pi4;
  assign n8 = pi3 & ~pi4;
  assign n9 = ~n7 & ~n8;
  assign n10 = ~pi1 & pi2;
  assign n11 = pi1 & ~pi2;
  assign n12 = ~n10 & ~n11;
  assign n13 = pi0 & ~n12;
  assign n14 = ~pi0 & n12;
  assign n15 = ~n13 & ~n14;
  assign n16 = n9 & n15;
  assign n17 = ~n9 & ~n15;
  assign po0 = n16 | n17;
endmodule


