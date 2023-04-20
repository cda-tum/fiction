//
// Created by benjamin on 11.04.23.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/networks/views/input_ordering_view.hpp>

#include <fiction/algorithms/network_transformation/fanout_substitution.hpp>
#include <fiction/networks/technology_network.hpp>
#include <utils/blueprints/network_blueprints.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/views/names_view.hpp>

#include <vector>

using namespace fiction;

TEST_CASE("Input_ordering consider all PIs", "[input-ordering]")
{
    auto mux21 = blueprints::test_fanout_nodes_rank<mockturtle::names_view<mockturtle::aig_network>>();

    auto ordered_aig_ntk{input_ordering_view{
        mockturtle::fanout_view{fanout_substitution<mockturtle::names_view<technology_network>>(mux21)}}};

    std::vector<mockturtle::node<technology_network>> nodes_order;
    std::vector<mockturtle::node<technology_network>> expected_nodes_order{0, 1, 5,  4,  2,  3,  6,  8,
                                                                           7, 9, 10, 11, 12, 13, 14, 15};
    ordered_aig_ntk.foreach_node([&](const auto& n) { nodes_order.push_back(n); });

    CHECK(nodes_order == expected_nodes_order);
}
