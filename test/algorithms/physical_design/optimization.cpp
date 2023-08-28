//
// Created by Simon Hofmann on 31.07.23.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/algorithms/physical_design/optimization.hpp>
#include <fiction/algorithms/physical_design/orthogonal.hpp>
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
void check_mapping_equiv(const Ntk& ntk)
{
    auto layout = orthogonal<Lyt>(ntk, {});
    post_layout_optimization<Lyt>(layout);

    check_eq(ntk, layout);
}

template <typename Lyt>
void check_mapping_equiv_all()
{
    check_mapping_equiv<Lyt>(blueprints::maj1_network<mockturtle::aig_network>());
    check_mapping_equiv<Lyt>(blueprints::maj4_network<mockturtle::aig_network>());
    check_mapping_equiv<Lyt>(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>());
    check_mapping_equiv<Lyt>(blueprints::and_or_network<technology_network>());
    check_mapping_equiv<Lyt>(blueprints::nary_operation_network<technology_network>());
    check_mapping_equiv<Lyt>(blueprints::constant_gate_input_maj_network<technology_network>());
    check_mapping_equiv<Lyt>(blueprints::half_adder_network<technology_network>());
    check_mapping_equiv<Lyt>(blueprints::full_adder_network<technology_network>());
    check_mapping_equiv<Lyt>(blueprints::mux21_network<technology_network>());
    check_mapping_equiv<Lyt>(blueprints::se_coloring_corner_case_network<technology_network>());
    // check_mapping_equiv<Lyt>(blueprints::fanout_substitution_corner_case_network<technology_network>());
    check_mapping_equiv<Lyt>(blueprints::inverter_network<technology_network>());
    check_mapping_equiv<Lyt>(blueprints::clpl<technology_network>());
    check_mapping_equiv<Lyt>(blueprints::one_to_five_path_difference_network<technology_network>());
    check_mapping_equiv<Lyt>(blueprints::nand_xnor_network<technology_network>());
}

TEST_CASE("Layout equivalence", "[optimization]")
{
    SECTION("Cartesian layouts")
    {
        using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

        check_mapping_equiv_all<gate_layout>();
    }

    SECTION("Corner Cases")
    {
        using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<>>>>;

        const auto layout_corner_case_1 = blueprints::optimization_layout_corner_case_outputs_1<gate_layout>();
        post_layout_optimization<gate_layout>(layout_corner_case_1);
        check_eq(blueprints::optimization_layout_corner_case_outputs_1<gate_layout>(), layout_corner_case_1);

        const auto layout_corner_case_2 = blueprints::optimization_layout_corner_case_outputs_2<gate_layout>();
        post_layout_optimization<gate_layout>(layout_corner_case_2);
        check_eq(blueprints::optimization_layout_corner_case_outputs_2<gate_layout>(), layout_corner_case_2);

        const auto layout_corner_case_3 = blueprints::optimization_layout_corner_case_wiring<gate_layout>();
        post_layout_optimization<gate_layout>(layout_corner_case_3);
        check_eq(blueprints::optimization_layout_corner_case_wiring<gate_layout>(), layout_corner_case_3);
    }
}

TEST_CASE("Optimization steps", "[optimization]")
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
        const coordinate<gate_layout> old_pos                   = {0, 2};
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
        const coordinate<gate_layout> old_pos                  = {2, 2};
        const auto [fanins, fanouts, to_clear, r1, r2, r3, r4] = detail::get_fanin_and_fanouts(obstr_lyt, old_pos);

        CHECK(fanins == std::vector<coordinate<gate_layout>>{{0, 2}, {2, 0}});
        CHECK(fanouts == std::vector<coordinate<gate_layout>>{{2, 3}});
        CHECK(to_clear == std::vector<coordinate<gate_layout>>{{1, 2}, {2, 1}});

        CHECK(r1 == coord_path{{0, 2}, {1, 2}, {2, 2}});
        CHECK(r2 == coord_path{{2, 0}, {2, 1}, {2, 2}});
        CHECK(r3 == coord_path{{2, 2}, {2, 3}});
        CHECK(r4.empty());
    }

    const coordinate<gate_layout>                   old_pos_1    = {2, 0};
    const coordinate<gate_layout>                   new_pos_1    = {1, 0};
    const std::tuple<bool, coordinate<gate_layout>> moved_gate_1 = detail::improve_gate_location(old_pos_1, obstr_lyt);
    // I I→=
    // ↓   ↓
    // = ▢ =
    // ↓   ↓
    // F→=→&
    // ↓   ↓
    // O ▢ O
    SECTION("Move gates 1")
    {
        CHECK(std::get<0>(moved_gate_1) == true);
        CHECK(std::get<1>(moved_gate_1) == new_pos_1);
        CHECK(obstr_lyt.is_pi_tile(new_pos_1) == true);
        CHECK(obstr_lyt.is_pi_tile(old_pos_1) == false);
    }

    const coordinate<gate_layout>                   old_pos_2    = {0, 2};
    const coordinate<gate_layout>                   new_pos_2    = {0, 1};
    const std::tuple<bool, coordinate<gate_layout>> moved_gate_2 = detail::improve_gate_location(old_pos_2, obstr_lyt);
    // I I→=
    // ↓   ↓
    // F→= =
    // ↓ ↓ ↓
    // = =→&
    // ↓   ↓
    // O ▢ O
    SECTION("Move gates 2")
    {
        CHECK(std::get<0>(moved_gate_2) == true);
        CHECK(std::get<1>(moved_gate_2) == new_pos_2);
        CHECK(obstr_lyt.fanout_size(obstr_lyt.get_node(old_pos_2)) == 1);
        CHECK(obstr_lyt.fanout_size(obstr_lyt.get_node(new_pos_2)) == 2);
    }

    const coordinate<gate_layout>                   old_pos_3    = {2, 2};
    const coordinate<gate_layout>                   new_pos_3    = {1, 1};
    const std::tuple<bool, coordinate<gate_layout>> moved_gate_3 = detail::improve_gate_location(old_pos_3, obstr_lyt);
    // I I ▢
    // ↓ ↓
    // F→&→=
    // ↓   ↓
    // = ▢ =
    // ↓   ↓
    // O ▢ O
    SECTION("Move gates 3")
    {
        CHECK(std::get<0>(moved_gate_3) == true);
        CHECK(std::get<1>(moved_gate_3) == new_pos_3);
        CHECK(obstr_lyt.is_and(obstr_lyt.get_node(new_pos_3)) == true);
        CHECK(obstr_lyt.is_and(obstr_lyt.get_node(old_pos_3)) == false);
    }

    detail::delete_wires(obstr_lyt);
    // I I ▢
    // ↓ ↓
    // F→&→=
    // ↓   ↓
    // O ▢ O
    //
    // ▢ ▢ ▢
    SECTION("Delete wires")
    {
        CHECK(obstr_lyt.is_po_tile(coordinate<gate_layout>{0, 2}) == true);
        CHECK(obstr_lyt.is_po_tile(coordinate<gate_layout>{0, 3}) == false);
        CHECK(obstr_lyt.is_po_tile(coordinate<gate_layout>{2, 2}) == true);
        CHECK(obstr_lyt.is_po_tile(coordinate<gate_layout>{2, 3}) == false);
    }

    detail::optimize_output_positions(obstr_lyt);
    // I I ▢
    // ↓ ↓
    // F→&→O
    // ↓
    // O ▢ ▢
    //
    // ▢ ▢ ▢
    SECTION("Optimize outputs")
    {
        CHECK(obstr_lyt.is_po_tile(coordinate<gate_layout>{0, 2}) == true);
        CHECK(obstr_lyt.is_po_tile(coordinate<gate_layout>{1, 2}) == false);
        CHECK(obstr_lyt.is_po_tile(coordinate<gate_layout>{2, 2}) == false);
        CHECK(obstr_lyt.is_po_tile(coordinate<gate_layout>{2, 1}) == true);
    }

    SECTION("Optimized layout")
    {
        CHECK(obstr_lyt.is_pi_tile(coordinate<gate_layout>{0, 0}) == true);
        CHECK(obstr_lyt.is_pi_tile(coordinate<gate_layout>{1, 0}) == true);
        CHECK(obstr_lyt.is_buf(obstr_lyt.get_node(coordinate<gate_layout>{0, 1})) == true);
        CHECK(obstr_lyt.is_and(obstr_lyt.get_node(coordinate<gate_layout>{1, 1})) == true);
        CHECK(obstr_lyt.is_po_tile(coordinate<gate_layout>{0, 2}) == true);
        CHECK(obstr_lyt.is_po_tile(coordinate<gate_layout>{2, 1}) == true);
    }
}
