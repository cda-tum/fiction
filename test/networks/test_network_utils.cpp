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
 * @brief Tests for `fiction/networks/network_utils.hpp`.
 * @author Benjamin Hien (hibenj)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"

#include <fiction/networks/network_utils.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/mutable_rank_view.hpp>
#include <fiction/networks/virtual_pi_network.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/views/fanout_view.hpp>

#include <string>
#include <vector>

using namespace fiction;
using namespace fiction::networks;

TEST_CASE("Number of constant fanins", "[network-utils]")
{
    const auto maj4 = blueprints::maj4_network<mockturtle::mig_network>();

    maj4.foreach_node([&maj4](const auto& n) { CHECK(num_constant_fanins(maj4, n) == 0ul); });

    const auto and_inv = blueprints::unbalanced_and_inv_network<mockturtle::mig_network>();

    CHECK(num_constant_fanins(and_inv, 3) == 1ul);
}

TEST_CASE("High-degree fanin nodes", "[network-utils]")
{
    const auto maj4 = blueprints::maj4_network<mockturtle::mig_network>();

    CHECK(has_high_degree_fanin_nodes(maj4, 2));
    CHECK(!has_high_degree_fanin_nodes(maj4, 3));

    const auto and_inv = blueprints::unbalanced_and_inv_network<mockturtle::mig_network>();

    CHECK(has_high_degree_fanin_nodes(and_inv, 1));
    CHECK(!has_high_degree_fanin_nodes(and_inv, 2));
    CHECK(!has_high_degree_fanin_nodes(and_inv, 3));

    CHECK(high_degree_fanin_exception{}.what() ==
          std::string{"network contains nodes that exceed the supported fanin size"});
}

TEST_CASE("Incoming primary input", "[network-utils]")
{
    const auto maj4 = blueprints::maj4_network<mockturtle::mig_network>();

    // constant node
    CHECK(!has_incoming_primary_input(maj4, mockturtle::node<mockturtle::mig_network>{0}));

    // PI nodes
    CHECK(!has_incoming_primary_input(maj4, mockturtle::node<mockturtle::mig_network>{1}));
    CHECK(!has_incoming_primary_input(maj4, mockturtle::node<mockturtle::mig_network>{2}));
    CHECK(!has_incoming_primary_input(maj4, mockturtle::node<mockturtle::mig_network>{3}));
    CHECK(!has_incoming_primary_input(maj4, mockturtle::node<mockturtle::mig_network>{4}));
    CHECK(!has_incoming_primary_input(maj4, mockturtle::node<mockturtle::mig_network>{5}));

    // MAJ nodes with incoming PIs
    CHECK(has_incoming_primary_input(maj4, mockturtle::node<mockturtle::mig_network>{6}));
    CHECK(has_incoming_primary_input(maj4, mockturtle::node<mockturtle::mig_network>{7}));
    CHECK(has_incoming_primary_input(maj4, mockturtle::node<mockturtle::mig_network>{8}));

    // MAJ node without incoming PIs
    CHECK(!has_incoming_primary_input(maj4, mockturtle::node<mockturtle::mig_network>{9}));
}

TEST_CASE("Inverse levels", "[network-utils]")
{
    const auto tec = blueprints::one_to_five_path_difference_network<mockturtle::fanout_view<technology_network>>();

    const auto inv_levels = inverse_levels(tec);

    // there should be 11 nodes in the technology network (2 constants, 2 PIs, 6 BUFs, 1 AND)
    REQUIRE(inv_levels.size() == 11);

    // constant does not get a level assigned
    CHECK(inv_levels[0] == 0);
    CHECK(inv_levels[1] == 0);
    CHECK(inv_levels[2] == 2);
    CHECK(inv_levels[3] == 6);
    CHECK(inv_levels[4] == 1);
    CHECK(inv_levels[5] == 5);
    CHECK(inv_levels[6] == 4);
    CHECK(inv_levels[7] == 3);
    CHECK(inv_levels[8] == 2);
    CHECK(inv_levels[9] == 1);
    CHECK(inv_levels[10] == 0);
}

TEST_CASE("Initialize a copy network with virtual primary inputs", "[network-utils]")
{
    SECTION("network with two distinct constants")
    {
        technology_network tec{};

        const auto x1 = tec.create_pi();
        const auto x2 = tec.create_pi();
        tec.create_po(tec.create_and(x1, x2));

        const auto [dest, old2new] = initialize_copy_network_with_virtual_pis(tec);

        CHECK(dest.num_pis() == 2);
        CHECK(dest.num_gates() == 0);
        CHECK(dest.get_node(old2new[tec.get_constant(false)]) == dest.get_node(dest.get_constant(false)));
        CHECK(dest.get_node(old2new[tec.get_constant(true)]) == dest.get_node(dest.get_constant(true)));
        CHECK(dest.is_pi(dest.get_node(old2new[x1])));
        CHECK(dest.is_pi(dest.get_node(old2new[x2])));
        CHECK(dest.get_node(old2new[x1]) != dest.get_node(old2new[x2]));
    }
    SECTION("network with one constant")
    {
        mockturtle::aig_network aig{};

        const auto x1 = aig.create_pi();
        aig.create_po(aig.create_and(x1, aig.get_constant(true)));

        const auto [dest, old2new] = initialize_copy_network_with_virtual_pis(aig);

        CHECK(dest.num_pis() == 1);
        CHECK(dest.get_node(old2new[aig.get_constant(true)]) == dest.get_node(dest.get_constant(false)));
        CHECK(dest.is_pi(dest.get_node(old2new[x1])));
    }
    SECTION("virtual primary inputs map to virtual copies of the copied real inputs")
    {
        virtual_pi_network<technology_network> vpi{};

        const auto x1 = vpi.create_pi();
        const auto x2 = vpi.create_pi();
        const auto v1 = vpi.create_virtual_pi(x1);
        vpi.create_po(vpi.create_and(x2, v1));

        const auto [dest, old2new] = initialize_copy_network_with_virtual_pis(vpi);

        CHECK(dest.num_real_pis() == 2);
        CHECK(dest.num_virtual_pis() == 1);
        CHECK(dest.is_real_pi(dest.get_node(old2new[x1])));
        CHECK(dest.is_virtual_pi(dest.get_node(old2new[v1])));
        CHECK(dest.get_real_pi(dest.get_node(old2new[v1])) == dest.get_node(old2new[x1]));
    }
    SECTION("ranked networks copy their inputs in storage order")
    {
        technology_network tec{};

        const auto x1 = tec.create_pi();
        const auto x2 = tec.create_pi();
        const auto x3 = tec.create_pi();
        tec.create_po(tec.create_and(x1, x3));
        tec.create_po(x2);

        const views::mutable_rank_view ranked{tec};

        // no structured bindings here: the static analyzer cannot follow them into the lambdas below
        const auto  copy    = initialize_copy_network_with_virtual_pis(ranked);
        const auto& dest    = copy.first;
        const auto& old2new = copy.second;

        // the copy of the i-th stored input is the i-th stored input of the destination
        std::vector<mockturtle::node<technology_network>> copies{};
        ranked.foreach_pi_unranked([&](const auto& n) { copies.push_back(dest.get_node(old2new[n])); });

        std::vector<mockturtle::node<technology_network>> dest_pis{};
        dest.foreach_pi_unranked([&](const auto& n) { dest_pis.push_back(n); });

        CHECK(copies == dest_pis);
        CHECK(copies.size() == 3);
    }
}

TEST_CASE("Barycenters of ranked nodes", "[network-utils]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto x3 = tec.create_pi();
    const auto a1 = tec.create_and(x1, x3);  // fanins at rank positions 0 and 2
    const auto b1 = tec.create_buf(x2);      // fanin at rank position 1
    const auto c1 = tec.create_and(tec.get_constant(true), x3);
    tec.create_po(a1);
    tec.create_po(b1);
    tec.create_po(c1);

    const views::mutable_rank_view ranked{tec};

    REQUIRE(ranked.rank_position(tec.get_node(x1)) == 0);
    REQUIRE(ranked.rank_position(tec.get_node(x2)) == 1);
    REQUIRE(ranked.rank_position(tec.get_node(x3)) == 2);

    const auto centers = barycenters(ranked, {tec.get_node(a1), tec.get_node(b1), tec.get_node(c1)});

    REQUIRE(centers.size() == 3);
    CHECK(centers[0] == 1.0);
    CHECK(centers[1] == 1.0);
    CHECK(centers[2] == 2.0);  // the constant fanin is ignored

    CHECK(barycenters(ranked, {}).empty());

    // a node whose only fanin is constant has barycenter 0
    technology_network constant_only{};
    const auto         y  = constant_only.create_pi();
    const auto         nc = constant_only.create_not(constant_only.get_constant(false));
    constant_only.create_po(constant_only.create_and(y, nc));

    const views::mutable_rank_view ranked_constant{constant_only};

    CHECK(barycenters(ranked_constant, {constant_only.get_node(nc)}) == std::vector<double>{0.0});
}
