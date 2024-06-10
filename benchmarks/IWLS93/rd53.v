// Benchmark "top" written by ABC on Mon Feb 19 11:52:44 2024

module top ( 
    i_3_, i_4_, i_1_, i_2_, i_0_,
    o_1_, o_2_, o_0_  );
  input  i_3_, i_4_, i_1_, i_2_, i_0_;
  output o_1_, o_2_, o_0_;
  wire new_n9, new_n10, new_n11, new_n12, new_n13, new_n14, new_n15, new_n16,
    new_n17, new_n18, new_n19, new_n20, new_n21, new_n22, new_n23, new_n24,
    new_n25, new_n26, new_n27, new_n28, new_n29, new_n30, new_n31, new_n32,
    new_n33, new_n34, new_n35, new_n36, new_n37, new_n38, new_n39, new_n40,
    new_n41, new_n42, new_n43, new_n44, new_n45, new_n46, new_n47, new_n48,
    new_n49, new_n50, new_n51, new_n52, new_n53, new_n54, new_n56, new_n57,
    new_n58, new_n59, new_n60, new_n61, new_n62, new_n63, new_n64, new_n65,
    new_n66, new_n67, new_n68, new_n69, new_n70, new_n71, new_n72, new_n73,
    new_n74, new_n75, new_n76, new_n77, new_n78, new_n79, new_n80, new_n81,
    new_n82, new_n83, new_n84, new_n85, new_n86, new_n87, new_n89, new_n90,
    new_n91, new_n92, new_n93, new_n94, new_n95, new_n96, new_n97, new_n98,
    new_n99, new_n100;
  assign new_n9 = ~i_3_ & ~i_4_;
  assign new_n10 = i_1_ & ~i_2_;
  assign new_n11 = ~i_0_ & new_n10;
  assign new_n12 = new_n9 & new_n11;
  assign new_n13 = i_3_ & ~i_4_;
  assign new_n14 = ~i_1_ & ~i_2_;
  assign new_n15 = ~i_0_ & new_n14;
  assign new_n16 = new_n13 & new_n15;
  assign new_n17 = i_3_ & i_4_;
  assign new_n18 = i_1_ & i_2_;
  assign new_n19 = i_0_ & new_n18;
  assign new_n20 = new_n17 & new_n19;
  assign new_n21 = ~new_n12 & ~new_n16;
  assign new_n22 = ~new_n20 & new_n21;
  assign new_n23 = ~i_0_ & new_n18;
  assign new_n24 = new_n13 & new_n23;
  assign new_n25 = ~i_1_ & i_2_;
  assign new_n26 = ~i_0_ & new_n25;
  assign new_n27 = new_n17 & new_n26;
  assign new_n28 = new_n9 & new_n26;
  assign new_n29 = new_n9 & new_n19;
  assign new_n30 = ~new_n27 & ~new_n28;
  assign new_n31 = ~new_n29 & new_n30;
  assign new_n32 = new_n22 & ~new_n24;
  assign new_n33 = new_n31 & new_n32;
  assign new_n34 = i_0_ & new_n14;
  assign new_n35 = new_n9 & new_n34;
  assign new_n36 = i_0_ & new_n25;
  assign new_n37 = new_n13 & new_n36;
  assign new_n38 = ~i_3_ & i_4_;
  assign new_n39 = i_0_ & new_n10;
  assign new_n40 = new_n38 & new_n39;
  assign new_n41 = ~new_n35 & ~new_n37;
  assign new_n42 = ~new_n40 & new_n41;
  assign new_n43 = new_n23 & new_n38;
  assign new_n44 = new_n13 & new_n39;
  assign new_n45 = new_n11 & new_n17;
  assign new_n46 = ~new_n43 & ~new_n44;
  assign new_n47 = ~new_n45 & new_n46;
  assign new_n48 = new_n36 & new_n38;
  assign new_n49 = new_n15 & new_n38;
  assign new_n50 = new_n17 & new_n34;
  assign new_n51 = ~new_n48 & ~new_n49;
  assign new_n52 = ~new_n50 & new_n51;
  assign new_n53 = new_n42 & new_n47;
  assign new_n54 = new_n52 & new_n53;
  assign o_1_ = ~new_n33 | ~new_n54;
  assign new_n56 = ~i_3_ & i_1_;
  assign new_n57 = ~i_0_ & new_n56;
  assign new_n58 = i_4_ & new_n57;
  assign new_n59 = i_3_ & i_2_;
  assign new_n60 = ~i_1_ & new_n59;
  assign new_n61 = ~i_4_ & new_n60;
  assign new_n62 = ~new_n58 & ~new_n61;
  assign new_n63 = ~i_4_ & new_n23;
  assign new_n64 = ~i_4_ & new_n39;
  assign new_n65 = i_3_ & new_n34;
  assign new_n66 = ~new_n63 & ~new_n64;
  assign new_n67 = ~new_n65 & new_n66;
  assign new_n68 = ~i_3_ & ~i_2_;
  assign new_n69 = i_0_ & new_n68;
  assign new_n70 = i_4_ & new_n69;
  assign new_n71 = i_4_ & new_n26;
  assign new_n72 = ~i_3_ & i_2_;
  assign new_n73 = i_0_ & new_n72;
  assign new_n74 = ~i_4_ & new_n73;
  assign new_n75 = ~new_n70 & ~new_n71;
  assign new_n76 = ~new_n74 & new_n75;
  assign new_n77 = i_3_ & ~i_2_;
  assign new_n78 = i_1_ & new_n77;
  assign new_n79 = ~i_4_ & new_n78;
  assign new_n80 = ~i_0_ & new_n77;
  assign new_n81 = i_4_ & new_n80;
  assign new_n82 = ~i_1_ & new_n72;
  assign new_n83 = i_4_ & new_n82;
  assign new_n84 = ~new_n79 & ~new_n81;
  assign new_n85 = ~new_n83 & new_n84;
  assign new_n86 = new_n67 & new_n76;
  assign new_n87 = new_n85 & new_n86;
  assign o_2_ = ~new_n62 | ~new_n87;
  assign new_n89 = i_0_ & new_n59;
  assign new_n90 = i_4_ & new_n89;
  assign new_n91 = i_3_ & i_1_;
  assign new_n92 = i_0_ & new_n91;
  assign new_n93 = i_4_ & new_n92;
  assign new_n94 = ~new_n90 & ~new_n93;
  assign new_n95 = i_4_ & new_n19;
  assign new_n96 = i_3_ & new_n19;
  assign new_n97 = i_1_ & new_n59;
  assign new_n98 = i_4_ & new_n97;
  assign new_n99 = ~new_n95 & ~new_n96;
  assign new_n100 = ~new_n98 & new_n99;
  assign o_0_ = ~new_n94 | ~new_n100;
endmodule


