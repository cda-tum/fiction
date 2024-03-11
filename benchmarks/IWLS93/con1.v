// Benchmark "top" written by ABC on Mon Feb 19 11:52:42 2024

module top ( 
    a, b, c, d, f, g, h,
    f0, f1  );
  input  a, b, c, d, f, g, h;
  output f0, f1;
  wire new_n10, new_n11, new_n12, new_n13, new_n14, new_n15, new_n16,
    new_n17, new_n18, new_n20, new_n21, new_n22, new_n23, new_n24, new_n25,
    new_n26, new_n27, new_n28;
  assign new_n10 = a & b;
  assign new_n11 = ~c & d;
  assign new_n12 = ~b & new_n11;
  assign new_n13 = c & d;
  assign new_n14 = f & new_n13;
  assign new_n15 = b & h;
  assign new_n16 = ~f & new_n15;
  assign new_n17 = ~new_n12 & ~new_n14;
  assign new_n18 = ~new_n16 & new_n17;
  assign f0 = new_n10 | ~new_n18;
  assign new_n20 = ~a & ~b;
  assign new_n21 = ~a & f;
  assign new_n22 = ~new_n20 & ~new_n21;
  assign new_n23 = ~f & new_n10;
  assign new_n24 = ~f & ~g;
  assign new_n25 = ~b & ~d;
  assign new_n26 = f & new_n25;
  assign new_n27 = ~new_n23 & ~new_n24;
  assign new_n28 = ~new_n26 & new_n27;
  assign f1 = ~new_n22 | ~new_n28;
endmodule


