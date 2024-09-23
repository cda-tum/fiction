//
// Created by marcel on 24.05.21.
//

#ifndef FICTION_NETWORK_BLUEPRINTS_HPP
#define FICTION_NETWORK_BLUEPRINTS_HPP

#include <mockturtle/views/names_view.hpp>

namespace blueprints
{

template <typename Ntk>
mockturtle::names_view<Ntk> maj1_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto a = ntk.create_pi("a");
    const auto b = ntk.create_pi("b");
    const auto c = ntk.create_pi("c");

    const auto m = ntk.create_maj(a, b, c);

    ntk.create_po(m, "f");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> maj4_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto a = ntk.create_pi("a");
    const auto b = ntk.create_pi("b");
    const auto c = ntk.create_pi("c");
    const auto d = ntk.create_pi("d");
    const auto e = ntk.create_pi("e");

    const auto m1 = ntk.create_maj(a, b, c);
    const auto m2 = ntk.create_maj(b, c, d);
    const auto m3 = ntk.create_maj(c, d, e);
    const auto m4 = ntk.create_maj(m1, m2, m3);

    ntk.create_po(m1, "m1");
    ntk.create_po(m2, "m2");
    ntk.create_po(m3, "m3");
    ntk.create_po(m4, "m4");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> unbalanced_and_inv_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto a = ntk.create_pi("a");
    const auto b = ntk.create_pi("b");

    const auto n = ntk.create_not(b);
    const auto f = ntk.create_and(a, n);

    ntk.create_po(f, "f");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> and_or_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto a = ntk.create_pi("a");
    const auto b = ntk.create_pi("b");

    const auto f1 = ntk.create_and(a, b);
    const auto f2 = ntk.create_or(a, b);

    ntk.create_po(f1, "f1");
    ntk.create_po(f2, "f2");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> multi_output_and_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto a = ntk.create_pi("a");
    const auto b = ntk.create_pi("b");

    const auto f = ntk.create_and(a, b);

    ntk.create_po(f, "f1");
    ntk.create_po(f, "f2");
    ntk.create_po(f, "f3");
    ntk.create_po(f, "f4");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> nary_operation_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi();
    const auto x2 = ntk.create_pi();
    const auto x3 = ntk.create_pi();
    const auto x4 = ntk.create_pi();
    const auto x5 = ntk.create_pi();

    const auto f1 = ntk.create_nary_and({x1, x2, x3});
    const auto f2 = ntk.create_nary_or({x2, x3, x4});
    const auto f3 = ntk.create_nary_xor({x3, x4, x5});

    ntk.create_po(f1);
    ntk.create_po(f2);
    ntk.create_po(f3);

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> constant_gate_input_maj_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi();
    const auto x2 = ntk.create_pi();

    const auto a1 = ntk.create_and(x1, x2);
    const auto m1 = ntk.create_maj(x1, x2, ntk.get_constant(true));
    const auto a2 = ntk.create_and(ntk.create_not(a1), m1);

    ntk.create_po(a2);

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> half_adder_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("a");
    const auto x2 = ntk.create_pi("b");

    const auto sum = ntk.create_and(x1, x2);
    const auto c   = ntk.create_xor(x1, x2);

    ntk.create_po(sum, "sum");
    ntk.create_po(c, "carry");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> full_adder_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("a");
    const auto x2 = ntk.create_pi("b");
    const auto x3 = ntk.create_pi("cin");

    const auto w1 = ntk.create_and(x1, x2);
    const auto w2 = ntk.create_xor(x1, x2);
    const auto w3 = ntk.create_and(x3, w2);

    const auto c   = ntk.create_or(w1, w3);
    const auto sum = ntk.create_xor(x3, w2);

    ntk.create_po(sum, "sum");
    ntk.create_po(c, "carry");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> mux21_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi();
    const auto x2 = ntk.create_pi();
    const auto x3 = ntk.create_pi();

    const auto n1  = ntk.create_not(x3);
    const auto a1  = ntk.create_and(x1, n1);
    const auto a2  = ntk.create_and(x2, x3);
    const auto mux = ntk.create_or(a1, a2);

    ntk.create_po(mux);

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> se_coloring_corner_case_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("x1");
    const auto x2 = ntk.create_pi("x2");

    const auto x3 = ntk.create_buf(x1);
    const auto x4 = ntk.create_buf(x2);

    const auto x5 = ntk.create_and(x3, x4);

    const auto x6 = ntk.create_buf(x5);

    const auto x7 = ntk.create_and(x3, x6);
    const auto x8 = ntk.create_and(x4, x6);

    ntk.create_po(x7, "f1");
    ntk.create_po(x8, "f2");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> fanout_substitution_corner_case_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("x1");

    const auto fo1 = ntk.create_buf(x1);
    const auto fo2 = ntk.create_buf(fo1);
    const auto fo3 = ntk.create_buf(fo1);

    const auto n1 = ntk.create_not(fo2);
    const auto n2 = ntk.create_not(fo2);
    const auto n3 = ntk.create_not(fo3);
    const auto n4 = ntk.create_not(fo3);

    ntk.create_po(n1, "f1");
    ntk.create_po(n2, "f2");
    ntk.create_po(n3, "f3");
    ntk.create_po(n4, "f4");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> inverter_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("x1");

    const auto fo1 = ntk.create_buf(x1);

    const auto n1 = ntk.create_not(fo1);
    const auto n2 = ntk.create_not(fo1);

    ntk.create_po(n1, "f1");
    ntk.create_po(n2, "f2");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> clpl()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("x1");
    const auto x2 = ntk.create_pi("x2");
    const auto x3 = ntk.create_pi("x3");
    const auto x4 = ntk.create_pi("x4");
    const auto x5 = ntk.create_pi("x5");
    //    const auto x6 = ntk.create_pi("x6");
    //    const auto x7 = ntk.create_pi("x7");
    //    const auto x8 = ntk.create_pi("x8");
    //    const auto x9 = ntk.create_pi("x9");
    //    const auto x10 = ntk.create_pi("x10");
    //    const auto x11 = ntk.create_pi("x11");

    const auto a1 = ntk.create_and(x1, x2);
    const auto o1 = ntk.create_or(a1, x3);
    const auto a2 = ntk.create_and(o1, x4);
    const auto o2 = ntk.create_or(a2, x5);
    //    const auto a3 = ntk.create_and(o2, x6);
    //    const auto o3 = ntk.create_or(a3, x7);
    //    const auto a4 = ntk.create_and(o3, x8);
    //    const auto o4 = ntk.create_or(a4, x9);
    //    const auto a5 = ntk.create_and(o4, x10);
    //    const auto o5 = ntk.create_or(a5, x11);

    ntk.create_po(o1, "f1");
    ntk.create_po(o2, "f2");
    //    ntk.create_po(o3, "f3");
    //    ntk.create_po(o4, "f4");
    //    ntk.create_po(o5, "f5");

    return ntk;
}

template <typename Ntk>
Ntk one_to_five_path_difference_network()
{
    Ntk ntk{};

    const auto x1 = ntk.create_pi();
    const auto x2 = ntk.create_pi();

    const auto p1 = ntk.create_buf(x1);
    const auto p2 = ntk.create_buf(ntk.create_buf(ntk.create_buf(ntk.create_buf(ntk.create_buf(x2)))));

    const auto a = ntk.create_and(p1, p2);

    ntk.create_po(a);

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> nand_xnor_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("x1");
    const auto x2 = ntk.create_pi("x2");

    const auto nand1 = ntk.create_nand(x1, x2);
    const auto nor1  = ntk.create_nor(x1, x2);
    const auto xor1  = ntk.create_xor(nand1, nor1);
    const auto xnor1 = ntk.create_xnor(nand1, nor1);

    ntk.create_po(xor1, "f1");
    ntk.create_po(xnor1, "f2");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> topolinano_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("x1");
    const auto x2 = ntk.create_pi("x2");

    const auto fo1 = ntk.create_buf(x1);
    const auto fo2 = ntk.create_buf(fo1);
    const auto a1  = ntk.create_and(x1, x2);
    const auto o1  = ntk.create_or(a1, fo2);

    ntk.create_po(fo1, "f1");
    ntk.create_po(fo2, "f2");
    ntk.create_po(o1, "f3");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> multi_output_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto x1 = ntk.create_pi("x1");
    const auto x2 = ntk.create_pi("x2");

    const auto a1 = ntk.create_and(x1, x2);

    ntk.create_po(a1, "f1");
    ntk.create_po(a1, "f2");

    return ntk;
}

template <typename Ntk>
mockturtle::names_view<Ntk> parity_network()
{
    mockturtle::names_view<Ntk> ntk{};

    const auto pi00 = ntk.create_pi("pi00");
    const auto pi01 = ntk.create_pi("pi01");
    const auto pi02 = ntk.create_pi("pi02");
    const auto pi03 = ntk.create_pi("pi03");
    const auto pi04 = ntk.create_pi("pi04");
    const auto pi05 = ntk.create_pi("pi05");
    const auto pi06 = ntk.create_pi("pi06");
    const auto pi07 = ntk.create_pi("pi07");
    const auto pi08 = ntk.create_pi("pi08");
    const auto pi09 = ntk.create_pi("pi09");
    const auto pi10 = ntk.create_pi("pi10");
    const auto pi11 = ntk.create_pi("pi11");
    const auto pi12 = ntk.create_pi("pi12");
    const auto pi13 = ntk.create_pi("pi13");
    const auto pi14 = ntk.create_pi("pi14");
    const auto pi15 = ntk.create_pi("pi15");

    const auto new_n18_ = ntk.create_and(pi14, ntk.create_not(pi15));
    const auto new_n19_ = ntk.create_and(ntk.create_not(pi14), pi15);
    const auto new_n20_ = ntk.create_and(ntk.create_not(new_n18_), ntk.create_not(new_n19_));
    const auto new_n21_ = ntk.create_and(pi12, ntk.create_not(pi13));
    const auto new_n22_ = ntk.create_and(ntk.create_not(pi12), pi13);
    const auto new_n23_ = ntk.create_and(ntk.create_not(new_n21_), ntk.create_not(new_n22_));
    const auto new_n24_ = ntk.create_and(new_n20_, ntk.create_not(new_n23_));
    const auto new_n25_ = ntk.create_and(ntk.create_not(new_n20_), new_n23_);
    const auto new_n26_ = ntk.create_and(ntk.create_not(new_n24_), ntk.create_not(new_n25_));
    const auto new_n27_ = ntk.create_and(pi10, ntk.create_not(pi11));
    const auto new_n28_ = ntk.create_and(ntk.create_not(pi10), pi11);
    const auto new_n29_ = ntk.create_and(ntk.create_not(new_n27_), ntk.create_not(new_n28_));
    const auto new_n30_ = ntk.create_and(pi08, ntk.create_not(pi09));
    const auto new_n31_ = ntk.create_and(ntk.create_not(pi08), pi09);
    const auto new_n32_ = ntk.create_and(ntk.create_not(new_n30_), ntk.create_not(new_n31_));
    const auto new_n33_ = ntk.create_and(new_n29_, ntk.create_not(new_n32_));
    const auto new_n34_ = ntk.create_and(ntk.create_not(new_n29_), new_n32_);
    const auto new_n35_ = ntk.create_and(ntk.create_not(new_n33_), ntk.create_not(new_n34_));
    const auto new_n36_ = ntk.create_and(new_n26_, ntk.create_not(new_n35_));
    const auto new_n37_ = ntk.create_and(ntk.create_not(new_n26_), new_n35_);
    const auto new_n38_ = ntk.create_and(ntk.create_not(new_n36_), ntk.create_not(new_n37_));
    const auto new_n39_ = ntk.create_and(pi06, ntk.create_not(pi07));
    const auto new_n40_ = ntk.create_and(ntk.create_not(pi06), pi07);
    const auto new_n41_ = ntk.create_and(ntk.create_not(new_n39_), ntk.create_not(new_n40_));
    const auto new_n42_ = ntk.create_and(pi04, ntk.create_not(pi05));
    const auto new_n43_ = ntk.create_and(ntk.create_not(pi04), pi05);
    const auto new_n44_ = ntk.create_and(ntk.create_not(new_n42_), ntk.create_not(new_n43_));
    const auto new_n45_ = ntk.create_and(new_n41_, ntk.create_not(new_n44_));
    const auto new_n46_ = ntk.create_and(ntk.create_not(new_n41_), new_n44_);
    const auto new_n47_ = ntk.create_and(ntk.create_not(new_n45_), ntk.create_not(new_n46_));
    const auto new_n48_ = ntk.create_and(pi02, ntk.create_not(pi03));
    const auto new_n49_ = ntk.create_and(ntk.create_not(pi02), pi03);
    const auto new_n50_ = ntk.create_and(ntk.create_not(new_n48_), ntk.create_not(new_n49_));
    const auto new_n51_ = ntk.create_and(pi00, ntk.create_not(pi01));
    const auto new_n52_ = ntk.create_and(ntk.create_not(pi00), pi01);
    const auto new_n53_ = ntk.create_and(ntk.create_not(new_n51_), ntk.create_not(new_n52_));
    const auto new_n54_ = ntk.create_and(new_n50_, ntk.create_not(new_n53_));
    const auto new_n55_ = ntk.create_and(ntk.create_not(new_n50_), new_n53_);
    const auto new_n56_ = ntk.create_and(ntk.create_not(new_n54_), ntk.create_not(new_n55_));
    const auto new_n57_ = ntk.create_and(new_n47_, ntk.create_not(new_n56_));
    const auto new_n58_ = ntk.create_and(ntk.create_not(new_n47_), new_n56_);
    const auto new_n59_ = ntk.create_and(ntk.create_not(new_n57_), ntk.create_not(new_n58_));
    const auto new_n60_ = ntk.create_and(new_n38_, ntk.create_not(new_n59_));
    const auto new_n61_ = ntk.create_and(ntk.create_not(new_n38_), new_n59_);
    ntk.create_po(ntk.create_or(new_n60_, new_n61_), "po0");

    return ntk;
}
}  // namespace blueprints

#endif  // FICTION_NETWORK_BLUEPRINTS_HPP
