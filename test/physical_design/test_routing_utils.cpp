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
 * @brief Tests for `fiction/physical_design/routing_utils.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include "utils/allocation_failure.hpp"
#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/physical_design/routing_utils.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <algorithm>
#include <new>
#include <stdexcept>
#include <vector>

using namespace fiction;
using namespace fiction::test;
using namespace fiction::physical_design;

/** @brief Checks objective coordinates and logical input indices. @tparam Lyt Layout type.
 * @param objectives Extracted objectives. @param expected_objectives Expected connections.
 */
template <typename Lyt>
void check_containing_objectives(const std::vector<routing_objective<Lyt>>& objectives,
                                 const std::vector<routing_objective<Lyt>>& expected_objectives)
{
    CHECK(objectives.size() == expected_objectives.size());

    std::ranges::for_each(objectives, [&expected_objectives](const auto& obj)
                          { CHECK(std::ranges::find(expected_objectives, obj) != expected_objectives.cend()); });
}

TEST_CASE("Extract routing objectives", "[routing-utils]")
{
    SECTION("Simple wire connection")
    {
        const auto layout     = blueprints::straight_wire_gate_layout<cart_gate_clk_lyt>();
        const auto objectives = extract_routing_objectives(layout);

        check_containing_objectives(objectives, {{.source = {0, 1}, .target = {2, 1}}});
    }
    SECTION("Two paths wire connections")
    {
        const auto layout     = blueprints::unbalanced_and_layout<cart_gate_clk_lyt>();
        const auto objectives = extract_routing_objectives(layout);

        check_containing_objectives(objectives, {{.source = {0, 2}, .target = {2, 0}},
                                                 {.source = {1, 0}, .target = {2, 0}, .input_index = 1},
                                                 {.source = {2, 0}, .target = {3, 0}}});
    }
    SECTION("Three paths wire connections")
    {
        const auto layout     = blueprints::three_wire_paths_gate_layout<cart_gate_clk_lyt>();
        const auto objectives = extract_routing_objectives(layout);

        check_containing_objectives(objectives, {{.source = {0, 0}, .target = {4, 0}},
                                                 {.source = {0, 2}, .target = {4, 2}},
                                                 {.source = {0, 4}, .target = {4, 4}}});
    }
    SECTION("Direct gate connections")
    {
        const auto layout     = blueprints::xor_maj_gate_layout<cart_gate_clk_lyt>();
        const auto objectives = extract_routing_objectives(layout);

        check_containing_objectives(objectives, {{.source = {1, 1}, .target = {2, 1}},
                                                 {.source = {2, 0}, .target = {2, 1}, .input_index = 1},
                                                 {.source = {3, 1}, .target = {2, 1}, .input_index = 2},
                                                 {.source = {1, 1}, .target = {1, 0}},
                                                 {.source = {2, 0}, .target = {1, 0}, .input_index = 1},
                                                 {.source = {2, 1}, .target = {2, 2}},
                                                 {.source = {1, 0}, .target = {0, 0}}});
    }
    SECTION("Two incoming gate wires")
    {
        const auto layout     = blueprints::use_and_gate_layout<cart_gate_clk_lyt>();
        const auto objectives = extract_routing_objectives(layout);

        check_containing_objectives(objectives, {{.source = {0, 1}, .target = {1, 2}},
                                                 {.source = {3, 3}, .target = {1, 2}, .input_index = 1},
                                                 {.source = {1, 2}, .target = {3, 2}}});
    }
}

/** @brief Checks a retained tile after clearing its connections. @tparam Lyt Layout type.
 * @param lyt Layout. @param t Retained tile.
 */
template <typename Lyt>
void check_non_empty_tile(const Lyt& lyt, const tile<Lyt>& t)
{
    CHECK(!lyt.is_empty_tile(t));
    CHECK(lyt.has_no_incoming_signal(t));
    CHECK(lyt.has_no_outgoing_signal(t));
}

TEST_CASE("Clear routing", "[routing-utils]")
{
    SECTION("Simple wire connection")
    {
        auto layout = blueprints::straight_wire_gate_layout<cart_gate_clk_lyt>();

        clear_routing(layout);

        CHECK(layout.is_empty_tile({1, 1}));
        check_non_empty_tile(layout, {0, 1});
        check_non_empty_tile(layout, {2, 1});
    }
    SECTION("Direct gate connections")
    {
        auto layout = blueprints::xor_maj_gate_layout<cart_gate_clk_lyt>();

        clear_routing(layout);

        check_non_empty_tile(layout, {1, 1});
        check_non_empty_tile(layout, {2, 0});
        check_non_empty_tile(layout, {3, 1});
        check_non_empty_tile(layout, {2, 1});
        check_non_empty_tile(layout, {1, 0});
        check_non_empty_tile(layout, {2, 2});
        check_non_empty_tile(layout, {0, 0});
    }
    SECTION("Crossings")
    {
        auto layout = blueprints::crossing_layout<cart_gate_clk_lyt>();

        clear_routing(layout);

        check_non_empty_tile(layout, {1, 0});
        check_non_empty_tile(layout, {0, 1});
        check_non_empty_tile(layout, {2, 0});
        check_non_empty_tile(layout, {0, 2});
        check_non_empty_tile(layout, {1, 1});
        check_non_empty_tile(layout, {2, 2});
        check_non_empty_tile(layout, {3, 1});
        check_non_empty_tile(layout, {3, 2});

        CHECK(layout.is_empty_tile({2, 1}));
        CHECK(layout.is_empty_tile({1, 2}));
        CHECK(layout.is_empty_tile({2, 1, 1}));
    }
    SECTION("Fan-outs")
    {
        auto layout = blueprints::fanout_layout<cart_gate_clk_lyt>();

        clear_routing(layout);

        check_non_empty_tile(layout, {0, 1});
        check_non_empty_tile(layout, {1, 1});
        check_non_empty_tile(layout, {2, 1});
        check_non_empty_tile(layout, {1, 0});
        check_non_empty_tile(layout, {2, 0});
        check_non_empty_tile(layout, {1, 2});

        CHECK(layout.is_empty_tile({2, 2}));
    }
}

TEST_CASE("Routing preserves duplicate destination ports and retained identities", "[routing-ports]")
{
    cart_gate_clk_lyt layout{{6, 4, 2}, layouts::clocking::twoddwave()};
    const auto        a          = layout.create_pi("a", {0, 0});
    const auto        gate       = layout.create_lt(a, a, {4, 2});
    const auto        po         = layout.create_po(gate, "f", {5, 2});
    const auto        objectives = extract_routing_objectives(layout);
    CHECK(std::ranges::find(objectives, routing_objective<cart_gate_clk_lyt>{{0, 0}, {4, 2}, 0}) != objectives.end());
    CHECK(std::ranges::find(objectives, routing_objective<cart_gate_clk_lyt>{{0, 0}, {4, 2}, 1}) != objectives.end());
    CHECK((routing_objective<cart_gate_clk_lyt>{{0, 0}, {4, 2}, 0} !=
           routing_objective<cart_gate_clk_lyt>{{0, 0}, {4, 2}, 1}));
    clear_routing(layout);
    CHECK(layout.contains(a));
    CHECK(layout.contains(gate));
    CHECK(layout.contains(po));
    CHECK_FALSE(layout.source({gate, 0}));
    CHECK_FALSE(layout.source({gate, 1}));
    const layout_coordinate_path<cart_gate_clk_lyt> second{{0, 0}, {1, 0}, {4, 2}};
    route_path(layout, second, {gate, 1});
    CHECK_FALSE(layout.source({gate, 0}));
    /** @brief Routed source connected to the second input port. */
    const auto second_source = layout.source({gate, 1});
    REQUIRE(second_source.has_value());
    if (!second_source.has_value())
    {
        return;
    }
    /** @brief Retained identity of the second routed wire. */
    const auto second_wire = *second_source;
    CHECK(layout.source({second_wire, 0}) == a);
    const layout_coordinate_path<cart_gate_clk_lyt> first{{0, 0}, {0, 1}, {4, 2}};
    route_path(layout, first, {gate, 0});
    CHECK(layout.source({gate, 1}) == second_wire);
    CHECK(layout.is_lt(gate));
    layout.move_object(gate, {4, 3});
    CHECK(layout.source({gate, 1}) == second_wire);
}

TEST_CASE("Invalid routing endpoints reject before creating wires", "[routing-ports]")
{
    cart_gate_clk_lyt                               layout{{5, 3, 2}};
    const auto                                      a    = layout.create_pi("a", {0, 0});
    const auto                                      b    = layout.create_pi("b", {0, 1});
    const auto                                      gate = layout.create_lt(a, b, {4, 0});
    const layout_coordinate_path<cart_gate_clk_lyt> path{{0, 0}, {1, 0}, {4, 0}};
    CHECK_THROWS_AS(route_path(layout, path, {gate, 2}), std::out_of_range);
    CHECK(layout.size() == 3);
    CHECK(layout.source({gate, 0}) == a);
    CHECK(layout.source({gate, 1}) == b);
    const layout_coordinate_path<cart_gate_clk_lyt> mismatch{{0, 0}, {1, 0}, {4, 1}};
    CHECK_THROWS_AS(route_path(layout, mismatch, {gate, 0}), std::invalid_argument);
    CHECK_FALSE(layout.find_object({1, 0}));
    const auto blocker = layout.create_buf({1, 0});
    layout.create_buf({1, 0, 1});
    CHECK_THROWS_AS(route_path(layout, path, {gate, 0}), std::invalid_argument);
    CHECK(layout.find_object({1, 0}) == blocker);
    CHECK(layout.source({gate, 0}) == a);
}

TEST_CASE("Rerouting distinct sources preserves noncommutative input order", "[routing-ports]")
{
    cart_gate_clk_lyt layout{{5, 3, 2}, layouts::clocking::twoddwave()};
    const auto        a    = layout.create_pi("a", {0, 0});
    const auto        b    = layout.create_pi("b", {0, 1});
    const auto        gate = layout.create_lt(a, b, {4, 1});
    clear_routing(layout);
    const layout_coordinate_path<cart_gate_clk_lyt> second{{0, 1}, {1, 1}, {4, 1}};
    const layout_coordinate_path<cart_gate_clk_lyt> first{{0, 0}, {1, 0}, {4, 1}};
    route_path(layout, second, {gate, 1});
    route_path(layout, first, {gate, 0});
    /** @brief Routed source connected to the first input port. */
    const auto first_source = layout.source({gate, 0});
    /** @brief Routed source connected to the second input port. */
    const auto second_source = layout.source({gate, 1});
    CHECK((first_source.has_value() && layout.source({*first_source, 0}) == a));
    CHECK((second_source.has_value() && layout.source({*second_source, 0}) == b));
    CHECK(layout.is_lt(gate));
    const auto objectives = extract_routing_objectives(layout);
    CHECK(std::ranges::find(objectives, routing_objective<cart_gate_clk_lyt>{{0, 0}, {4, 1}, 0}) != objectives.end());
    CHECK(std::ranges::find(objectives, routing_objective<cart_gate_clk_lyt>{{0, 1}, {4, 1}, 1}) != objectives.end());
}

TEST_CASE("Routing paths propagate allocation failure", "[routing-utils]")
{
    require_allocation_failure_support();
    layout_coordinate_path<cart_gate_clk_lyt>                  path{};
    path_collection<layout_coordinate_path<cart_gate_clk_lyt>> collection{};
    path_set<layout_coordinate_path<cart_gate_clk_lyt>>        paths{};
    /** @brief Checks that a failed path allocation reaches the caller. */
    const auto fail = [&](auto&& append)
    {
        bool threw{};
        allocation_budget = 0;
        try
        {
            append();
        }
        catch (const std::bad_alloc&)
        {
            threw = true;
        }
        catch (...)
        {
            allocation_budget.reset();
            throw;
        }
        allocation_budget.reset();
        CHECK(threw);
    };
    fail([&] { path.append({0, 0}); });
    CHECK(path.empty());
    path.append({0, 0});
    fail([&] { collection.add(path); });
    CHECK(collection.empty());
    fail([&] { paths.add(path); });
    CHECK(paths.empty());
    collection.add(path);
    paths.add(path);
    CHECK(collection.front() == path);
    CHECK(paths.contains(path));
}

TEST_CASE("Coordinate paths expose construction and edits", "[routing-utils]")
{
    const std::vector<tile<cart_gate_clk_lyt>> coordinates{{0, 0}, {1, 0}};
    layout_coordinate_path<cart_gate_clk_lyt>  path{coordinates.cbegin(), coordinates.cend()};
    path.push_back({2, 0});
    path.insert(path.begin(), {0, 1});
    path.front() = {1, 1};
    path.back()  = {3, 0};
    CHECK(path.source() == tile<cart_gate_clk_lyt>{1, 1});
    CHECK(path.target() == tile<cart_gate_clk_lyt>{3, 0});
    CHECK(path.size() == 4);
}
