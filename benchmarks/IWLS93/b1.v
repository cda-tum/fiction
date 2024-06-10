// Benchmark "top" written by ABC on Mon Feb 19 11:52:42 2024

module top ( 
    pa, pb, pc,
    pd, pe, pf, pg  );
  input  pa, pb, pc;
  output pd, pe, pf, pg;
  wire new_n8, new_n9, new_n11, new_n12, new_n13, new_n14, new_n15, new_n16,
    new_n17, new_n18, new_n19, new_n20;
  assign new_n8 = ~pa & ~pb;
  assign new_n9 = pa & pb;
  assign pe = ~new_n8 & ~new_n9;
  assign new_n11 = pb & pc;
  assign new_n12 = ~pa & ~pc;
  assign new_n13 = pa & ~pb;
  assign new_n14 = pa & pc;
  assign new_n15 = ~pa & pb;
  assign new_n16 = ~pb & ~pc;
  assign new_n17 = ~new_n14 & ~new_n15;
  assign new_n18 = ~new_n16 & new_n17;
  assign new_n19 = ~new_n11 & ~new_n12;
  assign new_n20 = ~new_n13 & new_n19;
  assign pf = new_n18 & new_n20;
  assign pg = ~pc;
  assign pd = pc;
endmodule


