// Benchmark "top" written by ABC on Mon Feb 19 11:52:41 2024

module top ( 
    pdel1, pratr, pbull1, pbull0, pwatch, povacc, pverr_n, porwd_n,
    pvlenesr, pmmerr, paccrpy, pvacc, pcat4, pvst1, pcapsd, pcat3, pcat2,
    pcat1, plsd, pvst0, pcat0, pibt2, pkbg_n, pmarssr, pend, pvsumesr,
    pbull5, ppluto4, pstar2, pbull4, ppluto5, ppy, pstar3, pbull3,
    pcomppar, pibt1, pstar0, pbull2, pibt0, pstar1, ppluto0, pfbi, powl_n,
    ppluto1, piclr, ppluto2, pbull6, pcat5, ppluto3,
    pbull2_p, pend_p, pbull3_p, porwd_f, pbull4_p, ppluto3_p, pwatch_p,
    pbull5_p, pbull6_p, ppluto5_p, ppy_p, ppluto4_p, plsd_p, pvlenesr_p,
    pfbi_p, pstar1_p, pvsumesr_p, paccrpy_p, pkbg_f, pmarssr_p, pstar0_p,
    pstar3_p, pdel1_p, pstar2_p, pvst0_p, pcomppar_p, powl_f, psdo,
    pvst1_p, ppluto1_p, pratr_p, ppluto0_p, povacc_p, ppluto2_p, pbull0_p,
    pverr_f, pbull1_p  );
  input  pdel1, pratr, pbull1, pbull0, pwatch, povacc, pverr_n, porwd_n,
    pvlenesr, pmmerr, paccrpy, pvacc, pcat4, pvst1, pcapsd, pcat3, pcat2,
    pcat1, plsd, pvst0, pcat0, pibt2, pkbg_n, pmarssr, pend, pvsumesr,
    pbull5, ppluto4, pstar2, pbull4, ppluto5, ppy, pstar3, pbull3,
    pcomppar, pibt1, pstar0, pbull2, pibt0, pstar1, ppluto0, pfbi, powl_n,
    ppluto1, piclr, ppluto2, pbull6, pcat5, ppluto3;
  output pbull2_p, pend_p, pbull3_p, porwd_f, pbull4_p, ppluto3_p, pwatch_p,
    pbull5_p, pbull6_p, ppluto5_p, ppy_p, ppluto4_p, plsd_p, pvlenesr_p,
    pfbi_p, pstar1_p, pvsumesr_p, paccrpy_p, pkbg_f, pmarssr_p, pstar0_p,
    pstar3_p, pdel1_p, pstar2_p, pvst0_p, pcomppar_p, powl_f, psdo,
    pvst1_p, ppluto1_p, pratr_p, ppluto0_p, povacc_p, ppluto2_p, pbull0_p,
    pverr_f, pbull1_p;
  wire new_n87, new_n88, new_n89, new_n90, new_n91, new_n92, new_n93,
    new_n94, new_n95, new_n96, new_n98, new_n99, new_n100, new_n101,
    new_n102, new_n103, new_n104, new_n105, new_n106, new_n107, new_n108,
    new_n109, new_n110, new_n111, new_n112, new_n113, new_n114, new_n115,
    new_n116, new_n117, new_n118, new_n119, new_n120, new_n121, new_n123,
    new_n124, new_n125, new_n126, new_n128, new_n129, new_n130, new_n131,
    new_n132, new_n133, new_n134, new_n136, new_n137, new_n138, new_n139,
    new_n140, new_n141, new_n142, new_n143, new_n145, new_n146, new_n147,
    new_n148, new_n149, new_n150, new_n151, new_n152, new_n153, new_n154,
    new_n155, new_n156, new_n157, new_n158, new_n159, new_n160, new_n161,
    new_n162, new_n163, new_n164, new_n166, new_n167, new_n169, new_n170,
    new_n171, new_n172, new_n173, new_n174, new_n175, new_n177, new_n178,
    new_n179, new_n180, new_n181, new_n182, new_n183, new_n184, new_n185,
    new_n186, new_n187, new_n188, new_n189, new_n190, new_n191, new_n192,
    new_n193, new_n194, new_n196, new_n197, new_n199, new_n200, new_n201,
    new_n202, new_n204, new_n205, new_n206, new_n207, new_n209, new_n210,
    new_n211, new_n212, new_n213, new_n214, new_n215, new_n216, new_n217,
    new_n218, new_n220, new_n222, new_n223, new_n224, new_n225, new_n226,
    new_n227, new_n228, new_n229, new_n230, new_n231, new_n232, new_n233,
    new_n234, new_n235, new_n236, new_n237, new_n238, new_n239, new_n240,
    new_n241, new_n243, new_n244, new_n245, new_n246, new_n247, new_n248,
    new_n249, new_n250, new_n251, new_n252, new_n253, new_n254, new_n256,
    new_n258, new_n260, new_n261, new_n262, new_n263, new_n264, new_n265,
    new_n266, new_n267, new_n268, new_n269, new_n270, new_n271, new_n272,
    new_n274, new_n276, new_n277, new_n279, new_n280, new_n281, new_n282,
    new_n283, new_n284, new_n285, new_n286, new_n287, new_n290, new_n291,
    new_n292, new_n293, new_n294, new_n295, new_n296, new_n298, new_n299,
    new_n301, new_n302, new_n303, new_n304, new_n305, new_n306, new_n308,
    new_n309, new_n310, new_n311, new_n313, new_n314, new_n316, new_n317,
    new_n318, new_n320, new_n321, new_n323, new_n324, new_n327, new_n328,
    new_n330, new_n331, new_n333, new_n334, new_n335, new_n336, new_n337,
    new_n338, new_n339, new_n340, new_n342, new_n343, new_n344;
  assign new_n87 = pwatch & powl_n;
  assign new_n88 = ~pbull2 & new_n87;
  assign new_n89 = pbull0 & new_n88;
  assign new_n90 = pbull1 & new_n89;
  assign new_n91 = ~pbull1 & powl_n;
  assign new_n92 = ~pwatch & powl_n;
  assign new_n93 = ~pbull0 & powl_n;
  assign new_n94 = ~new_n91 & ~new_n92;
  assign new_n95 = ~new_n93 & new_n94;
  assign new_n96 = pbull2 & ~new_n95;
  assign pbull2_p = new_n90 | new_n96;
  assign new_n98 = pfbi & powl_n;
  assign new_n99 = pstar0 & pstar1;
  assign new_n100 = ~pstar2 & new_n99;
  assign new_n101 = new_n98 & ~new_n100;
  assign new_n102 = pibt2 & ~pibt0;
  assign new_n103 = pibt1 & new_n102;
  assign new_n104 = ~pcat4 & new_n103;
  assign new_n105 = pibt2 & pibt0;
  assign new_n106 = ~pibt1 & new_n105;
  assign new_n107 = ~pcat3 & new_n106;
  assign new_n108 = ~pibt2 & pibt1;
  assign new_n109 = ~pcat0 & new_n108;
  assign new_n110 = ~pibt0 & new_n109;
  assign new_n111 = pibt1 & new_n105;
  assign new_n112 = ~pcat5 & new_n111;
  assign new_n113 = pibt0 & new_n108;
  assign new_n114 = ~pcat1 & new_n113;
  assign new_n115 = ~pibt1 & new_n102;
  assign new_n116 = ~pcat2 & new_n115;
  assign new_n117 = ~new_n104 & ~new_n107;
  assign new_n118 = ~new_n110 & new_n117;
  assign new_n119 = ~new_n112 & ~new_n114;
  assign new_n120 = ~new_n116 & new_n119;
  assign new_n121 = new_n118 & new_n120;
  assign porwd_f = ~pwatch | new_n121;
  assign new_n123 = new_n98 & ~porwd_f;
  assign new_n124 = ~new_n101 & ~new_n123;
  assign new_n125 = new_n98 & new_n124;
  assign new_n126 = pend & powl_n;
  assign pend_p = new_n125 | new_n126;
  assign new_n128 = pbull0 & pbull2;
  assign new_n129 = pbull1 & pwatch;
  assign new_n130 = new_n128 & new_n129;
  assign new_n131 = ~pbull3 & new_n130;
  assign new_n132 = powl_n & new_n131;
  assign new_n133 = pbull3 & ~new_n130;
  assign new_n134 = powl_n & new_n133;
  assign pbull3_p = new_n132 | new_n134;
  assign new_n136 = ~pbull3 & powl_n;
  assign new_n137 = pbull4 & new_n136;
  assign new_n138 = powl_n & ~new_n130;
  assign new_n139 = pbull4 & new_n138;
  assign new_n140 = pbull3 & new_n130;
  assign new_n141 = powl_n & new_n140;
  assign new_n142 = ~pbull4 & new_n141;
  assign new_n143 = ~new_n137 & ~new_n139;
  assign pbull4_p = new_n142 | ~new_n143;
  assign new_n145 = pvst1 & new_n126;
  assign new_n146 = ~pkbg_n & powl_n;
  assign new_n147 = ~pbull5 & pbull6;
  assign new_n148 = ~pbull3 & new_n147;
  assign new_n149 = pbull4 & new_n148;
  assign new_n150 = ~pbull0 & new_n149;
  assign new_n151 = pbull1 & new_n150;
  assign new_n152 = ~pbull2 & new_n151;
  assign new_n153 = new_n87 & new_n152;
  assign new_n154 = ~pcomppar & new_n126;
  assign new_n155 = ~pvst0 & new_n126;
  assign new_n156 = ~pmmerr & new_n155;
  assign new_n157 = ~new_n145 & ~new_n146;
  assign new_n158 = ~new_n153 & new_n157;
  assign new_n159 = ~new_n154 & ~new_n156;
  assign new_n160 = new_n158 & new_n159;
  assign new_n161 = pibt0 & ~new_n160;
  assign new_n162 = pibt2 & new_n161;
  assign new_n163 = ~pibt1 & new_n162;
  assign new_n164 = powl_n & ppluto3;
  assign ppluto3_p = new_n163 | new_n164;
  assign new_n166 = povacc & powl_n;
  assign new_n167 = ~pvacc & new_n166;
  assign pwatch_p = new_n87 | new_n167;
  assign new_n169 = pbull5 & new_n138;
  assign new_n170 = pbull4 & new_n141;
  assign new_n171 = ~pbull5 & new_n170;
  assign new_n172 = pbull5 & new_n136;
  assign new_n173 = pbull5 & pbull4_p;
  assign new_n174 = ~new_n169 & ~new_n171;
  assign new_n175 = ~new_n172 & ~new_n173;
  assign pbull5_p = ~new_n174 | ~new_n175;
  assign new_n177 = powl_n & pbull6;
  assign new_n178 = ~pbull4 & new_n177;
  assign new_n179 = ~pbull5 & new_n177;
  assign new_n180 = ~pbull2 & new_n177;
  assign new_n181 = ~pbull3 & new_n177;
  assign new_n182 = pbull6 & ~new_n95;
  assign new_n183 = pbull5 & new_n141;
  assign new_n184 = ~pbull6 & new_n183;
  assign new_n185 = pbull4 & new_n184;
  assign new_n186 = ~new_n95 & new_n130;
  assign new_n187 = pbull4 & new_n186;
  assign new_n188 = pbull5 & new_n187;
  assign new_n189 = pbull3 & new_n188;
  assign new_n190 = ~new_n178 & ~new_n179;
  assign new_n191 = ~new_n180 & ~new_n181;
  assign new_n192 = new_n190 & new_n191;
  assign new_n193 = ~new_n182 & ~new_n185;
  assign new_n194 = ~new_n189 & new_n193;
  assign pbull6_p = ~new_n192 | ~new_n194;
  assign new_n196 = pibt1 & new_n162;
  assign new_n197 = ppluto5 & powl_n;
  assign ppluto5_p = new_n196 | new_n197;
  assign new_n199 = pfbi & ~piclr;
  assign new_n200 = pdel1 & new_n199;
  assign new_n201 = ~pfbi & ~piclr;
  assign new_n202 = ppy & new_n201;
  assign ppy_p = new_n200 | new_n202;
  assign new_n204 = ~pibt0 & ~new_n160;
  assign new_n205 = pibt2 & new_n204;
  assign new_n206 = pibt1 & new_n205;
  assign new_n207 = ppluto4 & powl_n;
  assign ppluto4_p = new_n206 | new_n207;
  assign new_n209 = new_n87 & ~new_n121;
  assign new_n210 = ~pstar3 & new_n209;
  assign new_n211 = new_n100 & new_n210;
  assign new_n212 = pfbi & new_n211;
  assign new_n213 = plsd & powl_n;
  assign new_n214 = ~pstar3 & new_n213;
  assign new_n215 = ~new_n100 & new_n213;
  assign new_n216 = ~pfbi & new_n213;
  assign new_n217 = ~new_n212 & ~new_n214;
  assign new_n218 = ~new_n215 & ~new_n216;
  assign plsd_p = ~new_n217 | ~new_n218;
  assign new_n220 = pvlenesr & powl_n;
  assign pvlenesr_p = new_n146 | new_n220;
  assign new_n222 = ~porwd_n & new_n101;
  assign new_n223 = pfbi & new_n101;
  assign new_n224 = ~porwd_n & new_n209;
  assign new_n225 = ~new_n98 & ~new_n224;
  assign new_n226 = new_n209 & ~new_n225;
  assign new_n227 = ~porwd_n & new_n226;
  assign new_n228 = ~new_n99 & new_n226;
  assign new_n229 = pfbi & new_n209;
  assign new_n230 = powl_n & ~new_n99;
  assign new_n231 = ~new_n100 & new_n230;
  assign new_n232 = pstar2 & new_n231;
  assign new_n233 = new_n224 & new_n232;
  assign new_n234 = ~new_n99 & new_n101;
  assign new_n235 = pfbi & new_n232;
  assign new_n236 = ~new_n222 & ~new_n223;
  assign new_n237 = ~new_n227 & ~new_n228;
  assign new_n238 = new_n236 & new_n237;
  assign new_n239 = ~new_n234 & ~new_n235;
  assign new_n240 = ~new_n229 & ~new_n233;
  assign new_n241 = new_n239 & new_n240;
  assign pfbi_p = ~new_n238 | ~new_n241;
  assign new_n243 = new_n98 & ~new_n99;
  assign new_n244 = pstar0 & new_n243;
  assign new_n245 = ~pstar1 & new_n224;
  assign new_n246 = pstar0 & new_n245;
  assign new_n247 = pstar1 & new_n230;
  assign new_n248 = ~pstar0 & new_n247;
  assign new_n249 = ~porwd_n & ~porwd_f;
  assign new_n250 = ~pfbi & ~new_n249;
  assign new_n251 = powl_n & new_n250;
  assign new_n252 = pstar1 & new_n251;
  assign new_n253 = ~new_n244 & ~new_n246;
  assign new_n254 = ~new_n248 & ~new_n252;
  assign pstar1_p = ~new_n253 | ~new_n254;
  assign new_n256 = pvsumesr & powl_n;
  assign pvsumesr_p = new_n145 | new_n256;
  assign new_n258 = paccrpy & powl_n;
  assign paccrpy_p = new_n125 | new_n258;
  assign new_n260 = ~pcat1 & new_n109;
  assign new_n261 = pwatch & new_n260;
  assign new_n262 = ~new_n100 & new_n261;
  assign new_n263 = ~new_n100 & ~porwd_f;
  assign new_n264 = ~pstar3 & new_n261;
  assign new_n265 = ~pstar3 & ~porwd_f;
  assign new_n266 = ~new_n262 & ~new_n263;
  assign new_n267 = ~new_n264 & ~new_n265;
  assign new_n268 = new_n266 & new_n267;
  assign new_n269 = pkbg_n & ~new_n268;
  assign new_n270 = ~powl_n & ~new_n98;
  assign new_n271 = pkbg_n & new_n124;
  assign new_n272 = ~new_n269 & ~new_n270;
  assign pkbg_f = new_n271 | ~new_n272;
  assign new_n274 = pmarssr & powl_n;
  assign pmarssr_p = new_n153 | new_n274;
  assign new_n276 = pstar0 & new_n251;
  assign new_n277 = ~pstar0 & ~new_n225;
  assign pstar0_p = new_n276 | new_n277;
  assign new_n279 = ~pstar3 & new_n99;
  assign new_n280 = ~new_n225 & new_n279;
  assign new_n281 = pstar2 & new_n280;
  assign new_n282 = pstar3 & new_n230;
  assign new_n283 = ~pstar2 & pstar3;
  assign new_n284 = powl_n & new_n283;
  assign new_n285 = pstar3 & new_n251;
  assign new_n286 = ~new_n281 & ~new_n282;
  assign new_n287 = ~new_n284 & ~new_n285;
  assign pstar3_p = ~new_n286 | ~new_n287;
  assign pdel1_p = pcapsd & ~piclr;
  assign new_n290 = new_n98 & new_n99;
  assign new_n291 = ~pstar2 & new_n290;
  assign new_n292 = new_n100 & new_n224;
  assign new_n293 = pstar2 & new_n230;
  assign new_n294 = pstar2 & new_n251;
  assign new_n295 = ~new_n291 & ~new_n292;
  assign new_n296 = ~new_n293 & ~new_n294;
  assign pstar2_p = ~new_n295 | ~new_n296;
  assign new_n298 = pvst1 & new_n199;
  assign new_n299 = pvst0 & new_n201;
  assign pvst0_p = new_n298 | new_n299;
  assign new_n301 = pcomppar & powl_n;
  assign new_n302 = ~pfbi & new_n301;
  assign new_n303 = ~pdel1 & new_n301;
  assign new_n304 = ~pcomppar & new_n98;
  assign new_n305 = pdel1 & new_n304;
  assign new_n306 = ~new_n302 & ~new_n303;
  assign pcomppar_p = new_n305 | ~new_n306;
  assign new_n308 = pkbg_n & ~piclr;
  assign new_n309 = ~pend & new_n308;
  assign new_n310 = ~new_n152 & new_n309;
  assign new_n311 = ~pwatch & new_n309;
  assign powl_f = new_n310 | new_n311;
  assign new_n313 = ppy & new_n199;
  assign new_n314 = pvst1 & new_n201;
  assign pvst1_p = new_n313 | new_n314;
  assign new_n316 = new_n108 & ~new_n160;
  assign new_n317 = pibt0 & new_n316;
  assign new_n318 = powl_n & ppluto1;
  assign ppluto1_p = new_n317 | new_n318;
  assign new_n320 = pratr & powl_n;
  assign new_n321 = ~new_n154 & ~new_n320;
  assign pratr_p = new_n156 | ~new_n321;
  assign new_n323 = ~pibt0 & new_n316;
  assign new_n324 = ppluto0 & powl_n;
  assign ppluto0_p = new_n323 | new_n324;
  assign povacc_p = pvacc & ~piclr;
  assign new_n327 = ~pibt1 & new_n205;
  assign new_n328 = powl_n & ppluto2;
  assign ppluto2_p = new_n327 | new_n328;
  assign new_n330 = pbull0 & new_n92;
  assign new_n331 = ~pbull0 & new_n87;
  assign pbull0_p = new_n330 | new_n331;
  assign new_n333 = ~new_n152 & ~new_n268;
  assign new_n334 = pverr_n & new_n333;
  assign new_n335 = ~pwatch & new_n124;
  assign new_n336 = pverr_n & new_n335;
  assign new_n337 = new_n124 & ~new_n152;
  assign new_n338 = pverr_n & new_n337;
  assign new_n339 = ~new_n334 & ~new_n336;
  assign new_n340 = ~new_n270 & ~new_n338;
  assign pverr_f = ~new_n339 | ~new_n340;
  assign new_n342 = pbull0 & new_n87;
  assign new_n343 = ~pbull1 & new_n342;
  assign new_n344 = pbull1 & ~new_n95;
  assign pbull1_p = new_n343 | new_n344;
  assign psdo = pvst0;
endmodule


