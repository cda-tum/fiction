// Benchmark "top" written by ABC on Mon Feb 19 11:52:42 2024

module top ( 
    pp, pa0, pq, pb0, pr, pc0, ps, pd0, pt, pe0, pu, pf0, pv, pw, px, py,
    pz, pa, pb, pc, pd, pe, pf, pg, ph, pi, pj, pk, pl, pm, pn, po,
    pg0, ph0, pi0  );
  input  pp, pa0, pq, pb0, pr, pc0, ps, pd0, pt, pe0, pu, pf0, pv, pw,
    px, py, pz, pa, pb, pc, pd, pe, pf, pg, ph, pi, pj, pk, pl, pm, pn, po;
  output pg0, ph0, pi0;
  wire new_n36, new_n37, new_n38, new_n39, new_n40, new_n41, new_n42,
    new_n43, new_n44, new_n45, new_n46, new_n47, new_n48, new_n49, new_n50,
    new_n51, new_n52, new_n53, new_n54, new_n55, new_n56, new_n57, new_n58,
    new_n59, new_n60, new_n61, new_n62, new_n63, new_n64, new_n65, new_n66,
    new_n67, new_n68, new_n69, new_n70, new_n71, new_n72, new_n73, new_n74,
    new_n75, new_n76, new_n77, new_n78, new_n79, new_n80, new_n81, new_n82,
    new_n83, new_n84, new_n85, new_n86, new_n87, new_n88, new_n89, new_n90,
    new_n91, new_n92, new_n93, new_n94, new_n95, new_n96, new_n97, new_n98,
    new_n99, new_n100, new_n101, new_n102, new_n103, new_n104, new_n105,
    new_n106, new_n107, new_n108, new_n109, new_n110, new_n111, new_n112,
    new_n113, new_n114, new_n115, new_n116, new_n117, new_n118, new_n119,
    new_n120, new_n121, new_n122, new_n123, new_n124, new_n125, new_n126,
    new_n127, new_n128, new_n129, new_n130, new_n131, new_n132, new_n133,
    new_n134, new_n135, new_n136, new_n137, new_n138, new_n139, new_n140,
    new_n141, new_n142, new_n143, new_n144, new_n145, new_n146, new_n147,
    new_n148, new_n149, new_n151, new_n152, new_n153, new_n154, new_n155,
    new_n156, new_n157, new_n158;
  assign new_n36 = ~pq & pa;
  assign new_n37 = pq & ~pa;
  assign new_n38 = ~new_n36 & ~new_n37;
  assign new_n39 = ~ps & pc;
  assign new_n40 = ps & ~pc;
  assign new_n41 = ~new_n39 & ~new_n40;
  assign new_n42 = ~pr & pb;
  assign new_n43 = pr & ~pb;
  assign new_n44 = ~new_n42 & ~new_n43;
  assign new_n45 = ~pt & pd;
  assign new_n46 = pt & ~pd;
  assign new_n47 = ~new_n45 & ~new_n46;
  assign new_n48 = new_n38 & new_n41;
  assign new_n49 = new_n44 & new_n48;
  assign new_n50 = new_n47 & new_n49;
  assign new_n51 = new_n44 & new_n45;
  assign new_n52 = new_n38 & new_n51;
  assign new_n53 = new_n41 & new_n52;
  assign new_n54 = new_n38 & new_n42;
  assign new_n55 = new_n38 & new_n39;
  assign new_n56 = new_n44 & new_n55;
  assign new_n57 = ~new_n53 & ~new_n54;
  assign new_n58 = ~new_n36 & ~new_n56;
  assign new_n59 = new_n57 & new_n58;
  assign new_n60 = ~new_n50 & new_n59;
  assign new_n61 = ~new_n59 & ~new_n60;
  assign new_n62 = new_n59 & new_n60;
  assign new_n63 = ~new_n61 & ~new_n62;
  assign new_n64 = ~py & pi;
  assign new_n65 = py & ~pi;
  assign new_n66 = ~new_n64 & ~new_n65;
  assign new_n67 = ~pa0 & pk;
  assign new_n68 = pa0 & ~pk;
  assign new_n69 = ~new_n67 & ~new_n68;
  assign new_n70 = ~pz & pj;
  assign new_n71 = pz & ~pj;
  assign new_n72 = ~new_n70 & ~new_n71;
  assign new_n73 = ~pb0 & pl;
  assign new_n74 = pb0 & ~pl;
  assign new_n75 = ~new_n73 & ~new_n74;
  assign new_n76 = new_n66 & new_n69;
  assign new_n77 = new_n72 & new_n76;
  assign new_n78 = new_n75 & new_n77;
  assign new_n79 = new_n72 & new_n73;
  assign new_n80 = new_n66 & new_n79;
  assign new_n81 = new_n69 & new_n80;
  assign new_n82 = new_n66 & new_n70;
  assign new_n83 = new_n66 & new_n67;
  assign new_n84 = new_n72 & new_n83;
  assign new_n85 = ~new_n81 & ~new_n82;
  assign new_n86 = ~new_n64 & ~new_n84;
  assign new_n87 = new_n85 & new_n86;
  assign new_n88 = ~new_n78 & new_n87;
  assign new_n89 = ~new_n87 & ~new_n88;
  assign new_n90 = new_n87 & new_n88;
  assign new_n91 = ~new_n89 & ~new_n90;
  assign new_n92 = ~pu & pe;
  assign new_n93 = pu & ~pe;
  assign new_n94 = ~new_n92 & ~new_n93;
  assign new_n95 = ~pw & pg;
  assign new_n96 = pw & ~pg;
  assign new_n97 = ~new_n95 & ~new_n96;
  assign new_n98 = ~pv & pf;
  assign new_n99 = pv & ~pf;
  assign new_n100 = ~new_n98 & ~new_n99;
  assign new_n101 = ~px & ph;
  assign new_n102 = px & ~ph;
  assign new_n103 = ~new_n101 & ~new_n102;
  assign new_n104 = new_n94 & new_n97;
  assign new_n105 = new_n100 & new_n104;
  assign new_n106 = new_n103 & new_n105;
  assign new_n107 = new_n100 & new_n101;
  assign new_n108 = new_n94 & new_n107;
  assign new_n109 = new_n97 & new_n108;
  assign new_n110 = new_n94 & new_n98;
  assign new_n111 = new_n94 & new_n95;
  assign new_n112 = new_n100 & new_n111;
  assign new_n113 = ~new_n109 & ~new_n110;
  assign new_n114 = ~new_n92 & ~new_n112;
  assign new_n115 = new_n113 & new_n114;
  assign new_n116 = ~new_n106 & new_n115;
  assign new_n117 = ~new_n115 & ~new_n116;
  assign new_n118 = new_n115 & new_n116;
  assign new_n119 = ~new_n117 & ~new_n118;
  assign new_n120 = ~pc0 & pm;
  assign new_n121 = pc0 & ~pm;
  assign new_n122 = ~new_n120 & ~new_n121;
  assign new_n123 = ~pe0 & po;
  assign new_n124 = pe0 & ~po;
  assign new_n125 = ~new_n123 & ~new_n124;
  assign new_n126 = ~pd0 & pn;
  assign new_n127 = pd0 & ~pn;
  assign new_n128 = ~new_n126 & ~new_n127;
  assign new_n129 = pp & ~pf0;
  assign new_n130 = ~pp & pf0;
  assign new_n131 = ~new_n129 & ~new_n130;
  assign new_n132 = new_n122 & new_n125;
  assign new_n133 = new_n128 & new_n132;
  assign new_n134 = new_n131 & new_n133;
  assign new_n135 = new_n128 & new_n129;
  assign new_n136 = new_n122 & new_n135;
  assign new_n137 = new_n125 & new_n136;
  assign new_n138 = new_n122 & new_n126;
  assign new_n139 = new_n122 & new_n123;
  assign new_n140 = new_n128 & new_n139;
  assign new_n141 = ~new_n137 & ~new_n138;
  assign new_n142 = ~new_n120 & ~new_n140;
  assign new_n143 = new_n141 & new_n142;
  assign new_n144 = ~new_n134 & new_n143;
  assign new_n145 = ~new_n143 & ~new_n144;
  assign new_n146 = new_n143 & new_n144;
  assign new_n147 = ~new_n145 & ~new_n146;
  assign new_n148 = new_n63 & new_n91;
  assign new_n149 = new_n119 & new_n148;
  assign ph0 = new_n147 & new_n149;
  assign new_n151 = new_n119 & new_n145;
  assign new_n152 = new_n63 & new_n151;
  assign new_n153 = new_n91 & new_n152;
  assign new_n154 = new_n63 & new_n117;
  assign new_n155 = new_n63 & new_n89;
  assign new_n156 = new_n119 & new_n155;
  assign new_n157 = ~new_n153 & ~new_n154;
  assign new_n158 = ~new_n61 & ~new_n156;
  assign pi0 = ~new_n157 | ~new_n158;
  assign pg0 = ~ph0 & ~pi0;
endmodule


