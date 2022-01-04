// Benchmark "top" written by ABC on Fri Sep  4 10:02:12 2020

module top (
    pi0, pi1, pi2,
    po0, po1, po2, po3  );
  input  pi0, pi1, pi2;
  output po0, po1, po2, po3;
  wire new_n8_, new_n9_, new_n11_, new_n12_;
  assign new_n8_ = pi0 & pi1;
  assign new_n9_ = ~pi0 & ~pi1;
  assign po1 = ~new_n8_ & ~new_n9_;
  assign new_n11_ = ~pi2 & new_n8_;
  assign new_n12_ = pi2 & new_n9_;
  assign po2 = new_n11_ | new_n12_;
  assign po3 = ~pi2;
  assign po0 = pi2;
endmodule


