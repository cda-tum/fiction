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
 * @author Marcel Walter (marcelwa)
 * @author Benjamin Hien (hibenj)
 */

#include <fiction/networks/network_utils.hpp>

// Test dependencies follow the defining header to expose missing direct includes.
#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"

#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/mutable_rank_view.hpp>
#include <fiction/networks/virtual_pi_network.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/views/depth_view.hpp>
#include <mockturtle/views/fanout_view.hpp>

#include <cstdint>
#include <string>
#include <vector>

using namespace fiction;
using namespace fiction::networks;

TEST_CASE("Number of constant fanins", "[network-utils]")
{
    const auto maj4 = blueprints::maj4_network<mockturtle::mig_network>();

    maj4.foreach_node(
        [&maj4](const auto& n)
        {
            CAPTURE(n);
            CHECK(num_constant_fanins(maj4, n) == 0ul);
        });

    const auto and_inv = blueprints::unbalanced_and_inv_network<mockturtle::mig_network>();

    REQUIRE(and_inv.num_pos() == 1);
    /** @brief AND output with one constant fanin and two primary-input fanins. */
    const auto output = and_inv.get_node(and_inv.po_at(0));
    CHECK(num_constant_fanins(and_inv, output) == 1ul);
    CHECK(has_incoming_primary_input(and_inv, output));
}

TEST_CASE("High-degree fanin nodes", "[network-utils]")
{
    const auto maj4 = blueprints::maj4_network<mockturtle::mig_network>();

    CHECK(has_high_degree_fanin_nodes(maj4, 2));
    CHECK_FALSE(has_high_degree_fanin_nodes(maj4, 3));

    const auto and_inv = blueprints::unbalanced_and_inv_network<mockturtle::mig_network>();

    CHECK(has_high_degree_fanin_nodes(and_inv, 1));
    CHECK_FALSE(has_high_degree_fanin_nodes(and_inv, 2));
    CHECK_FALSE(has_high_degree_fanin_nodes(and_inv, 3));

    CHECK(high_degree_fanin_exception{}.what() ==
          std::string{"network contains nodes that exceed the supported fanin size"});
}

TEST_CASE("Incoming primary input", "[network-utils]")
{
    const auto maj4 = blueprints::maj4_network<mockturtle::mig_network>();

    CHECK_FALSE(has_incoming_primary_input(maj4, maj4.get_node(maj4.get_constant(false))));

    /** @brief Direct-PI results for all five declared inputs. */
    std::vector<bool> input_results{};
    maj4.foreach_pi([&](const auto& n) { input_results.push_back(has_incoming_primary_input(maj4, n)); });
    CHECK(input_results == std::vector<bool>{false, false, false, false, false});

    /** @brief Direct-PI results for all four output drivers in declared order. */
    std::vector<bool> output_results{};
    maj4.foreach_po([&](const auto& f)
                    { output_results.push_back(has_incoming_primary_input(maj4, maj4.get_node(f))); });
    CHECK(output_results == std::vector<bool>{true, true, true, false});
}

TEST_CASE("Inverse levels", "[network-utils]")
{
    const auto tec = blueprints::one_to_five_path_difference_network<mockturtle::fanout_view<technology_network>>();

    const auto inv_levels = inverse_levels(tec);

    // Two constants stay at level zero; the input paths contain one and five buffers before the AND.
    CHECK(inv_levels == std::vector<uint32_t>{0, 0, 2, 6, 1, 5, 4, 3, 2, 1, 0});
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

TEST_CASE("Buffer chains", "[network-utils]")
{
    technology_network tec{};

    const auto x = tec.create_pi();

    SECTION("a chain of length zero returns the signal")
    {
        CHECK(buffer_chain(tec, x, 0) == x);
        CHECK(tec.num_gates() == 0);
    }
    SECTION("every buffer extends the chain by one level")
    {
        const auto end = buffer_chain(tec, x, 3);

        tec.create_po(end);

        CHECK(tec.num_gates() == 3);
        CHECK(tec.is_buf(tec.get_node(end)));
        CHECK(mockturtle::depth_view{tec}.depth() == 3);
    }
}
