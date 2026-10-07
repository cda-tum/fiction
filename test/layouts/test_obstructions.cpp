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

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/layout_base.hpp>
#include <fiction/layouts/obstructions.hpp>

using namespace fiction;
using namespace fiction::layouts;

TEST_CASE("Coordinate obstruction: Cartesian layout", "[obstruction-layout]")
{
    using layout = cartesian_layout;

    const layout lyt{{4, 4}};

    obstructions obstr_lyt{};

    lyt.foreach_coordinate([&obstr_lyt](const auto& c) { CHECK(!obstr_lyt.is_obstructed_coordinate(c)); });

    // add artificial obstruction
    obstr_lyt.obstruct_coordinate({0, 0});
    obstr_lyt.obstruct_coordinate({1, 0});
    obstr_lyt.obstruct_coordinate({2, 0});
    obstr_lyt.obstruct_coordinate({3, 0});
    obstr_lyt.obstruct_coordinate({4, 0});

    CHECK(obstr_lyt.is_obstructed_coordinate({0, 0}));
    CHECK(obstr_lyt.is_obstructed_coordinate({1, 0}));
    CHECK(obstr_lyt.is_obstructed_coordinate({2, 0}));
    CHECK(obstr_lyt.is_obstructed_coordinate({3, 0}));
    CHECK(obstr_lyt.is_obstructed_coordinate({4, 0}));

    // remove some artificial obstructions
    obstr_lyt.clear_obstructed_coordinate({0, 0});
    obstr_lyt.clear_obstructed_coordinate({1, 0});
    obstr_lyt.clear_obstructed_coordinate({2, 0});

    CHECK(!obstr_lyt.is_obstructed_coordinate({0, 0}));
    CHECK(!obstr_lyt.is_obstructed_coordinate({1, 0}));
    CHECK(!obstr_lyt.is_obstructed_coordinate({2, 0}));
    CHECK(obstr_lyt.is_obstructed_coordinate({3, 0}));
    CHECK(obstr_lyt.is_obstructed_coordinate({4, 0}));

    // remove all obstructions
    obstr_lyt.clear_obstructed_coordinates();

    CHECK(!obstr_lyt.is_obstructed_coordinate({0, 0}));
    CHECK(!obstr_lyt.is_obstructed_coordinate({1, 0}));
    CHECK(!obstr_lyt.is_obstructed_coordinate({2, 0}));
    CHECK(!obstr_lyt.is_obstructed_coordinate({3, 0}));
    CHECK(!obstr_lyt.is_obstructed_coordinate({4, 0}));
}

TEST_CASE("Coordinate obstruction: Gate-level layout", "[obstruction-layout]")
{
    using layout = gate_level_layout<cartesian_layout>;

    const auto lyt = blueprints::xor_maj_gate_layout<layout>();

    auto obstr_lyt = lyt.clone();

    CHECK(obstr_lyt.is_obstructed_coordinate({1, 1}));
    CHECK(obstr_lyt.is_obstructed_coordinate({2, 0}));
    CHECK(obstr_lyt.is_obstructed_coordinate({3, 1}));
    CHECK(obstr_lyt.is_obstructed_coordinate({2, 1}));
    CHECK(obstr_lyt.is_obstructed_coordinate({1, 0}));
    CHECK(obstr_lyt.is_obstructed_coordinate({2, 2}));
    CHECK(obstr_lyt.is_obstructed_coordinate({0, 0}));

    CHECK(!obstr_lyt.is_obstructed_coordinate({0, 1}));
    CHECK(!obstr_lyt.is_obstructed_coordinate({0, 2}));
    CHECK(!obstr_lyt.is_obstructed_coordinate({1, 2}));
    CHECK(!obstr_lyt.is_obstructed_coordinate({3, 0}));
    CHECK(!obstr_lyt.is_obstructed_coordinate({3, 2}));

    // add artificial obstructions
    obstr_lyt.obstruct_coordinate({0, 1});
    obstr_lyt.obstruct_coordinate({1, 2});
    obstr_lyt.obstruct_coordinate({3, 2});

    CHECK(obstr_lyt.is_obstructed_coordinate({0, 1}));
    CHECK(!obstr_lyt.is_obstructed_coordinate({0, 2}));
    CHECK(obstr_lyt.is_obstructed_coordinate({1, 2}));
    CHECK(!obstr_lyt.is_obstructed_coordinate({3, 0}));
    CHECK(obstr_lyt.is_obstructed_coordinate({3, 2}));

    // remove some manually added obstructions
    obstr_lyt.clear_obstructed_coordinate({0, 1});
    obstr_lyt.clear_obstructed_coordinate({1, 2});

    CHECK(!obstr_lyt.is_obstructed_coordinate({0, 1}));
    CHECK(!obstr_lyt.is_obstructed_coordinate({1, 2}));
    CHECK(obstr_lyt.is_obstructed_coordinate({3, 2}));

    // removing an obstruction that was not added manually should not change anything
    obstr_lyt.clear_obstructed_coordinate({1, 1});
    obstr_lyt.clear_obstructed_coordinate({2, 0});
    obstr_lyt.clear_obstructed_coordinate({3, 1});

    CHECK(obstr_lyt.is_obstructed_coordinate({1, 1}));
    CHECK(obstr_lyt.is_obstructed_coordinate({2, 0}));
    CHECK(obstr_lyt.is_obstructed_coordinate({3, 1}));

    // remove all obstructions
    obstr_lyt.clear_obstructed_coordinates();

    CHECK(!obstr_lyt.is_obstructed_coordinate({0, 1}));
    CHECK(!obstr_lyt.is_obstructed_coordinate({0, 2}));
    CHECK(!obstr_lyt.is_obstructed_coordinate({1, 2}));
    CHECK(!obstr_lyt.is_obstructed_coordinate({3, 0}));
    CHECK(!obstr_lyt.is_obstructed_coordinate({3, 2}));
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
                                                { CHECK(!obstr_lyt.is_obstructed_connection(c, ac)); });
            });

        // add artificial obstruction
        obstr_lyt.obstruct_connection({0, 0}, {0, 1});
        obstr_lyt.obstruct_connection({2, 2}, {2, 3});
        obstr_lyt.obstruct_connection({2, 4}, {4, 0});

        CHECK(obstr_lyt.is_obstructed_connection({0, 0}, {0, 1}));
        CHECK(!obstr_lyt.is_obstructed_connection({1, 0}, {0, 0}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 2}, {2, 3}));
        CHECK(!obstr_lyt.is_obstructed_connection({2, 3}, {2, 2}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 4}, {4, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({4, 0}, {2, 4}));

        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {0, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {1, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({3, 3}, {2, 2}));
        CHECK(!obstr_lyt.is_obstructed_connection({3, 3}, {2, 3}));

        // remove some artificial obstructions
        obstr_lyt.clear_obstructed_connection({0, 0}, {0, 1});

        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {0, 1}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 2}, {2, 3}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 4}, {4, 0}));

        // remove all artificial obstructions
        obstr_lyt.clear_obstructed_connections();

        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {0, 1}));
        CHECK(!obstr_lyt.is_obstructed_connection({1, 0}, {0, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({2, 2}, {2, 3}));
        CHECK(!obstr_lyt.is_obstructed_connection({2, 3}, {2, 2}));
        CHECK(!obstr_lyt.is_obstructed_connection({2, 4}, {4, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({4, 0}, {2, 4}));

        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {0, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {1, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({3, 3}, {2, 2}));
        CHECK(!obstr_lyt.is_obstructed_connection({3, 3}, {2, 3}));
    }
    SECTION("Gate-level layout")
    {
        using layout = gate_level_layout<cartesian_layout>;

        const auto lyt = blueprints::xor_maj_gate_layout<layout>();

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
        CHECK(!obstr_lyt.is_obstructed_connection({2, 3}, {2, 2}));
        CHECK(obstr_lyt.is_obstructed_connection({2, 4}, {4, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({4, 0}, {2, 4}));

        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {0, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {1, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({3, 3}, {2, 2}));
        CHECK(!obstr_lyt.is_obstructed_connection({3, 3}, {2, 3}));

        // remove some artificial obstructions
        obstr_lyt.clear_obstructed_connection({0, 0}, {0, 1});

        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {0, 1}));
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

        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {0, 1}));
        CHECK(!obstr_lyt.is_obstructed_connection({2, 2}, {2, 3}));
        CHECK(!obstr_lyt.is_obstructed_connection({2, 3}, {2, 2}));
        CHECK(!obstr_lyt.is_obstructed_connection({2, 4}, {4, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({4, 0}, {2, 4}));

        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {0, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({0, 0}, {1, 0}));
        CHECK(!obstr_lyt.is_obstructed_connection({3, 3}, {2, 2}));
        CHECK(!obstr_lyt.is_obstructed_connection({3, 3}, {2, 3}));
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
    uint32_t positions{};
    constraints.foreach_obstructed_coordinate(
        [&constraints, &positions](const auto& c)
        {
            CHECK(constraints.is_obstructed_coordinate(c));
            ++positions;
        });
    uint32_t connections{};
    constraints.foreach_obstructed_connection(
        [&constraints, &connections](const auto& source, const auto& target)
        {
            CHECK(constraints.is_obstructed_connection(source, target));
            ++connections;
        });
    CHECK(positions == 2);
    CHECK(connections == 1);

    gate_level_layout<cartesian_layout> layout{{2, 1, 1}, clocking::twoddwave()};
    const auto                          input = layout.create_pi("a", {0, 0});
    layout.create_po(input, "f", {1, 0});
    positions   = 0;
    connections = 0;
    layout.foreach_obstructed_coordinate([&positions](const auto&) { ++positions; });
    layout.foreach_obstructed_connection([&connections](const auto&, const auto&) { ++connections; });
    CHECK(positions == 0);
    CHECK(connections == 0);
    layout.obstruct_coordinate({0, 0});
    layout.obstruct_connection({0, 0}, {1, 0});
    layout.foreach_obstructed_coordinate(
        [&positions](const auto& c)
        {
            CHECK(c == layout_base::coordinate{0, 0});
            ++positions;
        });
    layout.foreach_obstructed_connection(
        [&connections](const auto& source, const auto& target)
        {
            CHECK(source == layout_base::coordinate{0, 0});
            CHECK(target == layout_base::coordinate{1, 0});
            ++connections;
        });
    CHECK(positions == 1);
    CHECK(connections == 1);
}
