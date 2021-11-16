// Benchmark "top" written by ABC on Fri Sep  4 10:02:12 2020

module top (
    x, y,
    XOR  );
  input  x, y;
  output XOR;
  wire new_n4_, new_n5_;
  assign new_n4_ = ~x & y;
  assign new_n5_ = x & ~y;
  assign XOR = new_n4_ | new_n5_;
endmodule


