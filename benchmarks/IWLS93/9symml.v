// Benchmark "top" written by ABC on Mon Feb 19 11:52:41 2024

module top ( 
    _9, _8, _7, _6, _5, _4, _3, _2, _1,
    _52  );
  input  _9, _8, _7, _6, _5, _4, _3, _2, _1;
  output _52;
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
    new_n224, new_n225, new_n226;
  assign new_n11 = _9 & ~_8;
  assign new_n12 = _8 & ~_7;
  assign new_n13 = ~new_n11 & ~new_n12;
  assign new_n14 = _6 & ~new_n13;
  assign new_n15 = ~_5 & new_n14;
  assign new_n16 = _7 & ~_5;
  assign new_n17 = ~_7 & _5;
  assign new_n18 = ~_6 & _4;
  assign new_n19 = ~new_n16 & ~new_n17;
  assign new_n20 = ~new_n18 & new_n19;
  assign new_n21 = ~_9 & ~new_n20;
  assign new_n22 = _8 & new_n21;
  assign new_n23 = ~_5 & _1;
  assign new_n24 = _8 & ~_1;
  assign new_n25 = ~new_n23 & ~new_n24;
  assign new_n26 = _9 & ~new_n25;
  assign new_n27 = ~_4 & new_n26;
  assign new_n28 = ~new_n15 & ~new_n22;
  assign new_n29 = ~new_n27 & new_n28;
  assign new_n30 = ~_3 & ~new_n29;
  assign new_n31 = ~_4 & _3;
  assign new_n32 = _4 & ~_3;
  assign new_n33 = ~new_n31 & ~new_n32;
  assign new_n34 = ~_5 & ~new_n33;
  assign new_n35 = _6 & new_n34;
  assign new_n36 = ~_1 & new_n35;
  assign new_n37 = _7 & ~_4;
  assign new_n38 = ~_5 & new_n37;
  assign new_n39 = _1 & new_n38;
  assign new_n40 = ~_8 & _7;
  assign new_n41 = ~new_n17 & ~new_n40;
  assign new_n42 = _9 & ~new_n41;
  assign new_n43 = ~_3 & new_n42;
  assign new_n44 = ~_8 & _3;
  assign new_n45 = _7 & ~_3;
  assign new_n46 = ~new_n44 & ~new_n45;
  assign new_n47 = _4 & ~new_n46;
  assign new_n48 = ~_1 & new_n47;
  assign new_n49 = ~new_n39 & ~new_n43;
  assign new_n50 = ~new_n48 & new_n49;
  assign new_n51 = ~_6 & ~new_n50;
  assign new_n52 = ~new_n30 & ~new_n36;
  assign new_n53 = ~new_n51 & new_n52;
  assign new_n54 = _2 & ~new_n53;
  assign new_n55 = ~_2 & ~new_n13;
  assign new_n56 = _1 & new_n55;
  assign new_n57 = ~new_n12 & ~new_n40;
  assign new_n58 = ~_9 & ~new_n57;
  assign new_n59 = _2 & new_n58;
  assign new_n60 = ~_6 & _2;
  assign new_n61 = ~_7 & _6;
  assign new_n62 = ~new_n60 & ~new_n61;
  assign new_n63 = ~new_n40 & new_n62;
  assign new_n64 = _5 & ~new_n63;
  assign new_n65 = ~_1 & new_n64;
  assign new_n66 = ~new_n56 & ~new_n59;
  assign new_n67 = ~new_n65 & new_n66;
  assign new_n68 = _3 & ~new_n67;
  assign new_n69 = ~_7 & ~_5;
  assign new_n70 = _2 & new_n69;
  assign new_n71 = ~_2 & new_n45;
  assign new_n72 = ~new_n70 & ~new_n71;
  assign new_n73 = _8 & ~new_n72;
  assign new_n74 = _1 & new_n73;
  assign new_n75 = ~_8 & ~_3;
  assign new_n76 = _5 & new_n75;
  assign new_n77 = _2 & new_n76;
  assign new_n78 = ~_3 & _2;
  assign new_n79 = ~_5 & _3;
  assign new_n80 = ~new_n78 & ~new_n79;
  assign new_n81 = ~_9 & ~new_n80;
  assign new_n82 = _6 & new_n81;
  assign new_n83 = _3 & ~new_n25;
  assign new_n84 = ~_2 & new_n83;
  assign new_n85 = ~new_n77 & ~new_n82;
  assign new_n86 = ~new_n84 & new_n85;
  assign new_n87 = _7 & ~new_n86;
  assign new_n88 = ~new_n68 & ~new_n74;
  assign new_n89 = ~new_n87 & new_n88;
  assign new_n90 = ~_4 & ~new_n89;
  assign new_n91 = _3 & ~_2;
  assign new_n92 = ~new_n78 & ~new_n91;
  assign new_n93 = ~_5 & _4;
  assign new_n94 = _5 & ~_4;
  assign new_n95 = ~new_n93 & ~new_n94;
  assign new_n96 = ~_6 & ~new_n92;
  assign new_n97 = ~new_n95 & new_n96;
  assign new_n98 = _1 & new_n97;
  assign new_n99 = ~_4 & _1;
  assign new_n100 = _4 & ~_1;
  assign new_n101 = ~new_n99 & ~new_n100;
  assign new_n102 = ~_5 & ~new_n101;
  assign new_n103 = _6 & new_n102;
  assign new_n104 = _3 & new_n103;
  assign new_n105 = ~_3 & _1;
  assign new_n106 = ~_9 & _3;
  assign new_n107 = _9 & ~_1;
  assign new_n108 = ~new_n105 & ~new_n106;
  assign new_n109 = ~new_n107 & new_n108;
  assign new_n110 = _5 & ~new_n109;
  assign new_n111 = ~_6 & new_n110;
  assign new_n112 = _4 & new_n111;
  assign new_n113 = ~_8 & _6;
  assign new_n114 = ~_7 & new_n113;
  assign new_n115 = _4 & new_n114;
  assign new_n116 = ~_6 & _5;
  assign new_n117 = ~new_n93 & ~new_n116;
  assign new_n118 = ~new_n57 & ~new_n117;
  assign new_n119 = new_n57 & new_n95;
  assign new_n120 = _9 & ~new_n119;
  assign new_n121 = ~_3 & new_n120;
  assign new_n122 = ~new_n115 & ~new_n118;
  assign new_n123 = ~new_n121 & new_n122;
  assign new_n124 = _1 & ~new_n123;
  assign new_n125 = ~new_n104 & ~new_n112;
  assign new_n126 = ~new_n124 & new_n125;
  assign new_n127 = ~_2 & ~new_n126;
  assign new_n128 = _5 & ~_3;
  assign new_n129 = ~new_n106 & ~new_n128;
  assign new_n130 = _6 & ~new_n129;
  assign new_n131 = ~_8 & new_n130;
  assign new_n132 = _4 & new_n131;
  assign new_n133 = _7 & new_n11;
  assign new_n134 = ~_7 & _4;
  assign new_n135 = _2 & new_n134;
  assign new_n136 = ~new_n133 & ~new_n135;
  assign new_n137 = ~_5 & ~new_n136;
  assign new_n138 = _3 & new_n137;
  assign new_n139 = _9 & ~_5;
  assign new_n140 = ~new_n17 & ~new_n139;
  assign new_n141 = ~_6 & ~new_n140;
  assign new_n142 = _3 & new_n141;
  assign new_n143 = _3 & new_n37;
  assign new_n144 = _7 & ~_2;
  assign new_n145 = ~new_n79 & ~new_n134;
  assign new_n146 = ~new_n144 & new_n145;
  assign new_n147 = _6 & ~new_n146;
  assign new_n148 = _5 & _4;
  assign new_n149 = ~_3 & new_n148;
  assign new_n150 = ~new_n143 & ~new_n147;
  assign new_n151 = ~new_n149 & new_n150;
  assign new_n152 = ~_9 & ~new_n151;
  assign new_n153 = _9 & ~_3;
  assign new_n154 = _7 & new_n153;
  assign new_n155 = ~_2 & new_n154;
  assign new_n156 = ~new_n142 & ~new_n152;
  assign new_n157 = ~new_n155 & new_n156;
  assign new_n158 = _8 & ~new_n157;
  assign new_n159 = ~new_n132 & ~new_n138;
  assign new_n160 = ~new_n158 & new_n159;
  assign new_n161 = ~_1 & ~new_n160;
  assign new_n162 = _6 & ~_3;
  assign new_n163 = ~new_n18 & ~new_n162;
  assign new_n164 = ~_8 & ~new_n163;
  assign new_n165 = _2 & new_n164;
  assign new_n166 = _8 & ~_4;
  assign new_n167 = ~new_n100 & ~new_n166;
  assign new_n168 = _7 & ~new_n167;
  assign new_n169 = ~_6 & new_n168;
  assign new_n170 = _6 & ~new_n57;
  assign new_n171 = ~_2 & new_n170;
  assign new_n172 = ~new_n165 & ~new_n169;
  assign new_n173 = ~new_n171 & new_n172;
  assign new_n174 = _5 & ~new_n173;
  assign new_n175 = _8 & ~_6;
  assign new_n176 = _4 & new_n175;
  assign new_n177 = ~_8 & _2;
  assign new_n178 = _1 & new_n177;
  assign new_n179 = ~new_n176 & ~new_n178;
  assign new_n180 = ~_7 & ~new_n179;
  assign new_n181 = _3 & new_n180;
  assign new_n182 = ~_6 & _3;
  assign new_n183 = _8 & ~_2;
  assign new_n184 = ~new_n113 & ~new_n182;
  assign new_n185 = ~new_n183 & new_n184;
  assign new_n186 = ~_5 & ~new_n185;
  assign new_n187 = _7 & new_n186;
  assign new_n188 = _4 & new_n187;
  assign new_n189 = _8 & ~_5;
  assign new_n190 = ~new_n94 & ~new_n189;
  assign new_n191 = ~_2 & ~new_n190;
  assign new_n192 = new_n72 & ~new_n191;
  assign new_n193 = _6 & ~new_n192;
  assign new_n194 = _1 & new_n193;
  assign new_n195 = ~new_n174 & ~new_n181;
  assign new_n196 = ~new_n188 & ~new_n194;
  assign new_n197 = new_n195 & new_n196;
  assign new_n198 = ~_9 & ~new_n197;
  assign new_n199 = ~_7 & ~new_n101;
  assign new_n200 = ~_3 & new_n199;
  assign new_n201 = ~_8 & ~_4;
  assign new_n202 = ~_5 & new_n201;
  assign new_n203 = _3 & new_n202;
  assign new_n204 = ~_2 & ~new_n57;
  assign new_n205 = ~_1 & new_n204;
  assign new_n206 = ~new_n200 & ~new_n203;
  assign new_n207 = ~new_n205 & new_n206;
  assign new_n208 = _6 & ~new_n207;
  assign new_n209 = ~new_n57 & ~new_n95;
  assign new_n210 = ~_6 & new_n209;
  assign new_n211 = _2 & ~new_n33;
  assign new_n212 = _3 & ~new_n117;
  assign new_n213 = _6 & _5;
  assign new_n214 = ~_1 & new_n213;
  assign new_n215 = ~new_n211 & ~new_n212;
  assign new_n216 = ~new_n214 & new_n215;
  assign new_n217 = ~_8 & ~new_n216;
  assign new_n218 = ~_7 & new_n217;
  assign new_n219 = ~new_n208 & ~new_n210;
  assign new_n220 = ~new_n218 & new_n219;
  assign new_n221 = _9 & ~new_n220;
  assign new_n222 = ~new_n161 & ~new_n198;
  assign new_n223 = ~new_n221 & new_n222;
  assign new_n224 = ~new_n54 & ~new_n90;
  assign new_n225 = ~new_n98 & ~new_n127;
  assign new_n226 = new_n224 & new_n225;
  assign _52 = ~new_n223 | ~new_n226;
endmodule


