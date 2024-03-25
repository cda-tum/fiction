//
// Created by marcel on 14.10.21.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"

#include <fiction/networks/technology_network.hpp>
#include <fiction/utils/network_utils.hpp>

#include <mockturtle/networks/mig.hpp>
#include <mockturtle/views/fanout_view.hpp>

#include <string>
#include <type_traits>

using namespace fiction;

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
