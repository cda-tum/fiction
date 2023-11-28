// Benchmark "top" written by ABC on Mon Nov 27 12:35:17 2023

module top ( 
    pi00, pi01, pi02, pi03, pi04, pi05, pi06, pi07, pi08, pi09, pi10, pi11,
    pi12, pi13, pi14, pi15,
    po0  );
  input  pi00, pi01, pi02, pi03, pi04, pi05, pi06, pi07, pi08, pi09,
    pi10, pi11, pi12, pi13, pi14, pi15;
  output po0;
  wire new_n18, new_n19, new_n20, new_n21, new_n22, new_n23, new_n24,
    new_n25, new_n26, new_n27, new_n28, new_n29, new_n30, new_n31, new_n32,
    new_n33, new_n34, new_n35, new_n36, new_n37, new_n38, new_n39, new_n40,
    new_n41, new_n42, new_n43, new_n44, new_n45, new_n46, new_n47, new_n48,
    new_n49, new_n50, new_n51, new_n52, new_n53, new_n54, new_n55, new_n56,
    new_n57, new_n58, new_n59, new_n60, new_n61;
  assign new_n18 = ~pi12 & ~pi13;
  assign new_n19 = pi12 & pi13;
  assign new_n20 = ~new_n18 & ~new_n19;
  assign new_n21 = ~pi01 & ~pi08;
  assign new_n22 = pi01 & pi08;
  assign new_n23 = ~new_n21 & ~new_n22;
  assign new_n24 = ~pi00 & ~pi03;
  assign new_n25 = pi00 & pi03;
  assign new_n26 = ~new_n24 & ~new_n25;
  assign new_n27 = new_n23 & ~new_n26;
  assign new_n28 = ~new_n23 & new_n26;
  assign new_n29 = ~new_n27 & ~new_n28;
  assign new_n30 = new_n20 & ~new_n29;
  assign new_n31 = ~new_n20 & new_n29;
  assign new_n32 = ~new_n30 & ~new_n31;
  assign new_n33 = pi05 & ~pi10;
  assign new_n34 = ~pi05 & pi10;
  assign new_n35 = ~new_n33 & ~new_n34;
  assign new_n36 = ~pi02 & ~pi04;
  assign new_n37 = pi02 & pi04;
  assign new_n38 = ~new_n36 & ~new_n37;
  assign new_n39 = ~pi06 & ~pi07;
  assign new_n40 = pi06 & pi07;
  assign new_n41 = ~new_n39 & ~new_n40;
  assign new_n42 = new_n38 & ~new_n41;
  assign new_n43 = ~new_n38 & new_n41;
  assign new_n44 = ~new_n42 & ~new_n43;
  assign new_n45 = new_n35 & new_n44;
  assign new_n46 = ~new_n35 & ~new_n44;
  assign new_n47 = ~new_n45 & ~new_n46;
  assign new_n48 = ~pi14 & ~pi15;
  assign new_n49 = pi14 & pi15;
  assign new_n50 = ~new_n48 & ~new_n49;
  assign new_n51 = pi09 & ~pi11;
  assign new_n52 = ~pi09 & pi11;
  assign new_n53 = ~new_n51 & ~new_n52;
  assign new_n54 = new_n50 & ~new_n53;
  assign new_n55 = ~new_n50 & new_n53;
  assign new_n56 = ~new_n54 & ~new_n55;
  assign new_n57 = new_n47 & ~new_n56;
  assign new_n58 = ~new_n47 & new_n56;
  assign new_n59 = ~new_n57 & ~new_n58;
  assign new_n60 = new_n32 & new_n59;
  assign new_n61 = ~new_n32 & ~new_n59;
  assign po0 = new_n60 | new_n61;
endmodule


