// Benchmark "top" written by ABC on Mon Feb 19 11:52:43 2024

module top ( 
    pa1, pb2, pp, pa0, pc2, pq, pb0, pc1, pr, pa2, pb1, pc0, ps, pd0, pe1,
    pf2, pt, pd1, pe0, pg2, pu, pd2, pf0, pg1, pv, pe2, pf1, pg0, pw, ph0,
    pi1, px, ph1, pi0, py, ph2, pj0, pk1, pz, pj1, pk0, pl0, pm1, pl1, pm0,
    pn0, po1, pn1, po0, pp0, pq1, pa, pp1, pq0, pb, pr0, ps1, pr1, ps0, pd,
    pt0, pu1, pe, pt1, pu0, pf, pv0, pw1, pg, pv1, pw0, ph, px0, py1, pi,
    px1, py0, pj, pz0, pk, pz1, pl, pm, pn, po,
    pc3, pd4, pb3, pe4, pa3, pf4, pg4, pg3, pa4, pf3, pb4, pe3, pc4, pd3,
    pj2, pk3, pl4, pj3, pk2, pm4, pi3, pn4, ph3, pi2, po4, ph4, pn2, po3,
    pi4, pn3, po2, pj4, pl2, pm3, pk4, pl3, pm2, pr2, ps3, pt4, pr3, ps2,
    pu4, pp2, pq3, pv4, pp3, pq2, pp4, pv2, pw3, pq4, pv3, pw2, pr4, pt2,
    pu3, ps4, pt3, pu2, pz2, pz3, px2, py3, px3, py2  );
  input  pa1, pb2, pp, pa0, pc2, pq, pb0, pc1, pr, pa2, pb1, pc0, ps,
    pd0, pe1, pf2, pt, pd1, pe0, pg2, pu, pd2, pf0, pg1, pv, pe2, pf1, pg0,
    pw, ph0, pi1, px, ph1, pi0, py, ph2, pj0, pk1, pz, pj1, pk0, pl0, pm1,
    pl1, pm0, pn0, po1, pn1, po0, pp0, pq1, pa, pp1, pq0, pb, pr0, ps1,
    pr1, ps0, pd, pt0, pu1, pe, pt1, pu0, pf, pv0, pw1, pg, pv1, pw0, ph,
    px0, py1, pi, px1, py0, pj, pz0, pk, pz1, pl, pm, pn, po;
  output pc3, pd4, pb3, pe4, pa3, pf4, pg4, pg3, pa4, pf3, pb4, pe3, pc4, pd3,
    pj2, pk3, pl4, pj3, pk2, pm4, pi3, pn4, ph3, pi2, po4, ph4, pn2, po3,
    pi4, pn3, po2, pj4, pl2, pm3, pk4, pl3, pm2, pr2, ps3, pt4, pr3, ps2,
    pu4, pp2, pq3, pv4, pp3, pq2, pp4, pv2, pw3, pq4, pv3, pw2, pr4, pt2,
    pu3, ps4, pt3, pu2, pz2, pz3, px2, py3, px3, py2;
  wire new_n152, new_n153, new_n155, new_n156, new_n157, new_n158, new_n159,
    new_n160, new_n161, new_n162, new_n163, new_n164, new_n165, new_n166,
    new_n167, new_n168, new_n169, new_n170, new_n171, new_n172, new_n173,
    new_n174, new_n176, new_n177, new_n178, new_n179, new_n180, new_n181,
    new_n182, new_n183, new_n184, new_n185, new_n186, new_n187, new_n188,
    new_n189, new_n190, new_n191, new_n192, new_n193, new_n195, new_n196,
    new_n198, new_n199, new_n200, new_n201, new_n202, new_n203, new_n204,
    new_n206, new_n207, new_n209, new_n210, new_n211, new_n212, new_n214,
    new_n215, new_n216, new_n217, new_n219, new_n220, new_n221, new_n222,
    new_n223, new_n224, new_n225, new_n226, new_n227, new_n229, new_n230,
    new_n231, new_n232, new_n233, new_n234, new_n235, new_n236, new_n237,
    new_n238, new_n239, new_n240, new_n241, new_n242, new_n243, new_n244,
    new_n245, new_n246, new_n247, new_n248, new_n249, new_n251, new_n252,
    new_n253, new_n256, new_n257, new_n258, new_n259, new_n260, new_n262,
    new_n263, new_n264, new_n265, new_n266, new_n267, new_n268, new_n269,
    new_n270, new_n271, new_n272, new_n273, new_n274, new_n275, new_n276,
    new_n279, new_n280, new_n282, new_n283, new_n284, new_n285, new_n288,
    new_n289, new_n290, new_n293, new_n294, new_n295, new_n296, new_n297,
    new_n298, new_n299, new_n300, new_n301, new_n302, new_n303, new_n304,
    new_n305, new_n306, new_n307, new_n308, new_n309, new_n310, new_n311,
    new_n312, new_n313, new_n314, new_n315, new_n318, new_n319, new_n322,
    new_n323, new_n325, new_n326, new_n327, new_n329, new_n330, new_n331,
    new_n332, new_n333, new_n334, new_n335, new_n336, new_n337, new_n338,
    new_n339, new_n340, new_n341, new_n342, new_n343, new_n344, new_n345,
    new_n346, new_n348, new_n349, new_n350, new_n351, new_n353, new_n354,
    new_n357, new_n358, new_n359, new_n360, new_n363, new_n364, new_n366,
    new_n367, new_n368, new_n369, new_n372, new_n373, new_n376, new_n377,
    new_n380, new_n381, new_n382, new_n383, new_n384, new_n387, new_n388,
    new_n390, new_n391, new_n392, new_n393, new_n395, new_n396, new_n400,
    new_n401, new_n402, new_n403, new_n405, new_n406, new_n408, new_n409,
    new_n410, new_n411, new_n412, new_n413, new_n414, new_n415, new_n416,
    new_n417, new_n418, new_n419, new_n420, new_n422, new_n423, new_n426,
    new_n427, new_n428, new_n429, new_n430, new_n431, new_n432, new_n433,
    new_n434, new_n435, new_n436, new_n437, new_n438, new_n439, new_n440,
    new_n443, new_n444, new_n446, new_n447, new_n448, new_n449, new_n450,
    new_n451, new_n452, new_n453, new_n454, new_n456, new_n457, new_n460,
    new_n463, new_n464, new_n466, new_n467, new_n470, new_n471, new_n475,
    new_n476;
  assign new_n152 = pp0 & pu0;
  assign new_n153 = pn0 & ~pp0;
  assign pc3 = new_n152 | new_n153;
  assign new_n155 = pe2 & ~ph2;
  assign new_n156 = ~pe2 & ph2;
  assign new_n157 = ~new_n155 & ~new_n156;
  assign new_n158 = ~pb2 & pa2;
  assign new_n159 = py1 & new_n157;
  assign new_n160 = ~pb & new_n159;
  assign new_n161 = pc2 & new_n160;
  assign new_n162 = new_n158 & new_n161;
  assign new_n163 = ~pf2 & ~pe2;
  assign new_n164 = ~ph2 & new_n163;
  assign new_n165 = ~pg2 & new_n164;
  assign new_n166 = ~new_n157 & new_n165;
  assign new_n167 = ~pb & new_n166;
  assign new_n168 = pc2 & new_n167;
  assign new_n169 = new_n158 & new_n168;
  assign new_n170 = pp1 & new_n169;
  assign new_n171 = pd2 & new_n170;
  assign new_n172 = pq0 & new_n171;
  assign new_n173 = pb2 & ~pc2;
  assign new_n174 = ~pb & new_n173;
  assign pl2 = pb1 & new_n174;
  assign new_n176 = ~pa2 & pf;
  assign new_n177 = pl2 & new_n176;
  assign new_n178 = pq & new_n177;
  assign new_n179 = pq1 & new_n157;
  assign new_n180 = ~pb & new_n179;
  assign new_n181 = pc2 & new_n180;
  assign new_n182 = new_n158 & new_n181;
  assign new_n183 = ~pb & new_n176;
  assign new_n184 = new_n173 & new_n183;
  assign new_n185 = ~pb1 & new_n184;
  assign new_n186 = pi & new_n185;
  assign new_n187 = ~pp1 & new_n169;
  assign new_n188 = pd2 & new_n187;
  assign new_n189 = ~pq0 & new_n188;
  assign new_n190 = ~new_n182 & ~new_n186;
  assign new_n191 = ~new_n189 & new_n190;
  assign new_n192 = ~new_n162 & ~new_n172;
  assign new_n193 = ~new_n178 & new_n192;
  assign pd4 = ~new_n191 | ~new_n193;
  assign new_n195 = pp0 & pv0;
  assign new_n196 = pm0 & ~pp0;
  assign pb3 = new_n195 | new_n196;
  assign new_n198 = pj & new_n185;
  assign new_n199 = ~pb & new_n158;
  assign new_n200 = pc2 & new_n199;
  assign new_n201 = new_n157 & new_n200;
  assign new_n202 = pr1 & new_n201;
  assign new_n203 = pr & new_n177;
  assign new_n204 = ~new_n198 & ~new_n202;
  assign pe4 = new_n203 | ~new_n204;
  assign new_n206 = pp0 & pw0;
  assign new_n207 = pl0 & ~pp0;
  assign pa3 = new_n206 | new_n207;
  assign new_n209 = pk & new_n185;
  assign new_n210 = ps1 & new_n201;
  assign new_n211 = ps & new_n177;
  assign new_n212 = ~new_n209 & ~new_n210;
  assign pf4 = new_n211 | ~new_n212;
  assign new_n214 = pl & new_n185;
  assign new_n215 = pt1 & new_n201;
  assign new_n216 = pt & new_n177;
  assign new_n217 = ~new_n214 & ~new_n215;
  assign pg4 = new_n216 | ~new_n217;
  assign new_n219 = pb2 & pa2;
  assign new_n220 = ~ps0 & new_n219;
  assign new_n221 = pg & new_n220;
  assign new_n222 = ~pf & new_n221;
  assign new_n223 = ~pb & new_n222;
  assign new_n224 = ~pc2 & ~ps0;
  assign new_n225 = pg & new_n224;
  assign new_n226 = ~pf & new_n225;
  assign new_n227 = ~pb & new_n226;
  assign pg3 = new_n223 | new_n227;
  assign new_n229 = pf2 & pe2;
  assign new_n230 = ~new_n163 & ~new_n229;
  assign new_n231 = ~pg2 & new_n230;
  assign new_n232 = pg2 & ~new_n230;
  assign new_n233 = ~new_n231 & ~new_n232;
  assign new_n234 = ph2 & new_n233;
  assign new_n235 = ~ph2 & ~new_n233;
  assign new_n236 = ~new_n234 & ~new_n235;
  assign new_n237 = pc2 & ~pb;
  assign new_n238 = ~pf & new_n219;
  assign new_n239 = new_n237 & new_n238;
  assign new_n240 = pr0 & ~new_n236;
  assign new_n241 = new_n239 & new_n240;
  assign new_n242 = ~pr0 & new_n230;
  assign new_n243 = ~ph2 & new_n242;
  assign new_n244 = new_n239 & new_n243;
  assign new_n245 = ~pr0 & ~new_n230;
  assign new_n246 = ph2 & new_n245;
  assign new_n247 = new_n239 & new_n246;
  assign new_n248 = ~new_n244 & ~new_n247;
  assign new_n249 = ~new_n241 & new_n248;
  assign pa4 = pl1 & ~new_n249;
  assign new_n251 = ~pc2 & ~pf;
  assign new_n252 = pg & new_n251;
  assign new_n253 = ~pb & ~new_n252;
  assign pf3 = ~pa | ~new_n253;
  assign pb4 = pm1 & ~new_n249;
  assign new_n256 = pb2 & ~pa2;
  assign new_n257 = py1 & new_n256;
  assign new_n258 = new_n237 & new_n257;
  assign new_n259 = pp1 & new_n158;
  assign new_n260 = new_n237 & new_n259;
  assign pk2 = new_n258 | new_n260;
  assign new_n262 = new_n157 & pk2;
  assign new_n263 = ~pq0 & new_n262;
  assign new_n264 = pd2 & ~px1;
  assign new_n265 = new_n229 & new_n264;
  assign new_n266 = ~pg2 & ps0;
  assign new_n267 = ph2 & new_n266;
  assign new_n268 = new_n265 & new_n267;
  assign new_n269 = pa2 & ~new_n268;
  assign new_n270 = ~pr0 & new_n269;
  assign new_n271 = pb2 & pf;
  assign new_n272 = ~pb & new_n270;
  assign new_n273 = new_n271 & new_n272;
  assign new_n274 = ~pc2 & ~pb;
  assign new_n275 = new_n271 & new_n274;
  assign new_n276 = ~new_n273 & ~new_n275;
  assign pe3 = new_n263 | ~new_n276;
  assign pc4 = pn1 & ~new_n249;
  assign new_n279 = pp0 & pt0;
  assign new_n280 = po0 & ~pp0;
  assign pd3 = new_n279 | new_n280;
  assign new_n282 = ~pb & new_n219;
  assign new_n283 = ~pb2 & ~pa2;
  assign new_n284 = ~pb & new_n283;
  assign new_n285 = ~new_n282 & ~new_n284;
  assign pj2 = new_n274 | ~new_n285;
  assign pk3 = pv0 & ~new_n248;
  assign new_n288 = new_n163 & new_n239;
  assign new_n289 = pg2 & new_n288;
  assign new_n290 = ph2 & new_n289;
  assign pl4 = ~pd2 & new_n290;
  assign pj3 = pu0 & ~new_n248;
  assign new_n293 = pz1 & ~new_n219;
  assign new_n294 = new_n157 & new_n293;
  assign new_n295 = pc2 & new_n294;
  assign new_n296 = ~pd2 & ~new_n219;
  assign new_n297 = ~new_n157 & new_n296;
  assign new_n298 = pc2 & new_n297;
  assign new_n299 = ~pq0 & ~py1;
  assign new_n300 = pc2 & new_n299;
  assign new_n301 = ~new_n157 & new_n300;
  assign new_n302 = ~pa2 & new_n301;
  assign new_n303 = ~new_n165 & ~new_n219;
  assign new_n304 = ~new_n157 & new_n303;
  assign new_n305 = pc2 & new_n304;
  assign new_n306 = pq0 & py1;
  assign new_n307 = pc2 & new_n306;
  assign new_n308 = ~new_n157 & new_n307;
  assign new_n309 = ~pa2 & new_n308;
  assign new_n310 = ~new_n305 & ~new_n309;
  assign new_n311 = ~pb & ~new_n302;
  assign new_n312 = new_n310 & new_n311;
  assign new_n313 = ~new_n295 & ~new_n298;
  assign new_n314 = pa & ~new_n283;
  assign new_n315 = new_n313 & new_n314;
  assign pm4 = ~new_n312 | ~new_n315;
  assign pi3 = pt0 & ~new_n248;
  assign new_n318 = ~pb & pe;
  assign new_n319 = ~pd & new_n318;
  assign pn4 = pa & new_n319;
  assign ph3 = pg & ~new_n248;
  assign new_n322 = pb1 & pp0;
  assign new_n323 = ~pb & ~new_n219;
  assign pm2 = new_n237 | new_n323;
  assign new_n325 = pp0 & pm2;
  assign new_n326 = ~pp0 & ph;
  assign new_n327 = ~new_n322 & ~new_n325;
  assign pi2 = new_n326 | ~new_n327;
  assign new_n329 = ~pr0 & new_n158;
  assign new_n330 = ~pc2 & new_n329;
  assign new_n331 = pp0 & new_n330;
  assign new_n332 = ~pb & new_n331;
  assign new_n333 = ~pr0 & new_n268;
  assign new_n334 = new_n237 & new_n333;
  assign new_n335 = pa2 & new_n334;
  assign new_n336 = new_n271 & new_n335;
  assign new_n337 = ~pc2 & new_n256;
  assign new_n338 = ~pb & new_n337;
  assign new_n339 = ~pp0 & pr0;
  assign new_n340 = new_n237 & new_n339;
  assign new_n341 = pa2 & new_n340;
  assign new_n342 = new_n271 & new_n341;
  assign new_n343 = ~pb & new_n251;
  assign new_n344 = ~new_n342 & ~new_n343;
  assign new_n345 = ~new_n332 & ~new_n336;
  assign new_n346 = ~new_n338 & new_n345;
  assign po4 = ~new_n344 | ~new_n346;
  assign new_n348 = pm & new_n185;
  assign new_n349 = pu1 & new_n201;
  assign new_n350 = pu & new_n177;
  assign new_n351 = ~new_n348 & ~new_n349;
  assign ph4 = new_n350 | ~new_n351;
  assign new_n353 = po1 & pp0;
  assign new_n354 = py & ~pp0;
  assign pn2 = new_n353 | new_n354;
  assign po3 = pz0 & ~new_n248;
  assign new_n357 = pn & new_n185;
  assign new_n358 = pv1 & new_n201;
  assign new_n359 = pv & new_n177;
  assign new_n360 = ~new_n357 & ~new_n358;
  assign pi4 = new_n359 | ~new_n360;
  assign pn3 = py0 & ~new_n248;
  assign new_n363 = pn1 & pp0;
  assign new_n364 = pz & ~pp0;
  assign po2 = new_n363 | new_n364;
  assign new_n366 = po & new_n185;
  assign new_n367 = pw1 & new_n201;
  assign new_n368 = pw & new_n177;
  assign new_n369 = ~new_n366 & ~new_n367;
  assign pj4 = new_n368 | ~new_n369;
  assign pm3 = px0 & ~new_n248;
  assign new_n372 = pp & new_n185;
  assign new_n373 = px & new_n177;
  assign pk4 = new_n372 | new_n373;
  assign pl3 = pw0 & ~new_n248;
  assign new_n376 = pk1 & pp0;
  assign new_n377 = pc0 & ~pp0;
  assign pr2 = new_n376 | new_n377;
  assign ps3 = pd1 & ~new_n249;
  assign new_n380 = new_n157 & new_n158;
  assign new_n381 = new_n157 & new_n256;
  assign new_n382 = ~new_n380 & ~new_n381;
  assign new_n383 = ~new_n238 & new_n382;
  assign new_n384 = new_n237 & ~new_n383;
  assign pt4 = pf2 & new_n384;
  assign pr3 = pc1 & ~new_n249;
  assign new_n387 = pj1 & pp0;
  assign new_n388 = pd0 & ~pp0;
  assign ps2 = new_n387 | new_n388;
  assign new_n390 = ~pa2 & new_n173;
  assign new_n391 = pf & new_n390;
  assign new_n392 = ~pb & new_n391;
  assign new_n393 = pg2 & new_n384;
  assign pu4 = new_n392 | new_n393;
  assign new_n395 = pm1 & pp0;
  assign new_n396 = pa0 & ~pp0;
  assign pp2 = new_n395 | new_n396;
  assign pq3 = pb1 & ~new_n249;
  assign pv4 = ph2 & new_n384;
  assign new_n400 = pr0 & new_n239;
  assign new_n401 = ~new_n236 & new_n400;
  assign new_n402 = pg & new_n401;
  assign new_n403 = pa1 & ~new_n248;
  assign pp3 = new_n402 | new_n403;
  assign new_n405 = pl1 & pp0;
  assign new_n406 = pb0 & ~pp0;
  assign pq2 = new_n405 | new_n406;
  assign new_n408 = pa2 & new_n271;
  assign new_n409 = pp0 & new_n408;
  assign new_n410 = ~pb & new_n409;
  assign new_n411 = pa2 & new_n173;
  assign new_n412 = ~pb & new_n411;
  assign new_n413 = ~pc2 & pa2;
  assign new_n414 = pp0 & new_n413;
  assign new_n415 = ~pb & new_n414;
  assign new_n416 = new_n270 & new_n271;
  assign new_n417 = ~new_n251 & ~new_n416;
  assign new_n418 = ~pb & ~new_n417;
  assign new_n419 = ~new_n410 & ~new_n412;
  assign new_n420 = ~new_n415 & ~new_n418;
  assign pp4 = ~new_n419 | ~new_n420;
  assign new_n422 = pg1 & pp0;
  assign new_n423 = pg0 & ~pp0;
  assign pv2 = new_n422 | new_n423;
  assign pw3 = ph1 & ~new_n249;
  assign new_n426 = ~new_n157 & new_n256;
  assign new_n427 = ~pd2 & new_n426;
  assign new_n428 = ~pb & new_n427;
  assign new_n429 = pc2 & new_n158;
  assign new_n430 = ~new_n157 & new_n429;
  assign new_n431 = ~new_n165 & new_n430;
  assign new_n432 = ~pb & new_n431;
  assign new_n433 = ~new_n165 & new_n426;
  assign new_n434 = ~pb & new_n433;
  assign new_n435 = ~pd2 & new_n430;
  assign new_n436 = ~pb & new_n435;
  assign new_n437 = ~new_n418 & ~new_n434;
  assign new_n438 = ~new_n436 & new_n437;
  assign new_n439 = ~new_n428 & ~new_n432;
  assign new_n440 = ~new_n174 & new_n439;
  assign pq4 = ~new_n438 | ~new_n440;
  assign pv3 = pg1 & ~new_n249;
  assign new_n443 = pa1 & pp0;
  assign new_n444 = ph0 & ~pp0;
  assign pw2 = new_n443 | new_n444;
  assign new_n446 = pd2 & new_n158;
  assign new_n447 = new_n165 & new_n446;
  assign new_n448 = new_n237 & new_n447;
  assign new_n449 = pd2 & new_n256;
  assign new_n450 = new_n165 & new_n449;
  assign new_n451 = new_n237 & new_n450;
  assign new_n452 = pd2 & ~new_n383;
  assign new_n453 = new_n237 & new_n452;
  assign new_n454 = ~new_n448 & ~new_n451;
  assign pr4 = new_n453 | ~new_n454;
  assign new_n456 = pi1 & pp0;
  assign new_n457 = pe0 & ~pp0;
  assign pt2 = new_n456 | new_n457;
  assign pu3 = pf1 & ~new_n249;
  assign new_n460 = pe2 & new_n384;
  assign ps4 = ~new_n276 | new_n460;
  assign pt3 = pe1 & ~new_n249;
  assign new_n463 = ph1 & pp0;
  assign new_n464 = pf0 & ~pp0;
  assign pu2 = new_n463 | new_n464;
  assign new_n466 = pp0 & px0;
  assign new_n467 = pk0 & ~pp0;
  assign pz2 = new_n466 | new_n467;
  assign pz3 = pk1 & ~new_n249;
  assign new_n470 = pp0 & pz0;
  assign new_n471 = pi0 & ~pp0;
  assign px2 = new_n470 | new_n471;
  assign py3 = pj1 & ~new_n249;
  assign px3 = pi1 & ~new_n249;
  assign new_n475 = pp0 & py0;
  assign new_n476 = pj0 & ~pp0;
  assign py2 = new_n475 | new_n476;
endmodule


