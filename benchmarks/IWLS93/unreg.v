// Benchmark "top" written by ABC on Mon Feb 19 11:52:45 2024

module top ( 
    pp, pa0, pq, pb0, pc0, ps, pd0, pt, pe0, pu, pf0, pv, pg0, pw, ph0, px,
    pi0, py, pj0, pz, pk0, pa, pb, pc, pd, pe, pf, pg, ph, pi, pj, pk, pl,
    pm, pn, po,
    pa1, pl0, pm0, pn0, po0, pp0, pq0, pr0, ps0, pt0, pu0, pv0, pw0, px0,
    py0, pz0  );
  input  pp, pa0, pq, pb0, pc0, ps, pd0, pt, pe0, pu, pf0, pv, pg0, pw,
    ph0, px, pi0, py, pj0, pz, pk0, pa, pb, pc, pd, pe, pf, pg, ph, pi, pj,
    pk, pl, pm, pn, po;
  output pa1, pl0, pm0, pn0, po0, pp0, pq0, pr0, ps0, pt0, pu0, pv0, pw0, px0,
    py0, pz0;
  wire new_n53, new_n54, new_n55, new_n56, new_n57, new_n58, new_n60,
    new_n61, new_n62, new_n63, new_n64, new_n65, new_n67, new_n68, new_n69,
    new_n70, new_n71, new_n72, new_n74, new_n75, new_n76, new_n77, new_n78,
    new_n79, new_n81, new_n82, new_n83, new_n84, new_n85, new_n86, new_n88,
    new_n89, new_n90, new_n91, new_n92, new_n93, new_n95, new_n96, new_n97,
    new_n98, new_n99, new_n100, new_n102, new_n103, new_n104, new_n105,
    new_n106, new_n107, new_n109, new_n110, new_n111, new_n112, new_n113,
    new_n114, new_n116, new_n117, new_n118, new_n119, new_n120, new_n121,
    new_n123, new_n124, new_n125, new_n126, new_n127, new_n128, new_n130,
    new_n131, new_n132, new_n133, new_n134, new_n135, new_n137, new_n138,
    new_n139, new_n140, new_n141, new_n142, new_n144, new_n145, new_n146,
    new_n147, new_n148, new_n149, new_n151, new_n152, new_n153, new_n154,
    new_n155, new_n156, new_n158, new_n159, new_n160, new_n161, new_n162,
    new_n163;
  assign new_n53 = ~pu & ~pk0;
  assign new_n54 = ~pt & ~pm;
  assign new_n55 = pd0 & pt;
  assign new_n56 = ~new_n54 & ~new_n55;
  assign new_n57 = ~ps & new_n56;
  assign new_n58 = pu & new_n57;
  assign pa1 = new_n53 | new_n58;
  assign new_n60 = ~pu & ~pv;
  assign new_n61 = ~pt & ~pd;
  assign new_n62 = pt & pw;
  assign new_n63 = ~new_n61 & ~new_n62;
  assign new_n64 = ~ps & new_n63;
  assign new_n65 = pu & new_n64;
  assign pl0 = new_n60 | new_n65;
  assign new_n67 = ~pu & ~pw;
  assign new_n68 = ~pt & ~pc;
  assign new_n69 = pt & px;
  assign new_n70 = ~new_n68 & ~new_n69;
  assign new_n71 = ~ps & new_n70;
  assign new_n72 = pu & new_n71;
  assign pm0 = new_n67 | new_n72;
  assign new_n74 = ~pu & ~px;
  assign new_n75 = ~pt & ~pb;
  assign new_n76 = pt & py;
  assign new_n77 = ~new_n75 & ~new_n76;
  assign new_n78 = ~ps & new_n77;
  assign new_n79 = pu & new_n78;
  assign pn0 = new_n74 | new_n79;
  assign new_n81 = ~pu & ~py;
  assign new_n82 = ~pq & pt;
  assign new_n83 = ~pt & ~pa;
  assign new_n84 = ~new_n82 & ~new_n83;
  assign new_n85 = ~ps & new_n84;
  assign new_n86 = pu & new_n85;
  assign po0 = new_n81 | new_n86;
  assign new_n88 = ~pu & ~pz;
  assign new_n89 = ~pt & ~ph;
  assign new_n90 = pa0 & pt;
  assign new_n91 = ~new_n89 & ~new_n90;
  assign new_n92 = ~ps & new_n91;
  assign new_n93 = pu & new_n92;
  assign pp0 = new_n88 | new_n93;
  assign new_n95 = ~pa0 & ~pu;
  assign new_n96 = ~pt & ~pg;
  assign new_n97 = pb0 & pt;
  assign new_n98 = ~new_n96 & ~new_n97;
  assign new_n99 = ~ps & new_n98;
  assign new_n100 = pu & new_n99;
  assign pq0 = new_n95 | new_n100;
  assign new_n102 = ~pb0 & ~pu;
  assign new_n103 = ~pt & ~pf;
  assign new_n104 = pc0 & pt;
  assign new_n105 = ~new_n103 & ~new_n104;
  assign new_n106 = ~ps & new_n105;
  assign new_n107 = pu & new_n106;
  assign pr0 = new_n102 | new_n107;
  assign new_n109 = ~pc0 & ~pu;
  assign new_n110 = ~pt & ~pe;
  assign new_n111 = pt & pv;
  assign new_n112 = ~new_n110 & ~new_n111;
  assign new_n113 = ~ps & new_n112;
  assign new_n114 = pu & new_n113;
  assign ps0 = new_n109 | new_n114;
  assign new_n116 = ~pd0 & ~pu;
  assign new_n117 = ~pt & ~pl;
  assign new_n118 = pt & pe0;
  assign new_n119 = ~new_n117 & ~new_n118;
  assign new_n120 = ~ps & new_n119;
  assign new_n121 = pu & new_n120;
  assign pt0 = new_n116 | new_n121;
  assign new_n123 = ~pe0 & ~pu;
  assign new_n124 = ~pt & ~pk;
  assign new_n125 = pt & pf0;
  assign new_n126 = ~new_n124 & ~new_n125;
  assign new_n127 = ~ps & new_n126;
  assign new_n128 = pu & new_n127;
  assign pu0 = new_n123 | new_n128;
  assign new_n130 = ~pu & ~pf0;
  assign new_n131 = ~pt & ~pj;
  assign new_n132 = pt & pg0;
  assign new_n133 = ~new_n131 & ~new_n132;
  assign new_n134 = ~ps & new_n133;
  assign new_n135 = pu & new_n134;
  assign pv0 = new_n130 | new_n135;
  assign new_n137 = ~pu & ~pg0;
  assign new_n138 = ~pt & ~pi;
  assign new_n139 = pt & pz;
  assign new_n140 = ~new_n138 & ~new_n139;
  assign new_n141 = ~ps & new_n140;
  assign new_n142 = pu & new_n141;
  assign pw0 = new_n137 | new_n142;
  assign new_n144 = ~pu & ~ph0;
  assign new_n145 = ~pp & ~pt;
  assign new_n146 = pt & pi0;
  assign new_n147 = ~new_n145 & ~new_n146;
  assign new_n148 = ~ps & new_n147;
  assign new_n149 = pu & new_n148;
  assign px0 = new_n144 | new_n149;
  assign new_n151 = ~pu & ~pi0;
  assign new_n152 = ~pt & ~po;
  assign new_n153 = pt & pj0;
  assign new_n154 = ~new_n152 & ~new_n153;
  assign new_n155 = ~ps & new_n154;
  assign new_n156 = pu & new_n155;
  assign py0 = new_n151 | new_n156;
  assign new_n158 = ~pu & ~pj0;
  assign new_n159 = ~pt & ~pn;
  assign new_n160 = pt & pk0;
  assign new_n161 = ~new_n159 & ~new_n160;
  assign new_n162 = ~ps & new_n161;
  assign new_n163 = pu & new_n162;
  assign pz0 = new_n158 | new_n163;
endmodule


