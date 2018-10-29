// Benchmark "b02" written by ABC on Wed Sep  5 10:17:19 2018

module b02 ( clock, 
    LINEA,
    U_REG  );
  input  clock;
  input  LINEA;
  output U_REG;
  reg U_REG, STATO_REG_2_, STATO_REG_1_, STATO_REG_0_;
  wire n15_1, n17, n18, n19, n20_1, n21, n22, n23, n25, n26, n27, n28, n29,
    n31, n32, n33, n34, n6, n10, n15, n20;
  assign n15_1 = ~STATO_REG_1_ & ~STATO_REG_0_;
  assign n6 = STATO_REG_2_ & n15_1;
  assign n17 = ~LINEA & STATO_REG_2_;
  assign n18 = LINEA & ~STATO_REG_2_;
  assign n19 = ~n17 & ~n18;
  assign n20_1 = ~STATO_REG_0_ & n19;
  assign n21 = STATO_REG_1_ & ~n20_1;
  assign n22 = ~LINEA & ~STATO_REG_2_;
  assign n23 = STATO_REG_0_ & ~n22;
  assign n10 = n21 | n23;
  assign n25 = ~STATO_REG_2_ & STATO_REG_1_;
  assign n26 = ~STATO_REG_0_ & n25;
  assign n27 = ~LINEA & ~STATO_REG_1_;
  assign n28 = ~STATO_REG_2_ & ~n27;
  assign n29 = STATO_REG_0_ & ~n28;
  assign n15 = n26 | n29;
  assign n31 = STATO_REG_0_ & ~n18;
  assign n32 = ~STATO_REG_1_ & ~n31;
  assign n33 = ~LINEA & ~STATO_REG_0_;
  assign n34 = ~STATO_REG_2_ & n33;
  assign n20 = n32 | n34;
  always @ (posedge clock) begin
    U_REG <= n6;
    STATO_REG_2_ <= n10;
    STATO_REG_1_ <= n15;
    STATO_REG_0_ <= n20;
  end
endmodule


