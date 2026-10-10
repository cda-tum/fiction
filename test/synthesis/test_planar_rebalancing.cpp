/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Tests for `fiction/synthesis/planar_rebalancing.hpp`.
 * @author Benjamin Hien (hibenj)
 */

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"

#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/mutable_rank_view.hpp>
#include <fiction/synthesis/fanout_substitution.hpp>
#include <fiction/synthesis/network_balancing.hpp>
#include <fiction/synthesis/node_duplication_planarization.hpp>
#include <fiction/synthesis/planar_fanout_substitution.hpp>
#include <fiction/synthesis/planar_rebalancing.hpp>
#include <fiction/utils/graph/mincross.hpp>
#include <fiction/verification/virtual_miter.hpp>

#include <mockturtle/algorithms/cleanup.hpp>
#include <mockturtle/algorithms/equivalence_checking.hpp>

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <vector>

using namespace fiction;
using namespace fiction::networks;
using namespace fiction::networks::views;
using namespace fiction::synthesis;
using namespace fiction::utils::graph;
using namespace fiction::verification;

namespace
{

/**
 * Balances with unified outputs and ranks the given network without substituting fanouts.
 */
template <typename Ntk>
mutable_rank_view<technology_network> rank(const Ntk& ntk)
{
    network_balancing_params ps{};
    ps.unify_outputs           = true;
    ps.buffer_constant_outputs = false;

    return mutable_rank_view{network_balancing<technology_network>(ntk, ps)};
}

/**
 * Counts the crossings of a ranked network without reordering it.
 */
template <typename Ntk>
uint64_t count_crossings(const Ntk& ntk)
{
    mincross_params ps{};
    ps.optimize = false;
    mincross_stats st{};

    mincross(ntk, ps, &st);

    return st.num_crossings;
}

/**
 * Counts the buffers of a network that drive exactly one consumer.
 */
template <typename Ntk>
uint32_t count_chain_buffers(const Ntk& ntk)
{
    uint32_t count = 0;
    ntk.foreach_gate(
        [&ntk, &count](const auto& n)
        {
            if (ntk.is_buf(n) && ntk.fanout_size(n) == 1)
            {
                ++count;
            }
        });

    return count;
}

/**
 * Checks that the network computes the same functions as the original, virtual primary inputs included.
 */
template <typename Spec, typename Impl>
void check_equivalent(const Spec& spec, const Impl& impl)
{
    const auto miter = virtual_miter<technology_network>(spec, impl);
    REQUIRE(miter.has_value());

    if (miter.has_value())  // the optional-access check does not model REQUIRE
    {
        mockturtle::equivalence_checking_stats st{};
        const auto                             cec = mockturtle::equivalence_checking(*miter, {}, &st);

        REQUIRE(cec.has_value());
        CHECK(cec.value_or(false));
    }
}

/**
 * Checks the full contract: balanced with unified outputs, fanout-substituted, crossing-free, equivalent.
 */
template <typename Spec, typename Impl>
void check_contract(const Spec& spec, const Impl& impl, const fanout_substitution_params& fs_ps = {})
{
    CHECK(is_balanced(impl, {.unify_outputs = true, .buffer_constant_outputs = false}));
    CHECK(is_fanout_substituted(impl, fs_ps));
    CHECK(count_crossings(impl) == 0);
    check_equivalent(spec, impl);
}

}  // namespace

TEST_CASE("Buffer chains are removed and the minimum is re-inserted", "[planar-rebalancing]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();

    // x1 reaches the gate through two buffers that balancing would not need
    const auto b1 = tec.create_buf(x1);
    const auto b2 = tec.create_buf(b1);
    const auto f1 = tec.create_and(b2, x2);
    tec.create_po(f1);

    // the input is not balanced: x2 enters the gate directly
    const mutable_rank_view ranked{tec};
    REQUIRE(!is_balanced(ranked));

    const auto rebalanced = planar_rebalancing(ranked);

    check_contract(tec, rebalanced);
    // both chain buffers of x1 are gone, and the gate sits right above its inputs: no buffer is needed
    CHECK(count_chain_buffers(rebalanced) == 0);
    CHECK(rebalanced.size() == tec.size() - 2);
}

TEST_CASE("Primary inputs that are primary outputs", "[planar-rebalancing]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();

    tec.create_po(x1);
    tec.create_po(tec.create_and(x1, x2));

    // balancing buffers the direct output; removing that buffer makes x1 drive the output again
    const auto substituted = planar_fanout_substitution(node_duplication_planarization(rank(tec)));
    const auto rebalanced  = planar_rebalancing(substituted);

    check_contract(tec, rebalanced);
    CHECK(rebalanced.depth() == substituted.depth());
}

TEST_CASE("Inputs that are not planar are rejected", "[planar-rebalancing]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();

    // x1 -> buffer and x2 -> gate cross once the gate precedes the buffer in its rank
    tec.create_po(tec.create_and(x1, x2));
    tec.create_po(x1);

    const auto ranked = rank(tec);
    REQUIRE(count_crossings(ranked) > 0);

    CHECK_THROWS_AS(planar_rebalancing(ranked), std::runtime_error);
}

TEST_CASE("Gates with constant fanins", "[planar-rebalancing]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();

    const auto f1 = tec.create_and(x1, tec.get_constant(true));
    const auto f2 = tec.create_or(f1, x2);
    tec.create_po(f2);

    const auto ranked     = rank(tec);
    const auto rebalanced = planar_rebalancing(ranked);

    check_contract(tec, rebalanced);
}

TEST_CASE("Constant outputs", "[planar-rebalancing]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();

    tec.create_po(tec.create_and(x1, x2));
    tec.create_po(tec.get_constant(true));
    tec.create_po(tec.get_constant(false));

    const auto substituted = planar_fanout_substitution(node_duplication_planarization(rank(tec)));
    const auto rebalanced  = planar_rebalancing(substituted);

    check_contract(tec, rebalanced);
    CHECK(rebalanced.num_pos() == 3);
}

TEST_CASE("A node that drives an output and other gates", "[planar-rebalancing]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();

    const auto f1 = tec.create_and(x1, x2);
    tec.create_po(f1);
    tec.create_po(tec.create_not(f1));

    const auto ranked     = planar_fanout_substitution(rank(tec));
    const auto rebalanced = planar_rebalancing(ranked);

    check_contract(tec, rebalanced);
    CHECK(mockturtle::cleanup_dangling(rebalanced).size() == rebalanced.size());
}

TEST_CASE("The planarization pipeline on blueprint networks", "[planar-rebalancing]")
{
    for (const auto& ntk :
         {blueprints::full_adder_network<technology_network>(), blueprints::parity_network<technology_network>(),
          blueprints::mux21_network<technology_network>(), blueprints::clpl<technology_network>()})
    {
        const auto planar      = node_duplication_planarization(rank(ntk));
        const auto substituted = planar_fanout_substitution(planar);
        const auto rebalanced  = planar_rebalancing(substituted);

        check_contract(ntk, rebalanced);
        CHECK(rebalanced.size() <= substituted.size());
        CHECK(rebalanced.num_virtual_pis() == planar.num_virtual_pis());
        CHECK(rebalanced.rank_width(0) == planar.rank_width(0));
        CHECK(mockturtle::cleanup_dangling(rebalanced).size() == rebalanced.size());
    }
}

TEST_CASE("Fanout degree three is kept", "[planar-rebalancing]")
{
    const auto ntk    = blueprints::parity_network<technology_network>();
    const auto planar = node_duplication_planarization(rank(ntk));

    planar_fanout_substitution_params ps{};
    ps.degree = 3;

    const auto substituted = planar_fanout_substitution(planar, ps);
    const auto rebalanced  = planar_rebalancing(substituted);

    check_contract(ntk, rebalanced, {.degree = 3});
}

TEST_CASE("Progress is reported", "[planar-rebalancing]")
{
    const auto ntk         = blueprints::full_adder_network<technology_network>();
    const auto substituted = planar_fanout_substitution(node_duplication_planarization(rank(ntk)));

    std::vector<std::size_t> done{};
    std::size_t              total = 0;

    planar_rebalancing_params ps{};
    ps.on_progress = [&done, &total](std::string_view, const std::size_t d, const std::size_t t)
    {
        done.push_back(d);
        total = t;
    };

    const auto rebalanced = planar_rebalancing(substituted, ps);

    check_contract(ntk, rebalanced);
    CHECK(total == 3);
    REQUIRE(!done.empty());
    CHECK(done.back() == total);
}

TEST_CASE("An output driven directly by a node that also drives a gate", "[planar-rebalancing]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();

    const auto n = tec.create_not(x1);
    const auto m = tec.create_not(x2);
    const auto g = tec.create_and(n, m);
    tec.create_po(g);
    tec.create_po(n);

    const mutable_rank_view ranked{tec};
    REQUIRE(count_crossings(ranked) == 0);

    const auto rebalanced = planar_rebalancing(ranked);

    // the input is not fanout-substituted (n drives the gate and the output), so the result is not either
    CHECK(is_balanced(rebalanced, {.unify_outputs = true, .buffer_constant_outputs = false}));
    CHECK(count_crossings(rebalanced) == 0);
    check_equivalent(tec, rebalanced);
}
