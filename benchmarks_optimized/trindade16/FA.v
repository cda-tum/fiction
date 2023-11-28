// Benchmark "top" written by ABC on Mon Nov 27 12:21:23 2023

module top ( 
    x, y, cin,
    s, cout  );
  input  x, y, cin;
  output s, cout;
  wire new_n6, new_n7, new_n8, new_n9, new_n10;
  assign new_n6 = y & cin;
  assign new_n7 = ~y & ~cin;
  assign new_n8 = ~new_n6 & ~new_n7;
  assign new_n9 = x & ~new_n8;
  assign new_n10 = ~x & new_n8;
  assign s = new_n9 | new_n10;
  assign cout = ~new_n7 & ~new_n10;
endmodule


