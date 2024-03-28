// Benchmark "top" written by ABC on Mon Feb 19 11:52:44 2024

module top ( 
    pp, pq, pa, pb, pc, pd, pe, pg, ph, pi, pj, pk, pl, pm, pn, po,
    pa0, pb0, pr, pc0, ps, pd0, pt, pu, pv, pw, px, py, pz  );
  input  pp, pq, pa, pb, pc, pd, pe, pg, ph, pi, pj, pk, pl, pm, pn, po;
  output pa0, pb0, pr, pc0, ps, pd0, pt, pu, pv, pw, px, py, pz;
  wire new_n31, new_n32, new_n33, new_n34, new_n36, new_n37, new_n38,
    new_n39, new_n40, new_n41, new_n42, new_n43, new_n45, new_n47, new_n48,
    new_n49, new_n50, new_n51, new_n52, new_n53, new_n54, new_n55, new_n56,
    new_n58, new_n59, new_n60, new_n62, new_n63, new_n64, new_n65, new_n66,
    new_n68, new_n69, new_n70, new_n71, new_n72, new_n73, new_n75, new_n76,
    new_n77, new_n78, new_n79, new_n81, new_n82, new_n83, new_n84, new_n85,
    new_n86, new_n87, new_n88;
  assign ps = pm | ~pn;
  assign new_n31 = pa & ps;
  assign new_n32 = ~pl & new_n31;
  assign new_n33 = ~pm & new_n32;
  assign new_n34 = pn & new_n32;
  assign pa0 = new_n33 | new_n34;
  assign new_n36 = pa & pk;
  assign new_n37 = ~pd & new_n36;
  assign new_n38 = ~pe & new_n36;
  assign new_n39 = ~pc & new_n36;
  assign new_n40 = ~new_n37 & ~new_n38;
  assign new_n41 = ~new_n39 & new_n40;
  assign new_n42 = ~pl & ~new_n41;
  assign new_n43 = pm & new_n42;
  assign pb0 = pn & new_n43;
  assign new_n45 = ~pb & ~pm;
  assign pr = pn | ~new_n45;
  assign new_n47 = ~pb & pn;
  assign new_n48 = pa & ~new_n47;
  assign new_n49 = ~pl & new_n48;
  assign new_n50 = pd & pe;
  assign new_n51 = pc & new_n50;
  assign new_n52 = pn & ~new_n51;
  assign new_n53 = pk & new_n52;
  assign new_n54 = pm & new_n49;
  assign new_n55 = new_n53 & new_n54;
  assign new_n56 = ~pb & new_n54;
  assign pc0 = new_n55 | new_n56;
  assign new_n58 = pa & ~pk;
  assign new_n59 = ~pl & new_n58;
  assign new_n60 = pm & new_n59;
  assign pd0 = pn & new_n60;
  assign new_n62 = pi & pj;
  assign new_n63 = pk & new_n62;
  assign new_n64 = pg & new_n63;
  assign new_n65 = ph & new_n64;
  assign new_n66 = pm & new_n65;
  assign pt = ~pn | ~new_n66;
  assign new_n68 = ~pg & pn;
  assign new_n69 = pn & ~new_n62;
  assign new_n70 = ~ph & pn;
  assign new_n71 = ~new_n68 & ~new_n69;
  assign new_n72 = ~new_n70 & new_n71;
  assign new_n73 = pk & ~new_n72;
  assign pu = ~pm | ~new_n73;
  assign new_n75 = pb & pk;
  assign new_n76 = ~new_n50 & new_n75;
  assign new_n77 = ~ps & new_n75;
  assign new_n78 = ~pc & new_n75;
  assign new_n79 = ~new_n76 & ~new_n77;
  assign px = new_n78 | ~new_n79;
  assign new_n81 = ~pc & pm;
  assign new_n82 = pe & pk;
  assign new_n83 = pn & new_n82;
  assign new_n84 = pm & ~new_n83;
  assign new_n85 = ~pd & pm;
  assign new_n86 = ~new_n81 & ~new_n84;
  assign new_n87 = ~new_n85 & new_n86;
  assign new_n88 = ~pl & ~new_n87;
  assign pz = ~pa | ~new_n88;
  assign pv = ~pp;
  assign pw = ~po;
  assign py = ~pq;
endmodule


