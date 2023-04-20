//
// Created by benjamin on 13.04.23.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"

#include <fiction/algorithms/network_transformation/fanout_substitution.hpp>
#include <fiction/algorithms/network_transformation/inverter_balancing.hpp>

using namespace fiction;

TEST_CASE("One Balancing", "[inverter-balancing]")
{
    auto test_nw = blueprints::test_inverter_balancing<mockturtle::names_view<mockturtle::aig_network>>();
    auto aig_ntk{fanout_substitution<mockturtle::names_view<technology_network>>(test_nw)};

    CHECK(aig_ntk.num_gates() == 9);

    auto balanced_aig_ntk{inverter_balancing(aig_ntk)};

    CHECK(balanced_aig_ntk.num_gates() == 8);
}

TEST_CASE("Double Balancing", "[inverter-balancing]")
{
    auto test_nw =
        blueprints::fanout_substitution_corner_case_network<mockturtle::names_view<mockturtle::aig_network>>();

    auto aig_ntk{fanout_substitution<mockturtle::names_view<technology_network>>(test_nw)};

    CHECK(aig_ntk.num_gates() == 7);

    auto balanced_aig_ntk{inverter_balancing(aig_ntk)};

    CHECK(balanced_aig_ntk.num_gates() == 4);
}