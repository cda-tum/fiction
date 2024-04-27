// Benchmark "top" written by ABC on Mon Feb 19 11:52:43 2024

module top ( 
    pa, pb, pc, pd, pe,
    pp, pq, pr, ps, pt, pu, pf, pg, ph, pi, pj, pk, pl, pm, pn, po  );
  input  pa, pb, pc, pd, pe;
  output pp, pq, pr, ps, pt, pu, pf, pg, ph, pi, pj, pk, pl, pm, pn, po;
  wire new_n22, new_n23, new_n24, new_n27, new_n28, new_n31, new_n34,
    new_n35, new_n36, new_n39, new_n40, new_n43, new_n46, new_n49;
  assign new_n22 = ~pa & pe;
  assign new_n23 = ~pc & new_n22;
  assign new_n24 = pb & new_n23;
  assign pp = pd & new_n24;
  assign pq = ~pd & new_n24;
  assign new_n27 = pc & new_n22;
  assign new_n28 = ~pb & new_n27;
  assign pr = pd & new_n28;
  assign ps = ~pd & new_n28;
  assign new_n31 = ~pb & new_n23;
  assign pt = pd & new_n31;
  assign pu = ~pd & new_n31;
  assign new_n34 = pa & pe;
  assign new_n35 = pc & new_n34;
  assign new_n36 = pb & new_n35;
  assign pf = pd & new_n36;
  assign pg = ~pd & new_n36;
  assign new_n39 = ~pc & new_n34;
  assign new_n40 = pb & new_n39;
  assign ph = pd & new_n40;
  assign pi = ~pd & new_n40;
  assign new_n43 = ~pb & new_n35;
  assign pj = pd & new_n43;
  assign pk = ~pd & new_n43;
  assign new_n46 = ~pb & new_n39;
  assign pl = pd & new_n46;
  assign pm = ~pd & new_n46;
  assign new_n49 = pb & new_n27;
  assign pn = pd & new_n49;
  assign po = ~pd & new_n49;
endmodule


