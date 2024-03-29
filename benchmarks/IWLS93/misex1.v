// Benchmark "top" written by ABC on Mon Feb 19 11:52:44 2024

module top ( 
    rmwb, yskip, page, xskip, dmpst0, dmpst1, dmpst2, dmpst3,
    adctlp0b, adctlp1b, adctlp2b, dmnst0b, dmnst1b, dmnst2b, dmnst3b  );
  input  rmwb, yskip, page, xskip, dmpst0, dmpst1, dmpst2, dmpst3;
  output adctlp0b, adctlp1b, adctlp2b, dmnst0b, dmnst1b, dmnst2b, dmnst3b;
  wire new_n16, new_n17, new_n18, new_n19, new_n20, new_n21, new_n22,
    new_n23, new_n24, new_n25, new_n26, new_n27, new_n28, new_n29, new_n30,
    new_n31, new_n32, new_n34, new_n35, new_n36, new_n37, new_n38, new_n39,
    new_n40, new_n41, new_n42, new_n43, new_n44, new_n46, new_n47, new_n49,
    new_n50, new_n51, new_n52, new_n53, new_n55, new_n56, new_n57, new_n58,
    new_n59, new_n60, new_n61, new_n62, new_n64, new_n65, new_n66, new_n67,
    new_n69, new_n70, new_n71;
  assign new_n16 = dmpst0 & dmpst2;
  assign new_n17 = ~dmpst3 & new_n16;
  assign new_n18 = ~dmpst1 & ~dmpst2;
  assign new_n19 = dmpst3 & new_n18;
  assign new_n20 = dmpst0 & new_n19;
  assign new_n21 = ~new_n17 & ~new_n20;
  assign new_n22 = ~dmpst1 & dmpst2;
  assign new_n23 = ~dmpst3 & new_n22;
  assign new_n24 = ~xskip & new_n23;
  assign new_n25 = dmpst1 & ~dmpst2;
  assign new_n26 = dmpst3 & new_n25;
  assign new_n27 = ~dmpst0 & new_n26;
  assign new_n28 = ~dmpst0 & dmpst1;
  assign new_n29 = ~dmpst2 & new_n28;
  assign new_n30 = ~yskip & new_n29;
  assign new_n31 = ~new_n24 & ~new_n27;
  assign new_n32 = ~new_n30 & new_n31;
  assign adctlp0b = ~new_n21 | ~new_n32;
  assign new_n34 = dmpst0 & dmpst1;
  assign new_n35 = ~dmpst3 & new_n34;
  assign new_n36 = ~new_n23 & ~new_n35;
  assign new_n37 = ~new_n20 & new_n36;
  assign new_n38 = ~dmpst3 & new_n25;
  assign new_n39 = yskip & new_n38;
  assign new_n40 = ~dmpst0 & ~dmpst1;
  assign new_n41 = ~dmpst3 & new_n40;
  assign new_n42 = page & new_n41;
  assign new_n43 = ~new_n27 & ~new_n39;
  assign new_n44 = ~new_n42 & new_n43;
  assign adctlp1b = ~new_n37 | ~new_n44;
  assign new_n46 = ~new_n29 & ~new_n41;
  assign new_n47 = ~new_n20 & new_n46;
  assign adctlp2b = ~new_n36 | ~new_n47;
  assign new_n49 = ~rmwb & new_n23;
  assign new_n50 = ~xskip & ~dmpst0;
  assign new_n51 = new_n23 & new_n50;
  assign new_n52 = ~new_n30 & ~new_n49;
  assign new_n53 = ~new_n51 & new_n52;
  assign dmnst0b = new_n27 | ~new_n53;
  assign new_n55 = ~yskip & ~dmpst0;
  assign new_n56 = new_n38 & new_n55;
  assign new_n57 = xskip & new_n23;
  assign new_n58 = ~page & ~dmpst0;
  assign new_n59 = ~dmpst3 & new_n18;
  assign new_n60 = new_n58 & new_n59;
  assign new_n61 = ~new_n56 & ~new_n57;
  assign new_n62 = ~new_n60 & new_n61;
  assign dmnst1b = ~new_n21 | ~new_n62;
  assign new_n64 = dmpst0 & new_n38;
  assign new_n65 = ~new_n23 & ~new_n64;
  assign new_n66 = ~new_n20 & ~new_n39;
  assign new_n67 = ~new_n42 & new_n66;
  assign dmnst2b = ~new_n65 | ~new_n67;
  assign new_n69 = dmpst1 & dmpst2;
  assign new_n70 = ~dmpst3 & new_n69;
  assign new_n71 = dmpst0 & new_n70;
  assign dmnst3b = new_n27 | new_n71;
endmodule


