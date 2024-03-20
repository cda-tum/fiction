// Benchmark "top" written by ABC on Mon Feb 19 11:52:45 2024

module top ( 
    pa, pb, pc, pd, pe, pf, pg, ph, pi, pj,
    pp, pq, pk, pl, pm, pn, po  );
  input  pa, pb, pc, pd, pe, pf, pg, ph, pi, pj;
  output pp, pq, pk, pl, pm, pn, po;
  wire new_n18, new_n19, new_n20, new_n21, new_n22, new_n23, new_n24,
    new_n25, new_n26, new_n27, new_n28, new_n29, new_n30, new_n31, new_n32,
    new_n33, new_n34, new_n35, new_n36, new_n37, new_n39, new_n40, new_n41,
    new_n42, new_n44, new_n45, new_n46, new_n47, new_n48, new_n49, new_n50,
    new_n51, new_n52, new_n53, new_n55, new_n56, new_n57, new_n58, new_n59,
    new_n61, new_n62, new_n63, new_n65, new_n66, new_n67, new_n68, new_n69,
    new_n71, new_n72, new_n73;
  assign new_n18 = ~pe & ph;
  assign new_n19 = pd & new_n18;
  assign new_n20 = ~pj & new_n19;
  assign new_n21 = pc & ~ph;
  assign new_n22 = ~pa & new_n21;
  assign new_n23 = ~pb & new_n22;
  assign new_n24 = ~pi & new_n23;
  assign new_n25 = ~pi & ~pj;
  assign new_n26 = ~ph & pj;
  assign new_n27 = pi & new_n26;
  assign new_n28 = pf & new_n27;
  assign new_n29 = ~pb & pc;
  assign new_n30 = pj & new_n29;
  assign new_n31 = ~pa & new_n30;
  assign new_n32 = pi & new_n31;
  assign new_n33 = ph & new_n32;
  assign new_n34 = ~new_n20 & ~new_n24;
  assign new_n35 = pg & new_n34;
  assign new_n36 = ~new_n25 & ~new_n28;
  assign new_n37 = ~new_n33 & new_n36;
  assign pp = ~new_n35 | ~new_n37;
  assign new_n39 = ph & pj;
  assign new_n40 = ~pb & new_n39;
  assign new_n41 = ~pc & new_n40;
  assign new_n42 = ~pa & new_n41;
  assign pm = ~ph & new_n25;
  assign new_n44 = ~pi & pj;
  assign new_n45 = ph & new_n44;
  assign new_n46 = pe & pi;
  assign new_n47 = ~pj & new_n46;
  assign new_n48 = pd & new_n47;
  assign new_n49 = ph & new_n48;
  assign new_n50 = pg & ~new_n42;
  assign new_n51 = ~pm & new_n50;
  assign new_n52 = ~new_n28 & ~new_n45;
  assign new_n53 = ~new_n49 & new_n52;
  assign pq = ~new_n51 | ~new_n53;
  assign new_n55 = ~ph & pi;
  assign new_n56 = pi & pj;
  assign new_n57 = ~new_n45 & ~new_n56;
  assign new_n58 = ~new_n25 & ~new_n26;
  assign new_n59 = ~new_n55 & new_n58;
  assign pk = ~new_n57 | ~new_n59;
  assign new_n61 = ph & ~pj;
  assign new_n62 = ~new_n56 & ~new_n61;
  assign new_n63 = ~new_n26 & ~new_n55;
  assign pl = ~new_n62 | ~new_n63;
  assign new_n65 = ~pa & ~pb;
  assign new_n66 = ~pm & ~new_n61;
  assign new_n67 = new_n65 & new_n66;
  assign new_n68 = ~pc & ~new_n26;
  assign new_n69 = new_n57 & new_n68;
  assign pn = ~new_n67 | ~new_n69;
  assign new_n71 = ~pm & ~new_n56;
  assign new_n72 = pg & ~new_n26;
  assign new_n73 = ~new_n55 & new_n72;
  assign po = ~new_n71 | ~new_n73;
endmodule


