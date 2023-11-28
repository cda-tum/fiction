// Benchmark "top" written by ABC on Mon Nov 27 12:38:45 2023

module top ( 
    x, y,
    XOR  );
  input  x, y;
  output XOR;
  wire new_n4, new_n5;
  assign new_n4 = x & ~y;
  assign new_n5 = ~x & y;
  assign XOR = new_n4 | new_n5;
endmodule


