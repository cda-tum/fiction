// Benchmark "top" written by ABC on Mon Feb 19 11:52:44 2024

module top ( 
    pp, pq, pr, ps, pa, pb, pc, pd, pe, pf, pg, ph, pi, pj, pk, pl, pm, pn,
    po,
    pa0, pb0, pt, pu, pv, pw, px, py, pz  );
  input  pp, pq, pr, ps, pa, pb, pc, pd, pe, pf, pg, ph, pi, pj, pk, pl,
    pm, pn, po;
  output pa0, pb0, pt, pu, pv, pw, px, py, pz;
  wire new_n29, new_n30, new_n31, new_n32, new_n33, new_n34, new_n35,
    new_n36, new_n37, new_n38, new_n39, new_n40, new_n41, new_n43, new_n44,
    new_n45, new_n46, new_n47, new_n48, new_n49, new_n51, new_n53, new_n54,
    new_n56, new_n57, new_n58, new_n59, new_n60, new_n61, new_n63, new_n64,
    new_n65, new_n66, new_n67, new_n68, new_n70, new_n71, new_n72, new_n73,
    new_n74, new_n75, new_n77, new_n78, new_n79, new_n80, new_n81, new_n82,
    new_n84, new_n85, new_n86, new_n87, new_n88, new_n89;
  assign new_n29 = pl & pm;
  assign new_n30 = pn & new_n29;
  assign new_n31 = po & new_n30;
  assign new_n32 = pp & new_n31;
  assign new_n33 = pq & new_n32;
  assign new_n34 = pj & ~pk;
  assign new_n35 = ~pi & new_n34;
  assign new_n36 = ~pr & new_n33;
  assign new_n37 = new_n35 & new_n36;
  assign new_n38 = pr & ~new_n33;
  assign new_n39 = new_n35 & new_n38;
  assign new_n40 = pg & pi;
  assign new_n41 = ~new_n37 & ~new_n39;
  assign pa0 = new_n40 | ~new_n41;
  assign new_n43 = pr & new_n33;
  assign new_n44 = ~ps & new_n43;
  assign new_n45 = new_n35 & new_n44;
  assign new_n46 = ps & ~new_n43;
  assign new_n47 = new_n35 & new_n46;
  assign new_n48 = ph & pi;
  assign new_n49 = ~new_n45 & ~new_n47;
  assign pb0 = new_n48 | ~new_n49;
  assign new_n51 = new_n35 & new_n43;
  assign pt = ps & new_n51;
  assign new_n53 = ~pl & new_n35;
  assign new_n54 = pa & pi;
  assign pu = new_n53 | new_n54;
  assign new_n56 = ~pl & pm;
  assign new_n57 = new_n35 & new_n56;
  assign new_n58 = pl & ~pm;
  assign new_n59 = new_n35 & new_n58;
  assign new_n60 = pb & pi;
  assign new_n61 = ~new_n57 & ~new_n59;
  assign pv = new_n60 | ~new_n61;
  assign new_n63 = ~pn & new_n29;
  assign new_n64 = new_n35 & new_n63;
  assign new_n65 = pn & ~new_n29;
  assign new_n66 = new_n35 & new_n65;
  assign new_n67 = pc & pi;
  assign new_n68 = ~new_n64 & ~new_n66;
  assign pw = new_n67 | ~new_n68;
  assign new_n70 = ~po & new_n30;
  assign new_n71 = new_n35 & new_n70;
  assign new_n72 = po & ~new_n30;
  assign new_n73 = new_n35 & new_n72;
  assign new_n74 = pd & pi;
  assign new_n75 = ~new_n71 & ~new_n73;
  assign px = new_n74 | ~new_n75;
  assign new_n77 = ~pp & new_n31;
  assign new_n78 = new_n35 & new_n77;
  assign new_n79 = pp & ~new_n31;
  assign new_n80 = new_n35 & new_n79;
  assign new_n81 = pe & pi;
  assign new_n82 = ~new_n78 & ~new_n80;
  assign py = new_n81 | ~new_n82;
  assign new_n84 = ~pq & new_n32;
  assign new_n85 = new_n35 & new_n84;
  assign new_n86 = pq & ~new_n32;
  assign new_n87 = new_n35 & new_n86;
  assign new_n88 = pf & pi;
  assign new_n89 = ~new_n85 & ~new_n87;
  assign pz = new_n88 | ~new_n89;
endmodule


