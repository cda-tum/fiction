// Benchmark "top" written by ABC on Mon Nov 27 12:23:08 2023

module top ( 
    a, b, c,
    parity  );
  input  a, b, c;
  output parity;
  wire new_n5, new_n6, new_n7, new_n8, new_n9;
  assign new_n5 = b & ~c;
  assign new_n6 = ~b & c;
  assign new_n7 = ~new_n5 & ~new_n6;
  assign new_n8 = a & ~new_n7;
  assign new_n9 = ~a & new_n7;
  assign parity = ~new_n8 & ~new_n9;
endmodule


