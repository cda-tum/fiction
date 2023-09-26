//
// Created by Simon Hofmann on 26.09.23.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/algorithms/physical_design/orthogonal.hpp>
#include <fiction/io/read_gate_level_layout.hpp>
#include <fiction/io/write_gate_level_layout.hpp>
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

#include <map>
#include <sstream>
#include <string>

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
    write_gate_level_layout(layout, layout_stream);

    std::cout << layout_stream.str();
    const auto read_layout = read_gate_level_layout<Lyt>(layout_stream, get_name(layout));

    compare_written_and_read_layout(layout, read_layout);

    check_eq(ntk, layout);
    check_eq(ntk, read_layout);
    check_eq(layout, read_layout);
}

template <typename Lyt>
void check_parsing_equiv_layout(const Lyt& lyt)
{
    std::stringstream layout_stream{};
    write_gate_level_layout(lyt, layout_stream);
    const auto read_layout = read_gate_level_layout<Lyt>(layout_stream, lyt.get_layout_name());

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

    check_parsing_equiv_layout(blueprints::straight_wire_gate_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout(blueprints::or_not_gate_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout(blueprints::crossing_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout(blueprints::tautology_gate_layout<cart_gate_clk_lyt>());
}

TEST_CASE("Write empty gate_level layout", "[sqd]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    const gate_layout layout{{}, "empty"};

    std::stringstream layout_stream{};

    write_gate_level_layout(layout, layout_stream);

    const auto read_layout = read_gate_level_layout<gate_layout>(layout_stream, "empty");

    compare_written_and_read_layout(layout, read_layout);
}

TEST_CASE("Write and read layouts")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    check_parsing_equiv_all<gate_layout>();
}
