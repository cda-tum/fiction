// Benchmark "top" written by ABC on Fri Sep  4 10:02:12 2020

module top (
    A, B, Cin,
    M3  );
  input  A, B, Cin;
  output M3;
  wire new_n5_, new_n6_, new_n7_, new_n8_, new_n9_, new_n10_, new_n11_,
    new_n12_, new_n13_, new_n14_, new_n15_, new_n16_, new_n17_;
  assign new_n5_ = A & B;
  assign new_n6_ = A & Cin;
  assign new_n7_ = ~new_n5_ & ~new_n6_;
  assign new_n8_ = B & Cin;
  assign new_n9_ = new_n7_ & ~new_n8_;
  assign new_n10_ = A & new_n9_;
  assign new_n11_ = B & new_n9_;
  assign new_n12_ = ~new_n10_ & ~new_n11_;
  assign new_n13_ = ~new_n5_ & new_n12_;
  assign new_n14_ = ~new_n9_ & ~new_n13_;
  assign new_n15_ = ~Cin & ~new_n13_;
  assign new_n16_ = ~Cin & ~new_n9_;
  assign new_n17_ = ~new_n15_ & ~new_n16_;
  assign M3 = new_n14_ | ~new_n17_;
endmodule


