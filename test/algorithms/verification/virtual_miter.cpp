//
// Created by benjamin on 31.07.24.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/verification/virtual_miter.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/virtual_pi_network.hpp>

#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/networks/xmg.hpp>
#include <mockturtle/views/dont_care_view.hpp>

using namespace fiction;

TEST_CASE("Virtual miter different num_pis", "[virtual-miter]")
{
    virtual_pi_network<technology_network> vpi_ntk_1{};
    const auto                             x1_v = vpi_ntk_1.create_pi();
    const auto                             x2_v = vpi_ntk_1.create_pi();
    const auto                             v1   = vpi_ntk_1.create_pi();
    const auto                             a1_v = vpi_ntk_1.create_and(x1_v, x2_v);
    const auto                             o1_v = vpi_ntk_1.create_or(v1, x2_v);
    vpi_ntk_1.create_po(a1_v);
    vpi_ntk_1.create_po(o1_v);

    virtual_pi_network<technology_network> vpi_ntk_2{};
    const auto                             x1_v2 = vpi_ntk_2.create_pi();
    const auto                             x2_v2 = vpi_ntk_2.create_pi();
    const auto                             v1_2  = vpi_ntk_2.create_virtual_pi(x1_v2);
    const auto                             a1_v2 = vpi_ntk_2.create_and(v1_2, x2_v2);
    const auto                             o1_v2 = vpi_ntk_2.create_or(x1_v2, x2_v2);
    vpi_ntk_2.create_po(a1_v2);
    vpi_ntk_2.create_po(o1_v2);

    auto miter_network = fiction::virtual_miter<technology_network>(vpi_ntk_1, vpi_ntk_2);
    CHECK(!miter_network.has_value());
}

TEST_CASE("Virtual miter with technology networks", "[virtual-miter]")
{
    technology_network tec{};
    const auto         x1 = tec.create_pi();
    const auto         x2 = tec.create_pi();
    const auto         a1 = tec.create_and(x1, x2);
    const auto         o1 = tec.create_or(x1, x2);
    tec.create_po(a1);
    tec.create_po(o1);

    mockturtle::dont_care_view<technology_network, false, true> const tec_dc(tec);

    virtual_pi_network<technology_network> vpi_ntk_1{};
    const auto                             x1_v = vpi_ntk_1.create_pi();
    const auto                             x2_v = vpi_ntk_1.create_pi();
    const auto                             v1   = vpi_ntk_1.create_virtual_pi(x1_v);
    const auto                             a1_v = vpi_ntk_1.create_and(x1_v, x2_v);
    const auto                             o1_v = vpi_ntk_1.create_or(v1, x2_v);
    vpi_ntk_1.create_po(a1_v);
    vpi_ntk_1.create_po(o1_v);

    virtual_pi_network<technology_network> vpi_ntk_2{};
    const auto                             x1_v2 = vpi_ntk_2.create_pi();
    const auto                             x2_v2 = vpi_ntk_2.create_pi();
    const auto                             v1_2  = vpi_ntk_2.create_virtual_pi(x1_v2);
    const auto                             a1_v2 = vpi_ntk_2.create_and(v1_2, x2_v2);
    const auto                             o1_v2 = vpi_ntk_2.create_or(x1_v2, x2_v2);
    vpi_ntk_2.create_po(a1_v2);
    vpi_ntk_2.create_po(o1_v2);

    // check for the exodc path 1
    mockturtle::equivalence_checking_stats st;
    auto                                   maybe_cec_m =
        mockturtle::equivalence_checking(*fiction::virtual_miter<technology_network>(tec, tec_dc), {}, &st);
    REQUIRE(maybe_cec_m.has_value());
    if (maybe_cec_m.has_value())
    {
        CHECK(*maybe_cec_m == 1);
    }
    // check for the exodc path 2
    maybe_cec_m = mockturtle::equivalence_checking(*fiction::virtual_miter<technology_network>(tec_dc, tec), {}, &st);
    REQUIRE(maybe_cec_m.has_value());
    if (maybe_cec_m.has_value())
    {
        CHECK(*maybe_cec_m == 1);
    }
    // check for the handle virtual pi path 1
    maybe_cec_m =
        mockturtle::equivalence_checking(*fiction::virtual_miter<technology_network>(vpi_ntk_1, tec), {}, &st);
    REQUIRE(maybe_cec_m.has_value());
    if (maybe_cec_m.has_value())
    {
        CHECK(*maybe_cec_m == 1);
    }
    // check for the handle virtual pi path 2
    maybe_cec_m =
        mockturtle::equivalence_checking(*fiction::virtual_miter<technology_network>(tec, vpi_ntk_2), {}, &st);
    REQUIRE(maybe_cec_m.has_value());
    if (maybe_cec_m.has_value())
    {
        CHECK(*maybe_cec_m == 1);
    }
    // check for the handle virtual pi path 3
    maybe_cec_m =
        mockturtle::equivalence_checking(*fiction::virtual_miter<technology_network>(vpi_ntk_1, vpi_ntk_2), {}, &st);
    REQUIRE(maybe_cec_m.has_value());
    if (maybe_cec_m.has_value())
    {
        CHECK(*maybe_cec_m == 1);
    }
}

TEMPLATE_TEST_CASE("Virtual miter with mockturtle networks", "[virtual-miter]", mockturtle::aig_network,
                   mockturtle::xag_network, mockturtle::mig_network, mockturtle::xmg_network)
{
    TestType   test_ntk{};
    const auto x1 = test_ntk.create_pi();
    const auto x2 = test_ntk.create_pi();
    const auto a1 = test_ntk.create_and(x1, x2);
    const auto o1 = test_ntk.create_or(x1, x2);
    test_ntk.create_po(a1);
    test_ntk.create_po(o1);

    mockturtle::dont_care_view<TestType, false, true> const test_dc(test_ntk);

    virtual_pi_network<technology_network> vpi_ntk_1{};
    const auto                             x1_v = vpi_ntk_1.create_pi();
    const auto                             x2_v = vpi_ntk_1.create_pi();
    const auto                             v1   = vpi_ntk_1.create_virtual_pi(x1_v);
    const auto                             a1_v = vpi_ntk_1.create_and(x1_v, x2_v);
    const auto                             o1_v = vpi_ntk_1.create_or(v1, x2_v);
    vpi_ntk_1.create_po(a1_v);
    vpi_ntk_1.create_po(o1_v);

    virtual_pi_network<technology_network> vpi_ntk_2{};
    const auto                             x1_v2 = vpi_ntk_2.create_pi();
    const auto                             x2_v2 = vpi_ntk_2.create_pi();
    const auto                             v1_2  = vpi_ntk_2.create_virtual_pi(x1_v2);
    const auto                             a1_v2 = vpi_ntk_2.create_and(v1_2, x2_v2);
    const auto                             o1_v2 = vpi_ntk_2.create_or(x1_v2, x2_v2);
    vpi_ntk_2.create_po(a1_v2);
    vpi_ntk_2.create_po(o1_v2);

    // check for the exodc path 1
    mockturtle::equivalence_checking_stats st;
    auto                                   maybe_cec_m =
        mockturtle::equivalence_checking(*fiction::virtual_miter<technology_network>(test_ntk, test_dc), {}, &st);
    REQUIRE(maybe_cec_m.has_value());
    if (maybe_cec_m.has_value())
    {
        CHECK(*maybe_cec_m == 1);
    }
    // check for the exodc path 2
    maybe_cec_m =
        mockturtle::equivalence_checking(*fiction::virtual_miter<technology_network>(test_dc, test_ntk), {}, &st);
    REQUIRE(maybe_cec_m.has_value());
    if (maybe_cec_m.has_value())
    {
        CHECK(*maybe_cec_m == 1);
    }
    // check for the handle virtual pi path 1
    maybe_cec_m =
        mockturtle::equivalence_checking(*fiction::virtual_miter<technology_network>(vpi_ntk_1, test_ntk), {}, &st);
    REQUIRE(maybe_cec_m.has_value());
    if (maybe_cec_m.has_value())
    {
        CHECK(*maybe_cec_m == 1);
    }
    // check for the handle virtual pi path 2
    maybe_cec_m =
        mockturtle::equivalence_checking(*fiction::virtual_miter<technology_network>(test_ntk, vpi_ntk_2), {}, &st);
    REQUIRE(maybe_cec_m.has_value());
    if (maybe_cec_m.has_value())
    {
        CHECK(*maybe_cec_m == 1);
    }
    // check for the handle virtual pi path 3
    maybe_cec_m =
        mockturtle::equivalence_checking(*fiction::virtual_miter<technology_network>(vpi_ntk_1, vpi_ntk_2), {}, &st);
    REQUIRE(maybe_cec_m.has_value());
    if (maybe_cec_m.has_value())
    {
        CHECK(*maybe_cec_m == 1);
    }
}
