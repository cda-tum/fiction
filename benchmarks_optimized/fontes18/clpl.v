// Benchmark "top" written by ABC on Mon Nov 27 12:33:31 2023

module top ( 
    pi00, pi01, pi02, pi03, pi04, pi05, pi06, pi07, pi08, pi09, pi10,
    po0, po1, po2, po3, po4  );
  input  pi00, pi01, pi02, pi03, pi04, pi05, pi06, pi07, pi08, pi09,
    pi10;
  output po0, po1, po2, po3, po4;
  wire new_n17, new_n19, new_n21, new_n23, new_n25;
  assign new_n17 = pi00 & pi04;
  assign po2 = pi02 | new_n17;
  assign new_n19 = pi05 & po2;
  assign po1 = pi03 | new_n19;
  assign new_n21 = pi06 & po1;
  assign po0 = pi01 | new_n21;
  assign new_n23 = pi08 & po0;
  assign po4 = pi07 | new_n23;
  assign new_n25 = pi09 & po4;
  assign po3 = pi10 | new_n25;
endmodule


