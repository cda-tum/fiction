// Benchmark "top" written by ABC on Mon Feb 19 11:52:43 2024

module top ( 
    pv27_3, pv7_2, pv7_1, pv10_0, pv11_0, pv14_0, pv22_5, pv27_0, pv7_7,
    pv15_0, pv22_4, pv12_0, pv29_0, pv13_0, pv7_4, pv18_0, pv7_3, pv8_0,
    pv7_6, pv9_0, pv16_0, pv22_3, pv7_5, pv17_0, pv22_2,
    pv27_4, pv32_0, pv33_0, pv34_0, pv35_0, pv36_0, pv37_0, pv38_0, pv27_2,
    pv27_1, pv28_0, pv30_0, pv31_0  );
  input  pv27_3, pv7_2, pv7_1, pv10_0, pv11_0, pv14_0, pv22_5, pv27_0,
    pv7_7, pv15_0, pv22_4, pv12_0, pv29_0, pv13_0, pv7_4, pv18_0, pv7_3,
    pv8_0, pv7_6, pv9_0, pv16_0, pv22_3, pv7_5, pv17_0, pv22_2;
  output pv27_4, pv32_0, pv33_0, pv34_0, pv35_0, pv36_0, pv37_0, pv38_0,
    pv27_2, pv27_1, pv28_0, pv30_0, pv31_0;
  wire new_n41, new_n43, new_n45, new_n47, new_n50, new_n51, new_n53,
    new_n54, new_n55, new_n56, new_n57, new_n58, new_n59, new_n60, new_n61,
    new_n62, new_n63, new_n65, new_n66, new_n67, new_n68, new_n69, new_n70,
    new_n74;
  assign pv27_4 = pv27_3 | pv22_2;
  assign pv32_0 = pv11_0 & pv22_5;
  assign new_n41 = pv14_0 & pv22_3;
  assign pv33_0 = ~pv22_5 & new_n41;
  assign new_n43 = pv22_3 & pv17_0;
  assign pv34_0 = ~pv22_5 & new_n43;
  assign new_n45 = pv14_0 & pv22_4;
  assign pv35_0 = ~pv22_5 & new_n45;
  assign new_n47 = pv22_4 & pv17_0;
  assign pv36_0 = ~pv22_5 & new_n47;
  assign pv37_0 = ~pv22_5 & pv16_0;
  assign new_n50 = ~pv14_0 & ~pv13_0;
  assign new_n51 = ~pv12_0 & new_n50;
  assign pv38_0 = pv15_0 | ~new_n51;
  assign new_n53 = ~pv7_2 & ~pv7_1;
  assign new_n54 = ~pv7_5 & new_n53;
  assign new_n55 = ~pv7_3 & new_n54;
  assign new_n56 = ~pv7_7 & new_n55;
  assign new_n57 = ~pv7_4 & new_n56;
  assign new_n58 = ~pv7_6 & new_n57;
  assign new_n59 = pv27_0 & pv29_0;
  assign new_n60 = ~new_n58 & new_n59;
  assign new_n61 = pv8_0 & new_n58;
  assign new_n62 = pv29_0 & new_n61;
  assign new_n63 = ~pv9_0 & new_n62;
  assign pv27_2 = new_n60 | new_n63;
  assign new_n65 = ~pv27_0 & pv29_0;
  assign new_n66 = pv29_0 & new_n58;
  assign new_n67 = ~pv8_0 & new_n66;
  assign new_n68 = ~pv9_0 & new_n67;
  assign new_n69 = pv9_0 & new_n62;
  assign new_n70 = ~new_n65 & ~new_n68;
  assign pv27_1 = new_n69 | ~new_n70;
  assign pv28_0 = pv10_0 | new_n67;
  assign pv30_0 = pv22_5 & pv18_0;
  assign new_n74 = pv11_0 & ~pv22_5;
  assign pv31_0 = pv32_0 | new_n74;
endmodule


