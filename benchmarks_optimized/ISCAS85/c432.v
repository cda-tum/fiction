// Benchmark "top" written by ABC on Mon Nov 27 16:48:36 2023

module top ( 
    \1 , \4 , \8 , \11 , \14 , \17 , \21 , \24 , \27 , \30 , \34 , \37 ,
    \40 , \43 , \47 , \50 , \53 , \56 , \60 , \63 , \66 , \69 , \73 , \76 ,
    \79 , \82 , \86 , \89 , \92 , \95 , \99 , \102 , \105 , \108 , \112 ,
    \115 ,
    \223 , \329 , \370 , \421 , \430 , \431 , \432   );
  input  \1 , \4 , \8 , \11 , \14 , \17 , \21 , \24 , \27 , \30 , \34 ,
    \37 , \40 , \43 , \47 , \50 , \53 , \56 , \60 , \63 , \66 , \69 , \73 ,
    \76 , \79 , \82 , \86 , \89 , \92 , \95 , \99 , \102 , \105 , \108 ,
    \112 , \115 ;
  output \223 , \329 , \370 , \421 , \430 , \431 , \432 ;
  wire new_n44, new_n45, new_n46, new_n47, new_n48, new_n49, new_n50,
    new_n51, new_n52, new_n53, new_n54, new_n55, new_n56, new_n57, new_n58,
    new_n59, new_n61, new_n62, new_n63, new_n64, new_n65, new_n66, new_n67,
    new_n68, new_n69, new_n70, new_n71, new_n72, new_n73, new_n74, new_n75,
    new_n76, new_n77, new_n78, new_n79, new_n80, new_n81, new_n82, new_n83,
    new_n84, new_n85, new_n86, new_n87, new_n88, new_n89, new_n90, new_n91,
    new_n92, new_n93, new_n94, new_n96, new_n97, new_n98, new_n99,
    new_n100, new_n101, new_n102, new_n103, new_n104, new_n105, new_n106,
    new_n107, new_n108, new_n109, new_n110, new_n111, new_n112, new_n113,
    new_n114, new_n115, new_n116, new_n117, new_n118, new_n119, new_n120,
    new_n121, new_n122, new_n123, new_n124, new_n125, new_n126, new_n127,
    new_n128, new_n129, new_n131, new_n132, new_n133, new_n134, new_n135,
    new_n136, new_n137, new_n138, new_n139, new_n140, new_n141, new_n142,
    new_n144, new_n145, new_n146, new_n147, new_n148, new_n149, new_n150,
    new_n151, new_n153, new_n155, new_n156, new_n157, new_n158, new_n159,
    new_n160, new_n161, new_n162, new_n163, new_n164;
  assign new_n44 = ~\24  & \30 ;
  assign new_n45 = ~\37  & \43 ;
  assign new_n46 = ~\63  & \69 ;
  assign new_n47 = ~\11  & \17 ;
  assign new_n48 = ~\89  & \95 ;
  assign new_n49 = ~\102  & \108 ;
  assign new_n50 = ~\76  & \82 ;
  assign new_n51 = ~\1  & \4 ;
  assign new_n52 = ~\50  & \56 ;
  assign new_n53 = ~new_n45 & ~new_n46;
  assign new_n54 = ~new_n47 & ~new_n48;
  assign new_n55 = ~new_n49 & ~new_n50;
  assign new_n56 = ~new_n51 & ~new_n52;
  assign new_n57 = new_n55 & new_n56;
  assign new_n58 = new_n53 & new_n54;
  assign new_n59 = new_n57 & new_n58;
  assign \223  = new_n44 | ~new_n59;
  assign new_n61 = \63  & \223 ;
  assign new_n62 = \69  & ~new_n61;
  assign new_n63 = ~\73  & new_n62;
  assign new_n64 = \102  & \223 ;
  assign new_n65 = \108  & ~new_n64;
  assign new_n66 = ~\112  & new_n65;
  assign new_n67 = \1  & \223 ;
  assign new_n68 = \4  & ~new_n67;
  assign new_n69 = ~\8  & new_n68;
  assign new_n70 = \11  & \223 ;
  assign new_n71 = \17  & ~new_n70;
  assign new_n72 = ~\21  & new_n71;
  assign new_n73 = \37  & \223 ;
  assign new_n74 = \43  & ~new_n73;
  assign new_n75 = ~\47  & new_n74;
  assign new_n76 = \50  & \223 ;
  assign new_n77 = \56  & ~new_n76;
  assign new_n78 = ~\60  & new_n77;
  assign new_n79 = \30  & new_n59;
  assign new_n80 = ~new_n44 & ~new_n79;
  assign new_n81 = ~\34  & ~new_n80;
  assign new_n82 = \76  & \223 ;
  assign new_n83 = \82  & ~new_n82;
  assign new_n84 = ~\86  & new_n83;
  assign new_n85 = \89  & \223 ;
  assign new_n86 = \95  & ~new_n85;
  assign new_n87 = ~\99  & new_n86;
  assign new_n88 = ~new_n66 & ~new_n81;
  assign new_n89 = ~new_n69 & ~new_n72;
  assign new_n90 = ~new_n75 & ~new_n78;
  assign new_n91 = ~new_n84 & ~new_n87;
  assign new_n92 = new_n90 & new_n91;
  assign new_n93 = new_n88 & new_n89;
  assign new_n94 = new_n92 & new_n93;
  assign \329  = new_n63 | ~new_n94;
  assign new_n96 = \99  & \329 ;
  assign new_n97 = new_n86 & ~new_n96;
  assign new_n98 = ~\105  & new_n97;
  assign new_n99 = \73  & ~new_n94;
  assign new_n100 = new_n62 & ~new_n99;
  assign new_n101 = ~\79  & new_n100;
  assign new_n102 = \86  & \329 ;
  assign new_n103 = new_n83 & ~new_n102;
  assign new_n104 = ~\92  & new_n103;
  assign new_n105 = ~new_n101 & ~new_n104;
  assign new_n106 = \112  & \329 ;
  assign new_n107 = ~\115  & new_n65;
  assign new_n108 = ~new_n106 & new_n107;
  assign new_n109 = \60  & \329 ;
  assign new_n110 = new_n77 & ~new_n109;
  assign new_n111 = ~\66  & new_n110;
  assign new_n112 = ~new_n80 & ~\329 ;
  assign new_n113 = ~new_n81 & ~new_n112;
  assign new_n114 = ~\40  & ~new_n113;
  assign new_n115 = \8  & \329 ;
  assign new_n116 = new_n68 & ~new_n115;
  assign new_n117 = ~\14  & new_n116;
  assign new_n118 = \21  & \329 ;
  assign new_n119 = new_n71 & ~new_n118;
  assign new_n120 = ~\27  & new_n119;
  assign new_n121 = \47  & \329 ;
  assign new_n122 = new_n74 & ~new_n121;
  assign new_n123 = ~\53  & new_n122;
  assign new_n124 = ~new_n108 & ~new_n111;
  assign new_n125 = ~new_n114 & ~new_n117;
  assign new_n126 = ~new_n120 & ~new_n123;
  assign new_n127 = new_n125 & new_n126;
  assign new_n128 = new_n105 & new_n124;
  assign new_n129 = new_n127 & new_n128;
  assign \370  = new_n98 | ~new_n129;
  assign new_n131 = \14  & \370 ;
  assign new_n132 = new_n116 & ~new_n131;
  assign new_n133 = \27  & \370 ;
  assign new_n134 = new_n119 & ~new_n133;
  assign new_n135 = ~new_n113 & ~\370 ;
  assign new_n136 = ~new_n114 & ~new_n135;
  assign new_n137 = ~new_n134 & new_n136;
  assign new_n138 = \66  & \370 ;
  assign new_n139 = new_n110 & ~new_n138;
  assign new_n140 = \53  & \370 ;
  assign new_n141 = new_n122 & ~new_n140;
  assign new_n142 = ~new_n139 & ~new_n141;
  assign \430  = ~new_n137 | ~new_n142;
  assign new_n144 = ~new_n100 & ~new_n103;
  assign new_n145 = ~\370  & ~new_n144;
  assign new_n146 = new_n105 & ~new_n145;
  assign new_n147 = \105  & ~new_n129;
  assign new_n148 = new_n97 & ~new_n147;
  assign new_n149 = ~\108  & ~new_n148;
  assign new_n150 = new_n146 & new_n149;
  assign new_n151 = ~\430  & new_n150;
  assign \421  = ~new_n132 & ~new_n151;
  assign new_n153 = new_n142 & ~new_n146;
  assign \431  = ~new_n137 | new_n153;
  assign new_n155 = new_n101 & ~new_n111;
  assign new_n156 = \92  & new_n98;
  assign new_n157 = new_n103 & ~new_n156;
  assign new_n158 = new_n148 & ~new_n157;
  assign new_n159 = new_n100 & ~new_n110;
  assign new_n160 = ~\370  & new_n159;
  assign new_n161 = ~new_n155 & ~new_n160;
  assign new_n162 = ~new_n141 & new_n161;
  assign new_n163 = ~new_n158 & new_n162;
  assign new_n164 = new_n136 & ~new_n163;
  assign \432  = new_n134 | new_n164;
endmodule


