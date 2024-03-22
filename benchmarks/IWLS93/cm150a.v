// Benchmark "top" written by ABC on Mon Feb 19 11:52:42 2024

module top ( 
    pp, pq, pr, ps, pt, pu, pa, pb, pc, pd, pe, pf, pg, ph, pi, pj, pk, pl,
    pm, pn, po,
    pv  );
  input  pp, pq, pr, ps, pt, pu, pa, pb, pc, pd, pe, pf, pg, ph, pi, pj,
    pk, pl, pm, pn, po;
  output pv;
  wire new_n23, new_n24, new_n25, new_n26, new_n27, new_n28, new_n29,
    new_n30, new_n31, new_n32, new_n33, new_n34, new_n35, new_n36, new_n37,
    new_n38, new_n39, new_n40, new_n41, new_n42, new_n43, new_n44, new_n45,
    new_n46, new_n47, new_n48, new_n49, new_n50, new_n51, new_n52, new_n53,
    new_n54, new_n55, new_n56, new_n57, new_n58, new_n59, new_n60, new_n61,
    new_n62, new_n63, new_n64, new_n65, new_n66, new_n67, new_n68, new_n69,
    new_n70, new_n71, new_n72, new_n73, new_n74, new_n75, new_n76, new_n77,
    new_n78, new_n79, new_n80, new_n81, new_n82, new_n83, new_n84, new_n85,
    new_n86, new_n87, new_n88, new_n89, new_n90, new_n91, new_n92, new_n93,
    new_n94, new_n95, new_n96, new_n97;
  assign new_n23 = ~pp & pq;
  assign new_n24 = ~pq & ~po;
  assign new_n25 = ~pp & ~po;
  assign new_n26 = ~new_n23 & ~new_n24;
  assign new_n27 = ~new_n25 & new_n26;
  assign new_n28 = pr & new_n27;
  assign new_n29 = pq & ~pn;
  assign new_n30 = ~pq & ~pm;
  assign new_n31 = ~pm & ~pn;
  assign new_n32 = ~new_n29 & ~new_n30;
  assign new_n33 = ~new_n31 & new_n32;
  assign new_n34 = new_n27 & new_n33;
  assign new_n35 = ~pr & new_n33;
  assign new_n36 = ~new_n28 & ~new_n34;
  assign new_n37 = ~new_n35 & new_n36;
  assign new_n38 = ps & new_n37;
  assign new_n39 = pq & ~pl;
  assign new_n40 = ~pq & ~pk;
  assign new_n41 = ~pk & ~pl;
  assign new_n42 = ~new_n39 & ~new_n40;
  assign new_n43 = ~new_n41 & new_n42;
  assign new_n44 = pr & new_n43;
  assign new_n45 = pq & ~pj;
  assign new_n46 = ~pq & ~pi;
  assign new_n47 = ~pi & ~pj;
  assign new_n48 = ~new_n45 & ~new_n46;
  assign new_n49 = ~new_n47 & new_n48;
  assign new_n50 = new_n43 & new_n49;
  assign new_n51 = ~pr & new_n49;
  assign new_n52 = ~new_n44 & ~new_n50;
  assign new_n53 = ~new_n51 & new_n52;
  assign new_n54 = new_n37 & new_n53;
  assign new_n55 = ~ps & new_n53;
  assign new_n56 = ~new_n38 & ~new_n54;
  assign new_n57 = ~new_n55 & new_n56;
  assign new_n58 = pt & new_n57;
  assign new_n59 = pq & ~ph;
  assign new_n60 = ~pq & ~pg;
  assign new_n61 = ~pg & ~ph;
  assign new_n62 = ~new_n59 & ~new_n60;
  assign new_n63 = ~new_n61 & new_n62;
  assign new_n64 = pr & new_n63;
  assign new_n65 = pq & ~pf;
  assign new_n66 = ~pq & ~pe;
  assign new_n67 = ~pe & ~pf;
  assign new_n68 = ~new_n65 & ~new_n66;
  assign new_n69 = ~new_n67 & new_n68;
  assign new_n70 = new_n63 & new_n69;
  assign new_n71 = ~pr & new_n69;
  assign new_n72 = ~new_n64 & ~new_n70;
  assign new_n73 = ~new_n71 & new_n72;
  assign new_n74 = ps & new_n73;
  assign new_n75 = pq & ~pd;
  assign new_n76 = ~pq & ~pc;
  assign new_n77 = ~pc & ~pd;
  assign new_n78 = ~new_n75 & ~new_n76;
  assign new_n79 = ~new_n77 & new_n78;
  assign new_n80 = pr & new_n79;
  assign new_n81 = pq & ~pb;
  assign new_n82 = ~pq & ~pa;
  assign new_n83 = ~pa & ~pb;
  assign new_n84 = ~new_n81 & ~new_n82;
  assign new_n85 = ~new_n83 & new_n84;
  assign new_n86 = new_n79 & new_n85;
  assign new_n87 = ~pr & new_n85;
  assign new_n88 = ~new_n80 & ~new_n86;
  assign new_n89 = ~new_n87 & new_n88;
  assign new_n90 = new_n73 & new_n89;
  assign new_n91 = ~ps & new_n89;
  assign new_n92 = ~new_n74 & ~new_n90;
  assign new_n93 = ~new_n91 & new_n92;
  assign new_n94 = new_n57 & new_n93;
  assign new_n95 = ~pt & new_n93;
  assign new_n96 = ~new_n58 & ~new_n94;
  assign new_n97 = ~new_n95 & new_n96;
  assign pv = pu | new_n97;
endmodule


