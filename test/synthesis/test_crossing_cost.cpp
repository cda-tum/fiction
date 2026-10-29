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
 * @brief Tests for `fiction/synthesis/crossing_cost.hpp`.
 * @author Benjamin Hien (hibenj)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "utils/network_ranking_utils.hpp"

#include <fiction/networks/technology_network.hpp>
#include <fiction/synthesis/crossing_cost.hpp>
#include <fiction/synthesis/crossing_gate_planarization.hpp>

#include <mockturtle/traits.hpp>

#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

using namespace fiction;
using namespace fiction::networks;
using namespace fiction::synthesis;

namespace
{

/** @brief Node identifier of the technology network. */
using node_t = mockturtle::node<technology_network>;

/**
 * Two gates that read the same two inputs: every drawing has exactly one crossing.
 */
technology_network single_crossing_network()
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();

    tec.create_po(tec.create_and(x1, x2));
    tec.create_po(tec.create_or(x1, x2));

    return tec;
}

}  // namespace

TEST_CASE("Level positions and fanin positions", "[crossing-cost]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto g  = tec.create_and(x2, tec.get_constant(true));
    tec.create_po(g);
    tec.create_po(x1);

    const std::vector<node_t> level{tec.get_node(x2), tec.get_node(x1)};

    const auto pos = synthesis::detail::level_positions(tec, level);

    CHECK(pos[tec.get_node(x2)] == 0);
    CHECK(pos[tec.get_node(x1)] == 1);
    CHECK(pos[tec.get_node(g)] == std::numeric_limits<std::size_t>::max());

    // the constant fanin lies outside the level and is skipped
    CHECK(synthesis::detail::fanin_positions(tec, tec.get_node(g), pos) == std::vector<std::size_t>{0});
}

TEST_CASE("Crossing minimization removes avoidable crossings", "[crossing-cost]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();

    // the gates read the inputs in the opposite order of the inputs
    tec.create_po(tec.create_not(x2));
    tec.create_po(tec.create_buf(x1));

    const auto ranked = rank(tec);

    const auto                               upper = ranked.get_ranks(1);
    std::vector<node_t>                      lower{ranked.get_node(x1), ranked.get_node(x2)};
    const crossing_gate_planarization_params cg_ps{};

    SECTION("a crossing minimization reorders the lower level")
    {
        synthesis::detail::minimize_crossings(ranked, upper, lower, 1);

        CHECK(lower == std::vector<node_t>{ranked.get_node(x2), ranked.get_node(x1)});

        CHECK(synthesis::detail::crossing_cost(ranked, upper, lower, 1, cg_ps).num_crossings == 0);
    }
    SECTION("the cost of the reordered level is zero")
    {
        const auto result = synthesis::detail::crossing_cost(ranked, upper, lower, 0, cg_ps);

        CHECK(result.num_crossings == 0);
        CHECK(result.cost == 0);
    }
}

TEST_CASE("Unavoidable crossings are charged one crossing gate each plus the padding of the other edges",
          "[crossing-cost]")
{
    const auto [xor_gates, expected_cost] = GENERATE(table<bool, uint64_t>({{false, 87}, {true, 18}}));
    CAPTURE(xor_gates, expected_cost);

    const auto ranked = rank(single_crossing_network());

    const auto                upper = ranked.get_ranks(1);
    const std::vector<node_t> lower = ranked.get_ranks(0);

    const crossing_gate_planarization_params cg_ps{.xor_gates = xor_gates};

    auto lower_copy = lower;

    const auto result = synthesis::detail::crossing_cost(ranked, upper, lower_copy, 32, cg_ps);

    // One crossing gate and two padded edges: 59 + 2*14 nodes, or 10 + 2*4 nodes with XORs.
    CHECK(result.num_crossings == 1);
    CHECK(result.cost == expected_cost);
}

TEST_CASE("Levels with more crossings than allowed are not costed", "[crossing-cost]")
{
    const auto [limit, expected_cost] =
        GENERATE(table<uint32_t, uint64_t>({{0, std::numeric_limits<uint64_t>::max()}, {1, 18}}));
    CAPTURE(limit, expected_cost);

    const auto ranked = rank(single_crossing_network());

    const auto          upper = ranked.get_ranks(1);
    std::vector<node_t> lower = ranked.get_ranks(0);

    const crossing_gate_planarization_params cg_ps{.xor_gates = true, .max_crossings_per_level = limit};

    const auto result = synthesis::detail::crossing_cost(ranked, upper, lower, 32, cg_ps);

    CHECK(result.cost == expected_cost);
    CHECK(result.num_crossings == 1);
}
