// Benchmark "top" written by ABC on Mon Nov 27 12:35:35 2023

module top ( 
    pi0, pi1, pi2, pi3, pi4,
    po0, po1  );
  input  pi0, pi1, pi2, pi3, pi4;
  output po0, po1;
  wire new_n8, new_n9, new_n10, new_n12;
  assign new_n8 = pi0 & pi2;
  assign new_n9 = pi2 & pi3;
  assign new_n10 = pi1 & ~new_n9;
  assign po0 = new_n8 | new_n10;
  assign new_n12 = pi4 & ~new_n9;
  assign po1 = new_n10 | new_n12;
endmodule


