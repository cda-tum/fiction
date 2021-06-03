//
// Created by marcel on 02.06.21.
//

#include "catch.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking.hpp"

#include <fiction/algorithms/fanout_substitution.hpp>
#include <fiction/algorithms/orthogonal.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/networks/topology_network.hpp>
#include <fiction/io/print_layout.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/views/fanout_view.hpp>

#include <type_traits>

using namespace fiction;

TEST_CASE("South-east coloring", "[algorithms]")
{
    const auto check = [](const auto& net)
    {
        auto container = detail::east_south_coloring(net);
        CHECK(detail::is_east_south_colored(container.color_ntk));
    };

    check(mockturtle::fanout_view{
        fanout_substitution<topology_network>(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>())});
    check(mockturtle::fanout_view{
        fanout_substitution<topology_network>(blueprints::maj1_network<mockturtle::aig_network>())});
    check(mockturtle::fanout_view{
        fanout_substitution<topology_network>(blueprints::maj4_network<mockturtle::aig_network>())});
    check(mockturtle::fanout_view{
        fanout_substitution<topology_network>(blueprints::se_coloring_corner_case_network<topology_network>())});
}

TEST_CASE("Layout equivalence", "[algorithms]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout>>;

    const auto check = [](const auto& net)
    {
        auto layout = orthogonal<gate_layout>(net);

        print_gate_level_layout(std::cout, layout);

        check_eq(net, layout);
    };

    check(mockturtle::fanout_view{
        fanout_substitution<topology_network>(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>())});
    check(mockturtle::fanout_view{
        fanout_substitution<topology_network>(blueprints::maj1_network<mockturtle::aig_network>())});
    check(mockturtle::fanout_view{
        fanout_substitution<topology_network>(blueprints::maj4_network<mockturtle::aig_network>())});
    check(mockturtle::fanout_view{
        fanout_substitution<topology_network>(blueprints::se_coloring_corner_case_network<topology_network>())});
}
