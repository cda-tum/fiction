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
 * @brief Tests for `fiction/physical_design/post_layout_optimization.hpp`.
 * @author Simon Hofmann (simon1hofmann)
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"
#include "utils/progress_recorder.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/physical_design/orthogonal.hpp>
#include <fiction/physical_design/post_layout_optimization.hpp>
#include <fiction/physical_design/routing_utils.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <cstdint>
#include <stdexcept>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::networks;
using namespace fiction::physical_design;

template <typename Lyt, typename Ntk>
static void check_layout_equiv(const Ntk& ntk)
{
    auto layout = orthogonal<Lyt>(ntk, {});

    post_layout_optimization_stats stats{};
    post_layout_optimization<Lyt>(layout, {}, &stats);

    check_eq(ntk, layout);

    CHECK(mockturtle::to_seconds(stats.time_total) > 0);
}

template <typename Lyt>
static void check_layout_equiv_all()
{
    SECTION("maj1_network")
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
    SECTION("inverter_network")
    {
        check_layout_equiv<Lyt>(blueprints::inverter_network<technology_network>());
    }
    SECTION("clpl")
    {
        check_layout_equiv<Lyt>(blueprints::clpl<technology_network>());
    }
    SECTION("fanout_substitution_corner_case_network")
    {
        check_layout_equiv<Lyt>(blueprints::fanout_substitution_corner_case_network<technology_network>());
    }
    SECTION("nand_xnor_network")
    {
        check_layout_equiv<Lyt>(blueprints::nand_xnor_network<technology_network>());
    }
}

TEST_CASE("Layout equivalence", "[post_layout_optimization]")
{
    SECTION("Cartesian layouts")
    {
        using gate_layout = gate_level_layout<cartesian_layout>;

        check_layout_equiv_all<gate_layout>();
    }

    SECTION("Corner cases")
    {
        using gate_layout = gate_level_layout<cartesian_layout>;

        SECTION("optimization_layout_corner_case_outputs_1")
        {
            auto layout_corner_case_1 = blueprints::optimization_layout_corner_case_outputs_1<gate_layout>();
            post_layout_optimization_stats stats_corner_case_1{};
            post_layout_optimization<gate_layout>(layout_corner_case_1, {}, &stats_corner_case_1);
            check_eq(blueprints::optimization_layout_corner_case_outputs_1<gate_layout>(), layout_corner_case_1);
        }

        SECTION("optimization_layout_corner_case_outputs_2")
        {
            auto layout_corner_case_2 = blueprints::optimization_layout_corner_case_outputs_2<gate_layout>();
            post_layout_optimization_stats stats_corner_case_2{};
            post_layout_optimization<gate_layout>(layout_corner_case_2, {}, &stats_corner_case_2);
            check_eq(blueprints::optimization_layout_corner_case_outputs_2<gate_layout>(), layout_corner_case_2);
        }

        SECTION("optimization_layout_corner_case_outputs_3")
        {
            auto layout_corner_case_3 = blueprints::optimization_layout_corner_case_outputs_3<gate_layout>();
            post_layout_optimization_stats stats_corner_case_3{};
            post_layout_optimization<gate_layout>(layout_corner_case_3, {}, &stats_corner_case_3);
            check_eq(blueprints::optimization_layout_corner_case_outputs_3<gate_layout>(), layout_corner_case_3);
        }

        SECTION("optimization_layout_corner_case_outputs_4")
        {
            auto layout_corner_case_4 = blueprints::optimization_layout_corner_case_outputs_4<gate_layout>();
            post_layout_optimization_stats stats_corner_case_4{};
            post_layout_optimization<gate_layout>(layout_corner_case_4, {}, &stats_corner_case_4);
            check_eq(blueprints::optimization_layout_corner_case_outputs_4<gate_layout>(), layout_corner_case_4);
        }

        SECTION("optimization_layout_corner_case_outputs_5")
        {
            auto layout_corner_case_5 = blueprints::optimization_layout_corner_case_outputs_5<gate_layout>();
            post_layout_optimization_stats stats_corner_case_5{};
            post_layout_optimization<gate_layout>(layout_corner_case_5, {}, &stats_corner_case_5);
            check_eq(blueprints::optimization_layout_corner_case_outputs_5<gate_layout>(), layout_corner_case_5);
        }

        SECTION("optimization_layout_corner_case_inputs")
        {
            auto layout_corner_case_3 = blueprints::optimization_layout_corner_case_inputs<gate_layout>();
            post_layout_optimization_stats stats_corner_case_3{};
            post_layout_optimization<gate_layout>(layout_corner_case_3, {}, &stats_corner_case_3);
            check_eq(blueprints::optimization_layout_corner_case_inputs<gate_layout>(), layout_corner_case_3);
        }
    }

    SECTION("Maximum gate relocations")
    {
        using gate_layout = gate_level_layout<cartesian_layout>;

        for (int64_t max_gate_relocations = 0; max_gate_relocations < 10; max_gate_relocations++)
        {
            auto layout = orthogonal<gate_layout>(blueprints::mux21_network<technology_network>(), {});

            post_layout_optimization_stats  stats{};
            post_layout_optimization_params params{};
            params.max_gate_relocations = max_gate_relocations;
            post_layout_optimization<gate_layout>(layout, params, &stats);

            check_eq(blueprints::mux21_network<technology_network>(), layout);
        }
    }

    SECTION("Optimize POs only")
    {
        using gate_layout = gate_level_layout<cartesian_layout>;

        auto layout = orthogonal<gate_layout>(blueprints::mux21_network<technology_network>(), {});

        post_layout_optimization_stats  stats{};
        post_layout_optimization_params params{};
        params.optimize_pos_only = true;
        post_layout_optimization<gate_layout>(layout, params, &stats);

        check_eq(blueprints::mux21_network<technology_network>(), layout);
    }

    SECTION("Timeout")
    {
        using gate_layout = gate_level_layout<cartesian_layout>;

        auto layout = orthogonal<gate_layout>(blueprints::mux21_network<technology_network>(), {});

        post_layout_optimization_stats  stats{};
        post_layout_optimization_params params{};
        params.timeout = 1000000;
        post_layout_optimization<gate_layout>(layout, params, &stats);

        check_eq(blueprints::mux21_network<technology_network>(), layout);
    }

    SECTION("Timeout exceeded")
    {
        using gate_layout = gate_level_layout<cartesian_layout>;

        auto layout = orthogonal<gate_layout>(blueprints::mux21_network<technology_network>(), {});

        post_layout_optimization_stats  stats{};
        post_layout_optimization_params params{};
        params.timeout = 0;
        post_layout_optimization<gate_layout>(layout, params, &stats);

        check_eq(blueprints::mux21_network<technology_network>(), layout);
        CHECK(stats.area_improvement == 0);
    }

    SECTION("Planar optimization with planar layout")
    {
        using gate_layout = gate_level_layout<cartesian_layout>;

        auto layout = blueprints::planar_unoptimized_layout<gate_layout>();

        post_layout_optimization_stats  stats{};
        post_layout_optimization_params params{};
        params.planar_optimization = true;
        post_layout_optimization<gate_layout>(layout, params, &stats);

        check_eq(blueprints::planar_unoptimized_layout<gate_layout>(), layout);
        CHECK(layout.layers() == 1);
    }

    SECTION("Planar optimization with crossing layout")
    {
        using gate_layout = gate_level_layout<cartesian_layout>;

        auto layout = blueprints::planar_optimization_layout<gate_layout>();

        post_layout_optimization_stats  stats{};
        post_layout_optimization_params params{};

        params.planar_optimization = true;
        post_layout_optimization<gate_layout>(layout, params, &stats);
        /** @brief Object at the position whose inverter has been moved. */
        const auto planar_object = layout.find_object({1, 0});
        CHECK((planar_object.has_value() && !layout.is_inv(*planar_object)));

        params.planar_optimization = false;
        post_layout_optimization<gate_layout>(layout, params, &stats);
        /** @brief Object at the position whose inverter has been restored. */
        const auto crossing_object = layout.find_object({1, 0});
        CHECK((crossing_object.has_value() && layout.is_inv(*crossing_object)));
    }
}

TEST_CASE("Wrong clocking scheme", "[post_layout_optimization]")
{
    using gate_layout = gate_level_layout<cartesian_layout>;

    auto layout    = blueprints::use_and_gate_layout<gate_layout>();
    auto obstr_lyt = gate_layout(layout);

    SECTION("Call functions")
    {
        post_layout_optimization_stats stats_wrong_clocking_scheme{};

        CHECK_THROWS_AS(post_layout_optimization<gate_layout>(obstr_lyt, {}, &stats_wrong_clocking_scheme),
                        std::invalid_argument);
    }
}

TEST_CASE("Optimization accepts interior terminals", "[post_layout_optimization]")
{
    using gate_layout = gate_level_layout<cartesian_layout>;

    SECTION("Interior primary input")
    {
        auto layout = blueprints::pi_not_in_border_optimization_layout<gate_layout>();
        CHECK_NOTHROW(post_layout_optimization<gate_layout>(layout));
    }

    SECTION("Interior primary output")
    {
        auto layout = blueprints::po_not_in_border_optimization_layout<gate_layout>();
        CHECK_NOTHROW(post_layout_optimization<gate_layout>(layout));
    }

    SECTION("PO have to be moved to borders during optimization")
    {
        auto layout = blueprints::po_have_to_be_moved_to_border_optimization_layout<gate_layout>();
        post_layout_optimization<gate_layout>(layout);

        layout.foreach_pi(
            [&layout](const auto& pi) noexcept
            {
                const auto tile = layout.get_tile(pi);
                CHECK((layout.is_at_northern_border(tile) || layout.is_at_western_border(tile)));
            });

        layout.foreach_po(
            [&layout](const auto& po) noexcept
            {
                const auto tile = layout.get_tile(po);
                CHECK((layout.is_at_eastern_border(tile) || layout.is_at_southern_border(tile)));
            });
    }
}

TEST_CASE("Post-layout optimization reports progress", "[post_layout_optimization]")
{
    using gate_layout = gate_level_layout<cartesian_layout>;

    const auto ntk    = blueprints::mux21_network<technology_network>();
    auto       layout = orthogonal<gate_layout>(ntk);

    progress_recorder               rec{};
    post_layout_optimization_params params{};
    params.on_progress = rec.callback();

    post_layout_optimization<gate_layout>(layout, params);

    check_eq(ntk, layout);

    // every optimization pass restarts the relocation count
    CHECK(rec.is_consistent("gate relocations"));
    // the nested wiring reduction reports through the same callback
    CHECK(rec.is_consistent("wire paths"));
}

TEST_CASE("Post-layout optimization edits its caller and preserves terminal identities", "[post-layout-ports]")
{
    gate_level_layout<cartesian_layout> layout{{6, 3}, layouts::clocking::twoddwave()};
    const auto                          a           = layout.create_pi("a", {0, 0});
    const auto                          wire        = layout.create_buf(a, {1, 0});
    const auto                          po          = layout.create_po(wire, "f", {5, 0});
    const auto                          independent = layout;
    post_layout_optimization_params     params{};
    params.timeout = 0;
    post_layout_optimization_stats stats{};
    post_layout_optimization(layout, params, &stats);
    CHECK(layout.contains(a.object));
    CHECK(layout.contains(po.object));
    CHECK(layout.source({po.object, 0}) == wire);
    CHECK(layout.width() == 6);
    CHECK(layout.height() == 1);
    CHECK(independent.height() == 3);
    CHECK(stats.y_size_before == 3);
    CHECK(stats.y_size_after == 1);
}

TEST_CASE("Post-layout optimization rejects outside objects without mutation", "[post-layout-ports]")
{
    gate_level_layout<cartesian_layout> layout{{4, 4}, layouts::clocking::twoddwave()};
    const auto                          pi = layout.create_pi("a", {-1, 1});
    const auto                          po = layout.create_po(pi, "f", {3, 1});
    CHECK_THROWS_AS(post_layout_optimization(layout), std::invalid_argument);
    CHECK(layout.get_tile(pi.object) == gate_level_layout<cartesian_layout>::tile{-1, 1});
    CHECK(layout.source({po.object, 0}) == pi);
    CHECK(layout.height() == 4);
}

TEST_CASE("Post-layout rerouting preserves noncommutative input order", "[post-layout-ports]")
{
    gate_level_layout<cartesian_layout> layout{{4, 4, 2}, layouts::clocking::twoddwave()};
    const auto                          a    = layout.create_pi("a", {0, 0});
    const auto                          b    = layout.create_pi("b", {0, 2});
    auto                                left = a;
    for (const gate_level_layout<cartesian_layout>::tile t :
         {gate_level_layout<cartesian_layout>::tile{1, 0}, {2, 0}, {3, 0}, {3, 1}})
    {
        left = layout.create_buf(left, t);
    }
    const auto                      right = layout.create_buf(layout.create_buf(b, {1, 2}), {2, 2});
    const auto                      gate  = layout.create_lt(left, right, {3, 2});
    const auto                      po    = layout.create_po(gate, "f", {3, 3});
    post_layout_optimization_params params{};
    params.max_gate_relocations = 20;
    post_layout_optimization(layout, params);
    CHECK(layout.contains(gate.object));
    CHECK(layout.contains(po.object));
    CHECK(layout.is_lt(gate.object));
    const auto objectives = extract_routing_objectives(layout);
    CHECK(std::ranges::find(objectives,
                            routing_objective<gate_level_layout<cartesian_layout>>{
                                layout.get_tile(a.object), layout.get_tile(gate.object), 0}) != objectives.end());
    CHECK(std::ranges::find(objectives,
                            routing_objective<gate_level_layout<cartesian_layout>>{
                                layout.get_tile(b.object), layout.get_tile(gate.object), 1}) != objectives.end());
    CHECK(layout.area() <= 16);
}
