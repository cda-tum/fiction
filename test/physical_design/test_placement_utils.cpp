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
 * @brief Tests for ordered layout input ports during placement.
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/physical_design/placement_utils.hpp>

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::networks;
using namespace fiction::physical_design;

TEST_CASE("Placement reduces a constant input without changing variable order", "[placement-utils]")
{
    technology_network         ntk{};
    const auto                 a = ntk.create_pi();
    const auto                 b = ntk.create_pi();
    kitty::dynamic_truth_table function{3};
    kitty::create_from_expression(function, "(a!c)");
    const auto gate = ntk.create_node({a, ntk.get_constant(true), b}, function);

    gate_level_layout<cartesian_layout> lyt{{3, 3}};
    const auto                          left   = lyt.create_pi("a", {0, 0});
    const auto                          right  = lyt.create_pi("b", {1, 0});
    const auto                          placed = place(lyt, {1, 1}, ntk, ntk.get_node(gate), left, right, true);
    CHECK(lyt.input_count(placed) == 2);
    CHECK(lyt.source({placed, 0}) == left);
    CHECK(lyt.source({placed, 1}) == right);
    kitty::dynamic_truth_table expected{2};
    kitty::create_from_expression(expected, "(a!b)");
    CHECK(lyt.object_function(placed) == expected);
}
