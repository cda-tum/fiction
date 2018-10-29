// Benchmark "b07" written by ABC on Wed Sep  5 10:17:19 2018

module b07 ( clock, 
    START,
    PUNTI_RETTA_REG_7_, PUNTI_RETTA_REG_6_, PUNTI_RETTA_REG_5_,
    PUNTI_RETTA_REG_4_, PUNTI_RETTA_REG_3_, PUNTI_RETTA_REG_2_,
    PUNTI_RETTA_REG_1_, PUNTI_RETTA_REG_0_  );
  input  clock;
  input  START;
  output PUNTI_RETTA_REG_7_, PUNTI_RETTA_REG_6_, PUNTI_RETTA_REG_5_,
    PUNTI_RETTA_REG_4_, PUNTI_RETTA_REG_3_, PUNTI_RETTA_REG_2_,
    PUNTI_RETTA_REG_1_, PUNTI_RETTA_REG_0_;
  reg PUNTI_RETTA_REG_7_, PUNTI_RETTA_REG_6_, PUNTI_RETTA_REG_5_,
    PUNTI_RETTA_REG_4_, PUNTI_RETTA_REG_3_, PUNTI_RETTA_REG_2_,
    PUNTI_RETTA_REG_1_, PUNTI_RETTA_REG_0_, CONT_REG_7_, CONT_REG_6_,
    CONT_REG_5_, CONT_REG_4_, CONT_REG_3_, CONT_REG_2_, CONT_REG_1_,
    CONT_REG_0_, MAR_REG_7_, MAR_REG_6_, MAR_REG_5_, MAR_REG_4_,
    MAR_REG_3_, MAR_REG_2_, MAR_REG_1_, MAR_REG_0_, X_REG_7_, X_REG_6_,
    X_REG_5_, X_REG_4_, X_REG_3_, X_REG_2_, X_REG_1_, X_REG_0_, Y_REG_3_,
    Y_REG_1_, Y_REG_5_, T_REG_3_, T_REG_5_, T_REG_1_, T_REG_0_, T_REG_4_,
    T_REG_6_, T_REG_2_, Y_REG_4_, Y_REG_0_, Y_REG_2_, Y_REG_6_,
    STATO_REG_2_, STATO_REG_1_, STATO_REG_0_;
  wire n157_1, n158, n159, n160, n161, n162_1, n163, n164, n165, n166,
    n167_1, n168, n169, n170, n171, n172_1, n173, n174, n175, n176, n177_1,
    n178, n179, n180, n181, n182_1, n183, n184, n185, n186, n187_1, n188,
    n189, n190, n191, n192_1, n193, n194, n195, n196, n197_1, n198, n199,
    n200, n201, n202_1, n203, n204, n205, n206, n207_1, n208, n209, n210,
    n211, n212_1, n213, n214, n215, n216, n217_1, n218, n219, n220, n221,
    n222_1, n223, n224, n225, n226, n227_1, n228, n229, n230, n231, n232_1,
    n233, n234, n235, n236, n237_1, n238, n239, n240, n241, n242_1, n243,
    n244, n245, n246, n247_1, n248, n249, n250, n251, n252_1, n253, n254,
    n255, n256, n257, n258, n259, n260, n261, n262, n263, n264, n265, n266,
    n267, n268, n269, n270, n271, n272, n273, n274, n275, n277, n278, n279,
    n280, n281, n282, n283, n284, n285, n286, n288, n289, n290, n291, n292,
    n293, n294, n295, n296, n297, n299, n300, n301, n302, n303, n304, n305,
    n306, n307, n308, n310, n311, n312, n313, n314, n315, n316, n317, n318,
    n319, n321, n322, n323, n324, n325, n326, n327, n328, n329, n330, n332,
    n333, n334, n335, n336, n337, n338, n339, n340, n341, n343, n344, n345,
    n346, n347, n348, n349, n351, n352, n353, n354, n355, n356, n357, n359,
    n360, n362, n363, n365, n366, n368, n369, n371, n372, n374, n375, n377,
    n378, n380, n381, n382, n383, n388, n389, n390, n391, n392, n393, n395,
    n396, n397, n398, n399, n401, n402, n403, n404, n405, n407, n408, n410,
    n411, n412, n413, n414, n415, n416, n417, n418, n419, n420, n421, n422,
    n423, n424, n425, n426, n428, n429, n430, n432, n433, n434, n436, n437,
    n438, n440, n441, n442, n444, n445, n446, n447, n448, n449, n451, n452,
    n453, n454, n455, n456, n457, n458, n460, n461, n462, n463, n464, n465,
    n466, n468, n469, n470, n472, n473, n475, n477, n478, n480, n481, n483,
    n484, n486, n487, n489, n490, n492, n493, n495, n496, n498, n500, n501,
    n503, n504, n506, n508, n509, n510, n512, n513, n514, n515, n516, n517,
    n519, n520, n521, n20, n24, n28, n32, n36, n40, n44, n48, n52, n57,
    n62, n67, n72, n77, n82, n87, n92, n97, n102, n107, n112, n117, n122,
    n127, n132, n137, n142, n147, n152, n157, n162, n167, n172, n177, n182,
    n187, n192, n197, n202, n207, n212, n217, n222, n227, n232, n237, n242,
    n247, n252;
  assign n157_1 = STATO_REG_2_ & STATO_REG_1_;
  assign n158 = ~MAR_REG_7_ & ~MAR_REG_6_;
  assign n159 = ~MAR_REG_5_ & n158;
  assign n160 = MAR_REG_3_ & n159;
  assign n161 = ~MAR_REG_4_ & n160;
  assign n162_1 = MAR_REG_1_ & MAR_REG_0_;
  assign n163 = MAR_REG_2_ & n162_1;
  assign n164 = n161 & n163;
  assign n165 = n157_1 & n164;
  assign n166 = ~STATO_REG_2_ & ~STATO_REG_1_;
  assign n167_1 = STATO_REG_0_ & n166;
  assign n168 = ~n165 & ~n167_1;
  assign n169 = ~START & ~n168;
  assign n170 = STATO_REG_2_ & n169;
  assign n171 = ~X_REG_7_ & ~X_REG_6_;
  assign n172_1 = ~X_REG_5_ & n171;
  assign n173 = ~X_REG_4_ & n172_1;
  assign n174 = X_REG_1_ & ~X_REG_0_;
  assign n175 = ~X_REG_2_ & n174;
  assign n176 = ~X_REG_3_ & n175;
  assign n177_1 = n173 & n176;
  assign n178 = n170 & ~n177_1;
  assign n179 = CONT_REG_7_ & n178;
  assign n180 = PUNTI_RETTA_REG_7_ & ~n169;
  assign n181 = ~STATO_REG_1_ & ~STATO_REG_0_;
  assign n182_1 = STATO_REG_2_ & STATO_REG_0_;
  assign n183 = STATO_REG_2_ & ~n181;
  assign n184 = ~n182_1 & n183;
  assign n185 = X_REG_6_ & ~n184;
  assign n186 = n157_1 & ~n164;
  assign n187_1 = ~n165 & ~n186;
  assign n188 = CONT_REG_6_ & ~n187_1;
  assign n189 = ~n185 & ~n188;
  assign n190 = T_REG_5_ & n181;
  assign n191 = X_REG_5_ & ~STATO_REG_2_;
  assign n192_1 = Y_REG_5_ & n182_1;
  assign n193 = ~n190 & ~n191;
  assign n194 = ~n192_1 & n193;
  assign n195 = X_REG_5_ & ~n184;
  assign n196 = CONT_REG_5_ & ~n187_1;
  assign n197_1 = ~n195 & ~n196;
  assign n198 = ~n194 & ~n197_1;
  assign n199 = n194 & n197_1;
  assign n200 = T_REG_4_ & n181;
  assign n201 = X_REG_4_ & ~STATO_REG_2_;
  assign n202_1 = Y_REG_4_ & n182_1;
  assign n203 = ~n200 & ~n201;
  assign n204 = ~n202_1 & n203;
  assign n205 = X_REG_4_ & ~n184;
  assign n206 = CONT_REG_4_ & ~n187_1;
  assign n207_1 = ~n205 & ~n206;
  assign n208 = ~n204 & ~n207_1;
  assign n209 = n204 & n207_1;
  assign n210 = T_REG_3_ & n181;
  assign n211 = X_REG_3_ & ~STATO_REG_2_;
  assign n212_1 = Y_REG_3_ & n182_1;
  assign n213 = ~n210 & ~n211;
  assign n214 = ~n212_1 & n213;
  assign n215 = X_REG_3_ & ~n184;
  assign n216 = CONT_REG_3_ & ~n187_1;
  assign n217_1 = ~n215 & ~n216;
  assign n218 = ~n214 & ~n217_1;
  assign n219 = n214 & n217_1;
  assign n220 = T_REG_2_ & n181;
  assign n221 = X_REG_2_ & ~STATO_REG_2_;
  assign n222_1 = Y_REG_2_ & n182_1;
  assign n223 = ~n220 & ~n221;
  assign n224 = ~n222_1 & n223;
  assign n225 = X_REG_2_ & ~n184;
  assign n226 = CONT_REG_2_ & ~n187_1;
  assign n227_1 = ~n225 & ~n226;
  assign n228 = ~n224 & ~n227_1;
  assign n229 = n224 & n227_1;
  assign n230 = T_REG_1_ & n181;
  assign n231 = X_REG_1_ & ~STATO_REG_2_;
  assign n232_1 = Y_REG_1_ & n182_1;
  assign n233 = ~n230 & ~n231;
  assign n234 = ~n232_1 & n233;
  assign n235 = X_REG_1_ & ~n184;
  assign n236 = CONT_REG_1_ & ~n187_1;
  assign n237_1 = ~n235 & ~n236;
  assign n238 = ~n234 & ~n237_1;
  assign n239 = X_REG_0_ & ~STATO_REG_2_;
  assign n240 = T_REG_0_ & n181;
  assign n241 = Y_REG_0_ & n182_1;
  assign n242_1 = ~n239 & ~n240;
  assign n243 = ~n241 & n242_1;
  assign n244 = ~n157_1 & n243;
  assign n245 = X_REG_0_ & ~n184;
  assign n246 = CONT_REG_0_ & ~n187_1;
  assign n247_1 = ~n245 & ~n246;
  assign n248 = ~n244 & ~n247_1;
  assign n249 = n234 & n237_1;
  assign n250 = n248 & ~n249;
  assign n251 = ~n238 & ~n250;
  assign n252_1 = ~n229 & ~n251;
  assign n253 = ~n228 & ~n252_1;
  assign n254 = ~n219 & ~n253;
  assign n255 = ~n218 & ~n254;
  assign n256 = ~n209 & ~n255;
  assign n257 = ~n208 & ~n256;
  assign n258 = ~n199 & ~n257;
  assign n259 = ~n198 & ~n258;
  assign n260 = ~n189 & ~n259;
  assign n261 = T_REG_6_ & n181;
  assign n262 = X_REG_6_ & ~STATO_REG_2_;
  assign n263 = Y_REG_6_ & n182_1;
  assign n264 = ~n261 & ~n262;
  assign n265 = ~n263 & n264;
  assign n266 = n189 & n259;
  assign n267 = ~n265 & ~n266;
  assign n268 = ~n260 & ~n267;
  assign n269 = CONT_REG_7_ & ~n187_1;
  assign n270 = n268 & ~n269;
  assign n271 = ~n268 & n269;
  assign n272 = ~n270 & ~n271;
  assign n273 = n170 & n177_1;
  assign n274 = n272 & n273;
  assign n275 = ~n179 & ~n180;
  assign n20 = n274 | ~n275;
  assign n277 = CONT_REG_6_ & n178;
  assign n278 = PUNTI_RETTA_REG_6_ & ~n169;
  assign n279 = ~n189 & n265;
  assign n280 = n189 & ~n265;
  assign n281 = ~n279 & ~n280;
  assign n282 = ~n259 & ~n281;
  assign n283 = n259 & n281;
  assign n284 = ~n282 & ~n283;
  assign n285 = n273 & n284;
  assign n286 = ~n277 & ~n278;
  assign n24 = n285 | ~n286;
  assign n288 = CONT_REG_5_ & n178;
  assign n289 = PUNTI_RETTA_REG_5_ & ~n169;
  assign n290 = n194 & ~n197_1;
  assign n291 = ~n194 & n197_1;
  assign n292 = ~n290 & ~n291;
  assign n293 = ~n257 & ~n292;
  assign n294 = n257 & n292;
  assign n295 = ~n293 & ~n294;
  assign n296 = n273 & n295;
  assign n297 = ~n288 & ~n289;
  assign n28 = n296 | ~n297;
  assign n299 = CONT_REG_4_ & n178;
  assign n300 = PUNTI_RETTA_REG_4_ & ~n169;
  assign n301 = n204 & ~n207_1;
  assign n302 = ~n204 & n207_1;
  assign n303 = ~n301 & ~n302;
  assign n304 = ~n255 & ~n303;
  assign n305 = n255 & n303;
  assign n306 = ~n304 & ~n305;
  assign n307 = n273 & n306;
  assign n308 = ~n299 & ~n300;
  assign n32 = n307 | ~n308;
  assign n310 = CONT_REG_3_ & n178;
  assign n311 = PUNTI_RETTA_REG_3_ & ~n169;
  assign n312 = n214 & ~n217_1;
  assign n313 = ~n214 & n217_1;
  assign n314 = ~n312 & ~n313;
  assign n315 = ~n253 & ~n314;
  assign n316 = n253 & n314;
  assign n317 = ~n315 & ~n316;
  assign n318 = n273 & n317;
  assign n319 = ~n310 & ~n311;
  assign n36 = n318 | ~n319;
  assign n321 = CONT_REG_2_ & n178;
  assign n322 = PUNTI_RETTA_REG_2_ & ~n169;
  assign n323 = n224 & ~n227_1;
  assign n324 = ~n224 & n227_1;
  assign n325 = ~n323 & ~n324;
  assign n326 = ~n251 & ~n325;
  assign n327 = n251 & n325;
  assign n328 = ~n326 & ~n327;
  assign n329 = n273 & n328;
  assign n330 = ~n321 & ~n322;
  assign n40 = n329 | ~n330;
  assign n332 = CONT_REG_1_ & n178;
  assign n333 = PUNTI_RETTA_REG_1_ & ~n169;
  assign n334 = ~n234 & n237_1;
  assign n335 = n234 & ~n237_1;
  assign n336 = ~n334 & ~n335;
  assign n337 = ~n248 & ~n336;
  assign n338 = n248 & n336;
  assign n339 = ~n337 & ~n338;
  assign n340 = n273 & ~n339;
  assign n341 = ~n332 & ~n333;
  assign n44 = n340 | ~n341;
  assign n343 = CONT_REG_0_ & n178;
  assign n344 = n244 & ~n247_1;
  assign n345 = ~n244 & n247_1;
  assign n346 = ~n344 & ~n345;
  assign n347 = n273 & ~n346;
  assign n348 = PUNTI_RETTA_REG_0_ & ~n169;
  assign n349 = ~n343 & ~n347;
  assign n48 = n348 | ~n349;
  assign n351 = n177_1 & n186;
  assign n352 = START & STATO_REG_0_;
  assign n353 = n166 & n352;
  assign n354 = ~n351 & ~n353;
  assign n355 = CONT_REG_7_ & n354;
  assign n356 = STATO_REG_2_ & ~n354;
  assign n357 = n272 & n356;
  assign n52 = n355 | n357;
  assign n359 = CONT_REG_6_ & n354;
  assign n360 = n284 & n356;
  assign n57 = n359 | n360;
  assign n362 = CONT_REG_5_ & n354;
  assign n363 = n295 & n356;
  assign n62 = n362 | n363;
  assign n365 = CONT_REG_4_ & n354;
  assign n366 = n306 & n356;
  assign n67 = n365 | n366;
  assign n368 = CONT_REG_3_ & n354;
  assign n369 = n317 & n356;
  assign n72 = n368 | n369;
  assign n371 = CONT_REG_2_ & n354;
  assign n372 = n328 & n356;
  assign n77 = n371 | n372;
  assign n374 = CONT_REG_1_ & n354;
  assign n375 = ~n339 & n356;
  assign n82 = n374 | n375;
  assign n377 = CONT_REG_0_ & n354;
  assign n378 = ~n346 & n356;
  assign n87 = n377 | n378;
  assign n380 = STATO_REG_1_ & STATO_REG_0_;
  assign n381 = ~STATO_REG_2_ & n352;
  assign n382 = ~n380 & ~n381;
  assign n383 = ~n186 & n382;
  assign n92 = MAR_REG_7_ & n383;
  assign n97 = MAR_REG_6_ & n383;
  assign n102 = MAR_REG_5_ & n383;
  assign n107 = MAR_REG_4_ & n383;
  assign n388 = ~n166 & ~n383;
  assign n389 = n163 & n388;
  assign n390 = ~MAR_REG_3_ & n389;
  assign n391 = ~n163 & ~n166;
  assign n392 = ~n383 & ~n391;
  assign n393 = MAR_REG_3_ & ~n392;
  assign n112 = n390 | n393;
  assign n395 = n162_1 & n388;
  assign n396 = ~MAR_REG_2_ & n395;
  assign n397 = ~n162_1 & ~n166;
  assign n398 = ~n383 & ~n397;
  assign n399 = MAR_REG_2_ & ~n398;
  assign n117 = n396 | n399;
  assign n401 = MAR_REG_0_ & n388;
  assign n402 = ~MAR_REG_1_ & n401;
  assign n403 = ~MAR_REG_0_ & ~n166;
  assign n404 = ~n383 & ~n403;
  assign n405 = MAR_REG_1_ & ~n404;
  assign n122 = n402 | n405;
  assign n407 = MAR_REG_0_ & n383;
  assign n408 = ~MAR_REG_0_ & n388;
  assign n127 = n407 | n408;
  assign n410 = ~STATO_REG_2_ & ~STATO_REG_0_;
  assign n411 = STATO_REG_1_ & n410;
  assign n412 = STATO_REG_2_ & ~STATO_REG_1_;
  assign n413 = ~n411 & ~n412;
  assign n414 = X_REG_7_ & n413;
  assign n415 = ~STATO_REG_2_ & ~n413;
  assign n416 = MAR_REG_3_ & ~MAR_REG_2_;
  assign n417 = ~MAR_REG_0_ & n416;
  assign n418 = MAR_REG_1_ & n417;
  assign n419 = ~MAR_REG_3_ & ~MAR_REG_2_;
  assign n420 = MAR_REG_0_ & n419;
  assign n421 = ~MAR_REG_1_ & n420;
  assign n422 = ~n418 & ~n421;
  assign n423 = n415 & ~n422;
  assign n424 = ~n410 & ~n413;
  assign n425 = n272 & n424;
  assign n426 = ~n414 & ~n423;
  assign n132 = n425 | ~n426;
  assign n428 = X_REG_6_ & n413;
  assign n429 = n284 & n424;
  assign n430 = ~n423 & ~n428;
  assign n137 = n429 | ~n430;
  assign n432 = X_REG_5_ & n413;
  assign n433 = n295 & n424;
  assign n434 = ~n423 & ~n432;
  assign n142 = n433 | ~n434;
  assign n436 = X_REG_4_ & n413;
  assign n437 = n306 & n424;
  assign n438 = ~n423 & ~n436;
  assign n147 = n437 | ~n438;
  assign n440 = X_REG_3_ & n413;
  assign n441 = n317 & n424;
  assign n442 = ~n423 & ~n440;
  assign n152 = n441 | ~n442;
  assign n444 = X_REG_2_ & n413;
  assign n445 = n162_1 & n416;
  assign n446 = n422 & ~n445;
  assign n447 = n415 & ~n446;
  assign n448 = n328 & n424;
  assign n449 = ~n444 & ~n447;
  assign n157 = n448 | ~n449;
  assign n451 = X_REG_1_ & n413;
  assign n452 = ~MAR_REG_1_ & MAR_REG_0_;
  assign n453 = MAR_REG_3_ & n163;
  assign n454 = n422 & ~n452;
  assign n455 = ~n453 & n454;
  assign n456 = n415 & ~n455;
  assign n457 = ~n339 & n424;
  assign n458 = ~n451 & ~n456;
  assign n162 = n457 | ~n458;
  assign n460 = X_REG_0_ & n413;
  assign n461 = ~MAR_REG_2_ & ~MAR_REG_1_;
  assign n462 = ~MAR_REG_3_ & n461;
  assign n463 = n446 & ~n462;
  assign n464 = n415 & ~n463;
  assign n465 = ~n346 & n424;
  assign n466 = ~n460 & ~n464;
  assign n167 = n465 | ~n466;
  assign n468 = STATO_REG_2_ & n181;
  assign n469 = ~n422 & n468;
  assign n470 = Y_REG_3_ & ~n468;
  assign n172 = n469 | n470;
  assign n472 = Y_REG_1_ & ~n468;
  assign n473 = ~n455 & n468;
  assign n177 = n472 | n473;
  assign n475 = Y_REG_5_ & ~n468;
  assign n182 = n469 | n475;
  assign n477 = n317 & n380;
  assign n478 = T_REG_3_ & ~n380;
  assign n187 = n477 | n478;
  assign n480 = n295 & n380;
  assign n481 = T_REG_5_ & ~n380;
  assign n192 = n480 | n481;
  assign n483 = ~n339 & n380;
  assign n484 = T_REG_1_ & ~n380;
  assign n197 = n483 | n484;
  assign n486 = ~n346 & n380;
  assign n487 = T_REG_0_ & ~n380;
  assign n202 = n486 | n487;
  assign n489 = n306 & n380;
  assign n490 = T_REG_4_ & ~n380;
  assign n207 = n489 | n490;
  assign n492 = n284 & n380;
  assign n493 = T_REG_6_ & ~n380;
  assign n212 = n492 | n493;
  assign n495 = n328 & n380;
  assign n496 = T_REG_2_ & ~n380;
  assign n217 = n495 | n496;
  assign n498 = Y_REG_4_ & ~n468;
  assign n222 = n469 | n498;
  assign n500 = Y_REG_0_ & ~n468;
  assign n501 = ~n463 & n468;
  assign n227 = n500 | n501;
  assign n503 = Y_REG_2_ & ~n468;
  assign n504 = ~n446 & n468;
  assign n232 = n503 | n504;
  assign n506 = Y_REG_6_ & ~n468;
  assign n237 = n469 | n506;
  assign n508 = START & n164;
  assign n509 = STATO_REG_1_ & ~n508;
  assign n510 = STATO_REG_2_ & ~n509;
  assign n242 = n380 | n510;
  assign n512 = ~START & n164;
  assign n513 = ~STATO_REG_0_ & ~n512;
  assign n514 = ~n410 & ~n513;
  assign n515 = STATO_REG_1_ & ~n514;
  assign n516 = ~STATO_REG_1_ & n352;
  assign n517 = ~n515 & ~n516;
  assign n247 = n182_1 | ~n517;
  assign n519 = ~n165 & ~n166;
  assign n520 = ~START & ~n519;
  assign n521 = ~n181 & ~n410;
  assign n252 = n520 | ~n521;
  always @ (posedge clock) begin
    PUNTI_RETTA_REG_7_ <= n20;
    PUNTI_RETTA_REG_6_ <= n24;
    PUNTI_RETTA_REG_5_ <= n28;
    PUNTI_RETTA_REG_4_ <= n32;
    PUNTI_RETTA_REG_3_ <= n36;
    PUNTI_RETTA_REG_2_ <= n40;
    PUNTI_RETTA_REG_1_ <= n44;
    PUNTI_RETTA_REG_0_ <= n48;
    CONT_REG_7_ <= n52;
    CONT_REG_6_ <= n57;
    CONT_REG_5_ <= n62;
    CONT_REG_4_ <= n67;
    CONT_REG_3_ <= n72;
    CONT_REG_2_ <= n77;
    CONT_REG_1_ <= n82;
    CONT_REG_0_ <= n87;
    MAR_REG_7_ <= n92;
    MAR_REG_6_ <= n97;
    MAR_REG_5_ <= n102;
    MAR_REG_4_ <= n107;
    MAR_REG_3_ <= n112;
    MAR_REG_2_ <= n117;
    MAR_REG_1_ <= n122;
    MAR_REG_0_ <= n127;
    X_REG_7_ <= n132;
    X_REG_6_ <= n137;
    X_REG_5_ <= n142;
    X_REG_4_ <= n147;
    X_REG_3_ <= n152;
    X_REG_2_ <= n157;
    X_REG_1_ <= n162;
    X_REG_0_ <= n167;
    Y_REG_3_ <= n172;
    Y_REG_1_ <= n177;
    Y_REG_5_ <= n182;
    T_REG_3_ <= n187;
    T_REG_5_ <= n192;
    T_REG_1_ <= n197;
    T_REG_0_ <= n202;
    T_REG_4_ <= n207;
    T_REG_6_ <= n212;
    T_REG_2_ <= n217;
    Y_REG_4_ <= n222;
    Y_REG_0_ <= n227;
    Y_REG_2_ <= n232;
    Y_REG_6_ <= n237;
    STATO_REG_2_ <= n242;
    STATO_REG_1_ <= n247;
    STATO_REG_0_ <= n252;
  end
endmodule


