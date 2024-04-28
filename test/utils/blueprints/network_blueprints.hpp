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

}  // namespace blueprints

#endif  // FICTION_NETWORK_BLUEPRINTS_HPP
