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
 * @brief Tests for placement functions, ordered input ports, and constant reduction.
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/networks/name_utils.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/physical_design/placement_utils.hpp>
#include <fiction/types.hpp>

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/topo_view.hpp>

#include <stdexcept>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::networks;
using namespace fiction::physical_design;

TEST_CASE("Placement reduces a constant input without changing variable order", "[placement-utils]")
{
    named_technology_network   ntk{};
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
    /** @brief Gate constructor selected before each independent network fixture. */
    const auto create_comparison = GENERATE(&technology_network::create_lt, &technology_network::create_le,
                                            &technology_network::create_gt, &technology_network::create_ge);
    named_technology_network ntk{};
    ntk.set_network_name("named_spec");
    const auto a = ntk.create_pi("left");
    const auto b = ntk.create_pi("right");
    ntk.create_and(a, b);
    /** @brief Ordered comparison whose function must survive both placement maps. */
    const auto gate = (ntk.*create_comparison)(a, b);
    ntk.create_po(gate, "result");
    const mockturtle::topo_view view{mockturtle::fanout_view{ntk}};
    const auto&                 named = static_cast<const named_technology_network&>(view);

    gate_layout<cartesian_layout>                                    lyt{{3, 3}};
    const auto                                                       left  = lyt.create_pi("left", {0, 0});
    const auto                                                       right = lyt.create_pi("right", {1, 0});
    mockturtle::node_map<layout_object_id, named_technology_network> node2pos{named};
    node2pos[a]       = left;
    node2pos[b]       = right;
    const auto placed = place(lyt, {1, 1}, named, named.get_node(gate), node2pos);
    CHECK(lyt.source({placed, 0}) == left);
    CHECK(lyt.source({placed, 1}) == right);
    CHECK(lyt.object_function(placed) == named.node_function(named.get_node(gate)));

    mockturtle::node_map<branching_signal_container<>, named_technology_network> branches{named};
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
    named_technology_network   ntk{};
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

TEST_CASE("Mapped placement preserves three ordered inputs", "[placement-utils]")
{
    /** @brief Named specification with a three-input function. */
    named_technology_network ntk{};
    /** @brief Source signals in the function's variable order. */
    const auto a = ntk.create_pi("a");
    /** @brief Second source signal. */
    const auto b = ntk.create_pi("b");
    /** @brief Third source signal. */
    const auto c = ntk.create_pi("c");
    /** @brief Multiplexer truth table with three distinct variable roles. */
    kitty::dynamic_truth_table function{3};
    kitty::create_from_hex_string(function, "ca");
    /** @brief Source function to place through the node map. */
    const auto gate = ntk.create_node({a, b, c}, function);

    /** @brief Layout receiving the mapped function. */
    gate_layout<cartesian_layout> lyt{{3, 3}};
    /** @brief First layout input port. */
    const auto left = lyt.create_pi("a", {0, 0});
    /** @brief Second layout input port. */
    const auto middle = lyt.create_pi("b", {1, 0});
    /** @brief Third layout input port. */
    const auto right = lyt.create_pi("c", {2, 0});
    /** @brief Existing source-to-layout input mapping. */
    mockturtle::node_map<layout_object_id, named_technology_network> node2pos{ntk};
    node2pos[a] = left;
    node2pos[b] = middle;
    node2pos[c] = right;
    /** @brief Placed gate with the specification's input order and function. */
    const auto placed = place(lyt, {1, 1}, ntk, ntk.get_node(gate), node2pos);

    CHECK(lyt.input_count(placed) == 3);
    CHECK(lyt.source({placed, 0}) == left);
    CHECK(lyt.source({placed, 1}) == middle);
    CHECK(lyt.source({placed, 2}) == right);
    CHECK(lyt.object_function(placed) == function);
}

TEST_CASE("Two-input placement validates constant reduction", "[placement-utils]")
{
    /** @brief Source specification for invalid reductions. */
    named_technology_network ntk{};
    /** @brief First source input. */
    const auto a = ntk.create_pi();
    /** @brief Second source input. */
    const auto b = ntk.create_pi();
    /** @brief Third source input. */
    const auto c = ntk.create_pi();
    /** @brief Layout receiving a two-input reduction. */
    gate_layout<cartesian_layout> lyt{{3, 3}};
    /** @brief First layout input. */
    const auto left = lyt.create_pi("a", {0, 0});
    /** @brief Second layout input. */
    const auto right = lyt.create_pi("b", {1, 0});

    SECTION("Majority requires an explicit constant")
    {
        /** @brief Majority with no constant provided to the two-input placement. */
        const auto gate = ntk.create_maj(a, b, c);
        CHECK_THROWS_AS(place(lyt, {1, 1}, ntk, ntk.get_node(gate), left, right), std::invalid_argument);
    }

    SECTION("A constant cannot reduce four variables to two")
    {
        /** @brief Fourth input of the unsupported reduction. */
        const auto d = ntk.create_pi();
        /** @brief Four-variable function outside the three-input reduction contract. */
        kitty::dynamic_truth_table function{4};
        kitty::create_from_hex_string(function, "9acd");
        /** @brief Function whose constant reduction still has too many variables. */
        const auto gate = ntk.create_node({a, b, c, d}, function);
        CHECK_THROWS_AS(place(lyt, {1, 1}, ntk, ntk.get_node(gate), left, right, false), std::invalid_argument);
    }
}
