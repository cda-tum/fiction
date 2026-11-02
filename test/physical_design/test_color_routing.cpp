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
 * @brief Tests for `fiction/physical_design/color_routing.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/layouts/layout_base.hpp>
#include <fiction/physical_design/color_routing.hpp>
#include <fiction/physical_design/routing_utils.hpp>
#include <fiction/utils/graph/graph_coloring.hpp>

#include <stdexcept>
#include <vector>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::physical_design;
using namespace fiction::utils::graph;

TEST_CASE("Color routing validates endpoints before changing the layout", "[color-routing]")
{
    /** @brief Layout whose existing connection must survive failed routing. */
    cartesian_gate_layout layout{{2, 2}, clocking::twoddwave()};
    /** @brief Existing input and buffer. */
    const auto input = layout.create_pi("input", {0, 0});
    /** @brief Existing connected buffer. */
    const auto output = layout.create_buf(input, {1, 1});
    /** @brief Objective with an out-of-extent source. */
    const std::vector<routing_objective> objectives{{.source = {2, 0}, .target = {1, 1}, .input_index = 0}};
    /** @brief Path limit for each contract check. */
    color_routing_params params{};
    CHECK_THROWS_AS(color_routing(layout, objectives, params), std::invalid_argument);
    params.path_limit = 1;
    CHECK_THROWS_AS(color_routing(layout, objectives, params), std::invalid_argument);
    params.path_limit = 0;
    CHECK_FALSE(color_routing(layout, objectives, params));
    CHECK(layout.source({output, 0}) == input);
    CHECK(layout.num_wires() == 1);
    CHECK(layout.get_tile(output) == layout_base::coordinate{1, 1});
}

/**
 * @brief Requires routing success before checking functional equivalence.
 * @tparam Spec Specification layout type.
 * @tparam Impl Implementation layout type.
 * @param spec Specification layout.
 * @param impl Layout to route.
 * @param objectives Connections to route.
 * @param ps Routing parameters.
 */
template <typename Spec, typename Impl>
void check_color_routing(const Spec& spec, Impl& impl, const std::vector<routing_objective>& objectives,
                         color_routing_params ps = {})
{
    const auto success = color_routing(impl, objectives, ps);

    REQUIRE(success);

    check_eq(spec, impl);
}

TEST_CASE("Simple wire connection", "[color-routing]")
{
    auto spec_layout = blueprints::straight_wire_gate_layout<cartesian_gate_layout>();
    auto impl_layout = blueprints::straight_wire_gate_layout<cartesian_gate_layout>();

    const auto objectives = extract_routing_objectives(impl_layout);

    // remove the wire routing from the implementation
    clear_routing(impl_layout);

    /** @brief Independent graph-coloring backend. */
    const auto engine = GENERATE(graph_coloring_engine::MCS, graph_coloring_engine::SAT);
    CAPTURE(engine);
    const color_routing_params ps{.engine = engine};
    check_color_routing(spec_layout, impl_layout, objectives, ps);
}

TEST_CASE("Color routing preserves destination input indices", "[color-routing]")
{
    cartesian_gate_layout layout{{2, 2}, clocking::twoddwave()};
    const auto            a    = layout.create_pi("a", {0, 1});
    const auto            b    = layout.create_pi("b", {1, 0});
    const auto            gate = layout.create_lt(a, b, {1, 1});
    layout.disconnect({gate, 0});
    layout.disconnect({gate, 1});
    const std::vector<routing_objective> objectives{{.source = {1, 0}, .target = {1, 1}, .input_index = 1},
                                                    {.source = {0, 1}, .target = {1, 1}, .input_index = 0}};

    REQUIRE(color_routing(layout, objectives));
    CHECK(layout.source({gate, 0}) == a);
    CHECK(layout.source({gate, 1}) == b);
}

TEST_CASE("Color routing connects repeated sources to distinct inputs", "[color-routing]")
{
    cartesian_gate_layout layout{{2, 2}, clocking::twoddwave()};
    const auto            a          = layout.create_pi("a", {0, 0});
    const auto            gate       = layout.create_lt(a, a, {1, 1});
    const auto            objectives = extract_routing_objectives(layout);
    clear_routing(layout);

    /** @brief Crossing mode for this independent route. */
    const auto crossings = GENERATE(false, true);
    CAPTURE(crossings);
    const color_routing_params ps{.crossings = crossings};

    REQUIRE(color_routing(layout, objectives, ps));
    const auto first_source  = layout.source({gate, 0});
    const auto second_source = layout.source({gate, 1});
    REQUIRE(first_source.has_value());
    REQUIRE(second_source.has_value());
    CHECK(first_source != second_source);
    /** @brief Routed object on the first gate input. */
    const auto first_wire = *first_source;  // NOLINT(bugprone-unchecked-optional-access): REQUIRE passed.
    /** @brief Routed object on the second gate input. */
    const auto second_wire = *second_source;  // NOLINT(bugprone-unchecked-optional-access): REQUIRE passed.
    CHECK(layout.source({first_wire, 0}) == a);
    CHECK(layout.source({second_wire, 0}) == a);
}

TEST_CASE("Two paths wire connections", "[color-routing]")
{
    auto spec_layout = blueprints::unbalanced_and_layout<cartesian_gate_layout>();
    auto impl_layout = blueprints::unbalanced_and_layout<cartesian_gate_layout>();

    const auto objectives = extract_routing_objectives(impl_layout);

    // remove the wire routing from the implementation
    clear_routing(impl_layout);

    /** @brief Independent graph-coloring backend. */
    const auto engine = GENERATE(graph_coloring_engine::MCS, graph_coloring_engine::SAT);
    CAPTURE(engine);
    const color_routing_params ps{.engine = engine};
    check_color_routing(spec_layout, impl_layout, objectives, ps);
}

TEST_CASE("Three paths wire connections", "[color-routing]")
{
    auto spec_layout = blueprints::three_wire_paths_gate_layout<cartesian_gate_layout>();
    auto impl_layout = blueprints::three_wire_paths_gate_layout<cartesian_gate_layout>();

    const auto objectives = extract_routing_objectives(impl_layout);

    // remove the wire routing from the implementation
    clear_routing(impl_layout);

    /** @brief Independent graph-coloring backend. */
    const auto engine = GENERATE(graph_coloring_engine::MCS, graph_coloring_engine::SAT);
    CAPTURE(engine);
    const color_routing_params ps{.engine = engine};
    check_color_routing(spec_layout, impl_layout, objectives, ps);
}

TEST_CASE("Direct gate connections", "[color-routing]")
{
    auto spec_layout = blueprints::xor_maj_gate_layout<cartesian_gate_layout>();
    auto impl_layout = blueprints::xor_maj_gate_layout<cartesian_gate_layout>();

    const auto objectives = extract_routing_objectives(impl_layout);

    // remove the wire routing from the implementation
    clear_routing(impl_layout);

    /** @brief Independent graph-coloring backend. */
    const auto engine = GENERATE(graph_coloring_engine::MCS, graph_coloring_engine::SAT);
    CAPTURE(engine);
    const color_routing_params ps{.engine = engine};
    check_color_routing(spec_layout, impl_layout, objectives, ps);
}

TEST_CASE("Partial routing", "[color-routing]")
{
    auto spec_layout = blueprints::use_and_gate_layout<cartesian_gate_layout>();
    auto impl_layout = blueprints::use_and_gate_layout<cartesian_gate_layout>();

    auto objectives = extract_routing_objectives(impl_layout);
    objectives.push_back({.source = {0, 3}, .target = {3, 0}});  // additional unsatisfiable objective

    // remove the wire routing from the implementation
    clear_routing(impl_layout);

    /** @brief Independent graph-coloring backend. */
    const auto engine = GENERATE(graph_coloring_engine::MCS, graph_coloring_engine::SAT);
    CAPTURE(engine);
    const color_routing_params ps{.conduct_partial_routing = true, .engine = engine};
    check_color_routing(spec_layout, impl_layout, objectives, ps);
}

TEST_CASE("Routing with crossings", "[color-routing]")
{
    auto spec_layout = blueprints::crossing_layout<cartesian_gate_layout>();
    auto impl_layout = blueprints::crossing_layout<cartesian_gate_layout>();

    auto objectives = extract_routing_objectives(impl_layout);

    // remove the wire routing from the implementation
    clear_routing(impl_layout);

    SECTION("Without path limit")
    {
        color_routing_params ps{.crossings = true};
        SECTION("MCS")
        {
            ps.engine = graph_coloring_engine::MCS;
            check_color_routing(spec_layout, impl_layout, objectives, ps);
        }
        SECTION("SAT")
        {
            ps.engine = graph_coloring_engine::SAT;
            check_color_routing(spec_layout, impl_layout, objectives, ps);
        }
    }
    SECTION("With path limit")
    {
        color_routing_params ps{.crossings = true, .path_limit = 2};

        SECTION("MCS")
        {
            ps.engine = graph_coloring_engine::MCS;
            check_color_routing(spec_layout, impl_layout, objectives, ps);
        }
        SECTION("SAT")
        {
            ps.engine = graph_coloring_engine::SAT;
            check_color_routing(spec_layout, impl_layout, objectives, ps);
        }
    }
}

TEST_CASE("Routing failure", "[color-routing]")
{
    cartesian_gate_layout layout{{4, 5, 2}, clocking::twoddwave()};

    const auto x1 = layout.create_pi("x1", {0, 1});
    const auto x2 = layout.create_pi("x2", {1, 0});

    layout.create_pi("x3", {0, 2});
    layout.create_and(x1, x2, {1, 3});

    const std::vector<routing_objective> objectives{{.source = {0, 1}, .target = {1, 3}, .input_index = 0},
                                                    {.source = {1, 0}, .target = {1, 3}, .input_index = 1}};

    SECTION("MCS")
    {
        const color_routing_params ps{.crossings = true, .engine = graph_coloring_engine::MCS};
        // routing should fail
        CHECK_FALSE(color_routing(layout, objectives, ps));
    }
    SECTION("SAT")
    {
        const color_routing_params ps{.crossings = true, .engine = graph_coloring_engine::SAT};
        // routing should fail
        CHECK_FALSE(color_routing(layout, objectives, ps));
    }
}

TEST_CASE("Routing failure 2", "[color-routing]")
{
    cartesian_gate_layout layout{{4, 5, 2}, clocking::twoddwave()};

    const auto x1 = layout.create_pi("x1", {0, 1});
    const auto x2 = layout.create_pi("x2", {1, 0});

    layout.create_pi("x3", {0, 3});
    layout.create_and(x1, x2, {1, 3});

    const std::vector<routing_objective> objectives{{.source = {0, 1}, .target = {1, 3}, .input_index = 0},
                                                    {.source = {1, 0}, .target = {1, 3}, .input_index = 1}};

    SECTION("MCS")
    {
        const color_routing_params ps{.crossings = true, .engine = graph_coloring_engine::MCS};
        // routing should fail
        CHECK_FALSE(color_routing(layout, objectives, ps));
    }
    SECTION("SAT")
    {
        const color_routing_params ps{.crossings = true, .engine = graph_coloring_engine::SAT};
        // routing should fail
        CHECK_FALSE(color_routing(layout, objectives, ps));
    }
}

TEST_CASE("Routing failure 3", "[color-routing]")
{
    cartesian_gate_layout layout{{3, 5, 2}, clocking::twoddwave()};

    const auto x1 = layout.create_pi("x1", {0, 0});
    const auto w1 = layout.create_buf(x1, {0, 1});
    const auto x2 = layout.create_pi("x2", {1, 0});
    const auto w2 = layout.create_buf(x2, {1, 1});
    const auto a1 = layout.create_and(w1, w2, {2, 1});

    SECTION("Wires without connections")
    {
        color_routing_params ps{.crossings = true};
        layout.disconnect({a1, 0});
        layout.disconnect({a1, 1});

        const std::vector<routing_objective> objectives{{.source = {0, 1}, .target = {2, 1}, .input_index = 0},
                                                        {.source = {1, 1}, .target = {2, 1}, .input_index = 1}};

        SECTION("k = 3")
        {
            ps.path_limit = 3;
            CHECK_FALSE(color_routing(layout, objectives, ps));
        }
        SECTION("no path limit")
        {
            CHECK_FALSE(color_routing(layout, objectives, ps));
        }
    }
    SECTION("Goal node behind wire crossing")
    {
        color_routing_params ps{.crossings = true};
        layout.disconnect({a1, 0});

        const std::vector<routing_objective> objectives{{.source = {0, 1}, .target = {2, 1}, .input_index = 0},
                                                        {.source = {1, 1}, .target = {2, 1}, .input_index = 1}};

        SECTION("k = 3")
        {
            ps.path_limit = 3;
            CHECK_FALSE(color_routing(layout, objectives, ps));
        }
        SECTION("no path limit")
        {
            CHECK_FALSE(color_routing(layout, objectives, ps));
        }
    }
}
