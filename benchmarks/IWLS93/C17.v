// Benchmark "top" written by ABC on Mon Feb 19 11:52:42 2024

module top ( 
    p_1gat_0_, p_6gat_3_, p_7gat_4_, p_2gat_1_, p_3gat_2_,
    p_22gat_10_, p_23gat_9_  );
  input  p_1gat_0_, p_6gat_3_, p_7gat_4_, p_2gat_1_, p_3gat_2_;
  output p_22gat_10_, p_23gat_9_;
  wire new_n8, new_n9, new_n10, new_n12;
  assign new_n8 = p_6gat_3_ & p_3gat_2_;
  assign new_n9 = p_2gat_1_ & ~new_n8;
  assign new_n10 = p_1gat_0_ & p_3gat_2_;
  assign p_22gat_10_ = new_n9 | new_n10;
  assign new_n12 = p_7gat_4_ & ~new_n8;
  assign p_23gat_9_ = new_n9 | new_n12;
endmodule


