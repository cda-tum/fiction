// Benchmark "top" written by ABC on Mon Feb 19 11:52:44 2024

module top ( 
    pp, pa0, pq, pr, ps, pt, pu, pv, pw, px, py, pz, pa, pb, pc, pd, pe,
    pf, pg, ph, pi, pj, pk, pl, pm, pn, po,
    pb0, pc0, pd0, pe0, pf0, pg0, ph0, pi0, pj0, pk0, pl0, pm0, pn0, po0,
    pp0, pq0, pr0  );
  input  pp, pa0, pq, pr, ps, pt, pu, pv, pw, px, py, pz, pa, pb, pc, pd,
    pe, pf, pg, ph, pi, pj, pk, pl, pm, pn, po;
  output pb0, pc0, pd0, pe0, pf0, pg0, ph0, pi0, pj0, pk0, pl0, pm0, pn0, po0,
    pp0, pq0, pr0;
  wire new_n45, new_n46, new_n47, new_n48, new_n49, new_n50, new_n51,
    new_n52, new_n53, new_n63, new_n64, new_n65, new_n67, new_n68, new_n69,
    new_n70, new_n71, new_n72, new_n73, new_n75, new_n76, new_n77, new_n78,
    new_n79, new_n80, new_n81, new_n83, new_n84, new_n85, new_n86, new_n87,
    new_n88, new_n89, new_n91, new_n92, new_n93, new_n94, new_n95, new_n96,
    new_n97, new_n99, new_n100, new_n101, new_n102, new_n103, new_n104,
    new_n105, new_n107, new_n108, new_n109, new_n110, new_n111, new_n112,
    new_n113, new_n115, new_n116, new_n117, new_n118, new_n119, new_n120,
    new_n121;
  assign new_n45 = pt & pu;
  assign new_n46 = pv & new_n45;
  assign new_n47 = pw & new_n46;
  assign new_n48 = px & new_n47;
  assign new_n49 = py & new_n48;
  assign new_n50 = pz & new_n49;
  assign new_n51 = pj & ~pk;
  assign new_n52 = ~pi & new_n51;
  assign new_n53 = new_n50 & new_n52;
  assign pb0 = pa0 & new_n53;
  assign pc0 = pa & pi;
  assign pd0 = pb & pi;
  assign pe0 = pc & pi;
  assign pf0 = pd & pi;
  assign pg0 = pe & pi;
  assign ph0 = pf & pi;
  assign pi0 = pg & pi;
  assign pj0 = ph & pi;
  assign new_n63 = pl & pb0;
  assign new_n64 = ~pt & new_n52;
  assign new_n65 = ~new_n63 & ~new_n64;
  assign pk0 = pc0 | ~new_n65;
  assign new_n67 = pt & ~pu;
  assign new_n68 = new_n52 & new_n67;
  assign new_n69 = pm & pb0;
  assign new_n70 = ~pt & pu;
  assign new_n71 = new_n52 & new_n70;
  assign new_n72 = ~pd0 & ~new_n71;
  assign new_n73 = ~new_n68 & ~new_n69;
  assign pl0 = ~new_n72 | ~new_n73;
  assign new_n75 = pv & ~new_n45;
  assign new_n76 = new_n52 & new_n75;
  assign new_n77 = pn & pb0;
  assign new_n78 = ~pv & new_n45;
  assign new_n79 = new_n52 & new_n78;
  assign new_n80 = ~pe0 & ~new_n79;
  assign new_n81 = ~new_n76 & ~new_n77;
  assign pm0 = ~new_n80 | ~new_n81;
  assign new_n83 = pw & ~new_n46;
  assign new_n84 = new_n52 & new_n83;
  assign new_n85 = po & pb0;
  assign new_n86 = ~pw & new_n46;
  assign new_n87 = new_n52 & new_n86;
  assign new_n88 = ~pf0 & ~new_n87;
  assign new_n89 = ~new_n84 & ~new_n85;
  assign pn0 = ~new_n88 | ~new_n89;
  assign new_n91 = px & ~new_n47;
  assign new_n92 = new_n52 & new_n91;
  assign new_n93 = pp & pb0;
  assign new_n94 = ~px & new_n47;
  assign new_n95 = new_n52 & new_n94;
  assign new_n96 = ~pg0 & ~new_n95;
  assign new_n97 = ~new_n92 & ~new_n93;
  assign po0 = ~new_n96 | ~new_n97;
  assign new_n99 = py & ~new_n48;
  assign new_n100 = new_n52 & new_n99;
  assign new_n101 = pq & pb0;
  assign new_n102 = ~py & new_n48;
  assign new_n103 = new_n52 & new_n102;
  assign new_n104 = ~ph0 & ~new_n103;
  assign new_n105 = ~new_n100 & ~new_n101;
  assign pp0 = ~new_n104 | ~new_n105;
  assign new_n107 = pz & ~new_n49;
  assign new_n108 = new_n52 & new_n107;
  assign new_n109 = pr & pb0;
  assign new_n110 = ~pz & new_n49;
  assign new_n111 = new_n52 & new_n110;
  assign new_n112 = ~pi0 & ~new_n111;
  assign new_n113 = ~new_n108 & ~new_n109;
  assign pq0 = ~new_n112 | ~new_n113;
  assign new_n115 = pa0 & ~new_n50;
  assign new_n116 = new_n52 & new_n115;
  assign new_n117 = ps & pb0;
  assign new_n118 = ~pa0 & new_n50;
  assign new_n119 = new_n52 & new_n118;
  assign new_n120 = ~pj0 & ~new_n119;
  assign new_n121 = ~new_n116 & ~new_n117;
  assign pr0 = ~new_n120 | ~new_n121;
endmodule


