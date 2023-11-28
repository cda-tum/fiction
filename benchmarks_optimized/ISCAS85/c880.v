// Benchmark "top" written by ABC on Mon Nov 27 16:56:17 2023

module top ( 
    \1 , \8 , \13 , \17 , \26 , \29 , \36 , \42 , \51 , \55 , \59 , \68 ,
    \72 , \73 , \74 , \75 , \80 , \85 , \86 , \87 , \88 , \89 , \90 , \91 ,
    \96 , \101 , \106 , \111 , \116 , \121 , \126 , \130 , \135 , \138 ,
    \143 , \146 , \149 , \152 , \153 , \156 , \159 , \165 , \171 , \177 ,
    \183 , \189 , \195 , \201 , \207 , \210 , \219 , \228 , \237 , \246 ,
    \255 , \259 , \260 , \261 , \267 , \268 ,
    \388 , \389 , \390 , \391 , \418 , \419 , \420 , \421 , \422 , \423 ,
    \446 , \447 , \448 , \449 , \450 , \767 , \768 , \850 , \863 , \864 ,
    \865 , \866 , \874 , \878 , \879 , \880   );
  input  \1 , \8 , \13 , \17 , \26 , \29 , \36 , \42 , \51 , \55 , \59 ,
    \68 , \72 , \73 , \74 , \75 , \80 , \85 , \86 , \87 , \88 , \89 , \90 ,
    \91 , \96 , \101 , \106 , \111 , \116 , \121 , \126 , \130 , \135 ,
    \138 , \143 , \146 , \149 , \152 , \153 , \156 , \159 , \165 , \171 ,
    \177 , \183 , \189 , \195 , \201 , \207 , \210 , \219 , \228 , \237 ,
    \246 , \255 , \259 , \260 , \261 , \267 , \268 ;
  output \388 , \389 , \390 , \391 , \418 , \419 , \420 , \421 , \422 , \423 ,
    \446 , \447 , \448 , \449 , \450 , \767 , \768 , \850 , \863 , \864 ,
    \865 , \866 , \874 , \878 , \879 , \880 ;
  wire new_n87, new_n89, new_n93, new_n94, new_n96, new_n97, new_n99,
    new_n102, new_n104, new_n107, new_n109, new_n110, new_n111, new_n113,
    new_n116, new_n117, new_n118, new_n119, new_n120, new_n121, new_n122,
    new_n123, new_n124, new_n125, new_n126, new_n127, new_n128, new_n129,
    new_n130, new_n131, new_n132, new_n133, new_n134, new_n135, new_n136,
    new_n137, new_n138, new_n139, new_n140, new_n141, new_n143, new_n144,
    new_n145, new_n146, new_n147, new_n148, new_n149, new_n150, new_n151,
    new_n152, new_n153, new_n154, new_n155, new_n156, new_n157, new_n158,
    new_n159, new_n160, new_n161, new_n162, new_n163, new_n164, new_n165,
    new_n166, new_n167, new_n168, new_n170, new_n171, new_n172, new_n173,
    new_n174, new_n175, new_n176, new_n177, new_n178, new_n179, new_n180,
    new_n181, new_n182, new_n183, new_n184, new_n185, new_n186, new_n187,
    new_n188, new_n189, new_n190, new_n191, new_n192, new_n193, new_n194,
    new_n195, new_n196, new_n197, new_n198, new_n199, new_n200, new_n201,
    new_n202, new_n203, new_n204, new_n205, new_n206, new_n207, new_n208,
    new_n209, new_n210, new_n211, new_n212, new_n213, new_n214, new_n215,
    new_n216, new_n217, new_n218, new_n219, new_n221, new_n222, new_n223,
    new_n224, new_n225, new_n226, new_n227, new_n228, new_n229, new_n230,
    new_n231, new_n232, new_n233, new_n234, new_n235, new_n236, new_n237,
    new_n238, new_n239, new_n240, new_n241, new_n242, new_n243, new_n244,
    new_n245, new_n246, new_n247, new_n248, new_n249, new_n250, new_n251,
    new_n252, new_n253, new_n254, new_n255, new_n256, new_n258, new_n259,
    new_n260, new_n261, new_n262, new_n263, new_n264, new_n265, new_n266,
    new_n267, new_n268, new_n269, new_n270, new_n272, new_n273, new_n274,
    new_n275, new_n276, new_n277, new_n278, new_n279, new_n280, new_n281,
    new_n282, new_n283, new_n284, new_n285, new_n286, new_n288, new_n289,
    new_n290, new_n291, new_n292, new_n293, new_n294, new_n295, new_n296,
    new_n297, new_n298, new_n299, new_n300, new_n301, new_n302, new_n303,
    new_n304, new_n305, new_n306, new_n307, new_n308, new_n309, new_n310,
    new_n311, new_n312, new_n313, new_n314, new_n315, new_n316, new_n317,
    new_n318, new_n319, new_n320, new_n321, new_n322, new_n323, new_n324,
    new_n325, new_n326, new_n327, new_n328, new_n329, new_n330, new_n332,
    new_n333, new_n334, new_n335, new_n336, new_n337, new_n338, new_n339,
    new_n340, new_n341, new_n342, new_n343, new_n344, new_n345, new_n347,
    new_n348, new_n349, new_n350, new_n351, new_n352, new_n353, new_n354,
    new_n355, new_n356, new_n357, new_n359, new_n360, new_n361, new_n362,
    new_n363, new_n364, new_n365, new_n366, new_n367, new_n368, new_n369,
    new_n370, new_n372, new_n373, new_n374, new_n375, new_n376, new_n377,
    new_n378, new_n379, new_n380, new_n381, new_n382, new_n383;
  assign new_n87 = \29  & \42 ;
  assign \388  = \75  & new_n87;
  assign new_n89 = \29  & \80 ;
  assign \389  = \36  & new_n89;
  assign \390  = \36  & new_n87;
  assign \391  = \85  & \86 ;
  assign new_n93 = \1  & \13 ;
  assign new_n94 = \8  & \17 ;
  assign \418  = new_n93 & new_n94;
  assign new_n96 = \17  & \26 ;
  assign new_n97 = new_n93 & new_n96;
  assign \419  = \390  | ~new_n97;
  assign new_n99 = \59  & \80 ;
  assign \420  = ~\75  | ~new_n99;
  assign \421  = ~\36  | ~new_n99;
  assign new_n102 = \42  & \59 ;
  assign \422  = ~\36  | ~new_n102;
  assign new_n104 = ~\87  & ~\88 ;
  assign \423  = \90  & ~new_n104;
  assign \446  = ~\390  | ~new_n97;
  assign new_n107 = \1  & \51 ;
  assign \447  = \26  & new_n107;
  assign new_n109 = \8  & \55 ;
  assign new_n110 = \68  & new_n109;
  assign new_n111 = new_n93 & new_n110;
  assign \448  = \29  & new_n111;
  assign new_n113 = \59  & \74 ;
  assign \449  = new_n111 & new_n113;
  assign \450  = \89  & ~new_n104;
  assign new_n116 = ~\121  & ~\126 ;
  assign new_n117 = \121  & \126 ;
  assign new_n118 = ~new_n116 & ~new_n117;
  assign new_n119 = \130  & ~\135 ;
  assign new_n120 = ~\130  & \135 ;
  assign new_n121 = ~new_n119 & ~new_n120;
  assign new_n122 = new_n118 & ~new_n121;
  assign new_n123 = ~new_n118 & new_n121;
  assign new_n124 = ~new_n122 & ~new_n123;
  assign new_n125 = ~\91  & ~\96 ;
  assign new_n126 = \91  & \96 ;
  assign new_n127 = ~new_n125 & ~new_n126;
  assign new_n128 = ~\111  & ~\116 ;
  assign new_n129 = \111  & \116 ;
  assign new_n130 = ~new_n128 & ~new_n129;
  assign new_n131 = ~\101  & ~\106 ;
  assign new_n132 = \101  & \106 ;
  assign new_n133 = ~new_n131 & ~new_n132;
  assign new_n134 = new_n130 & ~new_n133;
  assign new_n135 = ~new_n130 & new_n133;
  assign new_n136 = ~new_n134 & ~new_n135;
  assign new_n137 = new_n127 & ~new_n136;
  assign new_n138 = ~new_n127 & new_n136;
  assign new_n139 = ~new_n137 & ~new_n138;
  assign new_n140 = new_n124 & new_n139;
  assign new_n141 = ~new_n124 & ~new_n139;
  assign \767  = ~new_n140 & ~new_n141;
  assign new_n143 = \159  & ~\183 ;
  assign new_n144 = ~\159  & \183 ;
  assign new_n145 = ~new_n143 & ~new_n144;
  assign new_n146 = ~\130  & ~\177 ;
  assign new_n147 = \130  & \177 ;
  assign new_n148 = ~new_n146 & ~new_n147;
  assign new_n149 = \201  & new_n148;
  assign new_n150 = ~\201  & ~new_n148;
  assign new_n151 = ~new_n149 & ~new_n150;
  assign new_n152 = ~\171  & ~\207 ;
  assign new_n153 = \171  & \207 ;
  assign new_n154 = ~new_n152 & ~new_n153;
  assign new_n155 = \165  & new_n154;
  assign new_n156 = ~\165  & ~new_n154;
  assign new_n157 = ~new_n155 & ~new_n156;
  assign new_n158 = \189  & ~\195 ;
  assign new_n159 = ~\189  & \195 ;
  assign new_n160 = ~new_n158 & ~new_n159;
  assign new_n161 = new_n157 & new_n160;
  assign new_n162 = ~new_n157 & ~new_n160;
  assign new_n163 = ~new_n161 & ~new_n162;
  assign new_n164 = new_n151 & ~new_n163;
  assign new_n165 = ~new_n151 & new_n163;
  assign new_n166 = ~new_n164 & ~new_n165;
  assign new_n167 = new_n145 & new_n166;
  assign new_n168 = ~new_n145 & ~new_n166;
  assign \768  = new_n167 | new_n168;
  assign new_n170 = \121  & \210 ;
  assign new_n171 = \255  & \267 ;
  assign new_n172 = \72  & \73 ;
  assign new_n173 = new_n102 & new_n172;
  assign new_n174 = new_n111 & new_n173;
  assign new_n175 = ~\228  & ~new_n174;
  assign new_n176 = \219  & ~\261 ;
  assign new_n177 = \201  & ~new_n176;
  assign new_n178 = new_n175 & new_n177;
  assign new_n179 = \55  & \447 ;
  assign new_n180 = \75  & ~\268 ;
  assign new_n181 = new_n89 & new_n180;
  assign new_n182 = new_n179 & new_n181;
  assign new_n183 = \59  & \156 ;
  assign new_n184 = \17  & \447 ;
  assign new_n185 = ~new_n183 & new_n184;
  assign new_n186 = \1  & ~new_n185;
  assign new_n187 = \153  & ~new_n186;
  assign new_n188 = \75  & new_n102;
  assign new_n189 = new_n94 & new_n107;
  assign new_n190 = ~new_n188 & new_n189;
  assign new_n191 = \17  & ~\42 ;
  assign new_n192 = ~\17  & \42 ;
  assign new_n193 = ~new_n191 & ~new_n192;
  assign new_n194 = \447  & new_n183;
  assign new_n195 = ~new_n193 & new_n194;
  assign new_n196 = ~new_n190 & ~new_n195;
  assign new_n197 = \126  & ~new_n196;
  assign new_n198 = ~new_n182 & ~new_n187;
  assign new_n199 = ~new_n197 & new_n198;
  assign new_n200 = \219  & \261 ;
  assign new_n201 = ~\201  & ~new_n200;
  assign new_n202 = ~new_n178 & ~new_n201;
  assign new_n203 = new_n199 & new_n202;
  assign new_n204 = ~\237  & ~\246 ;
  assign new_n205 = ~new_n174 & new_n204;
  assign new_n206 = \201  & ~new_n200;
  assign new_n207 = new_n205 & new_n206;
  assign new_n208 = ~new_n199 & ~new_n207;
  assign new_n209 = ~new_n203 & ~new_n208;
  assign new_n210 = ~\201  & ~\261 ;
  assign new_n211 = ~new_n199 & ~new_n210;
  assign new_n212 = \201  & \261 ;
  assign new_n213 = ~new_n211 & ~new_n212;
  assign new_n214 = \219  & new_n213;
  assign new_n215 = ~\228  & ~\246 ;
  assign new_n216 = ~\201  & new_n215;
  assign new_n217 = ~new_n214 & new_n216;
  assign new_n218 = ~new_n209 & ~new_n217;
  assign new_n219 = ~new_n170 & ~new_n171;
  assign \850  = new_n218 | ~new_n219;
  assign new_n221 = \106  & \210 ;
  assign new_n222 = \111  & ~new_n196;
  assign new_n223 = \143  & ~new_n186;
  assign new_n224 = ~new_n182 & ~new_n222;
  assign new_n225 = ~new_n223 & new_n224;
  assign new_n226 = \183  & ~new_n225;
  assign new_n227 = new_n205 & new_n226;
  assign new_n228 = ~new_n215 & ~new_n225;
  assign new_n229 = ~\183  & ~new_n228;
  assign new_n230 = new_n175 & new_n225;
  assign new_n231 = ~new_n227 & ~new_n230;
  assign new_n232 = ~new_n229 & new_n231;
  assign new_n233 = \116  & ~new_n196;
  assign new_n234 = \146  & ~new_n186;
  assign new_n235 = ~new_n182 & ~new_n233;
  assign new_n236 = ~new_n234 & new_n235;
  assign new_n237 = \189  & ~new_n236;
  assign new_n238 = ~\189  & new_n236;
  assign new_n239 = ~new_n237 & ~new_n238;
  assign new_n240 = \121  & ~new_n196;
  assign new_n241 = \149  & ~new_n186;
  assign new_n242 = ~new_n182 & ~new_n240;
  assign new_n243 = ~new_n241 & new_n242;
  assign new_n244 = \195  & ~new_n243;
  assign new_n245 = ~\195  & new_n243;
  assign new_n246 = ~new_n213 & ~new_n245;
  assign new_n247 = ~new_n244 & ~new_n246;
  assign new_n248 = new_n239 & ~new_n247;
  assign new_n249 = ~new_n237 & ~new_n248;
  assign new_n250 = ~\183  & new_n225;
  assign new_n251 = ~new_n226 & ~new_n250;
  assign new_n252 = new_n249 & ~new_n251;
  assign new_n253 = ~new_n249 & new_n251;
  assign new_n254 = \219  & ~new_n252;
  assign new_n255 = ~new_n253 & new_n254;
  assign new_n256 = ~new_n221 & ~new_n232;
  assign \863  = new_n255 | ~new_n256;
  assign new_n258 = \111  & \210 ;
  assign new_n259 = ~new_n239 & new_n247;
  assign new_n260 = \219  & ~new_n248;
  assign new_n261 = ~new_n259 & new_n260;
  assign new_n262 = ~\189  & new_n215;
  assign new_n263 = \189  & new_n175;
  assign new_n264 = ~new_n262 & ~new_n263;
  assign new_n265 = new_n239 & new_n264;
  assign new_n266 = \255  & \259 ;
  assign new_n267 = ~new_n205 & new_n237;
  assign new_n268 = ~new_n258 & ~new_n266;
  assign new_n269 = ~new_n267 & new_n268;
  assign new_n270 = ~new_n265 & new_n269;
  assign \864  = new_n261 | ~new_n270;
  assign new_n272 = \116  & \210 ;
  assign new_n273 = ~new_n244 & ~new_n245;
  assign new_n274 = ~\228  & ~new_n214;
  assign new_n275 = new_n273 & ~new_n274;
  assign new_n276 = \219  & ~new_n213;
  assign new_n277 = ~new_n273 & new_n276;
  assign new_n278 = \195  & \237 ;
  assign new_n279 = ~\246  & ~new_n278;
  assign new_n280 = ~new_n243 & ~new_n279;
  assign new_n281 = \255  & \260 ;
  assign new_n282 = \195  & new_n174;
  assign new_n283 = ~new_n272 & ~new_n281;
  assign new_n284 = ~new_n282 & new_n283;
  assign new_n285 = ~new_n280 & new_n284;
  assign new_n286 = ~new_n277 & new_n285;
  assign \865  = new_n275 | ~new_n286;
  assign new_n288 = new_n181 & new_n184;
  assign new_n289 = new_n179 & ~new_n183;
  assign new_n290 = \143  & new_n289;
  assign new_n291 = \8  & \138 ;
  assign new_n292 = \91  & ~new_n196;
  assign new_n293 = ~new_n288 & ~new_n291;
  assign new_n294 = ~new_n290 & new_n293;
  assign new_n295 = ~new_n292 & new_n294;
  assign new_n296 = ~\159  & new_n295;
  assign new_n297 = \159  & ~new_n295;
  assign new_n298 = ~new_n296 & ~new_n297;
  assign new_n299 = \146  & new_n289;
  assign new_n300 = ~\138  & ~new_n288;
  assign new_n301 = \51  & ~new_n300;
  assign new_n302 = \96  & ~new_n196;
  assign new_n303 = ~new_n299 & ~new_n301;
  assign new_n304 = ~new_n302 & new_n303;
  assign new_n305 = \165  & ~new_n304;
  assign new_n306 = ~\165  & new_n304;
  assign new_n307 = ~new_n305 & ~new_n306;
  assign new_n308 = \149  & new_n289;
  assign new_n309 = \17  & ~new_n300;
  assign new_n310 = \101  & ~new_n196;
  assign new_n311 = ~new_n308 & ~new_n309;
  assign new_n312 = ~new_n310 & new_n311;
  assign new_n313 = \171  & ~new_n312;
  assign new_n314 = ~\171  & new_n312;
  assign new_n315 = \106  & ~new_n196;
  assign new_n316 = \153  & new_n289;
  assign new_n317 = \138  & \152 ;
  assign new_n318 = ~new_n288 & ~new_n317;
  assign new_n319 = ~new_n316 & new_n318;
  assign new_n320 = ~new_n315 & new_n319;
  assign new_n321 = \177  & ~new_n320;
  assign new_n322 = ~\177  & new_n320;
  assign new_n323 = ~new_n226 & ~new_n253;
  assign new_n324 = ~new_n322 & ~new_n323;
  assign new_n325 = ~new_n321 & ~new_n324;
  assign new_n326 = ~new_n314 & ~new_n325;
  assign new_n327 = ~new_n313 & ~new_n326;
  assign new_n328 = new_n307 & ~new_n327;
  assign new_n329 = ~new_n305 & ~new_n328;
  assign new_n330 = new_n298 & new_n329;
  assign \866  = ~new_n296 & ~new_n330;
  assign new_n332 = \177  & new_n174;
  assign new_n333 = ~new_n321 & ~new_n322;
  assign new_n334 = \219  & new_n325;
  assign new_n335 = ~\228  & ~new_n334;
  assign new_n336 = new_n333 & ~new_n335;
  assign new_n337 = \219  & ~new_n333;
  assign new_n338 = ~new_n323 & new_n337;
  assign new_n339 = \177  & \237 ;
  assign new_n340 = ~\246  & ~new_n339;
  assign new_n341 = ~new_n320 & ~new_n340;
  assign new_n342 = \101  & \210 ;
  assign new_n343 = ~new_n332 & ~new_n342;
  assign new_n344 = ~new_n341 & new_n343;
  assign new_n345 = ~new_n338 & new_n344;
  assign \874  = new_n336 | ~new_n345;
  assign new_n347 = \210  & \268 ;
  assign new_n348 = ~new_n298 & ~new_n329;
  assign new_n349 = ~new_n330 & ~new_n348;
  assign new_n350 = \219  & ~new_n349;
  assign new_n351 = ~new_n205 & new_n297;
  assign new_n352 = ~\159  & new_n215;
  assign new_n353 = \159  & new_n175;
  assign new_n354 = ~new_n352 & ~new_n353;
  assign new_n355 = new_n298 & new_n354;
  assign new_n356 = ~new_n347 & ~new_n351;
  assign new_n357 = ~new_n355 & new_n356;
  assign \878  = new_n350 | ~new_n357;
  assign new_n359 = \165  & new_n174;
  assign new_n360 = ~new_n307 & new_n327;
  assign new_n361 = \219  & ~new_n328;
  assign new_n362 = ~new_n360 & new_n361;
  assign new_n363 = \165  & \237 ;
  assign new_n364 = ~\246  & ~new_n363;
  assign new_n365 = ~new_n304 & ~new_n364;
  assign new_n366 = \91  & \210 ;
  assign new_n367 = \228  & new_n307;
  assign new_n368 = ~new_n359 & ~new_n366;
  assign new_n369 = ~new_n365 & new_n368;
  assign new_n370 = ~new_n367 & new_n369;
  assign \879  = new_n362 | ~new_n370;
  assign new_n372 = \96  & \210 ;
  assign new_n373 = ~new_n313 & ~new_n314;
  assign new_n374 = ~new_n335 & new_n373;
  assign new_n375 = \219  & ~new_n373;
  assign new_n376 = ~new_n325 & new_n375;
  assign new_n377 = \171  & \237 ;
  assign new_n378 = ~\246  & ~new_n377;
  assign new_n379 = ~new_n312 & ~new_n378;
  assign new_n380 = \171  & new_n174;
  assign new_n381 = ~new_n372 & ~new_n380;
  assign new_n382 = ~new_n379 & new_n381;
  assign new_n383 = ~new_n376 & new_n382;
  assign \880  = new_n374 | ~new_n383;
endmodule


