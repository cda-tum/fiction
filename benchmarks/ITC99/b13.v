// Benchmark "b13" written by ABC on Wed Sep  5 10:17:20 2018

module b13 ( clock, 
    EOC, DATA_IN_7_, DATA_IN_6_, DATA_IN_5_, DATA_IN_4_, DATA_IN_3_,
    DATA_IN_2_, DATA_IN_1_, DATA_IN_0_, DSR,
    SOC_REG, LOAD_DATO_REG, ADD_MPX2_REG, CANALE_REG_3_, CANALE_REG_2_,
    CANALE_REG_1_, CANALE_REG_0_, MUX_EN_REG, ERROR_REG, DATA_OUT_REG  );
  input  clock;
  input  EOC, DATA_IN_7_, DATA_IN_6_, DATA_IN_5_, DATA_IN_4_, DATA_IN_3_,
    DATA_IN_2_, DATA_IN_1_, DATA_IN_0_, DSR;
  output SOC_REG, LOAD_DATO_REG, ADD_MPX2_REG, CANALE_REG_3_, CANALE_REG_2_,
    CANALE_REG_1_, CANALE_REG_0_, MUX_EN_REG, ERROR_REG, DATA_OUT_REG;
  reg CANALE_REG_3_, CANALE_REG_2_, CANALE_REG_1_, CANALE_REG_0_,
    CONTA_TMP_REG_3_, CONTA_TMP_REG_2_, CONTA_TMP_REG_1_, CONTA_TMP_REG_0_,
    ITFC_STATE_REG_1_, ITFC_STATE_REG_0_, OUT_REG_REG_7_, OUT_REG_REG_6_,
    OUT_REG_REG_5_, OUT_REG_REG_4_, OUT_REG_REG_3_, OUT_REG_REG_2_,
    OUT_REG_REG_1_, OUT_REG_REG_0_, NEXT_BIT_REG_3_, NEXT_BIT_REG_2_,
    NEXT_BIT_REG_1_, NEXT_BIT_REG_0_, TX_CONTA_REG_9_, TX_CONTA_REG_8_,
    TX_CONTA_REG_7_, TX_CONTA_REG_6_, TX_CONTA_REG_5_, TX_CONTA_REG_4_,
    TX_CONTA_REG_3_, TX_CONTA_REG_2_, TX_CONTA_REG_1_, TX_CONTA_REG_0_,
    LOAD_REG, SEND_DATA_REG, SEND_EN_REG, MUX_EN_REG, TRE_REG,
    LOAD_DATO_REG, SOC_REG, SEND_REG, MPX_REG, CONFIRM_REG, SHOT_REG,
    ADD_MPX2_REG, RDY_REG, ERROR_REG, S1_REG_2_, S1_REG_1_, S1_REG_0_,
    S2_REG_1_, S2_REG_0_, TX_END_REG, DATA_OUT_REG;
  wire n180, n181, n182, n183_1, n184, n185, n186, n188_1, n189, n190, n191,
    n193_1, n194, n195, n196, n197, n198_1, n200, n202, n204, n205, n207,
    n208_1, n210, n212, n213_1, n214, n216, n217_1, n218, n220, n221,
    n222_1, n224, n225, n227, n228, n230_1, n231, n233, n234, n236, n237,
    n239, n240_1, n242, n243, n245_1, n246, n247, n248, n249, n250_1, n251,
    n252, n253, n254_1, n255, n256, n257, n258, n260, n261, n262, n263_1,
    n264, n266, n267, n268_1, n269, n270, n271, n273_1, n274, n275, n276,
    n277, n279, n280, n281, n282, n283_1, n284, n285, n286, n287, n288_1,
    n289, n290, n291, n292, n294, n295, n296, n297, n298, n300, n301, n302,
    n303, n304, n306, n307, n308, n309, n310, n312, n313, n314, n315, n316,
    n318, n319, n320, n321, n322, n324, n325, n326, n327, n328, n330, n331,
    n332, n333, n334, n336, n337, n338, n339, n340, n342, n343, n345, n346,
    n347, n348, n350, n351, n352, n353, n354, n356, n357, n358, n360, n361,
    n362, n363, n364, n365, n367, n369, n371, n372, n374, n376, n377, n378,
    n379, n381, n382, n383, n385, n386, n387, n388, n391, n392, n393, n394,
    n396, n397, n398, n399, n400, n401, n404, n405, n406, n407, n408, n410,
    n411, n412, n413, n414, n416, n417, n418, n421, n423, n424, n425, n426,
    n427, n428, n429, n430, n431, n432, n433, n434, n435, n436, n437, n438,
    n439, n440, n42, n46, n50, n54, n58, n63, n68, n73, n78, n83, n88, n93,
    n98, n103, n108, n113, n118, n123, n128, n133, n138, n143, n148, n153,
    n158, n163, n168, n173, n178, n183, n188, n193, n198, n203, n208, n213,
    n217, n222, n226, n230, n235, n240, n245, n250, n254, n259, n263, n268,
    n273, n278, n283, n288, n293;
  assign n180 = S1_REG_1_ & ~S1_REG_0_;
  assign n181 = S1_REG_2_ & n180;
  assign n182 = CANALE_REG_3_ & ~n181;
  assign n183_1 = CONTA_TMP_REG_1_ & CONTA_TMP_REG_0_;
  assign n184 = CONTA_TMP_REG_2_ & n183_1;
  assign n185 = n181 & ~n184;
  assign n186 = CONTA_TMP_REG_3_ & n185;
  assign n42 = n182 | n186;
  assign n188_1 = CONTA_TMP_REG_2_ & n185;
  assign n189 = n183_1 & n185;
  assign n190 = CANALE_REG_2_ & ~n181;
  assign n191 = ~n188_1 & ~n189;
  assign n46 = n190 | ~n191;
  assign n193_1 = CANALE_REG_1_ & ~n181;
  assign n194 = ~CONTA_TMP_REG_1_ & CONTA_TMP_REG_0_;
  assign n195 = n185 & n194;
  assign n196 = ~CONTA_TMP_REG_0_ & n185;
  assign n197 = CONTA_TMP_REG_1_ & n196;
  assign n198_1 = ~n193_1 & ~n195;
  assign n50 = n197 | ~n198_1;
  assign n200 = CANALE_REG_0_ & ~n181;
  assign n54 = n196 | n200;
  assign n202 = n181 & n184;
  assign n58 = CONTA_TMP_REG_3_ & ~n202;
  assign n204 = n181 & n183_1;
  assign n205 = CONTA_TMP_REG_2_ & ~n204;
  assign n63 = n189 | n205;
  assign n207 = CONTA_TMP_REG_0_ & n181;
  assign n208_1 = CONTA_TMP_REG_1_ & ~n207;
  assign n68 = n195 | n208_1;
  assign n210 = CONTA_TMP_REG_0_ & ~n181;
  assign n73 = n196 | n210;
  assign n212 = ITFC_STATE_REG_1_ & TX_END_REG;
  assign n213_1 = ITFC_STATE_REG_0_ & ~n212;
  assign n214 = ITFC_STATE_REG_1_ & ~ITFC_STATE_REG_0_;
  assign n78 = n213_1 | n214;
  assign n216 = ~ITFC_STATE_REG_0_ & SHOT_REG;
  assign n217_1 = ITFC_STATE_REG_0_ & TX_END_REG;
  assign n218 = ITFC_STATE_REG_1_ & ~n217_1;
  assign n83 = n216 | n218;
  assign n220 = LOAD_REG & ~TRE_REG;
  assign n221 = DATA_IN_7_ & n220;
  assign n222_1 = OUT_REG_REG_7_ & ~n220;
  assign n88 = n221 | n222_1;
  assign n224 = DATA_IN_6_ & n220;
  assign n225 = OUT_REG_REG_6_ & ~n220;
  assign n93 = n224 | n225;
  assign n227 = DATA_IN_5_ & n220;
  assign n228 = OUT_REG_REG_5_ & ~n220;
  assign n98 = n227 | n228;
  assign n230_1 = DATA_IN_4_ & n220;
  assign n231 = OUT_REG_REG_4_ & ~n220;
  assign n103 = n230_1 | n231;
  assign n233 = DATA_IN_3_ & n220;
  assign n234 = OUT_REG_REG_3_ & ~n220;
  assign n108 = n233 | n234;
  assign n236 = DATA_IN_2_ & n220;
  assign n237 = OUT_REG_REG_2_ & ~n220;
  assign n113 = n236 | n237;
  assign n239 = DATA_IN_1_ & n220;
  assign n240_1 = OUT_REG_REG_1_ & ~n220;
  assign n118 = n239 | n240_1;
  assign n242 = DATA_IN_0_ & n220;
  assign n243 = OUT_REG_REG_0_ & ~n220;
  assign n123 = n242 | n243;
  assign n245_1 = NEXT_BIT_REG_2_ & NEXT_BIT_REG_1_;
  assign n246 = ~TX_CONTA_REG_2_ & ~TX_CONTA_REG_0_;
  assign n247 = ~TX_CONTA_REG_1_ & n246;
  assign n248 = TX_CONTA_REG_3_ & ~n247;
  assign n249 = ~TX_CONTA_REG_4_ & ~n248;
  assign n250_1 = TX_CONTA_REG_6_ & TX_CONTA_REG_5_;
  assign n251 = ~n249 & n250_1;
  assign n252 = ~TX_CONTA_REG_9_ & ~TX_CONTA_REG_7_;
  assign n253 = ~TX_CONTA_REG_8_ & n252;
  assign n254_1 = ~n251 & n253;
  assign n255 = SEND_EN_REG & ~n254_1;
  assign n256 = NEXT_BIT_REG_0_ & n255;
  assign n257 = n245_1 & n256;
  assign n258 = NEXT_BIT_REG_3_ & ~n256;
  assign n128 = n257 | n258;
  assign n260 = ~NEXT_BIT_REG_2_ & NEXT_BIT_REG_1_;
  assign n261 = n256 & n260;
  assign n262 = NEXT_BIT_REG_2_ & ~n256;
  assign n263_1 = NEXT_BIT_REG_2_ & ~NEXT_BIT_REG_1_;
  assign n264 = ~n261 & ~n262;
  assign n133 = n263_1 | ~n264;
  assign n266 = NEXT_BIT_REG_1_ & ~n256;
  assign n267 = ~NEXT_BIT_REG_3_ & ~NEXT_BIT_REG_2_;
  assign n268_1 = ~NEXT_BIT_REG_0_ & ~n267;
  assign n269 = NEXT_BIT_REG_0_ & ~n263_1;
  assign n270 = ~n268_1 & ~n269;
  assign n271 = n255 & n270;
  assign n138 = n266 | n271;
  assign n273_1 = ~NEXT_BIT_REG_2_ & ~NEXT_BIT_REG_1_;
  assign n274 = ~NEXT_BIT_REG_0_ & ~n273_1;
  assign n275 = ~NEXT_BIT_REG_3_ & ~n274;
  assign n276 = n255 & ~n275;
  assign n277 = NEXT_BIT_REG_0_ & ~n255;
  assign n143 = n276 | n277;
  assign n279 = TX_CONTA_REG_9_ & ~SEND_EN_REG;
  assign n280 = TX_CONTA_REG_1_ & TX_CONTA_REG_0_;
  assign n281 = TX_CONTA_REG_2_ & n280;
  assign n282 = TX_CONTA_REG_3_ & n281;
  assign n283_1 = TX_CONTA_REG_4_ & n282;
  assign n284 = TX_CONTA_REG_5_ & n283_1;
  assign n285 = TX_CONTA_REG_6_ & n284;
  assign n286 = TX_CONTA_REG_7_ & n285;
  assign n287 = TX_CONTA_REG_8_ & n286;
  assign n288_1 = ~TX_CONTA_REG_9_ & n287;
  assign n289 = TX_CONTA_REG_9_ & ~n287;
  assign n290 = ~n288_1 & ~n289;
  assign n291 = SEND_EN_REG & n254_1;
  assign n292 = ~n290 & n291;
  assign n148 = n279 | n292;
  assign n294 = TX_CONTA_REG_8_ & ~SEND_EN_REG;
  assign n295 = ~TX_CONTA_REG_8_ & n286;
  assign n296 = TX_CONTA_REG_8_ & ~n286;
  assign n297 = ~n295 & ~n296;
  assign n298 = n291 & ~n297;
  assign n153 = n294 | n298;
  assign n300 = TX_CONTA_REG_7_ & ~SEND_EN_REG;
  assign n301 = ~TX_CONTA_REG_7_ & n285;
  assign n302 = TX_CONTA_REG_7_ & ~n285;
  assign n303 = ~n301 & ~n302;
  assign n304 = n291 & ~n303;
  assign n158 = n300 | n304;
  assign n306 = TX_CONTA_REG_6_ & ~SEND_EN_REG;
  assign n307 = ~TX_CONTA_REG_6_ & n284;
  assign n308 = TX_CONTA_REG_6_ & ~n284;
  assign n309 = ~n307 & ~n308;
  assign n310 = n291 & ~n309;
  assign n163 = n306 | n310;
  assign n312 = TX_CONTA_REG_5_ & ~SEND_EN_REG;
  assign n313 = ~TX_CONTA_REG_5_ & n283_1;
  assign n314 = TX_CONTA_REG_5_ & ~n283_1;
  assign n315 = ~n313 & ~n314;
  assign n316 = n291 & ~n315;
  assign n168 = n312 | n316;
  assign n318 = TX_CONTA_REG_4_ & ~SEND_EN_REG;
  assign n319 = ~TX_CONTA_REG_4_ & n282;
  assign n320 = TX_CONTA_REG_4_ & ~n282;
  assign n321 = ~n319 & ~n320;
  assign n322 = n291 & ~n321;
  assign n173 = n318 | n322;
  assign n324 = TX_CONTA_REG_3_ & ~SEND_EN_REG;
  assign n325 = ~TX_CONTA_REG_3_ & n281;
  assign n326 = TX_CONTA_REG_3_ & ~n281;
  assign n327 = ~n325 & ~n326;
  assign n328 = n291 & ~n327;
  assign n178 = n324 | n328;
  assign n330 = TX_CONTA_REG_2_ & ~SEND_EN_REG;
  assign n331 = ~TX_CONTA_REG_2_ & n280;
  assign n332 = TX_CONTA_REG_2_ & ~n280;
  assign n333 = ~n331 & ~n332;
  assign n334 = n291 & ~n333;
  assign n183 = n330 | n334;
  assign n336 = TX_CONTA_REG_1_ & ~SEND_EN_REG;
  assign n337 = ~TX_CONTA_REG_1_ & TX_CONTA_REG_0_;
  assign n338 = TX_CONTA_REG_1_ & ~TX_CONTA_REG_0_;
  assign n339 = ~n337 & ~n338;
  assign n340 = n291 & ~n339;
  assign n188 = n336 | n340;
  assign n342 = TX_CONTA_REG_0_ & ~SEND_EN_REG;
  assign n343 = ~TX_CONTA_REG_0_ & n291;
  assign n193 = n342 | n343;
  assign n345 = ~ITFC_STATE_REG_1_ & ITFC_STATE_REG_0_;
  assign n346 = LOAD_REG & ~n345;
  assign n347 = ~ITFC_STATE_REG_1_ & ~ITFC_STATE_REG_0_;
  assign n348 = SHOT_REG & n347;
  assign n198 = n346 | n348;
  assign n350 = RDY_REG & S1_REG_1_;
  assign n351 = S1_REG_0_ & n350;
  assign n352 = SEND_DATA_REG & ~n351;
  assign n353 = S1_REG_1_ & S1_REG_0_;
  assign n354 = S1_REG_2_ & n353;
  assign n203 = n352 | n354;
  assign n356 = SEND_EN_REG & ~TX_END_REG;
  assign n357 = TRE_REG & SEND_REG;
  assign n358 = DSR & n357;
  assign n208 = n356 | n358;
  assign n360 = S1_REG_2_ & S1_REG_0_;
  assign n361 = ~S1_REG_1_ & n360;
  assign n362 = ~EOC & n361;
  assign n363 = MUX_EN_REG & ~n362;
  assign n364 = ~S1_REG_2_ & ~S1_REG_1_;
  assign n365 = ~S1_REG_0_ & n364;
  assign n213 = n363 | n365;
  assign n367 = ~TRE_REG & ~TX_END_REG;
  assign n217 = LOAD_REG | ~n367;
  assign n369 = LOAD_DATO_REG & ~n181;
  assign n222 = n362 | n369;
  assign n371 = SOC_REG & ~n180;
  assign n372 = ~S1_REG_2_ & n180;
  assign n226 = n371 | n372;
  assign n374 = SEND_REG & ~n214;
  assign n230 = n345 | n374;
  assign n376 = S2_REG_1_ & ~S2_REG_0_;
  assign n377 = CONFIRM_REG & n376;
  assign n378 = ~MPX_REG & n377;
  assign n379 = MPX_REG & ~n377;
  assign n235 = n378 | n379;
  assign n381 = CONFIRM_REG & ~n347;
  assign n382 = ITFC_STATE_REG_1_ & ITFC_STATE_REG_0_;
  assign n383 = TX_END_REG & n382;
  assign n240 = n381 | n383;
  assign n385 = ~S2_REG_1_ & S2_REG_0_;
  assign n386 = ~CONFIRM_REG & ~S2_REG_0_;
  assign n387 = S2_REG_1_ & n386;
  assign n388 = SHOT_REG & ~n387;
  assign n245 = n385 | n388;
  assign n250 = ADD_MPX2_REG | n378;
  assign n391 = ~S2_REG_1_ & ~S2_REG_0_;
  assign n392 = SEND_DATA_REG & n391;
  assign n393 = MPX_REG & n377;
  assign n394 = RDY_REG & ~n393;
  assign n254 = n392 | n394;
  assign n396 = LOAD_REG & TRE_REG;
  assign n397 = ~LOAD_REG & ERROR_REG;
  assign n398 = ~n396 & ~n397;
  assign n399 = ~SEND_REG & ~n398;
  assign n400 = DSR & TRE_REG;
  assign n401 = SEND_REG & ~n400;
  assign n259 = n399 | n401;
  assign n263 = n180 | n360;
  assign n404 = ~EOC & ~S1_REG_1_;
  assign n405 = ~S1_REG_2_ & ~n350;
  assign n406 = ~n404 & ~n405;
  assign n407 = S1_REG_0_ & ~n406;
  assign n408 = S1_REG_2_ & ~S1_REG_0_;
  assign n268 = n407 | n408;
  assign n410 = ~RDY_REG & ~S1_REG_2_;
  assign n411 = S1_REG_1_ & n410;
  assign n412 = EOC & S1_REG_2_;
  assign n413 = ~S1_REG_1_ & n412;
  assign n414 = ~n411 & ~n413;
  assign n273 = ~S1_REG_0_ | ~n414;
  assign n416 = ~MPX_REG & CONFIRM_REG;
  assign n417 = S2_REG_1_ & ~n416;
  assign n418 = ~S2_REG_0_ & n417;
  assign n278 = n385 | n418;
  assign n283 = n377 | n392;
  assign n421 = ~NEXT_BIT_REG_1_ & n267;
  assign n288 = n256 & n421;
  assign n423 = OUT_REG_REG_1_ & NEXT_BIT_REG_3_;
  assign n424 = OUT_REG_REG_5_ & n263_1;
  assign n425 = OUT_REG_REG_3_ & n245_1;
  assign n426 = OUT_REG_REG_7_ & n260;
  assign n427 = ~n423 & ~n424;
  assign n428 = ~n425 & n427;
  assign n429 = ~n426 & n428;
  assign n430 = ~NEXT_BIT_REG_0_ & ~n429;
  assign n431 = OUT_REG_REG_2_ & n245_1;
  assign n432 = OUT_REG_REG_6_ & n260;
  assign n433 = OUT_REG_REG_0_ & NEXT_BIT_REG_3_;
  assign n434 = OUT_REG_REG_4_ & n263_1;
  assign n435 = ~n431 & ~n432;
  assign n436 = ~n421 & n435;
  assign n437 = ~n433 & n436;
  assign n438 = ~n434 & n437;
  assign n439 = NEXT_BIT_REG_0_ & ~n438;
  assign n440 = ~n430 & ~n439;
  assign n293 = ~n255 | ~n440;
  always @ (posedge clock) begin
    CANALE_REG_3_ <= n42;
    CANALE_REG_2_ <= n46;
    CANALE_REG_1_ <= n50;
    CANALE_REG_0_ <= n54;
    CONTA_TMP_REG_3_ <= n58;
    CONTA_TMP_REG_2_ <= n63;
    CONTA_TMP_REG_1_ <= n68;
    CONTA_TMP_REG_0_ <= n73;
    ITFC_STATE_REG_1_ <= n78;
    ITFC_STATE_REG_0_ <= n83;
    OUT_REG_REG_7_ <= n88;
    OUT_REG_REG_6_ <= n93;
    OUT_REG_REG_5_ <= n98;
    OUT_REG_REG_4_ <= n103;
    OUT_REG_REG_3_ <= n108;
    OUT_REG_REG_2_ <= n113;
    OUT_REG_REG_1_ <= n118;
    OUT_REG_REG_0_ <= n123;
    NEXT_BIT_REG_3_ <= n128;
    NEXT_BIT_REG_2_ <= n133;
    NEXT_BIT_REG_1_ <= n138;
    NEXT_BIT_REG_0_ <= n143;
    TX_CONTA_REG_9_ <= n148;
    TX_CONTA_REG_8_ <= n153;
    TX_CONTA_REG_7_ <= n158;
    TX_CONTA_REG_6_ <= n163;
    TX_CONTA_REG_5_ <= n168;
    TX_CONTA_REG_4_ <= n173;
    TX_CONTA_REG_3_ <= n178;
    TX_CONTA_REG_2_ <= n183;
    TX_CONTA_REG_1_ <= n188;
    TX_CONTA_REG_0_ <= n193;
    LOAD_REG <= n198;
    SEND_DATA_REG <= n203;
    SEND_EN_REG <= n208;
    MUX_EN_REG <= n213;
    TRE_REG <= n217;
    LOAD_DATO_REG <= n222;
    SOC_REG <= n226;
    SEND_REG <= n230;
    MPX_REG <= n235;
    CONFIRM_REG <= n240;
    SHOT_REG <= n245;
    ADD_MPX2_REG <= n250;
    RDY_REG <= n254;
    ERROR_REG <= n259;
    S1_REG_2_ <= n263;
    S1_REG_1_ <= n268;
    S1_REG_0_ <= n273;
    S2_REG_1_ <= n278;
    S2_REG_0_ <= n283;
    TX_END_REG <= n288;
    DATA_OUT_REG <= n293;
  end
endmodule


