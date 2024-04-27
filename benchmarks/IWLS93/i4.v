// Benchmark "top" written by ABC on Mon Feb 19 11:52:43 2024

module top ( 
    pv28_20_, pv56_12_, pv56_23_, pv88_6_, pv88_19_, pv120_16_, pv120_29_,
    pv168_9_, pv180_10_, pv28_10_, pv56_13_, pv56_22_, pv88_7_, pv88_29_,
    pv120_15_, pv180_4_, pv56_14_, pv56_25_, pv88_8_, pv88_17_, pv88_28_,
    pv120_18_, pv180_5_, pv180_12_, pv192_0_, pv56_15_, pv56_24_, pv88_9_,
    pv88_18_, pv88_27_, pv120_17_, pv168_8_, pv180_6_, pv88_2_, pv88_15_,
    pv88_26_, pv120_12_, pv88_3_, pv88_16_, pv88_25_, pv120_11_, pv180_0_,
    pv56_10_, pv56_21_, pv88_4_, pv88_13_, pv88_24_, pv120_14_, pv126_5_,
    pv180_1_, pv56_11_, pv56_20_, pv88_5_, pv88_14_, pv88_23_, pv120_13_,
    pv180_2_, pv28_8_, pv56_5_, pv88_11_, pv88_22_, pv120_5_, pv132_0_,
    pv156_6_, pv168_1_, pv189_0_, pv28_9_, pv56_4_, pv88_12_, pv88_21_,
    pv120_6_, pv156_5_, pv156_10_, pv168_2_, pv189_1_, pv28_6_, pv56_7_,
    pv88_20_, pv120_3_, pv120_10_, pv156_8_, pv189_2_, pv28_7_, pv56_6_,
    pv88_10_, pv120_4_, pv168_0_, pv28_4_, pv56_9_, pv120_1_, pv168_5_,
    pv28_5_, pv56_8_, pv120_2_, pv156_9_, pv156_14_, pv168_6_, pv28_2_,
    pv156_13_, pv28_3_, pv120_0_, pv156_12_, pv168_4_, pv28_0_, pv186_1_,
    pv28_1_, pv144_4_, pv186_2_, pv144_5_, pv156_0_, pv132_5_, pv144_6_,
    pv56_1_, pv120_9_, pv120_30_, pv132_4_, pv156_2_, pv56_0_, pv88_30_,
    pv132_3_, pv144_8_, pv156_1_, pv28_19_, pv56_3_, pv88_31_, pv120_7_,
    pv132_2_, pv144_9_, pv156_4_, pv56_2_, pv120_8_, pv120_20_, pv132_1_,
    pv186_0_, pv28_17_, pv120_21_, pv126_3_, pv144_10_, pv168_12_,
    pv183_2_, pv28_18_, pv28_27_, pv120_22_, pv126_4_, pv28_15_, pv28_26_,
    pv88_0_, pv120_23_, pv126_1_, pv144_12_, pv168_14_, pv28_16_, pv28_25_,
    pv88_1_, pv120_24_, pv120_31_, pv126_2_, pv168_13_, pv28_13_, pv28_24_,
    pv56_16_, pv56_27_, pv120_25_, pv144_14_, pv180_14_, pv192_2_,
    pv28_14_, pv28_23_, pv56_17_, pv56_26_, pv120_19_, pv120_26_, pv126_0_,
    pv144_0_, pv144_13_, pv180_8_, pv180_13_, pv192_1_, pv28_11_, pv28_22_,
    pv56_18_, pv120_27_, pv144_1_, pv168_10_, pv180_9_, pv183_0_, pv28_12_,
    pv28_21_, pv56_19_, pv120_28_, pv144_2_, pv183_1_,
    pv198_3_, pv198_0_, pv198_2_, pv198_1_, pv194_0_, pv194_1_  );
  input  pv28_20_, pv56_12_, pv56_23_, pv88_6_, pv88_19_, pv120_16_,
    pv120_29_, pv168_9_, pv180_10_, pv28_10_, pv56_13_, pv56_22_, pv88_7_,
    pv88_29_, pv120_15_, pv180_4_, pv56_14_, pv56_25_, pv88_8_, pv88_17_,
    pv88_28_, pv120_18_, pv180_5_, pv180_12_, pv192_0_, pv56_15_, pv56_24_,
    pv88_9_, pv88_18_, pv88_27_, pv120_17_, pv168_8_, pv180_6_, pv88_2_,
    pv88_15_, pv88_26_, pv120_12_, pv88_3_, pv88_16_, pv88_25_, pv120_11_,
    pv180_0_, pv56_10_, pv56_21_, pv88_4_, pv88_13_, pv88_24_, pv120_14_,
    pv126_5_, pv180_1_, pv56_11_, pv56_20_, pv88_5_, pv88_14_, pv88_23_,
    pv120_13_, pv180_2_, pv28_8_, pv56_5_, pv88_11_, pv88_22_, pv120_5_,
    pv132_0_, pv156_6_, pv168_1_, pv189_0_, pv28_9_, pv56_4_, pv88_12_,
    pv88_21_, pv120_6_, pv156_5_, pv156_10_, pv168_2_, pv189_1_, pv28_6_,
    pv56_7_, pv88_20_, pv120_3_, pv120_10_, pv156_8_, pv189_2_, pv28_7_,
    pv56_6_, pv88_10_, pv120_4_, pv168_0_, pv28_4_, pv56_9_, pv120_1_,
    pv168_5_, pv28_5_, pv56_8_, pv120_2_, pv156_9_, pv156_14_, pv168_6_,
    pv28_2_, pv156_13_, pv28_3_, pv120_0_, pv156_12_, pv168_4_, pv28_0_,
    pv186_1_, pv28_1_, pv144_4_, pv186_2_, pv144_5_, pv156_0_, pv132_5_,
    pv144_6_, pv56_1_, pv120_9_, pv120_30_, pv132_4_, pv156_2_, pv56_0_,
    pv88_30_, pv132_3_, pv144_8_, pv156_1_, pv28_19_, pv56_3_, pv88_31_,
    pv120_7_, pv132_2_, pv144_9_, pv156_4_, pv56_2_, pv120_8_, pv120_20_,
    pv132_1_, pv186_0_, pv28_17_, pv120_21_, pv126_3_, pv144_10_,
    pv168_12_, pv183_2_, pv28_18_, pv28_27_, pv120_22_, pv126_4_, pv28_15_,
    pv28_26_, pv88_0_, pv120_23_, pv126_1_, pv144_12_, pv168_14_, pv28_16_,
    pv28_25_, pv88_1_, pv120_24_, pv120_31_, pv126_2_, pv168_13_, pv28_13_,
    pv28_24_, pv56_16_, pv56_27_, pv120_25_, pv144_14_, pv180_14_,
    pv192_2_, pv28_14_, pv28_23_, pv56_17_, pv56_26_, pv120_19_, pv120_26_,
    pv126_0_, pv144_0_, pv144_13_, pv180_8_, pv180_13_, pv192_1_, pv28_11_,
    pv28_22_, pv56_18_, pv120_27_, pv144_1_, pv168_10_, pv180_9_, pv183_0_,
    pv28_12_, pv28_21_, pv56_19_, pv120_28_, pv144_2_, pv183_1_;
  output pv198_3_, pv198_0_, pv198_2_, pv198_1_, pv194_0_, pv194_1_;
  wire new_n199, new_n200, new_n201, new_n202, new_n203, new_n204, new_n205,
    new_n206, new_n207, new_n208, new_n209, new_n210, new_n211, new_n212,
    new_n213, new_n214, new_n215, new_n216, new_n217, new_n218, new_n219,
    new_n220, new_n221, new_n222, new_n223, new_n224, new_n225, new_n226,
    new_n227, new_n228, new_n229, new_n230, new_n231, new_n232, new_n233,
    new_n234, new_n235, new_n236, new_n237, new_n238, new_n239, new_n240,
    new_n241, new_n242, new_n243, new_n244, new_n245, new_n246, new_n247,
    new_n248, new_n249, new_n250, new_n251, new_n252, new_n253, new_n254,
    new_n255, new_n256, new_n257, new_n258, new_n260, new_n261, new_n262,
    new_n263, new_n264, new_n265, new_n266, new_n267, new_n268, new_n269,
    new_n270, new_n271, new_n272, new_n273, new_n274, new_n275, new_n276,
    new_n277, new_n278, new_n279, new_n280, new_n281, new_n282, new_n283,
    new_n284, new_n285, new_n286, new_n287, new_n288, new_n289, new_n290,
    new_n291, new_n292, new_n293, new_n294, new_n295, new_n296, new_n297,
    new_n298, new_n299, new_n300, new_n301, new_n302, new_n303, new_n304,
    new_n305, new_n306, new_n307, new_n308, new_n309, new_n310, new_n311,
    new_n312, new_n313, new_n314, new_n315, new_n316, new_n317, new_n318,
    new_n319, new_n321, new_n322, new_n323, new_n324, new_n325, new_n326,
    new_n327, new_n328, new_n329, new_n330, new_n331, new_n332, new_n333,
    new_n334, new_n335, new_n336, new_n337, new_n338, new_n339, new_n340,
    new_n341, new_n342, new_n343, new_n344, new_n345, new_n346, new_n347,
    new_n348, new_n349, new_n350, new_n351, new_n352, new_n353, new_n354,
    new_n355, new_n356, new_n357, new_n358, new_n359, new_n360, new_n361,
    new_n362, new_n363, new_n364, new_n365, new_n366, new_n367, new_n368,
    new_n369, new_n370, new_n371, new_n372, new_n373, new_n374, new_n375,
    new_n376, new_n377, new_n378, new_n379, new_n380, new_n382, new_n383,
    new_n384, new_n385, new_n386, new_n387, new_n388, new_n389, new_n390,
    new_n391, new_n392, new_n393, new_n394, new_n395, new_n396, new_n397,
    new_n398, new_n399, new_n400, new_n401, new_n402, new_n403, new_n404,
    new_n405, new_n406, new_n407, new_n408, new_n409, new_n410, new_n411,
    new_n412, new_n413, new_n414, new_n415, new_n416, new_n417, new_n418,
    new_n419, new_n420, new_n421, new_n422, new_n423, new_n424, new_n425,
    new_n426, new_n427, new_n428, new_n429, new_n430, new_n431, new_n432,
    new_n433, new_n434, new_n435, new_n436, new_n437, new_n438, new_n439,
    new_n440, new_n441;
  assign new_n199 = pv88_22_ & pv120_22_;
  assign new_n200 = pv180_0_ & pv120_24_;
  assign new_n201 = pv88_24_ & new_n200;
  assign new_n202 = pv180_1_ & new_n201;
  assign new_n203 = pv180_1_ & pv120_25_;
  assign new_n204 = pv180_0_ & new_n203;
  assign new_n205 = pv88_25_ & new_n204;
  assign new_n206 = pv180_2_ & new_n205;
  assign new_n207 = pv88_23_ & pv120_23_;
  assign new_n208 = pv180_0_ & new_n207;
  assign new_n209 = pv132_1_ & pv180_9_;
  assign new_n210 = pv180_8_ & new_n209;
  assign new_n211 = pv126_1_ & new_n210;
  assign new_n212 = pv180_10_ & new_n211;
  assign new_n213 = pv120_30_ & pv88_30_;
  assign new_n214 = pv88_31_ & pv120_31_;
  assign new_n215 = pv180_8_ & new_n214;
  assign new_n216 = pv132_0_ & pv180_8_;
  assign new_n217 = pv126_0_ & new_n216;
  assign new_n218 = pv180_9_ & new_n217;
  assign new_n219 = ~new_n215 & ~new_n218;
  assign new_n220 = ~new_n212 & ~new_n213;
  assign new_n221 = new_n219 & new_n220;
  assign new_n222 = pv192_0_ & ~new_n221;
  assign new_n223 = pv192_1_ & new_n222;
  assign new_n224 = pv120_29_ & pv180_5_;
  assign new_n225 = pv180_4_ & new_n224;
  assign new_n226 = pv88_29_ & new_n225;
  assign new_n227 = pv180_6_ & new_n226;
  assign new_n228 = pv88_26_ & pv120_26_;
  assign new_n229 = pv88_27_ & pv120_27_;
  assign new_n230 = pv180_4_ & new_n229;
  assign new_n231 = pv180_4_ & pv120_28_;
  assign new_n232 = pv88_28_ & new_n231;
  assign new_n233 = pv180_5_ & new_n232;
  assign new_n234 = ~new_n230 & ~new_n233;
  assign new_n235 = ~new_n227 & ~new_n228;
  assign new_n236 = new_n234 & new_n235;
  assign new_n237 = pv192_0_ & ~new_n236;
  assign new_n238 = pv132_5_ & pv180_13_;
  assign new_n239 = pv180_12_ & new_n238;
  assign new_n240 = pv126_5_ & new_n239;
  assign new_n241 = pv180_14_ & new_n240;
  assign new_n242 = pv132_2_ & pv126_2_;
  assign new_n243 = pv132_3_ & pv126_3_;
  assign new_n244 = pv180_12_ & new_n243;
  assign new_n245 = pv180_12_ & pv132_4_;
  assign new_n246 = pv126_4_ & new_n245;
  assign new_n247 = pv180_13_ & new_n246;
  assign new_n248 = ~new_n244 & ~new_n247;
  assign new_n249 = ~new_n241 & ~new_n242;
  assign new_n250 = new_n248 & new_n249;
  assign new_n251 = pv192_0_ & pv192_1_;
  assign new_n252 = ~new_n250 & new_n251;
  assign new_n253 = pv192_2_ & new_n252;
  assign new_n254 = ~new_n223 & ~new_n237;
  assign new_n255 = ~new_n253 & new_n254;
  assign new_n256 = ~new_n206 & ~new_n208;
  assign new_n257 = ~new_n199 & ~new_n202;
  assign new_n258 = new_n256 & new_n257;
  assign pv198_3_ = ~new_n255 | ~new_n258;
  assign new_n260 = pv28_2_ & pv56_2_;
  assign new_n261 = pv56_4_ & pv144_0_;
  assign new_n262 = pv28_4_ & new_n261;
  assign new_n263 = pv144_1_ & new_n262;
  assign new_n264 = pv56_5_ & pv144_1_;
  assign new_n265 = pv144_0_ & new_n264;
  assign new_n266 = pv28_5_ & new_n265;
  assign new_n267 = pv144_2_ & new_n266;
  assign new_n268 = pv28_3_ & pv56_3_;
  assign new_n269 = pv144_0_ & new_n268;
  assign new_n270 = pv56_13_ & pv144_9_;
  assign new_n271 = pv144_8_ & new_n270;
  assign new_n272 = pv28_13_ & new_n271;
  assign new_n273 = pv144_10_ & new_n272;
  assign new_n274 = pv28_10_ & pv56_10_;
  assign new_n275 = pv56_11_ & pv28_11_;
  assign new_n276 = pv144_8_ & new_n275;
  assign new_n277 = pv56_12_ & pv144_8_;
  assign new_n278 = pv28_12_ & new_n277;
  assign new_n279 = pv144_9_ & new_n278;
  assign new_n280 = ~new_n276 & ~new_n279;
  assign new_n281 = ~new_n273 & ~new_n274;
  assign new_n282 = new_n280 & new_n281;
  assign new_n283 = pv183_0_ & ~new_n282;
  assign new_n284 = pv183_1_ & new_n283;
  assign new_n285 = pv56_9_ & pv144_5_;
  assign new_n286 = pv144_4_ & new_n285;
  assign new_n287 = pv28_9_ & new_n286;
  assign new_n288 = pv144_6_ & new_n287;
  assign new_n289 = pv28_6_ & pv56_6_;
  assign new_n290 = pv56_7_ & pv28_7_;
  assign new_n291 = pv144_4_ & new_n290;
  assign new_n292 = pv56_8_ & pv144_4_;
  assign new_n293 = pv28_8_ & new_n292;
  assign new_n294 = pv144_5_ & new_n293;
  assign new_n295 = ~new_n291 & ~new_n294;
  assign new_n296 = ~new_n288 & ~new_n289;
  assign new_n297 = new_n295 & new_n296;
  assign new_n298 = pv183_0_ & ~new_n297;
  assign new_n299 = pv56_17_ & pv144_13_;
  assign new_n300 = pv144_12_ & new_n299;
  assign new_n301 = pv28_17_ & new_n300;
  assign new_n302 = pv144_14_ & new_n301;
  assign new_n303 = pv56_14_ & pv28_14_;
  assign new_n304 = pv56_15_ & pv28_15_;
  assign new_n305 = pv144_12_ & new_n304;
  assign new_n306 = pv144_12_ & pv56_16_;
  assign new_n307 = pv28_16_ & new_n306;
  assign new_n308 = pv144_13_ & new_n307;
  assign new_n309 = ~new_n305 & ~new_n308;
  assign new_n310 = ~new_n302 & ~new_n303;
  assign new_n311 = new_n309 & new_n310;
  assign new_n312 = pv183_0_ & pv183_1_;
  assign new_n313 = ~new_n311 & new_n312;
  assign new_n314 = pv183_2_ & new_n313;
  assign new_n315 = ~new_n284 & ~new_n298;
  assign new_n316 = ~new_n314 & new_n315;
  assign new_n317 = ~new_n267 & ~new_n269;
  assign new_n318 = ~new_n260 & ~new_n263;
  assign new_n319 = new_n317 & new_n318;
  assign pv198_0_ = ~new_n316 | ~new_n319;
  assign new_n321 = pv88_6_ & pv120_6_;
  assign new_n322 = pv168_0_ & pv120_8_;
  assign new_n323 = pv88_8_ & new_n322;
  assign new_n324 = pv168_1_ & new_n323;
  assign new_n325 = pv168_1_ & pv120_9_;
  assign new_n326 = pv168_0_ & new_n325;
  assign new_n327 = pv88_9_ & new_n326;
  assign new_n328 = pv168_2_ & new_n327;
  assign new_n329 = pv88_7_ & pv120_7_;
  assign new_n330 = pv168_0_ & new_n329;
  assign new_n331 = pv168_9_ & pv120_17_;
  assign new_n332 = pv168_8_ & new_n331;
  assign new_n333 = pv88_17_ & new_n332;
  assign new_n334 = pv168_10_ & new_n333;
  assign new_n335 = pv120_14_ & pv88_14_;
  assign new_n336 = pv120_15_ & pv88_15_;
  assign new_n337 = pv168_8_ & new_n336;
  assign new_n338 = pv120_16_ & pv168_8_;
  assign new_n339 = pv88_16_ & new_n338;
  assign new_n340 = pv168_9_ & new_n339;
  assign new_n341 = ~new_n337 & ~new_n340;
  assign new_n342 = ~new_n334 & ~new_n335;
  assign new_n343 = new_n341 & new_n342;
  assign new_n344 = pv189_0_ & ~new_n343;
  assign new_n345 = pv189_1_ & new_n344;
  assign new_n346 = pv120_13_ & pv168_5_;
  assign new_n347 = pv168_4_ & new_n346;
  assign new_n348 = pv88_13_ & new_n347;
  assign new_n349 = pv168_6_ & new_n348;
  assign new_n350 = pv120_10_ & pv88_10_;
  assign new_n351 = pv120_11_ & pv88_11_;
  assign new_n352 = pv168_4_ & new_n351;
  assign new_n353 = pv120_12_ & pv168_4_;
  assign new_n354 = pv88_12_ & new_n353;
  assign new_n355 = pv168_5_ & new_n354;
  assign new_n356 = ~new_n352 & ~new_n355;
  assign new_n357 = ~new_n349 & ~new_n350;
  assign new_n358 = new_n356 & new_n357;
  assign new_n359 = pv189_0_ & ~new_n358;
  assign new_n360 = pv120_21_ & pv168_13_;
  assign new_n361 = pv168_12_ & new_n360;
  assign new_n362 = pv88_21_ & new_n361;
  assign new_n363 = pv168_14_ & new_n362;
  assign new_n364 = pv120_18_ & pv88_18_;
  assign new_n365 = pv88_19_ & pv120_19_;
  assign new_n366 = pv168_12_ & new_n365;
  assign new_n367 = pv120_20_ & pv168_12_;
  assign new_n368 = pv88_20_ & new_n367;
  assign new_n369 = pv168_13_ & new_n368;
  assign new_n370 = ~new_n366 & ~new_n369;
  assign new_n371 = ~new_n363 & ~new_n364;
  assign new_n372 = new_n370 & new_n371;
  assign new_n373 = pv189_0_ & pv189_1_;
  assign new_n374 = ~new_n372 & new_n373;
  assign new_n375 = pv189_2_ & new_n374;
  assign new_n376 = ~new_n345 & ~new_n359;
  assign new_n377 = ~new_n375 & new_n376;
  assign new_n378 = ~new_n328 & ~new_n330;
  assign new_n379 = ~new_n321 & ~new_n324;
  assign new_n380 = new_n378 & new_n379;
  assign pv198_2_ = ~new_n377 | ~new_n380;
  assign new_n382 = pv28_18_ & pv56_18_;
  assign new_n383 = pv56_20_ & pv156_0_;
  assign new_n384 = pv28_20_ & new_n383;
  assign new_n385 = pv156_1_ & new_n384;
  assign new_n386 = pv56_21_ & pv156_1_;
  assign new_n387 = pv156_0_ & new_n386;
  assign new_n388 = pv28_21_ & new_n387;
  assign new_n389 = pv156_2_ & new_n388;
  assign new_n390 = pv28_19_ & pv56_19_;
  assign new_n391 = pv156_0_ & new_n390;
  assign new_n392 = pv120_1_ & pv156_9_;
  assign new_n393 = pv156_8_ & new_n392;
  assign new_n394 = pv88_1_ & new_n393;
  assign new_n395 = pv156_10_ & new_n394;
  assign new_n396 = pv28_26_ & pv56_26_;
  assign new_n397 = pv28_27_ & pv56_27_;
  assign new_n398 = pv156_8_ & new_n397;
  assign new_n399 = pv156_8_ & pv120_0_;
  assign new_n400 = pv88_0_ & new_n399;
  assign new_n401 = pv156_9_ & new_n400;
  assign new_n402 = ~new_n398 & ~new_n401;
  assign new_n403 = ~new_n395 & ~new_n396;
  assign new_n404 = new_n402 & new_n403;
  assign new_n405 = pv186_0_ & ~new_n404;
  assign new_n406 = pv186_1_ & new_n405;
  assign new_n407 = pv56_25_ & pv156_5_;
  assign new_n408 = pv156_4_ & new_n407;
  assign new_n409 = pv28_25_ & new_n408;
  assign new_n410 = pv156_6_ & new_n409;
  assign new_n411 = pv56_22_ & pv28_22_;
  assign new_n412 = pv56_23_ & pv28_23_;
  assign new_n413 = pv156_4_ & new_n412;
  assign new_n414 = pv56_24_ & pv156_4_;
  assign new_n415 = pv28_24_ & new_n414;
  assign new_n416 = pv156_5_ & new_n415;
  assign new_n417 = ~new_n413 & ~new_n416;
  assign new_n418 = ~new_n410 & ~new_n411;
  assign new_n419 = new_n417 & new_n418;
  assign new_n420 = pv186_0_ & ~new_n419;
  assign new_n421 = pv120_5_ & pv156_13_;
  assign new_n422 = pv156_12_ & new_n421;
  assign new_n423 = pv88_5_ & new_n422;
  assign new_n424 = pv156_14_ & new_n423;
  assign new_n425 = pv88_2_ & pv120_2_;
  assign new_n426 = pv88_3_ & pv120_3_;
  assign new_n427 = pv156_12_ & new_n426;
  assign new_n428 = pv120_4_ & pv156_12_;
  assign new_n429 = pv88_4_ & new_n428;
  assign new_n430 = pv156_13_ & new_n429;
  assign new_n431 = ~new_n427 & ~new_n430;
  assign new_n432 = ~new_n424 & ~new_n425;
  assign new_n433 = new_n431 & new_n432;
  assign new_n434 = pv186_1_ & pv186_0_;
  assign new_n435 = ~new_n433 & new_n434;
  assign new_n436 = pv186_2_ & new_n435;
  assign new_n437 = ~new_n406 & ~new_n420;
  assign new_n438 = ~new_n436 & new_n437;
  assign new_n439 = ~new_n389 & ~new_n391;
  assign new_n440 = ~new_n382 & ~new_n385;
  assign new_n441 = new_n439 & new_n440;
  assign pv198_1_ = ~new_n438 | ~new_n441;
  assign pv194_0_ = pv28_0_ & pv56_0_;
  assign pv194_1_ = pv28_1_ & pv56_1_;
endmodule


