//
// Created by simon on 12.06.24.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/algorithms/physical_design/a_star_pr.hpp>
#include <fiction/algorithms/physical_design/apply_gate_library.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/technology/qca_one_library.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/names_view.hpp>

#include <type_traits>

using namespace fiction;

template <typename Lyt, typename Ntk>
void check_a_star_pr_equiv(const Ntk& ntk)
{
    a_star_pr_stats  stats{};
    a_star_pr_params params{};
    params.return_first = true;

    const auto layout = a_star_pr<Lyt>(ntk, params, &stats);

    check_eq(ntk, layout);
}

template <typename Lyt>
void check_a_star_pr_equiv_all()
{
    check_a_star_pr_equiv<Lyt>(blueprints::maj1_network<mockturtle::aig_network>());
    check_a_star_pr_equiv<Lyt>(blueprints::maj4_network<mockturtle::aig_network>());
    check_a_star_pr_equiv<Lyt>(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>());
    check_a_star_pr_equiv<Lyt>(blueprints::and_or_network<technology_network>());
    check_a_star_pr_equiv<Lyt>(blueprints::multi_output_and_network<technology_network>());
    check_a_star_pr_equiv<Lyt>(blueprints::half_adder_network<technology_network>());
    check_a_star_pr_equiv<Lyt>(blueprints::full_adder_network<technology_network>());
    check_a_star_pr_equiv<Lyt>(blueprints::mux21_network<technology_network>());
    check_a_star_pr_equiv<Lyt>(blueprints::se_coloring_corner_case_network<technology_network>());
    check_a_star_pr_equiv<Lyt>(blueprints::fanout_substitution_corner_case_network<technology_network>());
    check_a_star_pr_equiv<Lyt>(blueprints::inverter_network<technology_network>());
    check_a_star_pr_equiv<Lyt>(blueprints::clpl<technology_network>());
    check_a_star_pr_equiv<Lyt>(blueprints::one_to_five_path_difference_network<technology_network>());
    check_a_star_pr_equiv<Lyt>(blueprints::nand_xnor_network<technology_network>());
}

TEST_CASE("Layout equivalence", "[algorithms]")
{
    SECTION("Cartesian layouts")
    {
        using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

        check_a_star_pr_equiv_all<gate_layout>();
    }
}

TEST_CASE("Gate library application", "[a_star_pr]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    using cell_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    const auto check = [](const auto& ntk)
    {
        a_star_pr_stats  stats{};
        a_star_pr_params params{};
        params.return_first = true;

        const auto layout = a_star_pr<gate_layout>(ntk, params, &stats);

        CHECK_NOTHROW(apply_gate_library<cell_layout, qca_one_library>(layout));
    };

    check(blueprints::maj1_network<mockturtle::names_view<mockturtle::aig_network>>());
}

TEST_CASE("Different parameters", "[a_star_pr]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    const auto ntk    = blueprints::mux21_network<technology_network>();

    a_star_pr_stats stats{};

    a_star_pr_params params{};

    params.return_first = true;
    const auto layout1  = a_star_pr<gate_layout>(ntk, params, &stats);
    check_eq(ntk, layout1);

    params.verbose     = true;
    const auto layout2 = a_star_pr<gate_layout>(ntk, params, &stats);
    check_eq(ntk, layout2);

    params.high_effort = true;
    const auto layout3 = a_star_pr<gate_layout>(ntk, params, &stats);
    check_eq(ntk, layout3);

    params.timeout      = 0;
    params.return_first = false;
    const auto layout4  = a_star_pr<gate_layout>(ntk, params, &stats);
    check_eq(ntk, layout4);
}

TEST_CASE("Name conservation after A* physical design", "[a_star_pr]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto maj = blueprints::maj1_network<mockturtle::aig_network>();
    maj.set_network_name("maj");

    a_star_pr_stats  stats{};
    a_star_pr_params params{};
    params.return_first = true;

    const auto layout = a_star_pr<gate_layout>(maj, params, &stats);

    // network name
    CHECK(layout.get_layout_name() == "maj");

    // PI names
    CHECK(layout.get_name(layout.pi_at(0)) == "a");  // first PI
    CHECK(layout.get_name(layout.pi_at(1)) == "b");  // second PI
    CHECK(layout.get_name(layout.pi_at(2)) == "c");  // third PI

    // PO names
    CHECK(layout.get_output_name(0) == "f");
}