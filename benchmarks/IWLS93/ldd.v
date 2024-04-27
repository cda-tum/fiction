// Benchmark "top" written by ABC on Mon Feb 19 11:52:44 2024

module top ( 
    pa, pb, pc, pd, pe, pf, pg, ph, pi,
    pp, pa0, pq, pb0, pr, ps, pt, pu, pv, pw, px, py, pz, pj, pk, pl, pm,
    pn, po  );
  input  pa, pb, pc, pd, pe, pf, pg, ph, pi;
  output pp, pa0, pq, pb0, pr, ps, pt, pu, pv, pw, px, py, pz, pj, pk, pl, pm,
    pn, po;
  wire new_n29, new_n30, new_n31, new_n32, new_n34, new_n35, new_n37,
    new_n38, new_n39, new_n40, new_n41, new_n42, new_n43, new_n44, new_n45,
    new_n46, new_n47, new_n48, new_n49, new_n50, new_n51, new_n52, new_n53,
    new_n55, new_n56, new_n58, new_n59, new_n60, new_n61, new_n62, new_n63,
    new_n64, new_n65, new_n66, new_n67, new_n68, new_n69, new_n70, new_n71,
    new_n72, new_n74, new_n76, new_n78, new_n79, new_n81, new_n82, new_n83,
    new_n84, new_n85, new_n86, new_n87, new_n88, new_n90, new_n91, new_n92,
    new_n93, new_n95, new_n96, new_n98, new_n99, new_n101, new_n102,
    new_n103, new_n104, new_n105, new_n106, new_n107, new_n108, new_n109,
    new_n111, new_n112, new_n113, new_n115, new_n116, new_n117, new_n118,
    new_n119, new_n120, new_n121, new_n122, new_n123, new_n124, new_n125,
    new_n127, new_n128, new_n129, new_n130, new_n131, new_n132, new_n133,
    new_n134, new_n136, new_n137, new_n138;
  assign new_n29 = ~pa & pc;
  assign new_n30 = ~pb & new_n29;
  assign new_n31 = pi & new_n30;
  assign new_n32 = ~pi & new_n30;
  assign pp = new_n31 | new_n32;
  assign new_n34 = pa & pc;
  assign new_n35 = ~pb & new_n34;
  assign pu = pi & new_n35;
  assign new_n37 = ~pb & ~pc;
  assign new_n38 = ~pe & new_n37;
  assign new_n39 = ~pd & new_n38;
  assign new_n40 = pf & new_n39;
  assign new_n41 = ~pa & new_n40;
  assign new_n42 = ~pa & ~pb;
  assign new_n43 = ~pd & new_n42;
  assign new_n44 = ~pc & new_n43;
  assign new_n45 = pe & new_n44;
  assign new_n46 = ~pc & ~pd;
  assign new_n47 = ~pf & new_n46;
  assign new_n48 = ~pe & new_n47;
  assign new_n49 = pg & new_n48;
  assign new_n50 = ~pb & new_n49;
  assign new_n51 = ~pa & new_n50;
  assign new_n52 = ~pu & ~new_n41;
  assign new_n53 = ~new_n45 & ~new_n51;
  assign pa0 = ~new_n52 | ~new_n53;
  assign new_n55 = ~pa & ~pc;
  assign new_n56 = ~pb & new_n55;
  assign pb0 = pd & new_n56;
  assign new_n58 = ~pd & ~pe;
  assign new_n59 = ~pg & new_n58;
  assign new_n60 = ~pf & new_n59;
  assign new_n61 = ph & new_n60;
  assign new_n62 = ~pa & new_n61;
  assign new_n63 = ~pc & new_n62;
  assign new_n64 = ~pb & new_n63;
  assign new_n65 = ~ph & new_n60;
  assign new_n66 = ~pa & new_n65;
  assign new_n67 = ~pc & new_n66;
  assign new_n68 = ~pb & new_n67;
  assign new_n69 = ~new_n64 & ~new_n68;
  assign new_n70 = ~new_n41 & new_n69;
  assign new_n71 = ~new_n45 & ~pb0;
  assign new_n72 = ~new_n51 & new_n71;
  assign pq = ~new_n70 | ~new_n72;
  assign new_n74 = pb & new_n55;
  assign pr = pi & new_n74;
  assign new_n76 = pb & new_n29;
  assign ps = pi & new_n76;
  assign new_n78 = pa & ~pc;
  assign new_n79 = ~pb & new_n78;
  assign pt = pi & new_n79;
  assign new_n81 = ~pi & new_n35;
  assign new_n82 = ~ps & ~pt;
  assign new_n83 = ~pu & ~new_n81;
  assign new_n84 = new_n82 & new_n83;
  assign new_n85 = ~new_n41 & ~new_n64;
  assign new_n86 = ~new_n45 & ~pr;
  assign new_n87 = ~new_n51 & new_n86;
  assign new_n88 = new_n85 & new_n87;
  assign pv = ~new_n84 | ~new_n88;
  assign new_n90 = ~pu & ~new_n64;
  assign new_n91 = new_n82 & new_n90;
  assign new_n92 = ~new_n41 & ~new_n51;
  assign new_n93 = new_n86 & new_n92;
  assign pw = ~new_n91 | ~new_n93;
  assign new_n95 = ~new_n45 & ~pt;
  assign new_n96 = ~new_n51 & ~pr;
  assign py = ~new_n95 | ~new_n96;
  assign new_n98 = ~pu & ~new_n45;
  assign new_n99 = ~new_n64 & ~pr;
  assign pz = ~new_n98 | ~new_n99;
  assign new_n101 = pb & new_n78;
  assign new_n102 = pi & new_n101;
  assign new_n103 = ~pi & new_n101;
  assign new_n104 = ~pi & new_n79;
  assign new_n105 = ~new_n31 & ~new_n64;
  assign new_n106 = ~new_n102 & new_n105;
  assign new_n107 = ~pr & ~ps;
  assign new_n108 = ~new_n51 & new_n107;
  assign new_n109 = new_n106 & new_n108;
  assign pn = pt | new_n104;
  assign new_n111 = ~new_n81 & ~new_n103;
  assign new_n112 = ~pu & new_n111;
  assign new_n113 = ~pn & new_n112;
  assign pj = ~new_n109 | ~new_n113;
  assign new_n115 = ~pi & new_n74;
  assign new_n116 = ~pi & new_n76;
  assign new_n117 = ~new_n31 & ~new_n41;
  assign new_n118 = ~new_n102 & new_n117;
  assign new_n119 = ~pr & ~new_n115;
  assign new_n120 = ~new_n45 & new_n119;
  assign new_n121 = new_n118 & new_n120;
  assign new_n122 = ~ps & ~new_n116;
  assign new_n123 = ~pt & ~new_n103;
  assign new_n124 = ~pu & new_n123;
  assign new_n125 = new_n122 & new_n124;
  assign pk = ~new_n121 | ~new_n125;
  assign new_n127 = ~pb0 & ~new_n64;
  assign new_n128 = ~new_n102 & new_n127;
  assign new_n129 = ~pp & ~new_n41;
  assign new_n130 = new_n128 & new_n129;
  assign new_n131 = ~pr & ~new_n116;
  assign new_n132 = ~ps & ~new_n81;
  assign new_n133 = ~pt & new_n132;
  assign new_n134 = new_n131 & new_n133;
  assign pl = ~new_n130 | ~new_n134;
  assign new_n136 = ~new_n115 & ~new_n116;
  assign new_n137 = ~pu & new_n136;
  assign new_n138 = ~pr & new_n132;
  assign pm = ~new_n137 | ~new_n138;
  assign po = new_n102 | new_n103;
  assign px = pw;
endmodule


