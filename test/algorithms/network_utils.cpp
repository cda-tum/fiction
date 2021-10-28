//
// Created by marcel on 14.10.21.
//

#include "catch.hpp"
#include "utils/blueprints/network_blueprints.hpp"

#include <fiction/algorithms/network_utils.hpp>

#include <mockturtle/networks/mig.hpp>

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
}
