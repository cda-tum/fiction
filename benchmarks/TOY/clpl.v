module top(pi00, pi01, pi02, pi03, pi04, pi05, pi06, pi07, pi08, pi09, pi10, po0, po1, po2, po3, po4);
  input  pi00, pi01, pi02, pi03, pi04, pi05, pi06, pi07, pi08, pi09, pi10;
  output po0, po1, po2, po3, po4;
  wire n18, n20, n22, n24, n26;
  assign n18 = pi00 & pi04;
  assign po2 = n18 | pi02;
  assign n20 = po2 & pi05;
  assign po1 = n20 | pi03;
  assign n22 = po1 & pi06;
  assign po0 = n22 | pi01;
  assign n24 = po0 & pi08;
  assign po4 = n24 | pi07;
  assign n26 = po4 & pi09;
  assign po3 = n26 | pi10;
endmodule


