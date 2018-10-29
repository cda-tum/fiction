// Benchmark "b01" written by ABC on Wed Sep  5 10:17:19 2018

module b01 ( clock, 
    LINE1, LINE2,
    OUTP_REG, OVERFLW_REG  );
  input  clock;
  input  LINE1, LINE2;
  output OUTP_REG, OVERFLW_REG;
  reg OVERFLW_REG, STATO_REG_2_, STATO_REG_1_, STATO_REG_0_, OUTP_REG;
  wire n20, n22, n23, n24_1, n25, n26, n27, n28, n29_1, n31, n32, n33, n34,
    n35, n36, n37, n38, n39, n41, n42, n43, n44, n45, n46, n47, n48, n49,
    n50, n51, n53, n54, n55, n56, n57, n58, n10, n14, n19, n24, n29;
  assign n20 = ~STATO_REG_2_ & STATO_REG_1_;
  assign n10 = STATO_REG_0_ & n20;
  assign n22 = ~LINE1 & ~LINE2;
  assign n23 = ~STATO_REG_0_ & n22;
  assign n24_1 = ~STATO_REG_1_ & ~n23;
  assign n25 = STATO_REG_2_ & n24_1;
  assign n26 = LINE1 & LINE2;
  assign n27 = STATO_REG_1_ & ~STATO_REG_0_;
  assign n28 = ~n26 & ~n27;
  assign n29_1 = ~STATO_REG_2_ & ~n28;
  assign n14 = n25 | n29_1;
  assign n31 = ~STATO_REG_0_ & ~n22;
  assign n32 = STATO_REG_1_ & n22;
  assign n33 = ~n31 & ~n32;
  assign n34 = STATO_REG_2_ & n33;
  assign n35 = ~STATO_REG_1_ & STATO_REG_0_;
  assign n36 = ~n26 & n35;
  assign n37 = STATO_REG_2_ & ~n26;
  assign n38 = n27 & ~n37;
  assign n39 = ~n34 & ~n36;
  assign n19 = n38 | ~n39;
  assign n41 = n26 & n27;
  assign n42 = n10 & ~n26;
  assign n43 = ~n22 & ~n27;
  assign n44 = STATO_REG_2_ & n43;
  assign n45 = ~STATO_REG_0_ & n26;
  assign n46 = STATO_REG_0_ & ~n26;
  assign n47 = ~n45 & ~n46;
  assign n48 = ~STATO_REG_1_ & n47;
  assign n49 = ~STATO_REG_2_ & n48;
  assign n50 = ~n41 & ~n42;
  assign n51 = ~n44 & n50;
  assign n24 = n49 | ~n51;
  assign n53 = ~LINE1 & LINE2;
  assign n54 = LINE1 & ~LINE2;
  assign n55 = ~n53 & ~n54;
  assign n56 = STATO_REG_2_ & ~n27;
  assign n57 = ~n55 & ~n56;
  assign n58 = n55 & n56;
  assign n29 = n57 | n58;
  always @ (posedge clock) begin
    OVERFLW_REG <= n10;
    STATO_REG_2_ <= n14;
    STATO_REG_1_ <= n19;
    STATO_REG_0_ <= n24;
    OUTP_REG <= n29;
  end
endmodule


