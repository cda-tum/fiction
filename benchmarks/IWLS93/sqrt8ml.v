// Benchmark "top" written by ABC on Mon Feb 19 11:52:45 2024

module top ( 
    v_6_, v_7_, v_4_, v_5_, v_2_, v_3_, v_0_, v_1_,
    sqrt_3_, sqrt_2_, sqrt_1_, sqrt_0_  );
  input  v_6_, v_7_, v_4_, v_5_, v_2_, v_3_, v_0_, v_1_;
  output sqrt_3_, sqrt_2_, sqrt_1_, sqrt_0_;
  wire new_n13, new_n14, new_n15, new_n16, new_n17, new_n18, new_n19,
    new_n20, new_n21, new_n22, new_n23, new_n24, new_n25, new_n26, new_n27,
    new_n28, new_n29, new_n30, new_n31, new_n32, new_n33, new_n34, new_n35,
    new_n36, new_n37, new_n38, new_n39, new_n40, new_n41, new_n42, new_n43,
    new_n44, new_n45, new_n46, new_n47, new_n48, new_n49, new_n50, new_n51,
    new_n52, new_n53, new_n54, new_n55, new_n56, new_n57, new_n58, new_n59,
    new_n60, new_n61, new_n62, new_n63, new_n64, new_n65, new_n66, new_n67,
    new_n68, new_n69, new_n70, new_n71, new_n72, new_n73, new_n74, new_n75,
    new_n76, new_n77, new_n78, new_n79, new_n80, new_n81, new_n82, new_n83,
    new_n84, new_n85, new_n86, new_n87, new_n88, new_n89, new_n90, new_n91,
    new_n92, new_n93, new_n94, new_n95, new_n96, new_n97, new_n98, new_n99,
    new_n100, new_n101, new_n102, new_n103, new_n104, new_n105, new_n106,
    new_n107, new_n108, new_n109, new_n110, new_n111, new_n112, new_n113,
    new_n114, new_n115, new_n116, new_n117, new_n118, new_n119, new_n120,
    new_n121, new_n122, new_n123, new_n124, new_n125, new_n126, new_n127,
    new_n128, new_n129, new_n130, new_n131, new_n132, new_n133, new_n134,
    new_n135, new_n136, new_n137, new_n138, new_n139, new_n140, new_n141,
    new_n142, new_n143, new_n144, new_n145, new_n146, new_n147, new_n148,
    new_n149, new_n150, new_n151, new_n152, new_n153, new_n154, new_n155,
    new_n156, new_n157, new_n158, new_n159, new_n160, new_n161, new_n162,
    new_n163, new_n164, new_n165, new_n166, new_n167, new_n168, new_n169,
    new_n170, new_n171, new_n172, new_n173, new_n174, new_n175, new_n176,
    new_n177, new_n178, new_n179, new_n180, new_n181, new_n182, new_n183,
    new_n185, new_n186, new_n188, new_n189, new_n191, new_n192;
  assign new_n13 = ~v_6_ & ~v_7_;
  assign new_n14 = v_6_ & v_7_;
  assign new_n15 = ~new_n13 & ~new_n14;
  assign new_n16 = v_7_ & new_n13;
  assign new_n17 = ~new_n13 & ~new_n15;
  assign new_n18 = ~new_n16 & ~new_n17;
  assign new_n19 = v_5_ & new_n13;
  assign new_n20 = v_5_ & ~new_n13;
  assign new_n21 = ~new_n19 & ~new_n20;
  assign new_n22 = v_4_ & new_n13;
  assign new_n23 = v_4_ & ~new_n13;
  assign new_n24 = ~new_n22 & ~new_n23;
  assign new_n25 = new_n21 & new_n24;
  assign new_n26 = v_6_ & new_n13;
  assign new_n27 = ~v_6_ & ~new_n13;
  assign new_n28 = ~new_n26 & ~new_n27;
  assign new_n29 = new_n13 & ~new_n28;
  assign new_n30 = ~new_n13 & new_n28;
  assign new_n31 = ~new_n29 & ~new_n30;
  assign new_n32 = new_n25 & new_n31;
  assign new_n33 = ~new_n13 & ~new_n31;
  assign new_n34 = ~new_n32 & ~new_n33;
  assign new_n35 = new_n18 & ~new_n34;
  assign new_n36 = ~new_n13 & new_n35;
  assign new_n37 = ~new_n13 & ~new_n35;
  assign new_n38 = ~new_n36 & ~new_n37;
  assign new_n39 = new_n13 & new_n28;
  assign new_n40 = ~new_n13 & ~new_n28;
  assign new_n41 = ~new_n39 & ~new_n40;
  assign new_n42 = v_3_ & new_n13;
  assign new_n43 = v_3_ & ~new_n13;
  assign new_n44 = ~new_n42 & ~new_n43;
  assign new_n45 = v_2_ & new_n13;
  assign new_n46 = v_2_ & ~new_n13;
  assign new_n47 = ~new_n45 & ~new_n46;
  assign new_n48 = v_1_ & new_n13;
  assign new_n49 = v_1_ & ~new_n13;
  assign new_n50 = ~new_n48 & ~new_n49;
  assign new_n51 = v_0_ & new_n13;
  assign new_n52 = v_0_ & ~new_n13;
  assign new_n53 = ~new_n51 & ~new_n52;
  assign new_n54 = new_n21 & ~new_n24;
  assign new_n55 = new_n21 & ~new_n54;
  assign new_n56 = ~new_n41 & ~new_n55;
  assign new_n57 = ~new_n29 & ~new_n56;
  assign new_n58 = ~new_n18 & ~new_n57;
  assign new_n59 = new_n18 & new_n57;
  assign new_n60 = ~new_n58 & ~new_n59;
  assign new_n61 = ~new_n18 & new_n35;
  assign new_n62 = ~new_n35 & ~new_n60;
  assign new_n63 = ~new_n61 & ~new_n62;
  assign new_n64 = new_n41 & ~new_n55;
  assign new_n65 = ~new_n41 & new_n55;
  assign new_n66 = ~new_n64 & ~new_n65;
  assign new_n67 = ~new_n28 & new_n35;
  assign new_n68 = ~new_n35 & ~new_n66;
  assign new_n69 = ~new_n67 & ~new_n68;
  assign new_n70 = new_n35 & ~new_n44;
  assign new_n71 = ~new_n35 & ~new_n44;
  assign new_n72 = ~new_n70 & ~new_n71;
  assign new_n73 = new_n35 & ~new_n47;
  assign new_n74 = ~new_n35 & ~new_n47;
  assign new_n75 = ~new_n73 & ~new_n74;
  assign new_n76 = new_n72 & new_n75;
  assign new_n77 = ~new_n24 & new_n35;
  assign new_n78 = new_n24 & ~new_n35;
  assign new_n79 = ~new_n77 & ~new_n78;
  assign new_n80 = new_n35 & ~new_n79;
  assign new_n81 = ~new_n35 & new_n79;
  assign new_n82 = ~new_n80 & ~new_n81;
  assign new_n83 = new_n76 & new_n82;
  assign new_n84 = ~new_n35 & ~new_n82;
  assign new_n85 = ~new_n83 & ~new_n84;
  assign new_n86 = ~new_n21 & ~new_n24;
  assign new_n87 = ~new_n25 & ~new_n86;
  assign new_n88 = ~new_n21 & new_n35;
  assign new_n89 = ~new_n35 & ~new_n87;
  assign new_n90 = ~new_n88 & ~new_n89;
  assign new_n91 = new_n38 & ~new_n90;
  assign new_n92 = ~new_n38 & new_n90;
  assign new_n93 = ~new_n91 & ~new_n92;
  assign new_n94 = ~new_n85 & new_n93;
  assign new_n95 = ~new_n38 & ~new_n93;
  assign new_n96 = ~new_n94 & ~new_n95;
  assign new_n97 = new_n69 & ~new_n96;
  assign new_n98 = new_n63 & new_n97;
  assign new_n99 = ~new_n38 & new_n98;
  assign new_n100 = ~new_n38 & ~new_n98;
  assign new_n101 = ~new_n99 & ~new_n100;
  assign new_n102 = new_n38 & new_n90;
  assign new_n103 = ~new_n38 & ~new_n90;
  assign new_n104 = ~new_n102 & ~new_n103;
  assign new_n105 = new_n35 & new_n79;
  assign new_n106 = ~new_n35 & ~new_n79;
  assign new_n107 = ~new_n105 & ~new_n106;
  assign new_n108 = new_n35 & ~new_n50;
  assign new_n109 = ~new_n35 & ~new_n50;
  assign new_n110 = ~new_n108 & ~new_n109;
  assign new_n111 = new_n35 & ~new_n53;
  assign new_n112 = ~new_n35 & ~new_n53;
  assign new_n113 = ~new_n111 & ~new_n112;
  assign new_n114 = new_n72 & ~new_n75;
  assign new_n115 = new_n72 & ~new_n114;
  assign new_n116 = ~new_n107 & ~new_n115;
  assign new_n117 = ~new_n80 & ~new_n116;
  assign new_n118 = ~new_n104 & ~new_n117;
  assign new_n119 = ~new_n91 & ~new_n118;
  assign new_n120 = new_n69 & ~new_n119;
  assign new_n121 = new_n69 & ~new_n120;
  assign new_n122 = ~new_n63 & ~new_n121;
  assign new_n123 = new_n63 & new_n121;
  assign new_n124 = ~new_n122 & ~new_n123;
  assign new_n125 = ~new_n63 & new_n98;
  assign new_n126 = ~new_n98 & ~new_n124;
  assign new_n127 = ~new_n125 & ~new_n126;
  assign new_n128 = ~new_n69 & ~new_n119;
  assign new_n129 = new_n69 & new_n119;
  assign new_n130 = ~new_n128 & ~new_n129;
  assign new_n131 = ~new_n69 & new_n98;
  assign new_n132 = ~new_n98 & ~new_n130;
  assign new_n133 = ~new_n131 & ~new_n132;
  assign new_n134 = new_n104 & ~new_n117;
  assign new_n135 = ~new_n104 & new_n117;
  assign new_n136 = ~new_n134 & ~new_n135;
  assign new_n137 = ~new_n90 & new_n98;
  assign new_n138 = ~new_n98 & ~new_n136;
  assign new_n139 = ~new_n137 & ~new_n138;
  assign new_n140 = new_n98 & ~new_n110;
  assign new_n141 = ~new_n98 & ~new_n110;
  assign new_n142 = ~new_n140 & ~new_n141;
  assign new_n143 = new_n98 & ~new_n113;
  assign new_n144 = ~new_n98 & ~new_n113;
  assign new_n145 = ~new_n143 & ~new_n144;
  assign new_n146 = new_n142 & new_n145;
  assign new_n147 = ~new_n75 & new_n98;
  assign new_n148 = new_n75 & ~new_n98;
  assign new_n149 = ~new_n147 & ~new_n148;
  assign new_n150 = new_n98 & ~new_n149;
  assign new_n151 = ~new_n98 & new_n149;
  assign new_n152 = ~new_n150 & ~new_n151;
  assign new_n153 = new_n146 & new_n152;
  assign new_n154 = ~new_n98 & ~new_n152;
  assign new_n155 = ~new_n153 & ~new_n154;
  assign new_n156 = ~new_n35 & new_n98;
  assign new_n157 = ~new_n35 & ~new_n98;
  assign new_n158 = ~new_n156 & ~new_n157;
  assign new_n159 = ~new_n72 & ~new_n75;
  assign new_n160 = ~new_n76 & ~new_n159;
  assign new_n161 = ~new_n72 & new_n98;
  assign new_n162 = ~new_n98 & ~new_n160;
  assign new_n163 = ~new_n161 & ~new_n162;
  assign new_n164 = new_n158 & ~new_n163;
  assign new_n165 = ~new_n158 & new_n163;
  assign new_n166 = ~new_n164 & ~new_n165;
  assign new_n167 = ~new_n155 & new_n166;
  assign new_n168 = ~new_n158 & ~new_n166;
  assign new_n169 = ~new_n167 & ~new_n168;
  assign new_n170 = new_n107 & ~new_n115;
  assign new_n171 = ~new_n107 & new_n115;
  assign new_n172 = ~new_n170 & ~new_n171;
  assign new_n173 = ~new_n79 & new_n98;
  assign new_n174 = ~new_n98 & ~new_n172;
  assign new_n175 = ~new_n173 & ~new_n174;
  assign new_n176 = new_n101 & ~new_n175;
  assign new_n177 = ~new_n101 & new_n175;
  assign new_n178 = ~new_n176 & ~new_n177;
  assign new_n179 = ~new_n169 & new_n178;
  assign new_n180 = ~new_n101 & ~new_n178;
  assign new_n181 = ~new_n179 & ~new_n180;
  assign new_n182 = new_n139 & ~new_n181;
  assign new_n183 = new_n133 & new_n182;
  assign sqrt_0_ = ~new_n127 | ~new_n183;
  assign new_n185 = ~new_n101 & ~sqrt_0_;
  assign new_n186 = ~new_n101 & sqrt_0_;
  assign sqrt_3_ = new_n185 | new_n186;
  assign new_n188 = ~new_n158 & ~sqrt_0_;
  assign new_n189 = ~new_n158 & sqrt_0_;
  assign sqrt_2_ = new_n188 | new_n189;
  assign new_n191 = ~new_n98 & ~sqrt_0_;
  assign new_n192 = ~new_n98 & sqrt_0_;
  assign sqrt_1_ = new_n191 | new_n192;
endmodule


