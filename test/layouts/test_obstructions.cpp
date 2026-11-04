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
 * @brief Tests for `fiction/layouts/obstructions.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <fiction/layouts/obstructions.hpp>

// Test dependencies follow the defining header to expose missing direct includes.

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/layouts/layout_base.hpp>

#include <array>
#include <cstdint>
#include <string_view>
#include <utility>
#include <vector>

using namespace fiction;
using namespace fiction::layouts;

TEST_CASE("Coordinate obstruction: Cartesian layout", "[obstruction-layout]")
{
    using layout = cartesian_layout;

    const layout lyt{{4, 4}};

    obstructions obstr_lyt{};

    lyt.foreach_coordinate([&obstr_lyt](const auto& c) { CHECK_FALSE(obstr_lyt.is_obstructed_coordinate(c)); });

    // The last coordinate lies outside the layout: explicit obstructions have no layout bounds.
    for (int32_t x = 0; x < 5; ++x)
    {
        obstr_lyt.obstruct_coordinate({x, 0});
    }
    for (int32_t x = 0; x < 5; ++x)
    {
        CAPTURE(x);
        CHECK(obstr_lyt.is_obstructed_coordinate({x, 0}));
    }

    for (int32_t x = 0; x < 3; ++x)
    {
        obstr_lyt.clear_obstructed_coordinate({x, 0});
    }
    for (int32_t x = 0; x < 5; ++x)
    {
        CAPTURE(x);
        CHECK(obstr_lyt.is_obstructed_coordinate({x, 0}) == (x >= 3));
    }

    obstr_lyt.clear_obstructed_coordinates();
    for (int32_t x = 0; x < 5; ++x)
    {
        CAPTURE(x);
        CHECK_FALSE(obstr_lyt.is_obstructed_coordinate({x, 0}));
    }
}

TEST_CASE("Coordinate obstruction: Gate-level layout", "[obstruction-layout]")
{
    auto obstr_lyt = blueprints::xor_maj_gate_layout<cartesian_gate_layout>();

    /** @brief Expected obstruction states in y-major order for the blueprint's 4-by-3 frame. */
    using state = std::array<std::array<bool, 4>, 3>;
    const state occupied{{{{true, true, true, false}}, {{false, true, true, true}}, {{false, false, true, false}}}};
    const state added{{{{true, true, true, false}}, {{true, true, true, true}}, {{false, true, true, true}}}};
    const state one_manual{{{{true, true, true, false}}, {{false, true, true, true}}, {{false, false, true, true}}}};

    /** @brief Check every coordinate against an independent expected state. */
    const auto check_state = [&obstr_lyt](const std::string_view stage, const state& expected)
    {
        for (int32_t y = 0; y < 3; ++y)
        {
            for (int32_t x = 0; x < 4; ++x)
            {
                CAPTURE(stage, x, y);
                CHECK(obstr_lyt.is_obstructed_coordinate({x, y}) == expected[y][x]);
            }
        }
    };

    check_state("occupied", occupied);

    // add artificial obstructions
    obstr_lyt.obstruct_coordinate({0, 1});
    obstr_lyt.obstruct_coordinate({1, 2});
    obstr_lyt.obstruct_coordinate({3, 2});

    check_state("added", added);

    // remove some manually added obstructions
    obstr_lyt.clear_obstructed_coordinate({0, 1});
    obstr_lyt.clear_obstructed_coordinate({1, 2});

    check_state("partially cleared", one_manual);

    // removing an obstruction that was not added manually should not change anything
    obstr_lyt.clear_obstructed_coordinate({1, 1});
    obstr_lyt.clear_obstructed_coordinate({2, 0});
    obstr_lyt.clear_obstructed_coordinate({3, 1});

    check_state("occupied entries cleared", one_manual);

    // remove all obstructions
    obstr_lyt.clear_obstructed_coordinates();

    check_state("all manual entries cleared", occupied);
}

TEST_CASE("Connection obstruction", "[obstruction-layout]")
{
    SECTION("Cartesian layout")
    {
        using layout = cartesian_layout;

        const layout lyt{{4, 4}};

        obstructions obstr_lyt{};

        lyt.foreach_coordinate(
            [&obstr_lyt, &lyt](const auto& c)
            {
                lyt.foreach_adjacent_coordinate(c, [&obstr_lyt, &c](const auto& ac)
                                                { CHECK_FALSE(obstr_lyt.is_obstructed_connection(c, ac)); });
            });

        // add artificial obstruction
        obstr_lyt.obstruct_connection({0, 0}, {0, 1});
        obstr_lyt.obstruct_connection({2, 2}, {2, 3});
        obstr_lyt.obstruct_connection({2, 4}, {4, 0});

        CHECK(obstr_lyt.is_obstructed_connection({0, 0}, {0, 1}));
        CHECK_FALSE(obstr_lyt.is_obstructed_connection({1, 0}, {0, 0}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 2}, {2, 3}));
        CHECK_FALSE(obstr_lyt.is_obstructed_connection({2, 3}, {2, 2}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 4}, {4, 0}));
        CHECK_FALSE(obstr_lyt.is_obstructed_connection({4, 0}, {2, 4}));

        CHECK_FALSE(obstr_lyt.is_obstructed_connection({0, 0}, {0, 0}));
        CHECK_FALSE(obstr_lyt.is_obstructed_connection({0, 0}, {1, 0}));
        CHECK_FALSE(obstr_lyt.is_obstructed_connection({3, 3}, {2, 2}));
        CHECK_FALSE(obstr_lyt.is_obstructed_connection({3, 3}, {2, 3}));

        // remove some artificial obstructions
        obstr_lyt.clear_obstructed_connection({0, 0}, {0, 1});

        CHECK_FALSE(obstr_lyt.is_obstructed_connection({0, 0}, {0, 1}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 2}, {2, 3}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 4}, {4, 0}));

        // remove all artificial obstructions
        obstr_lyt.clear_obstructed_connections();

        CHECK_FALSE(obstr_lyt.is_obstructed_connection({0, 0}, {0, 1}));
        CHECK_FALSE(obstr_lyt.is_obstructed_connection({1, 0}, {0, 0}));
        CHECK_FALSE(obstr_lyt.is_obstructed_connection({2, 2}, {2, 3}));
        CHECK_FALSE(obstr_lyt.is_obstructed_connection({2, 3}, {2, 2}));
        CHECK_FALSE(obstr_lyt.is_obstructed_connection({2, 4}, {4, 0}));
        CHECK_FALSE(obstr_lyt.is_obstructed_connection({4, 0}, {2, 4}));

        CHECK_FALSE(obstr_lyt.is_obstructed_connection({0, 0}, {0, 0}));
        CHECK_FALSE(obstr_lyt.is_obstructed_connection({0, 0}, {1, 0}));
        CHECK_FALSE(obstr_lyt.is_obstructed_connection({3, 3}, {2, 2}));
        CHECK_FALSE(obstr_lyt.is_obstructed_connection({3, 3}, {2, 3}));
    }
    SECTION("Gate-level layout")
    {
        const auto lyt = blueprints::xor_maj_gate_layout<cartesian_gate_layout>();

        auto obstr_lyt = lyt.clone();

        CHECK(obstr_lyt.is_obstructed_connection({1, 1}, {2, 1}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 0}, {2, 1}));
        CHECK(obstr_lyt.is_obstructed_connection({3, 1}, {2, 1}));

        CHECK(obstr_lyt.is_obstructed_connection({1, 1}, {1, 0}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 0}, {1, 0}));

        CHECK(obstr_lyt.is_obstructed_connection({2, 1}, {2, 2}));
        CHECK(obstr_lyt.is_obstructed_connection({1, 0}, {0, 0}));

        // add artificial obstruction
        obstr_lyt.obstruct_connection({0, 0}, {0, 1});
        obstr_lyt.obstruct_connection({2, 2}, {2, 3});
        obstr_lyt.obstruct_connection({2, 4}, {4, 0});

        CHECK(obstr_lyt.is_obstructed_connection({0, 0}, {0, 1}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 2}, {2, 3}));
        CHECK_FALSE(obstr_lyt.is_obstructed_connection({2, 3}, {2, 2}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 4}, {4, 0}));
        CHECK_FALSE(obstr_lyt.is_obstructed_connection({4, 0}, {2, 4}));

        CHECK_FALSE(obstr_lyt.is_obstructed_connection({0, 0}, {0, 0}));
        CHECK_FALSE(obstr_lyt.is_obstructed_connection({0, 0}, {1, 0}));
        CHECK_FALSE(obstr_lyt.is_obstructed_connection({3, 3}, {2, 2}));
        CHECK_FALSE(obstr_lyt.is_obstructed_connection({3, 3}, {2, 3}));

        // remove some artificial obstructions
        obstr_lyt.clear_obstructed_connection({0, 0}, {0, 1});

        CHECK_FALSE(obstr_lyt.is_obstructed_connection({0, 0}, {0, 1}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 2}, {2, 3}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 4}, {4, 0}));

        // removing an obstruction that was not added manually should not change anything
        obstr_lyt.clear_obstructed_connection({1, 1}, {2, 1});
        obstr_lyt.clear_obstructed_connection({2, 0}, {2, 1});
        obstr_lyt.clear_obstructed_connection({3, 1}, {2, 1});

        CHECK(obstr_lyt.is_obstructed_connection({1, 1}, {2, 1}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 0}, {2, 1}));
        CHECK(obstr_lyt.is_obstructed_connection({3, 1}, {2, 1}));

        // remove all artificial obstructions
        obstr_lyt.clear_obstructed_connections();

        CHECK_FALSE(obstr_lyt.is_obstructed_connection({0, 0}, {0, 1}));
        CHECK_FALSE(obstr_lyt.is_obstructed_connection({2, 2}, {2, 3}));
        CHECK_FALSE(obstr_lyt.is_obstructed_connection({2, 3}, {2, 2}));
        CHECK_FALSE(obstr_lyt.is_obstructed_connection({2, 4}, {4, 0}));
        CHECK_FALSE(obstr_lyt.is_obstructed_connection({4, 0}, {2, 4}));

        CHECK_FALSE(obstr_lyt.is_obstructed_connection({0, 0}, {0, 0}));
        CHECK_FALSE(obstr_lyt.is_obstructed_connection({0, 0}, {1, 0}));
        CHECK_FALSE(obstr_lyt.is_obstructed_connection({3, 3}, {2, 2}));
        CHECK_FALSE(obstr_lyt.is_obstructed_connection({3, 3}, {2, 3}));
    }
}

TEST_CASE("Obstruction enumeration visits only manual entries", "[obstruction-layout]")
{
    obstructions constraints{};
    constraints.obstruct_coordinate({-1, 4, 8});
    constraints.obstruct_coordinate({-1, 4, 8});
    constraints.obstruct_coordinate({0, 0});
    constraints.obstruct_connection({-1, 4, 8}, {0, 0});
    constraints.obstruct_connection({-1, 4, 8}, {0, 0});
    std::vector<layout_base::coordinate> positions{};
    constraints.foreach_obstructed_coordinate([&positions](const auto& c) { positions.push_back(c); });
    std::vector<std::pair<layout_base::coordinate, layout_base::coordinate>> connections{};
    constraints.foreach_obstructed_connection([&connections](const auto& source, const auto& target)
                                              { connections.emplace_back(source, target); });
    CHECK_THAT(positions,
               Catch::Matchers::UnorderedRangeEquals(std::vector<layout_base::coordinate>{{-1, 4, 8}, {0, 0}}));
    CHECK_THAT(connections,
               Catch::Matchers::UnorderedRangeEquals(
                   std::vector<std::pair<layout_base::coordinate, layout_base::coordinate>>{{{-1, 4, 8}, {0, 0}}}));

    gate_layout<cartesian_layout> layout{{2, 1, 1}, clocking::twoddwave()};
    const auto                    input = layout.create_pi("a", {0, 0});
    layout.create_po(input, "f", {1, 0});
    positions.clear();
    connections.clear();
    layout.foreach_obstructed_coordinate([&positions](const auto& c) { positions.push_back(c); });
    layout.foreach_obstructed_connection([&connections](const auto& source, const auto& target)
                                         { connections.emplace_back(source, target); });
    CHECK(positions.empty());
    CHECK(connections.empty());
    layout.obstruct_coordinate({0, 0});
    layout.obstruct_connection({0, 0}, {1, 0});
    layout.foreach_obstructed_coordinate([&positions](const auto& c) { positions.push_back(c); });
    layout.foreach_obstructed_connection([&connections](const auto& source, const auto& target)
                                         { connections.emplace_back(source, target); });
    CHECK_THAT(positions, Catch::Matchers::UnorderedRangeEquals(std::vector<layout_base::coordinate>{{0, 0}}));
    CHECK_THAT(connections,
               Catch::Matchers::UnorderedRangeEquals(
                   std::vector<std::pair<layout_base::coordinate, layout_base::coordinate>>{{{0, 0}, {1, 0}}}));
}
