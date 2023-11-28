// Benchmark "top" written by ABC on Mon Nov 27 17:04:18 2023

module top ( 
    B0, B1, B2, B3, B4, B5, B6, B7, B8, B9, B10,
    M0, M1, M2, M3, E0, E1, E2  );
  input  B0, B1, B2, B3, B4, B5, B6, B7, B8, B9, B10;
  output M0, M1, M2, M3, E0, E1, E2;
  wire new_n19, new_n20, new_n21, new_n22, new_n23, new_n24, new_n25,
    new_n26, new_n27, new_n28, new_n29, new_n30, new_n31, new_n32, new_n33,
    new_n34, new_n35, new_n36, new_n37, new_n38, new_n39, new_n40, new_n41,
    new_n42, new_n43, new_n44, new_n45, new_n46, new_n47, new_n48, new_n49,
    new_n50, new_n51, new_n52, new_n53, new_n54, new_n55, new_n56, new_n57,
    new_n58, new_n59, new_n60, new_n61, new_n62, new_n63, new_n64, new_n65,
    new_n66, new_n67, new_n68, new_n69, new_n71, new_n72, new_n73, new_n74,
    new_n75, new_n76, new_n77, new_n78, new_n79, new_n80, new_n81, new_n82,
    new_n83, new_n84, new_n85, new_n86, new_n87, new_n88, new_n89, new_n90,
    new_n91, new_n92, new_n93, new_n94, new_n95, new_n96, new_n97, new_n98,
    new_n99, new_n100, new_n101, new_n102, new_n103, new_n104, new_n105,
    new_n106, new_n107, new_n108, new_n109, new_n110, new_n112, new_n113,
    new_n114, new_n115, new_n116, new_n117, new_n118, new_n119, new_n120,
    new_n121, new_n122, new_n123, new_n124, new_n125, new_n126, new_n127,
    new_n128, new_n129, new_n130, new_n131, new_n132, new_n133, new_n134,
    new_n135, new_n136, new_n137, new_n138, new_n139, new_n140, new_n141,
    new_n142, new_n143, new_n144, new_n145, new_n146, new_n147, new_n148,
    new_n149, new_n150, new_n151, new_n152, new_n153, new_n154, new_n155,
    new_n156, new_n157, new_n158, new_n160, new_n161, new_n162, new_n163,
    new_n164, new_n165, new_n166, new_n167, new_n168, new_n170, new_n171,
    new_n172, new_n173, new_n174, new_n175, new_n176, new_n177, new_n178,
    new_n179, new_n180, new_n181, new_n182, new_n183, new_n184, new_n186,
    new_n187, new_n188, new_n189, new_n190, new_n191;
  assign new_n19 = B8 & ~B9;
  assign new_n20 = B4 & B5;
  assign new_n21 = new_n19 & ~new_n20;
  assign new_n22 = ~B4 & ~B5;
  assign new_n23 = new_n21 & ~new_n22;
  assign new_n24 = B5 & B6;
  assign new_n25 = ~B5 & ~B6;
  assign new_n26 = B9 & ~new_n24;
  assign new_n27 = ~new_n25 & new_n26;
  assign new_n28 = B5 & ~B6;
  assign new_n29 = ~B7 & ~B8;
  assign new_n30 = B1 & B2;
  assign new_n31 = new_n29 & ~new_n30;
  assign new_n32 = ~B1 & ~B2;
  assign new_n33 = new_n28 & ~new_n32;
  assign new_n34 = new_n31 & new_n33;
  assign new_n35 = ~new_n23 & ~new_n27;
  assign new_n36 = ~new_n34 & new_n35;
  assign new_n37 = ~B10 & ~new_n36;
  assign new_n38 = B0 & B1;
  assign new_n39 = B4 & ~B7;
  assign new_n40 = new_n38 & new_n39;
  assign new_n41 = ~B8 & ~B9;
  assign new_n42 = ~B10 & new_n41;
  assign new_n43 = B3 & B4;
  assign new_n44 = ~B3 & ~B4;
  assign new_n45 = ~new_n43 & ~new_n44;
  assign new_n46 = B7 & new_n45;
  assign new_n47 = B1 & B4;
  assign new_n48 = ~B0 & ~new_n47;
  assign new_n49 = ~B6 & ~B7;
  assign new_n50 = ~B5 & new_n49;
  assign new_n51 = ~new_n48 & new_n50;
  assign new_n52 = ~new_n46 & ~new_n51;
  assign new_n53 = ~new_n40 & new_n42;
  assign new_n54 = ~new_n52 & new_n53;
  assign new_n55 = B6 & ~B7;
  assign new_n56 = B2 & B3;
  assign new_n57 = ~B2 & ~B3;
  assign new_n58 = new_n41 & ~new_n56;
  assign new_n59 = ~new_n57 & new_n58;
  assign new_n60 = ~B10 & ~new_n59;
  assign new_n61 = new_n55 & ~new_n60;
  assign new_n62 = B6 & B7;
  assign new_n63 = new_n19 & new_n62;
  assign new_n64 = B10 & ~new_n63;
  assign new_n65 = B6 & ~B8;
  assign new_n66 = B7 & ~new_n65;
  assign new_n67 = new_n64 & new_n66;
  assign new_n68 = ~new_n54 & ~new_n67;
  assign new_n69 = ~new_n61 & new_n68;
  assign M0 = new_n37 | ~new_n69;
  assign new_n71 = ~B8 & ~new_n62;
  assign new_n72 = new_n64 & ~new_n71;
  assign new_n73 = ~B7 & B9;
  assign new_n74 = ~new_n21 & ~new_n73;
  assign new_n75 = ~B6 & ~new_n74;
  assign new_n76 = B7 & new_n41;
  assign new_n77 = ~new_n43 & new_n76;
  assign new_n78 = B1 & ~B4;
  assign new_n79 = B2 & new_n38;
  assign new_n80 = ~B2 & ~new_n38;
  assign new_n81 = B4 & ~new_n79;
  assign new_n82 = ~new_n80 & new_n81;
  assign new_n83 = new_n49 & ~new_n78;
  assign new_n84 = ~new_n82 & new_n83;
  assign new_n85 = ~B5 & ~new_n73;
  assign new_n86 = ~new_n77 & new_n85;
  assign new_n87 = ~new_n84 & new_n86;
  assign new_n88 = B9 & new_n62;
  assign new_n89 = new_n49 & new_n56;
  assign new_n90 = ~new_n43 & ~new_n89;
  assign new_n91 = ~new_n31 & new_n41;
  assign new_n92 = ~new_n90 & new_n91;
  assign new_n93 = B4 & B6;
  assign new_n94 = new_n19 & new_n93;
  assign new_n95 = B5 & ~new_n88;
  assign new_n96 = ~new_n94 & new_n95;
  assign new_n97 = ~new_n92 & new_n96;
  assign new_n98 = ~new_n87 & ~new_n97;
  assign new_n99 = ~B10 & ~new_n75;
  assign new_n100 = ~new_n98 & new_n99;
  assign new_n101 = ~new_n72 & ~new_n100;
  assign new_n102 = ~B3 & B5;
  assign new_n103 = ~B6 & new_n102;
  assign new_n104 = new_n31 & new_n103;
  assign new_n105 = ~B2 & B4;
  assign new_n106 = B2 & new_n45;
  assign new_n107 = new_n41 & new_n55;
  assign new_n108 = ~new_n105 & new_n107;
  assign new_n109 = ~new_n106 & new_n108;
  assign new_n110 = ~new_n104 & ~new_n109;
  assign M1 = ~new_n101 & new_n110;
  assign new_n112 = ~B1 & new_n28;
  assign new_n113 = B2 & ~B5;
  assign new_n114 = B6 & new_n113;
  assign new_n115 = ~new_n112 & ~new_n114;
  assign new_n116 = new_n43 & ~new_n115;
  assign new_n117 = ~B2 & B3;
  assign new_n118 = ~B6 & new_n117;
  assign new_n119 = ~new_n102 & ~new_n118;
  assign new_n120 = new_n39 & ~new_n119;
  assign new_n121 = B6 & ~new_n20;
  assign new_n122 = ~new_n49 & ~new_n62;
  assign new_n123 = ~new_n121 & new_n122;
  assign new_n124 = new_n56 & new_n123;
  assign new_n125 = B7 & new_n43;
  assign new_n126 = new_n24 & ~new_n125;
  assign new_n127 = ~new_n124 & new_n126;
  assign new_n128 = ~new_n116 & ~new_n120;
  assign new_n129 = ~new_n127 & new_n128;
  assign new_n130 = new_n42 & ~new_n129;
  assign new_n131 = B9 & B10;
  assign new_n132 = B5 & B7;
  assign new_n133 = B9 & ~new_n132;
  assign new_n134 = ~new_n123 & ~new_n133;
  assign new_n135 = B8 & ~B10;
  assign new_n136 = ~new_n134 & new_n135;
  assign new_n137 = ~B9 & ~B10;
  assign new_n138 = new_n28 & new_n43;
  assign new_n139 = ~new_n121 & ~new_n138;
  assign new_n140 = B7 & ~new_n139;
  assign new_n141 = ~B3 & new_n38;
  assign new_n142 = B3 & ~new_n38;
  assign new_n143 = B4 & ~new_n141;
  assign new_n144 = ~new_n142 & new_n143;
  assign new_n145 = new_n25 & ~new_n144;
  assign new_n146 = B3 & B5;
  assign new_n147 = new_n78 & new_n146;
  assign new_n148 = ~new_n145 & ~new_n147;
  assign new_n149 = B2 & new_n29;
  assign new_n150 = ~new_n148 & new_n149;
  assign new_n151 = ~new_n140 & ~new_n150;
  assign new_n152 = new_n137 & ~new_n151;
  assign new_n153 = B7 & ~new_n41;
  assign new_n154 = ~new_n135 & new_n153;
  assign new_n155 = new_n134 & new_n154;
  assign new_n156 = ~new_n131 & ~new_n136;
  assign new_n157 = ~new_n155 & new_n156;
  assign new_n158 = ~new_n130 & new_n157;
  assign M2 = new_n152 | ~new_n158;
  assign new_n160 = B5 & B8;
  assign new_n161 = new_n62 & new_n160;
  assign new_n162 = new_n105 & new_n161;
  assign new_n163 = B4 & ~B5;
  assign new_n164 = ~B6 & ~new_n163;
  assign new_n165 = ~B5 & new_n29;
  assign new_n166 = new_n164 & new_n165;
  assign new_n167 = ~new_n162 & ~new_n166;
  assign new_n168 = ~B3 & new_n137;
  assign M3 = new_n167 | ~new_n168;
  assign new_n170 = ~B7 & ~new_n164;
  assign new_n171 = new_n20 & new_n119;
  assign new_n172 = ~new_n170 & ~new_n171;
  assign new_n173 = new_n38 & new_n56;
  assign new_n174 = new_n25 & new_n173;
  assign new_n175 = ~new_n112 & ~new_n174;
  assign new_n176 = ~new_n124 & new_n175;
  assign new_n177 = ~new_n172 & new_n176;
  assign new_n178 = ~B8 & ~new_n177;
  assign new_n179 = B9 & ~new_n161;
  assign new_n180 = B4 & ~new_n57;
  assign new_n181 = ~B9 & new_n161;
  assign new_n182 = new_n180 & new_n181;
  assign new_n183 = ~new_n179 & ~new_n182;
  assign new_n184 = ~new_n178 & new_n183;
  assign E0 = B10 | new_n184;
  assign new_n186 = new_n29 & ~new_n124;
  assign new_n187 = ~new_n25 & new_n186;
  assign new_n188 = new_n165 & new_n173;
  assign new_n189 = ~new_n161 & ~new_n188;
  assign new_n190 = new_n180 & ~new_n189;
  assign new_n191 = new_n137 & ~new_n190;
  assign E1 = new_n187 | ~new_n191;
  assign E2 = ~new_n137 | ~new_n186;
endmodule


