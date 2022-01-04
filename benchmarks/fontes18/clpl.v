// Benchmark "top" written by ABC on Fri Sep  4 10:02:12 2020

module top  (
    pi00, pi01, pi02, pi03, pi04, pi05, pi06, pi07, pi08, pi09, pi10,
    po0, po1, po2, po3, po4  );
  input  pi00, pi01, pi02, pi03, pi04, pi05, pi06, pi07, pi08, pi09,
    pi10;
  output po0, po1, po2, po3, po4;
  wire new_n17_, new_n19_, new_n21_, new_n23_, new_n25_;
  assign new_n17_ = pi00 & pi04;
  assign po2 = pi02 | new_n17_;
  assign new_n19_ = pi05 & po2;
  assign po1 = pi03 | new_n19_;
  assign new_n21_ = pi06 & po1;
  assign po0 = pi01 | new_n21_;
  assign new_n23_ = pi08 & po0;
  assign po4 = pi07 | new_n23_;
  assign new_n25_ = pi09 & po4;
  assign po3 = pi10 | new_n25_;
endmodule


