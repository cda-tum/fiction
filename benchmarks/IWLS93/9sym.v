// Benchmark "top" written by ABC on Mon Feb 19 11:52:41 2024

module top ( 
    i_7_, i_8_, i_5_, i_6_, i_3_, i_4_, i_1_, i_2_, i_0_,
    o_0_  );
  input  i_7_, i_8_, i_5_, i_6_, i_3_, i_4_, i_1_, i_2_, i_0_;
  output o_0_;
  wire new_n11, new_n12, new_n13, new_n14, new_n15, new_n16, new_n17,
    new_n18, new_n19, new_n20, new_n21, new_n22, new_n23, new_n24, new_n25,
    new_n26, new_n27, new_n28, new_n29, new_n30, new_n31, new_n32, new_n33,
    new_n34, new_n35, new_n36, new_n37, new_n38, new_n39, new_n40, new_n41,
    new_n42, new_n43, new_n44, new_n45, new_n46, new_n47, new_n48, new_n49,
    new_n50, new_n51, new_n52, new_n53, new_n54, new_n55, new_n56, new_n57,
    new_n58, new_n59, new_n60, new_n61, new_n62, new_n63, new_n64, new_n65,
    new_n66, new_n67, new_n68, new_n69, new_n70, new_n71, new_n72, new_n73,
    new_n74, new_n75, new_n76, new_n77, new_n78, new_n79, new_n80, new_n81,
    new_n82, new_n83, new_n84, new_n85, new_n86, new_n87, new_n88, new_n89,
    new_n90, new_n91, new_n92, new_n93, new_n94, new_n95, new_n96, new_n97,
    new_n98, new_n99, new_n100, new_n101, new_n102, new_n103, new_n104,
    new_n105, new_n106, new_n107, new_n108, new_n109, new_n110, new_n111,
    new_n112, new_n113, new_n114, new_n115, new_n116, new_n117, new_n118,
    new_n119, new_n120, new_n121, new_n122, new_n123, new_n124, new_n125,
    new_n126, new_n127, new_n128, new_n129, new_n130, new_n131, new_n132,
    new_n133, new_n134, new_n135, new_n136, new_n137, new_n138, new_n139,
    new_n140, new_n141, new_n142, new_n143, new_n144, new_n145, new_n146,
    new_n147, new_n148, new_n149, new_n150, new_n151, new_n152, new_n153,
    new_n154, new_n155, new_n156, new_n157, new_n158, new_n159, new_n160,
    new_n161, new_n162, new_n163, new_n164, new_n165, new_n166, new_n167,
    new_n168, new_n169, new_n170, new_n171, new_n172, new_n173, new_n174,
    new_n175, new_n176, new_n177, new_n178, new_n179, new_n180, new_n181,
    new_n182, new_n183, new_n184, new_n185, new_n186, new_n187, new_n188,
    new_n189, new_n190, new_n191, new_n192, new_n193, new_n194, new_n195,
    new_n196, new_n197, new_n198, new_n199, new_n200, new_n201, new_n202,
    new_n203, new_n204, new_n205, new_n206, new_n207, new_n208, new_n209,
    new_n210, new_n211, new_n212, new_n213, new_n214, new_n215, new_n216,
    new_n217, new_n218, new_n219, new_n220, new_n221, new_n222, new_n223,
    new_n224, new_n225, new_n226, new_n227, new_n228, new_n229, new_n230,
    new_n231, new_n232, new_n233, new_n234, new_n235, new_n236, new_n237,
    new_n238, new_n239, new_n240, new_n241, new_n242, new_n243, new_n244,
    new_n245, new_n246, new_n247, new_n248, new_n249, new_n250, new_n251,
    new_n252, new_n253, new_n254, new_n255, new_n256, new_n257, new_n258,
    new_n259, new_n260, new_n261, new_n262, new_n263, new_n264, new_n265,
    new_n266, new_n267, new_n268, new_n269, new_n270, new_n271, new_n272,
    new_n273, new_n274, new_n275, new_n276, new_n277, new_n278, new_n279,
    new_n280, new_n281, new_n282, new_n283, new_n284, new_n285, new_n286,
    new_n287, new_n288, new_n289, new_n290, new_n291, new_n292, new_n293,
    new_n294, new_n295, new_n296, new_n297, new_n298, new_n299, new_n300,
    new_n301, new_n302, new_n303, new_n304, new_n305, new_n306, new_n307,
    new_n308, new_n309, new_n310, new_n311, new_n312, new_n313, new_n314,
    new_n315, new_n316, new_n317, new_n318, new_n319, new_n320, new_n321,
    new_n322, new_n323, new_n324, new_n325, new_n326, new_n327, new_n328,
    new_n329, new_n330, new_n331, new_n332, new_n333, new_n334, new_n335,
    new_n336, new_n337, new_n338, new_n339, new_n340, new_n341, new_n342,
    new_n343, new_n344, new_n345;
  assign new_n11 = ~i_7_ & ~i_5_;
  assign new_n12 = i_4_ & new_n11;
  assign new_n13 = i_3_ & i_1_;
  assign new_n14 = ~i_0_ & new_n13;
  assign new_n15 = new_n12 & new_n14;
  assign new_n16 = ~i_7_ & ~i_6_;
  assign new_n17 = i_4_ & new_n16;
  assign new_n18 = i_3_ & i_2_;
  assign new_n19 = ~i_0_ & new_n18;
  assign new_n20 = new_n17 & new_n19;
  assign new_n21 = ~i_4_ & new_n11;
  assign new_n22 = i_0_ & new_n13;
  assign new_n23 = new_n21 & new_n22;
  assign new_n24 = ~new_n15 & ~new_n20;
  assign new_n25 = ~new_n23 & new_n24;
  assign new_n26 = ~i_8_ & i_6_;
  assign new_n27 = i_5_ & new_n26;
  assign new_n28 = i_1_ & ~i_2_;
  assign new_n29 = ~i_0_ & new_n28;
  assign new_n30 = new_n27 & new_n29;
  assign new_n31 = ~i_7_ & i_5_;
  assign new_n32 = ~i_4_ & new_n31;
  assign new_n33 = new_n14 & new_n32;
  assign new_n34 = ~i_7_ & i_8_;
  assign new_n35 = i_5_ & new_n34;
  assign new_n36 = i_3_ & ~i_2_;
  assign new_n37 = ~i_0_ & new_n36;
  assign new_n38 = new_n35 & new_n37;
  assign new_n39 = ~new_n30 & ~new_n33;
  assign new_n40 = ~new_n38 & new_n39;
  assign new_n41 = new_n25 & new_n40;
  assign new_n42 = ~i_7_ & ~i_8_;
  assign new_n43 = i_6_ & new_n42;
  assign new_n44 = ~i_1_ & new_n18;
  assign new_n45 = new_n43 & new_n44;
  assign new_n46 = i_7_ & i_8_;
  assign new_n47 = ~i_6_ & new_n46;
  assign new_n48 = ~i_1_ & i_2_;
  assign new_n49 = ~i_0_ & new_n48;
  assign new_n50 = new_n47 & new_n49;
  assign new_n51 = ~i_8_ & ~i_6_;
  assign new_n52 = i_5_ & new_n51;
  assign new_n53 = ~i_4_ & i_2_;
  assign new_n54 = i_1_ & new_n53;
  assign new_n55 = new_n52 & new_n54;
  assign new_n56 = ~new_n45 & ~new_n50;
  assign new_n57 = ~new_n55 & new_n56;
  assign new_n58 = i_4_ & new_n26;
  assign new_n59 = ~i_3_ & ~i_2_;
  assign new_n60 = i_0_ & new_n59;
  assign new_n61 = new_n58 & new_n60;
  assign new_n62 = ~i_5_ & new_n26;
  assign new_n63 = i_3_ & i_4_;
  assign new_n64 = ~i_1_ & new_n63;
  assign new_n65 = new_n62 & new_n64;
  assign new_n66 = i_7_ & ~i_8_;
  assign new_n67 = ~i_6_ & new_n66;
  assign new_n68 = ~i_5_ & i_2_;
  assign new_n69 = i_1_ & new_n68;
  assign new_n70 = new_n67 & new_n69;
  assign new_n71 = ~new_n61 & ~new_n65;
  assign new_n72 = ~new_n70 & new_n71;
  assign new_n73 = ~i_5_ & i_4_;
  assign new_n74 = ~i_1_ & new_n73;
  assign new_n75 = new_n47 & new_n74;
  assign new_n76 = ~i_4_ & new_n42;
  assign new_n77 = i_0_ & new_n18;
  assign new_n78 = new_n76 & new_n77;
  assign new_n79 = i_6_ & new_n66;
  assign new_n80 = ~i_5_ & ~i_2_;
  assign new_n81 = i_1_ & new_n80;
  assign new_n82 = new_n79 & new_n81;
  assign new_n83 = ~new_n75 & ~new_n78;
  assign new_n84 = ~new_n82 & new_n83;
  assign new_n85 = new_n57 & new_n72;
  assign new_n86 = new_n84 & new_n85;
  assign new_n87 = ~i_0_ & new_n63;
  assign new_n88 = new_n67 & new_n87;
  assign new_n89 = i_1_ & new_n36;
  assign new_n90 = new_n67 & new_n89;
  assign new_n91 = i_5_ & new_n16;
  assign new_n92 = ~i_4_ & i_1_;
  assign new_n93 = i_0_ & new_n92;
  assign new_n94 = new_n91 & new_n93;
  assign new_n95 = ~new_n88 & ~new_n90;
  assign new_n96 = ~new_n94 & new_n95;
  assign new_n97 = ~i_4_ & new_n34;
  assign new_n98 = i_0_ & new_n36;
  assign new_n99 = new_n97 & new_n98;
  assign new_n100 = i_8_ & ~i_6_;
  assign new_n101 = i_5_ & new_n100;
  assign new_n102 = ~i_0_ & new_n92;
  assign new_n103 = new_n101 & new_n102;
  assign new_n104 = i_6_ & new_n34;
  assign new_n105 = ~i_3_ & i_2_;
  assign new_n106 = ~i_1_ & new_n105;
  assign new_n107 = new_n104 & new_n106;
  assign new_n108 = ~new_n99 & ~new_n103;
  assign new_n109 = ~new_n107 & new_n108;
  assign new_n110 = ~i_5_ & i_6_;
  assign new_n111 = i_4_ & new_n110;
  assign new_n112 = new_n29 & new_n111;
  assign new_n113 = i_7_ & i_5_;
  assign new_n114 = i_4_ & new_n113;
  assign new_n115 = ~i_3_ & ~i_1_;
  assign new_n116 = ~i_0_ & new_n115;
  assign new_n117 = new_n114 & new_n116;
  assign new_n118 = i_5_ & new_n42;
  assign new_n119 = ~i_2_ & new_n63;
  assign new_n120 = new_n118 & new_n119;
  assign new_n121 = ~new_n112 & ~new_n117;
  assign new_n122 = ~new_n120 & new_n121;
  assign new_n123 = new_n96 & new_n109;
  assign new_n124 = new_n122 & new_n123;
  assign new_n125 = ~i_4_ & new_n113;
  assign new_n126 = i_1_ & new_n59;
  assign new_n127 = new_n125 & new_n126;
  assign new_n128 = ~i_8_ & ~i_4_;
  assign new_n129 = ~i_3_ & new_n128;
  assign new_n130 = i_1_ & i_2_;
  assign new_n131 = i_0_ & new_n130;
  assign new_n132 = new_n129 & new_n131;
  assign new_n133 = i_5_ & i_4_;
  assign new_n134 = ~i_3_ & new_n133;
  assign new_n135 = ~i_1_ & ~i_2_;
  assign new_n136 = i_0_ & new_n135;
  assign new_n137 = new_n134 & new_n136;
  assign new_n138 = ~new_n127 & ~new_n132;
  assign new_n139 = ~new_n137 & new_n138;
  assign new_n140 = i_5_ & ~i_6_;
  assign new_n141 = ~i_3_ & new_n140;
  assign new_n142 = i_0_ & new_n48;
  assign new_n143 = new_n141 & new_n142;
  assign new_n144 = ~i_3_ & i_4_;
  assign new_n145 = ~i_1_ & new_n144;
  assign new_n146 = new_n27 & new_n145;
  assign new_n147 = new_n67 & new_n142;
  assign new_n148 = ~new_n143 & ~new_n146;
  assign new_n149 = ~new_n147 & new_n148;
  assign new_n150 = ~i_6_ & new_n34;
  assign new_n151 = i_4_ & ~i_2_;
  assign new_n152 = i_0_ & new_n151;
  assign new_n153 = new_n150 & new_n152;
  assign new_n154 = ~i_3_ & ~i_4_;
  assign new_n155 = i_0_ & new_n154;
  assign new_n156 = new_n101 & new_n155;
  assign new_n157 = i_8_ & i_6_;
  assign new_n158 = i_5_ & new_n157;
  assign new_n159 = ~i_4_ & ~i_2_;
  assign new_n160 = ~i_0_ & new_n159;
  assign new_n161 = new_n158 & new_n160;
  assign new_n162 = ~new_n153 & ~new_n156;
  assign new_n163 = ~new_n161 & new_n162;
  assign new_n164 = new_n139 & new_n149;
  assign new_n165 = new_n163 & new_n164;
  assign new_n166 = new_n86 & new_n124;
  assign new_n167 = new_n165 & new_n166;
  assign new_n168 = i_8_ & ~i_5_;
  assign new_n169 = ~i_3_ & new_n168;
  assign new_n170 = i_0_ & new_n28;
  assign new_n171 = new_n169 & new_n170;
  assign new_n172 = ~i_5_ & new_n66;
  assign new_n173 = new_n19 & new_n172;
  assign new_n174 = ~i_5_ & new_n100;
  assign new_n175 = i_4_ & i_1_;
  assign new_n176 = ~i_0_ & new_n175;
  assign new_n177 = new_n174 & new_n176;
  assign new_n178 = ~new_n171 & ~new_n173;
  assign new_n179 = ~new_n177 & new_n178;
  assign new_n180 = ~i_7_ & i_6_;
  assign new_n181 = ~i_5_ & new_n180;
  assign new_n182 = i_3_ & ~i_1_;
  assign new_n183 = i_0_ & new_n182;
  assign new_n184 = new_n181 & new_n183;
  assign new_n185 = i_7_ & ~i_5_;
  assign new_n186 = i_4_ & new_n185;
  assign new_n187 = ~i_3_ & i_1_;
  assign new_n188 = ~i_0_ & new_n187;
  assign new_n189 = new_n186 & new_n188;
  assign new_n190 = i_7_ & i_6_;
  assign new_n191 = ~i_5_ & new_n190;
  assign new_n192 = ~i_0_ & new_n182;
  assign new_n193 = new_n191 & new_n192;
  assign new_n194 = ~new_n184 & ~new_n189;
  assign new_n195 = ~new_n193 & new_n194;
  assign new_n196 = new_n125 & new_n192;
  assign new_n197 = ~i_8_ & ~i_5_;
  assign new_n198 = i_4_ & new_n197;
  assign new_n199 = i_0_ & new_n187;
  assign new_n200 = new_n198 & new_n199;
  assign new_n201 = ~i_1_ & new_n36;
  assign new_n202 = new_n104 & new_n201;
  assign new_n203 = ~new_n196 & ~new_n200;
  assign new_n204 = ~new_n202 & new_n203;
  assign new_n205 = new_n179 & new_n195;
  assign new_n206 = new_n204 & new_n205;
  assign new_n207 = i_4_ & new_n66;
  assign new_n208 = new_n49 & new_n207;
  assign new_n209 = i_2_ & new_n144;
  assign new_n210 = new_n150 & new_n209;
  assign new_n211 = i_5_ & new_n180;
  assign new_n212 = new_n192 & new_n211;
  assign new_n213 = ~new_n208 & ~new_n210;
  assign new_n214 = ~new_n212 & new_n213;
  assign new_n215 = ~i_5_ & new_n46;
  assign new_n216 = i_3_ & ~i_4_;
  assign new_n217 = ~i_0_ & new_n216;
  assign new_n218 = new_n215 & new_n217;
  assign new_n219 = ~i_5_ & new_n42;
  assign new_n220 = i_1_ & new_n18;
  assign new_n221 = new_n219 & new_n220;
  assign new_n222 = i_5_ & new_n46;
  assign new_n223 = ~i_1_ & new_n154;
  assign new_n224 = new_n222 & new_n223;
  assign new_n225 = ~new_n218 & ~new_n221;
  assign new_n226 = ~new_n224 & new_n225;
  assign new_n227 = ~i_2_ & new_n144;
  assign new_n228 = new_n104 & new_n227;
  assign new_n229 = new_n89 & new_n181;
  assign new_n230 = new_n19 & new_n118;
  assign new_n231 = ~new_n228 & ~new_n229;
  assign new_n232 = ~new_n230 & new_n231;
  assign new_n233 = new_n214 & new_n226;
  assign new_n234 = new_n232 & new_n233;
  assign new_n235 = ~i_4_ & new_n185;
  assign new_n236 = new_n199 & new_n235;
  assign new_n237 = new_n47 & new_n81;
  assign new_n238 = new_n136 & new_n215;
  assign new_n239 = ~new_n236 & ~new_n237;
  assign new_n240 = ~new_n238 & new_n239;
  assign new_n241 = new_n183 & new_n235;
  assign new_n242 = i_1_ & new_n105;
  assign new_n243 = new_n174 & new_n242;
  assign new_n244 = ~i_5_ & new_n16;
  assign new_n245 = i_0_ & new_n63;
  assign new_n246 = new_n244 & new_n245;
  assign new_n247 = ~new_n241 & ~new_n243;
  assign new_n248 = ~new_n246 & new_n247;
  assign new_n249 = new_n64 & new_n150;
  assign new_n250 = new_n32 & new_n183;
  assign new_n251 = ~i_3_ & new_n157;
  assign new_n252 = new_n29 & new_n251;
  assign new_n253 = ~new_n249 & ~new_n250;
  assign new_n254 = ~new_n252 & new_n253;
  assign new_n255 = new_n240 & new_n248;
  assign new_n256 = new_n254 & new_n255;
  assign new_n257 = new_n206 & new_n234;
  assign new_n258 = new_n256 & new_n257;
  assign new_n259 = new_n52 & new_n209;
  assign new_n260 = i_7_ & ~i_6_;
  assign new_n261 = i_4_ & new_n260;
  assign new_n262 = new_n136 & new_n261;
  assign new_n263 = i_5_ & new_n260;
  assign new_n264 = ~i_1_ & new_n53;
  assign new_n265 = new_n263 & new_n264;
  assign new_n266 = ~new_n259 & ~new_n262;
  assign new_n267 = ~new_n265 & new_n266;
  assign new_n268 = ~i_4_ & new_n26;
  assign new_n269 = new_n170 & new_n268;
  assign new_n270 = ~i_4_ & ~i_1_;
  assign new_n271 = i_0_ & new_n270;
  assign new_n272 = new_n79 & new_n271;
  assign new_n273 = i_0_ & new_n53;
  assign new_n274 = new_n62 & new_n273;
  assign new_n275 = ~new_n269 & ~new_n272;
  assign new_n276 = ~new_n274 & new_n275;
  assign new_n277 = i_6_ & new_n46;
  assign new_n278 = ~i_1_ & new_n159;
  assign new_n279 = new_n277 & new_n278;
  assign new_n280 = ~i_0_ & new_n53;
  assign new_n281 = new_n27 & new_n280;
  assign new_n282 = i_5_ & ~i_2_;
  assign new_n283 = ~i_1_ & new_n282;
  assign new_n284 = new_n79 & new_n283;
  assign new_n285 = ~new_n279 & ~new_n281;
  assign new_n286 = ~new_n284 & new_n285;
  assign new_n287 = new_n267 & new_n276;
  assign new_n288 = new_n286 & new_n287;
  assign new_n289 = new_n155 & new_n211;
  assign new_n290 = new_n106 & new_n191;
  assign new_n291 = i_6_ & ~i_4_;
  assign new_n292 = ~i_3_ & new_n291;
  assign new_n293 = ~i_0_ & new_n130;
  assign new_n294 = new_n292 & new_n293;
  assign new_n295 = ~new_n289 & ~new_n290;
  assign new_n296 = ~new_n294 & new_n295;
  assign new_n297 = ~i_6_ & i_4_;
  assign new_n298 = ~i_3_ & new_n297;
  assign new_n299 = new_n293 & new_n298;
  assign new_n300 = ~i_5_ & new_n157;
  assign new_n301 = i_0_ & new_n159;
  assign new_n302 = new_n300 & new_n301;
  assign new_n303 = ~i_0_ & new_n151;
  assign new_n304 = new_n101 & new_n303;
  assign new_n305 = ~new_n299 & ~new_n302;
  assign new_n306 = ~new_n304 & new_n305;
  assign new_n307 = i_4_ & new_n190;
  assign new_n308 = ~i_0_ & new_n135;
  assign new_n309 = new_n307 & new_n308;
  assign new_n310 = new_n174 & new_n273;
  assign new_n311 = i_4_ & new_n140;
  assign new_n312 = new_n126 & new_n311;
  assign new_n313 = ~new_n309 & ~new_n310;
  assign new_n314 = ~new_n312 & new_n313;
  assign new_n315 = new_n296 & new_n306;
  assign new_n316 = new_n314 & new_n315;
  assign new_n317 = ~i_5_ & new_n34;
  assign new_n318 = new_n14 & new_n317;
  assign new_n319 = new_n242 & new_n317;
  assign new_n320 = new_n44 & new_n317;
  assign new_n321 = ~new_n318 & ~new_n319;
  assign new_n322 = ~new_n320 & new_n321;
  assign new_n323 = ~i_6_ & new_n42;
  assign new_n324 = i_4_ & i_2_;
  assign new_n325 = i_0_ & new_n324;
  assign new_n326 = new_n323 & new_n325;
  assign new_n327 = new_n43 & new_n209;
  assign new_n328 = new_n263 & new_n301;
  assign new_n329 = ~new_n326 & ~new_n327;
  assign new_n330 = ~new_n328 & new_n329;
  assign new_n331 = i_5_ & new_n66;
  assign new_n332 = new_n188 & new_n331;
  assign new_n333 = i_5_ & i_2_;
  assign new_n334 = ~i_0_ & new_n333;
  assign new_n335 = new_n150 & new_n334;
  assign new_n336 = i_4_ & new_n31;
  assign new_n337 = new_n188 & new_n336;
  assign new_n338 = ~new_n332 & ~new_n335;
  assign new_n339 = ~new_n337 & new_n338;
  assign new_n340 = new_n322 & new_n330;
  assign new_n341 = new_n339 & new_n340;
  assign new_n342 = new_n288 & new_n316;
  assign new_n343 = new_n341 & new_n342;
  assign new_n344 = new_n167 & new_n258;
  assign new_n345 = new_n343 & new_n344;
  assign o_0_ = ~new_n41 | ~new_n345;
endmodule


