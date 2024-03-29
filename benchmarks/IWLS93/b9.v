// Benchmark "top" written by ABC on Mon Feb 19 11:52:42 2024

module top ( 
    a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, a0, q, b0, r, c0, s,
    d0, t, e0, u, f0, v, g0, w, h0, x, i0, y, j0, z, k0, l0, m0, n0, o0,
    p0, q0, r0, s0, t0, u0, v0, w0, x0, y0, z0, a1, c1, b1, e1, d1, g1, f1,
    i1, h1, j1  );
  input  a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, a0, q, b0, r,
    c0, s, d0, t, e0, u, f0, v, g0, w, h0, x, i0, y, j0, z, k0, l0, m0, n0,
    o0;
  output p0, q0, r0, s0, t0, u0, v0, w0, x0, y0, z0, a1, c1, b1, e1, d1, g1,
    f1, i1, h1, j1;
  wire new_n63, new_n64, new_n65, new_n66, new_n67, new_n68, new_n69,
    new_n70, new_n71, new_n72, new_n73, new_n74, new_n75, new_n76, new_n78,
    new_n80, new_n81, new_n82, new_n83, new_n84, new_n85, new_n86, new_n87,
    new_n89, new_n90, new_n91, new_n92, new_n93, new_n94, new_n95, new_n96,
    new_n99, new_n100, new_n101, new_n102, new_n105, new_n107, new_n108,
    new_n109, new_n110, new_n111, new_n112, new_n113, new_n114, new_n115,
    new_n116, new_n117, new_n118, new_n120, new_n121, new_n123, new_n124,
    new_n125, new_n126, new_n127, new_n128, new_n129, new_n130, new_n131,
    new_n132, new_n133, new_n134, new_n135, new_n136, new_n137, new_n139,
    new_n140, new_n141, new_n142, new_n143, new_n144, new_n145, new_n146,
    new_n147, new_n148, new_n149, new_n150, new_n152, new_n153, new_n154,
    new_n155, new_n156, new_n157, new_n158, new_n159, new_n160, new_n161,
    new_n162, new_n163, new_n164, new_n165, new_n166, new_n167, new_n168,
    new_n171, new_n172, new_n174, new_n176, new_n177, new_n178, new_n179,
    new_n180, new_n181, new_n182;
  assign new_n63 = c0 & j0;
  assign new_n64 = ~l0 & ~new_n63;
  assign new_n65 = b0 & ~new_n64;
  assign new_n66 = ~n0 & ~new_n65;
  assign new_n67 = c0 & ~k0;
  assign new_n68 = ~j0 & ~k0;
  assign new_n69 = ~new_n67 & ~new_n68;
  assign new_n70 = ~e & new_n69;
  assign new_n71 = ~e & k;
  assign new_n72 = ~new_n70 & ~new_n71;
  assign new_n73 = ~b0 & ~new_n71;
  assign new_n74 = ~new_n72 & ~new_n73;
  assign new_n75 = o0 & new_n74;
  assign new_n76 = ~new_n65 & ~new_n75;
  assign b1 = new_n66 | new_n76;
  assign new_n78 = ~p & ~b1;
  assign p0 = ~q | new_n78;
  assign new_n80 = e & j0;
  assign new_n81 = b0 & ~c0;
  assign new_n82 = new_n80 & new_n81;
  assign new_n83 = ~e0 & ~g0;
  assign new_n84 = e & ~new_n83;
  assign new_n85 = b0 & k0;
  assign new_n86 = e & new_n85;
  assign new_n87 = ~new_n82 & ~new_n84;
  assign q0 = ~new_n86 & new_n87;
  assign new_n89 = ~c0 & j0;
  assign new_n90 = b0 & new_n89;
  assign new_n91 = i & new_n89;
  assign new_n92 = ~new_n90 & ~new_n91;
  assign new_n93 = e & new_n92;
  assign new_n94 = i & d0;
  assign new_n95 = ~c & ~new_n94;
  assign new_n96 = ~new_n93 & ~new_n95;
  assign x0 = n0 & o0;
  assign r0 = ~new_n96 | ~x0;
  assign new_n99 = ~b0 & ~l0;
  assign new_n100 = b0 & ~new_n63;
  assign new_n101 = ~new_n64 & ~new_n99;
  assign new_n102 = ~new_n100 & new_n101;
  assign s0 = ~v | ~new_n102;
  assign t0 = v | ~new_n102;
  assign new_n105 = b0 & c0;
  assign v0 = ~l0 & ~new_n105;
  assign new_n107 = ~p & r;
  assign new_n108 = ~f0 & ~h0;
  assign new_n109 = ~new_n107 & new_n108;
  assign new_n110 = o & ~new_n109;
  assign new_n111 = d & ~new_n110;
  assign new_n112 = ~m0 & new_n111;
  assign new_n113 = ~a & o;
  assign new_n114 = o & z;
  assign new_n115 = ~a & ~z;
  assign new_n116 = ~new_n113 & ~new_n114;
  assign new_n117 = ~new_n115 & new_n116;
  assign new_n118 = new_n111 & new_n117;
  assign w0 = new_n112 | new_n118;
  assign new_n120 = a0 & i0;
  assign new_n121 = b0 & new_n120;
  assign y0 = l & new_n121;
  assign new_n123 = o0 & ~new_n83;
  assign new_n124 = o0 & new_n69;
  assign new_n125 = b0 & new_n124;
  assign new_n126 = ~new_n123 & ~new_n125;
  assign new_n127 = n0 & ~new_n126;
  assign new_n128 = ~e & f;
  assign new_n129 = new_n127 & new_n128;
  assign new_n130 = h & b0;
  assign new_n131 = new_n70 & new_n130;
  assign new_n132 = ~n0 & new_n131;
  assign new_n133 = ~o0 & new_n131;
  assign new_n134 = ~new_n132 & ~new_n133;
  assign new_n135 = g & l0;
  assign new_n136 = b0 & new_n135;
  assign new_n137 = ~new_n129 & new_n134;
  assign z0 = new_n136 | ~new_n137;
  assign new_n139 = o0 & new_n94;
  assign new_n140 = b0 & j0;
  assign new_n141 = ~e & ~c0;
  assign new_n142 = new_n140 & new_n141;
  assign new_n143 = i & ~c0;
  assign new_n144 = j0 & new_n143;
  assign new_n145 = ~b0 & new_n144;
  assign new_n146 = ~j & ~e0;
  assign new_n147 = ~new_n142 & ~new_n145;
  assign new_n148 = new_n146 & new_n147;
  assign new_n149 = ~new_n139 & new_n148;
  assign new_n150 = ~n0 & new_n148;
  assign h1 = ~new_n149 & ~new_n150;
  assign new_n152 = ~e & o0;
  assign new_n153 = j0 & k0;
  assign new_n154 = c0 & ~new_n153;
  assign new_n155 = new_n83 & new_n154;
  assign new_n156 = ~b0 & new_n83;
  assign new_n157 = ~new_n155 & ~new_n156;
  assign new_n158 = new_n152 & new_n157;
  assign new_n159 = ~n & new_n158;
  assign new_n160 = ~j0 & ~l0;
  assign new_n161 = ~c0 & ~l0;
  assign new_n162 = ~new_n160 & ~new_n161;
  assign new_n163 = ~s & u;
  assign new_n164 = ~t & new_n163;
  assign new_n165 = new_n162 & new_n164;
  assign new_n166 = b0 & new_n165;
  assign new_n167 = ~new_n159 & ~new_n166;
  assign new_n168 = ~n0 & ~new_n166;
  assign c1 = ~new_n167 & ~new_n168;
  assign e1 = m & new_n121;
  assign new_n171 = w & ~y;
  assign new_n172 = b & x;
  assign g1 = new_n171 & new_n172;
  assign new_n174 = b & w;
  assign f1 = ~x & new_n174;
  assign new_n176 = i & j0;
  assign new_n177 = ~c0 & new_n176;
  assign new_n178 = ~e0 & ~new_n140;
  assign new_n179 = ~j & ~new_n177;
  assign new_n180 = new_n178 & new_n179;
  assign new_n181 = ~new_n139 & new_n180;
  assign new_n182 = ~n0 & new_n180;
  assign j1 = new_n181 | new_n182;
  assign u0 = ~v0;
  assign a1 = ~h1;
  assign d1 = ~c1;
  assign i1 = a1;
endmodule


