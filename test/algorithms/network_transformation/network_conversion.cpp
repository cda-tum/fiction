//
// Created by marcel on 21.05.21.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/algorithms/network_transformation/fanout_substitution.hpp>
#include <fiction/algorithms/network_transformation/network_balancing.hpp>
#include <fiction/algorithms/network_transformation/network_conversion.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/networks/technology_network.hpp>

#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/traits.hpp>

#include <type_traits>

using namespace fiction;

template <typename Ntk>
void to_x(const Ntk& ntk)
{
    SECTION("MIG")
    {
        const auto converted_mig = convert_network<mockturtle::mig_network>(ntk);

        check_eq(ntk, converted_mig);
    }

    SECTION("AIG")
    {
        const auto converted_aig = convert_network<mockturtle::aig_network>(ntk);

        check_eq(ntk, converted_aig);
    }

    SECTION("XAG")
    {
        const auto converted_xag = convert_network<mockturtle::xag_network>(ntk);

        check_eq(ntk, converted_xag);
    }

    SECTION("TEC")
    {
        const auto converted_tec = convert_network<technology_network>(ntk);

        check_eq(ntk, converted_tec);
    }
}

TEST_CASE("Name conservation after network conversion", "[network-conversion]")
{
    auto maj = blueprints::maj1_network<mockturtle::names_view<mockturtle::mig_network>>();
    maj.set_network_name("maj");

    const auto converted_maj = convert_network<mockturtle::names_view<fiction::technology_network>>(maj);

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
        const auto mig = blueprints::maj1_network<mockturtle::mig_network>();

        to_x(mig);
    }
    SECTION("AIG to X")
    {
        const auto aig = blueprints::maj1_network<mockturtle::aig_network>();

        to_x(aig);
    }
    SECTION("XAG to X")
    {
        const auto xag = blueprints::maj1_network<mockturtle::xag_network>();

        to_x(xag);
    }
    SECTION("TEC to X")
    {
        const auto tec = blueprints::maj1_network<fiction::technology_network>();

        to_x(tec);
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
    SECTION("TEC to X")
    {
        to_x(blueprints::maj4_network<fiction::technology_network>());
        to_x(blueprints::nary_operation_network<fiction::technology_network>());
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
    // no test for technology_network because it does not support inverted signals
}

TEST_CASE("Layout conversion", "[network-conversion]")
{
    SECTION("Gate layout to X")
    {
        using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

        REQUIRE(mockturtle::has_compute_v<gate_layout, kitty::dynamic_truth_table>);

        const auto layout = blueprints::xor_maj_gate_layout<gate_layout>();

        to_x(layout);
    }
}

TEST_CASE("Consistent network size after multiple conversions", "[network-conversion]")
{
    const auto tec = blueprints::se_coloring_corner_case_network<technology_network>();

    const auto converted = convert_network<technology_network>(
        convert_network<technology_network>(convert_network<technology_network>(tec)));

    CHECK(tec.size() == converted.size());

    const auto converted_aig = convert_network<technology_network>(blueprints::maj4_network<mockturtle::aig_network>());

    const auto converted_converted_aig = convert_network<technology_network>(
        convert_network<technology_network>(convert_network<technology_network>(converted_aig)));

    CHECK(converted_aig.size() == converted_converted_aig.size());
}

TEST_CASE("Consistent network size after fanout substitution and conversion", "[network-conversion]")
{
    const auto substituted_aig =
        fanout_substitution<technology_network>(blueprints::maj4_network<mockturtle::aig_network>());
    const auto converted_substituted_aig = convert_network<technology_network>(substituted_aig);
    CHECK(substituted_aig.size() == converted_substituted_aig.size());

    const auto substituted_tec = fanout_substitution<technology_network>(
        blueprints::fanout_substitution_corner_case_network<technology_network>());
    const auto converted_substituted_tec = convert_network<technology_network>(substituted_tec);
    CHECK(substituted_tec.size() == converted_substituted_tec.size());
}

TEST_CASE("Consistent network size after balancing and conversion", "[network-conversion]")
{
    const auto balanced_aig =
        network_balancing<technology_network>(blueprints::maj4_network<mockturtle::aig_network>());
    const auto converted_balanced_aig = convert_network<technology_network>(balanced_aig);
    CHECK(balanced_aig.size() == converted_balanced_aig.size());

    const auto balanced_tec = network_balancing<technology_network>(
        blueprints::fanout_substitution_corner_case_network<technology_network>());
    const auto converted_balanced_tec = convert_network<technology_network>(balanced_tec);
    CHECK(balanced_tec.size() == converted_balanced_tec.size());
}
