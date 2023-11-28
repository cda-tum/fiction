// Benchmark "top" written by ABC on Mon Nov 27 17:23:17 2023

module top ( 
    dest_x0, dest_x1, dest_x2, dest_x3, dest_x4, dest_x5, dest_x6, dest_x7,
    dest_x8, dest_x9, dest_x10, dest_x11, dest_x12, dest_x13, dest_x14,
    dest_x15, dest_x16, dest_x17, dest_x18, dest_x19, dest_x20, dest_x21,
    dest_x22, dest_x23, dest_x24, dest_x25, dest_x26, dest_x27, dest_x28,
    dest_x29, dest_y0, dest_y1, dest_y2, dest_y3, dest_y4, dest_y5,
    dest_y6, dest_y7, dest_y8, dest_y9, dest_y10, dest_y11, dest_y12,
    dest_y13, dest_y14, dest_y15, dest_y16, dest_y17, dest_y18, dest_y19,
    dest_y20, dest_y21, dest_y22, dest_y23, dest_y24, dest_y25, dest_y26,
    dest_y27, dest_y28, dest_y29,
    outport0, outport1, outport2  );
  input  dest_x0, dest_x1, dest_x2, dest_x3, dest_x4, dest_x5, dest_x6,
    dest_x7, dest_x8, dest_x9, dest_x10, dest_x11, dest_x12, dest_x13,
    dest_x14, dest_x15, dest_x16, dest_x17, dest_x18, dest_x19, dest_x20,
    dest_x21, dest_x22, dest_x23, dest_x24, dest_x25, dest_x26, dest_x27,
    dest_x28, dest_x29, dest_y0, dest_y1, dest_y2, dest_y3, dest_y4,
    dest_y5, dest_y6, dest_y7, dest_y8, dest_y9, dest_y10, dest_y11,
    dest_y12, dest_y13, dest_y14, dest_y15, dest_y16, dest_y17, dest_y18,
    dest_y19, dest_y20, dest_y21, dest_y22, dest_y23, dest_y24, dest_y25,
    dest_y26, dest_y27, dest_y28, dest_y29;
  output outport0, outport1, outport2;
  wire new_n64, new_n65, new_n66, new_n67, new_n68, new_n69, new_n70,
    new_n71, new_n72, new_n73, new_n74, new_n75, new_n76, new_n77, new_n78,
    new_n79, new_n80, new_n81, new_n82, new_n83, new_n84, new_n85, new_n86,
    new_n87, new_n88, new_n89, new_n90, new_n91, new_n92, new_n93, new_n94,
    new_n95, new_n96, new_n97, new_n98, new_n99, new_n100, new_n101,
    new_n102, new_n103, new_n104, new_n105, new_n106, new_n107, new_n108,
    new_n109, new_n110, new_n111, new_n112, new_n113, new_n114, new_n116,
    new_n117, new_n118, new_n119, new_n120, new_n121, new_n122, new_n123,
    new_n124, new_n125, new_n126, new_n127, new_n128, new_n129, new_n130,
    new_n131, new_n132, new_n133, new_n134, new_n135, new_n136, new_n137,
    new_n138, new_n139, new_n140, new_n141, new_n142, new_n143, new_n144,
    new_n145, new_n146, new_n147, new_n148, new_n149, new_n150, new_n151,
    new_n152, new_n153, new_n154, new_n155, new_n156, new_n157, new_n158,
    new_n159, new_n160, new_n161, new_n162, new_n163, new_n164, new_n165,
    new_n166, new_n167, new_n168, new_n169, new_n170, new_n172, new_n173,
    new_n174;
  assign new_n64 = dest_x23 & dest_x24;
  assign new_n65 = dest_x25 & new_n64;
  assign new_n66 = ~dest_x21 & ~dest_x22;
  assign new_n67 = dest_x19 & dest_x20;
  assign new_n68 = dest_x14 & dest_x15;
  assign new_n69 = ~dest_x9 & ~dest_x10;
  assign new_n70 = dest_x11 & ~new_n69;
  assign new_n71 = ~dest_x12 & ~dest_x13;
  assign new_n72 = ~new_n70 & new_n71;
  assign new_n73 = new_n68 & ~new_n72;
  assign new_n74 = ~dest_x16 & ~new_n73;
  assign new_n75 = dest_x17 & ~new_n74;
  assign new_n76 = ~dest_x18 & ~new_n75;
  assign new_n77 = new_n67 & ~new_n76;
  assign new_n78 = new_n66 & ~new_n77;
  assign new_n79 = new_n65 & ~new_n78;
  assign new_n80 = ~dest_x26 & ~new_n79;
  assign new_n81 = dest_x17 & ~dest_x18;
  assign new_n82 = new_n67 & new_n81;
  assign new_n83 = dest_x0 & dest_x1;
  assign new_n84 = dest_x2 & dest_x3;
  assign new_n85 = dest_x4 & dest_x5;
  assign new_n86 = dest_x6 & dest_x7;
  assign new_n87 = dest_x8 & dest_x11;
  assign new_n88 = new_n86 & new_n87;
  assign new_n89 = new_n84 & new_n85;
  assign new_n90 = new_n68 & new_n83;
  assign new_n91 = new_n89 & new_n90;
  assign new_n92 = new_n65 & new_n88;
  assign new_n93 = new_n82 & new_n92;
  assign new_n94 = new_n91 & new_n93;
  assign new_n95 = new_n80 & ~new_n94;
  assign new_n96 = dest_x27 & dest_x28;
  assign new_n97 = dest_x29 & new_n96;
  assign new_n98 = ~new_n95 & new_n97;
  assign new_n99 = ~dest_x1 & ~dest_x2;
  assign new_n100 = ~dest_x3 & ~dest_x4;
  assign new_n101 = ~dest_x5 & ~dest_x6;
  assign new_n102 = new_n100 & new_n101;
  assign new_n103 = new_n71 & new_n99;
  assign new_n104 = new_n102 & new_n103;
  assign new_n105 = ~dest_x7 & ~dest_x8;
  assign new_n106 = ~dest_x10 & ~dest_x16;
  assign new_n107 = ~dest_x26 & new_n106;
  assign new_n108 = new_n66 & new_n105;
  assign new_n109 = new_n107 & new_n108;
  assign new_n110 = new_n82 & new_n109;
  assign new_n111 = new_n80 & ~new_n110;
  assign new_n112 = ~new_n104 & ~new_n111;
  assign new_n113 = new_n104 & ~new_n110;
  assign new_n114 = ~new_n112 & ~new_n113;
  assign outport0 = ~new_n98 | ~new_n114;
  assign new_n116 = dest_y27 & dest_y28;
  assign new_n117 = dest_y29 & new_n116;
  assign new_n118 = dest_y14 & dest_y15;
  assign new_n119 = dest_y24 & dest_y25;
  assign new_n120 = dest_y19 & dest_y20;
  assign new_n121 = dest_y17 & ~dest_y18;
  assign new_n122 = new_n120 & new_n121;
  assign new_n123 = ~dest_x0 & dest_y0;
  assign new_n124 = dest_y1 & dest_y2;
  assign new_n125 = dest_y3 & dest_y4;
  assign new_n126 = dest_y5 & dest_y6;
  assign new_n127 = dest_y7 & dest_y8;
  assign new_n128 = dest_y11 & dest_y23;
  assign new_n129 = new_n127 & new_n128;
  assign new_n130 = new_n125 & new_n126;
  assign new_n131 = new_n123 & new_n124;
  assign new_n132 = new_n118 & new_n119;
  assign new_n133 = new_n131 & new_n132;
  assign new_n134 = new_n129 & new_n130;
  assign new_n135 = new_n117 & new_n122;
  assign new_n136 = new_n134 & new_n135;
  assign new_n137 = new_n133 & new_n136;
  assign new_n138 = ~dest_y21 & ~dest_y22;
  assign new_n139 = ~dest_y9 & ~dest_y10;
  assign new_n140 = dest_y11 & ~new_n139;
  assign new_n141 = ~dest_y12 & ~dest_y13;
  assign new_n142 = ~new_n140 & new_n141;
  assign new_n143 = new_n118 & ~new_n142;
  assign new_n144 = ~dest_y16 & ~new_n143;
  assign new_n145 = dest_y17 & ~new_n144;
  assign new_n146 = ~dest_y18 & ~new_n145;
  assign new_n147 = new_n120 & ~new_n146;
  assign new_n148 = new_n138 & ~new_n147;
  assign new_n149 = dest_y23 & new_n119;
  assign new_n150 = ~new_n148 & new_n149;
  assign new_n151 = ~dest_y26 & ~new_n150;
  assign new_n152 = new_n117 & ~new_n151;
  assign new_n153 = ~dest_y1 & ~dest_y2;
  assign new_n154 = ~dest_y3 & ~dest_y4;
  assign new_n155 = ~dest_y5 & ~dest_y6;
  assign new_n156 = ~dest_y7 & ~dest_y8;
  assign new_n157 = ~dest_y10 & ~dest_y16;
  assign new_n158 = ~dest_y26 & new_n157;
  assign new_n159 = new_n155 & new_n156;
  assign new_n160 = new_n153 & new_n154;
  assign new_n161 = new_n138 & new_n141;
  assign new_n162 = new_n160 & new_n161;
  assign new_n163 = new_n158 & new_n159;
  assign new_n164 = new_n122 & new_n163;
  assign new_n165 = new_n162 & new_n164;
  assign new_n166 = ~dest_x0 & ~dest_y0;
  assign new_n167 = new_n165 & new_n166;
  assign new_n168 = new_n152 & ~new_n167;
  assign new_n169 = ~new_n137 & ~new_n168;
  assign new_n170 = new_n114 & new_n169;
  assign outport1 = ~new_n98 | new_n170;
  assign new_n172 = dest_x0 & dest_y0;
  assign new_n173 = new_n165 & ~new_n172;
  assign new_n174 = new_n152 & ~new_n173;
  assign outport2 = ~outport0 & new_n174;
endmodule


