// Benchmark "top" written by ABC on Mon Feb 19 11:52:42 2024

module top ( 
    pp, pa0, pq, pb0, pr, pc0, ps, pd0, pt, pe0, pu, pf0, pv, pg0, pw, ph0,
    px, pi0, py, pj0, pz, pk0, pl0, pm0, pn0, po0, pp0, pa, pq0, pr0, pc,
    ps0, pd, pt0, pe, pu0, pf, pv0, pg, ph, pi, pj, pk, pl, pm, pn, po,
    pa1, pb2, pc2, pc1, pa2, pb1, pe1, pf2, pd1, pd2, pg1, pe2, pf1, pi1,
    ph1, pk1, pj1, pm1, pl1, po1, pn1, pq1, pp1, ps1, pr1, pu1, pt1, pw1,
    pv1, pw0, px0, py1, px1, py0, pz0, pz1  );
  input  pp, pa0, pq, pb0, pr, pc0, ps, pd0, pt, pe0, pu, pf0, pv, pg0,
    pw, ph0, px, pi0, py, pj0, pz, pk0, pl0, pm0, pn0, po0, pp0, pa, pq0,
    pr0, pc, ps0, pd, pt0, pe, pu0, pf, pv0, pg, ph, pi, pj, pk, pl, pm,
    pn, po;
  output pa1, pb2, pc2, pc1, pa2, pb1, pe1, pf2, pd1, pd2, pg1, pe2, pf1, pi1,
    ph1, pk1, pj1, pm1, pl1, po1, pn1, pq1, pp1, ps1, pr1, pu1, pt1, pw1,
    pv1, pw0, px0, py1, px1, py0, pz0, pz1;
  wire new_n84, new_n85, new_n86, new_n87, new_n88, new_n89, new_n90,
    new_n92, new_n93, new_n94, new_n95, new_n96, new_n97, new_n98, new_n99,
    new_n100, new_n101, new_n102, new_n104, new_n105, new_n106, new_n107,
    new_n108, new_n109, new_n110, new_n111, new_n112, new_n113, new_n115,
    new_n116, new_n117, new_n118, new_n119, new_n120, new_n121, new_n123,
    new_n124, new_n125, new_n126, new_n127, new_n128, new_n129, new_n130,
    new_n131, new_n132, new_n134, new_n135, new_n136, new_n137, new_n138,
    new_n139, new_n140, new_n142, new_n143, new_n144, new_n145, new_n146,
    new_n147, new_n148, new_n150, new_n151, new_n152, new_n153, new_n154,
    new_n155, new_n156, new_n157, new_n158, new_n159, new_n161, new_n162,
    new_n163, new_n164, new_n165, new_n167, new_n168, new_n169, new_n170,
    new_n171, new_n172, new_n173, new_n174, new_n175, new_n176, new_n178,
    new_n179, new_n180, new_n181, new_n182, new_n183, new_n184, new_n186,
    new_n187, new_n188, new_n189, new_n190, new_n191, new_n192, new_n193,
    new_n194, new_n196, new_n197, new_n198, new_n199, new_n200, new_n202,
    new_n203, new_n204, new_n205, new_n206, new_n207, new_n208, new_n210,
    new_n211, new_n212, new_n213, new_n214, new_n216, new_n217, new_n218,
    new_n219, new_n220, new_n221, new_n222, new_n224, new_n225, new_n226,
    new_n227, new_n228, new_n230, new_n231, new_n232, new_n233, new_n234,
    new_n235, new_n236, new_n238, new_n239, new_n240, new_n241, new_n242,
    new_n244, new_n245, new_n246, new_n247, new_n248, new_n249, new_n250,
    new_n252, new_n253, new_n254, new_n255, new_n256, new_n258, new_n259,
    new_n260, new_n261, new_n262, new_n263, new_n264, new_n266, new_n267,
    new_n268, new_n269, new_n270, new_n271, new_n273, new_n274, new_n275,
    new_n276, new_n277, new_n278, new_n279, new_n281, new_n282, new_n283,
    new_n284, new_n285, new_n287, new_n288, new_n289, new_n290, new_n291,
    new_n292, new_n293, new_n295, new_n296, new_n297, new_n298, new_n299,
    new_n301, new_n302, new_n303, new_n304, new_n305, new_n306, new_n307,
    new_n309, new_n310, new_n311, new_n312, new_n313, new_n315, new_n316,
    new_n317, new_n318, new_n319, new_n320, new_n321, new_n323, new_n324,
    new_n325, new_n326, new_n327, new_n328, new_n329, new_n331, new_n332,
    new_n333, new_n334, new_n335, new_n336, new_n337, new_n338, new_n339,
    new_n340, new_n341, new_n343, new_n344, new_n345, new_n346, new_n347,
    new_n348, new_n349, new_n350, new_n351, new_n352, new_n353, new_n354,
    new_n355, new_n356, new_n357, new_n358, new_n359, new_n360, new_n361,
    new_n362, new_n363, new_n365, new_n366, new_n367, new_n368, new_n369,
    new_n370, new_n371, new_n373, new_n374, new_n375, new_n376, new_n377,
    new_n378, new_n379, new_n381, new_n382, new_n383, new_n384, new_n385,
    new_n386, new_n387, new_n388, new_n389;
  assign new_n84 = pd & ~pl;
  assign new_n85 = pi & new_n84;
  assign new_n86 = pq & pd;
  assign new_n87 = ~pl & new_n86;
  assign new_n88 = pq & ~pl;
  assign new_n89 = ~pi & new_n88;
  assign new_n90 = ~new_n85 & ~new_n87;
  assign pa1 = new_n89 | ~new_n90;
  assign new_n92 = ps0 & ~pl;
  assign new_n93 = ~pp & new_n92;
  assign new_n94 = pk & new_n93;
  assign new_n95 = pp & pr0;
  assign new_n96 = ~pl & new_n95;
  assign new_n97 = pr0 & ps0;
  assign new_n98 = ~pl & new_n97;
  assign new_n99 = pr0 & ~pl;
  assign new_n100 = ~pk & new_n99;
  assign new_n101 = ~new_n94 & ~new_n96;
  assign new_n102 = ~new_n98 & ~new_n100;
  assign pb2 = ~new_n101 | ~new_n102;
  assign new_n104 = pt0 & ~pl;
  assign new_n105 = ~pp & new_n104;
  assign new_n106 = pk & new_n105;
  assign new_n107 = pp & ps0;
  assign new_n108 = ~pl & new_n107;
  assign new_n109 = ps0 & pt0;
  assign new_n110 = ~pl & new_n109;
  assign new_n111 = ~pk & new_n92;
  assign new_n112 = ~new_n106 & ~new_n108;
  assign new_n113 = ~new_n110 & ~new_n111;
  assign pc2 = ~new_n112 | ~new_n113;
  assign new_n115 = ps & ~pl;
  assign new_n116 = ~pj & new_n115;
  assign new_n117 = ps & pt;
  assign new_n118 = ~pl & new_n117;
  assign new_n119 = pt & ~pl;
  assign new_n120 = pj & new_n119;
  assign new_n121 = ~new_n116 & ~new_n118;
  assign pc1 = new_n120 | ~new_n121;
  assign new_n123 = ~pp & new_n99;
  assign new_n124 = pk & new_n123;
  assign new_n125 = pp & pq0;
  assign new_n126 = ~pl & new_n125;
  assign new_n127 = pq0 & pr0;
  assign new_n128 = ~pl & new_n127;
  assign new_n129 = pq0 & ~pl;
  assign new_n130 = ~pk & new_n129;
  assign new_n131 = ~new_n124 & ~new_n126;
  assign new_n132 = ~new_n128 & ~new_n130;
  assign pa2 = ~new_n131 | ~new_n132;
  assign new_n134 = pe & ~pl;
  assign new_n135 = pi & new_n134;
  assign new_n136 = pr & pe;
  assign new_n137 = ~pl & new_n136;
  assign new_n138 = pr & ~pl;
  assign new_n139 = ~pi & new_n138;
  assign new_n140 = ~new_n135 & ~new_n137;
  assign pb1 = new_n139 | ~new_n140;
  assign new_n142 = pu & ~pl;
  assign new_n143 = ~pj & new_n142;
  assign new_n144 = pu & pv;
  assign new_n145 = ~pl & new_n144;
  assign new_n146 = pv & ~pl;
  assign new_n147 = pj & new_n146;
  assign new_n148 = ~new_n143 & ~new_n145;
  assign pe1 = new_n147 | ~new_n148;
  assign new_n150 = ~pp & pk;
  assign new_n151 = ~pl & new_n150;
  assign new_n152 = pa & new_n151;
  assign new_n153 = pv0 & ~pl;
  assign new_n154 = ~pk & new_n153;
  assign new_n155 = pp & pv0;
  assign new_n156 = ~pl & new_n155;
  assign new_n157 = pa & new_n153;
  assign new_n158 = ~new_n152 & ~new_n154;
  assign new_n159 = ~new_n156 & ~new_n157;
  assign pf2 = ~new_n158 | ~new_n159;
  assign new_n161 = ~pj & new_n119;
  assign new_n162 = pt & pu;
  assign new_n163 = ~pl & new_n162;
  assign new_n164 = pj & new_n142;
  assign new_n165 = ~new_n161 & ~new_n163;
  assign pd1 = new_n164 | ~new_n165;
  assign new_n167 = pu0 & ~pl;
  assign new_n168 = ~pp & new_n167;
  assign new_n169 = pk & new_n168;
  assign new_n170 = pp & pt0;
  assign new_n171 = ~pl & new_n170;
  assign new_n172 = pt0 & pu0;
  assign new_n173 = ~pl & new_n172;
  assign new_n174 = ~pk & new_n104;
  assign new_n175 = ~new_n169 & ~new_n171;
  assign new_n176 = ~new_n173 & ~new_n174;
  assign pd2 = ~new_n175 | ~new_n176;
  assign new_n178 = pw & ~pl;
  assign new_n179 = ~pj & new_n178;
  assign new_n180 = pw & px;
  assign new_n181 = ~pl & new_n180;
  assign new_n182 = px & ~pl;
  assign new_n183 = pj & new_n182;
  assign new_n184 = ~new_n179 & ~new_n181;
  assign pg1 = new_n183 | ~new_n184;
  assign new_n186 = ~pp & new_n153;
  assign new_n187 = pk & new_n186;
  assign new_n188 = pp & pu0;
  assign new_n189 = ~pl & new_n188;
  assign new_n190 = pu0 & pv0;
  assign new_n191 = ~pl & new_n190;
  assign new_n192 = ~pk & new_n167;
  assign new_n193 = ~new_n187 & ~new_n189;
  assign new_n194 = ~new_n191 & ~new_n192;
  assign pe2 = ~new_n193 | ~new_n194;
  assign new_n196 = ~pj & new_n146;
  assign new_n197 = pv & pw;
  assign new_n198 = ~pl & new_n197;
  assign new_n199 = pj & new_n178;
  assign new_n200 = ~new_n196 & ~new_n198;
  assign pf1 = new_n199 | ~new_n200;
  assign new_n202 = py & ~pl;
  assign new_n203 = ~pj & new_n202;
  assign new_n204 = py & pz;
  assign new_n205 = ~pl & new_n204;
  assign new_n206 = pz & ~pl;
  assign new_n207 = pj & new_n206;
  assign new_n208 = ~new_n203 & ~new_n205;
  assign pi1 = new_n207 | ~new_n208;
  assign new_n210 = ~pj & new_n182;
  assign new_n211 = px & py;
  assign new_n212 = ~pl & new_n211;
  assign new_n213 = pj & new_n202;
  assign new_n214 = ~new_n210 & ~new_n212;
  assign ph1 = new_n213 | ~new_n214;
  assign new_n216 = pa0 & ~pl;
  assign new_n217 = ~pj & new_n216;
  assign new_n218 = pa0 & pb0;
  assign new_n219 = ~pl & new_n218;
  assign new_n220 = pb0 & ~pl;
  assign new_n221 = pj & new_n220;
  assign new_n222 = ~new_n217 & ~new_n219;
  assign pk1 = new_n221 | ~new_n222;
  assign new_n224 = ~pj & new_n206;
  assign new_n225 = pa0 & pz;
  assign new_n226 = ~pl & new_n225;
  assign new_n227 = pj & new_n216;
  assign new_n228 = ~new_n224 & ~new_n226;
  assign pj1 = new_n227 | ~new_n228;
  assign new_n230 = pc0 & ~pl;
  assign new_n231 = ~pj & new_n230;
  assign new_n232 = pc0 & pd0;
  assign new_n233 = ~pl & new_n232;
  assign new_n234 = pd0 & ~pl;
  assign new_n235 = pj & new_n234;
  assign new_n236 = ~new_n231 & ~new_n233;
  assign pm1 = new_n235 | ~new_n236;
  assign new_n238 = ~pj & new_n220;
  assign new_n239 = pb0 & pc0;
  assign new_n240 = ~pl & new_n239;
  assign new_n241 = pj & new_n230;
  assign new_n242 = ~new_n238 & ~new_n240;
  assign pl1 = new_n241 | ~new_n242;
  assign new_n244 = pe0 & ~pl;
  assign new_n245 = ~pj & new_n244;
  assign new_n246 = pe0 & pf0;
  assign new_n247 = ~pl & new_n246;
  assign new_n248 = pf0 & ~pl;
  assign new_n249 = pj & new_n248;
  assign new_n250 = ~new_n245 & ~new_n247;
  assign po1 = new_n249 | ~new_n250;
  assign new_n252 = ~pj & new_n234;
  assign new_n253 = pd0 & pe0;
  assign new_n254 = ~pl & new_n253;
  assign new_n255 = pj & new_n244;
  assign new_n256 = ~new_n252 & ~new_n254;
  assign pn1 = new_n255 | ~new_n256;
  assign new_n258 = pg0 & ~pl;
  assign new_n259 = ~pk & new_n258;
  assign new_n260 = pg0 & ph0;
  assign new_n261 = ~pl & new_n260;
  assign new_n262 = ph0 & ~pl;
  assign new_n263 = pk & new_n262;
  assign new_n264 = ~new_n259 & ~new_n261;
  assign pq1 = new_n263 | ~new_n264;
  assign new_n266 = pa & ~pl;
  assign new_n267 = pj & new_n266;
  assign new_n268 = pf0 & pa;
  assign new_n269 = ~pl & new_n268;
  assign new_n270 = ~pj & new_n248;
  assign new_n271 = ~new_n267 & ~new_n269;
  assign pp1 = new_n270 | ~new_n271;
  assign new_n273 = pi0 & ~pl;
  assign new_n274 = ~pk & new_n273;
  assign new_n275 = pi0 & pj0;
  assign new_n276 = ~pl & new_n275;
  assign new_n277 = pj0 & ~pl;
  assign new_n278 = pk & new_n277;
  assign new_n279 = ~new_n274 & ~new_n276;
  assign ps1 = new_n278 | ~new_n279;
  assign new_n281 = ~pk & new_n262;
  assign new_n282 = ph0 & pi0;
  assign new_n283 = ~pl & new_n282;
  assign new_n284 = pk & new_n273;
  assign new_n285 = ~new_n281 & ~new_n283;
  assign pr1 = new_n284 | ~new_n285;
  assign new_n287 = pk0 & ~pl;
  assign new_n288 = ~pk & new_n287;
  assign new_n289 = pk0 & pl0;
  assign new_n290 = ~pl & new_n289;
  assign new_n291 = pl0 & ~pl;
  assign new_n292 = pk & new_n291;
  assign new_n293 = ~new_n288 & ~new_n290;
  assign pu1 = new_n292 | ~new_n293;
  assign new_n295 = ~pk & new_n277;
  assign new_n296 = pj0 & pk0;
  assign new_n297 = ~pl & new_n296;
  assign new_n298 = pk & new_n287;
  assign new_n299 = ~new_n295 & ~new_n297;
  assign pt1 = new_n298 | ~new_n299;
  assign new_n301 = pm0 & ~pl;
  assign new_n302 = ~pk & new_n301;
  assign new_n303 = pm0 & pn0;
  assign new_n304 = ~pl & new_n303;
  assign new_n305 = pn0 & ~pl;
  assign new_n306 = pk & new_n305;
  assign new_n307 = ~new_n302 & ~new_n304;
  assign pw1 = new_n306 | ~new_n307;
  assign new_n309 = ~pk & new_n291;
  assign new_n310 = pl0 & pm0;
  assign new_n311 = ~pl & new_n310;
  assign new_n312 = pk & new_n301;
  assign new_n313 = ~new_n309 & ~new_n311;
  assign pv1 = new_n312 | ~new_n313;
  assign new_n315 = pf & ~pl;
  assign new_n316 = pi & new_n315;
  assign new_n317 = pf & pm;
  assign new_n318 = ~pl & new_n317;
  assign new_n319 = ~pl & pm;
  assign new_n320 = ~pi & new_n319;
  assign new_n321 = ~new_n316 & ~new_n318;
  assign pw0 = new_n320 | ~new_n321;
  assign new_n323 = pg & ~pl;
  assign new_n324 = pi & new_n323;
  assign new_n325 = pg & pn;
  assign new_n326 = ~pl & new_n325;
  assign new_n327 = ~pl & pn;
  assign new_n328 = ~pi & new_n327;
  assign new_n329 = ~new_n324 & ~new_n326;
  assign px0 = new_n328 | ~new_n329;
  assign new_n331 = pp0 & ~pl;
  assign new_n332 = ~pp & new_n331;
  assign new_n333 = pk & new_n332;
  assign new_n334 = pp & po0;
  assign new_n335 = ~pl & new_n334;
  assign new_n336 = po0 & pp0;
  assign new_n337 = ~pl & new_n336;
  assign new_n338 = po0 & ~pl;
  assign new_n339 = ~pk & new_n338;
  assign new_n340 = ~new_n333 & ~new_n335;
  assign new_n341 = ~new_n337 & ~new_n339;
  assign py1 = ~new_n340 | ~new_n341;
  assign new_n343 = po0 & pk;
  assign new_n344 = ~pl & new_n343;
  assign new_n345 = pa & new_n344;
  assign new_n346 = ~pp & new_n338;
  assign new_n347 = pk & new_n346;
  assign new_n348 = pp & pk;
  assign new_n349 = ~pl & new_n348;
  assign new_n350 = pa & new_n349;
  assign new_n351 = pp & new_n305;
  assign new_n352 = pa & new_n351;
  assign new_n353 = ~pk & new_n305;
  assign new_n354 = pn0 & new_n338;
  assign new_n355 = pa & new_n354;
  assign new_n356 = ~pp & po0;
  assign new_n357 = pn0 & new_n356;
  assign new_n358 = ~pl & new_n357;
  assign new_n359 = ~new_n345 & ~new_n347;
  assign new_n360 = ~new_n350 & ~new_n352;
  assign new_n361 = new_n359 & new_n360;
  assign new_n362 = ~new_n353 & ~new_n355;
  assign new_n363 = ~new_n358 & new_n362;
  assign px1 = ~new_n361 | ~new_n363;
  assign new_n365 = ph & ~pl;
  assign new_n366 = pi & new_n365;
  assign new_n367 = ph & po;
  assign new_n368 = ~pl & new_n367;
  assign new_n369 = ~pl & po;
  assign new_n370 = ~pi & new_n369;
  assign new_n371 = ~new_n366 & ~new_n368;
  assign py0 = new_n370 | ~new_n371;
  assign new_n373 = pc & ~pl;
  assign new_n374 = pi & new_n373;
  assign new_n375 = pp & pc;
  assign new_n376 = ~pl & new_n375;
  assign new_n377 = pp & ~pl;
  assign new_n378 = ~pi & new_n377;
  assign new_n379 = ~new_n374 & ~new_n376;
  assign pz0 = new_n378 | ~new_n379;
  assign new_n381 = ~pp & new_n129;
  assign new_n382 = pk & new_n381;
  assign new_n383 = pp & pp0;
  assign new_n384 = ~pl & new_n383;
  assign new_n385 = pp0 & pq0;
  assign new_n386 = ~pl & new_n385;
  assign new_n387 = ~pk & new_n331;
  assign new_n388 = ~new_n382 & ~new_n384;
  assign new_n389 = ~new_n386 & ~new_n387;
  assign pz1 = ~new_n388 | ~new_n389;
endmodule


