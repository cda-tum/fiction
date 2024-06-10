// Benchmark "top" written by ABC on Mon Feb 19 11:52:42 2024

module top ( 
    pp, pa, pb, pc, pd, pe, pf, pg, ph, pi, pj, pk, pl, pm, pn, po,
    pq, pr, ps, pt, pu  );
  input  pp, pa, pb, pc, pd, pe, pf, pg, ph, pi, pj, pk, pl, pm, pn, po;
  output pq, pr, ps, pt, pu;
  wire new_n22, new_n23, new_n24, new_n25, new_n26, new_n27, new_n28,
    new_n29, new_n30, new_n31, new_n32, new_n33, new_n35, new_n36, new_n37,
    new_n38, new_n39, new_n40, new_n41, new_n42, new_n43, new_n45, new_n46,
    new_n47, new_n48, new_n49, new_n50, new_n51, new_n52, new_n53, new_n54,
    new_n56, new_n57, new_n58, new_n59, new_n60, new_n61, new_n62, new_n63,
    new_n64, new_n65, new_n66, new_n68, new_n69, new_n70;
  assign new_n22 = pe & pf;
  assign new_n23 = ~pe & pf;
  assign new_n24 = ~new_n22 & ~new_n23;
  assign new_n25 = pc & pd;
  assign new_n26 = ~pj & ~new_n25;
  assign new_n27 = pj & new_n25;
  assign new_n28 = ~new_n26 & ~new_n27;
  assign new_n29 = ~new_n23 & new_n28;
  assign new_n30 = ~pa & ~new_n22;
  assign new_n31 = ~pa & new_n28;
  assign new_n32 = ~new_n30 & ~new_n31;
  assign new_n33 = ~new_n24 & ~new_n29;
  assign pq = ~new_n32 | ~new_n33;
  assign new_n35 = ~pj & new_n25;
  assign new_n36 = ~pl & ~new_n35;
  assign new_n37 = pl & new_n35;
  assign new_n38 = ~new_n36 & ~new_n37;
  assign new_n39 = ~new_n23 & new_n38;
  assign new_n40 = ~pb & ~new_n22;
  assign new_n41 = ~pb & new_n38;
  assign new_n42 = ~new_n40 & ~new_n41;
  assign new_n43 = ~new_n24 & ~new_n39;
  assign pr = ~new_n42 | ~new_n43;
  assign new_n45 = ~pj & ~pl;
  assign new_n46 = new_n25 & new_n45;
  assign new_n47 = ~pm & ~new_n46;
  assign new_n48 = pm & new_n46;
  assign new_n49 = ~new_n47 & ~new_n48;
  assign new_n50 = ~new_n23 & new_n49;
  assign new_n51 = ~pg & ~new_n22;
  assign new_n52 = ~pg & new_n49;
  assign new_n53 = ~new_n51 & ~new_n52;
  assign new_n54 = ~new_n24 & ~new_n50;
  assign ps = ~new_n53 | ~new_n54;
  assign new_n56 = ~pl & new_n25;
  assign new_n57 = ~pj & new_n56;
  assign new_n58 = ~pm & new_n57;
  assign new_n59 = ~pn & ~new_n58;
  assign new_n60 = pn & new_n58;
  assign new_n61 = ~new_n59 & ~new_n60;
  assign new_n62 = ~new_n23 & new_n61;
  assign new_n63 = ~ph & ~new_n22;
  assign new_n64 = ~ph & new_n61;
  assign new_n65 = ~new_n63 & ~new_n64;
  assign new_n66 = ~new_n24 & ~new_n62;
  assign pt = ~new_n65 | ~new_n66;
  assign new_n68 = pi & po;
  assign new_n69 = pp & pk;
  assign new_n70 = new_n68 & new_n69;
  assign pu = pd & new_n70;
endmodule


