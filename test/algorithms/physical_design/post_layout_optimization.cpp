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

TEST_CASE("Optimization steps", "[post_layout_optimization]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<>>>>;
    using coord_path  = layout_coordinate_path<obstruction_layout<gate_layout>>;

    const auto layout    = blueprints::optimization_layout<gate_layout>();
    auto       obstr_lyt = obstruction_layout<gate_layout>(layout);
    // I ▢ I
    // ↓   ↓
    // = ▢ =
    // ↓   ↓
    // F→=→&
    // ↓   ↓
    // O ▢ O
    SECTION("Get fanin and fanouts 1")
    {
        const coordinate<gate_layout> old_pos = {0, 2};

        const auto& [fanins, fanouts, to_clear, r1, r2, r3, r4] = detail::get_fanin_and_fanouts(obstr_lyt, old_pos);

        CHECK(fanins == std::vector<coordinate<gate_layout>>{{0, 0}});
        CHECK(fanouts == std::vector<coordinate<gate_layout>>{{2, 2}, {0, 3}});
        CHECK(to_clear == std::vector<coordinate<gate_layout>>{{0, 1}, {1, 2}});

        CHECK(r1 == coord_path{{0, 0}, {0, 1}, {0, 2}});
        CHECK(r2.empty());
        CHECK(r3 == coord_path{{0, 2}, {1, 2}, {2, 2}});
        CHECK(r4 == coord_path{{0, 2}, {0, 3}});
    }

    SECTION("Get fanin and fanouts 2")
    {
        const coordinate<gate_layout> old_pos = {2, 2};

        const auto& [fanins, fanouts, to_clear, r1, r2, r3, r4] = detail::get_fanin_and_fanouts(obstr_lyt, old_pos);

        CHECK(fanins == std::vector<coordinate<gate_layout>>{{0, 2}, {2, 0}});
        CHECK(fanouts == std::vector<coordinate<gate_layout>>{{2, 3}});
        CHECK(to_clear == std::vector<coordinate<gate_layout>>{{1, 2}, {2, 1}});

        CHECK(r1 == coord_path{{0, 2}, {1, 2}, {2, 2}});
        CHECK(r2 == coord_path{{2, 0}, {2, 1}, {2, 2}});
        CHECK(r3 == coord_path{{2, 2}, {2, 3}});
        CHECK(r4.empty());
    }

    const coordinate<gate_layout> old_pos_1 = {2, 0};
    const coordinate<gate_layout> new_pos_1 = {1, 0};

    const auto moved_gate_1 =
        detail::improve_gate_location(obstr_lyt, old_pos_1, {2, 2}, (obstr_lyt.x() + 1) * (obstr_lyt.y() + 1));
    // I I→=
    // ↓   ↓
    // = ▢ =
    // ↓   ↓
    // F→=→&
    // ↓   ↓
    // O ▢ O
    SECTION("Move gates 1")
    {
        CHECK(moved_gate_1);
        CHECK(obstr_lyt.is_pi_tile(new_pos_1));
        CHECK_FALSE(obstr_lyt.is_pi_tile(old_pos_1));
    }

    const coordinate<gate_layout> old_pos_2 = {0, 2};
    const coordinate<gate_layout> new_pos_2 = {0, 1};

    const auto moved_gate_2 =
        detail::improve_gate_location(obstr_lyt, old_pos_2, {2, 2}, (obstr_lyt.x() + 1) * (obstr_lyt.y() + 1));
    // I I→=
    // ↓   ↓
    // F→= =
    // ↓ ↓ ↓
    // = =→&
    // ↓   ↓
    // O ▢ O
    SECTION("Move gates 2")
    {
        CHECK(moved_gate_2);
        CHECK(obstr_lyt.fanout_size(obstr_lyt.get_node(old_pos_2)) == 1);
        CHECK(obstr_lyt.fanout_size(obstr_lyt.get_node(new_pos_2)) == 2);
    }

    const coordinate<gate_layout> old_pos_3 = {2, 2};
    const coordinate<gate_layout> new_pos_3 = {1, 1};

    const auto moved_gate_3 =
        detail::improve_gate_location(obstr_lyt, old_pos_3, {2, 2}, (obstr_lyt.x() + 1) * (obstr_lyt.y() + 1));
    // I I ▢
    // ↓ ↓
    // F→&→=
    // ↓   ↓
    // = ▢ =
    // ↓   ↓
    // O ▢ O
    SECTION("Move gates 3")
    {
        CHECK(moved_gate_3);
        CHECK(obstr_lyt.is_and(obstr_lyt.get_node(new_pos_3)));
        CHECK_FALSE(obstr_lyt.is_and(obstr_lyt.get_node(old_pos_3)));
    }

    const coordinate<gate_layout> old_pos_4 = {0, 3};
    const coordinate<gate_layout> new_pos_4 = {0, 2};

    const auto moved_gate_4 =
        detail::improve_gate_location(obstr_lyt, old_pos_4, {1, 1}, (obstr_lyt.x() + 1) * (obstr_lyt.y() + 1));
    // I I ▢
    // ↓ ↓
    // F→&→=
    // ↓   ↓
    // O ▢ =
    //     ↓
    // ▢ ▢ O
    SECTION("Move gates 4")
    {
        CHECK(moved_gate_4);
        CHECK(obstr_lyt.is_po(obstr_lyt.get_node(new_pos_4)));
        CHECK_FALSE(obstr_lyt.is_po(obstr_lyt.get_node(old_pos_4)));
    }

    const coordinate<gate_layout> old_pos_5 = {2, 3};
    const coordinate<gate_layout> new_pos_5 = {1, 2};

    const auto moved_gate_5 =
        detail::improve_gate_location(obstr_lyt, old_pos_5, {1, 1}, (obstr_lyt.x() + 1) * (obstr_lyt.y() + 1));
    // I I ▢
    // ↓ ↓
    // F→& ▢
    // ↓ ↓
    // O O ▢
    //
    // ▢ ▢ ▢
    SECTION("Move gates 5")
    {
        CHECK(moved_gate_5);
        CHECK(obstr_lyt.is_po(obstr_lyt.get_node(new_pos_5)));
        CHECK_FALSE(obstr_lyt.is_po(obstr_lyt.get_node(old_pos_5)));
    }

    SECTION("Optimized layout")
    {
        CHECK(obstr_lyt.is_pi_tile(coordinate<gate_layout>{0, 0}));
        CHECK(obstr_lyt.is_pi_tile(coordinate<gate_layout>{1, 0}));
        CHECK(obstr_lyt.is_buf(obstr_lyt.get_node(coordinate<gate_layout>{0, 1})));
        CHECK(obstr_lyt.is_and(obstr_lyt.get_node(coordinate<gate_layout>{1, 1})));
        CHECK(obstr_lyt.is_po_tile(coordinate<gate_layout>{0, 2}));
        CHECK(obstr_lyt.is_po_tile(coordinate<gate_layout>{1, 2}));
    }
}

TEST_CASE("Wrong clocking scheme", "[post_layout_optimization]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<>>>>;

    const auto layout    = blueprints::use_and_gate_layout<gate_layout>();
    auto       obstr_lyt = obstruction_layout<gate_layout>(layout);

    SECTION("Call functions")
    {
        const coordinate<gate_layout> old_pos_1 = {2, 0};

        const auto moved_gate_1 =
            detail::improve_gate_location(obstr_lyt, old_pos_1, {0, 0}, (obstr_lyt.x() + 1) * (obstr_lyt.y() + 1));

        CHECK_FALSE(moved_gate_1);

        post_layout_optimization_stats stats_wrong_clocking_scheme{};

        CHECK_NOTHROW(post_layout_optimization<gate_layout>(obstr_lyt, {}, &stats_wrong_clocking_scheme));
    }
}
