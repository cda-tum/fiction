//
// Created by simon on 12.06.24.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/algorithms/physical_design/apply_gate_library.hpp>
#include <fiction/algorithms/physical_design/graph_oriented_layout_design.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/technology/qca_one_library.hpp>
#include <fiction/utils/network_utils.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/names_view.hpp>

#include <type_traits>

using namespace fiction;

template <typename Lyt, typename Ntk>
void check_graph_oriented_layout_design_equiv(const Ntk& ntk)
{
    graph_oriented_layout_design_stats  stats{};
    graph_oriented_layout_design_params params{};
    params.timeout      = 100000;
    params.return_first = true;

    const auto layout = graph_oriented_layout_design<Lyt>(ntk, params, &stats);
    REQUIRE(layout.has_value());

    check_eq(ntk, *layout);
}

template <typename Lyt>
void check_graph_oriented_layout_design_equiv_all()
{
    check_graph_oriented_layout_design_equiv<Lyt>(blueprints::maj1_network<mockturtle::aig_network>());
    check_graph_oriented_layout_design_equiv<Lyt>(blueprints::maj4_network<mockturtle::aig_network>());
    check_graph_oriented_layout_design_equiv<Lyt>(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>());
    check_graph_oriented_layout_design_equiv<Lyt>(blueprints::and_or_network<technology_network>());
    check_graph_oriented_layout_design_equiv<Lyt>(blueprints::half_adder_network<mockturtle::mig_network>());
    check_graph_oriented_layout_design_equiv<Lyt>(blueprints::full_adder_network<technology_network>());
    check_graph_oriented_layout_design_equiv<Lyt>(blueprints::mux21_network<mockturtle::xag_network>());
    check_graph_oriented_layout_design_equiv<Lyt>(blueprints::se_coloring_corner_case_network<technology_network>());
    check_graph_oriented_layout_design_equiv<Lyt>(
        blueprints::fanout_substitution_corner_case_network<technology_network>());
    check_graph_oriented_layout_design_equiv<Lyt>(blueprints::inverter_network<technology_network>());
    check_graph_oriented_layout_design_equiv<Lyt>(blueprints::clpl<technology_network>());
    check_graph_oriented_layout_design_equiv<Lyt>(
        blueprints::one_to_five_path_difference_network<technology_network>());
    check_graph_oriented_layout_design_equiv<Lyt>(blueprints::nand_xnor_network<technology_network>());
}

TEST_CASE("Layout equivalence after graph-oriented layout design", "[graph-oriented-layout-design]")
{
    SECTION("Cartesian layouts")
    {
        using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

        check_graph_oriented_layout_design_equiv_all<gate_layout>();
    }
}

TEST_CASE("Gate library application", "[graph-oriented-layout-design]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    using cell_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    const auto check = [](const auto& ntk)
    {
        graph_oriented_layout_design_stats  stats{};
        graph_oriented_layout_design_params params{};
        params.timeout      = 100000;
        params.return_first = true;

        const auto layout = graph_oriented_layout_design<gate_layout>(ntk, params, &stats);
        REQUIRE(layout.has_value());

        CHECK_NOTHROW(apply_gate_library<cell_layout, qca_one_library>(*layout));
    };

    check(blueprints::maj1_network<mockturtle::names_view<mockturtle::aig_network>>());
}

TEST_CASE("Different parameters", "[graph-oriented-layout-design]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    const auto ntk    = blueprints::mux21_network<technology_network>();

    graph_oriented_layout_design_stats stats{};

    graph_oriented_layout_design_params params{};

    // Default timeout  return first found layout
    params.return_first = true;
    const auto layout1  = graph_oriented_layout_design<gate_layout>(ntk, params, &stats);

    REQUIRE(layout1.has_value());
    check_eq(ntk, *layout1);

    // Verbose mode and timeout
    params.timeout     = 100000;
    params.verbose     = true;
    const auto layout2 = graph_oriented_layout_design<gate_layout>(ntk, params, &stats);

    REQUIRE(layout2.has_value());
    check_eq(ntk, *layout2);

    // High effort mode
    params.high_effort_mode = true;
    params.verbose          = false;
    const auto layout3      = graph_oriented_layout_design<gate_layout>(ntk, params, &stats);

    REQUIRE(layout3.has_value());
    check_eq(ntk, *layout3);

    // Full search
    params.return_first = false;
    const auto layout4  = graph_oriented_layout_design<gate_layout>(ntk, params, &stats);

    REQUIRE(layout4.has_value());
    check_eq(ntk, *layout4);

    // More vertex expansions
    params.return_first          = true;
    params.num_vertex_expansions = 8;
    const auto layout5           = graph_oriented_layout_design<gate_layout>(ntk, params, &stats);

    REQUIRE(layout5.has_value());
    check_eq(ntk, *layout4);

    // Timeout limit reached
    params.timeout     = 0;
    const auto layout6 = graph_oriented_layout_design<gate_layout>(ntk, params, &stats);

    CHECK(!layout6.has_value());
}

TEST_CASE("Name conservation after graph-oriented layout design", "[graph-oriented-layout-design]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto maj = blueprints::maj1_network<mockturtle::aig_network>();
    maj.set_network_name("maj");

    graph_oriented_layout_design_stats  stats{};
    graph_oriented_layout_design_params params{};
    params.timeout      = 100000;
    params.return_first = true;

    const auto layout = graph_oriented_layout_design<gate_layout>(maj, params, &stats);

    REQUIRE(layout.has_value());

    // network name
    CHECK(layout->get_layout_name() == "maj");

    // PI names
    CHECK(layout->get_name(layout->pi_at(0)) == "a");  // first PI
    CHECK(layout->get_name(layout->pi_at(1)) == "b");  // second PI
    CHECK(layout->get_name(layout->pi_at(2)) == "c");  // third PI

    // PO names
    CHECK(layout->get_output_name(0) == "f");
}

TEST_CASE("High fanin exception", "[graph-oriented-layout-design]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    const auto ntk    = blueprints::maj1_network<technology_network>();

    graph_oriented_layout_design_stats stats{};

    graph_oriented_layout_design_params params{};

    CHECK_THROWS_AS(graph_oriented_layout_design<gate_layout>(ntk, params, &stats), high_degree_fanin_exception);
}
