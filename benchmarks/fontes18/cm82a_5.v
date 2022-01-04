// Benchmark "top" written by ABC on Fri Sep  4 10:02:12 2020

module top (
    pi0, pi1, pi2, pi3, pi4,
    po0, po1, po2  );
  input  pi0, pi1, pi2, pi3, pi4;
  output po0, po1, po2;
  wire new_n9_, new_n10_, new_n11_, new_n12_, new_n13_, new_n15_, new_n16_,
    new_n17_, new_n18_, new_n19_, new_n20_, new_n21_, new_n22_, new_n23_,
    new_n25_, new_n26_, new_n27_;
  assign new_n9_ = pi1 & ~pi2;
  assign new_n10_ = ~pi1 & pi2;
  assign new_n11_ = ~new_n9_ & ~new_n10_;
  assign new_n12_ = pi0 & new_n11_;
  assign new_n13_ = ~pi0 & ~new_n11_;
  assign po0 = new_n12_ | new_n13_;
  assign new_n15_ = pi3 & ~pi4;
  assign new_n16_ = ~pi3 & pi4;
  assign new_n17_ = ~new_n15_ & ~new_n16_;
  assign new_n18_ = ~pi1 & ~pi2;
  assign new_n19_ = pi0 & ~new_n18_;
  assign new_n20_ = pi1 & pi2;
  assign new_n21_ = ~new_n19_ & ~new_n20_;
  assign new_n22_ = new_n17_ & ~new_n21_;
  assign new_n23_ = ~new_n17_ & new_n21_;
  assign po1 = new_n22_ | new_n23_;
  assign new_n25_ = ~pi3 & ~pi4;
  assign new_n26_ = pi3 & pi4;
  assign new_n27_ = new_n21_ & ~new_n26_;
  assign po2 = ~new_n25_ & ~new_n27_;
endmodule


