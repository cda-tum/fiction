//
// Created by benjamin on 14.06.24.
//

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include <fiction/algorithms/verification/virtual_miter.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/virtual_pi_network.hpp>

#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/xag.hpp>

using namespace fiction;

TEST_CASE("Copy technology network and size consistency", "[virtual-pi-view]")
{
    technology_network tec{};

    const auto a = tec.create_pi();
    const auto b = tec.create_pi();

    const auto f1 = tec.create_and(a, b);

    virtual_pi_network vpi{tec};

    const auto c = vpi.create_virtual_pi(a);
    const auto d = vpi.create_virtual_pi(b);

    const auto f2 = vpi.create_and(b, c);
    const auto f3 = vpi.create_or(a, d);

    vpi.create_po(f1);
    vpi.create_po(f2);
    vpi.create_po(f3);

    // virtual_pi_network is a deep copy. THe original network won't be affected by any changes after the copy
    CHECK(tec.size() == 5);
    // This is the size disregarding virtual PIs. So this would correspond to the size in a normal technology_network
    CHECK(vpi.real_size() == 7);
    // This is the size with the virtual PIs
    CHECK(vpi.size() == 9);
    vpi.remove_virtual_input_nodes();
    // After removing/remapping the virtual PIs to their real PIs the sizes are equal
    CHECK(vpi.real_size() == vpi.size());
    CHECK(vpi.size() == 7);
}

TEMPLATE_TEST_CASE("Copy networks and size consistency", "[virtual-pi-view]", mockturtle::aig_network,
                   mockturtle::xag_network)
{
    TestType ntk{};

    const auto a = ntk.create_pi();
    const auto b = ntk.create_pi();

    const auto f1 = ntk.create_and(a, b);

    virtual_pi_network vpi{ntk};

    const auto c = vpi.create_virtual_pi(a);
    const auto d = vpi.create_virtual_pi(b);

    const auto f2 = vpi.create_and(b, c);
    const auto f3 = vpi.create_or(a, d);

    vpi.create_po(f1);
    vpi.create_po(f2);
    vpi.create_po(f3);

    // virtual_pi_network is a deep copy. THe original network won't be affected by any changes after the copy
    CHECK(ntk.size() == 4);
    // This is the size disregarding virtual PIs. So this would correspond to the size in a normal technology_network
    CHECK(vpi.real_size() == 6);
    // This is the size with the virtual PIs
    CHECK(vpi.size() == 8);
    vpi.remove_virtual_input_nodes();
    // After removing/remapping the virtual PIs to their real PIs the sizes are equal
    CHECK(vpi.real_size() == vpi.size());
    CHECK(vpi.size() == 6);
}

TEST_CASE("Remove PIs from technology network and check equivalence", "[virtual-pi-view]")
{
    technology_network                     tec{};
    virtual_pi_network<technology_network> vpi{};

    const auto a = vpi.create_pi();
    const auto b = vpi.create_pi();

    const auto a_t = tec.create_pi();
    const auto b_t = tec.create_pi();

    const auto c = vpi.create_virtual_pi(a);
    const auto d = vpi.create_virtual_pi(b);

    const auto f1 = vpi.create_and(a, b);
    const auto f2 = vpi.create_and(b, c);
    const auto f3 = vpi.create_or(a, d);

    const auto f1_t = tec.create_and(a_t, b);
    const auto f2_t = tec.create_and(b_t, a_t);
    const auto f3_t = tec.create_or(a_t, b_t);

    vpi.create_po(f1);
    vpi.create_po(f2);
    vpi.create_po(f3);

    tec.create_po(f1_t);
    tec.create_po(f2_t);
    tec.create_po(f3_t);

    vpi.remove_virtual_input_nodes();
    CHECK(vpi.real_size() == vpi.size());
    CHECK(vpi.size() == tec.size());

    mockturtle::equivalence_checking_stats st;
    const auto maybe_cec_m = mockturtle::equivalence_checking(*fiction::miter<technology_network>(tec, vpi), {}, &st);
    REQUIRE(maybe_cec_m.has_value());
    CHECK(*maybe_cec_m == 1);
}

TEMPLATE_TEST_CASE("Remove PIs and check equivalence", "[virtual-pi-view]", mockturtle::aig_network,
                   mockturtle::xag_network)
{
    TestType                     tec{};
    virtual_pi_network<TestType> vpi{};

    const auto a = vpi.create_pi();
    const auto b = vpi.create_pi();

    const auto a_t = tec.create_pi();
    const auto b_t = tec.create_pi();

    const auto c = vpi.create_virtual_pi(a);
    const auto d = vpi.create_virtual_pi(b);

    const auto f1 = vpi.create_and(a, b);
    const auto f2 = vpi.create_and(b, c);
    const auto f3 = vpi.create_and(a, d);

    const auto f1_t = tec.create_and(a_t, b);
    const auto f2_t = tec.create_and(b_t, a_t);
    const auto f3_t = tec.create_and(a_t, b_t);

    vpi.create_po(f1);
    vpi.create_po(f2);
    vpi.create_po(f3);

    tec.create_po(f1_t);
    tec.create_po(f2_t);
    tec.create_po(f3_t);

    vpi.remove_virtual_input_nodes();
    CHECK(vpi.real_size() == vpi.size());
    CHECK(vpi.size() == tec.size());

    mockturtle::equivalence_checking_stats st;
    const auto maybe_cec_m = mockturtle::equivalence_checking(*fiction::miter<technology_network>(tec, vpi), {}, &st);
    REQUIRE(maybe_cec_m.has_value());
    CHECK(*maybe_cec_m == 1);
}

TEST_CASE("Remove PIs and check equivalence with different sizes", "[virtual-pi-view]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto x3 = tec.create_pi();
    const auto x4 = tec.create_pi();
    const auto x5 = tec.create_pi();
    const auto f1 = tec.create_not(x2);
    const auto f2 = tec.create_nary_and({x1, x2, x3, x4});
    const auto f3 = tec.create_nary_and({x3, x4, x5});
    tec.create_po(f1);
    tec.create_po(f2);
    tec.create_po(f3);

    virtual_pi_network<technology_network> vpi{};

    const auto a = vpi.create_pi();
    const auto b = vpi.create_pi();
    const auto c = vpi.create_pi();
    const auto d = vpi.create_pi();

    const auto d_v = vpi.create_virtual_pi(d);

    const auto e = vpi.create_pi();

    const auto f7  = vpi.create_not(b);
    const auto f8  = vpi.create_and(a, b);
    const auto f9  = vpi.create_and(c, d);
    const auto f10 = vpi.create_buf(c);
    const auto f11 = vpi.create_and(d_v, e);

    const auto f12 = vpi.create_buf(f7);
    const auto f13 = vpi.create_and(f8, f9);
    const auto f14 = vpi.create_and(f10, f11);

    vpi.create_po(f12);
    vpi.create_po(f13);
    vpi.create_po(f14);

    vpi.remove_virtual_input_nodes();
    CHECK(vpi.real_size() == vpi.size());
    CHECK(vpi.size() - 2 == tec.size());  // the -2 is due to the buffers

    mockturtle::equivalence_checking_stats st;
    const auto maybe_cec_m = mockturtle::equivalence_checking(*fiction::miter<technology_network>(tec, vpi), {}, &st);
    REQUIRE(maybe_cec_m.has_value());
    CHECK(*maybe_cec_m == 1);
}
