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
#include <mockturtle/views/names_view.hpp>

#include <tuple>
#include <vector>

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
    check_layout_equiv<Lyt>(blueprints::maj1_network<mockturtle::aig_network>());
    check_layout_equiv<Lyt>(blueprints::maj4_network<mockturtle::aig_network>());
    check_layout_equiv<Lyt>(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>());
    check_layout_equiv<Lyt>(blueprints::and_or_network<technology_network>());
    check_layout_equiv<Lyt>(blueprints::nary_operation_network<technology_network>());
    check_layout_equiv<Lyt>(blueprints::constant_gate_input_maj_network<technology_network>());
    check_layout_equiv<Lyt>(blueprints::half_adder_network<technology_network>());
    check_layout_equiv<Lyt>(blueprints::full_adder_network<technology_network>());
    check_layout_equiv<Lyt>(blueprints::mux21_network<technology_network>());
    check_layout_equiv<Lyt>(blueprints::se_coloring_corner_case_network<technology_network>());
    // check_layout_equiv<Lyt>(blueprints::fanout_substitution_corner_case_network<technology_network>());
    // check_layout_equiv<Lyt>(blueprints::inverter_network<technology_network>());
    check_layout_equiv<Lyt>(blueprints::clpl<technology_network>());
    // check_layout_equiv<Lyt>(blueprints::one_to_five_path_difference_network<technology_network>());
    check_layout_equiv<Lyt>(blueprints::nand_xnor_network<technology_network>());
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
    }
}