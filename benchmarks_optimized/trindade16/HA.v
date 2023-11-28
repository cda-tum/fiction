// Benchmark "top" written by ABC on Mon Nov 27 12:21:49 2023

module top ( 
    x, y,
    s, cout  );
  input  x, y;
  output s, cout;
  wire new_n6;
  assign cout = x & y;
  assign new_n6 = ~x & ~y;
  assign s = ~cout & ~new_n6;
endmodule


