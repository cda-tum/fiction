// Benchmark "b04" written by ABC on Wed Sep  5 10:17:19 2018

module b04 ( clock, 
    RESTART, AVERAGE, ENABLE, DATA_IN_7_, DATA_IN_6_, DATA_IN_5_,
    DATA_IN_4_, DATA_IN_3_, DATA_IN_2_, DATA_IN_1_, DATA_IN_0_,
    DATA_OUT_REG_7_, DATA_OUT_REG_6_, DATA_OUT_REG_5_, DATA_OUT_REG_4_,
    DATA_OUT_REG_3_, DATA_OUT_REG_2_, DATA_OUT_REG_1_, DATA_OUT_REG_0_  );
  input  clock;
  input  RESTART, AVERAGE, ENABLE, DATA_IN_7_, DATA_IN_6_, DATA_IN_5_,
    DATA_IN_4_, DATA_IN_3_, DATA_IN_2_, DATA_IN_1_, DATA_IN_0_;
  output DATA_OUT_REG_7_, DATA_OUT_REG_6_, DATA_OUT_REG_5_, DATA_OUT_REG_4_,
    DATA_OUT_REG_3_, DATA_OUT_REG_2_, DATA_OUT_REG_1_, DATA_OUT_REG_0_;
  reg RMAX_REG_7_, RMAX_REG_6_, RMAX_REG_5_, RMAX_REG_4_, RMAX_REG_3_,
    RMAX_REG_2_, RMAX_REG_1_, RMAX_REG_0_, RMIN_REG_7_, RMIN_REG_6_,
    RMIN_REG_5_, RMIN_REG_4_, RMIN_REG_3_, RMIN_REG_2_, RMIN_REG_1_,
    RMIN_REG_0_, RLAST_REG_7_, RLAST_REG_6_, RLAST_REG_5_, RLAST_REG_4_,
    RLAST_REG_3_, RLAST_REG_2_, RLAST_REG_1_, RLAST_REG_0_, REG1_REG_7_,
    REG1_REG_6_, REG1_REG_5_, REG1_REG_4_, REG1_REG_3_, REG1_REG_2_,
    REG1_REG_1_, REG1_REG_0_, REG2_REG_7_, REG2_REG_6_, REG2_REG_5_,
    REG2_REG_4_, REG2_REG_3_, REG2_REG_2_, REG2_REG_1_, REG2_REG_0_,
    REG3_REG_7_, REG3_REG_6_, REG3_REG_5_, REG3_REG_4_, REG3_REG_3_,
    REG3_REG_2_, REG3_REG_1_, REG3_REG_0_, REG4_REG_7_, REG4_REG_6_,
    REG4_REG_5_, REG4_REG_4_, REG4_REG_3_, REG4_REG_2_, REG4_REG_1_,
    REG4_REG_0_, DATA_OUT_REG_7_, DATA_OUT_REG_6_, DATA_OUT_REG_5_,
    DATA_OUT_REG_4_, DATA_OUT_REG_3_, DATA_OUT_REG_2_, DATA_OUT_REG_1_,
    DATA_OUT_REG_0_, STATO_REG_1_, STATO_REG_0_;
  wire n218, n219, n220_1, n221, n222, n223, n224, n225_1, n226, n227, n228,
    n229, n230_1, n231, n232, n233, n234, n235_1, n236, n237, n238, n239,
    n240_1, n241, n242, n243, n244, n245_1, n246, n247, n248, n249, n251,
    n252, n253, n255_1, n256, n258, n259, n261, n262, n264, n265_1, n267,
    n268, n270_1, n271, n273, n274, n276, n277, n278, n279, n280_1, n281,
    n282, n283, n284, n285_1, n286, n287, n288, n289, n290_1, n291, n292,
    n293, n294, n295_1, n296, n297, n298, n299, n300_1, n301, n302, n303,
    n304, n305_1, n306, n307, n308, n309, n310_1, n311, n312, n314, n315_1,
    n317, n318, n320_1, n321, n323, n324_1, n326, n327, n329, n330, n332_1,
    n333, n335, n336_1, n337, n338, n339, n341, n342, n344_1, n345, n347,
    n348_1, n350, n351, n353, n354, n356, n357_1, n359, n360, n362, n363,
    n365, n366, n368, n369, n371, n372, n374, n375, n377, n378, n380, n381,
    n383, n384, n386, n387, n389, n390, n392, n393, n395, n396, n398, n399,
    n401, n402, n404, n405, n407, n408, n410, n411, n413, n414, n416, n417,
    n419, n420, n422, n423, n425, n426, n428, n429, n431, n432, n434, n435,
    n437, n438, n440, n441, n443, n444, n446, n447, n449, n450, n452, n453,
    n455, n456, n458, n459, n461, n462, n463, n464, n465, n466, n467, n468,
    n469, n470, n471, n472, n473, n474, n475, n476, n477, n478, n479, n480,
    n481, n482, n483, n484, n485, n486, n487, n488, n489, n490, n491, n492,
    n493, n494, n495, n496, n497, n498, n499, n500, n501, n502, n503, n504,
    n505, n506, n507, n508, n509, n510, n511, n512, n513, n514, n515, n516,
    n517, n518, n519, n520, n521, n522, n523, n524, n525, n526, n527, n528,
    n529, n530, n531, n532, n533, n534, n535, n536, n537, n538, n539, n540,
    n541, n542, n543, n544, n545, n546, n547, n548, n549, n550, n551, n552,
    n553, n554, n555, n556, n557, n558, n559, n560, n561, n562, n563, n564,
    n565, n566, n567, n568, n569, n570, n571, n572, n573, n574, n575, n576,
    n577, n578, n579, n580, n581, n582, n583, n584, n585, n586, n587, n588,
    n589, n590, n591, n592, n593, n594, n595, n596, n597, n598, n599, n600,
    n601, n602, n603, n604, n605, n606, n607, n608, n609, n610, n611, n612,
    n613, n614, n615, n616, n617, n618, n619, n620, n621, n622, n623, n624,
    n625, n626, n627, n628, n629, n630, n631, n632, n633, n634, n635, n636,
    n637, n638, n639, n640, n641, n642, n643, n644, n645, n646, n647, n648,
    n649, n650, n651, n652, n653, n654, n655, n656, n657, n658, n659, n660,
    n661, n662, n663, n664, n665, n666, n667, n668, n670, n671, n672, n673,
    n674, n675, n676, n677, n678, n679, n681, n682, n683, n684, n685, n686,
    n687, n688, n689, n690, n691, n692, n693, n694, n695, n696, n697, n698,
    n700, n701, n702, n703, n704, n705, n706, n707, n708, n709, n710, n711,
    n713, n714, n715, n716, n717, n718, n719, n720, n721, n722, n723, n724,
    n726, n727, n728, n729, n730, n731, n732, n733, n734, n735, n736, n737,
    n739, n740, n741, n742, n743, n744, n745, n746, n747, n748, n749, n750,
    n751, n753, n754, n755, n756, n757, n758, n759, n760, n761, n762, n40,
    n45, n50, n55, n60, n65, n70, n75, n80, n85, n90, n95, n100, n105,
    n110, n115, n120, n125, n130, n135, n140, n145, n150, n155, n160, n165,
    n170, n175, n180, n185, n190, n195, n200, n205, n210, n215, n220, n225,
    n230, n235, n240, n245, n250, n255, n260, n265, n270, n275, n280, n285,
    n290, n295, n300, n305, n310, n315, n320, n324, n328, n332, n336, n340,
    n344, n348, n352, n357;
  assign n218 = ~DATA_IN_6_ & RMAX_REG_6_;
  assign n219 = DATA_IN_7_ & ~RMAX_REG_7_;
  assign n220_1 = DATA_IN_5_ & ~RMAX_REG_5_;
  assign n221 = DATA_IN_6_ & ~RMAX_REG_6_;
  assign n222 = ~DATA_IN_4_ & RMAX_REG_4_;
  assign n223 = ~DATA_IN_5_ & RMAX_REG_5_;
  assign n224 = DATA_IN_3_ & ~RMAX_REG_3_;
  assign n225_1 = DATA_IN_4_ & ~RMAX_REG_4_;
  assign n226 = ~DATA_IN_2_ & RMAX_REG_2_;
  assign n227 = ~DATA_IN_3_ & RMAX_REG_3_;
  assign n228 = DATA_IN_1_ & ~RMAX_REG_1_;
  assign n229 = DATA_IN_2_ & ~RMAX_REG_2_;
  assign n230_1 = ~DATA_IN_1_ & RMAX_REG_1_;
  assign n231 = DATA_IN_0_ & ~RMAX_REG_0_;
  assign n232 = ~n230_1 & n231;
  assign n233 = ~n228 & ~n229;
  assign n234 = ~n232 & n233;
  assign n235_1 = ~n226 & ~n227;
  assign n236 = ~n234 & n235_1;
  assign n237 = ~n224 & ~n225_1;
  assign n238 = ~n236 & n237;
  assign n239 = ~n222 & ~n223;
  assign n240_1 = ~n238 & n239;
  assign n241 = ~n220_1 & ~n221;
  assign n242 = ~n240_1 & n241;
  assign n243 = ~n218 & ~n219;
  assign n244 = ~n242 & n243;
  assign n245_1 = ~DATA_IN_7_ & RMAX_REG_7_;
  assign n246 = ~n244 & ~n245_1;
  assign n247 = STATO_REG_1_ & ~n246;
  assign n248 = ~STATO_REG_0_ & ~n247;
  assign n249 = DATA_IN_7_ & ~n248;
  assign n357 = ~STATO_REG_1_ & ~STATO_REG_0_;
  assign n251 = ~STATO_REG_0_ & n246;
  assign n252 = ~n357 & ~n251;
  assign n253 = RMAX_REG_7_ & ~n252;
  assign n40 = n249 | n253;
  assign n255_1 = DATA_IN_6_ & ~n248;
  assign n256 = RMAX_REG_6_ & ~n252;
  assign n45 = n255_1 | n256;
  assign n258 = DATA_IN_5_ & ~n248;
  assign n259 = RMAX_REG_5_ & ~n252;
  assign n50 = n258 | n259;
  assign n261 = DATA_IN_4_ & ~n248;
  assign n262 = RMAX_REG_4_ & ~n252;
  assign n55 = n261 | n262;
  assign n264 = DATA_IN_3_ & ~n248;
  assign n265_1 = RMAX_REG_3_ & ~n252;
  assign n60 = n264 | n265_1;
  assign n267 = DATA_IN_2_ & ~n248;
  assign n268 = RMAX_REG_2_ & ~n252;
  assign n65 = n267 | n268;
  assign n270_1 = DATA_IN_1_ & ~n248;
  assign n271 = RMAX_REG_1_ & ~n252;
  assign n70 = n270_1 | n271;
  assign n273 = DATA_IN_0_ & ~n248;
  assign n274 = RMAX_REG_0_ & ~n252;
  assign n75 = n273 | n274;
  assign n276 = DATA_IN_6_ & ~RMIN_REG_6_;
  assign n277 = ~DATA_IN_7_ & RMIN_REG_7_;
  assign n278 = ~DATA_IN_5_ & RMIN_REG_5_;
  assign n279 = ~DATA_IN_6_ & RMIN_REG_6_;
  assign n280_1 = DATA_IN_4_ & ~RMIN_REG_4_;
  assign n281 = DATA_IN_5_ & ~RMIN_REG_5_;
  assign n282 = ~DATA_IN_3_ & RMIN_REG_3_;
  assign n283 = ~DATA_IN_4_ & RMIN_REG_4_;
  assign n284 = DATA_IN_2_ & ~RMIN_REG_2_;
  assign n285_1 = DATA_IN_3_ & ~RMIN_REG_3_;
  assign n286 = ~DATA_IN_1_ & RMIN_REG_1_;
  assign n287 = ~DATA_IN_2_ & RMIN_REG_2_;
  assign n288 = DATA_IN_1_ & ~RMIN_REG_1_;
  assign n289 = ~DATA_IN_0_ & RMIN_REG_0_;
  assign n290_1 = ~n288 & n289;
  assign n291 = ~n286 & ~n287;
  assign n292 = ~n290_1 & n291;
  assign n293 = ~n284 & ~n285_1;
  assign n294 = ~n292 & n293;
  assign n295_1 = ~n282 & ~n283;
  assign n296 = ~n294 & n295_1;
  assign n297 = ~n280_1 & ~n281;
  assign n298 = ~n296 & n297;
  assign n299 = ~n278 & ~n279;
  assign n300_1 = ~n298 & n299;
  assign n301 = ~n276 & ~n277;
  assign n302 = ~n300_1 & n301;
  assign n303 = DATA_IN_7_ & ~RMIN_REG_7_;
  assign n304 = ~n302 & ~n303;
  assign n305_1 = n246 & ~n304;
  assign n306 = ~STATO_REG_0_ & ~n305_1;
  assign n307 = ~n357 & ~n306;
  assign n308 = RMIN_REG_7_ & ~n307;
  assign n309 = STATO_REG_1_ & n246;
  assign n310_1 = ~n304 & n309;
  assign n311 = ~STATO_REG_0_ & ~n310_1;
  assign n312 = DATA_IN_7_ & ~n311;
  assign n80 = n308 | n312;
  assign n314 = RMIN_REG_6_ & ~n307;
  assign n315_1 = DATA_IN_6_ & ~n311;
  assign n85 = n314 | n315_1;
  assign n317 = RMIN_REG_5_ & ~n307;
  assign n318 = DATA_IN_5_ & ~n311;
  assign n90 = n317 | n318;
  assign n320_1 = RMIN_REG_4_ & ~n307;
  assign n321 = DATA_IN_4_ & ~n311;
  assign n95 = n320_1 | n321;
  assign n323 = RMIN_REG_3_ & ~n307;
  assign n324_1 = DATA_IN_3_ & ~n311;
  assign n100 = n323 | n324_1;
  assign n326 = RMIN_REG_2_ & ~n307;
  assign n327 = DATA_IN_2_ & ~n311;
  assign n105 = n326 | n327;
  assign n329 = RMIN_REG_1_ & ~n307;
  assign n330 = DATA_IN_1_ & ~n311;
  assign n110 = n329 | n330;
  assign n332_1 = RMIN_REG_0_ & ~n307;
  assign n333 = DATA_IN_0_ & ~n311;
  assign n115 = n332_1 | n333;
  assign n335 = ~ENABLE & ~STATO_REG_0_;
  assign n336_1 = ~n357 & ~n335;
  assign n337 = RLAST_REG_7_ & ~n336_1;
  assign n338 = STATO_REG_1_ & ~n335;
  assign n339 = DATA_IN_7_ & n338;
  assign n120 = n337 | n339;
  assign n341 = RLAST_REG_6_ & ~n336_1;
  assign n342 = DATA_IN_6_ & n338;
  assign n125 = n341 | n342;
  assign n344_1 = RLAST_REG_5_ & ~n336_1;
  assign n345 = DATA_IN_5_ & n338;
  assign n130 = n344_1 | n345;
  assign n347 = RLAST_REG_4_ & ~n336_1;
  assign n348_1 = DATA_IN_4_ & n338;
  assign n135 = n347 | n348_1;
  assign n350 = RLAST_REG_3_ & ~n336_1;
  assign n351 = DATA_IN_3_ & n338;
  assign n140 = n350 | n351;
  assign n353 = RLAST_REG_2_ & ~n336_1;
  assign n354 = DATA_IN_2_ & n338;
  assign n145 = n353 | n354;
  assign n356 = RLAST_REG_1_ & ~n336_1;
  assign n357_1 = DATA_IN_1_ & n338;
  assign n150 = n356 | n357_1;
  assign n359 = RLAST_REG_0_ & ~n336_1;
  assign n360 = DATA_IN_0_ & n338;
  assign n155 = n359 | n360;
  assign n362 = STATO_REG_1_ & ~STATO_REG_0_;
  assign n363 = ~STATO_REG_1_ & STATO_REG_0_;
  assign n352 = n362 | n363;
  assign n365 = REG1_REG_7_ & ~n352;
  assign n366 = DATA_IN_7_ & n362;
  assign n160 = n365 | n366;
  assign n368 = REG1_REG_6_ & ~n352;
  assign n369 = DATA_IN_6_ & n362;
  assign n165 = n368 | n369;
  assign n371 = REG1_REG_5_ & ~n352;
  assign n372 = DATA_IN_5_ & n362;
  assign n170 = n371 | n372;
  assign n374 = REG1_REG_4_ & ~n352;
  assign n375 = DATA_IN_4_ & n362;
  assign n175 = n374 | n375;
  assign n377 = REG1_REG_3_ & ~n352;
  assign n378 = DATA_IN_3_ & n362;
  assign n180 = n377 | n378;
  assign n380 = REG1_REG_2_ & ~n352;
  assign n381 = DATA_IN_2_ & n362;
  assign n185 = n380 | n381;
  assign n383 = REG1_REG_1_ & ~n352;
  assign n384 = DATA_IN_1_ & n362;
  assign n190 = n383 | n384;
  assign n386 = REG1_REG_0_ & ~n352;
  assign n387 = DATA_IN_0_ & n362;
  assign n195 = n386 | n387;
  assign n389 = REG2_REG_7_ & ~n352;
  assign n390 = REG1_REG_7_ & n362;
  assign n200 = n389 | n390;
  assign n392 = REG2_REG_6_ & ~n352;
  assign n393 = REG1_REG_6_ & n362;
  assign n205 = n392 | n393;
  assign n395 = REG2_REG_5_ & ~n352;
  assign n396 = REG1_REG_5_ & n362;
  assign n210 = n395 | n396;
  assign n398 = REG2_REG_4_ & ~n352;
  assign n399 = REG1_REG_4_ & n362;
  assign n215 = n398 | n399;
  assign n401 = REG2_REG_3_ & ~n352;
  assign n402 = REG1_REG_3_ & n362;
  assign n220 = n401 | n402;
  assign n404 = REG2_REG_2_ & ~n352;
  assign n405 = REG1_REG_2_ & n362;
  assign n225 = n404 | n405;
  assign n407 = REG2_REG_1_ & ~n352;
  assign n408 = REG1_REG_1_ & n362;
  assign n230 = n407 | n408;
  assign n410 = REG2_REG_0_ & ~n352;
  assign n411 = REG1_REG_0_ & n362;
  assign n235 = n410 | n411;
  assign n413 = REG3_REG_7_ & ~n352;
  assign n414 = REG2_REG_7_ & n362;
  assign n240 = n413 | n414;
  assign n416 = REG3_REG_6_ & ~n352;
  assign n417 = REG2_REG_6_ & n362;
  assign n245 = n416 | n417;
  assign n419 = REG3_REG_5_ & ~n352;
  assign n420 = REG2_REG_5_ & n362;
  assign n250 = n419 | n420;
  assign n422 = REG3_REG_4_ & ~n352;
  assign n423 = REG2_REG_4_ & n362;
  assign n255 = n422 | n423;
  assign n425 = REG3_REG_3_ & ~n352;
  assign n426 = REG2_REG_3_ & n362;
  assign n260 = n425 | n426;
  assign n428 = REG3_REG_2_ & ~n352;
  assign n429 = REG2_REG_2_ & n362;
  assign n265 = n428 | n429;
  assign n431 = REG3_REG_1_ & ~n352;
  assign n432 = REG2_REG_1_ & n362;
  assign n270 = n431 | n432;
  assign n434 = REG3_REG_0_ & ~n352;
  assign n435 = REG2_REG_0_ & n362;
  assign n275 = n434 | n435;
  assign n437 = REG4_REG_7_ & ~n352;
  assign n438 = REG3_REG_7_ & n362;
  assign n280 = n437 | n438;
  assign n440 = REG4_REG_6_ & ~n352;
  assign n441 = REG3_REG_6_ & n362;
  assign n285 = n440 | n441;
  assign n443 = REG4_REG_5_ & ~n352;
  assign n444 = REG3_REG_5_ & n362;
  assign n290 = n443 | n444;
  assign n446 = REG4_REG_4_ & ~n352;
  assign n447 = REG3_REG_4_ & n362;
  assign n295 = n446 | n447;
  assign n449 = REG4_REG_3_ & ~n352;
  assign n450 = REG3_REG_3_ & n362;
  assign n300 = n449 | n450;
  assign n452 = REG4_REG_2_ & ~n352;
  assign n453 = REG3_REG_2_ & n362;
  assign n305 = n452 | n453;
  assign n455 = REG4_REG_1_ & ~n352;
  assign n456 = REG3_REG_1_ & n362;
  assign n310 = n455 | n456;
  assign n458 = REG4_REG_0_ & ~n352;
  assign n459 = REG3_REG_0_ & n362;
  assign n315 = n458 | n459;
  assign n461 = STATO_REG_1_ & n352;
  assign n462 = ~RESTART & n461;
  assign n463 = AVERAGE & ENABLE;
  assign n464 = n462 & n463;
  assign n465 = REG4_REG_7_ & n464;
  assign n466 = ~ENABLE & n462;
  assign n467 = RLAST_REG_7_ & n466;
  assign n468 = RESTART & RMAX_REG_1_;
  assign n469 = ~RESTART & DATA_IN_1_;
  assign n470 = ~n468 & ~n469;
  assign n471 = RESTART & RMIN_REG_0_;
  assign n472 = ~RESTART & REG4_REG_0_;
  assign n473 = ~n471 & ~n472;
  assign n474 = RESTART & RMAX_REG_0_;
  assign n475 = ~RESTART & DATA_IN_0_;
  assign n476 = ~n474 & ~n475;
  assign n477 = ~n473 & ~n476;
  assign n478 = ~n470 & n477;
  assign n479 = RESTART & RMIN_REG_1_;
  assign n480 = ~RESTART & REG4_REG_1_;
  assign n481 = ~n479 & ~n480;
  assign n482 = n470 & ~n477;
  assign n483 = ~n481 & ~n482;
  assign n484 = ~n478 & ~n483;
  assign n485 = RESTART & RMAX_REG_2_;
  assign n486 = ~RESTART & DATA_IN_2_;
  assign n487 = ~n485 & ~n486;
  assign n488 = RESTART & RMIN_REG_2_;
  assign n489 = ~RESTART & REG4_REG_2_;
  assign n490 = ~n488 & ~n489;
  assign n491 = ~n487 & n490;
  assign n492 = n487 & ~n490;
  assign n493 = ~n491 & ~n492;
  assign n494 = n484 & ~n493;
  assign n495 = ~n484 & n493;
  assign n496 = ~n494 & ~n495;
  assign n497 = n478 & ~n481;
  assign n498 = ~n470 & ~n477;
  assign n499 = n481 & n498;
  assign n500 = n477 & n481;
  assign n501 = ~n477 & ~n481;
  assign n502 = ~n500 & ~n501;
  assign n503 = n470 & ~n502;
  assign n504 = ~n497 & ~n499;
  assign n505 = ~n503 & n504;
  assign n506 = n473 & ~n476;
  assign n507 = ~n473 & n476;
  assign n508 = ~n506 & ~n507;
  assign n509 = n505 & n508;
  assign n510 = ~n496 & ~n509;
  assign n511 = n496 & n509;
  assign n512 = ~n510 & ~n511;
  assign n513 = n505 & ~n508;
  assign n514 = ~n505 & n508;
  assign n515 = ~n513 & ~n514;
  assign n516 = ~n487 & ~n490;
  assign n517 = n487 & n490;
  assign n518 = ~n484 & ~n517;
  assign n519 = ~n516 & ~n518;
  assign n520 = RESTART & RMAX_REG_3_;
  assign n521 = ~RESTART & DATA_IN_3_;
  assign n522 = ~n520 & ~n521;
  assign n523 = RESTART & RMIN_REG_3_;
  assign n524 = ~RESTART & REG4_REG_3_;
  assign n525 = ~n523 & ~n524;
  assign n526 = ~n522 & n525;
  assign n527 = n522 & ~n525;
  assign n528 = ~n526 & ~n527;
  assign n529 = n519 & ~n528;
  assign n530 = ~n519 & n528;
  assign n531 = ~n529 & ~n530;
  assign n532 = n511 & n531;
  assign n533 = ~n511 & ~n531;
  assign n534 = ~n532 & ~n533;
  assign n535 = ~n512 & n515;
  assign n536 = ~n534 & n535;
  assign n537 = ~n522 & ~n525;
  assign n538 = n522 & n525;
  assign n539 = ~n519 & ~n538;
  assign n540 = ~n537 & ~n539;
  assign n541 = RESTART & RMAX_REG_4_;
  assign n542 = ~RESTART & DATA_IN_4_;
  assign n543 = ~n541 & ~n542;
  assign n544 = RESTART & RMIN_REG_4_;
  assign n545 = ~RESTART & REG4_REG_4_;
  assign n546 = ~n544 & ~n545;
  assign n547 = ~n543 & n546;
  assign n548 = n543 & ~n546;
  assign n549 = ~n547 & ~n548;
  assign n550 = n540 & ~n549;
  assign n551 = ~n540 & n549;
  assign n552 = ~n550 & ~n551;
  assign n553 = ~n532 & ~n552;
  assign n554 = n532 & n552;
  assign n555 = ~n553 & ~n554;
  assign n556 = RESTART & RMAX_REG_5_;
  assign n557 = ~RESTART & DATA_IN_5_;
  assign n558 = ~n556 & ~n557;
  assign n559 = RESTART & RMIN_REG_5_;
  assign n560 = ~RESTART & REG4_REG_5_;
  assign n561 = ~n559 & ~n560;
  assign n562 = ~n558 & n561;
  assign n563 = n558 & ~n561;
  assign n564 = ~n562 & ~n563;
  assign n565 = ~n543 & ~n546;
  assign n566 = n543 & n546;
  assign n567 = ~n540 & ~n566;
  assign n568 = ~n565 & ~n567;
  assign n569 = ~n564 & n568;
  assign n570 = n564 & ~n568;
  assign n571 = ~n569 & ~n570;
  assign n572 = ~n554 & ~n571;
  assign n573 = n554 & n571;
  assign n574 = ~n572 & ~n573;
  assign n575 = n536 & ~n555;
  assign n576 = ~n574 & n575;
  assign n577 = n558 & n561;
  assign n578 = RESTART & RMAX_REG_6_;
  assign n579 = ~RESTART & DATA_IN_6_;
  assign n580 = ~n578 & ~n579;
  assign n581 = RESTART & RMIN_REG_6_;
  assign n582 = ~RESTART & REG4_REG_6_;
  assign n583 = ~n581 & ~n582;
  assign n584 = ~n580 & n583;
  assign n585 = n580 & ~n583;
  assign n586 = ~n584 & ~n585;
  assign n587 = ~n558 & ~n561;
  assign n588 = n568 & ~n587;
  assign n589 = ~n577 & ~n586;
  assign n590 = ~n588 & n589;
  assign n591 = ~n568 & ~n577;
  assign n592 = n586 & ~n587;
  assign n593 = ~n591 & n592;
  assign n594 = ~n590 & ~n593;
  assign n595 = n573 & ~n594;
  assign n596 = ~n573 & n594;
  assign n597 = ~n595 & ~n596;
  assign n598 = n576 & ~n597;
  assign n599 = n595 & ~n598;
  assign n600 = ~RMAX_REG_7_ & ~RMIN_REG_7_;
  assign n601 = RMAX_REG_5_ & RMIN_REG_5_;
  assign n602 = ~RMAX_REG_3_ & ~RMIN_REG_3_;
  assign n603 = ~RMAX_REG_4_ & ~RMIN_REG_4_;
  assign n604 = RMAX_REG_3_ & RMIN_REG_3_;
  assign n605 = RMAX_REG_2_ & RMIN_REG_2_;
  assign n606 = ~RMAX_REG_1_ & ~RMIN_REG_1_;
  assign n607 = ~RMAX_REG_2_ & ~RMIN_REG_2_;
  assign n608 = RMAX_REG_0_ & RMIN_REG_0_;
  assign n609 = RMAX_REG_1_ & RMIN_REG_1_;
  assign n610 = ~n608 & ~n609;
  assign n611 = ~n606 & ~n607;
  assign n612 = ~n610 & n611;
  assign n613 = ~n604 & ~n605;
  assign n614 = ~n612 & n613;
  assign n615 = ~n602 & ~n603;
  assign n616 = ~n614 & n615;
  assign n617 = RMAX_REG_4_ & RMIN_REG_4_;
  assign n618 = ~n616 & ~n617;
  assign n619 = ~RMAX_REG_5_ & ~RMIN_REG_5_;
  assign n620 = ~n618 & ~n619;
  assign n621 = ~n601 & ~n620;
  assign n622 = ~RMAX_REG_6_ & n621;
  assign n623 = RMAX_REG_6_ & ~n621;
  assign n624 = ~RMIN_REG_6_ & ~n623;
  assign n625 = ~n622 & ~n624;
  assign n626 = ~n600 & ~n625;
  assign n627 = RMAX_REG_7_ & RMIN_REG_7_;
  assign n628 = ~n626 & ~n627;
  assign n629 = RESTART & n461;
  assign n630 = ~n628 & n629;
  assign n631 = n599 & n630;
  assign n632 = DATA_OUT_REG_7_ & ~n352;
  assign n633 = ~DATA_IN_7_ & ~REG4_REG_7_;
  assign n634 = DATA_IN_5_ & REG4_REG_5_;
  assign n635 = ~DATA_IN_3_ & ~REG4_REG_3_;
  assign n636 = ~DATA_IN_4_ & ~REG4_REG_4_;
  assign n637 = DATA_IN_3_ & REG4_REG_3_;
  assign n638 = DATA_IN_2_ & REG4_REG_2_;
  assign n639 = ~DATA_IN_1_ & ~REG4_REG_1_;
  assign n640 = ~DATA_IN_2_ & ~REG4_REG_2_;
  assign n641 = DATA_IN_0_ & REG4_REG_0_;
  assign n642 = DATA_IN_1_ & REG4_REG_1_;
  assign n643 = ~n641 & ~n642;
  assign n644 = ~n639 & ~n640;
  assign n645 = ~n643 & n644;
  assign n646 = ~n637 & ~n638;
  assign n647 = ~n645 & n646;
  assign n648 = ~n635 & ~n636;
  assign n649 = ~n647 & n648;
  assign n650 = DATA_IN_4_ & REG4_REG_4_;
  assign n651 = ~n649 & ~n650;
  assign n652 = ~DATA_IN_5_ & ~REG4_REG_5_;
  assign n653 = ~n651 & ~n652;
  assign n654 = ~n634 & ~n653;
  assign n655 = ~DATA_IN_6_ & n654;
  assign n656 = DATA_IN_6_ & ~n654;
  assign n657 = ~REG4_REG_6_ & ~n656;
  assign n658 = ~n655 & ~n657;
  assign n659 = ~n633 & ~n658;
  assign n660 = DATA_IN_7_ & REG4_REG_7_;
  assign n661 = ~n659 & ~n660;
  assign n662 = ENABLE & n462;
  assign n663 = ~AVERAGE & n662;
  assign n664 = ~n661 & n663;
  assign n665 = n599 & n664;
  assign n666 = ~n465 & ~n467;
  assign n667 = ~n631 & n666;
  assign n668 = ~n632 & n667;
  assign n320 = n665 | ~n668;
  assign n670 = REG4_REG_6_ & n464;
  assign n671 = RLAST_REG_6_ & n466;
  assign n672 = ~n595 & n598;
  assign n673 = ~n599 & ~n672;
  assign n674 = n630 & ~n673;
  assign n675 = DATA_OUT_REG_6_ & ~n352;
  assign n676 = n664 & ~n673;
  assign n677 = ~n670 & ~n671;
  assign n678 = ~n674 & n677;
  assign n679 = ~n675 & n678;
  assign n324 = n676 | ~n679;
  assign n681 = ~n576 & n597;
  assign n682 = ~n598 & ~n681;
  assign n683 = n664 & n682;
  assign n684 = n630 & n682;
  assign n685 = REG4_REG_5_ & n464;
  assign n686 = RLAST_REG_5_ & n466;
  assign n687 = ~AVERAGE & ENABLE;
  assign n688 = n661 & n687;
  assign n689 = ~RESTART & ~n688;
  assign n690 = RESTART & ~n628;
  assign n691 = ~n689 & ~n690;
  assign n692 = n461 & n691;
  assign n693 = n594 & n692;
  assign n694 = DATA_OUT_REG_5_ & ~n352;
  assign n695 = ~n685 & ~n686;
  assign n696 = ~n693 & n695;
  assign n697 = ~n694 & n696;
  assign n698 = ~n683 & ~n684;
  assign n328 = ~n697 | ~n698;
  assign n700 = n574 & ~n575;
  assign n701 = ~n576 & ~n700;
  assign n702 = n664 & n701;
  assign n703 = n630 & n701;
  assign n704 = REG4_REG_4_ & n464;
  assign n705 = RLAST_REG_4_ & n466;
  assign n706 = ~n571 & n692;
  assign n707 = DATA_OUT_REG_4_ & ~n352;
  assign n708 = ~n704 & ~n705;
  assign n709 = ~n706 & n708;
  assign n710 = ~n707 & n709;
  assign n711 = ~n702 & ~n703;
  assign n332 = ~n710 | ~n711;
  assign n713 = ~n536 & n555;
  assign n714 = ~n575 & ~n713;
  assign n715 = n664 & n714;
  assign n716 = n630 & n714;
  assign n717 = DATA_OUT_REG_3_ & ~n352;
  assign n718 = REG4_REG_3_ & n464;
  assign n719 = RLAST_REG_3_ & n466;
  assign n720 = ~n552 & n692;
  assign n721 = ~n718 & ~n719;
  assign n722 = ~n720 & n721;
  assign n723 = ~n715 & ~n716;
  assign n724 = ~n717 & n723;
  assign n336 = ~n722 | ~n724;
  assign n726 = n534 & ~n535;
  assign n727 = ~n536 & ~n726;
  assign n728 = n664 & n727;
  assign n729 = n630 & n727;
  assign n730 = DATA_OUT_REG_2_ & ~n352;
  assign n731 = REG4_REG_2_ & n464;
  assign n732 = RLAST_REG_2_ & n466;
  assign n733 = ~n731 & ~n732;
  assign n734 = ~n531 & n692;
  assign n735 = ~n728 & ~n729;
  assign n736 = ~n730 & n735;
  assign n737 = n733 & n736;
  assign n340 = n734 | ~n737;
  assign n739 = ~n512 & ~n515;
  assign n740 = n512 & n515;
  assign n741 = ~n739 & ~n740;
  assign n742 = n664 & ~n741;
  assign n743 = n630 & ~n741;
  assign n744 = DATA_OUT_REG_1_ & ~n352;
  assign n745 = REG4_REG_1_ & n464;
  assign n746 = RLAST_REG_1_ & n466;
  assign n747 = ~n745 & ~n746;
  assign n748 = ~n496 & n692;
  assign n749 = ~n742 & ~n743;
  assign n750 = ~n744 & n749;
  assign n751 = n747 & n750;
  assign n344 = n748 | ~n751;
  assign n753 = ~n515 & n664;
  assign n754 = ~n515 & n630;
  assign n755 = DATA_OUT_REG_0_ & ~n352;
  assign n756 = REG4_REG_0_ & n464;
  assign n757 = RLAST_REG_0_ & n466;
  assign n758 = ~n756 & ~n757;
  assign n759 = ~n505 & n692;
  assign n760 = ~n753 & ~n754;
  assign n761 = ~n755 & n760;
  assign n762 = n758 & n761;
  assign n348 = n759 | ~n762;
  always @ (posedge clock) begin
    RMAX_REG_7_ <= n40;
    RMAX_REG_6_ <= n45;
    RMAX_REG_5_ <= n50;
    RMAX_REG_4_ <= n55;
    RMAX_REG_3_ <= n60;
    RMAX_REG_2_ <= n65;
    RMAX_REG_1_ <= n70;
    RMAX_REG_0_ <= n75;
    RMIN_REG_7_ <= n80;
    RMIN_REG_6_ <= n85;
    RMIN_REG_5_ <= n90;
    RMIN_REG_4_ <= n95;
    RMIN_REG_3_ <= n100;
    RMIN_REG_2_ <= n105;
    RMIN_REG_1_ <= n110;
    RMIN_REG_0_ <= n115;
    RLAST_REG_7_ <= n120;
    RLAST_REG_6_ <= n125;
    RLAST_REG_5_ <= n130;
    RLAST_REG_4_ <= n135;
    RLAST_REG_3_ <= n140;
    RLAST_REG_2_ <= n145;
    RLAST_REG_1_ <= n150;
    RLAST_REG_0_ <= n155;
    REG1_REG_7_ <= n160;
    REG1_REG_6_ <= n165;
    REG1_REG_5_ <= n170;
    REG1_REG_4_ <= n175;
    REG1_REG_3_ <= n180;
    REG1_REG_2_ <= n185;
    REG1_REG_1_ <= n190;
    REG1_REG_0_ <= n195;
    REG2_REG_7_ <= n200;
    REG2_REG_6_ <= n205;
    REG2_REG_5_ <= n210;
    REG2_REG_4_ <= n215;
    REG2_REG_3_ <= n220;
    REG2_REG_2_ <= n225;
    REG2_REG_1_ <= n230;
    REG2_REG_0_ <= n235;
    REG3_REG_7_ <= n240;
    REG3_REG_6_ <= n245;
    REG3_REG_5_ <= n250;
    REG3_REG_4_ <= n255;
    REG3_REG_3_ <= n260;
    REG3_REG_2_ <= n265;
    REG3_REG_1_ <= n270;
    REG3_REG_0_ <= n275;
    REG4_REG_7_ <= n280;
    REG4_REG_6_ <= n285;
    REG4_REG_5_ <= n290;
    REG4_REG_4_ <= n295;
    REG4_REG_3_ <= n300;
    REG4_REG_2_ <= n305;
    REG4_REG_1_ <= n310;
    REG4_REG_0_ <= n315;
    DATA_OUT_REG_7_ <= n320;
    DATA_OUT_REG_6_ <= n324;
    DATA_OUT_REG_5_ <= n328;
    DATA_OUT_REG_4_ <= n332;
    DATA_OUT_REG_3_ <= n336;
    DATA_OUT_REG_2_ <= n340;
    DATA_OUT_REG_1_ <= n344;
    DATA_OUT_REG_0_ <= n348;
    STATO_REG_1_ <= n352;
    STATO_REG_0_ <= n357;
  end
endmodule


