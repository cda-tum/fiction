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
 * @brief Tests for `fiction/synthesis/planar_fanout_substitution.hpp`.
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
    ps.unify_outputs = true;

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
 * Checks the full contract: fanout-substituted, balanced, crossing-free, equivalent.
 */
template <typename Spec, typename Impl>
void check_contract(const Spec& spec, const Impl& impl, const fanout_substitution_params& fs_ps = {})
{
    CHECK(is_fanout_substituted(impl, fs_ps));
    CHECK(is_balanced(impl, {.unify_outputs = true}));
    CHECK(count_crossings(impl) == 0);
    check_equivalent(spec, impl);
}

}  // namespace

TEST_CASE("One input with five inverted outputs", "[planar-fanout-substitution]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();

    for (std::size_t i = 0; i < 5; ++i)
    {
        tec.create_po(tec.create_not(x1));
    }

    const auto ranked = rank(tec);
    REQUIRE(!is_fanout_substituted(ranked));
    REQUIRE(count_crossings(ranked) == 0);

    const auto substituted = planar_fanout_substitution(ranked);

    check_contract(tec, substituted);
    CHECK(substituted.rank_width(0) == 1);
    CHECK(substituted.num_pos() == 5);
}

TEST_CASE("Nodes of a rank with different fanout counts end up balanced", "[planar-fanout-substitution]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();

    // x1 drives four consecutive gates, x2 drives one: the input is planar
    tec.create_po(tec.create_not(x1));
    tec.create_po(tec.create_buf(x1));
    tec.create_po(tec.create_not(x1));
    tec.create_po(tec.create_buf(x1));
    tec.create_po(tec.create_not(x2));

    const auto ranked = rank(tec);
    REQUIRE(count_crossings(ranked) == 0);

    const auto substituted = planar_fanout_substitution(ranked);

    check_contract(tec, substituted);

    // the tree of x1 has two levels; x2 is padded by two buffers to stay level with the tree's leaves
    CHECK(substituted.depth() == ranked.depth() + 2);
    CHECK(substituted.size() == ranked.size() + 3 + 2);
}

TEST_CASE("Already substituted networks are copied unchanged", "[planar-fanout-substitution]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto f1 = tec.create_buf(x1);  // becomes a fanout node with two outputs
    tec.create_po(tec.create_not(f1));
    tec.create_po(tec.create_and(f1, x2));

    const auto ranked = rank(tec);
    REQUIRE(is_fanout_substituted(ranked));
    REQUIRE(count_crossings(ranked) == 0);

    const auto substituted = planar_fanout_substitution(ranked);

    check_contract(tec, substituted);
    CHECK(substituted.size() == ranked.size());
    CHECK(substituted.depth() == ranked.depth());
}

TEST_CASE("Planarized networks keep their virtual inputs and planarity", "[planar-fanout-substitution]")
{
    for (const auto& ntk :
         {blueprints::full_adder_network<technology_network>(), blueprints::parity_network<technology_network>(),
          blueprints::mux21_network<technology_network>()})
    {
        const auto planar = node_duplication_planarization(rank(ntk));
        REQUIRE(count_crossings(planar) == 0);

        const auto substituted = planar_fanout_substitution(planar);

        check_contract(ntk, substituted);
        CHECK(substituted.num_virtual_pis() == planar.num_virtual_pis());
        CHECK(substituted.rank_width(0) == planar.rank_width(0));

        // the primary inputs keep their order; both networks create them in the same order, so the ids match
        for (uint32_t i = 0; i < planar.rank_width(0); ++i)
        {
            CHECK(substituted.at_rank_position(0, i) == planar.at_rank_position(0, i));
        }
    }
}

TEST_CASE("The fanout degree is respected", "[planar-fanout-substitution]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();

    for (std::size_t i = 0; i < 7; ++i)
    {
        tec.create_po(tec.create_not(x1));
    }

    const auto ranked = rank(tec);
    REQUIRE(count_crossings(ranked) == 0);

    SECTION("degree 2")
    {
        const auto substituted = planar_fanout_substitution(ranked);

        check_contract(tec, substituted);
        // six fanout nodes in three levels
        CHECK(substituted.depth() == ranked.depth() + 3);
    }
    SECTION("degree 3")
    {
        planar_fanout_substitution_params ps{};
        ps.degree = 3;

        const auto substituted = planar_fanout_substitution(ranked, ps);

        check_contract(tec, substituted, {.degree = 3});
        // three fanout nodes in two levels
        CHECK(substituted.depth() == ranked.depth() + 2);
    }
}

TEST_CASE("Progress is reported once per rank", "[planar-fanout-substitution]")
{
    const auto ntk    = blueprints::full_adder_network<technology_network>();
    const auto ranked = node_duplication_planarization(rank(ntk));

    std::vector<std::size_t> done{};
    std::size_t              total = 0;

    planar_fanout_substitution_params ps{};
    ps.on_progress = [&done, &total](std::string_view, const std::size_t d, const std::size_t t)
    {
        done.push_back(d);
        total = t;
    };

    const auto substituted = planar_fanout_substitution(ranked, ps);

    check_contract(ntk, substituted);
    CHECK(total == ranked.depth() + 1);
    REQUIRE(!done.empty());
    CHECK(done.back() == total);
}

TEST_CASE("Invalid parameters and inputs are rejected", "[planar-fanout-substitution]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    tec.create_po(tec.create_and(x1, tec.create_buf(x2)));

    const mutable_rank_view unbalanced{tec};
    CHECK_THROWS_AS(planar_fanout_substitution(unbalanced), std::invalid_argument);

    const auto ranked = rank(blueprints::full_adder_network<technology_network>());

    planar_fanout_substitution_params ps{};
    ps.degree = 1;
    CHECK_THROWS_AS(planar_fanout_substitution(ranked, ps), std::invalid_argument);
}

TEST_CASE("No buffer is left without a consumer", "[planar-fanout-substitution]")
{
    for (const auto& ntk : {blueprints::full_adder_network<technology_network>(),
                            blueprints::parity_network<technology_network>(), blueprints::clpl<technology_network>()})
    {
        const auto planar = node_duplication_planarization(rank(ntk));

        for (const uint32_t degree : {2u, 3u})
        {
            planar_fanout_substitution_params ps{};
            ps.degree = degree;

            const auto substituted = planar_fanout_substitution(planar, ps);

            check_contract(ntk, substituted, {.degree = degree});
            CHECK(mockturtle::cleanup_dangling(substituted).size() == substituted.size());
        }
    }
}
