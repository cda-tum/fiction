// Benchmark "top" written by ABC on Mon Feb 19 11:52:44 2024

module top ( 
    pp, pq, pr, ps, pa, pb, pc, pd, pe, pf, pg, ph, pi, pj, pk, pl, pm, pn,
    po,
    pa0, pb0, pc0, pd0, pt, pe0, pu, pf0, pv, pg0, pw, ph0, px, py, pz  );
  input  pp, pq, pr, ps, pa, pb, pc, pd, pe, pf, pg, ph, pi, pj, pk, pl,
    pm, pn, po;
  output pa0, pb0, pc0, pd0, pt, pe0, pu, pf0, pv, pg0, pw, ph0, px, py, pz;
  wire new_n35, new_n36, new_n37, new_n38, new_n39, new_n40, new_n41,
    new_n42, new_n43, new_n44, new_n45, new_n46, new_n47, new_n48, new_n49,
    new_n50, new_n51, new_n52, new_n53, new_n54, new_n55, new_n56, new_n57,
    new_n58, new_n60, new_n61, new_n62, new_n63, new_n64, new_n65, new_n66,
    new_n67, new_n68, new_n69, new_n70, new_n71, new_n72, new_n73, new_n74,
    new_n75, new_n77, new_n78, new_n79, new_n80, new_n81, new_n82, new_n83,
    new_n84, new_n85, new_n86, new_n87, new_n88, new_n89, new_n90, new_n91,
    new_n92, new_n93, new_n94, new_n95, new_n96, new_n97, new_n98,
    new_n100, new_n101, new_n102, new_n103, new_n104, new_n105, new_n106,
    new_n107, new_n108, new_n109, new_n110, new_n111, new_n113, new_n114,
    new_n115, new_n116, new_n118, new_n119, new_n120, new_n121, new_n122,
    new_n124, new_n125, new_n126, new_n128, new_n129, new_n130, new_n131,
    new_n132, new_n134, new_n135, new_n136, new_n137, new_n138, new_n139,
    new_n140, new_n143, new_n144, new_n145, new_n146, new_n147, new_n148,
    new_n149, new_n151, new_n152, new_n153, new_n154, new_n155, new_n156,
    new_n157, new_n158, new_n159, new_n160, new_n161, new_n162, new_n163,
    new_n164, new_n165, new_n166, new_n167, new_n168, new_n169, new_n170,
    new_n171, new_n172, new_n174, new_n175, new_n176, new_n177, new_n178,
    new_n179, new_n180, new_n181, new_n182, new_n183, new_n184, new_n185,
    new_n186;
  assign new_n35 = pq & ~pc;
  assign new_n36 = ~pj & ~pk;
  assign new_n37 = ~pl & new_n36;
  assign new_n38 = ~pq & ~new_n37;
  assign new_n39 = ~pd & ~pi;
  assign new_n40 = ~pq & ~ph;
  assign new_n41 = ~pq & ~pg;
  assign new_n42 = ~pq & ~pi;
  assign new_n43 = ~pd & ~new_n37;
  assign new_n44 = ~pd & ~ph;
  assign new_n45 = ~pd & ~pg;
  assign new_n46 = ~new_n38 & ~new_n39;
  assign new_n47 = ~new_n40 & ~new_n41;
  assign new_n48 = new_n46 & new_n47;
  assign new_n49 = ~new_n44 & ~new_n45;
  assign new_n50 = ~new_n42 & ~new_n43;
  assign new_n51 = new_n49 & new_n50;
  assign new_n52 = new_n48 & new_n51;
  assign new_n53 = ph & new_n36;
  assign new_n54 = pg & new_n53;
  assign new_n55 = pi & new_n54;
  assign new_n56 = pl & ~new_n55;
  assign new_n57 = ~new_n35 & ~new_n52;
  assign new_n58 = pe & ~new_n56;
  assign pa0 = ~new_n57 | ~new_n58;
  assign new_n60 = ~pj & ~pl;
  assign new_n61 = ~pk & new_n60;
  assign new_n62 = ~pm & new_n61;
  assign new_n63 = ~pq & ~new_n62;
  assign new_n64 = ~pd & ~new_n62;
  assign new_n65 = ~new_n39 & ~new_n63;
  assign new_n66 = new_n47 & new_n65;
  assign new_n67 = ~new_n42 & ~new_n64;
  assign new_n68 = new_n49 & new_n67;
  assign new_n69 = new_n66 & new_n68;
  assign new_n70 = ph & new_n37;
  assign new_n71 = pg & new_n70;
  assign new_n72 = pi & new_n71;
  assign new_n73 = pm & ~new_n72;
  assign new_n74 = ~new_n35 & ~new_n69;
  assign new_n75 = pe & ~new_n73;
  assign pb0 = ~new_n74 | ~new_n75;
  assign new_n77 = ~pk & ~pm;
  assign new_n78 = ~pl & new_n77;
  assign new_n79 = ~pn & new_n78;
  assign new_n80 = ~pq & ~new_n79;
  assign new_n81 = pi & ~pj;
  assign new_n82 = ~pq & ~new_n81;
  assign new_n83 = ~pd & ~new_n79;
  assign new_n84 = ~pd & ~new_n81;
  assign new_n85 = ~new_n44 & ~new_n80;
  assign new_n86 = ~new_n41 & ~new_n82;
  assign new_n87 = new_n85 & new_n86;
  assign new_n88 = ~new_n45 & ~new_n84;
  assign new_n89 = ~new_n40 & ~new_n83;
  assign new_n90 = new_n88 & new_n89;
  assign new_n91 = new_n87 & new_n90;
  assign new_n92 = new_n60 & new_n77;
  assign new_n93 = ph & new_n92;
  assign new_n94 = pg & new_n93;
  assign new_n95 = pi & new_n94;
  assign new_n96 = pn & ~new_n95;
  assign new_n97 = ~new_n35 & ~new_n91;
  assign new_n98 = pe & ~new_n96;
  assign pc0 = ~new_n97 | ~new_n98;
  assign new_n100 = ~pa & ~new_n79;
  assign new_n101 = pc & ~pd;
  assign new_n102 = pq & ~new_n101;
  assign new_n103 = ~new_n100 & ~new_n102;
  assign new_n104 = ph & pi;
  assign new_n105 = ~pj & new_n104;
  assign new_n106 = pe & new_n103;
  assign new_n107 = new_n105 & new_n106;
  assign new_n108 = pg & new_n107;
  assign new_n109 = pe & ~new_n101;
  assign new_n110 = pq & po;
  assign new_n111 = new_n109 & new_n110;
  assign pd0 = new_n108 | new_n111;
  assign new_n113 = ~pb & ~po;
  assign new_n114 = ~pc & ~po;
  assign new_n115 = pb & ~pc;
  assign new_n116 = ~new_n113 & ~new_n114;
  assign pt = new_n115 | ~new_n116;
  assign new_n118 = ~pp & ps;
  assign new_n119 = pe & new_n118;
  assign new_n120 = ~pf & new_n118;
  assign new_n121 = pe & pf;
  assign new_n122 = ~new_n119 & ~new_n120;
  assign pu = ~new_n121 & new_n122;
  assign new_n124 = pq & pe;
  assign new_n125 = ~pc & new_n124;
  assign new_n126 = pd & pe;
  assign pf0 = new_n125 | new_n126;
  assign new_n128 = pe & ~pg;
  assign new_n129 = pc & new_n128;
  assign new_n130 = ~pd & new_n129;
  assign new_n131 = ~pq & pe;
  assign new_n132 = ~pg & new_n131;
  assign pv = new_n130 | new_n132;
  assign new_n134 = pq & pd;
  assign new_n135 = pg & ph;
  assign new_n136 = ~new_n134 & ~new_n135;
  assign new_n137 = pe & ~new_n35;
  assign new_n138 = new_n136 & new_n137;
  assign new_n139 = pg & new_n138;
  assign new_n140 = ph & new_n138;
  assign pw = new_n139 | new_n140;
  assign ph0 = pr & pe;
  assign new_n143 = pg & new_n104;
  assign new_n144 = ~new_n35 & ~new_n143;
  assign new_n145 = pe & ~new_n134;
  assign new_n146 = new_n144 & new_n145;
  assign new_n147 = ph & new_n146;
  assign new_n148 = pg & new_n147;
  assign new_n149 = pi & new_n146;
  assign px = new_n148 | new_n149;
  assign new_n151 = pi & new_n135;
  assign new_n152 = pj & ~new_n151;
  assign new_n153 = pe & pj;
  assign new_n154 = ~pq & new_n153;
  assign new_n155 = ~ph & new_n131;
  assign new_n156 = ~pi & new_n131;
  assign new_n157 = pc & pj;
  assign new_n158 = pe & new_n157;
  assign new_n159 = ~pd & new_n158;
  assign new_n160 = pe & ~ph;
  assign new_n161 = pc & new_n160;
  assign new_n162 = ~pd & new_n161;
  assign new_n163 = pe & ~pi;
  assign new_n164 = pc & new_n163;
  assign new_n165 = ~pd & new_n164;
  assign new_n166 = ~new_n130 & ~new_n154;
  assign new_n167 = ~new_n155 & ~new_n156;
  assign new_n168 = new_n166 & new_n167;
  assign new_n169 = ~new_n162 & ~new_n165;
  assign new_n170 = ~new_n132 & ~new_n159;
  assign new_n171 = new_n169 & new_n170;
  assign new_n172 = new_n168 & new_n171;
  assign py = new_n152 | new_n172;
  assign new_n174 = ~pq & ~new_n36;
  assign new_n175 = ~pd & ~new_n36;
  assign new_n176 = ~new_n39 & ~new_n174;
  assign new_n177 = new_n47 & new_n176;
  assign new_n178 = ~new_n42 & ~new_n175;
  assign new_n179 = new_n49 & new_n178;
  assign new_n180 = new_n177 & new_n179;
  assign new_n181 = pg & pi;
  assign new_n182 = ph & new_n181;
  assign new_n183 = ~pj & new_n182;
  assign new_n184 = pk & ~new_n183;
  assign new_n185 = ~new_n35 & ~new_n180;
  assign new_n186 = pe & ~new_n184;
  assign pz = ~new_n185 | ~new_n186;
  assign pe0 = pc;
  assign pg0 = pe;
endmodule


