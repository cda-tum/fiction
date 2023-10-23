//
// Created by Simon Hofmann on 26.09.23.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/algorithms/physical_design/orthogonal.hpp>
#include <fiction/io/read_fgl_layout.hpp>
#include <fiction/io/write_fgl_layout.hpp>
#include <fiction/layouts/bounding_box.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/views/names_view.hpp>

#include <sstream>

using namespace fiction;

template <typename WLyt, typename RLyt>
void compare_written_and_read_layout(const WLyt& wlyt, const RLyt& rlyt) noexcept
{
    CHECK(wlyt.get_layout_name() == rlyt.get_layout_name());

    const bounding_box_2d<WLyt> wbb{wlyt};
    const bounding_box_2d<RLyt> rbb{wlyt};

    CHECK(wbb.get_min() == rbb.get_min());
    CHECK(wbb.get_max() == rbb.get_max());

    CHECK(wbb.get_x_size() == rbb.get_x_size());
    CHECK(wbb.get_y_size() == rbb.get_y_size());

    CHECK(wlyt.num_pis() == rlyt.num_pis());
    CHECK(wlyt.num_pos() == rlyt.num_pos());
}

template <typename Lyt, typename Ntk>
void check_parsing_equiv(const Ntk& ntk)
{
    const auto layout = orthogonal<Lyt>(ntk, {});

    std::stringstream layout_stream{};
    write_fgl_layout(layout, layout_stream);

    const auto read_layout = read_fgl_layout<Lyt>(layout_stream, get_name(layout));

    compare_written_and_read_layout(layout, read_layout);

    check_eq(ntk, layout);
    check_eq(ntk, read_layout);
    check_eq(layout, read_layout);
}

template <typename Lyt>
void check_parsing_equiv_layout(const Lyt& lyt)
{
    std::stringstream layout_stream{};
    write_fgl_layout(lyt, layout_stream);
    const auto read_layout = read_fgl_layout<Lyt>(layout_stream, lyt.get_layout_name());

    compare_written_and_read_layout(lyt, read_layout);

    check_eq(lyt, read_layout);
    CHECK(lyt.get_layout_name() == read_layout.get_layout_name());
}

template <typename Lyt>
void check_parsing_equiv_all()
{
    check_parsing_equiv<Lyt>(blueprints::maj1_network<mockturtle::aig_network>());
    check_parsing_equiv<Lyt>(blueprints::maj4_network<mockturtle::aig_network>());
    check_parsing_equiv<Lyt>(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>());
    check_parsing_equiv<Lyt>(blueprints::and_or_network<technology_network>());
    check_parsing_equiv<Lyt>(blueprints::nary_operation_network<technology_network>());
    check_parsing_equiv<Lyt>(blueprints::constant_gate_input_maj_network<technology_network>());
    check_parsing_equiv<Lyt>(blueprints::half_adder_network<technology_network>());
    check_parsing_equiv<Lyt>(blueprints::full_adder_network<technology_network>());
    check_parsing_equiv<Lyt>(blueprints::mux21_network<technology_network>());
    check_parsing_equiv<Lyt>(blueprints::se_coloring_corner_case_network<technology_network>());
    check_parsing_equiv<Lyt>(blueprints::fanout_substitution_corner_case_network<technology_network>());
    check_parsing_equiv<Lyt>(blueprints::inverter_network<technology_network>());
    check_parsing_equiv<Lyt>(blueprints::clpl<technology_network>());
    check_parsing_equiv<Lyt>(blueprints::one_to_five_path_difference_network<technology_network>());
    check_parsing_equiv<Lyt>(blueprints::nand_xnor_network<technology_network>());
}

void check_parsing_equiv_layout_all()
{
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::straight_wire_gate_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::three_wire_paths_gate_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::xor_maj_gate_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::single_input_tautology_gate_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::tautology_gate_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::and_or_gate_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_odd_row_gate_clk_lyt>(blueprints::and_or_gate_layout<cart_odd_row_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_even_row_gate_clk_lyt>(
        blueprints::and_or_gate_layout<cart_even_row_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_odd_col_gate_clk_lyt>(blueprints::and_or_gate_layout<cart_odd_col_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_even_col_gate_clk_lyt>(
        blueprints::and_or_gate_layout<cart_even_col_gate_clk_lyt>());
    check_parsing_equiv_layout<hex_odd_row_gate_clk_lyt>(blueprints::and_or_gate_layout<hex_odd_row_gate_clk_lyt>());
    check_parsing_equiv_layout<hex_even_row_gate_clk_lyt>(blueprints::and_or_gate_layout<hex_even_row_gate_clk_lyt>());
    check_parsing_equiv_layout<hex_odd_col_gate_clk_lyt>(blueprints::and_or_gate_layout<hex_odd_col_gate_clk_lyt>());
    check_parsing_equiv_layout<hex_even_col_gate_clk_lyt>(blueprints::and_or_gate_layout<hex_even_col_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::and_not_gate_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::or_not_gate_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::use_and_gate_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::res_maj_gate_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::res_tautology_gate_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<hex_even_row_gate_clk_lyt>(
        blueprints::open_tautology_gate_layout<hex_even_row_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::crossing_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::fanout_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::unbalanced_and_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_odd_col_gate_clk_lyt>(
        blueprints::shifted_cart_and_or_inv_gate_layout<cart_odd_col_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_even_row_gate_clk_lyt>(
        blueprints::row_clocked_and_xor_gate_layout<cart_even_row_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::optimization_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(
        blueprints::optimization_layout_corner_case_outputs_1<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(
        blueprints::optimization_layout_corner_case_outputs_2<cart_gate_clk_lyt>());
}

TEST_CASE("Write empty gate_level layout", "[write-fgl-layout]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    const gate_layout layout{{}, "empty"};

    std::stringstream layout_stream{};
    write_fgl_layout(layout, layout_stream);
    const auto read_layout = read_fgl_layout<gate_layout>(layout_stream, "empty");

    compare_written_and_read_layout(layout, read_layout);
}

TEST_CASE("Write and read layouts", "[write-fgl-layout]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    check_parsing_equiv_all<gate_layout>();
    check_parsing_equiv_layout_all();
}
