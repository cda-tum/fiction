// Benchmark "top" written by ABC on Mon Nov 27 12:32:01 2023

module top ( 
    A, B, Cin,
    M3  );
  input  A, B, Cin;
  output M3;
  wire new_n5, new_n6, new_n7;
  assign new_n5 = B & ~Cin;
  assign new_n6 = ~B & Cin;
  assign new_n7 = A & ~new_n6;
  assign M3 = new_n5 | new_n7;
endmodule


