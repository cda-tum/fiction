// Benchmark "b10" written by ABC on Wed Sep  5 10:17:20 2018

module b10 ( clock, 
    R_BUTTON, G_BUTTON, KEY, START, TEST, RTS, RTR, V_IN_3_, V_IN_2_,
    V_IN_1_, V_IN_0_,
    CTS_REG, CTR_REG, V_OUT_REG_3_, V_OUT_REG_2_, V_OUT_REG_1_,
    V_OUT_REG_0_  );
  input  clock;
  input  R_BUTTON, G_BUTTON, KEY, START, TEST, RTS, RTR, V_IN_3_,
    V_IN_2_, V_IN_1_, V_IN_0_;
  output CTS_REG, CTR_REG, V_OUT_REG_3_, V_OUT_REG_2_, V_OUT_REG_1_,
    V_OUT_REG_0_;
  reg VOTO0_REG, STATO_REG_3_, STATO_REG_2_, STATO_REG_1_, STATO_REG_0_,
    V_OUT_REG_3_, V_OUT_REG_2_, V_OUT_REG_1_, V_OUT_REG_0_, SIGN_REG_3_,
    VOTO1_REG, CTR_REG, VOTO3_REG, LAST_R_REG, CTS_REG, VOTO2_REG,
    LAST_G_REG;
  wire n69_1, n70, n71, n72, n73_1, n74, n75, n76, n77_1, n78, n79, n80, n81,
    n82_1, n83, n84, n85, n86, n87_1, n88, n89, n90, n91_1, n92, n93, n94,
    n95, n96_1, n97, n99, n100, n101_1, n102, n103, n104, n105_1, n106,
    n107, n108, n109, n110_1, n111, n112, n113, n114, n115, n116, n117,
    n118, n119, n120, n121, n122, n123, n124, n125, n126, n127, n128, n129,
    n130, n131, n132, n133, n134, n136, n137, n138, n139, n140, n141, n143,
    n144, n145, n146, n147, n148, n150, n151, n152, n153, n154, n155, n156,
    n158, n159, n161, n162, n164, n165, n167, n168, n170, n171, n172, n173,
    n175, n176, n177, n178, n179, n180, n181, n182, n183, n184, n185, n186,
    n187, n188, n189, n191, n192, n193, n194, n195, n196, n197, n198, n200,
    n201, n202, n203, n204, n205, n206, n207, n208, n209, n210, n211, n212,
    n213, n214, n216, n217, n218, n219, n221, n222, n223, n224, n225, n226,
    n227, n228, n229, n230, n231, n232, n233, n235, n236, n237, n238, n239,
    n240, n241, n242, n243, n244, n246, n247, n36, n41, n46, n51, n56, n61,
    n65, n69, n73, n77, n82, n87, n91, n96, n101, n105, n110;
  assign n69_1 = ~STATO_REG_1_ & STATO_REG_0_;
  assign n70 = STATO_REG_3_ & ~STATO_REG_2_;
  assign n71 = n69_1 & n70;
  assign n72 = STATO_REG_2_ & ~STATO_REG_0_;
  assign n73_1 = STATO_REG_1_ & n72;
  assign n74 = ~STATO_REG_3_ & n73_1;
  assign n75 = ~n71 & ~n74;
  assign n76 = V_IN_0_ & ~n75;
  assign n77_1 = STATO_REG_3_ & ~SIGN_REG_3_;
  assign n78 = ~STATO_REG_3_ & ~STATO_REG_2_;
  assign n79 = ~STATO_REG_0_ & n78;
  assign n80 = KEY & n79;
  assign n81 = ~n77_1 & ~n80;
  assign n82_1 = STATO_REG_1_ & ~n81;
  assign n83 = ~n76 & ~n82_1;
  assign n84 = ~STATO_REG_2_ & STATO_REG_1_;
  assign n85 = ~STATO_REG_3_ & STATO_REG_0_;
  assign n86 = STATO_REG_3_ & ~STATO_REG_0_;
  assign n87_1 = ~n85 & ~n86;
  assign n88 = n84 & ~n87_1;
  assign n89 = START & n78;
  assign n90 = ~STATO_REG_1_ & n79;
  assign n91_1 = RTS & n73_1;
  assign n92 = ~n90 & ~n91_1;
  assign n93 = ~n71 & n92;
  assign n94 = ~n88 & ~n89;
  assign n95 = n93 & n94;
  assign n96_1 = ~n83 & ~n95;
  assign n97 = VOTO0_REG & n95;
  assign n36 = n96_1 | n97;
  assign n99 = RTR & ~STATO_REG_1_;
  assign n100 = n72 & n99;
  assign n101_1 = ~RTS & STATO_REG_2_;
  assign n102 = n69_1 & n101_1;
  assign n103 = ~RTR & ~STATO_REG_1_;
  assign n104 = ~n84 & ~n103;
  assign n105_1 = ~STATO_REG_0_ & n104;
  assign n106 = V_IN_3_ & V_IN_2_;
  assign n107 = V_IN_0_ & n106;
  assign n108 = V_IN_1_ & n107;
  assign n109 = STATO_REG_0_ & ~n108;
  assign n110_1 = ~n105_1 & ~n109;
  assign n111 = STATO_REG_3_ & n110_1;
  assign n112 = STATO_REG_1_ & STATO_REG_0_;
  assign n113 = ~RTR & n112;
  assign n114 = STATO_REG_2_ & n113;
  assign n115 = ~STATO_REG_3_ & STATO_REG_1_;
  assign n116 = ~START & n115;
  assign n117 = ~STATO_REG_2_ & n116;
  assign n118 = STATO_REG_0_ & n89;
  assign n119 = ~n114 & ~n117;
  assign n120 = ~n118 & n119;
  assign n121 = ~n100 & ~n102;
  assign n122 = ~n111 & n121;
  assign n123 = n120 & n122;
  assign n124 = n92 & n123;
  assign n125 = ~STATO_REG_0_ & ~n124;
  assign n126 = STATO_REG_3_ & ~n125;
  assign n127 = ~TEST & n79;
  assign n128 = VOTO1_REG & VOTO2_REG;
  assign n129 = ~VOTO3_REG & n128;
  assign n130 = ~VOTO0_REG & n129;
  assign n131 = n72 & n130;
  assign n132 = ~n127 & ~n131;
  assign n133 = ~STATO_REG_1_ & ~n132;
  assign n134 = ~n124 & n133;
  assign n41 = n126 | n134;
  assign n136 = STATO_REG_1_ & ~n87_1;
  assign n137 = ~STATO_REG_0_ & ~n130;
  assign n138 = ~n69_1 & ~n137;
  assign n139 = ~n124 & n138;
  assign n140 = STATO_REG_2_ & ~n139;
  assign n141 = ~n73_1 & ~n136;
  assign n46 = n140 | ~n141;
  assign n143 = STATO_REG_1_ & n124;
  assign n144 = ~STATO_REG_0_ & n115;
  assign n145 = STATO_REG_0_ & ~n124;
  assign n146 = ~n115 & n145;
  assign n147 = ~n73_1 & ~n143;
  assign n148 = ~n144 & n147;
  assign n51 = n146 | ~n148;
  assign n150 = ~STATO_REG_3_ & n130;
  assign n151 = ~STATO_REG_1_ & ~STATO_REG_0_;
  assign n152 = ~n150 & n151;
  assign n153 = ~n73_1 & ~n79;
  assign n154 = ~n152 & n153;
  assign n155 = ~n124 & ~n154;
  assign n156 = STATO_REG_0_ & n124;
  assign n56 = n155 | n156;
  assign n158 = VOTO3_REG & n100;
  assign n159 = V_OUT_REG_3_ & ~n100;
  assign n61 = n158 | n159;
  assign n161 = VOTO2_REG & n100;
  assign n162 = V_OUT_REG_2_ & ~n100;
  assign n65 = n161 | n162;
  assign n164 = VOTO1_REG & n100;
  assign n165 = V_OUT_REG_1_ & ~n100;
  assign n69 = n164 | n165;
  assign n167 = VOTO0_REG & n100;
  assign n168 = V_OUT_REG_0_ & ~n100;
  assign n73 = n167 | n168;
  assign n170 = STATO_REG_3_ & STATO_REG_0_;
  assign n171 = ~TEST & ~STATO_REG_1_;
  assign n172 = n79 & n171;
  assign n173 = SIGN_REG_3_ & ~n172;
  assign n77 = n170 | n173;
  assign n175 = V_IN_1_ & ~n75;
  assign n176 = KEY & ~VOTO1_REG;
  assign n177 = n84 & n176;
  assign n178 = ~n175 & ~n177;
  assign n179 = START & n79;
  assign n180 = G_BUTTON & ~LAST_G_REG;
  assign n181 = n179 & n180;
  assign n182 = ~KEY & n179;
  assign n183 = START & ~STATO_REG_2_;
  assign n184 = n69_1 & n183;
  assign n185 = n93 & ~n184;
  assign n186 = ~n181 & ~n182;
  assign n187 = n185 & n186;
  assign n188 = ~n178 & ~n187;
  assign n189 = VOTO1_REG & n187;
  assign n82 = n188 | n189;
  assign n191 = ~RTS & STATO_REG_1_;
  assign n192 = STATO_REG_3_ & ~STATO_REG_1_;
  assign n193 = STATO_REG_2_ & ~STATO_REG_1_;
  assign n194 = ~n84 & ~n193;
  assign n195 = ~n191 & ~n192;
  assign n196 = ~STATO_REG_0_ & n195;
  assign n197 = n194 & n196;
  assign n198 = CTR_REG & ~n197;
  assign n87 = n102 | n198;
  assign n200 = V_IN_3_ & ~n75;
  assign n201 = VOTO0_REG & ~VOTO2_REG;
  assign n202 = ~VOTO0_REG & VOTO2_REG;
  assign n203 = ~n201 & ~n202;
  assign n204 = ~VOTO1_REG & ~n203;
  assign n205 = VOTO1_REG & n203;
  assign n206 = ~n204 & ~n205;
  assign n207 = n112 & ~n206;
  assign n208 = ~n200 & ~n207;
  assign n209 = ~KEY & START;
  assign n210 = ~n112 & ~n209;
  assign n211 = n78 & ~n210;
  assign n212 = n185 & ~n211;
  assign n213 = ~n208 & ~n212;
  assign n214 = VOTO3_REG & n212;
  assign n91 = n213 | n214;
  assign n216 = KEY & STATO_REG_1_;
  assign n217 = n179 & n216;
  assign n218 = R_BUTTON & n217;
  assign n219 = LAST_R_REG & ~n217;
  assign n96 = n218 | n219;
  assign n221 = n72 & n192;
  assign n222 = RTR & n78;
  assign n223 = n69_1 & n222;
  assign n224 = STATO_REG_1_ & ~STATO_REG_0_;
  assign n225 = STATO_REG_3_ & n69_1;
  assign n226 = ~STATO_REG_3_ & ~STATO_REG_0_;
  assign n227 = RTR & ~n226;
  assign n228 = n194 & ~n224;
  assign n229 = ~n225 & n228;
  assign n230 = ~n227 & n229;
  assign n231 = CTS_REG & ~n230;
  assign n232 = ~n100 & ~n221;
  assign n233 = ~n223 & n232;
  assign n101 = n231 | ~n233;
  assign n235 = V_IN_2_ & ~n75;
  assign n236 = KEY & ~VOTO2_REG;
  assign n237 = n84 & n236;
  assign n238 = ~n235 & ~n237;
  assign n239 = R_BUTTON & ~LAST_R_REG;
  assign n240 = n179 & n239;
  assign n241 = ~n182 & ~n240;
  assign n242 = n185 & n241;
  assign n243 = ~n238 & ~n242;
  assign n244 = VOTO2_REG & n242;
  assign n105 = n243 | n244;
  assign n246 = G_BUTTON & n217;
  assign n247 = LAST_G_REG & ~n217;
  assign n110 = n246 | n247;
  always @ (posedge clock) begin
    VOTO0_REG <= n36;
    STATO_REG_3_ <= n41;
    STATO_REG_2_ <= n46;
    STATO_REG_1_ <= n51;
    STATO_REG_0_ <= n56;
    V_OUT_REG_3_ <= n61;
    V_OUT_REG_2_ <= n65;
    V_OUT_REG_1_ <= n69;
    V_OUT_REG_0_ <= n73;
    SIGN_REG_3_ <= n77;
    VOTO1_REG <= n82;
    CTR_REG <= n87;
    VOTO3_REG <= n91;
    LAST_R_REG <= n96;
    CTS_REG <= n101;
    VOTO2_REG <= n105;
    LAST_G_REG <= n110;
  end
endmodule


