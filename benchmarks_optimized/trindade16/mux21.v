// Benchmark "top" written by ABC on Mon Nov 27 12:22:23 2023

module top ( 
    in0, in1, in2,
    out  );
  input  in0, in1, in2;
  output out;
  wire new_n5, new_n6;
  assign new_n5 = ~in0 & ~in2;
  assign new_n6 = ~in1 & in2;
  assign out = ~new_n5 & ~new_n6;
endmodule


