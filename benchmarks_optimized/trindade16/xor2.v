// Benchmark "top" written by ABC on Mon Nov 27 12:23:46 2023

module top ( 
    a, b,
    out  );
  input  a, b;
  output out;
  wire new_n4, new_n5;
  assign new_n4 = ~a & ~b;
  assign new_n5 = a & b;
  assign out = ~new_n4 & ~new_n5;
endmodule


