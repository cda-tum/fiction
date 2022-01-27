module top ( 
    \1 , 2, 3, 6, 7,
    22, 23  );
  input  \1 , 2, 3, 6, 7;
  output 22, 23;
  wire n8, n9, n10, n12;
  assign n8 = \1  & 3;
  assign n9 = 3 & 6;
  assign n10 = 2 & ~n9;
  assign 22 = n8 | n10;
  assign n12 = 7 & ~n9;
  assign 23 = n10 | n12;
endmodule


