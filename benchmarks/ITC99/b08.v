// Benchmark "b08" written by ABC on Wed Sep  5 10:17:20 2018

module b08 ( clock, 
    START, I_7_, I_6_, I_5_, I_4_, I_3_, I_2_, I_1_, I_0_,
    O_REG_3_, O_REG_2_, O_REG_1_, O_REG_0_  );
  input  clock;
  input  START, I_7_, I_6_, I_5_, I_4_, I_3_, I_2_, I_1_, I_0_;
  output O_REG_3_, O_REG_2_, O_REG_1_, O_REG_0_;
  reg O_REG_0_, STATO_REG_1_, STATO_REG_0_, MAR_REG_2_, MAR_REG_1_,
    MAR_REG_0_, IN_R_REG_7_, IN_R_REG_6_, IN_R_REG_5_, IN_R_REG_4_,
    IN_R_REG_3_, IN_R_REG_2_, IN_R_REG_1_, IN_R_REG_0_, OUT_R_REG_3_,
    OUT_R_REG_2_, OUT_R_REG_1_, OUT_R_REG_0_, O_REG_3_, O_REG_2_, O_REG_1_;
  wire n77_1, n78, n79, n80, n81, n82_1, n83, n85, n86, n87_1, n88, n90, n91,
    n92_1, n94, n95, n96, n97_1, n99, n100, n101, n102_1, n103, n105, n106,
    n108, n109, n111, n112_1, n114, n115, n117_1, n118, n120, n121_1, n123,
    n124, n126, n127, n129, n130, n132, n133, n134, n135, n136, n137, n138,
    n139, n140, n141, n142, n143, n144, n145, n146, n147, n148, n149, n150,
    n151, n152, n153, n154, n155, n156, n157, n158, n159, n160, n161, n162,
    n163, n164, n165, n166, n167, n168, n169, n170, n171, n172, n173, n174,
    n175, n176, n177, n178, n179, n180, n181, n182, n183, n184, n185, n186,
    n187, n188, n189, n190, n191, n192, n193, n194, n195, n196, n197, n198,
    n199, n200, n201, n202, n203, n204, n205, n206, n207, n208, n209, n211,
    n212, n214, n215, n216, n217, n218, n219, n221, n223, n224, n226, n227,
    n229, n230, n28, n32, n37, n42, n47, n52, n57, n62, n67, n72, n77, n82,
    n87, n92, n97, n102, n107, n112, n117, n121, n125;
  assign n77_1 = MAR_REG_1_ & MAR_REG_0_;
  assign n78 = MAR_REG_2_ & n77_1;
  assign n79 = ~START & n78;
  assign n80 = STATO_REG_1_ & STATO_REG_0_;
  assign n81 = n79 & n80;
  assign n82_1 = OUT_R_REG_0_ & n81;
  assign n83 = O_REG_0_ & ~n81;
  assign n28 = n82_1 | n83;
  assign n85 = ~STATO_REG_1_ & STATO_REG_0_;
  assign n86 = STATO_REG_1_ & ~STATO_REG_0_;
  assign n87_1 = STATO_REG_0_ & ~n79;
  assign n88 = ~n85 & ~n86;
  assign n32 = n87_1 | ~n88;
  assign n90 = STATO_REG_1_ & n78;
  assign n91 = STATO_REG_0_ & ~n90;
  assign n92_1 = START & ~n91;
  assign n37 = n86 | n92_1;
  assign n94 = ~MAR_REG_2_ & n77_1;
  assign n95 = n80 & n94;
  assign n96 = ~STATO_REG_1_ & n91;
  assign n97_1 = MAR_REG_2_ & ~n96;
  assign n42 = n95 | n97_1;
  assign n99 = MAR_REG_0_ & n80;
  assign n100 = ~MAR_REG_1_ & n99;
  assign n101 = STATO_REG_1_ & ~MAR_REG_0_;
  assign n102_1 = n91 & ~n101;
  assign n103 = MAR_REG_1_ & ~n102_1;
  assign n47 = n100 | n103;
  assign n105 = MAR_REG_0_ & ~n91;
  assign n106 = ~MAR_REG_0_ & n80;
  assign n52 = n105 | n106;
  assign n108 = I_7_ & n85;
  assign n109 = IN_R_REG_7_ & ~n85;
  assign n57 = n108 | n109;
  assign n111 = I_6_ & n85;
  assign n112_1 = IN_R_REG_6_ & ~n85;
  assign n62 = n111 | n112_1;
  assign n114 = I_5_ & n85;
  assign n115 = IN_R_REG_5_ & ~n85;
  assign n67 = n114 | n115;
  assign n117_1 = I_4_ & n85;
  assign n118 = IN_R_REG_4_ & ~n85;
  assign n72 = n117_1 | n118;
  assign n120 = I_3_ & n85;
  assign n121_1 = IN_R_REG_3_ & ~n85;
  assign n77 = n120 | n121_1;
  assign n123 = I_2_ & n85;
  assign n124 = IN_R_REG_2_ & ~n85;
  assign n82 = n123 | n124;
  assign n126 = I_1_ & n85;
  assign n127 = IN_R_REG_1_ & ~n85;
  assign n87 = n126 | n127;
  assign n129 = I_0_ & n85;
  assign n130 = IN_R_REG_0_ & ~n85;
  assign n92 = n129 | n130;
  assign n132 = ~MAR_REG_2_ & ~MAR_REG_1_;
  assign n133 = MAR_REG_0_ & n132;
  assign n134 = ~MAR_REG_1_ & ~MAR_REG_0_;
  assign n135 = ~MAR_REG_2_ & n134;
  assign n136 = ~MAR_REG_2_ & ~MAR_REG_0_;
  assign n137 = MAR_REG_1_ & n136;
  assign n138 = ~n78 & ~n137;
  assign n139 = MAR_REG_1_ & ~MAR_REG_0_;
  assign n140 = MAR_REG_2_ & n139;
  assign n141 = n138 & ~n140;
  assign n142 = ~MAR_REG_1_ & MAR_REG_0_;
  assign n143 = MAR_REG_2_ & n142;
  assign n144 = MAR_REG_2_ & n134;
  assign n145 = ~n143 & ~n144;
  assign n146 = n141 & n145;
  assign n147 = ~IN_R_REG_4_ & ~n146;
  assign n148 = ~n94 & n145;
  assign n149 = IN_R_REG_4_ & ~n148;
  assign n150 = ~n133 & ~n135;
  assign n151 = ~n147 & n150;
  assign n152 = ~n149 & n151;
  assign n153 = n138 & ~n144;
  assign n154 = ~n133 & n153;
  assign n155 = ~n94 & n154;
  assign n156 = ~IN_R_REG_6_ & ~n155;
  assign n157 = ~n135 & n148;
  assign n158 = IN_R_REG_6_ & ~n157;
  assign n159 = ~n156 & ~n158;
  assign n160 = ~n140 & n159;
  assign n161 = ~n140 & n154;
  assign n162 = ~IN_R_REG_2_ & ~n161;
  assign n163 = ~n78 & n148;
  assign n164 = IN_R_REG_2_ & ~n163;
  assign n165 = ~n162 & ~n164;
  assign n166 = ~n135 & n165;
  assign n167 = n138 & ~n143;
  assign n168 = ~n94 & n167;
  assign n169 = ~IN_R_REG_3_ & n168;
  assign n170 = ~n133 & n138;
  assign n171 = n148 & n170;
  assign n172 = ~n135 & n171;
  assign n173 = IN_R_REG_5_ & n172;
  assign n174 = n141 & n148;
  assign n175 = ~IN_R_REG_5_ & n174;
  assign n176 = ~n137 & ~n140;
  assign n177 = n148 & n176;
  assign n178 = IN_R_REG_7_ & n177;
  assign n179 = ~n135 & ~n143;
  assign n180 = n154 & n179;
  assign n181 = ~IN_R_REG_7_ & n180;
  assign n182 = ~n173 & ~n175;
  assign n183 = ~n178 & n182;
  assign n184 = ~n181 & n183;
  assign n185 = ~n94 & n141;
  assign n186 = ~IN_R_REG_0_ & ~n185;
  assign n187 = ~n133 & n163;
  assign n188 = IN_R_REG_0_ & ~n187;
  assign n189 = ~n186 & ~n188;
  assign n190 = ~n135 & n189;
  assign n191 = ~n140 & n163;
  assign n192 = IN_R_REG_1_ & ~n191;
  assign n193 = ~IN_R_REG_1_ & ~n171;
  assign n194 = ~n192 & ~n193;
  assign n195 = ~n135 & n194;
  assign n196 = n86 & ~n169;
  assign n197 = n184 & n196;
  assign n198 = ~n190 & n197;
  assign n199 = ~n195 & n198;
  assign n200 = ~n152 & ~n160;
  assign n201 = ~n166 & n200;
  assign n202 = n199 & n201;
  assign n203 = ~n85 & ~n202;
  assign n204 = STATO_REG_1_ & ~n203;
  assign n205 = ~n157 & n204;
  assign n206 = ~n176 & n204;
  assign n207 = ~STATO_REG_1_ & ~n203;
  assign n208 = OUT_R_REG_3_ & ~n207;
  assign n209 = ~n205 & ~n206;
  assign n97 = n208 | ~n209;
  assign n211 = OUT_R_REG_2_ & ~n207;
  assign n212 = ~n153 & n204;
  assign n102 = n211 | n212;
  assign n214 = ~n133 & ~n144;
  assign n215 = ~n135 & n214;
  assign n216 = ~n94 & n215;
  assign n217 = n204 & ~n216;
  assign n218 = OUT_R_REG_1_ & ~n207;
  assign n219 = ~n206 & ~n217;
  assign n107 = n218 | ~n219;
  assign n221 = OUT_R_REG_0_ & ~n207;
  assign n112 = n206 | n221;
  assign n223 = OUT_R_REG_3_ & n81;
  assign n224 = O_REG_3_ & ~n81;
  assign n117 = n223 | n224;
  assign n226 = OUT_R_REG_2_ & n81;
  assign n227 = O_REG_2_ & ~n81;
  assign n121 = n226 | n227;
  assign n229 = OUT_R_REG_1_ & n81;
  assign n230 = O_REG_1_ & ~n81;
  assign n125 = n229 | n230;
  always @ (posedge clock) begin
    O_REG_0_ <= n28;
    STATO_REG_1_ <= n32;
    STATO_REG_0_ <= n37;
    MAR_REG_2_ <= n42;
    MAR_REG_1_ <= n47;
    MAR_REG_0_ <= n52;
    IN_R_REG_7_ <= n57;
    IN_R_REG_6_ <= n62;
    IN_R_REG_5_ <= n67;
    IN_R_REG_4_ <= n72;
    IN_R_REG_3_ <= n77;
    IN_R_REG_2_ <= n82;
    IN_R_REG_1_ <= n87;
    IN_R_REG_0_ <= n92;
    OUT_R_REG_3_ <= n97;
    OUT_R_REG_2_ <= n102;
    OUT_R_REG_1_ <= n107;
    OUT_R_REG_0_ <= n112;
    O_REG_3_ <= n117;
    O_REG_2_ <= n121;
    O_REG_1_ <= n125;
  end
endmodule


