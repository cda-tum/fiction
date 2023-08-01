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

using namespace fiction;

template <typename Lyt, typename Ntk>
void check_mapping_equiv(const Ntk& ntk)
{
    const auto layout     = orthogonal<Lyt>(ntk, {});
    const auto opt_layout = optimize<Lyt>(layout);

    check_eq(ntk, layout);
    check_eq(ntk, opt_layout);
    check_eq(layout, opt_layout);
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
    //check_mapping_equiv<Lyt>(blueprints::fanout_substitution_corner_case_network<technology_network>());
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
}

TEST_CASE("Optimization steps", "[optimization]")
{
    using coordinate  = offset::ucoord_t;
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coordinate>>>>;
    using coord_path  = layout_coordinate_path<fiction::detail::obs_gate_lyt>;

    gate_layout                   layout = blueprints::optimization_layout<gate_layout>();
    fiction::detail::obs_gate_lyt lyt    = fiction::obstruction_layout<gate_layout>(layout);
    // I ▢ I
    // ↓   ↓
    // = ▢ =
    // ↓   ↓
    // F→=→&
    // ↓   ↓
    // O ▢ O
    SECTION("Get fanin and fanouts 1")
    {
        coordinate old_pos                               = {0, 2};
        auto [fanins, fanouts, to_clear, r1, r2, r3, r4] = fiction::detail::get_fanin_and_fanouts(lyt, old_pos);
        CHECK(fanins == std::vector<coordinate>{{0, 0}});
        CHECK(fanouts == std::vector<coordinate>{{2, 2}, {0, 3}});
        CHECK(to_clear == std::vector<coordinate>{{0, 1}, {1, 2}});
        CHECK(r1 == coord_path{{0, 0}, {0, 1}, {0, 2}});
        CHECK(r2 == coord_path{});
        CHECK(r3 == coord_path{{0, 2}, {1, 2}, {2, 2}});
        CHECK(r4 == coord_path{{0, 2}, {0, 3}});
    }

    SECTION("Get fanin and fanouts 2")
    {
        coordinate old_pos                               = {2, 2};
        auto [fanins, fanouts, to_clear, r1, r2, r3, r4] = fiction::detail::get_fanin_and_fanouts(lyt, old_pos);
        CHECK(fanins == std::vector<coordinate>{{0, 2}, {2, 0}});
        CHECK(fanouts == std::vector<coordinate>{{2, 3}});
        CHECK(to_clear == std::vector<coordinate>{{1, 2}, {2, 1}});
        CHECK(r1 == coord_path{{0, 2}, {1, 2}, {2, 2}});
        CHECK(r2 == coord_path{{2, 0}, {2, 1}, {2, 2}});
        CHECK(r3 == coord_path{{2, 2}, {2, 3}});
        CHECK(r4 == coord_path{});
    }

    // calculate bounding box
    const auto bounding_box_before = bounding_box_2d(lyt);
    const auto width               = bounding_box_before.get_x_size();
    const auto height              = bounding_box_before.get_y_size();

    coordinate                           old_pos_1 = {2, 0};
    coordinate                           new_pos_1 = {1, 0};
    std::tuple<bool, detail::coordinate> moved_gate_1 =
        fiction::detail::move_gate(old_pos_1, lyt, static_cast<int>(width), static_cast<int>(height));
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
        CHECK(lyt.is_pi_tile(new_pos_1) == true);
        CHECK(lyt.is_pi_tile(old_pos_1) == false);
    }

    coordinate                           old_pos_2 = {0, 2};
    coordinate                           new_pos_2 = {0, 1};
    std::tuple<bool, detail::coordinate> moved_gate_2 =
        fiction::detail::move_gate(old_pos_2, lyt, static_cast<int>(width), static_cast<int>(height));
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
        CHECK(lyt.fanout_size(lyt.get_node(old_pos_2)) == 1);
        CHECK(lyt.fanout_size(lyt.get_node(new_pos_2)) == 2);
    }

    coordinate                           old_pos_3 = {2, 2};
    coordinate                           new_pos_3 = {1, 1};
    std::tuple<bool, detail::coordinate> moved_gate_3 =
        fiction::detail::move_gate(old_pos_3, lyt, static_cast<int>(width), static_cast<int>(height));
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
        CHECK(lyt.is_and(lyt.get_node(new_pos_3)) == true);
        CHECK(lyt.is_and(lyt.get_node(old_pos_3)) == false);
    }

    detail::delete_wires(lyt, static_cast<int>(width), static_cast<int>(height));
    // I I ▢
    // ↓ ↓
    // F→&→=
    // ↓   ↓
    // O ▢ O
    //
    // ▢ ▢ ▢
    SECTION("Delete wires")
    {
        CHECK(lyt.is_po_tile(coordinate{0, 2}) == true);
        CHECK(lyt.is_po_tile(coordinate{0, 3}) == false);
        CHECK(lyt.is_po_tile(coordinate{2, 2}) == true);
        CHECK(lyt.is_po_tile(coordinate{2, 3}) == false);
    }

    detail::optimize_output(lyt);
    // I I ▢
    // ↓ ↓
    // F→&→O
    // ↓
    // O ▢ ▢
    //
    // ▢ ▢ ▢
    SECTION("Optimize outputs")
    {
        CHECK(lyt.is_po_tile(coordinate{0, 2}) == true);
        CHECK(lyt.is_po_tile(coordinate{1, 2}) == false);
        CHECK(lyt.is_po_tile(coordinate{2, 2}) == false);
        CHECK(lyt.is_po_tile(coordinate{2, 1}) == true);
    }

    SECTION("Optimized layout")
    {
        CHECK(lyt.is_pi_tile(coordinate{0, 0}) == true);
        CHECK(lyt.is_pi_tile(coordinate{1, 0}) == true);
        CHECK(lyt.is_buf(lyt.get_node(coordinate{0, 1})) == true);
        CHECK(lyt.is_and(lyt.get_node(coordinate{1, 1})) == true);
        CHECK(lyt.is_po_tile(coordinate{0, 2}) == true);
        CHECK(lyt.is_po_tile(coordinate{2, 1}) == true);
    }
}
