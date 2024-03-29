// Benchmark "top" written by ABC on Mon Feb 19 11:52:42 2024

module top ( 
    pp, pa, pb, pc, pd, pe, pf, pg, ph, pi, pj, pk, pl, pm, pn, po,
    pq, pr, ps, pt  );
  input  pp, pa, pb, pc, pd, pe, pf, pg, ph, pi, pj, pk, pl, pm, pn, po;
  output pq, pr, ps, pt;
  wire new_n21, new_n22, new_n23, new_n24, new_n25, new_n26, new_n27,
    new_n28, new_n29, new_n30, new_n32, new_n33, new_n34, new_n35, new_n36,
    new_n37, new_n38, new_n39, new_n40, new_n41, new_n42, new_n43, new_n44,
    new_n45, new_n46, new_n47, new_n48, new_n49, new_n50, new_n51, new_n52,
    new_n53, new_n54, new_n55, new_n56, new_n57, new_n58, new_n59, new_n62,
    new_n63, new_n64, new_n65, new_n66, new_n67, new_n68, new_n69, new_n70,
    new_n71;
  assign new_n21 = pf & ph;
  assign new_n22 = pg & new_n21;
  assign new_n23 = pi & new_n22;
  assign new_n24 = pc & pd;
  assign new_n25 = pe & new_n24;
  assign new_n26 = pa & new_n23;
  assign new_n27 = pb & new_n25;
  assign new_n28 = new_n26 & new_n27;
  assign new_n29 = pk & new_n28;
  assign new_n30 = pj & new_n29;
  assign pq = pl & new_n30;
  assign new_n32 = ~pj & pk;
  assign new_n33 = ~ph & pi;
  assign new_n34 = ~pi & pj;
  assign new_n35 = ~pp & pe;
  assign new_n36 = ~pm & pn;
  assign new_n37 = ~pn & po;
  assign new_n38 = ~pl & pm;
  assign new_n39 = ~new_n35 & ~new_n36;
  assign new_n40 = ~new_n37 & ~new_n38;
  assign new_n41 = new_n39 & new_n40;
  assign new_n42 = pk & new_n41;
  assign new_n43 = ~pl & new_n41;
  assign new_n44 = ~new_n42 & ~new_n43;
  assign new_n45 = ~new_n32 & ~new_n33;
  assign new_n46 = ~new_n34 & ~new_n44;
  assign new_n47 = new_n45 & new_n46;
  assign new_n48 = ~pg & new_n47;
  assign new_n49 = ~ph & new_n48;
  assign new_n50 = ~pf & new_n49;
  assign new_n51 = pf & new_n47;
  assign new_n52 = ~ph & new_n51;
  assign new_n53 = pe & new_n52;
  assign new_n54 = pe & new_n49;
  assign new_n55 = pg & new_n51;
  assign new_n56 = pe & new_n55;
  assign new_n57 = ~new_n50 & ~new_n53;
  assign new_n58 = ~new_n54 & ~new_n56;
  assign new_n59 = new_n57 & new_n58;
  assign pr = pp | new_n59;
  assign ps = ~po | new_n59;
  assign new_n62 = ~pj & ~pl;
  assign new_n63 = ~pk & new_n62;
  assign new_n64 = ~pm & new_n63;
  assign new_n65 = ~pg & ~ph;
  assign new_n66 = ~pi & new_n65;
  assign new_n67 = ~pe & new_n64;
  assign new_n68 = ~pf & new_n66;
  assign new_n69 = new_n67 & new_n68;
  assign new_n70 = ~po & new_n69;
  assign new_n71 = ~pn & new_n70;
  assign pt = ~pp & new_n71;
endmodule


