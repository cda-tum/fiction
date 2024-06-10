// Benchmark "top" written by ABC on Mon Feb 19 11:52:43 2024

module top ( 
    pa, pb, pc, pd, pe, pf, pg, pi, pj, pk, pl, pm, pn, po,
    pp, pq, pr, ps, pt, pu, pv, pw, px, py, pz  );
  input  pa, pb, pc, pd, pe, pf, pg, pi, pj, pk, pl, pm, pn, po;
  output pp, pq, pr, ps, pt, pu, pv, pw, px, py, pz;
  wire new_n26, new_n27, new_n28, new_n29, new_n30, new_n31, new_n32,
    new_n33, new_n34, new_n35, new_n36, new_n38, new_n39, new_n40, new_n41,
    new_n42, new_n44, new_n45, new_n46, new_n47, new_n49, new_n50, new_n51,
    new_n52, new_n54, new_n55, new_n56, new_n58, new_n59, new_n60, new_n61,
    new_n62, new_n63, new_n64, new_n65, new_n66, new_n67, new_n68, new_n69,
    new_n70, new_n71, new_n72, new_n73, new_n74, new_n75, new_n76, new_n77,
    new_n78, new_n80, new_n81, new_n82, new_n84, new_n85, new_n86, new_n89,
    new_n90, new_n91;
  assign new_n26 = pe & pf;
  assign new_n27 = ~pc & ~pf;
  assign new_n28 = pc & ~pe;
  assign new_n29 = pc & pf;
  assign new_n30 = ~pc & pe;
  assign new_n31 = ~pe & ~pf;
  assign new_n32 = ~new_n29 & ~new_n30;
  assign new_n33 = ~new_n31 & new_n32;
  assign new_n34 = ~new_n26 & ~new_n27;
  assign new_n35 = ~new_n28 & new_n34;
  assign new_n36 = new_n33 & new_n35;
  assign pq = ~pd & new_n36;
  assign new_n38 = ~pa & ~pc;
  assign new_n39 = ~pb & ~pd;
  assign new_n40 = new_n38 & new_n39;
  assign new_n41 = pf & new_n40;
  assign new_n42 = ~pe & new_n41;
  assign pr = ~po & new_n42;
  assign new_n44 = pa & ~pc;
  assign new_n45 = new_n39 & new_n44;
  assign new_n46 = pf & new_n45;
  assign new_n47 = ~pe & new_n46;
  assign ps = ~po & new_n47;
  assign new_n49 = pb & ~pd;
  assign new_n50 = new_n38 & new_n49;
  assign new_n51 = pf & new_n50;
  assign new_n52 = ~pe & new_n51;
  assign pt = ~po & new_n52;
  assign new_n54 = new_n44 & new_n49;
  assign new_n55 = pf & new_n54;
  assign new_n56 = ~pe & new_n55;
  assign pu = ~po & new_n56;
  assign new_n58 = pc & po;
  assign new_n59 = ~pd & ~new_n29;
  assign new_n60 = pe & ~new_n58;
  assign new_n61 = new_n59 & new_n60;
  assign new_n62 = pb & pl;
  assign new_n63 = ~pa & new_n62;
  assign new_n64 = ~pb & pk;
  assign new_n65 = pa & new_n64;
  assign new_n66 = pb & pm;
  assign new_n67 = pa & new_n66;
  assign new_n68 = ~pb & pj;
  assign new_n69 = ~pa & new_n68;
  assign new_n70 = pf & ~new_n69;
  assign new_n71 = ~pn & po;
  assign new_n72 = new_n70 & new_n71;
  assign new_n73 = ~pi & new_n72;
  assign new_n74 = ~new_n63 & ~new_n65;
  assign new_n75 = ~new_n67 & new_n73;
  assign new_n76 = new_n74 & new_n75;
  assign new_n77 = new_n61 & new_n76;
  assign new_n78 = pc & new_n61;
  assign pv = new_n77 | new_n78;
  assign new_n80 = ~pc & ~pe;
  assign new_n81 = ~pd & pf;
  assign new_n82 = new_n80 & new_n81;
  assign pw = ~po & new_n82;
  assign new_n84 = pf & ~pn;
  assign new_n85 = po & new_n84;
  assign new_n86 = new_n61 & new_n85;
  assign px = new_n78 | new_n86;
  assign py = pg & po;
  assign new_n89 = ~pd & pg;
  assign new_n90 = ~pf & new_n89;
  assign new_n91 = ~pc & new_n89;
  assign pz = new_n90 | new_n91;
  assign pp = ~pq;
endmodule


