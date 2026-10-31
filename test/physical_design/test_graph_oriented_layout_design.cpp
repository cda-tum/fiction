/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Tests for `fiction/physical_design/graph_oriented_layout_design.hpp`.
 * @author Simon Hofmann (simon1hofmann)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "fiction/layouts/layout_base.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"
#include "utils/progress_recorder.hpp"

#include <fiction/layouts/gate_layout.hpp>
#include <fiction/networks/extract_layout_network.hpp>
#include <fiction/networks/network_utils.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/physical_design/apply_gate_library.hpp>
#include <fiction/physical_design/graph_oriented_layout_design.hpp>
#include <fiction/synthesis/network_conversion.hpp>
#include <fiction/technology/qca/qca_one_library.hpp>
#include <fiction/types.hpp>

#include <fmt/format.h>
#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/views/names_view.hpp>

#include <algorithm>
#include <array>
#include <atomic>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <future>
#include <optional>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

// The tests dereference optionals right after REQUIRE(x.has_value()), which the check does not model.
// NOLINTBEGIN(bugprone-unchecked-optional-access)

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::networks;
using namespace fiction::physical_design;
using namespace fiction::qca;
using namespace fiction::synthesis;

static_assert(
    std::same_as<decltype(&graph_oriented_layout_design),
                 std::optional<cartesian_gate_layout> (*)(
                     const named_technology_network&, graph_oriented_layout_design_params,
                     graph_oriented_layout_design_stats*, std::function<uint64_t(const cartesian_gate_layout&)>)>);

/**
 * @brief Accesses an optional after a Catch2 assertion established that it contains a value.
 * @tparam T Contained value type.
 * @param optional Optional value guarded by a preceding `REQUIRE`.
 * @return Reference to the contained value.
 */
template <typename T>
const T& required_value(const std::optional<T>& optional)
{
    return optional.value();  // NOLINT(bugprone-unchecked-optional-access) Catch2 REQUIRE guards each call.
}

/**
 * @brief Checks logic equivalence and removal of placement-search obstructions.
 * @tparam Ntk Network type.
 * @param ntk Network to place and route.
 */
template <typename Ntk>
void check_graph_oriented_layout_design_equiv(const Ntk& ntk)
{
    graph_oriented_layout_design_stats        stats{};
    const graph_oriented_layout_design_params params{.return_first = true, .timeout = 100000};

    const auto layout = graph_oriented_layout_design(fiction::synthesis::convert_network<named_technology_network>(ntk),
                                                     params, &stats);
    REQUIRE(layout.has_value());

    check_eq(ntk, required_value(layout));
    const auto& result = required_value(layout);
    result.foreach_coordinate([&result](const auto& c)
                              { CHECK(result.is_obstructed_coordinate(c) == !result.is_empty_tile(c)); });
}

/** @brief Checks GOLD equivalence across the network blueprints. */
void check_graph_oriented_layout_design_equiv_all()
{
    check_graph_oriented_layout_design_equiv(blueprints::maj1_network<mockturtle::aig_network>());
    check_graph_oriented_layout_design_equiv(blueprints::maj4_network<mockturtle::aig_network>());
    check_graph_oriented_layout_design_equiv(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>());
    check_graph_oriented_layout_design_equiv(blueprints::and_or_network<technology_network>());
    check_graph_oriented_layout_design_equiv(blueprints::half_adder_network<mockturtle::mig_network>());
    check_graph_oriented_layout_design_equiv(blueprints::full_adder_network<technology_network>());
    check_graph_oriented_layout_design_equiv(blueprints::mux21_network<mockturtle::xag_network>());
    check_graph_oriented_layout_design_equiv(blueprints::se_coloring_corner_case_network<technology_network>());
    check_graph_oriented_layout_design_equiv(blueprints::fanout_substitution_corner_case_network<technology_network>());
    check_graph_oriented_layout_design_equiv(blueprints::inverter_network<technology_network>());
    check_graph_oriented_layout_design_equiv(blueprints::clpl<technology_network>());
    check_graph_oriented_layout_design_equiv(blueprints::one_to_five_path_difference_network<technology_network>());
    check_graph_oriented_layout_design_equiv(blueprints::nand_xnor_network<technology_network>());
}

TEST_CASE("Layout equivalence after graph-oriented layout design", "[graph-oriented-layout-design]")
{
    SECTION("Cartesian layouts")
    {
        check_graph_oriented_layout_design_equiv_all();
    }
}

TEST_CASE("Gate library application", "[graph-oriented-layout-design]")
{
    const auto check = [](const auto& ntk)
    {
        graph_oriented_layout_design_stats        stats{};
        const graph_oriented_layout_design_params params{.return_first = true, .timeout = 100000};

        const auto layout = graph_oriented_layout_design(
            fiction::synthesis::convert_network<named_technology_network>(ntk), params, &stats);
        REQUIRE(layout.has_value());

        CHECK_NOTHROW(apply_gate_library<qca_one_library>(required_value(layout)));
    };

    check(blueprints::maj1_network<named_aig_network>());
}

TEST_CASE("Different parameters", "[graph-oriented-layout-design]")
{
    const auto ntk = blueprints::mux21_network<technology_network>();

    graph_oriented_layout_design_stats  stats{};
    graph_oriented_layout_design_params params{.return_first = true};

    SECTION("High-efficiency mode, return first found layout")
    {
        params.mode = graph_oriented_layout_design_params::effort_mode::HIGH_EFFICIENCY;

        const auto layout = graph_oriented_layout_design(ntk, params, &stats);
        REQUIRE(layout.has_value());
        check_eq(ntk, required_value(layout));
    }

    SECTION("Verbose mode with timeout")
    {
        params.mode    = graph_oriented_layout_design_params::effort_mode::HIGH_EFFICIENCY;
        params.timeout = 100000;
        params.verbose = true;

        const auto layout = graph_oriented_layout_design(ntk, params, &stats);
        REQUIRE(layout.has_value());
        check_eq(ntk, required_value(layout));
    }

    SECTION("High-effort mode")
    {
        params.mode    = graph_oriented_layout_design_params::effort_mode::HIGH_EFFORT;
        params.verbose = false;

        const auto layout = graph_oriented_layout_design(ntk, params, &stats);
        REQUIRE(layout.has_value());
        check_eq(ntk, required_value(layout));
    }

    SECTION("Highest-effort mode")
    {
        params.mode = graph_oriented_layout_design_params::effort_mode::HIGHEST_EFFORT;

        const auto layout = graph_oriented_layout_design(ntk, params, &stats);
        REQUIRE(layout.has_value());
        check_eq(ntk, required_value(layout));
    }

    SECTION("Maximum-effort mode")
    {
        params.mode = graph_oriented_layout_design_params::effort_mode::MAXIMUM_EFFORT;

        const auto layout = graph_oriented_layout_design(ntk, params, &stats);
        REQUIRE(layout.has_value());
        check_eq(ntk, required_value(layout));
    }

    SECTION("Maximum-effort mode with random seed")
    {
        params.mode = graph_oriented_layout_design_params::effort_mode::MAXIMUM_EFFORT;
        params.seed = 12345;

        const auto layout = graph_oriented_layout_design(ntk, params, &stats);
        REQUIRE(layout.has_value());
        check_eq(ntk, required_value(layout));
    }

    SECTION("More vertex expansions (num_vertex_expansions = 8)")
    {
        params.mode                  = graph_oriented_layout_design_params::effort_mode::HIGH_EFFORT;
        params.num_vertex_expansions = 8;

        const auto layout = graph_oriented_layout_design(ntk, params, &stats);
        REQUIRE(layout.has_value());
        check_eq(ntk, required_value(layout));
    }

    SECTION("Straight inverters")
    {
        params.mode                  = graph_oriented_layout_design_params::effort_mode::MAXIMUM_EFFORT;
        params.enable_multithreading = true;
        params.straight_inverters    = true;

        for (const auto& network :
             {blueprints::mux21_network<technology_network>(), blueprints::inverter_network<technology_network>(),
              blueprints::parity_network<technology_network>()})
        {
            CAPTURE(network.num_pis());
            const auto layout = graph_oriented_layout_design(network, params, &stats);
            REQUIRE(layout.has_value());
            const auto& checked_layout = required_value(layout);
            check_eq(network, checked_layout);

            checked_layout.foreach_gate(
                [&checked_layout](const auto& gate)
                {
                    if (checked_layout.is_inv(gate))
                    {
                        const auto layout_tile = checked_layout.get_tile(gate);
                        CAPTURE(layout_tile);
                        CHECK(checked_layout.has_opposite_incoming_and_outgoing_signals(layout_tile));
                    }
                });
        }
    }

    params.return_first = false;

    SECTION("Full search (return_first = false)")
    {
        params.mode = graph_oriented_layout_design_params::effort_mode::HIGH_EFFORT;

        const auto layout = graph_oriented_layout_design(ntk, params, &stats);
        REQUIRE(layout.has_value());
        check_eq(ntk, required_value(layout));
    }

    SECTION("Timeout limit reached")
    {
        params.mode    = graph_oriented_layout_design_params::effort_mode::HIGH_EFFICIENCY;
        params.timeout = 0;

        const auto layout = graph_oriented_layout_design(ntk, params, &stats);
        CHECK(!layout.has_value());
    }

    SECTION("Planar layout (z = 0)")
    {
        params.mode    = graph_oriented_layout_design_params::effort_mode::HIGH_EFFICIENCY;
        params.timeout = 100000;
        params.planar  = true;

        const auto layout = graph_oriented_layout_design(ntk, params, &stats);
        REQUIRE(layout.has_value());
        const auto& checked_layout = required_value(layout);
        check_eq(ntk, checked_layout);
        CHECK(checked_layout.layers() == 1);
    }

    SECTION("Randomize skip tiles PI placement")
    {
        params.mode                                = graph_oriented_layout_design_params::effort_mode::HIGH_EFFORT;
        params.tiles_to_skip_between_pis           = 3;
        params.randomize_tiles_to_skip_between_pis = true;
        params.seed                                = 42;

        const auto layout = graph_oriented_layout_design(ntk, params, &stats);
        REQUIRE(layout.has_value());
        check_eq(ntk, required_value(layout));
    }

    SECTION("Randomize skip tiles PI placement with zero value")
    {
        params.mode                                = graph_oriented_layout_design_params::effort_mode::HIGH_EFFORT;
        params.tiles_to_skip_between_pis           = 0;
        params.randomize_tiles_to_skip_between_pis = true;
        params.seed                                = 42;

        const auto layout = graph_oriented_layout_design(ntk, params, &stats);
        REQUIRE(layout.has_value());
        check_eq(ntk, required_value(layout));
    }
}

TEST_CASE("Multithreading", "[graph-oriented-layout-design]")
{
    const auto ntk = blueprints::mux21_network<technology_network>();

    graph_oriented_layout_design_stats stats{};
    // enable multithreading for all sections
    graph_oriented_layout_design_params params{.enable_multithreading = true};

    SECTION("Highest-effort mode with multithreading")
    {
        params.mode       = graph_oriented_layout_design_params::effort_mode::HIGHEST_EFFORT;
        const auto layout = graph_oriented_layout_design(ntk, params, &stats);
        REQUIRE(layout.has_value());
        check_eq(ntk, required_value(layout));
    }

    SECTION("High-efficiency mode, return first, multithreading")
    {
        params.seed                                = 42;
        params.randomize_tiles_to_skip_between_pis = true;
        params.tiles_to_skip_between_pis           = 3;
        params.mode                                = graph_oriented_layout_design_params::effort_mode::HIGH_EFFICIENCY;
        params.return_first                        = true;
        const auto layout                          = graph_oriented_layout_design(ntk, params, &stats);
        REQUIRE(layout.has_value());
        check_eq(ntk, required_value(layout));
    }

    SECTION("Maximum-effort mode with seed and multithreading")
    {
        params.mode       = graph_oriented_layout_design_params::effort_mode::MAXIMUM_EFFORT;
        params.seed       = 12345;
        const auto layout = graph_oriented_layout_design(ntk, params, &stats);
        REQUIRE(layout.has_value());
        check_eq(ntk, required_value(layout));
    }
}

TEST_CASE("Different cost objectives", "[graph-oriented-layout-design]")
{
    const auto ntk = blueprints::mux21_network<technology_network>();

    graph_oriented_layout_design_stats stats{};

    graph_oriented_layout_design_params params{.mode =
                                                   graph_oriented_layout_design_params::effort_mode::HIGHEST_EFFORT};

    // array of cost objectives to iterate over
    const std::array cost_objectives = {graph_oriented_layout_design_params::cost_objective::AREA,
                                        graph_oriented_layout_design_params::cost_objective::WIRES,
                                        graph_oriented_layout_design_params::cost_objective::CROSSINGS,
                                        graph_oriented_layout_design_params::cost_objective::ACP};

    // loop over each cost objective
    for (const auto& cost : cost_objectives)
    {
        CAPTURE(cost);
        params.cost       = cost;
        const auto layout = graph_oriented_layout_design(ntk, params, &stats);

        REQUIRE(layout.has_value());
        check_eq(ntk, required_value(layout));
    }
}

TEST_CASE("Skip tiles for PI placement", "[graph-oriented-layout-design]")
{
    const auto ntk = blueprints::clpl<technology_network>();

    graph_oriented_layout_design_stats  stats{};
    graph_oriented_layout_design_params params{.mode =
                                                   graph_oriented_layout_design_params::effort_mode::HIGH_EFFICIENCY,
                                               .return_first = true};

    for (int32_t skip = 0; skip < 5; ++skip)
    {
        SECTION(fmt::format("tiles_to_skip_between_pis = {}", skip))
        {
            params.tiles_to_skip_between_pis = skip;

            const auto layout_opt = graph_oriented_layout_design(ntk, params, &stats);
            REQUIRE(layout_opt.has_value());
            const auto& lyt = required_value(layout_opt);
            check_eq(ntk, lyt);

            // collect PI coordinates along top (y=0) and left (x=0)
            std::vector<int32_t> top_x, left_y;
            lyt.foreach_pi(
                [&](auto const& gate)
                {
                    const auto c = lyt.get_tile(gate);
                    if (c.y == 0)
                    {
                        top_x.push_back(c.x);
                    }
                    if (c.x == 0)
                    {
                        left_y.push_back(c.y);
                    }
                });

            std::ranges::sort(top_x);
            std::ranges::sort(left_y);

            // check gaps between consecutive PIs on each edge
            const auto min_gap = skip + 1;  // after placing a PI, leave `skip` empty tiles before next

            for (std::size_t i = 1; i < top_x.size(); ++i)
            {
                CAPTURE(skip, top_x);
                CHECK(top_x[i] >= top_x[i - 1] + min_gap);
            }
            for (std::size_t i = 1; i < left_y.size(); ++i)
            {
                CAPTURE(skip, left_y);
                CHECK(left_y[i] >= left_y[i - 1] + min_gap);
            }
        }
    }
}

TEST_CASE("Custom cost objective", "[graph-oriented-layout-design]")
{
    const auto ntk = blueprints::mux21_network<technology_network>();

    graph_oriented_layout_design_stats stats{};

    graph_oriented_layout_design_params params{.mode = graph_oriented_layout_design_params::effort_mode::HIGHEST_EFFORT,
                                               .return_first = true,
                                               .cost = graph_oriented_layout_design_params::cost_objective::CUSTOM};

    /** @brief Records callback evaluation without constraining invocation counts. */
    std::atomic_bool custom_cost_evaluated{false};
    /** @brief Caller-owned objective reused across calls and copied into a temporary argument. */
    const std::function<uint64_t(const cartesian_gate_layout&)> custom_cost_objective =
        [&custom_cost_evaluated](const cartesian_gate_layout& layout) -> uint64_t
    {
        custom_cost_evaluated = true;
        return (layout.num_wires() * 2) + layout.num_crossings();
    };

    const auto layout = graph_oriented_layout_design(ntk, params, &stats, custom_cost_objective);

    REQUIRE(layout.has_value());
    check_eq(ntk, required_value(layout));
    CHECK(custom_cost_evaluated.load());
    CHECK(static_cast<bool>(custom_cost_objective));

    // high-effort mode
    params.mode           = graph_oriented_layout_design_params::effort_mode::HIGH_EFFORT;
    custom_cost_evaluated = false;

    const auto layout_high_effort = graph_oriented_layout_design(ntk, params, &stats, custom_cost_objective);

    REQUIRE(layout_high_effort.has_value());
    check_eq(ntk, required_value(layout_high_effort));
    CHECK(custom_cost_evaluated.load());

    // maximum-effort mode
    params.mode           = graph_oriented_layout_design_params::effort_mode::MAXIMUM_EFFORT;
    params.seed           = 12345;
    custom_cost_evaluated = false;

    /** @brief Maximum-effort placement result for the custom cost objective. */
    const auto layout_maximum_effort = graph_oriented_layout_design(
        ntk, params, &stats, std::function<uint64_t(const cartesian_gate_layout&)>{custom_cost_objective});

    REQUIRE(layout_maximum_effort.has_value());
    check_eq(ntk, required_value(layout_maximum_effort));
    CHECK(custom_cost_evaluated.load());
    CHECK(static_cast<bool>(custom_cost_objective));
}

TEST_CASE("Name conservation after graph-oriented layout design", "[graph-oriented-layout-design]")
{
    auto maj = blueprints::maj1_network<mockturtle::aig_network>();
    maj.set_network_name("maj");

    graph_oriented_layout_design_stats        stats{};
    const graph_oriented_layout_design_params params{.return_first = true, .timeout = 100000};

    const auto layout = graph_oriented_layout_design(fiction::synthesis::convert_network<named_technology_network>(maj),
                                                     params, &stats);

    REQUIRE(layout.has_value());

    // network name
    const auto& checked_layout = required_value(layout);
    CHECK(checked_layout.get_layout_name() == "maj");

    // PI names
    CHECK(checked_layout.get_name(checked_layout.pi_at(0)) == "a");  // first PI
    CHECK(checked_layout.get_name(checked_layout.pi_at(1)) == "b");  // second PI
    CHECK(checked_layout.get_name(checked_layout.pi_at(2)) == "c");  // third PI

    // PO names
    CHECK(checked_layout.get_output_name(0) == "f");
}

TEST_CASE("High fanin exception", "[graph-oriented-layout-design]")
{
    const auto ntk = blueprints::maj1_network<technology_network>();

    graph_oriented_layout_design_stats stats{};

    const graph_oriented_layout_design_params params{};

    CHECK_THROWS_AS(graph_oriented_layout_design(ntk, params, &stats), high_degree_fanin_exception);
}

TEST_CASE("No custom cost objective provided exception", "[graph-oriented-layout-design]")
{
    const auto ntk = blueprints::mux21_network<technology_network>();

    graph_oriented_layout_design_stats stats{};

    const graph_oriented_layout_design_params params{
        .mode         = graph_oriented_layout_design_params::effort_mode::HIGHEST_EFFORT,
        .return_first = true,
        .cost         = graph_oriented_layout_design_params::cost_objective::CUSTOM};

    CHECK_THROWS_AS(graph_oriented_layout_design(ntk, params, &stats), std::invalid_argument);
}

TEST_CASE("PI spacing outside of its range is rejected", "[graph-oriented-layout-design]")
{
    const auto ntk = blueprints::mux21_network<technology_network>();

    graph_oriented_layout_design_params params{.return_first = true};

    for (const int32_t skip : {-1, 1'048'577, 2147483647})
    {
        CAPTURE(skip);
        params.tiles_to_skip_between_pis = skip;

        CHECK_THROWS_AS(graph_oriented_layout_design(ntk, params), std::invalid_argument);
    }
}

TEST_CASE("Random PI spacing respects each invocation's parameters", "[graph-oriented-layout-design]")
{
    const auto ntk = blueprints::mux21_network<technology_network>();

    const auto layouts = std::async(std::launch::async,
                                    [&ntk]
                                    {
                                        graph_oriented_layout_design_params params{
                                            .mode = graph_oriented_layout_design_params::effort_mode::HIGH_EFFICIENCY,
                                            .return_first = true,
                                            .seed         = 42};

                                        const auto zero_reference = graph_oriented_layout_design(ntk, params);
                                        params.randomize_tiles_to_skip_between_pis = true;
                                        params.tiles_to_skip_between_pis           = 3;
                                        const auto first_seeded          = graph_oriented_layout_design(ntk, params);
                                        params.tiles_to_skip_between_pis = 0;
                                        params.seed                      = 7;
                                        const auto zero_after            = graph_oriented_layout_design(ntk, params);
                                        params.tiles_to_skip_between_pis = 3;
                                        params.seed                      = 42;
                                        const auto repeated_seeded       = graph_oriented_layout_design(ntk, params);
                                        return std::array{zero_reference, first_seeded, zero_after, repeated_seeded};
                                    })
                             .get();

    for (const auto& lyt : layouts)
    {
        REQUIRE(lyt.has_value());
        check_eq(ntk, required_value(lyt));
    }

    /**
     * Collects PI positions in the network's input order.
     */
    const auto pi_positions = [](const cartesian_gate_layout& lyt)
    {
        std::vector<fiction::layouts::layout_base::coordinate> positions{};
        lyt.foreach_pi([&](const auto& pi) { positions.push_back(lyt.get_tile(pi)); });
        return positions;
    };
    CHECK(pi_positions(required_value(layouts[0])) == pi_positions(required_value(layouts[2])));
    CHECK(pi_positions(required_value(layouts[1])) == pi_positions(required_value(layouts[3])));
}

TEST_CASE("Graph-oriented layout design reports progress", "[graph-oriented-layout-design]")
{
    const auto ntk = blueprints::mux21_network<technology_network>();

    progress_recorder rec{};

    graph_oriented_layout_design_params params{.mode =
                                                   graph_oriented_layout_design_params::effort_mode::HIGH_EFFICIENCY,
                                               .return_first = true,
                                               .on_progress  = rec.callback()};

    params.enable_multithreading = GENERATE(false, true);
    CAPTURE(params.enable_multithreading);

    const auto layout = graph_oriented_layout_design(ntk, params);

    CHECK(static_cast<bool>(params.on_progress));
    REQUIRE(layout.has_value());
    check_eq(ntk, *layout);

    // the number of expansions is unknown in advance
    CHECK(rec.is_consistent("expansions"));
    CHECK(rec.final_count("expansions") > 0);
    const auto reports = rec.reports_of("expansions");
    REQUIRE(!reports.empty());
    CHECK(reports.back().total == 0);
}

// NOLINTEND(bugprone-unchecked-optional-access)

TEST_CASE("GOLD preserves declared PI order and noncommutative input ports", "[graph-oriented-layout-design]")
{
    named_technology_network ntk{};
    const auto               unused = ntk.create_pi("unused");
    const auto               a      = ntk.create_pi("a");
    const auto               b      = ntk.create_pi("b");
    ntk.create_po(ntk.create_lt(b, a), "less");
    ntk.set_network_name("ordered");
    static_cast<void>(unused);
    const graph_oriented_layout_design_params params{.return_first          = true,
                                                     .timeout               = 10000,
                                                     .enable_multithreading = false};
    const auto                                result = graph_oriented_layout_design(ntk, params);
    REQUIRE(result.has_value());
    const auto& layout = required_value(result);
    CHECK(layout.num_pis() == 3);
    CHECK(layout.get_name(layout.pi_at(0)) == "unused");
    CHECK(layout.get_name(layout.pi_at(1)) == "a");
    CHECK(layout.get_name(layout.pi_at(2)) == "b");
    CHECK(layout.get_output_name(0) == "less");
    CHECK(layout.get_layout_name() == "ordered");
    const mockturtle::default_simulator<kitty::dynamic_truth_table> simulator{3};
    const auto                                                      extracted = extract_layout_network(layout);
    CHECK(mockturtle::simulate<kitty::dynamic_truth_table>(extracted, simulator) ==
          mockturtle::simulate<kitty::dynamic_truth_table>(ntk, simulator));
    layout.foreach_object([&](const auto id) { CHECK(layout.is_within_bounds(layout.get_tile(id))); });
}

TEST_CASE("GOLD represents constant outputs explicitly and keeps empty networks empty",
          "[graph-oriented-layout-design]")
{
    const named_technology_network            empty{};
    const graph_oriented_layout_design_params params{.return_first          = true,
                                                     .timeout               = 10000,
                                                     .enable_multithreading = false};
    const auto                                empty_result = graph_oriented_layout_design(empty, params);
    REQUIRE(empty_result.has_value());
    CHECK(required_value(empty_result).is_empty());
    CHECK(required_value(empty_result).area() == 0);
    named_technology_network ntk{};
    ntk.create_po(ntk.get_constant(false), "zero");
    ntk.create_po(ntk.get_constant(true), "one");
    const auto result = graph_oriented_layout_design(ntk, params);
    REQUIRE(result.has_value());
    CHECK(required_value(result).num_pis() == 0);
    CHECK(required_value(result).num_pos() == 2);
    CHECK(required_value(result).num_gates() == 2);
    const mockturtle::default_simulator<kitty::dynamic_truth_table> simulator{0};
    CHECK(mockturtle::simulate<kitty::dynamic_truth_table>(extract_layout_network(required_value(result)), simulator) ==
          mockturtle::simulate<kitty::dynamic_truth_table>(ntk, simulator));
}

TEST_CASE("GOLD preserves mixed constant and logic output order", "[graph-oriented-layout-design]")
{
    named_technology_network ntk{};
    const auto               input = ntk.create_pi("input");
    ntk.create_po(input, "input_copy");
    ntk.create_po(ntk.get_constant(true), "one");
    ntk.create_po(ntk.get_constant(false), "zero");
    const graph_oriented_layout_design_params params{.return_first          = true,
                                                     .timeout               = 10000,
                                                     .enable_multithreading = false};
    const auto                                result = graph_oriented_layout_design(ntk, params);
    REQUIRE(result.has_value());
    CHECK(required_value(result).get_output_name(0) == "input_copy");
    CHECK(required_value(result).get_output_name(1) == "one");
    CHECK(required_value(result).get_output_name(2) == "zero");
    const mockturtle::default_simulator<kitty::dynamic_truth_table> simulator{1};
    CHECK(mockturtle::simulate<kitty::dynamic_truth_table>(extract_layout_network(required_value(result)), simulator) ==
          mockturtle::simulate<kitty::dynamic_truth_table>(ntk, simulator));
}

TEMPLATE_TEST_CASE("Named specification conversion for graph_oriented_layout_design", "[physical-network]",
                   mockturtle::aig_network, mockturtle::xag_network, mockturtle::mig_network)
{
    mockturtle::names_view<TestType> source{};
    source.set_network_name("named_spec");
    const auto a = source.create_pi("left");
    const auto b = source.create_pi("right");
    source.create_po(source.create_and(a, b), "result");
    const auto ntk = fiction::synthesis::convert_network<named_technology_network>(source);
    CHECK(ntk.get_network_name() == "named_spec");
    CHECK(ntk.get_name(ntk.make_signal(ntk.pi_at(0))) == "left");
    CHECK(ntk.get_name(ntk.make_signal(ntk.pi_at(1))) == "right");
    CHECK(ntk.get_output_name(0) == "result");
    const graph_oriented_layout_design_params ps{.return_first = true};
    const auto                                result = graph_oriented_layout_design(ntk, ps);
    REQUIRE(result.has_value());
    const auto& lyt = required_value(result);
    CHECK(lyt.get_layout_name() == "named_spec");
    CHECK(lyt.get_name(lyt.pi_at(0)) == "left");
    CHECK(lyt.get_name(lyt.pi_at(1)) == "right");
    CHECK(lyt.get_output_name(0) == "result");
    const auto extracted = extract_layout_network(lyt);
    CHECK(extracted.get_network_name() == "named_spec");
    CHECK(extracted.get_name(extracted.make_signal(extracted.pi_at(0))) == "left");
    CHECK(extracted.get_name(extracted.make_signal(extracted.pi_at(1))) == "right");
    CHECK(extracted.get_output_name(0) == "result");
    check_eq(source, extracted);
}

TEST_CASE("GOLD repeats completed sequential searches", "[graph-oriented-layout-design]")
{
    /** @brief Exercise fixed-spacing and seeded PI placement. */
    const auto randomize_spacing = GENERATE(false, true);
    /** @brief Captures geometry, functions, terminal kinds, and ordered connections without object identities. */
    const auto snapshot = [](const cartesian_gate_layout& layout)
    {
        /** @brief Placed objects keyed by their coordinates. */
        std::vector<std::tuple<layout_base::coordinate, kitty::dynamic_truth_table, bool, bool>> objects{};
        /** @brief Connections keyed by destination coordinate and input index. */
        std::vector<std::tuple<layout_base::coordinate, uint32_t, layout_base::coordinate>> connections{};
        /** @brief Terminal coordinates in their public input and output order. */
        std::array<std::vector<layout_base::coordinate>, 2> terminals{};
        layout.foreach_pi([&](const auto id) { terminals[0].push_back(layout.get_tile(id)); });
        layout.foreach_po([&](const auto id) { terminals[1].push_back(layout.get_tile(id)); });
        layout.foreach_object(
            [&](const auto id)
            {
                /** @brief Physical position independent of the layout's object identities. */
                const auto position = layout.get_tile(id);
                objects.emplace_back(position, layout.object_function(id), layout.is_pi(id), layout.is_po(id));
                layout.foreach_fanin(id, [&](const auto source, const auto index)
                                     { connections.emplace_back(position, index, layout.get_tile(source)); });
            });
        std::ranges::sort(objects, [](const auto& a, const auto& b) { return std::get<0>(a) < std::get<0>(b); });
        std::ranges::sort(connections);
        return std::tuple{layout.width(),     layout.height(),        layout.layers(),
                          std::move(objects), std::move(connections), std::move(terminals)};
    };

    for (const auto cost : {graph_oriented_layout_design_params::cost_objective::AREA,
                            graph_oriented_layout_design_params::cost_objective::ACP})
    {
        CAPTURE(cost);
        /** @brief Full-adder search exercises fanout substitution and ordered routing. */
        const auto ntk = blueprints::full_adder_network<technology_network>();
        /** @brief Stop at the first solution to keep this seeded-search regression bounded. */
        graph_oriented_layout_design_params params{
            .mode                                = graph_oriented_layout_design_params::effort_mode::HIGH_EFFICIENCY,
            .return_first                        = true,
            .timeout                             = 60000,
            .cost                                = cost,
            .enable_multithreading               = false,
            .seed                                = 7,
            .tiles_to_skip_between_pis           = randomize_spacing ? 1 : 0,
            .randomize_tiles_to_skip_between_pis = randomize_spacing};
        /** @brief Reference result for this input and seed. */
        const auto reference = graph_oriented_layout_design(ntk, params);
        REQUIRE(reference.has_value());
        check_eq(ntk, required_value(reference));

        params.seed                                = 42;
        params.tiles_to_skip_between_pis           = 3;
        params.randomize_tiles_to_skip_between_pis = true;
        /** @brief Intervening invocation with different randomization parameters. */
        const auto intervening = graph_oriented_layout_design(ntk, params);
        REQUIRE(intervening.has_value());
        check_eq(ntk, required_value(intervening));

        params.seed                                = 7;
        params.tiles_to_skip_between_pis           = randomize_spacing ? 1 : 0;
        params.randomize_tiles_to_skip_between_pis = randomize_spacing;
        /** @brief Repeating the original invocation must reproduce its complete physical result. */
        const auto repeated = graph_oriented_layout_design(ntk, params);
        REQUIRE(repeated.has_value());
        check_eq(ntk, required_value(repeated));
        CHECK(snapshot(required_value(reference)) == snapshot(required_value(repeated)));
    }
}

TEST_CASE("GOLD estimates geometric area on one-tile axes", "[graph-oriented-layout-design]")
{
    CHECK(fiction::physical_design::detail::candidate_layout_area(1, 1, {0, 0}) == 1.0);
    CHECK(fiction::physical_design::detail::candidate_layout_area(1, 6, {0, 2}) == 5.0);
    CHECK(fiction::physical_design::detail::candidate_layout_area(6, 1, {2, 0}) == 5.0);
    CHECK(fiction::physical_design::detail::candidate_layout_area(4, 5, {1, 1}) == 12.0);
    CHECK(fiction::physical_design::detail::candidate_layout_area(4, 5, {5, 6}) == 42.0);
}
