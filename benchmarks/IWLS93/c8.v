// Benchmark "top" written by ABC on Mon Feb 19 11:52:42 2024

module top ( 
    pp, pa0, pq, pb0, pr, pc0, ps, pu, pv, pw, px, py, pz, pa, pb, pc, pd,
    pe, pf, pg, ph, pi, pj, pk, pl, pm, pn, po,
    pd0, pe0, pf0, pg0, ph0, pi0, pj0, pk0, pl0, pm0, pn0, po0, pp0, pq0,
    pr0, ps0, pt0, pu0  );
  input  pp, pa0, pq, pb0, pr, pc0, ps, pu, pv, pw, px, py, pz, pa, pb,
    pc, pd, pe, pf, pg, ph, pi, pj, pk, pl, pm, pn, po;
  output pd0, pe0, pf0, pg0, ph0, pi0, pj0, pk0, pl0, pm0, pn0, po0, pp0, pq0,
    pr0, ps0, pt0, pu0;
  wire new_n47, new_n48, new_n49, new_n50, new_n52, new_n53, new_n54,
    new_n55, new_n57, new_n58, new_n59, new_n60, new_n62, new_n63, new_n64,
    new_n65, new_n67, new_n68, new_n69, new_n70, new_n72, new_n73, new_n74,
    new_n75, new_n77, new_n78, new_n79, new_n80, new_n82, new_n83, new_n84,
    new_n85, new_n87, new_n88, new_n89, new_n90, new_n91, new_n92, new_n93,
    new_n94, new_n95, new_n96, new_n97, new_n98, new_n99, new_n100,
    new_n101, new_n102, new_n103, new_n104, new_n105, new_n106, new_n107,
    new_n108, new_n109, new_n110, new_n111, new_n112, new_n113, new_n114,
    new_n115, new_n116, new_n117, new_n118, new_n119, new_n121, new_n122,
    new_n123, new_n124, new_n125, new_n126, new_n127, new_n128, new_n129,
    new_n130, new_n131, new_n132, new_n134, new_n135, new_n136, new_n137,
    new_n138, new_n139, new_n140, new_n141, new_n142, new_n143, new_n144,
    new_n145, new_n146, new_n147, new_n148, new_n149, new_n150, new_n151,
    new_n152, new_n153, new_n154, new_n155, new_n157, new_n158, new_n159,
    new_n160, new_n161, new_n162, new_n163, new_n164, new_n165, new_n166,
    new_n167, new_n168, new_n169, new_n170, new_n171, new_n172, new_n173,
    new_n174, new_n175, new_n176, new_n177, new_n178, new_n179, new_n180,
    new_n181, new_n183, new_n184, new_n185, new_n186, new_n187, new_n188,
    new_n189, new_n190, new_n191, new_n192, new_n193, new_n194, new_n195,
    new_n196, new_n197, new_n198, new_n199, new_n200, new_n201, new_n202,
    new_n203, new_n204, new_n205, new_n206, new_n207, new_n208, new_n210,
    new_n211, new_n212, new_n213, new_n214, new_n215, new_n216, new_n217,
    new_n218, new_n219, new_n220, new_n221, new_n222, new_n223, new_n224,
    new_n225, new_n226, new_n227, new_n228, new_n229, new_n230, new_n231,
    new_n232, new_n233, new_n234, new_n235, new_n236, new_n238, new_n239,
    new_n240, new_n241, new_n242, new_n243, new_n244, new_n245, new_n246,
    new_n247, new_n248, new_n249, new_n250, new_n251, new_n252, new_n253,
    new_n254, new_n255, new_n256, new_n257, new_n258, new_n259, new_n260,
    new_n261, new_n262, new_n263, new_n264, new_n266, new_n267, new_n268,
    new_n269, new_n270, new_n271, new_n272, new_n273, new_n274, new_n275,
    new_n276, new_n277, new_n278, new_n279, new_n280, new_n281, new_n282,
    new_n283, new_n284, new_n285, new_n286, new_n287, new_n288, new_n289,
    new_n290, new_n291, new_n292, new_n293, new_n294, new_n296, new_n297,
    new_n298, new_n299, new_n300, new_n301, new_n302, new_n303, new_n304;
  assign new_n47 = pc0 & ~pi;
  assign new_n48 = ~pc0 & ~pu;
  assign new_n49 = ~pu & ~pi;
  assign new_n50 = ~new_n47 & ~new_n48;
  assign pd0 = new_n49 | ~new_n50;
  assign new_n52 = pc0 & ~pj;
  assign new_n53 = ~pc0 & ~pv;
  assign new_n54 = ~pv & ~pj;
  assign new_n55 = ~new_n52 & ~new_n53;
  assign pe0 = new_n54 | ~new_n55;
  assign new_n57 = pc0 & ~pk;
  assign new_n58 = ~pc0 & ~pw;
  assign new_n59 = ~pw & ~pk;
  assign new_n60 = ~new_n57 & ~new_n58;
  assign pf0 = new_n59 | ~new_n60;
  assign new_n62 = pc0 & ~pl;
  assign new_n63 = ~pc0 & ~px;
  assign new_n64 = ~px & ~pl;
  assign new_n65 = ~new_n62 & ~new_n63;
  assign pg0 = new_n64 | ~new_n65;
  assign new_n67 = pc0 & ~pm;
  assign new_n68 = ~pc0 & ~py;
  assign new_n69 = ~py & ~pm;
  assign new_n70 = ~new_n67 & ~new_n68;
  assign ph0 = new_n69 | ~new_n70;
  assign new_n72 = pc0 & ~pn;
  assign new_n73 = ~pc0 & ~pz;
  assign new_n74 = ~pz & ~pn;
  assign new_n75 = ~new_n72 & ~new_n73;
  assign pi0 = new_n74 | ~new_n75;
  assign new_n77 = pc0 & ~po;
  assign new_n78 = ~pa0 & ~pc0;
  assign new_n79 = ~pa0 & ~po;
  assign new_n80 = ~new_n77 & ~new_n78;
  assign pj0 = new_n79 | ~new_n80;
  assign new_n82 = ~pp & pc0;
  assign new_n83 = ~pb0 & ~pc0;
  assign new_n84 = ~pp & ~pb0;
  assign new_n85 = ~new_n82 & ~new_n83;
  assign pk0 = new_n84 | ~new_n85;
  assign new_n87 = ~pq & pa;
  assign new_n88 = pi & new_n87;
  assign new_n89 = ps & pa;
  assign new_n90 = ~pq & new_n89;
  assign new_n91 = pr & pu;
  assign new_n92 = pq & new_n91;
  assign new_n93 = ~pr & ~pu;
  assign new_n94 = pq & new_n93;
  assign new_n95 = ~pq & ~ps;
  assign new_n96 = pi & new_n95;
  assign new_n97 = ~pu & new_n89;
  assign new_n98 = ~pr & new_n97;
  assign new_n99 = ~pr & pa;
  assign new_n100 = ~pu & new_n99;
  assign new_n101 = pi & new_n100;
  assign new_n102 = pr & pa;
  assign new_n103 = pu & new_n102;
  assign new_n104 = pi & new_n103;
  assign new_n105 = pu & new_n89;
  assign new_n106 = pr & new_n105;
  assign new_n107 = ~ps & new_n91;
  assign new_n108 = pi & new_n107;
  assign new_n109 = ~ps & new_n93;
  assign new_n110 = pi & new_n109;
  assign new_n111 = ~new_n108 & ~new_n110;
  assign new_n112 = ~new_n101 & ~new_n104;
  assign new_n113 = ~new_n106 & new_n112;
  assign new_n114 = new_n111 & new_n113;
  assign new_n115 = ~new_n88 & ~new_n90;
  assign new_n116 = ~new_n92 & new_n115;
  assign new_n117 = ~new_n94 & ~new_n96;
  assign new_n118 = ~new_n98 & new_n117;
  assign new_n119 = new_n116 & new_n118;
  assign pm0 = ~new_n114 | ~new_n119;
  assign new_n121 = ~pj & new_n95;
  assign new_n122 = ~pr & pv;
  assign new_n123 = ~pu & new_n122;
  assign new_n124 = pq & new_n123;
  assign new_n125 = ~pq & ps;
  assign new_n126 = ~pb & new_n125;
  assign new_n127 = ~new_n121 & ~new_n124;
  assign new_n128 = ~new_n126 & new_n127;
  assign new_n129 = ~pq & new_n128;
  assign new_n130 = new_n93 & new_n128;
  assign new_n131 = pv & new_n128;
  assign new_n132 = ~new_n129 & ~new_n130;
  assign pn0 = new_n131 | ~new_n132;
  assign new_n134 = ~pc & new_n125;
  assign new_n135 = ~pk & new_n95;
  assign new_n136 = ~pq & ~pk;
  assign new_n137 = ~pc & new_n136;
  assign new_n138 = ~new_n134 & ~new_n135;
  assign new_n139 = ~new_n137 & new_n138;
  assign new_n140 = ~pv & pw;
  assign new_n141 = pw & new_n139;
  assign new_n142 = ~new_n140 & new_n141;
  assign new_n143 = ~pv & new_n93;
  assign new_n144 = new_n139 & new_n143;
  assign new_n145 = pu & new_n144;
  assign new_n146 = pu & new_n141;
  assign new_n147 = pr & new_n141;
  assign new_n148 = ~pq & new_n139;
  assign new_n149 = pr & new_n144;
  assign new_n150 = ~new_n140 & new_n144;
  assign new_n151 = ~new_n148 & ~new_n149;
  assign new_n152 = ~new_n150 & new_n151;
  assign new_n153 = ~new_n146 & ~new_n147;
  assign new_n154 = ~new_n142 & ~new_n145;
  assign new_n155 = new_n153 & new_n154;
  assign po0 = ~new_n152 | ~new_n155;
  assign new_n157 = ~pd & new_n125;
  assign new_n158 = ~pl & new_n95;
  assign new_n159 = ~pq & ~pl;
  assign new_n160 = ~pd & new_n159;
  assign new_n161 = ~new_n157 & ~new_n158;
  assign new_n162 = ~new_n160 & new_n161;
  assign new_n163 = ~pv & ~pw;
  assign new_n164 = px & new_n163;
  assign new_n165 = px & new_n162;
  assign new_n166 = ~new_n164 & new_n165;
  assign new_n167 = ~pu & ~pw;
  assign new_n168 = ~pr & ~pv;
  assign new_n169 = new_n167 & new_n168;
  assign new_n170 = new_n162 & new_n169;
  assign new_n171 = pu & new_n170;
  assign new_n172 = pu & new_n165;
  assign new_n173 = pr & new_n165;
  assign new_n174 = ~pq & new_n162;
  assign new_n175 = pr & new_n170;
  assign new_n176 = ~new_n164 & new_n170;
  assign new_n177 = ~new_n174 & ~new_n175;
  assign new_n178 = ~new_n176 & new_n177;
  assign new_n179 = ~new_n172 & ~new_n173;
  assign new_n180 = ~new_n166 & ~new_n171;
  assign new_n181 = new_n179 & new_n180;
  assign pp0 = ~new_n178 | ~new_n181;
  assign new_n183 = ~pe & new_n125;
  assign new_n184 = ~pm & new_n95;
  assign new_n185 = ~pq & ~pm;
  assign new_n186 = ~pe & new_n185;
  assign new_n187 = ~new_n183 & ~new_n184;
  assign new_n188 = ~new_n186 & new_n187;
  assign new_n189 = ~pv & ~px;
  assign new_n190 = ~pw & new_n189;
  assign new_n191 = py & new_n190;
  assign new_n192 = py & new_n188;
  assign new_n193 = ~new_n191 & new_n192;
  assign new_n194 = ~pw & ~px;
  assign new_n195 = ~pu & new_n194;
  assign new_n196 = new_n168 & new_n195;
  assign new_n197 = new_n188 & new_n196;
  assign new_n198 = pu & new_n197;
  assign new_n199 = pu & new_n192;
  assign new_n200 = pr & new_n192;
  assign new_n201 = ~pq & new_n188;
  assign new_n202 = pr & new_n197;
  assign new_n203 = ~new_n191 & new_n197;
  assign new_n204 = ~new_n201 & ~new_n202;
  assign new_n205 = ~new_n203 & new_n204;
  assign new_n206 = ~new_n199 & ~new_n200;
  assign new_n207 = ~new_n193 & ~new_n198;
  assign new_n208 = new_n206 & new_n207;
  assign pq0 = ~new_n205 | ~new_n208;
  assign new_n210 = ~pf & new_n125;
  assign new_n211 = ~pn & new_n95;
  assign new_n212 = ~pq & ~pn;
  assign new_n213 = ~pf & new_n212;
  assign new_n214 = ~new_n210 & ~new_n211;
  assign new_n215 = ~new_n213 & new_n214;
  assign new_n216 = ~pw & ~py;
  assign new_n217 = ~px & new_n216;
  assign new_n218 = pz & new_n217;
  assign new_n219 = pz & new_n215;
  assign new_n220 = ~new_n218 & new_n219;
  assign new_n221 = ~py & new_n194;
  assign new_n222 = ~pu & new_n221;
  assign new_n223 = new_n168 & new_n222;
  assign new_n224 = new_n215 & new_n223;
  assign new_n225 = pr & new_n224;
  assign new_n226 = pr & new_n219;
  assign new_n227 = ~pu & ~pv;
  assign new_n228 = new_n219 & ~new_n227;
  assign new_n229 = ~pq & new_n215;
  assign new_n230 = new_n224 & ~new_n227;
  assign new_n231 = ~new_n218 & new_n224;
  assign new_n232 = ~new_n229 & ~new_n230;
  assign new_n233 = ~new_n231 & new_n232;
  assign new_n234 = ~new_n226 & ~new_n228;
  assign new_n235 = ~new_n220 & ~new_n225;
  assign new_n236 = new_n234 & new_n235;
  assign pr0 = ~new_n233 | ~new_n236;
  assign new_n238 = ~pg & new_n125;
  assign new_n239 = ~po & new_n95;
  assign new_n240 = ~pq & ~po;
  assign new_n241 = ~pg & new_n240;
  assign new_n242 = ~new_n238 & ~new_n239;
  assign new_n243 = ~new_n241 & new_n242;
  assign new_n244 = ~px & ~pz;
  assign new_n245 = ~py & new_n244;
  assign new_n246 = pa0 & new_n245;
  assign new_n247 = pa0 & new_n243;
  assign new_n248 = ~new_n246 & new_n247;
  assign new_n249 = ~pz & new_n217;
  assign new_n250 = ~pu & new_n249;
  assign new_n251 = new_n168 & new_n250;
  assign new_n252 = new_n243 & new_n251;
  assign new_n253 = pr & new_n252;
  assign new_n254 = pr & new_n247;
  assign new_n255 = ~pw & new_n227;
  assign new_n256 = new_n247 & ~new_n255;
  assign new_n257 = ~pq & new_n243;
  assign new_n258 = new_n252 & ~new_n255;
  assign new_n259 = ~new_n246 & new_n252;
  assign new_n260 = ~new_n257 & ~new_n258;
  assign new_n261 = ~new_n259 & new_n260;
  assign new_n262 = ~new_n254 & ~new_n256;
  assign new_n263 = ~new_n248 & ~new_n253;
  assign new_n264 = new_n262 & new_n263;
  assign ps0 = ~new_n261 | ~new_n264;
  assign new_n266 = ~ph & new_n125;
  assign new_n267 = ~pp & new_n95;
  assign new_n268 = ~pp & ~pq;
  assign new_n269 = ~ph & new_n268;
  assign new_n270 = ~new_n266 & ~new_n267;
  assign new_n271 = ~new_n269 & new_n270;
  assign new_n272 = ~pa0 & ~py;
  assign new_n273 = ~pz & new_n272;
  assign new_n274 = pb0 & new_n273;
  assign new_n275 = pb0 & new_n271;
  assign new_n276 = ~new_n274 & new_n275;
  assign new_n277 = ~pa0 & new_n245;
  assign new_n278 = ~pu & new_n163;
  assign new_n279 = ~pr & new_n277;
  assign new_n280 = new_n278 & new_n279;
  assign new_n281 = new_n271 & new_n280;
  assign new_n282 = pr & new_n281;
  assign new_n283 = pr & new_n275;
  assign new_n284 = ~pv & new_n167;
  assign new_n285 = ~px & new_n284;
  assign new_n286 = new_n275 & ~new_n285;
  assign new_n287 = ~pq & new_n271;
  assign new_n288 = new_n281 & ~new_n285;
  assign new_n289 = ~new_n274 & new_n281;
  assign new_n290 = ~new_n287 & ~new_n288;
  assign new_n291 = ~new_n289 & new_n290;
  assign new_n292 = ~new_n283 & ~new_n286;
  assign new_n293 = ~new_n276 & ~new_n282;
  assign new_n294 = new_n292 & new_n293;
  assign pt0 = ~new_n291 | ~new_n294;
  assign new_n296 = ~pb0 & ~pz;
  assign new_n297 = new_n272 & new_n296;
  assign new_n298 = pu & ~pw;
  assign new_n299 = new_n189 & new_n298;
  assign new_n300 = ~pr & new_n297;
  assign new_n301 = new_n299 & new_n300;
  assign new_n302 = pq & new_n301;
  assign new_n303 = pr & pc0;
  assign new_n304 = pq & new_n303;
  assign pu0 = new_n302 | new_n304;
  assign pl0 = pc0;
endmodule


