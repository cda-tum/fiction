// Benchmark "top" written by ABC on Mon Feb 19 11:52:45 2024

module top ( 
    v_6_, v_7_, v_4_, v_5_, v_2_, v_3_, v_0_, v_1_,
    sqrt_3_, sqrt_2_, sqrt_1_, sqrt_0_  );
  input  v_6_, v_7_, v_4_, v_5_, v_2_, v_3_, v_0_, v_1_;
  output sqrt_3_, sqrt_2_, sqrt_1_, sqrt_0_;
  wire new_n14, new_n15, new_n16, new_n17, new_n19, new_n20, new_n21,
    new_n22, new_n23, new_n24, new_n25, new_n26, new_n27, new_n28, new_n29,
    new_n30, new_n31, new_n32, new_n33, new_n34, new_n35, new_n36, new_n37,
    new_n38, new_n39, new_n40, new_n41, new_n42, new_n43, new_n44, new_n45,
    new_n46, new_n48, new_n49, new_n50, new_n51, new_n52, new_n53, new_n54,
    new_n55, new_n56, new_n57, new_n58, new_n59, new_n60, new_n61, new_n62,
    new_n63, new_n64, new_n65, new_n66, new_n67, new_n68, new_n69, new_n70,
    new_n71, new_n72, new_n73, new_n74, new_n75, new_n76, new_n77, new_n78,
    new_n79, new_n80, new_n81, new_n82, new_n83, new_n84, new_n85, new_n86,
    new_n87, new_n88, new_n89, new_n90, new_n91, new_n92, new_n93, new_n94,
    new_n95, new_n96, new_n97, new_n98, new_n99, new_n100, new_n101,
    new_n102, new_n103, new_n104, new_n105, new_n106, new_n107, new_n108,
    new_n109, new_n110, new_n111, new_n112;
  assign sqrt_3_ = v_6_ | v_7_;
  assign new_n14 = ~v_6_ & v_5_;
  assign new_n15 = ~v_6_ & v_4_;
  assign new_n16 = v_6_ & v_7_;
  assign new_n17 = ~new_n14 & ~new_n15;
  assign sqrt_2_ = new_n16 | ~new_n17;
  assign new_n19 = v_5_ & v_2_;
  assign new_n20 = ~v_7_ & new_n19;
  assign new_n21 = ~v_4_ & ~v_5_;
  assign new_n22 = ~v_6_ & new_n21;
  assign new_n23 = v_3_ & new_n22;
  assign new_n24 = v_5_ & v_3_;
  assign new_n25 = ~v_7_ & new_n24;
  assign new_n26 = v_7_ & new_n21;
  assign new_n27 = v_3_ & new_n26;
  assign new_n28 = ~new_n23 & ~new_n25;
  assign new_n29 = ~new_n27 & new_n28;
  assign new_n30 = v_2_ & new_n22;
  assign new_n31 = v_6_ & v_4_;
  assign new_n32 = v_7_ & new_n31;
  assign new_n33 = v_2_ & new_n26;
  assign new_n34 = ~new_n30 & ~new_n32;
  assign new_n35 = ~new_n33 & new_n34;
  assign new_n36 = v_6_ & v_5_;
  assign new_n37 = v_7_ & new_n36;
  assign new_n38 = v_4_ & v_5_;
  assign new_n39 = ~v_7_ & new_n38;
  assign new_n40 = ~v_6_ & ~v_5_;
  assign new_n41 = v_7_ & new_n40;
  assign new_n42 = ~v_4_ & new_n41;
  assign new_n43 = ~new_n37 & ~new_n39;
  assign new_n44 = ~new_n42 & new_n43;
  assign new_n45 = new_n29 & new_n35;
  assign new_n46 = new_n44 & new_n45;
  assign sqrt_1_ = new_n20 | ~new_n46;
  assign new_n48 = v_4_ & v_0_;
  assign new_n49 = v_6_ & ~v_5_;
  assign new_n50 = ~v_7_ & new_n49;
  assign new_n51 = new_n48 & new_n50;
  assign new_n52 = ~v_2_ & v_0_;
  assign new_n53 = ~v_3_ & new_n52;
  assign new_n54 = ~v_4_ & v_5_;
  assign new_n55 = v_6_ & new_n54;
  assign new_n56 = new_n53 & new_n55;
  assign new_n57 = ~new_n42 & ~new_n51;
  assign new_n58 = ~new_n56 & new_n57;
  assign new_n59 = v_4_ & v_3_;
  assign new_n60 = ~v_7_ & new_n59;
  assign new_n61 = v_1_ & new_n60;
  assign new_n62 = new_n22 & new_n52;
  assign new_n63 = v_2_ & new_n37;
  assign new_n64 = ~new_n61 & ~new_n62;
  assign new_n65 = ~new_n63 & new_n64;
  assign new_n66 = v_7_ & new_n24;
  assign new_n67 = v_1_ & new_n66;
  assign new_n68 = ~v_6_ & new_n38;
  assign new_n69 = v_1_ & new_n68;
  assign new_n70 = ~v_2_ & v_1_;
  assign new_n71 = new_n22 & new_n70;
  assign new_n72 = ~new_n67 & ~new_n69;
  assign new_n73 = ~new_n71 & new_n72;
  assign new_n74 = new_n58 & new_n65;
  assign new_n75 = new_n73 & new_n74;
  assign new_n76 = ~v_2_ & ~v_3_;
  assign new_n77 = ~v_7_ & new_n54;
  assign new_n78 = new_n76 & new_n77;
  assign new_n79 = v_2_ & v_3_;
  assign new_n80 = new_n22 & new_n79;
  assign new_n81 = new_n26 & new_n76;
  assign new_n82 = ~new_n78 & ~new_n80;
  assign new_n83 = ~new_n81 & new_n82;
  assign new_n84 = v_0_ & new_n60;
  assign new_n85 = v_0_ & new_n68;
  assign new_n86 = v_7_ & new_n38;
  assign new_n87 = v_0_ & new_n66;
  assign new_n88 = ~new_n85 & ~new_n86;
  assign new_n89 = ~new_n87 & new_n88;
  assign new_n90 = new_n83 & ~new_n84;
  assign new_n91 = new_n89 & new_n90;
  assign new_n92 = v_2_ & new_n68;
  assign new_n93 = v_2_ & new_n66;
  assign new_n94 = v_3_ & new_n68;
  assign new_n95 = ~new_n92 & ~new_n93;
  assign new_n96 = ~new_n94 & new_n95;
  assign new_n97 = v_4_ & v_1_;
  assign new_n98 = new_n50 & new_n97;
  assign new_n99 = v_2_ & new_n60;
  assign new_n100 = ~v_3_ & new_n70;
  assign new_n101 = new_n55 & new_n100;
  assign new_n102 = ~new_n98 & ~new_n99;
  assign new_n103 = ~new_n101 & new_n102;
  assign new_n104 = new_n50 & new_n59;
  assign new_n105 = v_4_ & v_2_;
  assign new_n106 = new_n50 & new_n105;
  assign new_n107 = v_3_ & new_n37;
  assign new_n108 = ~new_n104 & ~new_n106;
  assign new_n109 = ~new_n107 & new_n108;
  assign new_n110 = new_n96 & new_n103;
  assign new_n111 = new_n109 & new_n110;
  assign new_n112 = new_n75 & new_n91;
  assign sqrt_0_ = ~new_n111 | ~new_n112;
endmodule


