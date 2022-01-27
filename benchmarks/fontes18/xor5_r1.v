// Benchmark "top" written by ABC on Fri Sep  4 10:02:12 2020

module top  (
    pi0, pi1, pi2, pi3, pi4,
    po0  );
  input  pi0, pi1, pi2, pi3, pi4;
  output po0;
  wire new_n7_, new_n8_, new_n9_, new_n10_, new_n11_, new_n12_, new_n13_,
    new_n14_, new_n15_, new_n16_, new_n17_;
  assign new_n7_ = ~pi3 & pi4;
  assign new_n8_ = pi3 & ~pi4;
  assign new_n9_ = ~new_n7_ & ~new_n8_;
  assign new_n10_ = ~pi1 & pi2;
  assign new_n11_ = pi1 & ~pi2;
  assign new_n12_ = ~new_n10_ & ~new_n11_;
  assign new_n13_ = pi0 & ~new_n12_;
  assign new_n14_ = ~pi0 & new_n12_;
  assign new_n15_ = ~new_n13_ & ~new_n14_;
  assign new_n16_ = new_n9_ & new_n15_;
  assign new_n17_ = ~new_n9_ & ~new_n15_;
  assign po0 = new_n16_ | new_n17_;
endmodule


