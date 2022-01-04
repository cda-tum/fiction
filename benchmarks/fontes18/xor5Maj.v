// Benchmark "top" written by ABC on Fri Sep  4 10:02:12 2020

module top ( 
    x, y, z, p, q,
    M6  );
  input  x, y, z, p, q;
  output M6;
  wire new_n7_, new_n8_, new_n9_, new_n10_, new_n11_, new_n12_, new_n13_,
    new_n14_, new_n15_, new_n16_, new_n17_, new_n18_, new_n19_, new_n20_,
    new_n21_, new_n22_, new_n23_, new_n24_, new_n25_, new_n26_, new_n27_,
    new_n28_, new_n29_, new_n30_, new_n31_, new_n32_, new_n33_, new_n34_,
    new_n35_;
  assign new_n7_ = ~x & ~y;
  assign new_n8_ = ~y & ~z;
  assign new_n9_ = ~new_n7_ & ~new_n8_;
  assign new_n10_ = ~x & ~z;
  assign new_n11_ = new_n9_ & ~new_n10_;
  assign new_n12_ = ~x & y;
  assign new_n13_ = y & z;
  assign new_n14_ = ~new_n12_ & ~new_n13_;
  assign new_n15_ = ~x & z;
  assign new_n16_ = new_n14_ & ~new_n15_;
  assign new_n17_ = ~new_n11_ & ~new_n16_;
  assign new_n18_ = x & ~new_n11_;
  assign new_n19_ = x & ~new_n16_;
  assign new_n20_ = ~new_n18_ & ~new_n19_;
  assign new_n21_ = ~new_n17_ & new_n20_;
  assign new_n22_ = ~p & new_n21_;
  assign new_n23_ = ~q & new_n21_;
  assign new_n24_ = ~p & ~q;
  assign new_n25_ = ~new_n23_ & ~new_n24_;
  assign new_n26_ = ~new_n22_ & new_n25_;
  assign new_n27_ = p & q;
  assign new_n28_ = p & new_n21_;
  assign new_n29_ = ~new_n27_ & ~new_n28_;
  assign new_n30_ = q & new_n21_;
  assign new_n31_ = new_n29_ & ~new_n30_;
  assign new_n32_ = ~new_n26_ & ~new_n31_;
  assign new_n33_ = ~new_n21_ & ~new_n26_;
  assign new_n34_ = ~new_n21_ & ~new_n31_;
  assign new_n35_ = ~new_n33_ & ~new_n34_;
  assign M6 = new_n32_ | ~new_n35_;
endmodule


