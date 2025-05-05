//
// Created by benjamin on 13.04.23.
//

#include <catch2/catch_test_macros.hpp>

#include "fiction/algorithms/properties/count_gate_types.hpp"
#include "fiction/algorithms/verification/virtual_miter.hpp"
#include "fiction/networks/views/mutable_rank_view.hpp"

#include <fiction/algorithms/network_transformation/inverter_substitution.hpp>
#include <fiction/networks/technology_network.hpp>

#include <mockturtle/algorithms/equivalence_checking.hpp>

#include <cassert>
#include <cstdlib>
#include <vector>

using namespace fiction;

TEST_CASE("Minimal FO inverter substitution with output preservation, [inverter-substitution]")
{
    technology_network tec{};
    const auto         pi0  = tec.create_pi();
    const auto         buf0 = tec.create_buf(pi0);
    const auto         inv0 = tec.create_not(buf0);
    const auto         inv1 = tec.create_not(buf0);
    tec.create_po(inv0);
    tec.create_po(inv1);

    auto substituted_network = inverter_substitution(tec);

    count_gate_types_stats st_before{};
    count_gate_types(tec, &st_before);

    count_gate_types_stats st_after{};
    count_gate_types(substituted_network, &st_after);

    CHECK(st_before.num_inv == 2);
    CHECK(st_after.num_inv == 1);
    CHECK(st_before.num_fanout == st_after.num_fanout);
}

TEST_CASE("Ranked FO inverter substitution, [inverter-substitution]")
{
    technology_network tec{};
    const auto         pi0  = tec.create_pi();
    const auto         buf0 = tec.create_buf(pi0);
    const auto         inv0 = tec.create_not(buf0);
    const auto         inv1 = tec.create_not(buf0);
    const auto         buf1 = tec.create_buf(inv0);
    const auto         buf2 = tec.create_buf(inv1);
    tec.create_po(buf1);
    tec.create_po(buf2);

    std::vector<mockturtle::node<fiction::technology_network>> nodes;
    const auto                                                 tec_rank = mutable_rank_view(tec);
    tec_rank.foreach_node(
        [&](const auto n)
        {
            if (tec_rank.is_fanout(n))
            {
                nodes.push_back(n);
            }
        });
    CHECK(tec_rank.level(nodes[0]) == 1);

    auto substituted_network = inverter_substitution(tec_rank);
    nodes.clear();
    substituted_network.foreach_node(
        [&](const auto n)
        {
            if (substituted_network.is_fanout(n))
            {
                nodes.push_back(n);
            }
        });
    CHECK(substituted_network.level(nodes[0]) == 2);

    count_gate_types_stats st_before{};
    count_gate_types(tec, &st_before);

    count_gate_types_stats st_after{};
    count_gate_types(substituted_network, &st_after);

    CHECK(st_before.num_inv == 2);
    CHECK(st_after.num_inv == 1);
    CHECK(st_before.num_fanout == 1);
    CHECK(st_after.num_fanout == 1);
}

TEST_CASE("Minimal AND/OR inverter substitution, [inverter-substitution]")
{
    technology_network tec_and{};
    const auto         pi0  = tec_and.create_pi();
    const auto         pi1  = tec_and.create_pi();
    const auto         inv0 = tec_and.create_not(pi0);
    const auto         inv1 = tec_and.create_not(pi1);
    const auto         and0 = tec_and.create_and(inv0, inv1);
    tec_and.create_po(and0);
    detail::operation_mode mode                = detail::operation_mode::AND_OR_ONLY;
    auto                   substituted_network = inverter_substitution(tec_and, mode);

    count_gate_types_stats st_before{};
    count_gate_types(tec_and, &st_before);

    count_gate_types_stats st_after{};
    count_gate_types(substituted_network, &st_after);

    CHECK(st_before.num_inv == 2);
    CHECK(st_after.num_inv == 1);
    CHECK(st_before.num_and2 == st_after.num_or2);

    technology_network tec_or{};
    const auto         pi0_or  = tec_or.create_pi();
    const auto         pi1_or  = tec_or.create_pi();
    const auto         inv0_or = tec_or.create_not(pi0_or);
    const auto         inv1_or = tec_or.create_not(pi1_or);
    const auto         and0_or = tec_or.create_or(inv0_or, inv1_or);

    tec_or.create_po(and0_or);

    mode                = detail::operation_mode::AND_OR_ONLY;
    substituted_network = inverter_substitution(tec_or, mode);

    count_gate_types(tec_or, &st_before);
    count_gate_types(substituted_network, &st_after);

    CHECK(st_before.num_inv == 2);
    CHECK(st_after.num_inv == 1);
    CHECK(st_before.num_or2 == st_after.num_and2);
}

TEST_CASE("Propagating AND/OR inverter substitution, [inverter-substitution]")
{
    technology_network tec{};
    const auto         pi0  = tec.create_pi();
    const auto         pi1  = tec.create_pi();
    const auto         pi2  = tec.create_pi();
    const auto         inv0 = tec.create_not(pi0);
    const auto         inv1 = tec.create_not(pi1);
    const auto         buf0 = tec.create_buf(pi2);
    const auto         inv2 = tec.create_not(buf0);
    const auto         and0 = tec.create_and(inv0, inv1);
    const auto         or0  = tec.create_or(and0, inv2);
    tec.create_po(or0);

    const detail::operation_mode mode                = detail::operation_mode::AND_OR_ONLY;
    auto                         substituted_network = inverter_substitution(tec, mode);

    count_gate_types_stats st_before{};
    count_gate_types(tec, &st_before);

    count_gate_types_stats st_after{};
    count_gate_types(substituted_network, &st_after);

    CHECK(st_before.num_inv == 3);
    CHECK(st_after.num_inv == 1);
    CHECK(st_before.num_and2 == st_after.num_or2);
    CHECK(st_before.num_or2 == st_after.num_and2);

    mockturtle::equivalence_checking_stats eq_st;
    const auto                             cec_m = mockturtle::equivalence_checking(
        *fiction::virtual_miter<fiction::technology_network>(substituted_network, tec), {}, &eq_st);
    REQUIRE(cec_m.has_value());
    CHECK(cec_m == 1);
}
