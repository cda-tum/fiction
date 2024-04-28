// Benchmark "top" written by ABC on Mon Feb 19 11:52:45 2024

module top ( 
    a, b, c, d, e,
    xor5  );
  input  a, b, c, d, e;
  output xor5;
  wire new_n7, new_n8, new_n9, new_n10, new_n11, new_n12, new_n13, new_n14,
    new_n15, new_n16, new_n17, new_n18, new_n19, new_n20, new_n21, new_n22,
    new_n23, new_n24, new_n25, new_n26, new_n27, new_n28, new_n29, new_n30,
    new_n31, new_n32, new_n33, new_n34, new_n35, new_n36, new_n37, new_n38,
    new_n39, new_n40, new_n41, new_n42, new_n43, new_n44, new_n45, new_n46,
    new_n47, new_n48, new_n49, new_n50, new_n51, new_n52;
  assign new_n7 = a & ~e;
  assign new_n8 = b & ~c;
  assign new_n9 = d & new_n8;
  assign new_n10 = new_n7 & new_n9;
  assign new_n11 = b & c;
  assign new_n12 = ~d & new_n11;
  assign new_n13 = new_n7 & new_n12;
  assign new_n14 = a & e;
  assign new_n15 = ~d & new_n8;
  assign new_n16 = new_n14 & new_n15;
  assign new_n17 = ~new_n10 & ~new_n13;
  assign new_n18 = ~new_n16 & new_n17;
  assign new_n19 = d & new_n11;
  assign new_n20 = new_n14 & new_n19;
  assign new_n21 = ~b & c;
  assign new_n22 = ~d & new_n21;
  assign new_n23 = new_n14 & new_n22;
  assign new_n24 = d & new_n21;
  assign new_n25 = new_n7 & new_n24;
  assign new_n26 = ~b & ~c;
  assign new_n27 = d & new_n26;
  assign new_n28 = new_n14 & new_n27;
  assign new_n29 = ~new_n23 & ~new_n25;
  assign new_n30 = ~new_n28 & new_n29;
  assign new_n31 = new_n18 & ~new_n20;
  assign new_n32 = new_n30 & new_n31;
  assign new_n33 = ~a & ~e;
  assign new_n34 = new_n15 & new_n33;
  assign new_n35 = ~a & e;
  assign new_n36 = new_n9 & new_n35;
  assign new_n37 = new_n24 & new_n35;
  assign new_n38 = ~new_n34 & ~new_n36;
  assign new_n39 = ~new_n37 & new_n38;
  assign new_n40 = new_n19 & new_n33;
  assign new_n41 = ~d & new_n26;
  assign new_n42 = new_n7 & new_n41;
  assign new_n43 = new_n12 & new_n35;
  assign new_n44 = ~new_n40 & ~new_n42;
  assign new_n45 = ~new_n43 & new_n44;
  assign new_n46 = new_n27 & new_n33;
  assign new_n47 = new_n22 & new_n33;
  assign new_n48 = new_n35 & new_n41;
  assign new_n49 = ~new_n46 & ~new_n47;
  assign new_n50 = ~new_n48 & new_n49;
  assign new_n51 = new_n39 & new_n45;
  assign new_n52 = new_n50 & new_n51;
  assign xor5 = ~new_n32 | ~new_n52;
endmodule


