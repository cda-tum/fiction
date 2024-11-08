//
// Created by Simon Hofmann on 23.01.24.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/algorithms/physical_design/orthogonal.hpp>
#include <fiction/algorithms/physical_design/wiring_reduction.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/networks/technology_network.hpp>

#include <mockturtle/networks/aig.hpp>

using namespace fiction;

template <typename Lyt, typename Ntk>
void check_layout_equiv(const Ntk& ntk)
{
    const auto layout = orthogonal<Lyt>(ntk, {});

    wiring_reduction_stats stats{};
    wiring_reduction<Lyt>(layout, {}, &stats);

    check_eq(ntk, layout);

    CHECK(mockturtle::to_seconds(stats.time_total) > 0);
}

template <typename Lyt>
void check_layout_equiv_all()
{
    SECTION("Maj1 Network")
    {
        check_layout_equiv<Lyt>(blueprints::maj1_network<mockturtle::aig_network>());
    }
    SECTION("unbalanced_and_inv_network")
    {
        check_layout_equiv<Lyt>(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>());
    }
    SECTION("and_or_network")
    {
        check_layout_equiv<Lyt>(blueprints::and_or_network<technology_network>());
    }
    SECTION("nary_operation_network")
    {
        check_layout_equiv<Lyt>(blueprints::nary_operation_network<technology_network>());
    }
    SECTION("constant_gate_input_maj_network")
    {
        check_layout_equiv<Lyt>(blueprints::constant_gate_input_maj_network<technology_network>());
    }
    SECTION("half_adder_network")
    {
        check_layout_equiv<Lyt>(blueprints::half_adder_network<technology_network>());
    }
    SECTION("full_adder_network")
    {
        check_layout_equiv<Lyt>(blueprints::full_adder_network<technology_network>());
    }
    SECTION("mux21_network")
    {
        check_layout_equiv<Lyt>(blueprints::mux21_network<technology_network>());
    }
    SECTION("se_coloring_corner_case_network")
    {
        check_layout_equiv<Lyt>(blueprints::se_coloring_corner_case_network<technology_network>());
    }
    SECTION("clpl")
    {
        check_layout_equiv<Lyt>(blueprints::clpl<technology_network>());
    }
    SECTION("nand_xnor_network")
    {
        check_layout_equiv<Lyt>(blueprints::nand_xnor_network<technology_network>());
    }
    SECTION("fanout_substitution_corner_case_network")
    {
        check_layout_equiv<Lyt>(blueprints::fanout_substitution_corner_case_network<technology_network>());
    }
    SECTION("inverter_network")
    {
        check_layout_equiv<Lyt>(blueprints::inverter_network<technology_network>());
    }
}

TEST_CASE("Layout equivalence", "[wiring_reduction]")
{
    SECTION("Cartesian layouts")
    {
        using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

        check_layout_equiv_all<gate_layout>();
    }

    SECTION("Corner Cases")
    {
        using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<>>>>;

        const auto layout_corner_case_1 = blueprints::optimization_layout_corner_case_outputs_1<gate_layout>();
        wiring_reduction_stats stats_corner_case_1{};
        wiring_reduction<gate_layout>(layout_corner_case_1, {}, &stats_corner_case_1);
        check_eq(blueprints::optimization_layout_corner_case_outputs_1<gate_layout>(), layout_corner_case_1);

        const auto layout_corner_case_2 = blueprints::optimization_layout_corner_case_outputs_2<gate_layout>();
        wiring_reduction_stats stats_corner_case_2{};
        wiring_reduction<gate_layout>(layout_corner_case_2, {}, &stats_corner_case_2);
        check_eq(blueprints::optimization_layout_corner_case_outputs_2<gate_layout>(), layout_corner_case_2);

        const auto             layout_corner_case_3 = blueprints::optimization_layout_corner_case_inputs<gate_layout>();
        wiring_reduction_stats stats_corner_case_3{};
        wiring_reduction<gate_layout>(layout_corner_case_3, {}, &stats_corner_case_3);
        check_eq(blueprints::optimization_layout_corner_case_inputs<gate_layout>(), layout_corner_case_3);
    }

    SECTION("Timeout")
    {
        using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<>>>>;

        const auto layout = orthogonal<gate_layout>(blueprints::mux21_network<technology_network>(), {});

        wiring_reduction_stats  stats{};
        wiring_reduction_params params{};
        params.timeout = 1000000;
        wiring_reduction<gate_layout>(layout, params, &stats);

        check_eq(blueprints::mux21_network<technology_network>(), layout);
    }

    SECTION("Timeout exceeded")
    {
        using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<>>>>;

        const auto layout = orthogonal<gate_layout>(blueprints::mux21_network<technology_network>(), {});

        wiring_reduction_stats  stats{};
        wiring_reduction_params params{};
        params.timeout = 0;
        wiring_reduction<gate_layout>(layout, params, &stats);

        check_eq(blueprints::mux21_network<technology_network>(), layout);
        CHECK(stats.area_improvement == 0);
    }
}

TEST_CASE("Wrong clocking scheme", "[wiring_reduction]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<>>>>;

    const auto layout    = blueprints::use_and_gate_layout<gate_layout>();
    auto       obstr_lyt = obstruction_layout<gate_layout>(layout);

    SECTION("Call functions")
    {
        CHECK_NOTHROW(detail::create_wiring_reduction_layout(obstr_lyt, 1, 1, detail::search_direction::HORIZONTAL));

        wiring_reduction_stats stats_wrong_clocking_scheme{};

        CHECK_NOTHROW(wiring_reduction<gate_layout>(obstr_lyt, {}, &stats_wrong_clocking_scheme));
    }
}

TEST_CASE("Search Direction", "[wiring_reduction]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<>>>>;

    const auto layout    = blueprints::straight_wire_gate_layout<gate_layout>();
    auto       obstr_lyt = obstruction_layout<gate_layout>(layout);

    SECTION("Get")
    {
        auto lyt = detail::create_wiring_reduction_layout(obstr_lyt, 1, 1, detail::search_direction::HORIZONTAL);
        CHECK(lyt.get_search_direction() == detail::search_direction::HORIZONTAL);
    }
}
