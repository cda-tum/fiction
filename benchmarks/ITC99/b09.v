// Benchmark "b09" written by ABC on Wed Sep  5 10:17:20 2018

module b09 ( clock, 
    X,
    Y_REG  );
  input  clock;
  input  X;
  output Y_REG;
  reg D_IN_REG_0_, D_OUT_REG_7_, D_OUT_REG_6_, D_OUT_REG_5_, D_OUT_REG_4_,
    D_OUT_REG_3_, D_OUT_REG_2_, D_OUT_REG_1_, D_OUT_REG_0_, OLD_REG_7_,
    OLD_REG_6_, OLD_REG_5_, OLD_REG_4_, OLD_REG_3_, OLD_REG_2_, OLD_REG_1_,
    OLD_REG_0_, Y_REG, STATO_REG_1_, STATO_REG_0_, D_IN_REG_8_,
    D_IN_REG_7_, D_IN_REG_6_, D_IN_REG_5_, D_IN_REG_4_, D_IN_REG_3_,
    D_IN_REG_2_, D_IN_REG_1_;
  wire n87, n88, n89, n90, n91_1, n92, n93, n95_1, n96, n97, n98, n99,
    n100_1, n101, n102, n103, n104, n105_1, n106, n107, n108, n109, n110_1,
    n111, n112, n113, n114, n115_1, n116, n117, n118, n119, n120_1, n121,
    n122, n123, n124, n125_1, n126, n127, n128, n129, n130_1, n131, n132,
    n133, n134, n136, n137, n138, n139, n140_1, n142, n143, n144, n145,
    n147, n148, n149, n150, n152, n153, n154, n155, n157, n158, n159, n160,
    n162, n163, n164, n165, n167, n168, n169, n170, n172, n173, n174, n175,
    n177, n178, n180, n181, n183, n184, n186, n187, n189, n190, n192, n193,
    n195, n196, n198, n199, n200, n201, n202, n205, n206, n207, n208, n210,
    n211, n212, n213, n214, n6, n11, n16, n21, n26, n31, n36, n41, n46,
    n51, n56, n61, n66, n71, n76, n81, n86, n91, n95, n100, n105, n110,
    n115, n120, n125, n130, n135, n140;
  assign n87 = STATO_REG_1_ & ~STATO_REG_0_;
  assign n88 = STATO_REG_1_ & STATO_REG_0_;
  assign n89 = ~D_IN_REG_0_ & n88;
  assign n90 = ~n87 & ~n89;
  assign n91_1 = ~STATO_REG_1_ & STATO_REG_0_;
  assign n92 = ~D_IN_REG_0_ & n91_1;
  assign n93 = n90 & ~n92;
  assign n6 = D_IN_REG_1_ & ~n93;
  assign n95_1 = ~D_IN_REG_0_ & ~STATO_REG_0_;
  assign n96 = D_IN_REG_0_ & ~STATO_REG_1_;
  assign n97 = D_IN_REG_0_ & STATO_REG_0_;
  assign n98 = OLD_REG_6_ & ~D_IN_REG_7_;
  assign n99 = ~OLD_REG_6_ & D_IN_REG_7_;
  assign n100_1 = OLD_REG_7_ & ~D_IN_REG_8_;
  assign n101 = ~OLD_REG_7_ & D_IN_REG_8_;
  assign n102 = ~n98 & ~n99;
  assign n103 = ~n100_1 & n102;
  assign n104 = ~n101 & n103;
  assign n105_1 = OLD_REG_5_ & ~D_IN_REG_6_;
  assign n106 = ~OLD_REG_5_ & D_IN_REG_6_;
  assign n107 = OLD_REG_4_ & ~D_IN_REG_5_;
  assign n108 = ~OLD_REG_4_ & D_IN_REG_5_;
  assign n109 = ~n105_1 & ~n106;
  assign n110_1 = ~n107 & n109;
  assign n111 = ~n108 & n110_1;
  assign n112 = OLD_REG_3_ & ~D_IN_REG_4_;
  assign n113 = ~OLD_REG_3_ & D_IN_REG_4_;
  assign n114 = OLD_REG_2_ & ~D_IN_REG_3_;
  assign n115_1 = ~OLD_REG_2_ & D_IN_REG_3_;
  assign n116 = ~n112 & ~n113;
  assign n117 = ~n114 & n116;
  assign n118 = ~n115_1 & n117;
  assign n119 = OLD_REG_1_ & ~D_IN_REG_2_;
  assign n120_1 = ~OLD_REG_1_ & D_IN_REG_2_;
  assign n121 = OLD_REG_0_ & ~D_IN_REG_1_;
  assign n122 = ~OLD_REG_0_ & D_IN_REG_1_;
  assign n123 = ~n119 & ~n120_1;
  assign n124 = ~n121 & n123;
  assign n125_1 = ~n122 & n124;
  assign n126 = n104 & n111;
  assign n127 = n118 & n126;
  assign n128 = n125_1 & n127;
  assign n129 = n97 & ~n128;
  assign n130_1 = ~n95_1 & ~n96;
  assign n131 = ~n129 & n130_1;
  assign n132 = D_OUT_REG_7_ & n131;
  assign n133 = STATO_REG_0_ & ~n131;
  assign n134 = D_IN_REG_8_ & n133;
  assign n11 = n132 | n134;
  assign n136 = D_IN_REG_7_ & n133;
  assign n137 = n87 & ~n131;
  assign n138 = D_OUT_REG_7_ & n137;
  assign n139 = D_OUT_REG_6_ & n131;
  assign n140_1 = ~n136 & ~n138;
  assign n16 = n139 | ~n140_1;
  assign n142 = D_IN_REG_6_ & n133;
  assign n143 = D_OUT_REG_6_ & n137;
  assign n144 = D_OUT_REG_5_ & n131;
  assign n145 = ~n142 & ~n143;
  assign n21 = n144 | ~n145;
  assign n147 = D_IN_REG_5_ & n133;
  assign n148 = D_OUT_REG_5_ & n137;
  assign n149 = D_OUT_REG_4_ & n131;
  assign n150 = ~n147 & ~n148;
  assign n26 = n149 | ~n150;
  assign n152 = D_IN_REG_4_ & n133;
  assign n153 = D_OUT_REG_4_ & n137;
  assign n154 = D_OUT_REG_3_ & n131;
  assign n155 = ~n152 & ~n153;
  assign n31 = n154 | ~n155;
  assign n157 = D_IN_REG_3_ & n133;
  assign n158 = D_OUT_REG_3_ & n137;
  assign n159 = D_OUT_REG_2_ & n131;
  assign n160 = ~n157 & ~n158;
  assign n36 = n159 | ~n160;
  assign n162 = D_IN_REG_2_ & n133;
  assign n163 = D_OUT_REG_2_ & n137;
  assign n164 = D_OUT_REG_1_ & n131;
  assign n165 = ~n162 & ~n163;
  assign n41 = n164 | ~n165;
  assign n167 = D_IN_REG_1_ & n133;
  assign n168 = D_OUT_REG_1_ & n137;
  assign n169 = D_OUT_REG_0_ & n131;
  assign n170 = ~n167 & ~n168;
  assign n46 = n169 | ~n170;
  assign n172 = ~D_IN_REG_0_ & STATO_REG_0_;
  assign n173 = ~n87 & ~n172;
  assign n174 = OLD_REG_7_ & ~n173;
  assign n175 = D_IN_REG_8_ & n97;
  assign n51 = n174 | n175;
  assign n177 = OLD_REG_6_ & ~n173;
  assign n178 = D_IN_REG_7_ & n97;
  assign n56 = n177 | n178;
  assign n180 = OLD_REG_5_ & ~n173;
  assign n181 = D_IN_REG_6_ & n97;
  assign n61 = n180 | n181;
  assign n183 = OLD_REG_4_ & ~n173;
  assign n184 = D_IN_REG_5_ & n97;
  assign n66 = n183 | n184;
  assign n186 = OLD_REG_3_ & ~n173;
  assign n187 = D_IN_REG_4_ & n97;
  assign n71 = n186 | n187;
  assign n189 = OLD_REG_2_ & ~n173;
  assign n190 = D_IN_REG_3_ & n97;
  assign n76 = n189 | n190;
  assign n192 = OLD_REG_1_ & ~n173;
  assign n193 = D_IN_REG_2_ & n97;
  assign n81 = n192 | n193;
  assign n195 = OLD_REG_0_ & ~n173;
  assign n196 = D_IN_REG_1_ & n97;
  assign n86 = n195 | n196;
  assign n198 = ~D_IN_REG_0_ & D_OUT_REG_0_;
  assign n199 = n87 & n198;
  assign n200 = ~D_IN_REG_0_ & ~Y_REG;
  assign n201 = n91_1 & ~n200;
  assign n202 = ~n199 & ~n201;
  assign n91 = n129 | ~n202;
  assign n95 = STATO_REG_1_ | n97;
  assign n205 = ~D_IN_REG_0_ & STATO_REG_1_;
  assign n206 = ~STATO_REG_0_ & ~n205;
  assign n207 = n88 & n128;
  assign n208 = ~n172 & ~n206;
  assign n100 = n207 | ~n208;
  assign n210 = n88 & ~n128;
  assign n211 = ~n91_1 & ~n210;
  assign n212 = D_IN_REG_0_ & ~n211;
  assign n213 = n90 & ~n91_1;
  assign n214 = X & ~n213;
  assign n105 = n212 | n214;
  assign n110 = D_IN_REG_8_ & ~n93;
  assign n115 = D_IN_REG_7_ & ~n93;
  assign n120 = D_IN_REG_6_ & ~n93;
  assign n125 = D_IN_REG_5_ & ~n93;
  assign n130 = D_IN_REG_4_ & ~n93;
  assign n135 = D_IN_REG_3_ & ~n93;
  assign n140 = D_IN_REG_2_ & ~n93;
  always @ (posedge clock) begin
    D_IN_REG_0_ <= n6;
    D_OUT_REG_7_ <= n11;
    D_OUT_REG_6_ <= n16;
    D_OUT_REG_5_ <= n21;
    D_OUT_REG_4_ <= n26;
    D_OUT_REG_3_ <= n31;
    D_OUT_REG_2_ <= n36;
    D_OUT_REG_1_ <= n41;
    D_OUT_REG_0_ <= n46;
    OLD_REG_7_ <= n51;
    OLD_REG_6_ <= n56;
    OLD_REG_5_ <= n61;
    OLD_REG_4_ <= n66;
    OLD_REG_3_ <= n71;
    OLD_REG_2_ <= n76;
    OLD_REG_1_ <= n81;
    OLD_REG_0_ <= n86;
    Y_REG <= n91;
    STATO_REG_1_ <= n95;
    STATO_REG_0_ <= n100;
    D_IN_REG_8_ <= n105;
    D_IN_REG_7_ <= n110;
    D_IN_REG_6_ <= n115;
    D_IN_REG_5_ <= n120;
    D_IN_REG_4_ <= n125;
    D_IN_REG_3_ <= n130;
    D_IN_REG_2_ <= n135;
    D_IN_REG_1_ <= n140;
  end
endmodule


