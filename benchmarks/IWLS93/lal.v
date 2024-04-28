// Benchmark "top" written by ABC on Mon Feb 19 11:52:44 2024

module top ( 
    pp, pa0, pq, pr, ps, pt, pu, pv, pw, px, py, pz, pa, pb, pc, pd, pe,
    pf, pg, ph, pj, pk, pl, pm, pn, po,
    pb0, pc0, pd0, pe0, pf0, pg0, ph0, pi0, pj0, pk0, pl0, pm0, pn0, po0,
    pp0, pq0, pr0, ps0, pt0  );
  input  pp, pa0, pq, pr, ps, pt, pu, pv, pw, px, py, pz, pa, pb, pc, pd,
    pe, pf, pg, ph, pj, pk, pl, pm, pn, po;
  output pb0, pc0, pd0, pe0, pf0, pg0, ph0, pi0, pj0, pk0, pl0, pm0, pn0, po0,
    pp0, pq0, pr0, ps0, pt0;
  wire new_n47, new_n48, new_n49, new_n50, new_n51, new_n52, new_n53,
    new_n54, new_n55, new_n56, new_n57, new_n58, new_n59, new_n60, new_n61,
    new_n62, new_n63, new_n64, new_n66, new_n67, new_n68, new_n69, new_n70,
    new_n71, new_n73, new_n74, new_n75, new_n76, new_n77, new_n78, new_n79,
    new_n80, new_n81, new_n82, new_n83, new_n84, new_n85, new_n86, new_n87,
    new_n88, new_n89, new_n90, new_n91, new_n92, new_n93, new_n98, new_n99,
    new_n101, new_n102, new_n104, new_n105, new_n106, new_n107, new_n109,
    new_n110, new_n111, new_n112, new_n113, new_n114, new_n115, new_n116,
    new_n117, new_n118, new_n120, new_n121, new_n122, new_n123, new_n124,
    new_n125, new_n126, new_n128, new_n129, new_n130, new_n131, new_n132,
    new_n133, new_n134, new_n135, new_n136, new_n138, new_n139, new_n140,
    new_n141, new_n142, new_n143, new_n144, new_n145, new_n146, new_n147,
    new_n148, new_n150, new_n151, new_n152, new_n153, new_n154, new_n155,
    new_n156, new_n157, new_n158, new_n159, new_n160, new_n161, new_n162,
    new_n164, new_n165, new_n166, new_n167, new_n168, new_n169, new_n170,
    new_n171, new_n172, new_n173, new_n174, new_n175, new_n176, new_n178,
    new_n179, new_n180, new_n181, new_n182, new_n183, new_n184, new_n185,
    new_n186, new_n187, new_n188, new_n189;
  assign pb0 = ~pr & pj;
  assign new_n47 = pt & ~pv;
  assign new_n48 = pu & ~pv;
  assign new_n49 = ps & ~pv;
  assign new_n50 = ~new_n47 & ~new_n48;
  assign new_n51 = ~new_n49 & new_n50;
  assign new_n52 = pw & new_n51;
  assign new_n53 = px & new_n52;
  assign new_n54 = ~py & ~new_n53;
  assign new_n55 = ~pa0 & new_n54;
  assign new_n56 = ~pf & ~ph;
  assign new_n57 = pa0 & new_n56;
  assign new_n58 = ~pe & ~ph;
  assign new_n59 = pa0 & new_n58;
  assign new_n60 = pz & new_n56;
  assign new_n61 = pz & new_n58;
  assign new_n62 = ~new_n57 & ~new_n59;
  assign new_n63 = ~new_n60 & ~new_n61;
  assign new_n64 = new_n62 & new_n63;
  assign pc0 = new_n55 | new_n64;
  assign new_n66 = px & pz;
  assign new_n67 = new_n51 & new_n66;
  assign new_n68 = pw & new_n67;
  assign new_n69 = ~pa0 & ~pz;
  assign new_n70 = ~pa0 & ~py;
  assign new_n71 = ~new_n69 & ~new_n70;
  assign pe0 = ~new_n68 & ~new_n71;
  assign new_n73 = pc & ~pm;
  assign new_n74 = pd & ~pn;
  assign new_n75 = ~pc & pm;
  assign new_n76 = ~pd & pn;
  assign new_n77 = ~pb & pl;
  assign new_n78 = ~new_n75 & ~new_n76;
  assign new_n79 = ~new_n77 & new_n78;
  assign new_n80 = pa & new_n79;
  assign new_n81 = ~pk & new_n79;
  assign new_n82 = ~new_n80 & ~new_n81;
  assign new_n83 = ~new_n73 & ~new_n74;
  assign new_n84 = ~new_n82 & new_n83;
  assign new_n85 = ~pb & new_n84;
  assign new_n86 = ~pa & new_n85;
  assign new_n87 = pl & new_n84;
  assign new_n88 = ~pa & new_n87;
  assign new_n89 = pk & new_n85;
  assign new_n90 = pk & new_n87;
  assign new_n91 = ~new_n86 & ~new_n88;
  assign new_n92 = ~new_n89 & ~new_n90;
  assign new_n93 = new_n91 & new_n92;
  assign pf0 = ~pj & new_n93;
  assign pg0 = ~pj & ~po;
  assign ph0 = pp & ~pj;
  assign pi0 = ~pg | pj;
  assign new_n98 = pe & pf;
  assign new_n99 = ~ph & new_n98;
  assign pk0 = ~pq & new_n99;
  assign new_n101 = ~pq & ~new_n98;
  assign new_n102 = ~ph & new_n101;
  assign pl0 = ~ps & new_n102;
  assign new_n104 = ~pt & new_n102;
  assign new_n105 = ps & new_n104;
  assign new_n106 = pt & new_n102;
  assign new_n107 = ~ps & new_n106;
  assign pm0 = new_n105 | new_n107;
  assign new_n109 = ps & pt;
  assign new_n110 = pu & new_n109;
  assign new_n111 = ~pq & ~ph;
  assign new_n112 = ~pf & new_n111;
  assign new_n113 = ~pe & new_n111;
  assign new_n114 = ~new_n112 & ~new_n113;
  assign new_n115 = ~new_n110 & ~new_n114;
  assign new_n116 = pt & new_n115;
  assign new_n117 = ps & new_n116;
  assign new_n118 = pu & new_n115;
  assign pn0 = new_n117 | new_n118;
  assign new_n120 = ~pv & new_n110;
  assign new_n121 = ~pq & ~new_n120;
  assign new_n122 = ~pe & new_n121;
  assign new_n123 = ~pf & new_n121;
  assign new_n124 = ~new_n122 & ~new_n123;
  assign new_n125 = pv & ~new_n110;
  assign new_n126 = ~new_n124 & ~new_n125;
  assign po0 = ph | ~new_n126;
  assign new_n128 = ~pw & new_n120;
  assign new_n129 = ~pq & ~new_n128;
  assign new_n130 = ~pe & new_n129;
  assign new_n131 = ~pf & new_n129;
  assign new_n132 = ~new_n130 & ~new_n131;
  assign new_n133 = ps & new_n48;
  assign new_n134 = pt & new_n133;
  assign new_n135 = pw & ~new_n134;
  assign new_n136 = ~new_n132 & ~new_n135;
  assign pp0 = ph | ~new_n136;
  assign new_n138 = ~pw & new_n134;
  assign new_n139 = ~px & new_n138;
  assign new_n140 = ~pq & ~new_n139;
  assign new_n141 = ~pe & new_n140;
  assign new_n142 = ~pf & new_n140;
  assign new_n143 = ~new_n141 & ~new_n142;
  assign new_n144 = ~pw & new_n48;
  assign new_n145 = ps & new_n144;
  assign new_n146 = pt & new_n145;
  assign new_n147 = px & ~new_n146;
  assign new_n148 = ~new_n143 & ~new_n147;
  assign pq0 = ph | ~new_n148;
  assign new_n150 = ~px & new_n146;
  assign new_n151 = ~py & new_n150;
  assign new_n152 = ~pq & ~new_n151;
  assign new_n153 = ~pe & new_n152;
  assign new_n154 = ~pf & new_n152;
  assign new_n155 = ~new_n153 & ~new_n154;
  assign new_n156 = pt & pu;
  assign new_n157 = ~pv & ~pw;
  assign new_n158 = ~px & new_n157;
  assign new_n159 = new_n156 & new_n158;
  assign new_n160 = ps & new_n159;
  assign new_n161 = py & ~new_n160;
  assign new_n162 = ~new_n155 & ~new_n161;
  assign pr0 = ph | ~new_n162;
  assign new_n164 = ~py & new_n160;
  assign new_n165 = ~pz & new_n164;
  assign new_n166 = ~pq & ~new_n165;
  assign new_n167 = ~pe & new_n166;
  assign new_n168 = ~pf & new_n166;
  assign new_n169 = ~new_n167 & ~new_n168;
  assign new_n170 = ~pv & new_n156;
  assign new_n171 = ~pw & ~px;
  assign new_n172 = ~py & new_n171;
  assign new_n173 = new_n170 & new_n172;
  assign new_n174 = ps & new_n173;
  assign new_n175 = pz & ~new_n174;
  assign new_n176 = ~new_n169 & ~new_n175;
  assign ps0 = ph | ~new_n176;
  assign new_n178 = ~pz & new_n174;
  assign new_n179 = ~pa0 & new_n178;
  assign new_n180 = ~pq & ~new_n179;
  assign new_n181 = ~pe & new_n180;
  assign new_n182 = ~pf & new_n180;
  assign new_n183 = ~new_n181 & ~new_n182;
  assign new_n184 = ~px & ~py;
  assign new_n185 = ~pz & new_n184;
  assign new_n186 = new_n144 & new_n185;
  assign new_n187 = new_n109 & new_n186;
  assign new_n188 = pa0 & ~new_n187;
  assign new_n189 = ~new_n183 & ~new_n188;
  assign pt0 = ph | ~new_n189;
  assign pj0 = ~pe0;
  assign pd0 = pr;
endmodule


