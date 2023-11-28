// Benchmark "top" written by ABC on Mon Nov 27 16:49:01 2023

module top ( 
    \1 , \2 , \3 , \6 , \7 ,
    \22 , \23   );
  input  \1 , \2 , \3 , \6 , \7 ;
  output \22 , \23 ;
  wire new_n8, new_n9, new_n10, new_n12;
  assign new_n8 = \1  & \3 ;
  assign new_n9 = \3  & \6 ;
  assign new_n10 = \2  & ~new_n9;
  assign \22  = new_n8 | new_n10;
  assign new_n12 = \7  & ~new_n9;
  assign \23  = new_n10 | new_n12;
endmodule


