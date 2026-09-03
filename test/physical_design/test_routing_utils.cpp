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

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/physical_design/routing_utils.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <algorithm>
#include <vector>

using namespace fiction;
using namespace fiction::physical_design;

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
                                                 {.source = {1, 0}, .target = {2, 0}},
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
                                                 {.source = {2, 0}, .target = {2, 1}},
                                                 {.source = {3, 1}, .target = {2, 1}},
                                                 {.source = {1, 1}, .target = {1, 0}},
                                                 {.source = {2, 0}, .target = {1, 0}},
                                                 {.source = {2, 1}, .target = {2, 2}},
                                                 {.source = {1, 0}, .target = {0, 0}}});
    }
    SECTION("Two incoming gate wires")
    {
        const auto layout     = blueprints::use_and_gate_layout<cart_gate_clk_lyt>();
        const auto objectives = extract_routing_objectives(layout);

        check_containing_objectives(objectives, {{.source = {0, 1}, .target = {1, 2}},
                                                 {.source = {3, 3}, .target = {1, 2}},
                                                 {.source = {1, 2}, .target = {3, 2}}});
    }
}

template <typename Lyt>
void check_non_empty_tile(const Lyt& lyt, const tile<Lyt>& t) noexcept
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
