// Benchmark "b06" written by ABC on Wed Sep  5 10:17:19 2018

module b06 ( clock, 
    EQL, CONT_EQL,
    CC_MUX_REG_2_, CC_MUX_REG_1_, USCITE_REG_2_, USCITE_REG_1_,
    ENABLE_COUNT_REG, ACKOUT_REG  );
  input  clock;
  input  EQL, CONT_EQL;
  output CC_MUX_REG_2_, CC_MUX_REG_1_, USCITE_REG_2_, USCITE_REG_1_,
    ENABLE_COUNT_REG, ACKOUT_REG;
  reg ACKOUT_REG, STATE_REG_2_, STATE_REG_1_, STATE_REG_0_, CC_MUX_REG_2_,
    CC_MUX_REG_1_, USCITE_REG_2_, USCITE_REG_1_, ENABLE_COUNT_REG;
  wire n36, n37_1, n38, n39, n40, n41_1, n43, n44, n45_1, n46, n47, n48, n50,
    n51, n52_1, n53, n54, n55, n56, n57, n58, n60, n61, n62, n64, n65, n67,
    n68, n69, n70, n71, n72, n74, n76, n18, n22, n27, n32, n37, n41, n45,
    n49, n52;
  assign n36 = STATE_REG_2_ & STATE_REG_1_;
  assign n37_1 = STATE_REG_0_ & n36;
  assign n38 = ~CONT_EQL & ~n37_1;
  assign n39 = ~STATE_REG_2_ & ~STATE_REG_0_;
  assign n40 = ~EQL & STATE_REG_1_;
  assign n41_1 = n39 & n40;
  assign n18 = n38 | n41_1;
  assign n43 = ~STATE_REG_1_ & ~STATE_REG_0_;
  assign n44 = ~EQL & ~n43;
  assign n45_1 = STATE_REG_2_ & ~n44;
  assign n46 = ~STATE_REG_2_ & ~STATE_REG_1_;
  assign n47 = STATE_REG_0_ & n46;
  assign n48 = ~EQL & n47;
  assign n22 = n45_1 | n48;
  assign n50 = STATE_REG_1_ & n39;
  assign n51 = EQL & ~STATE_REG_2_;
  assign n52_1 = STATE_REG_0_ & n51;
  assign n53 = STATE_REG_2_ & ~STATE_REG_0_;
  assign n54 = ~STATE_REG_1_ & n53;
  assign n55 = ~EQL & n54;
  assign n56 = EQL & STATE_REG_1_;
  assign n57 = ~n50 & ~n52_1;
  assign n58 = ~n55 & n57;
  assign n27 = n56 | ~n58;
  assign n60 = STATE_REG_1_ & STATE_REG_0_;
  assign n61 = ~STATE_REG_1_ & n39;
  assign n62 = ~n60 & ~n61;
  assign n32 = n44 | ~n62;
  assign n64 = ~STATE_REG_0_ & n56;
  assign n65 = ~n45_1 & ~n47;
  assign n37 = n64 | ~n65;
  assign n67 = STATE_REG_2_ & STATE_REG_0_;
  assign n68 = EQL & ~STATE_REG_0_;
  assign n69 = STATE_REG_1_ & ~n68;
  assign n70 = EQL & ~STATE_REG_1_;
  assign n71 = ~n39 & ~n67;
  assign n72 = ~n69 & n71;
  assign n41 = n70 | ~n72;
  assign n74 = STATE_REG_2_ & n56;
  assign n45 = n55 | n74;
  assign n76 = EQL & ~n36;
  assign n49 = ~n62 | ~n76;
  assign n52 = n18;
  always @ (posedge clock) begin
    ACKOUT_REG <= n18;
    STATE_REG_2_ <= n22;
    STATE_REG_1_ <= n27;
    STATE_REG_0_ <= n32;
    CC_MUX_REG_2_ <= n37;
    CC_MUX_REG_1_ <= n41;
    USCITE_REG_2_ <= n45;
    USCITE_REG_1_ <= n49;
    ENABLE_COUNT_REG <= n52;
  end
endmodule


