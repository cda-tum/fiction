//
// Created by marcel on 21.05.21.
//

#include "catch.hpp"
#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking.hpp"

#include <fiction/algorithms/fanout_substitution.hpp>
#include <fiction/algorithms/network_balancing.hpp>
#include <fiction/algorithms/network_conversion.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
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

TEST_CASE("Name conservation", "[network-conversion]")
{
    auto maj = blueprints::maj1_network<mockturtle::names_view<mockturtle::mig_network>>();
    maj.set_network_name("maj");

    const auto converted_maj = convert_network<mockturtle::names_view<fiction::topology_network>>(maj);

    // network name
    CHECK(converted_maj.get_network_name() == "maj");

    // PI names
    CHECK(converted_maj.get_name(converted_maj.make_signal(2)) == "a");
    CHECK(converted_maj.get_name(converted_maj.make_signal(3)) == "b");
    CHECK(converted_maj.get_name(converted_maj.make_signal(4)) == "c");

    // PO names
    CHECK(converted_maj.get_output_name(0) == "f");
}

TEST_CASE("Simple network conversion", "[network-conversion]")
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
    SECTION("XAG to X")
    {
        auto xag = blueprints::maj1_network<mockturtle::xag_network>();

        to_x(xag);
    }
    SECTION("TOP to X")
    {
        auto top = blueprints::maj1_network<fiction::topology_network>();

        to_x(top);
    }
}

TEST_CASE("Complex network conversion", "[network-conversion]")
{
    SECTION("MIG to X")
    {
        to_x(blueprints::maj4_network<mockturtle::mig_network>());
        to_x(blueprints::nary_operation_network<mockturtle::mig_network>());
    }
    SECTION("AIG to X")
    {
        to_x(blueprints::maj4_network<mockturtle::aig_network>());
        to_x(blueprints::nary_operation_network<mockturtle::aig_network>());
    }
    SECTION("XAG to X")
    {
        to_x(blueprints::maj4_network<mockturtle::xag_network>());
        to_x(blueprints::nary_operation_network<mockturtle::xag_network>());
    }
    SECTION("TOP to X")
    {
        to_x(blueprints::maj4_network<fiction::topology_network>());
        to_x(blueprints::nary_operation_network<fiction::topology_network>());
    }
}

TEST_CASE("Constant inverted signal recovery conversion", "[network-conversion]")
{
    SECTION("MIG to X")
    {
        to_x(blueprints::constant_gate_input_maj_network<mockturtle::mig_network>());
    }
    SECTION("AIG to X")
    {
        to_x(blueprints::constant_gate_input_maj_network<mockturtle::aig_network>());
    }
    SECTION("XAG to X")
    {
        to_x(blueprints::constant_gate_input_maj_network<mockturtle::xag_network>());
    }
    // no test for topology_network because it does not support inverted signals
}

TEST_CASE("Layout conversion", "[network-conversion]")
{
    SECTION("Gate layout to X")
    {
        using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

        REQUIRE(mockturtle::has_compute_v<gate_layout, kitty::dynamic_truth_table>);

        auto layout = blueprints::xor_maj_gate_layout<gate_layout>();

        to_x(layout);
    }
}

TEST_CASE("Consistent network size after multiple conversions", "[network-conversion]")
{
    auto top = blueprints::se_coloring_corner_case_network<topology_network>();

    auto converted =
        convert_network<topology_network>(convert_network<topology_network>(convert_network<topology_network>(top)));

    CHECK(top.size() == converted.size());

    auto converted_aig = convert_network<topology_network>(blueprints::maj4_network<mockturtle::aig_network>());

    auto converted_converted_aig = convert_network<topology_network>(
        convert_network<topology_network>(convert_network<topology_network>(converted_aig)));

    CHECK(converted_aig.size() == converted_converted_aig.size());
}

TEST_CASE("Consistent network size after fanout substitution and conversion", "[network-conversion]")
{
    auto substituted_aig = fanout_substitution<topology_network>(blueprints::maj4_network<mockturtle::aig_network>());
    auto converted_substituted_aig = convert_network<topology_network>(substituted_aig);
    CHECK(substituted_aig.size() == converted_substituted_aig.size());

    auto substituted_top =
        fanout_substitution<topology_network>(blueprints::fanout_substitution_corner_case_network<topology_network>());
    auto converted_substituted_top = convert_network<topology_network>(substituted_top);
    CHECK(substituted_top.size() == converted_substituted_top.size());
}

TEST_CASE("Consistent network size after balancing and conversion", "[network-conversion]")
{
    auto balanced_aig = network_balancing<topology_network>(blueprints::maj4_network<mockturtle::aig_network>());
    auto converted_balanced_aig = convert_network<topology_network>(balanced_aig);
    CHECK(balanced_aig.size() == converted_balanced_aig.size());

    auto balanced_top =
        network_balancing<topology_network>(blueprints::fanout_substitution_corner_case_network<topology_network>());
    auto converted_balanced_top = convert_network<topology_network>(balanced_top);
    CHECK(balanced_top.size() == converted_balanced_top.size());
}
