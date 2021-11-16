// Benchmark "top" written by ABC on Fri Sep  4 10:02:12 2020

module top  (
    pi0, pi1, pi2, pi3, pi4, pi5, pi6, pi7,
    po0  );
  input  pi0, pi1, pi2, pi3, pi4, pi5, pi6, pi7;
  output po0;
  wire new_n10_, new_n11_, new_n12_, new_n13_, new_n14_, new_n15_, new_n16_,
    new_n17_;
  assign new_n10_ = pi6 & pi7;
  assign new_n11_ = ~pi4 & ~pi5;
  assign new_n12_ = new_n10_ & ~new_n11_;
  assign new_n13_ = pi4 & pi5;
  assign new_n14_ = ~new_n12_ & ~new_n13_;
  assign new_n15_ = pi3 & new_n14_;
  assign new_n16_ = pi1 & ~pi2;
  assign new_n17_ = ~pi0 & new_n16_;
  assign po0 = new_n15_ | ~new_n17_;
endmodule


