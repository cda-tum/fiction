//
// Created by Simon Hofmann on 31.07.23.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/algorithms/physical_design/orthogonal.hpp>
#include <fiction/algorithms/physical_design/post_layout_optimization.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/obstruction_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/utils/routing_utils.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/views/names_view.hpp>

#include <chrono>
#include <cstdint>
#include <vector>

using namespace fiction;

template <typename Lyt, typename Ntk>
void check_layout_equiv(const Ntk& ntk)
{
    const auto layout = orthogonal<Lyt>(ntk, {});

    post_layout_optimization_stats stats{};
    post_layout_optimization<Lyt>(layout, {}, &stats);

    check_eq(ntk, layout);

    CHECK(mockturtle::to_seconds(stats.time_total) > 0);
}

template <typename Lyt>
void check_layout_equiv_all()
{
    SECTION("maj1_network")
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
    SECTION("inverter_network")
    {
        check_layout_equiv<Lyt>(blueprints::inverter_network<technology_network>());
    }
    SECTION("clpl")
    {
        check_layout_equiv<Lyt>(blueprints::clpl<technology_network>());
    }
    SECTION("fanout_substitution_corner_case_network")
    {
        check_layout_equiv<Lyt>(blueprints::fanout_substitution_corner_case_network<technology_network>());
    }
    SECTION("nand_xnor_network")
    {
        check_layout_equiv<Lyt>(blueprints::nand_xnor_network<technology_network>());
    }
}

TEST_CASE("Layout equivalence", "[post_layout_optimization]")
{
    SECTION("Cartesian layouts")
    {
        using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

        check_layout_equiv_all<gate_layout>();
    }

    SECTION("Corner cases")
    {
        using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<>>>>;

        SECTION("optimization_layout_corner_case_outputs_1")
        {
            const auto layout_corner_case_1 = blueprints::optimization_layout_corner_case_outputs_1<gate_layout>();
            post_layout_optimization_stats stats_corner_case_1{};
            post_layout_optimization<gate_layout>(layout_corner_case_1, {}, &stats_corner_case_1);
            check_eq(blueprints::optimization_layout_corner_case_outputs_1<gate_layout>(), layout_corner_case_1);
        }

        SECTION("optimization_layout_corner_case_outputs_2")
        {
            const auto layout_corner_case_2 = blueprints::optimization_layout_corner_case_outputs_2<gate_layout>();
            post_layout_optimization_stats stats_corner_case_2{};
            post_layout_optimization<gate_layout>(layout_corner_case_2, {}, &stats_corner_case_2);
            check_eq(blueprints::optimization_layout_corner_case_outputs_2<gate_layout>(), layout_corner_case_2);
        }
    }

    SECTION("Maximum gate relocations")
    {
        using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<>>>>;

        for (int64_t max_gate_relocations = 0; max_gate_relocations < 10; max_gate_relocations++)
        {
            const auto layout = orthogonal<gate_layout>(blueprints::mux21_network<technology_network>(), {});

            post_layout_optimization_stats  stats{};
            post_layout_optimization_params params{};
            params.max_gate_relocations = max_gate_relocations;
            post_layout_optimization<gate_layout>(layout, params, &stats);

            check_eq(blueprints::mux21_network<technology_network>(), layout);
        }
    }

    SECTION("Optimize POs only")
    {
        using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<>>>>;

        const auto layout = orthogonal<gate_layout>(blueprints::mux21_network<technology_network>(), {});

        post_layout_optimization_stats  stats{};
        post_layout_optimization_params params{};
        params.optimize_pos_only = true;
        post_layout_optimization<gate_layout>(layout, params, &stats);

        check_eq(blueprints::mux21_network<technology_network>(), layout);
    }

    SECTION("Timeout")
    {
        using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<>>>>;

        const auto layout = orthogonal<gate_layout>(blueprints::mux21_network<technology_network>(), {});

        post_layout_optimization_stats  stats{};
        post_layout_optimization_params params{};
        params.timeout = 1000000;
        post_layout_optimization<gate_layout>(layout, params, &stats);

        check_eq(blueprints::mux21_network<technology_network>(), layout);
    }

    SECTION("Timeout exceeded")
    {
        using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<>>>>;

        const auto layout = orthogonal<gate_layout>(blueprints::mux21_network<technology_network>(), {});

        post_layout_optimization_stats  stats{};
        post_layout_optimization_params params{};
        params.timeout = 0;
        post_layout_optimization<gate_layout>(layout, params, &stats);

        check_eq(blueprints::mux21_network<technology_network>(), layout);
        CHECK(stats.area_improvement == 0);
    }

    SECTION("Planar optimization with planar layout")
    {
        using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<>>>>;

        const auto layout = blueprints::planar_unoptimized_layout<gate_layout>();

        post_layout_optimization_stats  stats{};
        post_layout_optimization_params params{};
        params.planar_optimization = true;
        post_layout_optimization<gate_layout>(layout, params, &stats);

        check_eq(blueprints::planar_unoptimized_layout<gate_layout>(), layout);
        CHECK(layout.z() == 0);
    }

    SECTION("Planar optimization with crossing layout")
    {
        using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<>>>>;

        const auto layout = blueprints::planar_optimization_layout<gate_layout>();

        post_layout_optimization_stats  stats{};
        post_layout_optimization_params params{};

        params.planar_optimization = true;
        post_layout_optimization<gate_layout>(layout, params, &stats);
        CHECK(!layout.is_inv(layout.get_node({1, 0})));

        params.planar_optimization = false;
        post_layout_optimization<gate_layout>(layout, params, &stats);
        CHECK(layout.is_inv(layout.get_node({1, 0})));
    }
}

TEST_CASE("Wrong clocking scheme", "[post_layout_optimization]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<>>>>;

    const auto layout    = blueprints::use_and_gate_layout<gate_layout>();
    auto       obstr_lyt = obstruction_layout<gate_layout>(layout);

    SECTION("Call functions")
    {
        post_layout_optimization_stats stats_wrong_clocking_scheme{};

        CHECK_NOTHROW(post_layout_optimization<gate_layout>(obstr_lyt, {}, &stats_wrong_clocking_scheme));
    }
}
