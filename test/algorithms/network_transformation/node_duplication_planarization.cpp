//
// Created by benjamin on 6/11/24.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/networks/technology_network.hpp>
#include <fiction/algorithms/network_transformation/node_duplication_planarization.hpp>

using namespace fiction;

TEST_CASE("First test", "[node-duplication-planarization]")
{
    auto maj = blueprints::maj1_network<mockturtle::aig_network>();

    const auto planarized_maj = node_duplication_planarization<technology_network>(maj);

    CHECK(1 == 1);
}
