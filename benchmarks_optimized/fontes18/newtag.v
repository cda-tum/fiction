// Benchmark "top" written by ABC on Mon Nov 27 12:34:55 2023

module top ( 
    pi0, pi1, pi2, pi3, pi4, pi5, pi6, pi7,
    po0  );
  input  pi0, pi1, pi2, pi3, pi4, pi5, pi6, pi7;
  output po0;
  wire new_n10, new_n11, new_n12, new_n13, new_n14, new_n15, new_n16,
    new_n17;
  assign new_n10 = pi4 & pi5;
  assign new_n11 = ~pi4 & ~pi5;
  assign new_n12 = pi6 & pi7;
  assign new_n13 = ~new_n11 & new_n12;
  assign new_n14 = pi3 & ~new_n10;
  assign new_n15 = ~new_n13 & new_n14;
  assign new_n16 = ~pi0 & pi1;
  assign new_n17 = ~pi2 & new_n16;
  assign po0 = new_n15 | ~new_n17;
endmodule


