//
// Created by benjamin on 11.04.23.
//

#include <catch2/catch_test_macros.hpp>
#include "fiction/networks/views/input_ordering_view.hpp"
#include "fiction/algorithms/network_transformation/fanout_substitution.hpp"
#include "fiction/networks/technology_network.hpp"

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
    mockturtle::aig_network aig_ntk{};

    const auto z = aig_ntk.create_pi();
    const auto a = aig_ntk.create_pi();
    const auto b = aig_ntk.create_pi();
    const auto c = aig_ntk.create_pi();
    const auto d = aig_ntk.create_pi();

    const auto f1 = aig_ntk.create_and(a, b);
    const auto f2 = aig_ntk.create_and(a, c);
    const auto f3 = aig_ntk.create_and(f1, d);
    const auto f4 = aig_ntk.create_and(f2, f3);
    const auto f5 = aig_ntk.create_and(f4, z);

    aig_ntk.create_po(f5);

    mockturtle::fanout_view topo_aig_ntk{mockturtle::fanout_view{fanout_substitution<mockturtle::names_view<technology_network>>(aig_ntk)}};

    std::cout<<"Topological Order"<<std::endl;
    topo_aig_ntk.foreach_node([&](const auto& n)
                         {
                             std::cout<<n<<std::endl;
                         });

    mockturtle::fanout_view ordered_aig_ntk{input_ordering_view{mockturtle::fanout_view{fanout_substitution<mockturtle::names_view<technology_network>>(aig_ntk)}}};

    std::cout<<"Topological Order"<<std::endl;
    ordered_aig_ntk.foreach_node([&](const auto& n)
                         {
                             std::cout<<n<<std::endl;
                         });
}