// Benchmark "top" written by ABC on Mon Feb 19 11:52:42 2024

module top ( 
    pa, pb, pc, pd, pe, pf, pg, ph, pi, pj, pk, pl, pm, pn,
    pp, pq, pr, ps, po  );
  input  pa, pb, pc, pd, pe, pf, pg, ph, pi, pj, pk, pl, pm, pn;
  output pp, pq, pr, ps, po;
  wire new_n20, new_n21, new_n22, new_n23, new_n24, new_n25, new_n26,
    new_n27, new_n28, new_n29, new_n30, new_n31, new_n32, new_n33, new_n34,
    new_n35, new_n36, new_n37, new_n39, new_n40, new_n41, new_n42, new_n43,
    new_n44, new_n45, new_n46, new_n47, new_n48, new_n50, new_n51, new_n52,
    new_n53, new_n54, new_n55, new_n56, new_n57, new_n58, new_n59, new_n60,
    new_n63, new_n64, new_n65, new_n66, new_n67, new_n68, new_n69, new_n70;
  assign new_n20 = pd & pe;
  assign new_n21 = pc & new_n20;
  assign new_n22 = pj & pn;
  assign new_n23 = new_n21 & new_n22;
  assign new_n24 = pd & pf;
  assign new_n25 = ~new_n23 & new_n24;
  assign new_n26 = ~pd & pf;
  assign new_n27 = ~new_n23 & new_n26;
  assign new_n28 = ~new_n25 & ~new_n27;
  assign new_n29 = ~pi & new_n21;
  assign new_n30 = ~pk & ~new_n29;
  assign new_n31 = pk & new_n29;
  assign new_n32 = ~new_n30 & ~new_n31;
  assign new_n33 = ~new_n27 & new_n32;
  assign new_n34 = ~pb & ~new_n25;
  assign new_n35 = ~pb & new_n32;
  assign new_n36 = ~new_n34 & ~new_n35;
  assign new_n37 = ~new_n28 & ~new_n33;
  assign pp = ~new_n36 | ~new_n37;
  assign new_n39 = ~pi & ~pk;
  assign new_n40 = new_n21 & new_n39;
  assign new_n41 = ~pl & ~new_n40;
  assign new_n42 = pl & new_n40;
  assign new_n43 = ~new_n41 & ~new_n42;
  assign new_n44 = ~new_n27 & new_n43;
  assign new_n45 = ~pg & ~new_n25;
  assign new_n46 = ~pg & new_n43;
  assign new_n47 = ~new_n45 & ~new_n46;
  assign new_n48 = ~new_n28 & ~new_n44;
  assign pq = ~new_n47 | ~new_n48;
  assign new_n50 = ~pk & new_n21;
  assign new_n51 = ~pi & new_n50;
  assign new_n52 = ~pl & new_n51;
  assign new_n53 = ~pm & ~new_n52;
  assign new_n54 = pm & new_n52;
  assign new_n55 = ~new_n53 & ~new_n54;
  assign new_n56 = ~new_n27 & new_n55;
  assign new_n57 = ~ph & ~new_n25;
  assign new_n58 = ~ph & new_n55;
  assign new_n59 = ~new_n57 & ~new_n58;
  assign new_n60 = ~new_n28 & ~new_n56;
  assign pr = ~new_n59 | ~new_n60;
  assign ps = pe & new_n22;
  assign new_n63 = ~pi & ~new_n21;
  assign new_n64 = pi & new_n21;
  assign new_n65 = ~new_n63 & ~new_n64;
  assign new_n66 = ~new_n27 & new_n65;
  assign new_n67 = ~pa & ~new_n25;
  assign new_n68 = ~pa & new_n65;
  assign new_n69 = ~new_n67 & ~new_n68;
  assign new_n70 = ~new_n28 & ~new_n66;
  assign po = ~new_n69 | ~new_n70;
endmodule


