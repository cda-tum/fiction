//
// Created by marcel on 21.05.21.
//

#include "catch.hpp"
#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking.hpp"

#include <fiction/algorithms/network_conversion.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/networks/topology_network.hpp>

#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/traits.hpp>

#include <type_traits>

using namespace fiction;

template <typename Ntk>
void to_x(Ntk ntk)
{
    SECTION("MIG")
    {
        auto converted_mig = convert_network<mockturtle::mig_network>(ntk);

        check_eq(ntk, converted_mig);
    }

    SECTION("AIG")
    {
        auto converted_aig = convert_network<mockturtle::aig_network>(ntk);

        check_eq(ntk, converted_aig);
    }

    SECTION("XAG")
    {
        auto converted_xag = convert_network<mockturtle::xag_network>(ntk);

        check_eq(ntk, converted_xag);
    }

    SECTION("TOP")
    {
        auto converted_top = convert_network<topology_network>(ntk);

        check_eq(ntk, converted_top);
    }
}

TEST_CASE("Simple network conversion", "[algorithms]")
{
    SECTION("MIG to X")
    {
        auto mig = blueprints::maj1_network<mockturtle::mig_network>();

        to_x(mig);
    }
    SECTION("AIG to X")
    {
        auto aig = blueprints::maj1_network<mockturtle::aig_network>();

        to_x(aig);
    }
    SECTION("TOP to X")
    {
        auto top = blueprints::maj1_network<fiction::topology_network>();

        to_x(top);
    }
}

TEST_CASE("Complex network conversion", "[algorithms]")
{
    SECTION("MIG to X")
    {
        auto mig = blueprints::maj4_network<mockturtle::mig_network>();

        to_x(mig);
    }
    SECTION("AIG to X")
    {
        auto aig = blueprints::maj4_network<mockturtle::aig_network>();

        to_x(aig);
    }
    SECTION("TOP to X")
    {
        auto top = blueprints::maj4_network<fiction::topology_network>();

        to_x(top);
    }
}

TEST_CASE("Layout conversion", "[algorithms]")
{
    SECTION("Gate layout to X")
    {
        using gate_layout = gate_level_layout<clocked_layout<tile_based_layout>>;

        REQUIRE(mockturtle::has_compute_v<gate_layout, kitty::dynamic_truth_table>);

        auto layout = blueprints::xor_maj_gate_layout<gate_layout>();

        to_x(layout);
    }
}
