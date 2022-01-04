// Benchmark "top" written by ABC on Fri Sep  4 10:02:12 2020

module top ( 
    x, y, z,
    H, I  );
  input  x, y, z;
  output H, I;
  wire new_n6_, new_n7_, new_n8_, new_n9_, new_n10_;
  assign new_n6_ = ~y & ~z;
  assign new_n7_ = y & z;
  assign new_n8_ = ~new_n6_ & ~new_n7_;
  assign new_n9_ = ~x & ~new_n8_;
  assign new_n10_ = x & new_n8_;
  assign H = ~new_n9_ & ~new_n10_;
  assign I = new_n7_ | new_n10_;
endmodule


