// Benchmark "top" written by ABC on Fri Sep  4 10:02:12 2020

module top (
    A, B, C, D, E,
    M3, M6  );
  input  A, B, C, D, E;
  output M3, M6;
  wire new_n8_, new_n9_, new_n10_, new_n11_, new_n12_, new_n13_, new_n14_,
    new_n15_, new_n16_, new_n17_, new_n18_, new_n20_, new_n21_, new_n22_,
    new_n23_, new_n24_, new_n25_, new_n26_, new_n27_, new_n28_, new_n29_,
    new_n30_;
  assign new_n8_ = A & B;
  assign new_n9_ = A & C;
  assign new_n10_ = ~new_n8_ & ~new_n9_;
  assign new_n11_ = B & C;
  assign new_n12_ = new_n10_ & ~new_n11_;
  assign new_n13_ = ~new_n9_ & ~new_n11_;
  assign new_n14_ = ~new_n8_ & new_n13_;
  assign new_n15_ = ~new_n12_ & ~new_n14_;
  assign new_n16_ = C & ~new_n12_;
  assign new_n17_ = ~new_n15_ & ~new_n16_;
  assign new_n18_ = C & ~new_n14_;
  assign M3 = ~new_n17_ | new_n18_;
  assign new_n20_ = D & E;
  assign new_n21_ = D & ~new_n12_;
  assign new_n22_ = ~new_n20_ & ~new_n21_;
  assign new_n23_ = E & ~new_n12_;
  assign new_n24_ = new_n22_ & ~new_n23_;
  assign new_n25_ = ~new_n20_ & ~new_n23_;
  assign new_n26_ = ~new_n21_ & new_n25_;
  assign new_n27_ = ~new_n24_ & ~new_n26_;
  assign new_n28_ = ~new_n12_ & ~new_n24_;
  assign new_n29_ = ~new_n12_ & ~new_n26_;
  assign new_n30_ = ~new_n28_ & ~new_n29_;
  assign M6 = new_n27_ | ~new_n30_;
endmodule


