// Benchmark "top" written by ABC on Mon Nov 27 12:22:47 2023

module top ( 
    a, b, c, p,
    parity_check  );
  input  a, b, c, p;
  output parity_check;
  wire new_n6, new_n7, new_n8, new_n9, new_n10, new_n11;
  assign new_n6 = ~a & ~c;
  assign new_n7 = a & c;
  assign new_n8 = ~new_n6 & ~new_n7;
  assign new_n9 = ~p & ~new_n8;
  assign new_n10 = b & ~new_n9;
  assign new_n11 = ~b & new_n9;
  assign parity_check = ~new_n10 & ~new_n11;
endmodule


