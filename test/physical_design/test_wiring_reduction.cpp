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
 * @brief Tests for `fiction/physical_design/wiring_reduction.hpp`.
 * @author Simon Hofmann (simon1hofmann)
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"
#include "utils/progress_recorder.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/physical_design/orthogonal.hpp>
#include <fiction/physical_design/routing_utils.hpp>
#include <fiction/physical_design/wiring_reduction.hpp>
#include <fiction/synthesis/network_conversion.hpp>
#include <fiction/types.hpp>

#include <mockturtle/networks/aig.hpp>

#include <cstdint>
#include <stdexcept>
#include <vector>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::networks;
using namespace fiction::physical_design;

/**
 * @brief Checks functional equivalence after wiring reduction.
 * @tparam Lyt Gate layout type.
 * @tparam Ntk Specification network type.
 * @param ntk Specification network.
 */
template <typename Lyt, typename Ntk>
static void check_layout_equiv(const Ntk& ntk)
{
    auto layout = orthogonal<typename Lyt::coordinate_layout>(
        fiction::synthesis::convert_network<named_technology_network>(ntk), {});

    wiring_reduction_stats stats{};
    wiring_reduction(layout, {}, &stats);

    check_eq(ntk, layout);
}

/**
 * @brief Checks distinct network shapes and gate functions.
 * @tparam Lyt Gate layout type.
 */
template <typename Lyt>
static void check_layout_equiv_all()
{
    SECTION("Maj1 Network")
    {
        check_layout_equiv<Lyt>(blueprints::maj1_network<mockturtle::aig_network>());
    }
    SECTION("unbalanced_and_inv_network")
    {
        check_layout_equiv<Lyt>(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>());
    }
    SECTION("and_or_network")
    {
        check_layout_equiv<Lyt>(blueprints::and_or_network<technology_network>());
    }
    SECTION("nary_operation_network")
    {
        check_layout_equiv<Lyt>(blueprints::nary_operation_network<technology_network>());
    }
    SECTION("constant_gate_input_maj_network")
    {
        check_layout_equiv<Lyt>(blueprints::constant_gate_input_maj_network<technology_network>());
    }
    SECTION("half_adder_network")
    {
        check_layout_equiv<Lyt>(blueprints::half_adder_network<technology_network>());
    }
    SECTION("full_adder_network")
    {
        check_layout_equiv<Lyt>(blueprints::full_adder_network<technology_network>());
    }
    SECTION("mux21_network")
    {
        check_layout_equiv<Lyt>(blueprints::mux21_network<technology_network>());
    }
    SECTION("se_coloring_corner_case_network")
    {
        check_layout_equiv<Lyt>(blueprints::se_coloring_corner_case_network<technology_network>());
    }
    SECTION("clpl")
    {
        check_layout_equiv<Lyt>(blueprints::clpl<technology_network>());
    }
    SECTION("nand_xnor_network")
    {
        check_layout_equiv<Lyt>(blueprints::nand_xnor_network<technology_network>());
    }
    SECTION("fanout_substitution_corner_case_network")
    {
        check_layout_equiv<Lyt>(blueprints::fanout_substitution_corner_case_network<technology_network>());
    }
    SECTION("inverter_network")
    {
        check_layout_equiv<Lyt>(blueprints::inverter_network<technology_network>());
    }
}

TEST_CASE("Wiring reduction counts physical wires without terminals", "[wiring-reduction]")
{
    gate_layout<cartesian_layout> layout{{2, 1}, clocking::twoddwave()};
    const auto                    input = layout.create_pi("a", {0, 0});
    layout.create_po(input, "f", {1, 0});
    wiring_reduction_stats stats{};
    wiring_reduction(layout, {}, &stats);
    CHECK(stats.num_wires_before == 0);
    CHECK(stats.num_wires_after == 0);
}

TEST_CASE("Layout equivalence", "[wiring-reduction]")
{
    SECTION("Cartesian layouts")
    {
        check_layout_equiv_all<cartesian_gate_layout>();
    }

    SECTION("Corner Cases")
    {
        auto layout_corner_case_1 = blueprints::optimization_layout_corner_case_outputs_1<cartesian_gate_layout>();
        wiring_reduction_stats stats_corner_case_1{};
        wiring_reduction(layout_corner_case_1, {}, &stats_corner_case_1);
        check_eq(blueprints::optimization_layout_corner_case_outputs_1<cartesian_gate_layout>(), layout_corner_case_1);

        auto layout_corner_case_2 = blueprints::optimization_layout_corner_case_outputs_2<cartesian_gate_layout>();
        wiring_reduction_stats stats_corner_case_2{};
        wiring_reduction(layout_corner_case_2, {}, &stats_corner_case_2);
        check_eq(blueprints::optimization_layout_corner_case_outputs_2<cartesian_gate_layout>(), layout_corner_case_2);

        auto layout_corner_case_3 = blueprints::optimization_layout_corner_case_outputs_3<cartesian_gate_layout>();
        wiring_reduction_stats stats_corner_case_3{};
        wiring_reduction(layout_corner_case_3, {}, &stats_corner_case_3);
        check_eq(blueprints::optimization_layout_corner_case_outputs_3<cartesian_gate_layout>(), layout_corner_case_3);

        auto layout_corner_case_4 = blueprints::optimization_layout_corner_case_outputs_4<cartesian_gate_layout>();
        wiring_reduction_stats stats_corner_case_4{};
        wiring_reduction(layout_corner_case_4, {}, &stats_corner_case_4);
        check_eq(blueprints::optimization_layout_corner_case_outputs_4<cartesian_gate_layout>(), layout_corner_case_4);

        auto layout_corner_case_5 = blueprints::optimization_layout_corner_case_outputs_5<cartesian_gate_layout>();
        wiring_reduction_stats stats_corner_case_5{};
        wiring_reduction(layout_corner_case_5, {}, &stats_corner_case_5);
        check_eq(blueprints::optimization_layout_corner_case_outputs_5<cartesian_gate_layout>(), layout_corner_case_5);

        auto layout_corner_case_inputs = blueprints::optimization_layout_corner_case_inputs<cartesian_gate_layout>();
        wiring_reduction_stats stats_corner_case_inputs{};
        wiring_reduction(layout_corner_case_inputs, {}, &stats_corner_case_inputs);
        check_eq(blueprints::optimization_layout_corner_case_inputs<cartesian_gate_layout>(),
                 layout_corner_case_inputs);
    }

    SECTION("Timeout")
    {
        auto layout = orthogonal<cartesian_layout>(blueprints::mux21_network<technology_network>(), {});

        wiring_reduction_stats        stats{};
        const wiring_reduction_params params{.timeout = 1000000};
        wiring_reduction(layout, params, &stats);

        check_eq(blueprints::mux21_network<technology_network>(), layout);
    }

    SECTION("Timeout exceeded")
    {
        auto layout = orthogonal<cartesian_layout>(blueprints::mux21_network<technology_network>(), {});

        wiring_reduction_stats        stats{};
        const wiring_reduction_params params{.timeout = 0};
        wiring_reduction(layout, params, &stats);

        check_eq(blueprints::mux21_network<technology_network>(), layout);
        CHECK(stats.area_improvement == 0);
    }
}

TEST_CASE("Wrong clocking scheme", "[wiring-reduction]")
{
    auto layout    = blueprints::use_and_gate_layout<cartesian_gate_layout>();
    auto obstr_lyt = cartesian_gate_layout(layout);

    SECTION("Call functions")
    {
        CHECK_NOTHROW(physical_design::detail::create_wiring_reduction_layout(
            obstr_lyt, 1, 1, physical_design::detail::search_direction::HORIZONTAL));

        wiring_reduction_stats stats_wrong_clocking_scheme{};

        CHECK_THROWS_AS(wiring_reduction(obstr_lyt, {}, &stats_wrong_clocking_scheme), std::invalid_argument);
    }
}

TEST_CASE("Search Direction", "[wiring-reduction]")
{
    auto layout    = blueprints::straight_wire_gate_layout<cartesian_gate_layout>();
    auto obstr_lyt = cartesian_gate_layout(layout);

    SECTION("Get")
    {
        auto lyt = physical_design::detail::create_wiring_reduction_layout(
            obstr_lyt, 1, 1, physical_design::detail::search_direction::HORIZONTAL);
        CHECK(lyt.get_search_direction() == physical_design::detail::search_direction::HORIZONTAL);
    }
}

TEST_CASE("Optimization accepts interior terminals", "[wiring-reduction]")
{
    SECTION("Interior primary input")
    {
        auto layout = blueprints::pi_not_in_border_optimization_layout<cartesian_gate_layout>();
        CHECK_NOTHROW(wiring_reduction(layout));
    }

    SECTION("Interior primary output")
    {
        auto layout = blueprints::po_not_in_border_optimization_layout<cartesian_gate_layout>();
        CHECK_NOTHROW(wiring_reduction(layout));
    }
}

TEST_CASE("Wiring reduction reports progress", "[wiring-reduction]")
{
    const auto ntk    = blueprints::mux21_network<technology_network>();
    auto       layout = orthogonal<cartesian_layout>(ntk);

    progress_recorder             rec{};
    const wiring_reduction_params params{.on_progress = rec.callback()};

    wiring_reduction(layout, params);

    check_eq(ntk, layout);

    // the number of wire paths is unknown in advance
    CHECK(rec.is_consistent("wire paths"));
    const auto reports = rec.reports_of("wire paths");
    REQUIRE(!reports.empty());
    CHECK(reports.back().total == 0);
}

TEST_CASE("Wiring reduction preserves identities and ordered input ports", "[wiring-reduction]")
{
    gate_layout<cartesian_layout> layout{{3, 6, 2}, layouts::clocking::twoddwave()};
    const auto                    a     = layout.create_pi("a", {0, 0});
    const auto                    b     = layout.create_pi("b", {2, 0});
    auto                          left  = a;
    auto                          right = b;
    for (int32_t y = 1; y < 4; ++y)
    {
        left  = layout.create_buf(left, {0, y});
        right = layout.create_buf(right, {2, y});
    }
    const auto             gate        = layout.create_lt(left, right, {1, 4});
    const auto             po          = layout.create_po(gate, "f", {1, 5});
    const auto             independent = layout;
    wiring_reduction_stats stats{};
    wiring_reduction(layout, {}, &stats);
    CHECK(layout.contains(a));
    CHECK(layout.contains(b));
    CHECK(layout.contains(gate));
    CHECK(layout.contains(po));
    CHECK(layout.source({po, 0}) == gate);
    const auto objectives = extract_routing_objectives(layout);
    CHECK_THAT(objectives, Catch::Matchers::UnorderedRangeEquals(
                               std::vector<routing_objective>{{layout.get_tile(a), layout.get_tile(gate), 0},
                                                              {layout.get_tile(b), layout.get_tile(gate), 1},
                                                              {layout.get_tile(gate), layout.get_tile(po), 0}}));
    CHECK(layout.is_lt(gate));
    CHECK(layout.num_wires() < independent.num_wires());
    CHECK(independent.height() == 6);
    CHECK(stats.y_size_after == layout.height());
}
TEST_CASE("Wiring reduction handles empty layouts and invalid geometry", "[wiring-reduction]")
{
    gate_layout<cartesian_layout> empty{{}, layouts::clocking::twoddwave()};
    wiring_reduction_stats        stats{};
    wiring_reduction(empty, {}, &stats);
    CHECK(empty.is_empty());
    CHECK(empty.area() == 0);
    CHECK(stats.area_improvement == 0);
    CHECK(stats.wiring_improvement == 0);
    gate_layout<cartesian_layout> invalid{{2, 2}, layouts::clocking::twoddwave()};
    const auto                    outside = invalid.create_buf({-1, 0});
    CHECK_THROWS_AS(wiring_reduction(invalid), std::invalid_argument);
    CHECK(invalid.get_tile(outside) == gate_layout<cartesian_layout>::tile{-1, 0});
}

TEST_CASE("Wiring reduction rejects dimensions beyond its routing range", "[wiring-reduction]")
{
    gate_layout<cartesian_layout> layout{{2147483647, 1}, layouts::clocking::twoddwave()};
    const auto                    object = layout.create_buf({0, 0});
    CHECK_THROWS_AS(wiring_reduction(layout), std::overflow_error);
    CHECK(layout.get_tile(object) == gate_layout<cartesian_layout>::tile{0, 0});
}
