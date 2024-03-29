// Benchmark "top" written by ABC on Mon Feb 19 11:52:42 2024

module top ( 
    pa, pb, pc, pd, pe, pf,
    pg, ph, pi, pj, pk, pl, pm, pn  );
  input  pa, pb, pc, pd, pe, pf;
  output pg, ph, pi, pj, pk, pl, pm, pn;
  wire new_n15, new_n16, new_n17, new_n18, new_n20, new_n22, new_n25,
    new_n27;
  assign new_n15 = pd & ~pe;
  assign new_n16 = ~pf & new_n15;
  assign new_n17 = ~pa & ~pc;
  assign new_n18 = ~pb & new_n16;
  assign pg = ~new_n17 | ~new_n18;
  assign new_n20 = pa & ~pc;
  assign ph = ~new_n18 | ~new_n20;
  assign new_n22 = pb & new_n16;
  assign pi = ~new_n17 | ~new_n22;
  assign pj = ~new_n20 | ~new_n22;
  assign new_n25 = ~pa & pc;
  assign pk = ~new_n18 | ~new_n25;
  assign new_n27 = pa & pc;
  assign pl = ~new_n18 | ~new_n27;
  assign pm = ~new_n22 | ~new_n25;
  assign pn = ~new_n22 | ~new_n27;
endmodule


