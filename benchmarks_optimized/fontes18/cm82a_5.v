// Benchmark "top" written by ABC on Mon Nov 27 12:33:50 2023

module top ( 
    pi0, pi1, pi2, pi3, pi4,
    po0, po1, po2  );
  input  pi0, pi1, pi2, pi3, pi4;
  output po0, po1, po2;
  wire new_n9, new_n10, new_n11, new_n12, new_n13, new_n14, new_n16, new_n17,
    new_n18, new_n19, new_n20, new_n21;
  assign new_n9 = pi1 & pi2;
  assign new_n10 = ~pi1 & ~pi2;
  assign new_n11 = pi0 & ~new_n10;
  assign new_n12 = ~new_n9 & new_n11;
  assign new_n13 = ~new_n9 & ~new_n10;
  assign new_n14 = ~pi0 & ~new_n13;
  assign po0 = ~new_n12 & ~new_n14;
  assign new_n16 = pi3 & pi4;
  assign new_n17 = ~pi3 & ~pi4;
  assign new_n18 = ~new_n16 & ~new_n17;
  assign new_n19 = ~new_n9 & ~new_n11;
  assign new_n20 = new_n18 & new_n19;
  assign new_n21 = ~new_n18 & ~new_n19;
  assign po1 = new_n20 | new_n21;
  assign po2 = ~new_n17 & ~new_n20;
endmodule


