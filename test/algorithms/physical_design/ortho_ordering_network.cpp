//
// Created by benjamin on 11.04.23.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/algorithms/physical_design/apply_gate_library.hpp>
#include <fiction/algorithms/physical_design/ortho_ordering_network.hpp>
#include <fiction/algorithms/verification/equivalence_checking.hpp>
#include <fiction/technology/qca_one_library.hpp>

#include <iostream>

using namespace fiction;

TEST_CASE("conditional_coloring", "[orthogonal-ordering]")
{
    const auto check_colors = [](const auto& ntk)
    {
        auto container = detail::conditional_coloring(ntk);
        CHECK(detail::is_east_south_colored(container.color_ntk));
    };

    check_colors(input_ordering_view{mockturtle::fanout_view{
        fanout_substitution<technology_network>(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>())}});
    check_colors(input_ordering_view{mockturtle::fanout_view{
        fanout_substitution<technology_network>(blueprints::maj1_network<mockturtle::aig_network>())}});
    check_colors(input_ordering_view{mockturtle::fanout_view{
        fanout_substitution<technology_network>(blueprints::maj4_network<mockturtle::aig_network>())}});
    check_colors(input_ordering_view{mockturtle::fanout_view{
        fanout_substitution<technology_network>(blueprints::se_coloring_corner_case_network<technology_network>())}});
    check_colors(input_ordering_view{mockturtle::fanout_view{fanout_substitution<technology_network>(
        blueprints::fanout_substitution_corner_case_network<technology_network>())}});
    check_colors(input_ordering_view{mockturtle::fanout_view{
        fanout_substitution<technology_network>(blueprints::nary_operation_network<technology_network>())}});
    check_colors(input_ordering_view{
        mockturtle::fanout_view{fanout_substitution<technology_network>(blueprints::clpl<technology_network>())}});
    check_colors(input_ordering_view{mockturtle::fanout_view{
        fanout_substitution<technology_network>(blueprints::half_adder_network<mockturtle::mig_network>())}});
    check_colors(input_ordering_view{mockturtle::fanout_view{
        fanout_substitution<technology_network>(blueprints::test_sort_inputs<mockturtle::mig_network>())}});
    check_colors(input_ordering_view{mockturtle::fanout_view{fanout_substitution<technology_network>(
        blueprints::test_fanout_nodes_coloring_null<mockturtle::mig_network>())}});
    check_colors(input_ordering_view{mockturtle::fanout_view{
        fanout_substitution<technology_network>(blueprints::test_fanout_nodes_rank<mockturtle::mig_network>())}});
    check_colors(input_ordering_view{mockturtle::fanout_view{
        fanout_substitution<technology_network>(blueprints::test_fanout_swap<mockturtle::mig_network>())}});
    check_colors(input_ordering_view{mockturtle::fanout_view{
        fanout_substitution<technology_network>(blueprints::coloring_fo_one_a<mockturtle::mig_network>())}});
    check_colors(input_ordering_view{mockturtle::fanout_view{
        fanout_substitution<technology_network>(blueprints::coloring_fo_one_b<technology_network>())}});
    check_colors(input_ordering_view{mockturtle::fanout_view{
        fanout_substitution<technology_network>(blueprints::par_check<mockturtle::mig_network>())}});
}

void check_stats(const orthogonal_physical_design_stats& st) noexcept
{
    CHECK(st.x_size > 0);
    CHECK(st.y_size > 0);
    CHECK(st.num_gates > 0);
    CHECK(st.num_wires > 0);
}

TEST_CASE("Ordering gate library application and design rule violation", "[orthogonal-ordering]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    using cell_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    const auto check_ortho_ordering = [](const auto& ntk)
    {
        orthogonal_physical_design_stats stats{};

        auto layout = orthogonal_ordering_network<gate_layout>(ntk, {}, &stats);

        check_stats(stats);
        check_eq(ntk, layout);

        CHECK_NOTHROW(apply_gate_library<cell_layout, qca_one_library>(layout));

        gate_level_drv_stats  st = {};
        gate_level_drv_params ps = {};
        std::stringstream     ss{};
        ps.out = &ss;
        gate_level_drvs(layout, ps, &st);

        CHECK(st.drvs == 0);
    };

    check_ortho_ordering(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>());
    check_ortho_ordering(blueprints::maj1_network<mockturtle::aig_network>());
    check_ortho_ordering(blueprints::maj4_network<mockturtle::aig_network>());
    check_ortho_ordering(blueprints::se_coloring_corner_case_network<technology_network>());
    check_ortho_ordering(blueprints::fanout_substitution_corner_case_network<technology_network>());
    check_ortho_ordering(blueprints::clpl<technology_network>());
    check_ortho_ordering(blueprints::half_adder_network<mockturtle::mig_network>());
    check_ortho_ordering(blueprints::test_sort_inputs<mockturtle::mig_network>());
    check_ortho_ordering(blueprints::test_fanout_nodes_coloring_null<mockturtle::mig_network>());
    check_ortho_ordering(blueprints::test_fanout_nodes_rank<mockturtle::mig_network>());
    check_ortho_ordering(blueprints::test_fanout_swap<mockturtle::mig_network>());
    check_ortho_ordering(blueprints::coloring_fo_one_a<mockturtle::mig_network>());
    check_ortho_ordering(blueprints::coloring_fo_one_a<technology_network>());
    check_ortho_ordering(blueprints::par_check<mockturtle::mig_network>());

    // constant input network
    check_ortho_ordering(blueprints::unbalanced_and_inv_network<mockturtle::mig_network>());
}

TEST_CASE("Ordering name conservation after orthogonal physical design", "[orthogonal-ordering]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto maj = blueprints::maj1_network<mockturtle::names_view<mockturtle::aig_network>>();
    maj.set_network_name("maj");

    const auto layout = orthogonal_ordering_network<gate_layout>(maj);

    // network name
    CHECK(layout.get_layout_name() == "maj");

    // PI names
    CHECK(layout.get_name(layout.pi_at(0)) == "a");  // first PI
    CHECK(layout.get_name(layout.pi_at(1)) == "b");  // second PI
    CHECK(layout.get_name(layout.pi_at(2)) == "c");  // third PI

    // PO names
    CHECK(layout.get_output_name(0) == "f");
}
