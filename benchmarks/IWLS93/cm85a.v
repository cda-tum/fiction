// Benchmark "top" written by ABC on Mon Feb 19 11:52:42 2024

module top ( 
    pa, pb, pc, pd, pe, pf, pg, ph, pi, pj, pk,
    pl, pm, pn  );
  input  pa, pb, pc, pd, pe, pf, pg, ph, pi, pj, pk;
  output pl, pm, pn;
  wire new_n15, new_n16, new_n17, new_n18, new_n19, new_n20, new_n21,
    new_n22, new_n23, new_n24, new_n25, new_n26, new_n27, new_n28, new_n29,
    new_n30, new_n31, new_n32, new_n33, new_n34, new_n35, new_n36, new_n37,
    new_n38, new_n40, new_n41, new_n43, new_n44, new_n45, new_n46, new_n47,
    new_n48, new_n49, new_n50, new_n51, new_n52, new_n53;
  assign new_n15 = ~pj & pk;
  assign new_n16 = ~ph & ~pi;
  assign new_n17 = ph & pi;
  assign new_n18 = ~new_n16 & ~new_n17;
  assign new_n19 = pf & ~pg;
  assign new_n20 = ~pf & pg;
  assign new_n21 = ~new_n19 & ~new_n20;
  assign new_n22 = ~pd & ~pe;
  assign new_n23 = pd & pe;
  assign new_n24 = ~new_n22 & ~new_n23;
  assign new_n25 = pb & ~new_n24;
  assign new_n26 = new_n21 & new_n25;
  assign new_n27 = ~new_n18 & new_n26;
  assign new_n28 = new_n15 & new_n27;
  assign new_n29 = new_n20 & new_n25;
  assign new_n30 = ~pd & pe;
  assign new_n31 = ~pa & ~new_n30;
  assign new_n32 = ~pa & ~pb;
  assign new_n33 = ~new_n31 & ~new_n32;
  assign new_n34 = ~new_n29 & ~new_n33;
  assign new_n35 = ~ph & pi;
  assign new_n36 = new_n34 & ~new_n35;
  assign new_n37 = ~new_n26 & new_n34;
  assign new_n38 = ~new_n36 & ~new_n37;
  assign pl = new_n28 | new_n38;
  assign new_n40 = pj & ~pk;
  assign new_n41 = ~new_n15 & ~new_n40;
  assign pm = new_n27 & new_n41;
  assign new_n43 = new_n27 & new_n40;
  assign new_n44 = new_n19 & new_n25;
  assign new_n45 = pd & ~pe;
  assign new_n46 = ~pc & ~new_n45;
  assign new_n47 = ~pb & ~pc;
  assign new_n48 = ~new_n46 & ~new_n47;
  assign new_n49 = ~new_n44 & ~new_n48;
  assign new_n50 = ph & ~pi;
  assign new_n51 = new_n49 & ~new_n50;
  assign new_n52 = ~new_n26 & new_n49;
  assign new_n53 = ~new_n51 & ~new_n52;
  assign pn = new_n43 | new_n53;
endmodule


