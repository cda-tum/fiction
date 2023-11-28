// Benchmark "top" written by ABC on Mon Nov 27 17:03:16 2023

module top ( 
    opcode0, opcode1, opcode2, opcode3, opcode4, op_ext0, op_ext1,
    sel_reg_dst0, sel_reg_dst1, sel_alu_opB0, sel_alu_opB1, alu_op0,
    alu_op1, alu_op2, alu_op_ext0, alu_op_ext1, alu_op_ext2, alu_op_ext3,
    halt, reg_write, sel_pc_opA, sel_pc_opB, beqz, bnez, bgez, bltz, jump,
    Cin, invA, invB, mem_write, sel_wb  );
  input  opcode0, opcode1, opcode2, opcode3, opcode4, op_ext0, op_ext1;
  output sel_reg_dst0, sel_reg_dst1, sel_alu_opB0, sel_alu_opB1, alu_op0,
    alu_op1, alu_op2, alu_op_ext0, alu_op_ext1, alu_op_ext2, alu_op_ext3,
    halt, reg_write, sel_pc_opA, sel_pc_opB, beqz, bnez, bgez, bltz, jump,
    Cin, invA, invB, mem_write, sel_wb;
  wire new_n33, new_n34, new_n35, new_n36, new_n37, new_n38, new_n39,
    new_n41, new_n42, new_n43, new_n44, new_n45, new_n47, new_n48, new_n49,
    new_n50, new_n51, new_n52, new_n54, new_n55, new_n56, new_n58, new_n59,
    new_n60, new_n61, new_n62, new_n63, new_n64, new_n66, new_n67, new_n68,
    new_n69, new_n70, new_n71, new_n72, new_n74, new_n75, new_n76, new_n77,
    new_n79, new_n80, new_n82, new_n83, new_n85, new_n86, new_n90, new_n91,
    new_n93, new_n94, new_n100, new_n104, new_n105, new_n106, new_n107,
    new_n108;
  assign new_n33 = opcode3 & opcode4;
  assign new_n34 = ~opcode0 & ~opcode1;
  assign new_n35 = ~opcode2 & new_n34;
  assign new_n36 = new_n33 & ~new_n35;
  assign new_n37 = ~opcode3 & ~opcode4;
  assign new_n38 = opcode1 & opcode2;
  assign new_n39 = new_n37 & new_n38;
  assign sel_reg_dst0 = new_n36 | new_n39;
  assign new_n41 = new_n33 & new_n35;
  assign new_n42 = ~new_n39 & ~new_n41;
  assign new_n43 = ~opcode3 & opcode4;
  assign new_n44 = ~opcode2 & new_n43;
  assign new_n45 = opcode1 & new_n44;
  assign sel_reg_dst1 = ~new_n42 | new_n45;
  assign new_n47 = opcode3 & ~opcode4;
  assign new_n48 = ~opcode2 & new_n47;
  assign new_n49 = opcode1 & new_n48;
  assign new_n50 = ~opcode0 & opcode1;
  assign new_n51 = new_n44 & new_n50;
  assign new_n52 = ~new_n41 & ~new_n49;
  assign sel_alu_opB0 = new_n51 | ~new_n52;
  assign new_n54 = ~opcode2 & opcode3;
  assign new_n55 = ~new_n43 & ~new_n54;
  assign new_n56 = ~new_n49 & ~new_n55;
  assign sel_alu_opB1 = ~sel_reg_dst0 & new_n56;
  assign new_n58 = opcode0 & new_n43;
  assign new_n59 = opcode2 & ~new_n58;
  assign new_n60 = opcode4 & ~op_ext0;
  assign new_n61 = new_n38 & new_n43;
  assign new_n62 = opcode4 & ~op_ext1;
  assign new_n63 = opcode1 & new_n54;
  assign new_n64 = ~new_n62 & new_n63;
  assign alu_op1 = new_n61 | new_n64;
  assign new_n66 = ~new_n60 & alu_op1;
  assign new_n67 = ~opcode2 & ~new_n66;
  assign new_n68 = ~opcode0 & alu_op1;
  assign new_n69 = ~new_n67 & ~new_n68;
  assign new_n70 = op_ext0 & new_n33;
  assign new_n71 = new_n50 & new_n70;
  assign new_n72 = ~new_n69 & ~new_n71;
  assign alu_op0 = ~new_n59 & ~new_n72;
  assign new_n74 = opcode0 & opcode1;
  assign new_n75 = ~opcode2 & ~new_n74;
  assign new_n76 = new_n33 & ~new_n75;
  assign new_n77 = ~new_n44 & ~new_n48;
  assign alu_op2 = new_n76 | ~new_n77;
  assign new_n79 = opcode2 & new_n33;
  assign new_n80 = opcode0 & new_n79;
  assign alu_op_ext0 = ~new_n42 | new_n80;
  assign new_n82 = ~new_n39 & ~new_n51;
  assign new_n83 = new_n33 & new_n38;
  assign alu_op_ext1 = ~new_n82 | new_n83;
  assign new_n85 = ~opcode1 & ~opcode2;
  assign new_n86 = new_n33 & new_n85;
  assign alu_op_ext2 = ~new_n82 | new_n86;
  assign alu_op_ext3 = ~new_n55 & ~alu_op_ext2;
  assign halt = new_n35 & new_n37;
  assign new_n90 = opcode2 & new_n47;
  assign new_n91 = ~new_n34 & ~new_n74;
  assign mem_write = new_n44 & ~new_n91;
  assign new_n93 = ~new_n37 & ~mem_write;
  assign new_n94 = ~sel_reg_dst1 & ~new_n93;
  assign reg_write = ~new_n90 & ~new_n94;
  assign jump = opcode2 & new_n37;
  assign sel_pc_opA = opcode0 & jump;
  assign sel_pc_opB = ~opcode0 & jump;
  assign beqz = new_n34 & new_n90;
  assign new_n100 = opcode0 & ~opcode1;
  assign bnez = new_n90 & new_n100;
  assign bgez = new_n74 & new_n90;
  assign bltz = new_n50 & new_n90;
  assign new_n104 = opcode0 & new_n48;
  assign new_n105 = ~new_n74 & new_n79;
  assign new_n106 = op_ext0 & new_n54;
  assign new_n107 = new_n74 & new_n106;
  assign new_n108 = ~new_n104 & ~new_n105;
  assign Cin = new_n107 | ~new_n108;
  assign invA = ~new_n69 & Cin;
  assign invB = new_n69 & Cin;
  assign sel_wb = new_n44 & new_n100;
endmodule


