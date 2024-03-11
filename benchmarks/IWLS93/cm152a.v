// Benchmark "top" written by ABC on Mon Feb 19 11:52:42 2024

module top ( 
    pa, pb, pc, pd, pe, pf, pg, ph, pi, pj, pk,
    pl  );
  input  pa, pb, pc, pd, pe, pf, pg, ph, pi, pj, pk;
  output pl;
  wire new_n13, new_n14, new_n15, new_n16, new_n17, new_n18, new_n19,
    new_n20, new_n21, new_n22, new_n23, new_n24, new_n25, new_n26, new_n27,
    new_n28, new_n29, new_n30, new_n31, new_n32, new_n33, new_n34, new_n35,
    new_n36, new_n37, new_n38, new_n39, new_n40, new_n41, new_n42;
  assign new_n13 = pd & pj;
  assign new_n14 = pi & new_n13;
  assign new_n15 = ~pk & new_n14;
  assign new_n16 = pe & ~pj;
  assign new_n17 = ~pi & new_n16;
  assign new_n18 = pk & new_n17;
  assign new_n19 = ~pi & ~pj;
  assign new_n20 = pa & new_n19;
  assign new_n21 = ~pk & new_n20;
  assign new_n22 = pc & pj;
  assign new_n23 = ~pi & new_n22;
  assign new_n24 = ~pk & new_n23;
  assign new_n25 = pi & ~pj;
  assign new_n26 = pb & new_n25;
  assign new_n27 = ~pk & new_n26;
  assign new_n28 = ph & pj;
  assign new_n29 = pi & new_n28;
  assign new_n30 = pk & new_n29;
  assign new_n31 = pf & ~pj;
  assign new_n32 = pi & new_n31;
  assign new_n33 = pk & new_n32;
  assign new_n34 = pg & pj;
  assign new_n35 = ~pi & new_n34;
  assign new_n36 = pk & new_n35;
  assign new_n37 = ~new_n33 & ~new_n36;
  assign new_n38 = ~new_n27 & ~new_n30;
  assign new_n39 = new_n37 & new_n38;
  assign new_n40 = ~new_n21 & ~new_n24;
  assign new_n41 = ~new_n15 & ~new_n18;
  assign new_n42 = new_n40 & new_n41;
  assign pl = ~new_n39 | ~new_n42;
endmodule


