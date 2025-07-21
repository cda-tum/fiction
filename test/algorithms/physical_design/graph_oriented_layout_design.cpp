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
#include <mockturtle/views/names_view.hpp>

#include <array>
#include <cstdint>
#include <functional>
#include <stdexcept>

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

    graph_oriented_layout_design_stats  stats{};
    graph_oriented_layout_design_params params{};
    params.return_first = true;

    SECTION("High-efficiency mode, return first found layout")
    {
        params.mode = graph_oriented_layout_design_params::effort_mode::HIGH_EFFICIENCY;

        const auto layout = graph_oriented_layout_design<gate_layout>(ntk, params, &stats);
        REQUIRE(layout.has_value());
        check_eq(ntk, *layout);
    }

    SECTION("Verbose mode with timeout")
    {
        params.mode    = graph_oriented_layout_design_params::effort_mode::HIGH_EFFICIENCY;
        params.timeout = 100000;
        params.verbose = true;

        const auto layout = graph_oriented_layout_design<gate_layout>(ntk, params, &stats);
        REQUIRE(layout.has_value());
        check_eq(ntk, *layout);
    }

    SECTION("High-effort mode")
    {
        params.mode    = graph_oriented_layout_design_params::effort_mode::HIGH_EFFORT;
        params.verbose = false;

        const auto layout = graph_oriented_layout_design<gate_layout>(ntk, params, &stats);
        REQUIRE(layout.has_value());
        check_eq(ntk, *layout);
    }

    SECTION("Highest-effort mode")
    {
        params.mode = graph_oriented_layout_design_params::effort_mode::HIGHEST_EFFORT;

        const auto layout = graph_oriented_layout_design<gate_layout>(ntk, params, &stats);
        REQUIRE(layout.has_value());
        check_eq(ntk, *layout);
    }

    SECTION("Maximum-effort mode")
    {
        params.mode = graph_oriented_layout_design_params::effort_mode::MAXIMUM_EFFORT;

        const auto layout = graph_oriented_layout_design<gate_layout>(ntk, params, &stats);
        REQUIRE(layout.has_value());
        check_eq(ntk, *layout);
    }

    SECTION("Maximum-effort mode with random seed")
    {
        params.mode = graph_oriented_layout_design_params::effort_mode::MAXIMUM_EFFORT;
        params.seed = 12345;

        const auto layout = graph_oriented_layout_design<gate_layout>(ntk, params, &stats);
        REQUIRE(layout.has_value());
        check_eq(ntk, *layout);
    }

    SECTION("More vertex expansions (num_vertex_expansions = 8)")
    {
        params.mode                  = graph_oriented_layout_design_params::effort_mode::HIGH_EFFORT;
        params.num_vertex_expansions = 8;

        const auto layout = graph_oriented_layout_design<gate_layout>(ntk, params, &stats);
        REQUIRE(layout.has_value());
        check_eq(ntk, *layout);
    }

    SECTION("Straight inverters.")
    {
        params.mode                  = graph_oriented_layout_design_params::effort_mode::MAXIMUM_EFFORT;
        params.enable_multithreading = true;
        params.straight_inverters    = true;

        for (const auto& network :
             {blueprints::mux21_network<technology_network>(), blueprints::inverter_network<technology_network>(),
              blueprints::parity_network<technology_network>()})
        {
            const auto layout = graph_oriented_layout_design<gate_layout>(network, params, &stats);
            REQUIRE(layout.has_value());
            check_eq(network, *layout);

            layout->foreach_gate(
                [&layout](const auto& gate)
                {
                    if (layout->is_inv(gate))
                    {
                        const auto layout_tile = layout->get_tile(gate);
                        const auto fanin       = layout->incoming_data_flow(layout_tile).front();
                        const auto fanout      = layout->outgoing_data_flow(layout_tile).front();

                        const bool vertical_straight_inverter = (fanin.x == layout_tile.x && layout_tile.x == fanout.x);
                        const bool horizontal_straight_inverter =
                            (fanin.y == layout_tile.y && layout_tile.y == fanout.y);
                        const bool straight_inverter = vertical_straight_inverter || horizontal_straight_inverter;

                        CHECK(straight_inverter);
                    }
                });
        }
    }

    params.return_first = false;

    SECTION("Full search (return_first = false)")
    {
        params.mode = graph_oriented_layout_design_params::effort_mode::HIGH_EFFORT;

        const auto layout = graph_oriented_layout_design<gate_layout>(ntk, params, &stats);
        REQUIRE(layout.has_value());
        check_eq(ntk, *layout);
    }

    SECTION("Timeout limit reached")
    {
        params.mode    = graph_oriented_layout_design_params::effort_mode::HIGH_EFFICIENCY;
        params.timeout = 0;

        const auto layout = graph_oriented_layout_design<gate_layout>(ntk, params, &stats);
        CHECK(!layout.has_value());
    }

    SECTION("Planar layout (z = 0)")
    {
        params.mode    = graph_oriented_layout_design_params::effort_mode::HIGH_EFFICIENCY;
        params.timeout = 100000;
        params.planar  = true;

        const auto layout = graph_oriented_layout_design<gate_layout>(ntk, params, &stats);
        REQUIRE(layout.has_value());
        check_eq(ntk, *layout);
        CHECK(layout->z() == 0);
    }
}

TEST_CASE("Multithreading", "[graph-oriented-layout-design]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    const auto ntk    = blueprints::mux21_network<technology_network>();

    graph_oriented_layout_design_stats  stats{};
    graph_oriented_layout_design_params params{};
    // enable multithreading for all sections
    params.enable_multithreading = true;

    SECTION("Highest-effort mode with multithreading")
    {
        params.mode       = graph_oriented_layout_design_params::effort_mode::HIGHEST_EFFORT;
        const auto layout = graph_oriented_layout_design<gate_layout>(ntk, params, &stats);
        REQUIRE(layout.has_value());
        check_eq(ntk, *layout);
    }

    SECTION("High-efficiency mode, return first, multithreading")
    {
        params.mode         = graph_oriented_layout_design_params::effort_mode::HIGH_EFFICIENCY;
        params.return_first = true;
        const auto layout   = graph_oriented_layout_design<gate_layout>(ntk, params, &stats);
        REQUIRE(layout.has_value());
        check_eq(ntk, *layout);
    }

    SECTION("Maximum-effort mode with seed and multithreading")
    {
        params.mode       = graph_oriented_layout_design_params::effort_mode::MAXIMUM_EFFORT;
        params.seed       = 12345;
        const auto layout = graph_oriented_layout_design<gate_layout>(ntk, params, &stats);
        REQUIRE(layout.has_value());
        check_eq(ntk, *layout);
    }
}

TEST_CASE("Different cost objectives", "[graph-oriented-layout-design]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    const auto ntk    = blueprints::mux21_network<technology_network>();

    graph_oriented_layout_design_stats stats{};

    graph_oriented_layout_design_params params{};

    params.mode = graph_oriented_layout_design_params::effort_mode::HIGHEST_EFFORT;

    // array of cost objectives to iterate over
    const std::array cost_objectives = {graph_oriented_layout_design_params::cost_objective::AREA,
                                        graph_oriented_layout_design_params::cost_objective::WIRES,
                                        graph_oriented_layout_design_params::cost_objective::CROSSINGS,
                                        graph_oriented_layout_design_params::cost_objective::ACP};

    // loop over each cost objective
    for (const auto& cost : cost_objectives)
    {
        params.cost       = cost;
        const auto layout = graph_oriented_layout_design<gate_layout>(ntk, params, &stats);

        REQUIRE(layout.has_value());
        check_eq(ntk, *layout);
    }
}

TEST_CASE("Custom cost objective", "[graph-oriented-layout-design]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    const auto ntk    = blueprints::mux21_network<technology_network>();

    graph_oriented_layout_design_stats stats{};

    graph_oriented_layout_design_params params{};
    params.mode         = graph_oriented_layout_design_params::effort_mode::HIGHEST_EFFORT;
    params.cost         = graph_oriented_layout_design_params::cost_objective::CUSTOM;
    params.return_first = true;

    // define a custom cost function
    const std::function<uint64_t(const gate_layout&)> custom_cost_objective = [](const gate_layout& layout) -> uint64_t
    {
        // Example custom logic for calculating cost
        return (layout.num_wires() * 2) + layout.num_crossings();
    };

    const auto layout = graph_oriented_layout_design<gate_layout>(ntk, params, &stats, custom_cost_objective);

    REQUIRE(layout.has_value());
    check_eq(ntk, *layout);

    // high-effort mode
    params.mode = graph_oriented_layout_design_params::effort_mode::HIGH_EFFORT;

    const auto layout_high_effort =
        graph_oriented_layout_design<gate_layout>(ntk, params, &stats, custom_cost_objective);

    REQUIRE(layout_high_effort.has_value());
    check_eq(ntk, *layout_high_effort);

    // maximum-effort mode
    params.mode = graph_oriented_layout_design_params::effort_mode::MAXIMUM_EFFORT;
    params.seed = 12345;

    const auto layout_maximum_effort =
        graph_oriented_layout_design<gate_layout>(ntk, params, &stats, custom_cost_objective);

    REQUIRE(layout_maximum_effort.has_value());
    check_eq(ntk, *layout_maximum_effort);
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

TEST_CASE("No custom cost objective provided exception", "[graph-oriented-layout-design]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;
    const auto ntk    = blueprints::mux21_network<technology_network>();

    graph_oriented_layout_design_stats stats{};

    graph_oriented_layout_design_params params{};
    params.mode         = graph_oriented_layout_design_params::effort_mode::HIGHEST_EFFORT;
    params.cost         = graph_oriented_layout_design_params::cost_objective::CUSTOM;
    params.return_first = true;

    CHECK_THROWS_AS(graph_oriented_layout_design<gate_layout>(ntk, params, &stats), std::invalid_argument);
}
