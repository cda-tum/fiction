module top (pi0, pi1, pi2, po0, po1, po2, po3);
  input  pi0, pi1, pi2;
  output po0, po1, po2, po3;
  wire n8, n9, n11, n12;
  assign n8 = pi0 & pi1;
  assign n9 = ~pi0 & ~pi1;
  assign po1 = ~n8 & ~n9;
  assign n11 = ~pi2 & n8;
  assign n12 = pi2 & n9;
  assign po2 = n11 | n12;
  assign po3 = ~pi2;
  assign po0 = pi2;
endmodule


