//
// Created by benjamin on 11.04.23.
//

#include <catch2/catch_test_macros.hpp>
#include "fiction/networks/views/input_ordering_view.hpp"
#include "fiction/algorithms/network_transformation/fanout_substitution.hpp"
#include "fiction/networks/technology_network.hpp"
#include "utils/blueprints/network_blueprints.hpp"

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/klut.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/networks/xmg.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/views/names_view.hpp>

#include <vector>

using namespace fiction;

TEST_CASE("Input_ordering consider all PIs", "[input-ordering]")
{
    auto mux21 = blueprints::test_fanout_swap<mockturtle::names_view<mockturtle::aig_network>>();

    mockturtle::fanout_view topo_aig_ntk{mockturtle::fanout_view{fanout_substitution<mockturtle::names_view<technology_network>>(mux21)}};

    std::cout<<"Topological Order"<<std::endl;
    topo_aig_ntk.foreach_node([&](const auto& n)
                         {
                             std::cout<<n<<std::endl;
                         });

    mockturtle::fanout_view ordered_aig_ntk{input_ordering_view{mockturtle::fanout_view{fanout_substitution<mockturtle::names_view<technology_network>>(mux21)}}};

    std::cout<<"Topological Order"<<std::endl;
    ordered_aig_ntk.foreach_node([&](const auto& n)
                         {
                             std::cout<<n<<std::endl;
                         });
}