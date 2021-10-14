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
