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
    technology_network tec{};

    CHECK(mockturtle::has_clear_visited_v<technology_network>);
    CHECK(mockturtle::has_visited_v<technology_network>);
    CHECK(mockturtle::has_set_visited_v<technology_network>);

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto x3 = tec.create_pi();

    const auto f1 = tec.create_and(x1, x2);
    const auto f2 = tec.create_and(x1, x2);
    const auto f3 = tec.create_and(x2, x3);
    const auto f4 = tec.create_not(x3);
    tec.create_po(f1);
    tec.create_po(f2);
    tec.create_po(f3);
    tec.create_po(f4);

    // auto maj = blueprints::maj1_network<mockturtle::aig_network>();

    const auto planarized_maj = node_duplication_planarization<technology_network>(tec);

    CHECK(1 == 1);
}

TEST_CASE("Multi Level test", "[node-duplication-planarization]")
{
    technology_network tec{};

    CHECK(mockturtle::has_clear_visited_v<technology_network>);
    CHECK(mockturtle::has_visited_v<technology_network>);
    CHECK(mockturtle::has_set_visited_v<technology_network>);

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto x3 = tec.create_pi();
    const auto x4 = tec.create_pi();
    const auto x5 = tec.create_pi();
    const auto f1 = tec.create_not(x2);
    const auto f2 = tec.create_nary_and({x1, x2, x3, x4});
    const auto f3 = tec.create_nary_and({x3, x4, x5});
    tec.create_po(f1);
    tec.create_po(f2);
    tec.create_po(f3);

    // auto maj = blueprints::maj1_network<mockturtle::aig_network>();

    const auto planarized_maj = node_duplication_planarization<technology_network>(tec);

    CHECK(1 == 1);
}
