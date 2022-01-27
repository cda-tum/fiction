// Benchmark "top" written by ABC on Fri Sep  4 10:02:12 2020

module top (
    pi0, pi1, pi2, pi3, pi4,
    po0  );
  input  pi0, pi1, pi2, pi3, pi4;
  output po0;
  wire new_n7_, new_n8_, new_n9_, new_n10_, new_n11_, new_n12_, new_n13_;
  assign new_n7_ = pi2 & pi4;
  assign new_n8_ = pi1 & new_n7_;
  assign new_n9_ = ~pi0 & ~new_n8_;
  assign new_n10_ = ~pi2 & ~pi4;
  assign new_n11_ = pi1 & ~new_n10_;
  assign new_n12_ = ~new_n7_ & ~new_n11_;
  assign new_n13_ = ~new_n9_ & ~new_n12_;
  assign po0 = pi3 | new_n13_;
endmodule


