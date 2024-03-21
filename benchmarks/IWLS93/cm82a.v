// Benchmark "top" written by ABC on Mon Feb 19 11:52:42 2024

module top ( 
    pa, pb, pc, pd, pe,
    pf, pg, ph  );
  input  pa, pb, pc, pd, pe;
  output pf, pg, ph;
  wire new_n9, new_n10, new_n11, new_n12, new_n13, new_n15, new_n16, new_n17,
    new_n18, new_n19, new_n20, new_n21, new_n22, new_n23, new_n24, new_n26,
    new_n27, new_n28, new_n29;
  assign new_n9 = pb & ~pc;
  assign new_n10 = ~pb & pc;
  assign new_n11 = ~new_n9 & ~new_n10;
  assign new_n12 = pa & new_n11;
  assign new_n13 = ~pa & ~new_n11;
  assign pf = new_n12 | new_n13;
  assign new_n15 = pd & ~pe;
  assign new_n16 = ~pd & pe;
  assign new_n17 = ~new_n15 & ~new_n16;
  assign new_n18 = ~pa & ~pc;
  assign new_n19 = ~pb & ~pc;
  assign new_n20 = ~pa & ~pb;
  assign new_n21 = ~new_n18 & ~new_n19;
  assign new_n22 = ~new_n20 & new_n21;
  assign new_n23 = new_n17 & new_n22;
  assign new_n24 = ~new_n17 & ~new_n22;
  assign pg = new_n23 | new_n24;
  assign new_n26 = pe & new_n22;
  assign new_n27 = pd & pe;
  assign new_n28 = pd & new_n22;
  assign new_n29 = ~new_n26 & ~new_n27;
  assign ph = new_n28 | ~new_n29;
endmodule


