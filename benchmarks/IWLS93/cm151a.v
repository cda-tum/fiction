// Benchmark "top" written by ABC on Mon Feb 19 11:52:42 2024

module top ( 
    pa, pb, pc, pd, pe, pf, pg, ph, pi, pj, pk, pl,
    pm, pn  );
  input  pa, pb, pc, pd, pe, pf, pg, ph, pi, pj, pk, pl;
  output pm, pn;
  wire new_n15, new_n16, new_n17, new_n18, new_n19, new_n20, new_n21,
    new_n22, new_n23, new_n24, new_n25, new_n26, new_n27, new_n28, new_n29,
    new_n30, new_n31, new_n32, new_n33, new_n34, new_n35, new_n36, new_n37,
    new_n38, new_n39, new_n40, new_n41, new_n42, new_n43, new_n44, new_n45,
    new_n46, new_n47, new_n48, new_n49;
  assign new_n15 = ~ph & pi;
  assign new_n16 = ~pg & ~pi;
  assign new_n17 = ~pg & ~ph;
  assign new_n18 = ~new_n15 & ~new_n16;
  assign new_n19 = ~new_n17 & new_n18;
  assign new_n20 = pj & new_n19;
  assign new_n21 = ~pf & pi;
  assign new_n22 = ~pe & ~pi;
  assign new_n23 = ~pe & ~pf;
  assign new_n24 = ~new_n21 & ~new_n22;
  assign new_n25 = ~new_n23 & new_n24;
  assign new_n26 = new_n19 & new_n25;
  assign new_n27 = ~pj & new_n25;
  assign new_n28 = ~new_n20 & ~new_n26;
  assign new_n29 = ~new_n27 & new_n28;
  assign new_n30 = pk & new_n29;
  assign new_n31 = ~pd & pi;
  assign new_n32 = ~pc & ~pi;
  assign new_n33 = ~pc & ~pd;
  assign new_n34 = ~new_n31 & ~new_n32;
  assign new_n35 = ~new_n33 & new_n34;
  assign new_n36 = pj & new_n35;
  assign new_n37 = ~pb & pi;
  assign new_n38 = ~pa & ~pi;
  assign new_n39 = ~pa & ~pb;
  assign new_n40 = ~new_n37 & ~new_n38;
  assign new_n41 = ~new_n39 & new_n40;
  assign new_n42 = new_n35 & new_n41;
  assign new_n43 = ~pj & new_n41;
  assign new_n44 = ~new_n36 & ~new_n42;
  assign new_n45 = ~new_n43 & new_n44;
  assign new_n46 = new_n29 & new_n45;
  assign new_n47 = ~pk & new_n45;
  assign new_n48 = ~new_n30 & ~new_n46;
  assign new_n49 = ~new_n47 & new_n48;
  assign pm = ~pl & new_n49;
  assign pn = ~pm;
endmodule


