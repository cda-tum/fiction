module top ( 
    \opcode0 , \opcode1 , \opcode2 , \opcode3 , \opcode4 ,
    \op_ext0 , \op_ext1 ,
    \sel_reg_dst0 , \sel_reg_dst1 , \sel_alu_opB0 , \sel_alu_opB1 ,
    \alu_op0 , \alu_op1 , \alu_op2 , \alu_op_ext0 ,
    \alu_op_ext1 , \alu_op_ext2 , \alu_op_ext3 , halt, reg_write,
    sel_pc_opA, sel_pc_opB, beqz, bnez, bgez, bltz, jump, Cin, invA, invB,
    mem_write, sel_wb  );
  input  \opcode0 , \opcode1 , \opcode2 , \opcode3 , \opcode4 ,
    \op_ext0 , \op_ext1 ;
  output \sel_reg_dst0 , \sel_reg_dst1 , \sel_alu_opB0 ,
    \sel_alu_opB1 , \alu_op0 , \alu_op1 , \alu_op2 ,
    \alu_op_ext0 , \alu_op_ext1 , \alu_op_ext2 , \alu_op_ext3 ,
    halt, reg_write, sel_pc_opA, sel_pc_opB, beqz, bnez, bgez, bltz, jump,
    Cin, invA, invB, mem_write, sel_wb;
  wire n35, n36, n37, n38, n39, n40, n41, n42, n43, n44, n45, n46, n47, n48,
    n50, n51, n52, n53, n54, n55, n56, n57, n58, n59, n60, n61, n63, n64,
    n65, n66, n67, n68, n69, n71, n72, n73, n74, n75, n76, n77, n78, n80,
    n81, n82, n83, n84, n85, n86, n87, n88, n89, n90, n91, n92, n93, n94,
    n95, n96, n97, n99, n100, n101, n102, n103, n104, n106, n107, n108,
    n109, n110, n111, n112, n113, n114, n115, n117, n118, n119, n120, n121,
    n123, n124, n125, n126, n127, n128, n129, n131, n132, n134, n135, n136,
    n137, n139, n140, n141, n142, n143, n145, n146, n147, n148, n149, n150,
    n151, n153, n154, n156, n158, n159, n160, n161, n162, n164, n165, n166,
    n167, n169, n170, n171, n173, n174, n175, n177, n179, n180, n181, n182,
    n183, n184, n185, n186, n188, n189, n190, n191, n192, n193, n194, n195,
    n197, n198, n200, n201, n202, n203, n205, n206, n207;
  assign n35 = \opcode0  & ~\opcode1 ;
  assign n36 = \opcode3  & \opcode4 ;
  assign n37 = n35 & n36;
  assign n38 = \opcode1  & \opcode3 ;
  assign n39 = \opcode4  & n38;
  assign n40 = ~n37 & ~n39;
  assign n41 = ~\opcode2  & ~n40;
  assign n42 = ~\opcode1  & \opcode3 ;
  assign n43 = \opcode4  & n42;
  assign n44 = ~\opcode3  & ~\opcode4 ;
  assign n45 = ~n36 & ~n44;
  assign n46 = \opcode1  & ~n45;
  assign n47 = ~n43 & ~n46;
  assign n48 = \opcode2  & ~n47;
  assign \sel_reg_dst0  = n41 | n48;
  assign n50 = ~\opcode0  & ~n36;
  assign n51 = ~\opcode0  & ~n50;
  assign n52 = ~\opcode1  & ~n51;
  assign n53 = ~\opcode3  & ~n44;
  assign n54 = \opcode1  & ~n53;
  assign n55 = ~n52 & ~n54;
  assign n56 = ~\opcode2  & ~n55;
  assign n57 = ~\opcode3  & \opcode4 ;
  assign n58 = ~\opcode3  & ~n57;
  assign n59 = \opcode1  & ~n58;
  assign n60 = \opcode1  & ~n59;
  assign n61 = \opcode2  & ~n60;
  assign \sel_reg_dst1  = ~n56 & ~n61;
  assign n63 = ~\opcode0  & ~n45;
  assign n64 = \opcode3  & ~n36;
  assign n65 = \opcode0  & ~n64;
  assign n66 = ~n63 & ~n65;
  assign n67 = \opcode1  & ~n66;
  assign n68 = ~n52 & ~n67;
  assign n69 = ~\opcode2  & ~n68;
  assign \sel_alu_opB0  = ~\opcode2  & ~n69;
  assign n71 = ~\opcode0  & ~\opcode3 ;
  assign n72 = ~n57 & n71;
  assign n73 = \opcode0  & ~n45;
  assign n74 = ~n72 & ~n73;
  assign n75 = ~\opcode1  & ~n74;
  assign n76 = ~n54 & ~n75;
  assign n77 = ~\opcode2  & ~n76;
  assign n78 = \opcode2  & ~n53;
  assign \sel_alu_opB1  = ~n77 & ~n78;
  assign n80 = ~\opcode0  & \opcode3 ;
  assign n81 = \opcode4  & \op_ext0 ;
  assign n82 = n80 & n81;
  assign n83 = \opcode3  & ~\op_ext1 ;
  assign n84 = ~n36 & n83;
  assign n85 = \opcode3  & ~\op_ext0 ;
  assign n86 = ~n36 & n85;
  assign n87 = \opcode3  & \op_ext0 ;
  assign n88 = ~n86 & ~n87;
  assign n89 = \op_ext1  & ~n88;
  assign n90 = ~n84 & ~n89;
  assign n91 = \opcode0  & ~n90;
  assign n92 = ~n82 & ~n91;
  assign n93 = \opcode1  & ~n92;
  assign n94 = ~\opcode2  & ~n93;
  assign n95 = \opcode0  & ~n53;
  assign n96 = \opcode0  & ~n95;
  assign n97 = \opcode2  & ~n96;
  assign \alu_op0  = ~n94 & ~n97;
  assign n99 = \opcode3  & \op_ext1 ;
  assign n100 = ~n84 & ~n99;
  assign n101 = \opcode1  & ~n100;
  assign n102 = ~\opcode2  & ~n101;
  assign n103 = \opcode1  & ~n54;
  assign n104 = \opcode2  & ~n103;
  assign \alu_op1  = ~n102 & ~n104;
  assign n106 = ~\opcode1  & ~n36;
  assign n107 = ~n44 & n106;
  assign n108 = ~n44 & n50;
  assign n109 = \opcode0  & ~n58;
  assign n110 = ~n108 & ~n109;
  assign n111 = \opcode1  & ~n110;
  assign n112 = ~n107 & ~n111;
  assign n113 = ~\opcode2  & ~n112;
  assign n114 = \opcode2  & \opcode3 ;
  assign n115 = \opcode4  & n114;
  assign \alu_op2  = n113 | n115;
  assign n117 = ~\opcode1  & ~\opcode2 ;
  assign n118 = ~n52 & n117;
  assign n119 = \opcode1  & ~n74;
  assign n120 = ~n37 & ~n119;
  assign n121 = \opcode2  & ~n120;
  assign \alu_op_ext0  = n118 | n121;
  assign n123 = ~\opcode0  & ~n53;
  assign n124 = ~\opcode0  & ~n123;
  assign n125 = \opcode1  & ~n124;
  assign n126 = \opcode1  & ~\opcode2 ;
  assign n127 = ~n125 & n126;
  assign n128 = \opcode1  & \opcode2 ;
  assign n129 = ~n45 & n128;
  assign \alu_op_ext1  = n127 | n129;
  assign n131 = ~n106 & ~n125;
  assign n132 = ~\opcode2  & ~n131;
  assign \alu_op_ext2  = ~n61 & ~n132;
  assign n134 = ~n80 & ~n109;
  assign n135 = \opcode1  & ~n134;
  assign n136 = ~\opcode2  & ~n107;
  assign n137 = ~n135 & n136;
  assign \alu_op_ext3  = ~n78 & ~n137;
  assign n139 = ~\opcode0  & ~n58;
  assign n140 = ~\opcode0  & ~n139;
  assign n141 = ~\opcode1  & ~n140;
  assign n142 = ~\opcode1  & ~n141;
  assign n143 = ~\opcode2  & ~n142;
  assign halt = ~\opcode2  & ~n143;
  assign n145 = ~\opcode1  & ~n134;
  assign n146 = ~n59 & ~n145;
  assign n147 = ~\opcode2  & ~n146;
  assign n148 = ~\opcode1  & \opcode4 ;
  assign n149 = \opcode1  & ~n64;
  assign n150 = ~n148 & ~n149;
  assign n151 = \opcode2  & ~n150;
  assign reg_write = n147 | n151;
  assign n153 = \opcode0  & ~n109;
  assign n154 = \opcode2  & ~n153;
  assign sel_pc_opA = \opcode2  & ~n154;
  assign n156 = \opcode2  & ~n140;
  assign sel_pc_opB = \opcode2  & ~n156;
  assign n158 = ~\opcode0  & ~n64;
  assign n159 = ~\opcode0  & ~n158;
  assign n160 = ~\opcode1  & ~n159;
  assign n161 = ~\opcode1  & ~n160;
  assign n162 = \opcode2  & ~n161;
  assign beqz = \opcode2  & ~n162;
  assign n164 = \opcode0  & ~n65;
  assign n165 = ~\opcode1  & ~n164;
  assign n166 = ~\opcode1  & ~n165;
  assign n167 = \opcode2  & ~n166;
  assign bnez = \opcode2  & ~n167;
  assign n169 = \opcode1  & ~n164;
  assign n170 = \opcode1  & ~n169;
  assign n171 = \opcode2  & ~n170;
  assign bgez = \opcode2  & ~n171;
  assign n173 = \opcode1  & ~n159;
  assign n174 = \opcode1  & ~n173;
  assign n175 = \opcode2  & ~n174;
  assign bltz = \opcode2  & ~n175;
  assign n177 = \opcode2  & ~n58;
  assign jump = \opcode2  & ~n177;
  assign n179 = \opcode0  & \opcode1 ;
  assign n180 = ~n88 & n179;
  assign n181 = n35 & ~n65;
  assign n182 = ~\opcode2  & ~n181;
  assign n183 = ~n180 & n182;
  assign n184 = \opcode1  & ~n51;
  assign n185 = ~n106 & ~n184;
  assign n186 = \opcode2  & ~n185;
  assign Cin = ~n183 & ~n186;
  assign n188 = \op_ext0  & n36;
  assign n189 = ~\op_ext1  & ~n188;
  assign n190 = ~\op_ext1  & ~n189;
  assign n191 = \opcode0  & ~n190;
  assign n192 = \opcode0  & ~n191;
  assign n193 = \opcode1  & ~n192;
  assign n194 = ~n165 & ~n193;
  assign n195 = ~\opcode2  & ~n194;
  assign invA = ~\opcode2  & ~n195;
  assign n197 = ~n90 & n179;
  assign n198 = ~\opcode2  & ~n197;
  assign invB = ~n186 & ~n198;
  assign n200 = ~\opcode1  & ~n124;
  assign n201 = \opcode1  & ~n96;
  assign n202 = ~n200 & ~n201;
  assign n203 = ~\opcode2  & ~n202;
  assign mem_write = ~\opcode2  & ~n203;
  assign n205 = ~\opcode1  & ~n96;
  assign n206 = ~\opcode1  & ~n205;
  assign n207 = ~\opcode2  & ~n206;
  assign sel_wb = ~\opcode2  & ~n207;
endmodule


