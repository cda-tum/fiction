// Benchmark "top" written by ABC on Mon Feb 19 11:52:42 2024

module top ( 
    pp, pq, pr, ps, pt, pu, pv, pa, pb, pc, pd, pe, pf, pg, ph, pi, pj, pk,
    pl, pm, po,
    pa0, pb0, pc0, pd0, pe0, pf0, pg0, pw, ph0, px, pi0, py, pj0, pz, pk0,
    pl0, pm0, pn0, po0, pp0  );
  input  pp, pq, pr, ps, pt, pu, pv, pa, pb, pc, pd, pe, pf, pg, ph, pi,
    pj, pk, pl, pm, po;
  output pa0, pb0, pc0, pd0, pe0, pf0, pg0, pw, ph0, px, pi0, py, pj0, pz,
    pk0, pl0, pm0, pn0, po0, pp0;
  wire new_n44, new_n45, new_n47, new_n48, new_n49, new_n50, new_n51,
    new_n52, new_n53, new_n54, new_n56, new_n57, new_n58, new_n60, new_n61,
    new_n62, new_n63, new_n64, new_n66, new_n67, new_n68, new_n70, new_n71,
    new_n72, new_n73, new_n74, new_n75, new_n76, new_n77, new_n78, new_n80,
    new_n81, new_n82, new_n83, new_n84, new_n86, new_n87, new_n88, new_n89,
    new_n91, new_n92, new_n93, new_n94, new_n96, new_n97, new_n98, new_n99,
    new_n101, new_n102, new_n103, new_n104;
  assign pf0 = pi & pj;
  assign pw = pv & pl;
  assign new_n44 = ~pp & pi;
  assign new_n45 = pk & new_n44;
  assign px = pq & new_n45;
  assign new_n47 = ~pq & pi;
  assign new_n48 = pk & new_n47;
  assign new_n49 = ~pa & new_n48;
  assign new_n50 = pm & ~new_n49;
  assign new_n51 = pi & pk;
  assign new_n52 = ~pq & new_n51;
  assign new_n53 = new_n50 & new_n52;
  assign new_n54 = po & new_n50;
  assign pi0 = new_n53 | new_n54;
  assign new_n56 = ~pl & pm;
  assign new_n57 = new_n52 & new_n56;
  assign new_n58 = pp & new_n56;
  assign py = new_n57 | new_n58;
  assign new_n60 = pb & new_n48;
  assign new_n61 = pm & ~new_n60;
  assign new_n62 = pq & new_n51;
  assign new_n63 = new_n61 & new_n62;
  assign new_n64 = pp & new_n61;
  assign pj0 = new_n63 | new_n64;
  assign new_n66 = pi & pm;
  assign new_n67 = ~pp & pk;
  assign new_n68 = new_n66 & new_n67;
  assign pz = pq & new_n68;
  assign new_n70 = ~pq & ~pi;
  assign new_n71 = ~pq & ~pk;
  assign new_n72 = ~new_n70 & ~new_n71;
  assign new_n73 = pm & new_n72;
  assign new_n74 = ~pi & new_n73;
  assign new_n75 = ~pq & pc;
  assign new_n76 = new_n73 & new_n75;
  assign new_n77 = ~pk & new_n73;
  assign new_n78 = ~new_n74 & ~new_n76;
  assign pk0 = new_n77 | ~new_n78;
  assign new_n80 = ~pd & new_n48;
  assign new_n81 = pm & ~new_n80;
  assign new_n82 = pr & new_n81;
  assign new_n83 = ~pp & new_n82;
  assign new_n84 = new_n52 & new_n81;
  assign pl0 = new_n83 | new_n84;
  assign new_n86 = ~pe & new_n48;
  assign new_n87 = pm & ~new_n86;
  assign new_n88 = new_n52 & new_n87;
  assign new_n89 = ps & new_n87;
  assign pm0 = new_n88 | new_n89;
  assign new_n91 = ~pf & new_n48;
  assign new_n92 = pm & ~new_n91;
  assign new_n93 = new_n52 & new_n92;
  assign new_n94 = pt & new_n92;
  assign pn0 = new_n93 | new_n94;
  assign new_n96 = ~pg & new_n48;
  assign new_n97 = pm & ~new_n96;
  assign new_n98 = new_n52 & new_n97;
  assign new_n99 = pu & new_n97;
  assign po0 = new_n98 | new_n99;
  assign new_n101 = ~ph & new_n48;
  assign new_n102 = pm & ~new_n101;
  assign new_n103 = new_n52 & new_n102;
  assign new_n104 = pv & new_n102;
  assign pp0 = new_n103 | new_n104;
  assign pa0 = ~pt;
  assign pg0 = ~pf0;
  assign pb0 = pu;
  assign pc0 = pq;
  assign pd0 = ps;
  assign pe0 = pr;
  assign ph0 = pp;
endmodule


