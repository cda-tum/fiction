// Benchmark "b03" written by ABC on Wed Sep  5 10:17:19 2018

module b03 ( clock, 
    REQUEST1, REQUEST2, REQUEST3, REQUEST4,
    GRANT_O_REG_3_, GRANT_O_REG_2_, GRANT_O_REG_1_, GRANT_O_REG_0_  );
  input  clock;
  input  REQUEST1, REQUEST2, REQUEST3, REQUEST4;
  output GRANT_O_REG_3_, GRANT_O_REG_2_, GRANT_O_REG_1_, GRANT_O_REG_0_;
  reg STATO_REG_0_, CODA0_REG_2_, CODA0_REG_1_, CODA0_REG_0_, CODA1_REG_2_,
    CODA1_REG_1_, CODA1_REG_0_, CODA2_REG_2_, CODA2_REG_1_, CODA2_REG_0_,
    CODA3_REG_2_, CODA3_REG_1_, CODA3_REG_0_, GRANT_REG_3_, GRANT_REG_2_,
    GRANT_REG_1_, GRANT_REG_0_, GRANT_O_REG_3_, GRANT_O_REG_2_,
    GRANT_O_REG_1_, GRANT_O_REG_0_, RU3_REG, FU1_REG, FU3_REG, RU1_REG,
    RU4_REG, FU2_REG, FU4_REG, RU2_REG, STATO_REG_1_;
  wire n99, n100, n101, n102, n103_1, n104, n105, n106, n107_1, n108, n109,
    n110, n111_1, n112, n113, n114, n115_1, n116, n117, n118, n119_1, n120,
    n121, n122, n123, n124_1, n126, n127, n128, n129_1, n130, n132, n133,
    n134_1, n135, n136, n138, n139_1, n140, n141, n143, n144_1, n145, n146,
    n148, n149_1, n150, n151, n153, n154_1, n155, n156, n158_1, n159, n160,
    n161, n163, n164, n165, n166, n168, n169, n171, n172, n174, n175, n177,
    n178, n179, n180, n182, n183, n184, n185, n187, n188, n189, n190, n192,
    n193, n194, n195, n197, n198, n200, n201, n203, n204, n206, n207, n209,
    n210, n212, n214, n216, n218, n219, n221, n222, n224, n226, n18, n23,
    n28, n33, n38, n43, n48, n53, n58, n63, n68, n73, n78, n83, n88, n93,
    n98, n103, n107, n111, n115, n119, n124, n129, n134, n139, n144, n149,
    n154, n158;
  assign n99 = STATO_REG_0_ & RU1_REG;
  assign n100 = ~FU1_REG & n99;
  assign n101 = ~FU3_REG & ~FU2_REG;
  assign n102 = ~FU1_REG & n101;
  assign n103_1 = ~FU4_REG & n102;
  assign n104 = STATO_REG_1_ & ~n103_1;
  assign n105 = ~RU3_REG & ~FU4_REG;
  assign n106 = RU4_REG & n105;
  assign n107_1 = ~RU2_REG & n106;
  assign n108 = ~FU2_REG & RU2_REG;
  assign n109 = RU3_REG & ~RU2_REG;
  assign n110 = ~FU3_REG & n109;
  assign n111_1 = ~n107_1 & ~n108;
  assign n112 = ~n110 & n111_1;
  assign n113 = STATO_REG_0_ & ~RU1_REG;
  assign n114 = ~n112 & n113;
  assign n115_1 = ~n100 & ~n104;
  assign n116 = ~n114 & n115_1;
  assign n117 = STATO_REG_1_ & ~n116;
  assign n118 = CODA1_REG_2_ & n117;
  assign n119_1 = ~STATO_REG_1_ & ~n116;
  assign n120 = ~RU3_REG & ~RU2_REG;
  assign n121 = ~RU1_REG & ~n120;
  assign n122 = n119_1 & ~n121;
  assign n123 = CODA0_REG_2_ & n116;
  assign n124_1 = ~n118 & ~n122;
  assign n23 = n123 | ~n124_1;
  assign n126 = CODA1_REG_1_ & n117;
  assign n127 = ~RU1_REG & ~n109;
  assign n128 = n119_1 & n127;
  assign n129_1 = CODA0_REG_1_ & n116;
  assign n130 = ~n126 & ~n128;
  assign n28 = n129_1 | ~n130;
  assign n132 = CODA1_REG_0_ & n117;
  assign n133 = ~RU1_REG & ~RU2_REG;
  assign n134_1 = n119_1 & n133;
  assign n135 = CODA0_REG_0_ & n116;
  assign n136 = ~n132 & ~n134_1;
  assign n33 = n135 | ~n136;
  assign n138 = CODA0_REG_2_ & n119_1;
  assign n139_1 = CODA2_REG_2_ & n117;
  assign n140 = CODA1_REG_2_ & n116;
  assign n141 = ~n138 & ~n139_1;
  assign n38 = n140 | ~n141;
  assign n143 = CODA0_REG_1_ & n119_1;
  assign n144_1 = CODA2_REG_1_ & n117;
  assign n145 = CODA1_REG_1_ & n116;
  assign n146 = ~n143 & ~n144_1;
  assign n43 = n145 | ~n146;
  assign n148 = CODA0_REG_0_ & n119_1;
  assign n149_1 = CODA2_REG_0_ & n117;
  assign n150 = CODA1_REG_0_ & n116;
  assign n151 = ~n148 & ~n149_1;
  assign n48 = n150 | ~n151;
  assign n153 = CODA1_REG_2_ & n119_1;
  assign n154_1 = CODA3_REG_2_ & n117;
  assign n155 = CODA2_REG_2_ & n116;
  assign n156 = ~n153 & ~n154_1;
  assign n53 = n155 | ~n156;
  assign n158_1 = CODA1_REG_1_ & n119_1;
  assign n159 = CODA3_REG_1_ & n117;
  assign n160 = CODA2_REG_1_ & n116;
  assign n161 = ~n158_1 & ~n159;
  assign n58 = n160 | ~n161;
  assign n163 = CODA1_REG_0_ & n119_1;
  assign n164 = CODA3_REG_0_ & n117;
  assign n165 = CODA2_REG_0_ & n116;
  assign n166 = ~n163 & ~n164;
  assign n63 = n165 | ~n166;
  assign n168 = CODA3_REG_2_ & n116;
  assign n169 = CODA2_REG_2_ & n119_1;
  assign n68 = n168 | n169;
  assign n171 = CODA3_REG_1_ & n116;
  assign n172 = CODA2_REG_1_ & n119_1;
  assign n73 = n171 | n172;
  assign n174 = CODA3_REG_0_ & n116;
  assign n175 = CODA2_REG_0_ & n119_1;
  assign n78 = n174 | n175;
  assign n177 = ~CODA0_REG_1_ & ~CODA0_REG_0_;
  assign n178 = CODA0_REG_2_ & n177;
  assign n179 = n104 & n178;
  assign n180 = GRANT_REG_3_ & ~n104;
  assign n83 = n179 | n180;
  assign n182 = ~CODA0_REG_2_ & ~CODA0_REG_0_;
  assign n183 = CODA0_REG_1_ & n182;
  assign n184 = n104 & n183;
  assign n185 = GRANT_REG_2_ & ~n104;
  assign n88 = n184 | n185;
  assign n187 = ~CODA0_REG_2_ & ~CODA0_REG_1_;
  assign n188 = CODA0_REG_0_ & n187;
  assign n189 = n104 & n188;
  assign n190 = GRANT_REG_1_ & ~n104;
  assign n93 = n189 | n190;
  assign n192 = CODA0_REG_2_ & CODA0_REG_1_;
  assign n193 = CODA0_REG_0_ & n192;
  assign n194 = n104 & n193;
  assign n195 = GRANT_REG_0_ & ~n104;
  assign n98 = n194 | n195;
  assign n197 = STATO_REG_0_ & GRANT_REG_3_;
  assign n198 = ~STATO_REG_0_ & GRANT_O_REG_3_;
  assign n103 = n197 | n198;
  assign n200 = STATO_REG_0_ & GRANT_REG_2_;
  assign n201 = ~STATO_REG_0_ & GRANT_O_REG_2_;
  assign n107 = n200 | n201;
  assign n203 = STATO_REG_0_ & GRANT_REG_1_;
  assign n204 = ~STATO_REG_0_ & GRANT_O_REG_1_;
  assign n111 = n203 | n204;
  assign n206 = STATO_REG_0_ & GRANT_REG_0_;
  assign n207 = ~STATO_REG_0_ & GRANT_O_REG_0_;
  assign n115 = n206 | n207;
  assign n209 = STATO_REG_0_ & RU3_REG;
  assign n210 = REQUEST3 & ~STATO_REG_0_;
  assign n119 = n209 | n210;
  assign n212 = ~STATO_REG_0_ & FU1_REG;
  assign n124 = n99 | n212;
  assign n214 = ~STATO_REG_0_ & FU3_REG;
  assign n129 = n209 | n214;
  assign n216 = REQUEST1 & ~STATO_REG_0_;
  assign n134 = n99 | n216;
  assign n218 = STATO_REG_0_ & RU4_REG;
  assign n219 = REQUEST4 & ~STATO_REG_0_;
  assign n139 = n218 | n219;
  assign n221 = STATO_REG_0_ & RU2_REG;
  assign n222 = ~STATO_REG_0_ & FU2_REG;
  assign n144 = n221 | n222;
  assign n224 = ~STATO_REG_0_ & FU4_REG;
  assign n149 = n218 | n224;
  assign n226 = REQUEST2 & ~STATO_REG_0_;
  assign n154 = n221 | n226;
  assign n18 = ~STATO_REG_0_;
  assign n158 = STATO_REG_0_;
  always @ (posedge clock) begin
    STATO_REG_0_ <= n18;
    CODA0_REG_2_ <= n23;
    CODA0_REG_1_ <= n28;
    CODA0_REG_0_ <= n33;
    CODA1_REG_2_ <= n38;
    CODA1_REG_1_ <= n43;
    CODA1_REG_0_ <= n48;
    CODA2_REG_2_ <= n53;
    CODA2_REG_1_ <= n58;
    CODA2_REG_0_ <= n63;
    CODA3_REG_2_ <= n68;
    CODA3_REG_1_ <= n73;
    CODA3_REG_0_ <= n78;
    GRANT_REG_3_ <= n83;
    GRANT_REG_2_ <= n88;
    GRANT_REG_1_ <= n93;
    GRANT_REG_0_ <= n98;
    GRANT_O_REG_3_ <= n103;
    GRANT_O_REG_2_ <= n107;
    GRANT_O_REG_1_ <= n111;
    GRANT_O_REG_0_ <= n115;
    RU3_REG <= n119;
    FU1_REG <= n124;
    FU3_REG <= n129;
    RU1_REG <= n134;
    RU4_REG <= n139;
    FU2_REG <= n144;
    FU4_REG <= n149;
    RU2_REG <= n154;
    STATO_REG_1_ <= n158;
  end
endmodule


