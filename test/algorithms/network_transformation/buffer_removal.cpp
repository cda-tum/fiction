//
// Created by benjamin on 9/24/24.
//

#include <catch2/catch_test_macros.hpp>

#include "fiction/algorithms/network_transformation/fanout_substitution.hpp"
#include "fiction/algorithms/network_transformation/network_balancing.hpp"
#include "fiction/algorithms/network_transformation/node_duplication_planarization.hpp"
#include "fiction/algorithms/verification/virtual_miter.hpp"
#include "fiction/networks/technology_network.hpp"
#include "fiction/utils/debug/network_writer.hpp"

#include <fiction/algorithms/network_transformation/buffer_removal.hpp>

#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/algorithms/miter.hpp>

using namespace fiction;

TEST_CASE("Remove one buffer", "[buffer-removal]")
{
    technology_network tec{};
    const auto         p0 = tec.create_pi();
    const auto         p1 = tec.create_pi();
    const auto         b0 = tec.create_buf(p0);
    const auto         a0 = tec.create_and(b0, p1);
    tec.create_po(a0);

    const auto removed = remove_buffer(tec);

    mockturtle::equivalence_checking_stats st;
    const auto cec_m = mockturtle::equivalence_checking(*mockturtle::miter<technology_network>(tec, removed), {}, &st);
    REQUIRE(cec_m.has_value());
    CHECK(cec_m.value() == 1);

    CHECK(tec.num_gates() == removed.num_gates() + 1);
}

TEST_CASE("Remove one buffer chain", "[buffer-removal]")
{
    technology_network tec{};
    const auto         p0 = tec.create_pi();
    const auto         p1 = tec.create_pi();
    const auto         b0 = tec.create_buf(p0);
    const auto         b1 = tec.create_buf(b0);
    const auto         b2 = tec.create_buf(b1);
    const auto         a0 = tec.create_and(b2, p1);
    tec.create_po(a0);

    const auto removed = remove_buffer(tec);

    mockturtle::equivalence_checking_stats st;
    const auto cec_m = mockturtle::equivalence_checking(*mockturtle::miter<technology_network>(tec, removed), {}, &st);
    REQUIRE(cec_m.has_value());
    CHECK(cec_m.value() == 1);

    CHECK(tec.num_gates() == removed.num_gates() + 3);
}

TEST_CASE("Remove multiple buffers", "[buffer-removal]")
{
    fiction::technology_network tec;
    const auto                  pi0 = tec.create_pi();
    const auto                  pi1 = tec.create_pi();
    const auto                  pi2 = tec.create_pi();
    const auto                  pi3 = tec.create_pi();
    const auto                  pi4 = tec.create_pi();

    const auto a0 = tec.create_and(pi0, pi1);
    const auto o0 = tec.create_or(pi0, pi1);
    const auto a1 = tec.create_and(pi3, pi4);
    const auto o1 = tec.create_or(pi3, pi4);

    const auto a2 = tec.create_and(o0, pi2);
    const auto a3 = tec.create_and(o1, pi2);
    const auto a4 = tec.create_and(a2, a3);

    tec.create_po(a0);
    tec.create_po(a4);
    tec.create_po(a1);

    fiction::network_balancing_params ps;
    ps.unify_outputs = true;

    const auto _b = fiction::network_balancing<fiction::technology_network>(
        fiction::fanout_substitution<fiction::technology_network>(tec), ps);

    fiction::node_duplication_planarization_params dps;
    dps.po_order = fiction::node_duplication_planarization_params::output_order::KEEP_PO_ORDER;

    auto planarized_b = fiction::node_duplication_planarization<fiction::technology_network>(_b, dps);
    // clone to get rid of the extended_rank_view
    auto planar = planarized_b.clone();
    // remove the buffer nodes
    auto removed = remove_buffer(planar);

    mockturtle::equivalence_checking_stats st;
    const auto                             cec_m =
        mockturtle::equivalence_checking(*fiction::virtual_miter<technology_network>(tec, removed), {}, &st);
    REQUIRE(cec_m.has_value());
    CHECK(cec_m.value() == 1);
}
