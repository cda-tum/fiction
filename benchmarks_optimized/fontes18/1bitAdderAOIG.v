// Benchmark "top" written by ABC on Mon Nov 27 12:31:24 2023

module top ( 
    x, y, z,
    H, I  );
  input  x, y, z;
  output H, I;
  wire new_n6, new_n7, new_n8, new_n9, new_n10;
  assign new_n6 = y & z;
  assign new_n7 = ~y & ~z;
  assign new_n8 = ~new_n6 & ~new_n7;
  assign new_n9 = x & ~new_n8;
  assign new_n10 = ~x & new_n8;
  assign H = new_n9 | new_n10;
  assign I = ~new_n7 & ~new_n10;
endmodule


