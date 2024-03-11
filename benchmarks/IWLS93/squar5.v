// Benchmark "top" written by ABC on Mon Feb 19 11:52:45 2024

module top ( 
    i_3_, i_4_, i_1_, i_2_, i_0_,
    o_1_, o_2_, o_0_, o_7_, o_5_, o_6_, o_3_, o_4_  );
  input  i_3_, i_4_, i_1_, i_2_, i_0_;
  output o_1_, o_2_, o_0_, o_7_, o_5_, o_6_, o_3_, o_4_;
  wire new_n14, new_n15, new_n16, new_n17, new_n18, new_n19, new_n20,
    new_n21, new_n22, new_n23, new_n24, new_n25, new_n26, new_n27, new_n28,
    new_n29, new_n30, new_n31, new_n32, new_n33, new_n34, new_n35, new_n36,
    new_n37, new_n38, new_n39, new_n40, new_n41, new_n42, new_n43, new_n45,
    new_n46, new_n47, new_n48, new_n49, new_n50, new_n51, new_n52, new_n53,
    new_n54, new_n55, new_n56, new_n57, new_n58, new_n59, new_n60, new_n61,
    new_n63, new_n64, new_n65, new_n66, new_n67, new_n68, new_n69, new_n70,
    new_n72, new_n73, new_n74, new_n75, new_n76, new_n77, new_n78, new_n79,
    new_n80, new_n81, new_n82, new_n83, new_n85, new_n86, new_n87, new_n88,
    new_n89, new_n90, new_n91, new_n92, new_n93, new_n94, new_n95, new_n96,
    new_n97, new_n98, new_n99, new_n101, new_n102, new_n103, new_n104,
    new_n105, new_n106, new_n107, new_n109, new_n110, new_n111, new_n112,
    new_n113, new_n114, new_n115, new_n116, new_n117, new_n118, new_n119,
    new_n120, new_n121, new_n123, new_n124, new_n125, new_n126, new_n127,
    new_n128, new_n129, new_n130, new_n131;
  assign new_n14 = ~i_3_ & ~i_4_;
  assign new_n15 = ~i_1_ & ~i_2_;
  assign new_n16 = i_0_ & new_n15;
  assign new_n17 = new_n14 & new_n16;
  assign new_n18 = ~i_3_ & i_4_;
  assign new_n19 = new_n16 & new_n18;
  assign new_n20 = ~new_n17 & ~new_n19;
  assign new_n21 = i_3_ & ~i_4_;
  assign new_n22 = ~i_1_ & i_2_;
  assign new_n23 = i_0_ & new_n22;
  assign new_n24 = new_n21 & new_n23;
  assign new_n25 = new_n18 & new_n23;
  assign new_n26 = i_1_ & i_2_;
  assign new_n27 = i_0_ & new_n26;
  assign new_n28 = new_n14 & new_n27;
  assign new_n29 = ~new_n24 & ~new_n25;
  assign new_n30 = ~new_n28 & new_n29;
  assign new_n31 = i_3_ & i_4_;
  assign new_n32 = new_n16 & new_n31;
  assign new_n33 = new_n16 & new_n21;
  assign new_n34 = new_n14 & new_n23;
  assign new_n35 = ~new_n32 & ~new_n33;
  assign new_n36 = ~new_n34 & new_n35;
  assign new_n37 = new_n21 & new_n27;
  assign new_n38 = new_n18 & new_n27;
  assign new_n39 = new_n27 & new_n31;
  assign new_n40 = ~new_n37 & ~new_n38;
  assign new_n41 = ~new_n39 & new_n40;
  assign new_n42 = new_n30 & new_n36;
  assign new_n43 = new_n41 & new_n42;
  assign o_1_ = ~new_n20 | ~new_n43;
  assign new_n45 = ~i_0_ & new_n26;
  assign new_n46 = new_n14 & new_n45;
  assign new_n47 = new_n18 & new_n45;
  assign new_n48 = ~new_n46 & ~new_n47;
  assign new_n49 = i_1_ & ~i_2_;
  assign new_n50 = i_0_ & new_n49;
  assign new_n51 = new_n21 & new_n50;
  assign new_n52 = new_n29 & ~new_n51;
  assign new_n53 = new_n31 & new_n45;
  assign new_n54 = new_n21 & new_n45;
  assign new_n55 = ~new_n53 & ~new_n54;
  assign new_n56 = ~new_n34 & new_n55;
  assign new_n57 = new_n31 & new_n50;
  assign new_n58 = ~new_n37 & ~new_n57;
  assign new_n59 = ~new_n39 & new_n58;
  assign new_n60 = new_n52 & new_n56;
  assign new_n61 = new_n59 & new_n60;
  assign o_2_ = ~new_n48 | ~new_n61;
  assign new_n63 = ~new_n51 & ~new_n57;
  assign new_n64 = ~new_n28 & new_n63;
  assign new_n65 = new_n14 & new_n50;
  assign new_n66 = new_n23 & new_n31;
  assign new_n67 = new_n18 & new_n50;
  assign new_n68 = ~new_n65 & ~new_n66;
  assign new_n69 = ~new_n67 & new_n68;
  assign new_n70 = new_n64 & new_n69;
  assign o_0_ = ~new_n41 | ~new_n70;
  assign new_n72 = ~i_0_ & new_n49;
  assign new_n73 = new_n21 & new_n72;
  assign new_n74 = ~new_n54 & ~new_n73;
  assign new_n75 = ~new_n33 & new_n74;
  assign new_n76 = ~i_0_ & new_n15;
  assign new_n77 = new_n21 & new_n76;
  assign new_n78 = ~i_0_ & new_n22;
  assign new_n79 = new_n21 & new_n78;
  assign new_n80 = ~new_n77 & ~new_n79;
  assign new_n81 = ~new_n24 & ~new_n51;
  assign new_n82 = ~new_n37 & new_n81;
  assign new_n83 = new_n75 & new_n80;
  assign o_7_ = ~new_n82 | ~new_n83;
  assign new_n85 = new_n18 & new_n78;
  assign new_n86 = new_n14 & new_n78;
  assign new_n87 = new_n31 & new_n78;
  assign new_n88 = ~new_n85 & ~new_n86;
  assign new_n89 = ~new_n87 & new_n88;
  assign new_n90 = ~new_n25 & ~new_n34;
  assign new_n91 = ~new_n66 & new_n90;
  assign new_n92 = new_n31 & new_n72;
  assign new_n93 = new_n18 & new_n72;
  assign new_n94 = ~new_n92 & ~new_n93;
  assign new_n95 = ~new_n46 & new_n94;
  assign new_n96 = ~new_n57 & ~new_n67;
  assign new_n97 = ~new_n28 & new_n96;
  assign new_n98 = new_n91 & new_n95;
  assign new_n99 = new_n97 & new_n98;
  assign o_5_ = ~new_n89 | ~new_n99;
  assign new_n101 = ~new_n47 & ~new_n92;
  assign new_n102 = ~new_n32 & new_n101;
  assign new_n103 = new_n31 & new_n76;
  assign new_n104 = ~new_n85 & ~new_n103;
  assign new_n105 = ~new_n25 & ~new_n57;
  assign new_n106 = ~new_n38 & new_n105;
  assign new_n107 = new_n102 & new_n104;
  assign o_6_ = ~new_n106 | ~new_n107;
  assign new_n109 = new_n14 & new_n72;
  assign new_n110 = ~new_n93 & ~new_n109;
  assign new_n111 = ~new_n73 & ~new_n92;
  assign new_n112 = ~new_n54 & new_n111;
  assign new_n113 = new_n110 & new_n112;
  assign new_n114 = ~new_n24 & ~new_n65;
  assign new_n115 = ~new_n67 & new_n114;
  assign new_n116 = ~new_n33 & ~new_n53;
  assign new_n117 = ~new_n32 & new_n116;
  assign new_n118 = ~new_n38 & ~new_n57;
  assign new_n119 = ~new_n39 & new_n118;
  assign new_n120 = new_n115 & new_n117;
  assign new_n121 = new_n119 & new_n120;
  assign o_3_ = ~new_n113 | ~new_n121;
  assign new_n123 = ~new_n79 & ~new_n87;
  assign new_n124 = ~new_n73 & new_n123;
  assign new_n125 = ~new_n19 & ~new_n32;
  assign new_n126 = ~new_n25 & new_n125;
  assign new_n127 = ~new_n53 & new_n101;
  assign new_n128 = ~new_n24 & ~new_n67;
  assign new_n129 = ~new_n51 & new_n128;
  assign new_n130 = new_n126 & new_n127;
  assign new_n131 = new_n129 & new_n130;
  assign o_4_ = ~new_n124 | ~new_n131;
endmodule


