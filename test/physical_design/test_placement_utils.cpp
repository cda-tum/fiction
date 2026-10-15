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
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/networks/name_utils.hpp>
#include <fiction/physical_design/placement_utils.hpp>
#include <fiction/types.hpp>

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/topo_view.hpp>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::networks;
using namespace fiction::physical_design;

TEST_CASE("Placement reduces a constant input without changing variable order", "[placement-utils]")
{
    tec_nt                     ntk{};
    const auto                 a = ntk.create_pi();
    const auto                 b = ntk.create_pi();
    kitty::dynamic_truth_table function{3};
    kitty::create_from_expression(function, "(a!c)");
    const auto gate = ntk.create_node({a, ntk.get_constant(true), b}, function);

    gate_layout<cartesian_layout> lyt{{3, 3}};
    const auto                    left   = lyt.create_pi("a", {0, 0});
    const auto                    right  = lyt.create_pi("b", {1, 0});
    const auto                    placed = place(lyt, {1, 1}, ntk, ntk.get_node(gate), left, right, true);
    CHECK(lyt.input_count(placed) == 2);
    CHECK(lyt.source({placed, 0}) == left);
    CHECK(lyt.source({placed, 1}) == right);
    kitty::dynamic_truth_table expected{2};
    kitty::create_from_expression(expected, "(a!b)");
    CHECK(lyt.object_function(placed) == expected);
}

TEST_CASE("Placement maps retain named-view node IDs", "[placement-utils]")
{
    tec_nt ntk{};
    ntk.set_network_name("named_spec");
    const auto a = ntk.create_pi("left");
    const auto b = ntk.create_pi("right");
    ntk.create_and(a, b);
    const auto gate = ntk.create_lt(a, b);
    ntk.create_po(gate, "result");
    const mockturtle::topo_view view{mockturtle::fanout_view{ntk}};
    const auto&                 named = static_cast<const tec_nt&>(view);

    gate_layout<cartesian_layout>                  lyt{{3, 3}};
    const auto                                     left  = lyt.create_pi("left", {0, 0});
    const auto                                     right = lyt.create_pi("right", {1, 0});
    mockturtle::node_map<layout_object_id, tec_nt> node2pos{named};
    node2pos[a]       = left;
    node2pos[b]       = right;
    const auto placed = place(lyt, {1, 1}, named, named.get_node(gate), node2pos);
    CHECK(lyt.source({placed, 0}) == left);
    CHECK(lyt.source({placed, 1}) == right);
    CHECK(lyt.object_function(placed) == named.node_function(named.get_node(gate)));

    mockturtle::node_map<branching_signal_container<>, tec_nt> branches{named};
    branches[a].update_branch(named.get_node(gate), left);
    branches[b].update_branch(named.get_node(gate), right);
    const auto branching = place(lyt, {2, 1}, named, named.get_node(gate), branches);
    branches[gate].update_branch(named.get_node(gate), branching);
    lyt.create_po(branching, "result", {2, 2});
    CHECK_NOTHROW(restore_names(named, lyt, branches));
    CHECK(lyt.source({branching, 0}) == left);
    CHECK(lyt.source({branching, 1}) == right);
    CHECK(lyt.object_function(branching) == named.node_function(named.get_node(gate)));
    CHECK(lyt.get_layout_name() == "named_spec");
    CHECK(lyt.get_name(left) == "left");
    CHECK(lyt.get_name(right) == "right");
    CHECK(lyt.get_output_name(0) == "result");
}

TEST_CASE("Placement reduces an explicit third constant", "[placement-utils]")
{
    tec_nt                     ntk{};
    const auto                 a = ntk.create_pi();
    const auto                 b = ntk.create_pi();
    const auto                 c = ntk.create_pi();
    kitty::dynamic_truth_table function{3};
    kitty::create_from_expression(function, "(a!c)");
    const auto gate = ntk.create_node({a, b, c}, function);

    gate_layout<cartesian_layout> lyt{{3, 3}};
    const auto                    left   = lyt.create_pi("a", {0, 0});
    const auto                    right  = lyt.create_pi("b", {1, 0});
    const auto                    placed = place(lyt, {1, 1}, ntk, ntk.get_node(gate), left, right, false);
    CHECK(lyt.source({placed, 0}) == left);
    CHECK(lyt.source({placed, 1}) == right);
    kitty::dynamic_truth_table expected{2};
    kitty::create_from_expression(expected, "a");
    CHECK(lyt.object_function(placed) == expected);
}
