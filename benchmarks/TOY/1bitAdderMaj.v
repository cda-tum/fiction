module top (A, B, Cin, M3);
  input  A, B, Cin;
  output M3;
  wire M11, M12, M13, M14, M1, M21, M22, M23, M2, M31, M32, M33, M34 ;
  assign M11 = A & B;
  assign M12 = A & Cin;
  assign M13 = B & Cin;
  assign M14 = M11 | M12;
  assign M1 = M13 | M14;
  assign M21 = ~M1 & A;
  assign M22 = ~M1 & B;
  assign M23 = M21 | M22;
  assign M2 = M23 | M11;
  assign M31 = ~Cin & M2;
  assign M32 = ~Cin & M1;
  assign M33 = M2 & M1;
  assign M34 = M31 | M32;
  assign M3 = M33 | M34;
endmodule
