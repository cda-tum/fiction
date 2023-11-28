// Benchmark "top" written by ABC on Mon Nov 27 12:36:29 2023

module top ( 
    x, y, z, p, q,
    M6  );
  input  x, y, z, p, q;
  output M6;
  wire new_n7, new_n8, new_n9, new_n10, new_n11, new_n12, new_n13, new_n14,
    new_n15, new_n16, new_n17;
  assign new_n7 = ~x & ~y;
  assign new_n8 = x & y;
  assign new_n9 = ~new_n7 & ~new_n8;
  assign new_n10 = z & new_n9;
  assign new_n11 = ~z & ~new_n9;
  assign new_n12 = ~new_n10 & ~new_n11;
  assign new_n13 = p & ~q;
  assign new_n14 = ~p & q;
  assign new_n15 = ~new_n13 & ~new_n14;
  assign new_n16 = new_n12 & new_n15;
  assign new_n17 = ~new_n12 & ~new_n15;
  assign M6 = new_n16 | new_n17;
endmodule


