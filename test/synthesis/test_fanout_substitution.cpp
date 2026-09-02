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
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/networks/technology_network.hpp>
#include <fiction/synthesis/fanout_substitution.hpp>
#include <fiction/synthesis/network_balancing.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/views/depth_view.hpp>
#include <mockturtle/views/names_view.hpp>

#include <cstdint>

using namespace fiction;

template <typename Ntk>
void substitute(const Ntk& ntk, const synthesis::fanout_substitution_params ps, const uint32_t size)
{
    const auto substituted = synthesis::fanout_substitution<networks::technology_network>(ntk, ps);

    CHECK(substituted.size() == size);
    CHECK(synthesis::is_fanout_substituted(substituted, ps));

    check_eq(ntk, substituted);
}

TEST_CASE("Name conservation after fanout substitution", "[fanout-substitution]")
{
    auto maj = blueprints::maj1_network<mockturtle::names_view<mockturtle::mig_network>>();
    maj.set_network_name("maj");

    const auto substituted_maj =
        synthesis::fanout_substitution<mockturtle::names_view<fiction::networks::technology_network>>(maj);

    // network name
    CHECK(substituted_maj.get_network_name() == "maj");

    // PI names
    CHECK(substituted_maj.get_name(substituted_maj.make_signal(2)) == "a");
    CHECK(substituted_maj.get_name(substituted_maj.make_signal(3)) == "b");
    CHECK(substituted_maj.get_name(substituted_maj.make_signal(4)) == "c");

    // PO names
    CHECK(substituted_maj.get_output_name(0) == "f");
}

TEST_CASE("Simple fanout substitution", "[fanout-substitution]")
{
    const auto tec = blueprints::multi_output_and_network<networks::technology_network>();

    const synthesis::fanout_substitution_params ps_depth{
        .strategy = synthesis::fanout_substitution_params::substitution_strategy::DEPTH};
    const synthesis::fanout_substitution_params ps_breadth{
        .strategy = synthesis::fanout_substitution_params::substitution_strategy::BREADTH};
    const synthesis::fanout_substitution_params ps_random{
        .strategy = synthesis::fanout_substitution_params::substitution_strategy::RANDOM};

    substitute(tec, ps_depth, tec.size() + 3);
    substitute(tec, ps_breadth, tec.size() + 3);
    substitute(tec, ps_random, tec.size() + 3);
}

TEST_CASE("Complex fanout substitution", "[fanout-substitution]")
{
    const auto tec = blueprints::maj4_network<networks::technology_network>();
    CHECK(!synthesis::is_fanout_substituted(tec));

    const synthesis::fanout_substitution_params ps_depth{
        .strategy = synthesis::fanout_substitution_params::substitution_strategy::DEPTH};
    const synthesis::fanout_substitution_params ps_breadth{
        .strategy = synthesis::fanout_substitution_params::substitution_strategy::BREADTH};
    const synthesis::fanout_substitution_params ps_random{
        .strategy = synthesis::fanout_substitution_params::substitution_strategy::RANDOM};

    substitute(tec, ps_depth, tec.size() + 7);
    substitute(tec, ps_breadth, tec.size() + 7);
    substitute(tec, ps_random, tec.size() + 7);

    const auto aig = blueprints::maj4_network<mockturtle::aig_network>();
    CHECK(!synthesis::is_fanout_substituted(aig));
    substitute(aig, ps_depth, aig.size() + 41);
    substitute(aig, ps_breadth, aig.size() + 41);
    substitute(aig, ps_random, aig.size() + 41);
}

TEST_CASE("Degree and threshold in fanout substitution", "[fanout-substitution]")
{
    const auto aig = blueprints::maj4_network<mockturtle::aig_network>();

    const synthesis::fanout_substitution_params ps_31{
        .strategy  = synthesis::fanout_substitution_params::substitution_strategy::BREADTH,
        .degree    = 3,
        .threshold = 1};
    const synthesis::fanout_substitution_params ps_22{
        .strategy  = synthesis::fanout_substitution_params::substitution_strategy::DEPTH,
        .degree    = 2,
        .threshold = 2};
    const synthesis::fanout_substitution_params ps_32{
        .strategy  = synthesis::fanout_substitution_params::substitution_strategy::RANDOM,
        .degree    = 3,
        .threshold = 2};

    substitute(aig, ps_31, aig.size() + 35);
    substitute(aig, ps_22, aig.size() + 34);
    substitute(aig, ps_32, aig.size() + 32);
}

TEST_CASE("Random fanout substitution with fixed vs. varying seeds", "[fanout-substitution]")
{
    const auto aig = blueprints::maj4_network<mockturtle::aig_network>();

    SECTION("Fixed seed yields deterministic behavior")
    {
        synthesis::fanout_substitution_params ps{
            .strategy = synthesis::fanout_substitution_params::substitution_strategy::RANDOM};
        ps.seed = 42;

        // expect no exceptions and consistent substitution
        substitute(aig, ps, aig.size() + 41);
    }

    SECTION("Different seeds produce different results")
    {
        synthesis::fanout_substitution_params ps{
            .strategy = synthesis::fanout_substitution_params::substitution_strategy::RANDOM};

        // compute baseline depth using seed = 1
        ps.seed = 1;
        const auto base_sub =
            mockturtle::depth_view{synthesis::fanout_substitution<networks::technology_network>(aig, ps)};
        const auto base_depth = base_sub.depth();

        bool found_different = false;
        // try seeds 2 through 20; break as soon as we see a different depth
        for (auto s = 2u; s <= 20u; ++s)
        {
            ps.seed = s;
            const auto sub =
                mockturtle::depth_view{synthesis::fanout_substitution<networks::technology_network>(aig, ps)};
            if (sub.depth() != base_depth)
            {
                found_different = true;
                break;
            }
        }

        REQUIRE(found_different);
    }
}

TEST_CASE("Consistent network size after multiple fanout substitutions", "[fanout-substitution]")
{
    const auto aig = blueprints::maj4_network<mockturtle::aig_network>();

    auto substituted = synthesis::fanout_substitution<networks::technology_network>(aig);

    auto subsubsubsubstituted = synthesis::fanout_substitution<networks::technology_network>(
        synthesis::fanout_substitution<networks::technology_network>(
            synthesis::fanout_substitution<networks::technology_network>(substituted)));

    CHECK(substituted.size() == subsubsubsubstituted.size());
}

TEST_CASE("Consistent fanout substitution after balancing", "[fanout-substitution]")
{
    const auto aig = blueprints::maj4_network<mockturtle::aig_network>();

    auto substituted = synthesis::fanout_substitution<networks::technology_network>(aig);

    CHECK(synthesis::is_fanout_substituted(substituted));
    auto balanced = synthesis::network_balancing<networks::technology_network>(substituted);
    CHECK(synthesis::is_fanout_substituted(balanced));

    auto tec = blueprints::fanout_substitution_corner_case_network<networks::technology_network>();

    auto substituted_tec = synthesis::fanout_substitution<networks::technology_network>(tec);
    CHECK(synthesis::is_fanout_substituted(substituted_tec));
    auto balanced_tec = synthesis::network_balancing<networks::technology_network>(substituted_tec);
    CHECK(synthesis::is_fanout_substituted(balanced_tec));
}
