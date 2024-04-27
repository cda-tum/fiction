// Benchmark "top" written by ABC on Mon Feb 19 11:52:41 2024

module top ( 
    i_5_, i_6_, i_3_, i_4_, i_1_, i_2_, i_0_,
    o_1_, o_2_, o_0_, o_9_, o_7_, o_8_, o_5_, o_6_, o_3_, o_4_  );
  input  i_5_, i_6_, i_3_, i_4_, i_1_, i_2_, i_0_;
  output o_1_, o_2_, o_0_, o_9_, o_7_, o_8_, o_5_, o_6_, o_3_, o_4_;
  wire new_n18, new_n19, new_n20, new_n21, new_n22, new_n23, new_n24,
    new_n25, new_n26, new_n27, new_n28, new_n29, new_n30, new_n31, new_n32,
    new_n33, new_n34, new_n35, new_n36, new_n37, new_n38, new_n39, new_n40,
    new_n41, new_n42, new_n43, new_n44, new_n45, new_n46, new_n47, new_n48,
    new_n49, new_n50, new_n51, new_n52, new_n53, new_n54, new_n55, new_n56,
    new_n57, new_n58, new_n59, new_n61, new_n62, new_n63, new_n64, new_n65,
    new_n66, new_n67, new_n68, new_n69, new_n70, new_n71, new_n72, new_n73,
    new_n74, new_n75, new_n76, new_n77, new_n78, new_n79, new_n80, new_n81,
    new_n82, new_n83, new_n84, new_n85, new_n86, new_n87, new_n88, new_n89,
    new_n90, new_n91, new_n92, new_n93, new_n94, new_n95, new_n96, new_n97,
    new_n98, new_n99, new_n100, new_n101, new_n102, new_n103, new_n104,
    new_n105, new_n106, new_n107, new_n108, new_n109, new_n110, new_n111,
    new_n112, new_n113, new_n114, new_n115, new_n116, new_n117, new_n118,
    new_n119, new_n120, new_n121, new_n123, new_n124, new_n125, new_n126,
    new_n127, new_n128, new_n129, new_n130, new_n131, new_n132, new_n133,
    new_n134, new_n135, new_n136, new_n137, new_n139, new_n140, new_n141,
    new_n143, new_n144, new_n146, new_n147, new_n148, new_n149, new_n150,
    new_n151, new_n152, new_n153, new_n154, new_n155, new_n156, new_n158,
    new_n159, new_n160, new_n162, new_n163, new_n164, new_n165, new_n166,
    new_n167, new_n168, new_n169, new_n170, new_n171, new_n172, new_n173,
    new_n174, new_n175, new_n176, new_n177, new_n178, new_n179, new_n180,
    new_n181, new_n182, new_n183, new_n184, new_n185, new_n186, new_n187,
    new_n188, new_n189, new_n190, new_n191, new_n192, new_n193, new_n194,
    new_n195, new_n196, new_n198, new_n199, new_n200, new_n201, new_n202,
    new_n203, new_n204, new_n205, new_n206, new_n207, new_n208, new_n209,
    new_n210, new_n211, new_n212, new_n213, new_n214, new_n215, new_n216,
    new_n217, new_n218, new_n219, new_n220;
  assign new_n18 = i_5_ & ~i_6_;
  assign new_n19 = ~i_4_ & new_n18;
  assign new_n20 = ~i_5_ & i_6_;
  assign new_n21 = i_4_ & new_n20;
  assign new_n22 = ~new_n19 & ~new_n21;
  assign new_n23 = i_5_ & ~i_1_;
  assign new_n24 = ~i_0_ & new_n23;
  assign new_n25 = ~i_6_ & new_n24;
  assign new_n26 = ~i_5_ & i_4_;
  assign new_n27 = i_3_ & i_1_;
  assign new_n28 = i_0_ & new_n27;
  assign new_n29 = new_n26 & new_n28;
  assign new_n30 = i_5_ & ~i_4_;
  assign new_n31 = ~i_3_ & ~i_1_;
  assign new_n32 = ~i_0_ & new_n31;
  assign new_n33 = new_n30 & new_n32;
  assign new_n34 = ~new_n25 & ~new_n29;
  assign new_n35 = ~new_n33 & new_n34;
  assign new_n36 = ~i_5_ & i_2_;
  assign new_n37 = i_0_ & new_n36;
  assign new_n38 = i_6_ & new_n37;
  assign new_n39 = ~i_5_ & i_1_;
  assign new_n40 = i_0_ & new_n39;
  assign new_n41 = i_6_ & new_n40;
  assign new_n42 = i_1_ & i_2_;
  assign new_n43 = i_0_ & new_n42;
  assign new_n44 = new_n26 & new_n43;
  assign new_n45 = ~new_n38 & ~new_n41;
  assign new_n46 = ~new_n44 & new_n45;
  assign new_n47 = i_5_ & ~i_3_;
  assign new_n48 = ~i_0_ & new_n47;
  assign new_n49 = ~i_6_ & new_n48;
  assign new_n50 = ~i_1_ & ~i_2_;
  assign new_n51 = ~i_0_ & new_n50;
  assign new_n52 = new_n30 & new_n51;
  assign new_n53 = i_5_ & ~i_2_;
  assign new_n54 = ~i_0_ & new_n53;
  assign new_n55 = ~i_6_ & new_n54;
  assign new_n56 = ~new_n49 & ~new_n52;
  assign new_n57 = ~new_n55 & new_n56;
  assign new_n58 = new_n35 & new_n46;
  assign new_n59 = new_n57 & new_n58;
  assign o_1_ = ~new_n22 | ~new_n59;
  assign new_n61 = ~i_6_ & ~i_4_;
  assign new_n62 = new_n28 & new_n61;
  assign new_n63 = new_n43 & new_n61;
  assign new_n64 = ~i_0_ & new_n26;
  assign new_n65 = ~i_6_ & new_n64;
  assign new_n66 = ~new_n62 & ~new_n63;
  assign new_n67 = ~new_n65 & new_n66;
  assign new_n68 = ~i_5_ & ~i_4_;
  assign new_n69 = ~i_0_ & new_n68;
  assign new_n70 = i_6_ & new_n69;
  assign new_n71 = i_0_ & new_n30;
  assign new_n72 = ~i_6_ & new_n71;
  assign new_n73 = ~i_4_ & ~i_2_;
  assign new_n74 = ~i_1_ & new_n73;
  assign new_n75 = new_n20 & new_n74;
  assign new_n76 = ~new_n70 & ~new_n72;
  assign new_n77 = ~new_n75 & new_n76;
  assign new_n78 = ~i_5_ & ~i_6_;
  assign new_n79 = ~i_3_ & i_4_;
  assign new_n80 = ~i_2_ & new_n79;
  assign new_n81 = new_n78 & new_n80;
  assign new_n82 = ~i_1_ & new_n26;
  assign new_n83 = ~i_6_ & new_n82;
  assign new_n84 = i_3_ & i_2_;
  assign new_n85 = i_1_ & new_n84;
  assign new_n86 = new_n19 & new_n85;
  assign new_n87 = ~new_n81 & ~new_n83;
  assign new_n88 = ~new_n86 & new_n87;
  assign new_n89 = new_n67 & new_n77;
  assign new_n90 = new_n88 & new_n89;
  assign new_n91 = i_5_ & i_4_;
  assign new_n92 = i_3_ & new_n91;
  assign new_n93 = ~i_1_ & i_2_;
  assign new_n94 = ~i_0_ & new_n93;
  assign new_n95 = new_n92 & new_n94;
  assign new_n96 = i_1_ & new_n91;
  assign new_n97 = i_6_ & new_n96;
  assign new_n98 = ~i_0_ & new_n79;
  assign new_n99 = ~i_6_ & new_n98;
  assign new_n100 = ~new_n95 & ~new_n97;
  assign new_n101 = ~new_n99 & new_n100;
  assign new_n102 = i_6_ & ~i_4_;
  assign new_n103 = new_n51 & new_n102;
  assign new_n104 = new_n32 & new_n102;
  assign new_n105 = i_0_ & new_n91;
  assign new_n106 = i_6_ & new_n105;
  assign new_n107 = ~new_n103 & ~new_n104;
  assign new_n108 = ~new_n106 & new_n107;
  assign new_n109 = i_4_ & i_1_;
  assign new_n110 = i_0_ & new_n109;
  assign new_n111 = i_6_ & new_n110;
  assign new_n112 = i_4_ & ~i_2_;
  assign new_n113 = ~i_0_ & new_n112;
  assign new_n114 = ~i_6_ & new_n113;
  assign new_n115 = i_4_ & i_2_;
  assign new_n116 = i_0_ & new_n115;
  assign new_n117 = i_6_ & new_n116;
  assign new_n118 = ~new_n111 & ~new_n114;
  assign new_n119 = ~new_n117 & new_n118;
  assign new_n120 = new_n101 & new_n108;
  assign new_n121 = new_n119 & new_n120;
  assign o_2_ = ~new_n90 | ~new_n121;
  assign new_n123 = i_6_ & new_n71;
  assign new_n124 = i_4_ & ~i_1_;
  assign new_n125 = ~i_0_ & new_n124;
  assign new_n126 = ~i_6_ & new_n125;
  assign new_n127 = i_1_ & new_n30;
  assign new_n128 = i_6_ & new_n127;
  assign new_n129 = ~new_n123 & ~new_n126;
  assign new_n130 = ~new_n128 & new_n129;
  assign new_n131 = i_5_ & i_6_;
  assign new_n132 = i_3_ & ~i_4_;
  assign new_n133 = i_2_ & new_n132;
  assign new_n134 = new_n131 & new_n133;
  assign new_n135 = ~new_n99 & ~new_n114;
  assign new_n136 = ~new_n134 & new_n135;
  assign new_n137 = ~new_n26 & new_n130;
  assign o_0_ = ~new_n136 | ~new_n137;
  assign new_n139 = i_4_ & new_n131;
  assign new_n140 = new_n85 & new_n91;
  assign new_n141 = ~new_n105 & ~new_n139;
  assign o_9_ = new_n140 | ~new_n141;
  assign new_n143 = i_3_ & ~i_2_;
  assign new_n144 = ~i_3_ & i_2_;
  assign o_7_ = new_n143 | new_n144;
  assign new_n146 = ~i_0_ & new_n144;
  assign new_n147 = i_0_ & new_n84;
  assign new_n148 = ~new_n146 & ~new_n147;
  assign new_n149 = ~i_3_ & ~i_2_;
  assign new_n150 = i_0_ & new_n149;
  assign new_n151 = i_1_ & ~i_2_;
  assign new_n152 = ~i_0_ & new_n151;
  assign new_n153 = i_3_ & new_n152;
  assign new_n154 = i_0_ & new_n50;
  assign new_n155 = ~new_n150 & ~new_n153;
  assign new_n156 = ~new_n154 & new_n155;
  assign o_5_ = ~new_n148 | ~new_n156;
  assign new_n158 = ~i_3_ & i_1_;
  assign new_n159 = ~i_1_ & new_n143;
  assign new_n160 = ~new_n158 & ~new_n159;
  assign o_6_ = new_n42 | ~new_n160;
  assign new_n162 = ~i_0_ & new_n39;
  assign new_n163 = i_6_ & new_n162;
  assign new_n164 = ~i_5_ & new_n154;
  assign new_n165 = ~new_n25 & ~new_n163;
  assign new_n166 = ~new_n164 & new_n165;
  assign new_n167 = ~i_5_ & i_3_;
  assign new_n168 = ~i_0_ & new_n42;
  assign new_n169 = new_n167 & new_n168;
  assign new_n170 = ~i_0_ & new_n84;
  assign new_n171 = new_n20 & new_n170;
  assign new_n172 = new_n78 & new_n150;
  assign new_n173 = ~new_n169 & ~new_n171;
  assign new_n174 = ~new_n172 & new_n173;
  assign new_n175 = new_n166 & new_n174;
  assign new_n176 = ~i_5_ & ~i_1_;
  assign new_n177 = i_0_ & new_n176;
  assign new_n178 = ~i_6_ & new_n177;
  assign new_n179 = i_5_ & new_n32;
  assign new_n180 = ~new_n55 & ~new_n178;
  assign new_n181 = ~new_n179 & new_n180;
  assign new_n182 = i_5_ & new_n28;
  assign new_n183 = i_5_ & new_n43;
  assign new_n184 = ~new_n182 & ~new_n183;
  assign new_n185 = ~new_n49 & new_n184;
  assign new_n186 = i_5_ & i_1_;
  assign new_n187 = i_0_ & new_n186;
  assign new_n188 = i_6_ & new_n187;
  assign new_n189 = i_5_ & new_n51;
  assign new_n190 = i_5_ & i_2_;
  assign new_n191 = i_0_ & new_n190;
  assign new_n192 = i_6_ & new_n191;
  assign new_n193 = ~new_n188 & ~new_n189;
  assign new_n194 = ~new_n192 & new_n193;
  assign new_n195 = new_n181 & new_n185;
  assign new_n196 = new_n194 & new_n195;
  assign o_3_ = ~new_n175 | ~new_n196;
  assign new_n198 = i_6_ & ~i_2_;
  assign new_n199 = ~i_1_ & new_n198;
  assign new_n200 = i_6_ & new_n43;
  assign new_n201 = i_6_ & new_n85;
  assign new_n202 = i_6_ & new_n28;
  assign new_n203 = ~new_n200 & ~new_n201;
  assign new_n204 = ~new_n202 & new_n203;
  assign new_n205 = i_1_ & new_n149;
  assign new_n206 = ~i_6_ & new_n205;
  assign new_n207 = ~i_1_ & new_n84;
  assign new_n208 = ~i_6_ & new_n207;
  assign new_n209 = i_6_ & new_n32;
  assign new_n210 = ~new_n206 & ~new_n208;
  assign new_n211 = ~new_n209 & new_n210;
  assign new_n212 = ~i_0_ & new_n158;
  assign new_n213 = ~i_6_ & new_n212;
  assign new_n214 = i_0_ & new_n93;
  assign new_n215 = ~i_6_ & new_n214;
  assign new_n216 = ~i_6_ & new_n152;
  assign new_n217 = ~new_n213 & ~new_n215;
  assign new_n218 = ~new_n216 & new_n217;
  assign new_n219 = new_n204 & new_n211;
  assign new_n220 = new_n218 & new_n219;
  assign o_4_ = new_n199 | ~new_n220;
  assign o_8_ = ~i_3_;
endmodule


