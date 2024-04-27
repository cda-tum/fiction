// Benchmark "top" written by ABC on Mon Feb 19 11:52:42 2024

module top ( 
    pa, pb, pc, pd,
    pe, pf, pg, ph, pi, pj, pk, pl, pm, pn  );
  input  pa, pb, pc, pd;
  output pe, pf, pg, ph, pi, pj, pk, pl, pm, pn;
  wire new_n15, new_n16, new_n19, new_n22, new_n23, new_n26, new_n29,
    new_n30;
  assign new_n15 = ~pc & ~pd;
  assign new_n16 = ~pb & new_n15;
  assign pe = pa | ~new_n16;
  assign pf = ~pa | ~new_n16;
  assign new_n19 = pb & new_n15;
  assign pg = pa | ~new_n19;
  assign ph = ~pa | ~new_n19;
  assign new_n22 = ~pd & ~new_n15;
  assign new_n23 = ~pb & new_n22;
  assign pi = pa | ~new_n23;
  assign pj = ~pa | ~new_n23;
  assign new_n26 = pb & new_n22;
  assign pk = pa | ~new_n26;
  assign pl = ~pa | ~new_n26;
  assign new_n29 = ~pc & pd;
  assign new_n30 = ~pb & new_n29;
  assign pm = pa | ~new_n30;
  assign pn = ~pa | ~new_n30;
endmodule


