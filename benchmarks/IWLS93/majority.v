// Benchmark "top" written by ABC on Mon Feb 19 11:52:44 2024

module top ( 
    pa, pb, pc, pd, pe,
    pf  );
  input  pa, pb, pc, pd, pe;
  output pf;
  wire new_n7, new_n8, new_n9, new_n10, new_n11, new_n12, new_n13, new_n14,
    new_n15, new_n16, new_n17, new_n18, new_n19, new_n20, new_n21, new_n22;
  assign new_n7 = ~pa & ~pe;
  assign new_n8 = ~pd & new_n7;
  assign new_n9 = ~pb & ~pe;
  assign new_n10 = ~pd & new_n9;
  assign new_n11 = ~pa & ~pc;
  assign new_n12 = ~pd & new_n11;
  assign new_n13 = ~pb & ~pc;
  assign new_n14 = ~pd & new_n13;
  assign new_n15 = ~pa & ~pb;
  assign new_n16 = ~pd & new_n15;
  assign new_n17 = ~pc & ~pe;
  assign new_n18 = ~pd & new_n17;
  assign new_n19 = ~new_n14 & ~new_n16;
  assign new_n20 = ~new_n18 & new_n19;
  assign new_n21 = ~new_n8 & ~new_n10;
  assign new_n22 = ~new_n12 & new_n21;
  assign pf = new_n20 & new_n22;
endmodule


