//
// Created by benjamin on 11.04.23.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/network_transformation/fanout_substitution.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/input_ordering_view.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/views/names_view.hpp>

#include <vector>

#include <utils/blueprints/network_blueprints.hpp>

using namespace fiction;

TEST_CASE("Input ordering view traits", "[input-ordering]")
{
    using ntk = mockturtle::fanout_view<mockturtle::names_view<technology_network>>;

    CHECK(mockturtle::is_network_type_v<ntk>);
    CHECK(mockturtle::has_foreach_fanout_v<ntk>);
    CHECK(!fiction::has_get_fo_two_v<ntk>);
    CHECK(!fiction::has_get_fo_one_v<ntk>);
    CHECK(!fiction::has_get_pi_to_pi_v<ntk>);
    CHECK(!fiction::has_nc_inv_flag_v<ntk>);
    CHECK(!fiction::has_nc_inv_num_v<ntk>);

    using ordered_ntk = input_ordering_view<mockturtle::fanout_view<mockturtle::names_view<technology_network>>>;

    CHECK(mockturtle::is_network_type_v<ordered_ntk>);
    CHECK(mockturtle::has_foreach_fanout_v<ntk>);
    CHECK(fiction::has_get_fo_two_v<ordered_ntk>);
    CHECK(fiction::has_get_fo_one_v<ordered_ntk>);
    CHECK(fiction::has_get_pi_to_pi_v<ordered_ntk>);
    CHECK(fiction::has_nc_inv_flag_v<ordered_ntk>);
    CHECK(fiction::has_nc_inv_num_v<ordered_ntk>);
}

TEST_CASE("Order mux technology network", "[input-ordering]")
{
    auto mux = blueprints::mux21_network<mockturtle::names_view<technology_network>>();

    auto ordered_tec_ntk{input_ordering_view{
        mockturtle::fanout_view{fanout_substitution<mockturtle::names_view<technology_network>>(mux)}}};

    std::vector<mockturtle::node<technology_network>> nodes_order;
    std::vector<mockturtle::node<technology_network>> expected_nodes_order{0, 1, 4, 2, 3, 5, 6, 7, 8, 9};
    ordered_tec_ntk.foreach_node([&](const auto& n) { nodes_order.push_back(n); });

    CHECK(nodes_order == expected_nodes_order);
}

TEST_CASE("Input_ordering consider all pis aig network", "[input-ordering]")
{
    auto test_nw = blueprints::test_fanout_nodes_rank<mockturtle::names_view<mockturtle::aig_network>>();

    auto ordered_aig_ntk{input_ordering_view{
        mockturtle::fanout_view{fanout_substitution<mockturtle::names_view<technology_network>>(test_nw)}}};

    std::vector<mockturtle::node<technology_network>> nodes_order;
    std::vector<mockturtle::node<technology_network>> expected_nodes_order{0, 1, 5,  4,  2,  3,  6,  8,
                                                                           7, 9, 10, 11, 12, 13, 14, 15};
    ordered_aig_ntk.foreach_node([&](const auto& n) { nodes_order.push_back(n); });

    CHECK(nodes_order == expected_nodes_order);
}

TEST_CASE("Test inverters after PIs", "[input-ordering]")
{
    auto test_nw = blueprints::input_ordering_inv_flag<mockturtle::names_view<technology_network>>();

    auto ordered_aig_ntk{input_ordering_view{
        mockturtle::fanout_view{fanout_substitution<mockturtle::names_view<technology_network>>(test_nw)}}};

    CHECK(ordered_aig_ntk.nc_inv_flag());
}
