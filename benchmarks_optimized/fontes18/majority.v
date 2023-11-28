// Benchmark "top" written by ABC on Mon Nov 27 12:34:13 2023

module top ( 
    pi0, pi1, pi2, pi3, pi4,
    po0  );
  input  pi0, pi1, pi2, pi3, pi4;
  output po0;
  wire new_n7, new_n8, new_n9, new_n10, new_n11, new_n12, new_n13;
  assign new_n7 = ~pi0 & ~pi1;
  assign new_n8 = pi2 & pi4;
  assign new_n9 = ~new_n7 & new_n8;
  assign new_n10 = ~pi2 & ~pi4;
  assign new_n11 = pi0 & pi1;
  assign new_n12 = ~new_n10 & new_n11;
  assign new_n13 = ~pi3 & ~new_n9;
  assign po0 = new_n12 | ~new_n13;
endmodule


