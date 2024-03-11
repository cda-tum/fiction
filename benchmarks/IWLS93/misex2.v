// Benchmark "top" written by ABC on Mon Feb 19 11:52:44 2024

module top ( 
    a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x,
    y,
    q1, p1, a1, c1, b1, e1, d1, g1, f1, i1, h1, k1, z, j1, m1, l1, o1, n1  );
  input  a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u,
    v, w, x, y;
  output q1, p1, a1, c1, b1, e1, d1, g1, f1, i1, h1, k1, z, j1, m1, l1, o1,
    n1;
  wire new_n44, new_n47, new_n48, new_n49, new_n50, new_n51, new_n52,
    new_n54, new_n55, new_n56, new_n57, new_n58, new_n59, new_n60, new_n61,
    new_n62, new_n64, new_n65, new_n66, new_n68, new_n70, new_n71, new_n72,
    new_n74, new_n75, new_n76, new_n77, new_n78, new_n79, new_n82, new_n83,
    new_n84, new_n85, new_n86, new_n87, new_n88, new_n89, new_n90, new_n91,
    new_n92, new_n93, new_n94, new_n95, new_n96, new_n98, new_n99,
    new_n100, new_n101, new_n102, new_n103, new_n104, new_n105, new_n107,
    new_n108, new_n109, new_n110, new_n111, new_n112, new_n113, new_n114,
    new_n115, new_n117, new_n118, new_n120, new_n121, new_n122, new_n123,
    new_n124, new_n125, new_n126, new_n128, new_n130, new_n131, new_n132,
    new_n133, new_n134, new_n135, new_n136, new_n137, new_n138, new_n139,
    new_n140, new_n141, new_n142, new_n143, new_n144, new_n145, new_n146,
    new_n147, new_n148, new_n149, new_n150, new_n152, new_n153, new_n154,
    new_n155, new_n156, new_n157, new_n159, new_n160;
  assign new_n44 = ~b & c;
  assign q1 = ~a & new_n44;
  assign p1 = ~a & b;
  assign new_n47 = k & ~r;
  assign new_n48 = j & new_n47;
  assign new_n49 = ~s & ~t;
  assign new_n50 = ~b & ~c;
  assign new_n51 = ~a & new_n50;
  assign new_n52 = new_n48 & new_n49;
  assign a1 = new_n51 & new_n52;
  assign new_n54 = s & ~t;
  assign new_n55 = ~r & new_n54;
  assign new_n56 = new_n51 & new_n55;
  assign new_n57 = k & l;
  assign new_n58 = ~j & new_n57;
  assign new_n59 = b & c;
  assign new_n60 = a & new_n59;
  assign new_n61 = m & new_n58;
  assign new_n62 = new_n60 & new_n61;
  assign c1 = new_n56 | new_n62;
  assign new_n64 = ~k & ~r;
  assign new_n65 = j & new_n64;
  assign new_n66 = new_n49 & new_n65;
  assign b1 = new_n51 & new_n66;
  assign new_n68 = j & k;
  assign e1 = new_n60 & new_n68;
  assign new_n70 = k & ~l;
  assign new_n71 = ~j & new_n70;
  assign new_n72 = ~m & new_n71;
  assign d1 = new_n60 & new_n72;
  assign new_n74 = a & new_n44;
  assign new_n75 = l & ~m;
  assign new_n76 = k & new_n75;
  assign new_n77 = c & ~j;
  assign new_n78 = a & new_n77;
  assign new_n79 = new_n76 & new_n78;
  assign g1 = new_n74 | new_n79;
  assign f1 = new_n58 & new_n60;
  assign new_n82 = ~u & ~v;
  assign new_n83 = ~t & new_n82;
  assign new_n84 = b & s;
  assign new_n85 = ~a & new_n84;
  assign new_n86 = ~w & new_n83;
  assign new_n87 = new_n85 & new_n86;
  assign new_n88 = ~v & ~w;
  assign new_n89 = u & new_n88;
  assign new_n90 = m & ~n;
  assign new_n91 = ~l & new_n90;
  assign new_n92 = ~p & ~q;
  assign new_n93 = ~o & new_n92;
  assign new_n94 = new_n91 & new_n93;
  assign new_n95 = q1 & new_n94;
  assign new_n96 = new_n89 & new_n95;
  assign i1 = new_n87 | new_n96;
  assign new_n98 = ~s & t;
  assign new_n99 = r & new_n98;
  assign new_n100 = e & ~f;
  assign new_n101 = ~d & new_n100;
  assign new_n102 = ~h & ~i;
  assign new_n103 = ~g & new_n102;
  assign new_n104 = new_n101 & new_n103;
  assign new_n105 = q1 & new_n104;
  assign h1 = new_n99 & new_n105;
  assign new_n107 = ~u & v;
  assign new_n108 = ~t & new_n107;
  assign new_n109 = ~w & new_n108;
  assign new_n110 = new_n85 & new_n109;
  assign new_n111 = p & ~q;
  assign new_n112 = ~o & new_n111;
  assign new_n113 = new_n91 & new_n112;
  assign new_n114 = q1 & new_n113;
  assign new_n115 = new_n89 & new_n114;
  assign k1 = new_n110 | new_n115;
  assign new_n117 = ~j & new_n47;
  assign new_n118 = new_n49 & new_n117;
  assign z = new_n51 & new_n118;
  assign new_n120 = w & new_n108;
  assign new_n121 = new_n85 & new_n120;
  assign new_n122 = p & q;
  assign new_n123 = ~o & new_n122;
  assign new_n124 = new_n91 & new_n123;
  assign new_n125 = q1 & new_n124;
  assign new_n126 = new_n89 & new_n125;
  assign j1 = new_n121 | new_n126;
  assign new_n128 = r & ~t;
  assign m1 = new_n51 & new_n128;
  assign new_n130 = j & ~y;
  assign new_n131 = b & ~c;
  assign new_n132 = a & new_n131;
  assign new_n133 = new_n130 & new_n132;
  assign new_n134 = ~k & ~y;
  assign new_n135 = b & j;
  assign new_n136 = a & new_n135;
  assign new_n137 = new_n134 & new_n136;
  assign new_n138 = ~new_n133 & ~new_n137;
  assign new_n139 = x & ~y;
  assign new_n140 = t & new_n139;
  assign new_n141 = ~b & j;
  assign new_n142 = ~a & new_n141;
  assign new_n143 = new_n140 & new_n142;
  assign new_n144 = j & new_n139;
  assign new_n145 = q1 & new_n144;
  assign new_n146 = j & new_n128;
  assign new_n147 = ~y & new_n146;
  assign new_n148 = new_n51 & new_n147;
  assign new_n149 = ~new_n143 & ~new_n145;
  assign new_n150 = ~new_n148 & new_n149;
  assign l1 = ~new_n138 | ~new_n150;
  assign new_n152 = b & ~k;
  assign new_n153 = a & new_n152;
  assign new_n154 = a & ~c;
  assign new_n155 = ~c & t;
  assign new_n156 = ~b & new_n155;
  assign new_n157 = ~new_n153 & ~new_n154;
  assign o1 = new_n156 | ~new_n157;
  assign new_n159 = ~j & new_n64;
  assign new_n160 = new_n49 & new_n159;
  assign n1 = new_n51 & new_n160;
endmodule


