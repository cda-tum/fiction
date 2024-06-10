// Benchmark "top" written by ABC on Mon Feb 19 11:52:43 2024

module top ( 
    pv28_20_, pv56_12_, pv56_23_, pv88_6_, pv88_19_, pv120_16_, pv120_29_,
    pv28_10_, pv56_13_, pv56_22_, pv88_7_, pv88_29_, pv120_15_, pv56_14_,
    pv56_25_, pv88_8_, pv88_17_, pv88_28_, pv120_18_, pv56_15_, pv56_24_,
    pv88_9_, pv88_18_, pv88_27_, pv120_17_, pv88_2_, pv88_15_, pv88_26_,
    pv120_12_, pv88_3_, pv88_16_, pv88_25_, pv120_11_, pv56_10_, pv56_21_,
    pv88_4_, pv88_13_, pv88_24_, pv120_14_, pv126_5_, pv56_11_, pv56_20_,
    pv88_5_, pv88_14_, pv88_23_, pv120_13_, pv28_8_, pv56_5_, pv88_11_,
    pv88_22_, pv120_5_, pv132_0_, pv28_9_, pv56_4_, pv88_12_, pv88_21_,
    pv120_6_, pv28_6_, pv56_7_, pv88_20_, pv120_3_, pv120_10_, pv28_7_,
    pv56_6_, pv88_10_, pv120_4_, pv28_4_, pv56_9_, pv120_1_, pv28_5_,
    pv56_8_, pv120_2_, pv28_2_, pv28_3_, pv120_0_, pv28_0_, pv28_1_,
    pv132_5_, pv56_1_, pv120_9_, pv120_30_, pv132_4_, pv56_0_, pv88_30_,
    pv132_3_, pv28_19_, pv56_3_, pv88_31_, pv120_7_, pv132_2_, pv56_2_,
    pv120_8_, pv120_20_, pv132_1_, pv28_17_, pv120_21_, pv126_3_, pv28_18_,
    pv28_27_, pv120_22_, pv126_4_, pv28_15_, pv28_26_, pv88_0_, pv120_23_,
    pv126_1_, pv28_16_, pv28_25_, pv88_1_, pv120_24_, pv120_31_, pv126_2_,
    pv28_13_, pv28_24_, pv56_16_, pv56_27_, pv120_25_, pv28_14_, pv28_23_,
    pv56_17_, pv56_26_, pv120_19_, pv120_26_, pv126_0_, pv28_11_, pv28_22_,
    pv56_18_, pv120_27_, pv28_12_, pv28_21_, pv56_19_, pv120_28_,
    pv138_3_, pv138_2_, pv138_1_, pv138_0_, pv134_1_, pv134_0_  );
  input  pv28_20_, pv56_12_, pv56_23_, pv88_6_, pv88_19_, pv120_16_,
    pv120_29_, pv28_10_, pv56_13_, pv56_22_, pv88_7_, pv88_29_, pv120_15_,
    pv56_14_, pv56_25_, pv88_8_, pv88_17_, pv88_28_, pv120_18_, pv56_15_,
    pv56_24_, pv88_9_, pv88_18_, pv88_27_, pv120_17_, pv88_2_, pv88_15_,
    pv88_26_, pv120_12_, pv88_3_, pv88_16_, pv88_25_, pv120_11_, pv56_10_,
    pv56_21_, pv88_4_, pv88_13_, pv88_24_, pv120_14_, pv126_5_, pv56_11_,
    pv56_20_, pv88_5_, pv88_14_, pv88_23_, pv120_13_, pv28_8_, pv56_5_,
    pv88_11_, pv88_22_, pv120_5_, pv132_0_, pv28_9_, pv56_4_, pv88_12_,
    pv88_21_, pv120_6_, pv28_6_, pv56_7_, pv88_20_, pv120_3_, pv120_10_,
    pv28_7_, pv56_6_, pv88_10_, pv120_4_, pv28_4_, pv56_9_, pv120_1_,
    pv28_5_, pv56_8_, pv120_2_, pv28_2_, pv28_3_, pv120_0_, pv28_0_,
    pv28_1_, pv132_5_, pv56_1_, pv120_9_, pv120_30_, pv132_4_, pv56_0_,
    pv88_30_, pv132_3_, pv28_19_, pv56_3_, pv88_31_, pv120_7_, pv132_2_,
    pv56_2_, pv120_8_, pv120_20_, pv132_1_, pv28_17_, pv120_21_, pv126_3_,
    pv28_18_, pv28_27_, pv120_22_, pv126_4_, pv28_15_, pv28_26_, pv88_0_,
    pv120_23_, pv126_1_, pv28_16_, pv28_25_, pv88_1_, pv120_24_, pv120_31_,
    pv126_2_, pv28_13_, pv28_24_, pv56_16_, pv56_27_, pv120_25_, pv28_14_,
    pv28_23_, pv56_17_, pv56_26_, pv120_19_, pv120_26_, pv126_0_, pv28_11_,
    pv28_22_, pv56_18_, pv120_27_, pv28_12_, pv28_21_, pv56_19_, pv120_28_;
  output pv138_3_, pv138_2_, pv138_1_, pv138_0_, pv134_1_, pv134_0_;
  wire new_n139, new_n140, new_n141, new_n142, new_n143, new_n144, new_n145,
    new_n146, new_n147, new_n148, new_n149, new_n150, new_n151, new_n152,
    new_n153, new_n154, new_n155, new_n156, new_n157, new_n158, new_n159,
    new_n160, new_n161, new_n162, new_n163, new_n164, new_n165, new_n166,
    new_n167, new_n168, new_n170, new_n171, new_n172, new_n173, new_n174,
    new_n175, new_n176, new_n177, new_n178, new_n179, new_n180, new_n181,
    new_n182, new_n183, new_n184, new_n185, new_n186, new_n187, new_n188,
    new_n189, new_n190, new_n191, new_n192, new_n193, new_n194, new_n195,
    new_n196, new_n197, new_n198, new_n199, new_n201, new_n202, new_n203,
    new_n204, new_n205, new_n206, new_n207, new_n208, new_n209, new_n210,
    new_n211, new_n212, new_n213, new_n214, new_n215, new_n216, new_n217,
    new_n218, new_n219, new_n220, new_n221, new_n222, new_n223, new_n224,
    new_n225, new_n226, new_n227, new_n228, new_n229, new_n230, new_n232,
    new_n233, new_n234, new_n235, new_n236, new_n237, new_n238, new_n239,
    new_n240, new_n241, new_n242, new_n243, new_n244, new_n245, new_n246,
    new_n247, new_n248, new_n249, new_n250, new_n251, new_n252, new_n253,
    new_n254, new_n255, new_n256, new_n257, new_n258, new_n259, new_n260,
    new_n261;
  assign new_n139 = ~pv132_3_ & ~pv126_3_;
  assign new_n140 = ~pv88_25_ & ~pv120_25_;
  assign new_n141 = ~pv132_4_ & ~pv126_4_;
  assign new_n142 = ~pv132_2_ & ~pv126_2_;
  assign new_n143 = ~pv126_5_ & ~pv132_5_;
  assign new_n144 = ~pv88_31_ & ~pv120_31_;
  assign new_n145 = ~pv120_30_ & ~pv88_30_;
  assign new_n146 = ~pv132_0_ & ~pv126_0_;
  assign new_n147 = ~pv132_1_ & ~pv126_1_;
  assign new_n148 = ~pv88_27_ & ~pv120_27_;
  assign new_n149 = ~pv120_29_ & ~pv88_29_;
  assign new_n150 = ~pv88_26_ & ~pv120_26_;
  assign new_n151 = ~pv88_23_ & ~pv120_23_;
  assign new_n152 = ~pv88_28_ & ~pv120_28_;
  assign new_n153 = ~pv88_24_ & ~pv120_24_;
  assign new_n154 = ~pv88_22_ & ~pv120_22_;
  assign new_n155 = ~new_n139 & ~new_n140;
  assign new_n156 = ~new_n141 & new_n155;
  assign new_n157 = ~new_n142 & new_n156;
  assign new_n158 = ~new_n143 & new_n157;
  assign new_n159 = ~new_n144 & new_n158;
  assign new_n160 = ~new_n145 & new_n159;
  assign new_n161 = ~new_n146 & new_n160;
  assign new_n162 = ~new_n147 & new_n161;
  assign new_n163 = ~new_n148 & new_n162;
  assign new_n164 = ~new_n149 & new_n163;
  assign new_n165 = ~new_n150 & new_n164;
  assign new_n166 = ~new_n151 & new_n165;
  assign new_n167 = ~new_n152 & new_n166;
  assign new_n168 = ~new_n153 & new_n167;
  assign pv138_3_ = ~new_n154 & new_n168;
  assign new_n170 = ~pv88_19_ & ~pv120_19_;
  assign new_n171 = ~pv88_9_ & ~pv120_9_;
  assign new_n172 = ~pv88_20_ & ~pv120_20_;
  assign new_n173 = ~pv120_18_ & ~pv88_18_;
  assign new_n174 = ~pv88_21_ & ~pv120_21_;
  assign new_n175 = ~pv120_15_ & ~pv88_15_;
  assign new_n176 = ~pv120_14_ & ~pv88_14_;
  assign new_n177 = ~pv120_16_ & ~pv88_16_;
  assign new_n178 = ~pv88_17_ & ~pv120_17_;
  assign new_n179 = ~pv120_11_ & ~pv88_11_;
  assign new_n180 = ~pv88_13_ & ~pv120_13_;
  assign new_n181 = ~pv120_10_ & ~pv88_10_;
  assign new_n182 = ~pv88_7_ & ~pv120_7_;
  assign new_n183 = ~pv120_12_ & ~pv88_12_;
  assign new_n184 = ~pv88_8_ & ~pv120_8_;
  assign new_n185 = ~pv88_6_ & ~pv120_6_;
  assign new_n186 = ~new_n170 & ~new_n171;
  assign new_n187 = ~new_n172 & new_n186;
  assign new_n188 = ~new_n173 & new_n187;
  assign new_n189 = ~new_n174 & new_n188;
  assign new_n190 = ~new_n175 & new_n189;
  assign new_n191 = ~new_n176 & new_n190;
  assign new_n192 = ~new_n177 & new_n191;
  assign new_n193 = ~new_n178 & new_n192;
  assign new_n194 = ~new_n179 & new_n193;
  assign new_n195 = ~new_n180 & new_n194;
  assign new_n196 = ~new_n181 & new_n195;
  assign new_n197 = ~new_n182 & new_n196;
  assign new_n198 = ~new_n183 & new_n197;
  assign new_n199 = ~new_n184 & new_n198;
  assign pv138_2_ = ~new_n185 & new_n199;
  assign new_n201 = ~pv88_3_ & ~pv120_3_;
  assign new_n202 = ~pv56_21_ & ~pv28_21_;
  assign new_n203 = ~pv88_4_ & ~pv120_4_;
  assign new_n204 = ~pv88_2_ & ~pv120_2_;
  assign new_n205 = ~pv88_5_ & ~pv120_5_;
  assign new_n206 = ~pv28_27_ & ~pv56_27_;
  assign new_n207 = ~pv28_26_ & ~pv56_26_;
  assign new_n208 = ~pv120_0_ & ~pv88_0_;
  assign new_n209 = ~pv120_1_ & ~pv88_1_;
  assign new_n210 = ~pv56_23_ & ~pv28_23_;
  assign new_n211 = ~pv56_25_ & ~pv28_25_;
  assign new_n212 = ~pv56_22_ & ~pv28_22_;
  assign new_n213 = ~pv28_19_ & ~pv56_19_;
  assign new_n214 = ~pv56_24_ & ~pv28_24_;
  assign new_n215 = ~pv28_20_ & ~pv56_20_;
  assign new_n216 = ~pv28_18_ & ~pv56_18_;
  assign new_n217 = ~new_n201 & ~new_n202;
  assign new_n218 = ~new_n203 & new_n217;
  assign new_n219 = ~new_n204 & new_n218;
  assign new_n220 = ~new_n205 & new_n219;
  assign new_n221 = ~new_n206 & new_n220;
  assign new_n222 = ~new_n207 & new_n221;
  assign new_n223 = ~new_n208 & new_n222;
  assign new_n224 = ~new_n209 & new_n223;
  assign new_n225 = ~new_n210 & new_n224;
  assign new_n226 = ~new_n211 & new_n225;
  assign new_n227 = ~new_n212 & new_n226;
  assign new_n228 = ~new_n213 & new_n227;
  assign new_n229 = ~new_n214 & new_n228;
  assign new_n230 = ~new_n215 & new_n229;
  assign pv138_1_ = ~new_n216 & new_n230;
  assign new_n232 = ~pv56_15_ & ~pv28_15_;
  assign new_n233 = ~pv56_5_ & ~pv28_5_;
  assign new_n234 = ~pv28_16_ & ~pv56_16_;
  assign new_n235 = ~pv56_14_ & ~pv28_14_;
  assign new_n236 = ~pv28_17_ & ~pv56_17_;
  assign new_n237 = ~pv56_11_ & ~pv28_11_;
  assign new_n238 = ~pv28_10_ & ~pv56_10_;
  assign new_n239 = ~pv56_12_ & ~pv28_12_;
  assign new_n240 = ~pv56_13_ & ~pv28_13_;
  assign new_n241 = ~pv56_7_ & ~pv28_7_;
  assign new_n242 = ~pv28_9_ & ~pv56_9_;
  assign new_n243 = ~pv28_6_ & ~pv56_6_;
  assign new_n244 = ~pv28_3_ & ~pv56_3_;
  assign new_n245 = ~pv28_8_ & ~pv56_8_;
  assign new_n246 = ~pv56_4_ & ~pv28_4_;
  assign new_n247 = ~pv28_2_ & ~pv56_2_;
  assign new_n248 = ~new_n232 & ~new_n233;
  assign new_n249 = ~new_n234 & new_n248;
  assign new_n250 = ~new_n235 & new_n249;
  assign new_n251 = ~new_n236 & new_n250;
  assign new_n252 = ~new_n237 & new_n251;
  assign new_n253 = ~new_n238 & new_n252;
  assign new_n254 = ~new_n239 & new_n253;
  assign new_n255 = ~new_n240 & new_n254;
  assign new_n256 = ~new_n241 & new_n255;
  assign new_n257 = ~new_n242 & new_n256;
  assign new_n258 = ~new_n243 & new_n257;
  assign new_n259 = ~new_n244 & new_n258;
  assign new_n260 = ~new_n245 & new_n259;
  assign new_n261 = ~new_n246 & new_n260;
  assign pv138_0_ = ~new_n247 & new_n261;
  assign pv134_1_ = pv28_1_ | pv56_1_;
  assign pv134_0_ = pv28_0_ | pv56_0_;
endmodule


