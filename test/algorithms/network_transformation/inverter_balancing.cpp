//
// Created by benjamin on 13.04.23.
//
#include "fiction/algorithms/network_transformation/inverter_balancing.hpp"

#include <catch2/catch_test_macros.hpp>

#include "fiction/algorithms/network_transformation/fanout_substitution.hpp"
#include "fiction/networks/views/input_ordering_view.hpp"
#include "utils/blueprints/network_blueprints.hpp"

using namespace fiction;

TEST_CASE("Double Balancing", "[inverter_balancing]")
{
    auto test_nw = blueprints::fanout_substitution_corner_case_network<mockturtle::names_view<mockturtle::aig_network>>();

    auto aig_ntk{input_ordering_view{mockturtle::fanout_view{fanout_substitution<mockturtle::names_view<technology_network>>(test_nw)}}};

    CHECK(aig_ntk.num_gates() == 7);

    auto balanced_aig_ntk{input_ordering_view{mockturtle::fanout_view{inverter_balancing(fanout_substitution<mockturtle::names_view<technology_network>>(test_nw))}}};

    CHECK(balanced_aig_ntk.num_gates() == 4);
}