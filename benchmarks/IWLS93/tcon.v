// Benchmark "top" written by ABC on Mon Feb 19 11:52:45 2024

module top ( 
    pp, pq, pr, pa, pb, pc, pd, pe, pf, pg, ph, pi, pk, pl, pm, pn, po,
    pa0, pb0, pc0, ps, pd0, pt, pe0, pu, pf0, pv, pg0, pw, ph0, px, py, pz  );
  input  pp, pq, pr, pa, pb, pc, pd, pe, pf, pg, ph, pi, pk, pl, pm, pn,
    po;
  output pa0, pb0, pc0, ps, pd0, pt, pe0, pu, pf0, pv, pg0, pw, ph0, px, py,
    pz;
  wire new_n34, new_n35, new_n36, new_n37, new_n39, new_n40, new_n41,
    new_n42, new_n44, new_n45, new_n46, new_n47, new_n49, new_n50, new_n51,
    new_n52, new_n54, new_n55, new_n56, new_n57, new_n59, new_n60, new_n61,
    new_n62, new_n64, new_n65, new_n66, new_n67, new_n69, new_n70, new_n71,
    new_n72;
  assign new_n34 = pa & pi;
  assign new_n35 = pa & pk;
  assign new_n36 = ~pi & pk;
  assign new_n37 = ~new_n34 & ~new_n35;
  assign pa0 = new_n36 | ~new_n37;
  assign new_n39 = pb & pi;
  assign new_n40 = pb & pl;
  assign new_n41 = ~pi & pl;
  assign new_n42 = ~new_n39 & ~new_n40;
  assign pb0 = new_n41 | ~new_n42;
  assign new_n44 = pc & pi;
  assign new_n45 = pc & pm;
  assign new_n46 = ~pi & pm;
  assign new_n47 = ~new_n44 & ~new_n45;
  assign pc0 = new_n46 | ~new_n47;
  assign new_n49 = pd & pi;
  assign new_n50 = pd & pn;
  assign new_n51 = ~pi & pn;
  assign new_n52 = ~new_n49 & ~new_n50;
  assign pd0 = new_n51 | ~new_n52;
  assign new_n54 = pe & pi;
  assign new_n55 = pe & po;
  assign new_n56 = ~pi & po;
  assign new_n57 = ~new_n54 & ~new_n55;
  assign pe0 = new_n56 | ~new_n57;
  assign new_n59 = pf & pi;
  assign new_n60 = pp & pf;
  assign new_n61 = pp & ~pi;
  assign new_n62 = ~new_n59 & ~new_n60;
  assign pf0 = new_n61 | ~new_n62;
  assign new_n64 = pg & pi;
  assign new_n65 = pq & pg;
  assign new_n66 = pq & ~pi;
  assign new_n67 = ~new_n64 & ~new_n65;
  assign pg0 = new_n66 | ~new_n67;
  assign new_n69 = ph & pi;
  assign new_n70 = pr & ph;
  assign new_n71 = pr & ~pi;
  assign new_n72 = ~new_n69 & ~new_n70;
  assign ph0 = new_n71 | ~new_n72;
  assign ps = pk;
  assign pt = pl;
  assign pu = pm;
  assign pv = pn;
  assign pw = po;
  assign px = pp;
  assign py = pq;
  assign pz = pr;
endmodule


