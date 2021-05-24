//
// Created by marcel on 21.05.21.
//

#include "network_conversion.hpp"

#include "catch.hpp"
#include "clocked_layout.hpp"
#include "gate_level_layout.hpp"
#include "layout_blueprints.hpp"
#include "network_blueprints.hpp"
#include "tile_based_layout.hpp"
#include "topology_network.hpp"

#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/algorithms/miter.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/klut.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/traits.hpp>

#include <type_traits>

using namespace fiction;

template <typename Ntk1, typename Ntk2>
void check_eq(Ntk1 ntk1, Ntk2 ntk2)
{
    auto miter = mockturtle::miter<mockturtle::klut_network>(ntk1, ntk2);

    REQUIRE(miter.has_value());

    auto eq = mockturtle::equivalence_checking(*miter);

    REQUIRE(eq.has_value());
    CHECK(*eq);
}

template <typename Ntk>
void to_x(Ntk ntk)
{
    SECTION("MIG")
    {
        auto converted_mig = convert<mockturtle::mig_network>(ntk);

        check_eq(ntk, converted_mig);
    }

    SECTION("AIG")
    {
        auto converted_aig = convert<mockturtle::aig_network>(ntk);

        check_eq(ntk, converted_aig);
    }

    SECTION("XAG")
    {
        auto converted_xag = convert<mockturtle::xag_network>(ntk);

        check_eq(ntk, converted_xag);
    }

    SECTION("TOP")
    {
        auto converted_top = convert<topology_network>(ntk);

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
