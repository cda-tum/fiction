// Benchmark "top" written by ABC on Mon Feb 19 11:52:43 2024

module top ( 
    pp, pa0, pq, pb0, pr, pc0, ps, pd0, pe0, pu, pf0, pv, pg0, pw, ph0, px,
    pi0, py, pj0, pz, pa, pb, pc, pd, pe, pf, pg, ph, pi, pj, pk, pl, pm,
    pn, po,
    pk0, pl0, pm0, pn0, po0, pp0, pq0, pr0, ps0, pt0, pu0, pv0, pw0, px0,
    py0, pz0  );
  input  pp, pa0, pq, pb0, pr, pc0, ps, pd0, pe0, pu, pf0, pv, pg0, pw,
    ph0, px, pi0, py, pj0, pz, pa, pb, pc, pd, pe, pf, pg, ph, pi, pj, pk,
    pl, pm, pn, po;
  output pk0, pl0, pm0, pn0, po0, pp0, pq0, pr0, ps0, pt0, pu0, pv0, pw0, px0,
    py0, pz0;
  wire new_n52, new_n53, new_n54, new_n55, new_n56, new_n57, new_n59,
    new_n60, new_n61, new_n62, new_n63, new_n64, new_n65, new_n67, new_n68,
    new_n69, new_n70, new_n71, new_n72, new_n73, new_n75, new_n76, new_n77,
    new_n78, new_n79, new_n80, new_n81, new_n83, new_n84, new_n85, new_n86,
    new_n87, new_n88, new_n89, new_n91, new_n92, new_n93, new_n94, new_n95,
    new_n96, new_n97, new_n99, new_n100, new_n101, new_n102, new_n103,
    new_n104, new_n105, new_n107, new_n108, new_n109, new_n110, new_n111,
    new_n112, new_n113, new_n115, new_n116, new_n117, new_n118, new_n119,
    new_n120, new_n121, new_n123, new_n124, new_n125, new_n126, new_n127,
    new_n128, new_n129, new_n131, new_n132, new_n133, new_n134, new_n135,
    new_n136, new_n137, new_n139, new_n140, new_n141, new_n142, new_n143,
    new_n144, new_n145, new_n147, new_n148, new_n149, new_n150, new_n151,
    new_n152, new_n153, new_n155, new_n156, new_n157, new_n158, new_n159,
    new_n160, new_n161, new_n163, new_n164, new_n165, new_n166, new_n167,
    new_n168, new_n169, new_n171, new_n172, new_n173, new_n174, new_n175,
    new_n176, new_n177;
  assign new_n52 = ~pp & ~pq;
  assign new_n53 = ~pr & pu;
  assign new_n54 = pr & ~pu;
  assign new_n55 = ~new_n53 & ~new_n54;
  assign new_n56 = pq & new_n55;
  assign new_n57 = ~new_n52 & ~new_n56;
  assign pk0 = ps | ~new_n57;
  assign new_n59 = ~pq & ~po;
  assign new_n60 = ~pr & ~pu;
  assign new_n61 = ~pv & ~new_n60;
  assign new_n62 = pv & new_n60;
  assign new_n63 = ~new_n61 & ~new_n62;
  assign new_n64 = pq & new_n63;
  assign new_n65 = ~new_n59 & ~new_n64;
  assign pl0 = ps | ~new_n65;
  assign new_n67 = ~pq & ~pn;
  assign new_n68 = ~pv & new_n60;
  assign new_n69 = pw & new_n68;
  assign new_n70 = ~pw & ~new_n68;
  assign new_n71 = ~new_n69 & ~new_n70;
  assign new_n72 = pq & new_n71;
  assign new_n73 = ~new_n67 & ~new_n72;
  assign pm0 = ps | ~new_n73;
  assign new_n75 = ~pq & ~pm;
  assign new_n76 = ~pw & new_n68;
  assign new_n77 = ~px & ~new_n76;
  assign new_n78 = px & new_n76;
  assign new_n79 = ~new_n77 & ~new_n78;
  assign new_n80 = pq & new_n79;
  assign new_n81 = ~new_n75 & ~new_n80;
  assign pn0 = ps | ~new_n81;
  assign new_n83 = ~pq & ~pl;
  assign new_n84 = ~px & new_n76;
  assign new_n85 = py & new_n84;
  assign new_n86 = ~py & ~new_n84;
  assign new_n87 = ~new_n85 & ~new_n86;
  assign new_n88 = pq & new_n87;
  assign new_n89 = ~new_n83 & ~new_n88;
  assign po0 = ps | ~new_n89;
  assign new_n91 = ~pq & ~pk;
  assign new_n92 = ~py & new_n84;
  assign new_n93 = ~pz & ~new_n92;
  assign new_n94 = pz & new_n92;
  assign new_n95 = ~new_n93 & ~new_n94;
  assign new_n96 = pq & new_n95;
  assign new_n97 = ~new_n91 & ~new_n96;
  assign pp0 = ps | ~new_n97;
  assign new_n99 = ~pq & ~pj;
  assign new_n100 = ~pz & new_n92;
  assign new_n101 = pa0 & new_n100;
  assign new_n102 = ~pa0 & ~new_n100;
  assign new_n103 = ~new_n101 & ~new_n102;
  assign new_n104 = pq & new_n103;
  assign new_n105 = ~new_n99 & ~new_n104;
  assign pq0 = ps | ~new_n105;
  assign new_n107 = ~pq & ~pi;
  assign new_n108 = ~pa0 & new_n100;
  assign new_n109 = ~pb0 & ~new_n108;
  assign new_n110 = pb0 & new_n108;
  assign new_n111 = ~new_n109 & ~new_n110;
  assign new_n112 = pq & new_n111;
  assign new_n113 = ~new_n107 & ~new_n112;
  assign pr0 = ps | ~new_n113;
  assign new_n115 = ~pq & ~ph;
  assign new_n116 = ~pb0 & new_n108;
  assign new_n117 = pc0 & new_n116;
  assign new_n118 = ~pc0 & ~new_n116;
  assign new_n119 = ~new_n117 & ~new_n118;
  assign new_n120 = pq & new_n119;
  assign new_n121 = ~new_n115 & ~new_n120;
  assign ps0 = ps | ~new_n121;
  assign new_n123 = ~pq & ~pg;
  assign new_n124 = ~pc0 & new_n116;
  assign new_n125 = ~pd0 & ~new_n124;
  assign new_n126 = pd0 & new_n124;
  assign new_n127 = ~new_n125 & ~new_n126;
  assign new_n128 = pq & new_n127;
  assign new_n129 = ~new_n123 & ~new_n128;
  assign pt0 = ps | ~new_n129;
  assign new_n131 = ~pq & ~pf;
  assign new_n132 = ~pd0 & new_n124;
  assign new_n133 = pe0 & new_n132;
  assign new_n134 = ~pe0 & ~new_n132;
  assign new_n135 = ~new_n133 & ~new_n134;
  assign new_n136 = pq & new_n135;
  assign new_n137 = ~new_n131 & ~new_n136;
  assign pu0 = ps | ~new_n137;
  assign new_n139 = ~pq & ~pe;
  assign new_n140 = ~pe0 & new_n132;
  assign new_n141 = ~pf0 & ~new_n140;
  assign new_n142 = pf0 & new_n140;
  assign new_n143 = ~new_n141 & ~new_n142;
  assign new_n144 = pq & new_n143;
  assign new_n145 = ~new_n139 & ~new_n144;
  assign pv0 = ps | ~new_n145;
  assign new_n147 = ~pq & ~pd;
  assign new_n148 = ~pf0 & new_n140;
  assign new_n149 = pg0 & new_n148;
  assign new_n150 = ~pg0 & ~new_n148;
  assign new_n151 = ~new_n149 & ~new_n150;
  assign new_n152 = pq & new_n151;
  assign new_n153 = ~new_n147 & ~new_n152;
  assign pw0 = ps | ~new_n153;
  assign new_n155 = ~pq & ~pc;
  assign new_n156 = ~pg0 & new_n148;
  assign new_n157 = ~ph0 & ~new_n156;
  assign new_n158 = ph0 & new_n156;
  assign new_n159 = ~new_n157 & ~new_n158;
  assign new_n160 = pq & new_n159;
  assign new_n161 = ~new_n155 & ~new_n160;
  assign px0 = ps | ~new_n161;
  assign new_n163 = ~pq & ~pb;
  assign new_n164 = ~ph0 & new_n156;
  assign new_n165 = pi0 & new_n164;
  assign new_n166 = ~pi0 & ~new_n164;
  assign new_n167 = ~new_n165 & ~new_n166;
  assign new_n168 = pq & new_n167;
  assign new_n169 = ~new_n163 & ~new_n168;
  assign py0 = ps | ~new_n169;
  assign new_n171 = ~pq & ~pa;
  assign new_n172 = ~pi0 & new_n164;
  assign new_n173 = ~pj0 & ~new_n172;
  assign new_n174 = pj0 & new_n172;
  assign new_n175 = ~new_n173 & ~new_n174;
  assign new_n176 = pq & new_n175;
  assign new_n177 = ~new_n171 & ~new_n176;
  assign pz0 = ps | ~new_n177;
endmodule


