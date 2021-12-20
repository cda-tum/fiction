// Benchmark "top" written by ABC on Fri Sep  4 10:02:12 2020

module top  (
    pi0, pi1, pi2, pi3, pi4,
    po0, po1  );
  input  pi0, pi1, pi2, pi3, pi4;
  output po0, po1;
  wire new_n8_, new_n9_, new_n10_, new_n12_;
  assign new_n8_ = pi0 & pi2;
  assign new_n9_ = pi2 & pi3;
  assign new_n10_ = pi1 & ~new_n9_;
  assign po0 = new_n8_ | new_n10_;
  assign new_n12_ = ~pi1 & ~pi4;
  assign po1 = ~new_n9_ & ~new_n12_;
endmodule


