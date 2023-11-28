// Benchmark "top" written by ABC on Mon Nov 27 12:32:23 2023

module top ( 
    A, B, C, D, E,
    M3, M6  );
  input  A, B, C, D, E;
  output M3, M6;
  wire new_n8, new_n9, new_n10, new_n12, new_n13, new_n14;
  assign new_n8 = B & C;
  assign new_n9 = ~B & ~C;
  assign new_n10 = A & ~new_n9;
  assign M3 = new_n8 | new_n10;
  assign new_n12 = ~D & ~M3;
  assign new_n13 = E & ~new_n12;
  assign new_n14 = D & M3;
  assign M6 = new_n13 | new_n14;
endmodule


