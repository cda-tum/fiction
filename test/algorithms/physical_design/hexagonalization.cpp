//
// Created by Simon Hofmann on 27.04.23.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/algorithms/physical_design/hexagonalization.hpp>
#include <fiction/algorithms/physical_design/orthogonal.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/types.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/views/names_view.hpp>

using namespace fiction;

template <typename Lyt, typename Ntk>
void check_mapping_equiv(const Ntk& ntk)
{
    const auto layout = orthogonal<Lyt>(ntk, {});

    hexagonalization_stats  stats{};
    hexagonalization_params params{};
    const auto              hex_layout = hexagonalization<hex_even_row_gate_clk_lyt, Lyt>(layout, params, &stats);

    params.input_mode = hexagonalization_params::hexagonalization_input_mode::EXTEND;
    const auto hex_layout_top_pis  = hexagonalization<hex_even_row_gate_clk_lyt, Lyt>(layout, params, &stats);

    check_eq(ntk, layout);
    check_eq(ntk, hex_layout);
    check_eq(ntk, hex_layout_top_pis);
    check_eq(layout, hex_layout);
    check_eq(layout, hex_layout_top_pis);

    hex_layout_top_pis.foreach_pi([&](const auto& gate) { CHECK(hex_layout_top_pis.get_tile(gate).y == 0); });
}

template <typename Lyt>
void check_mapping_equiv_layout(const Lyt& lyt)
{
    hexagonalization_stats  stats{};
    hexagonalization_params params{};
    const auto              hex_layout = hexagonalization<hex_even_row_gate_clk_lyt, Lyt>(lyt, params, &stats);

    params.input_mode = hexagonalization_params::hexagonalization_input_mode::EXTEND;
    const auto hex_layout_top_pis  = hexagonalization<hex_even_row_gate_clk_lyt, Lyt>(lyt, params, &stats);

    check_eq(lyt, hex_layout);
    check_eq(lyt, hex_layout_top_pis);

    CHECK(lyt.get_layout_name() == hex_layout.get_layout_name());
    CHECK(lyt.get_layout_name() == hex_layout_top_pis.get_layout_name());

    hex_layout_top_pis.foreach_pi([&](const auto& gate) { CHECK(hex_layout_top_pis.get_tile(gate).y == 0); });
}

template <typename Lyt>
void check_mapping_equiv_layout_with_planar_rerouting(const Lyt& lyt)
{
    hexagonalization_stats  stats{};
    hexagonalization_params params{};

    params.input_mode =hexagonalization_params::hexagonalization_input_mode::EXTEND_PLANAR;
    const auto hex_layout                  = hexagonalization<hex_even_row_gate_clk_lyt, Lyt>(lyt, params, &stats);

    check_eq(lyt, hex_layout);

    CHECK(lyt.get_layout_name() == hex_layout.get_layout_name());

    hex_layout.foreach_pi([&](const auto& gate) { CHECK(hex_layout.get_tile(gate).y == 0); });
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
    check_mapping_equiv<Lyt>(blueprints::fanout_substitution_corner_case_network<technology_network>());
    check_mapping_equiv<Lyt>(blueprints::inverter_network<technology_network>());
    check_mapping_equiv<Lyt>(blueprints::clpl<technology_network>());
    check_mapping_equiv<Lyt>(blueprints::one_to_five_path_difference_network<technology_network>());
    check_mapping_equiv<Lyt>(blueprints::nand_xnor_network<technology_network>());

    check_mapping_equiv_layout(blueprints::straight_wire_gate_layout<cart_gate_clk_lyt>());
    check_mapping_equiv_layout(blueprints::or_not_gate_layout<cart_gate_clk_lyt>());
    check_mapping_equiv_layout(blueprints::crossing_layout<cart_gate_clk_lyt>());
    check_mapping_equiv_layout(blueprints::tautology_gate_layout<cart_gate_clk_lyt>());
    check_mapping_equiv_layout(blueprints::ge_gt_le_lt_layout<cart_gate_clk_lyt>());

    check_mapping_equiv_layout_with_planar_rerouting(blueprints::straight_wire_gate_layout<cart_gate_clk_lyt>());
    check_mapping_equiv_layout_with_planar_rerouting(blueprints::or_not_gate_layout<cart_gate_clk_lyt>());
    check_mapping_equiv_layout_with_planar_rerouting(blueprints::crossing_layout<cart_gate_clk_lyt>());
    check_mapping_equiv_layout_with_planar_rerouting(blueprints::tautology_gate_layout<cart_gate_clk_lyt>());
    check_mapping_equiv_layout_with_planar_rerouting(blueprints::ge_gt_le_lt_layout<cart_gate_clk_lyt>());
}

TEST_CASE("Layout equivalence", "[hexagonalization]")
{
    SECTION("Cartesian layouts")
    {
        using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

        check_mapping_equiv_all<gate_layout>();
    }
}

TEST_CASE("Cartesian to hexagonal")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    using hex_lyt     = hex_even_row_gate_clk_lyt;

    constexpr const auto layout_height = 3;
    CHECK(detail::to_hex<gate_layout, hex_lyt>(coordinate<gate_layout>(0, 0, 0), layout_height) ==
          offset::ucoord_t(1, 0, 0));
    CHECK(detail::to_hex<gate_layout, hex_lyt>(coordinate<gate_layout>(0, 0, 1), layout_height) ==
          offset::ucoord_t(1, 0, 1));
    CHECK(detail::to_hex<gate_layout, hex_lyt>(coordinate<gate_layout>(1, 0, 0), layout_height) ==
          offset::ucoord_t(2, 1, 0));
    CHECK(detail::to_hex<gate_layout, hex_lyt>(coordinate<gate_layout>(1, 0, 1), layout_height) ==
          offset::ucoord_t(2, 1, 1));
    CHECK(detail::to_hex<gate_layout, hex_lyt>(coordinate<gate_layout>(2, 0, 0), layout_height) ==
          offset::ucoord_t(2, 2, 0));
    CHECK(detail::to_hex<gate_layout, hex_lyt>(coordinate<gate_layout>(2, 0, 1), layout_height) ==
          offset::ucoord_t(2, 2, 1));

    CHECK(detail::to_hex<gate_layout, hex_lyt>(coordinate<gate_layout>(0, 1, 0), layout_height) ==
          offset::ucoord_t(1, 1, 0));
    CHECK(detail::to_hex<gate_layout, hex_lyt>(coordinate<gate_layout>(0, 1, 1), layout_height) ==
          offset::ucoord_t(1, 1, 1));
    CHECK(detail::to_hex<gate_layout, hex_lyt>(coordinate<gate_layout>(1, 1, 0), layout_height) ==
          offset::ucoord_t(1, 2, 0));
    CHECK(detail::to_hex<gate_layout, hex_lyt>(coordinate<gate_layout>(1, 1, 1), layout_height) ==
          offset::ucoord_t(1, 2, 1));
    CHECK(detail::to_hex<gate_layout, hex_lyt>(coordinate<gate_layout>(2, 1, 0), layout_height) ==
          offset::ucoord_t(2, 3, 0));
    CHECK(detail::to_hex<gate_layout, hex_lyt>(coordinate<gate_layout>(2, 1, 1), layout_height) ==
          offset::ucoord_t(2, 3, 1));

    CHECK(detail::to_hex<gate_layout, hex_lyt>(coordinate<gate_layout>(0, 2, 0), layout_height) ==
          offset::ucoord_t(0, 2, 0));
    CHECK(detail::to_hex<gate_layout, hex_lyt>(coordinate<gate_layout>(0, 2, 1), layout_height) ==
          offset::ucoord_t(0, 2, 1));
    CHECK(detail::to_hex<gate_layout, hex_lyt>(coordinate<gate_layout>(1, 2, 0), layout_height) ==
          offset::ucoord_t(1, 3, 0));
    CHECK(detail::to_hex<gate_layout, hex_lyt>(coordinate<gate_layout>(1, 2, 1), layout_height) ==
          offset::ucoord_t(1, 3, 1));
    CHECK(detail::to_hex<gate_layout, hex_lyt>(coordinate<gate_layout>(2, 2, 0), layout_height) ==
          offset::ucoord_t(1, 4, 0));
    CHECK(detail::to_hex<gate_layout, hex_lyt>(coordinate<gate_layout>(2, 2, 1), layout_height) ==
          offset::ucoord_t(1, 4, 1));
}
