// Benchmark "top" written by ABC on Mon Nov 27 12:32:53 2023

module top ( 
    pi0, pi1, pi2,
    po0, po1, po2, po3  );
  input  pi0, pi1, pi2;
  output po0, po1, po2, po3;
  wire new_n8, new_n9, new_n11, new_n12;
  assign new_n8 = pi0 & pi1;
  assign new_n9 = ~pi0 & ~pi1;
  assign po1 = ~new_n8 & ~new_n9;
  assign new_n11 = pi2 & new_n9;
  assign new_n12 = ~pi2 & new_n8;
  assign po2 = new_n11 | new_n12;
  assign po3 = ~pi2;
  assign po0 = pi2;
endmodule


