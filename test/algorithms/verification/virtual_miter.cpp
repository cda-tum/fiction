//
// Created by benjamin on 8/26/24.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/verification/virtual_miter.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/virtual_pi_network.hpp>

#include <mockturtle/algorithms/equivalence_checking.hpp>

using namespace fiction;

TEST_CASE("Miter network positions", "[virtual-miter]")
{
    technology_network tec{};
    const auto         x1 = tec.create_pi();
    const auto         x2 = tec.create_pi();
    const auto         a1 = tec.create_and(x1, x2);
    const auto         o1 = tec.create_or(x1, x2);
    tec.create_po(a1);
    tec.create_po(o1);

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

    mockturtle::equivalence_checking_stats st;
    auto                                   maybe_cec_m =
        mockturtle::equivalence_checking(*fiction::virtual_miter<technology_network>(vpi_ntk_1, tec), {}, &st);
    REQUIRE(maybe_cec_m.has_value());
    CHECK(*maybe_cec_m == 1);

    maybe_cec_m =
        mockturtle::equivalence_checking(*fiction::virtual_miter<technology_network>(tec, vpi_ntk_2), {}, &st);
    REQUIRE(maybe_cec_m.has_value());
    CHECK(*maybe_cec_m == 1);

    maybe_cec_m =
        mockturtle::equivalence_checking(*fiction::virtual_miter<technology_network>(vpi_ntk_1, vpi_ntk_2), {}, &st);
    REQUIRE(maybe_cec_m.has_value());
    CHECK(*maybe_cec_m == 1);
}