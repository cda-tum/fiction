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
 * @brief Benchmarks for A* routing and its layout optimization callers.
 */

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../utils/blueprints/network_blueprints.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/physical_design/graph_oriented_layout_design.hpp>
#include <fiction/physical_design/orthogonal.hpp>
#include <fiction/physical_design/path_finding/a_star.hpp>
#include <fiction/physical_design/post_layout_optimization.hpp>
#include <fiction/physical_design/wiring_reduction.hpp>
#include <fiction/synthesis/network_conversion.hpp>
#include <fiction/types.hpp>
#include <fiction/verification/equivalence_checking.hpp>

#include <mockturtle/networks/aig.hpp>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::physical_design;
using namespace fiction::physical_design::path_finding;
using namespace fiction::verification;

TEST_CASE("Benchmark A* routing callers", "[benchmark]")
{
    /** @brief Technology network shared by the routing workloads. */
    const auto network = synthesis::convert_network<tec_nt>(blueprints::parity_network<mockturtle::aig_network>());
    /** @brief Routed input cloned inside each optimization measurement. */
    const auto layout = orthogonal<cartesian_layout>(network);

    /** @brief Wiring reduction measurement with input cloning included. */
    const auto reduce_wiring = [&]()
    {
        /** @brief Independent layout for one routing run. */
        auto result = layout.clone();
        wiring_reduction(result);
        return result;
    };
    REQUIRE(equivalence_checking(network, reduce_wiring()) != eq_type::NO);
    BENCHMARK("wiring_reduction: parity")
    {
        return reduce_wiring();
    };

    /** @brief Bounded gate relocation workload without a time cutoff. */
    post_layout_optimization_params post_params{};
    post_params.max_gate_relocations = 8;
    /** @brief Post-layout optimization measurement with input cloning included. */
    const auto optimize_layout = [&]()
    {
        /** @brief Independent layout for one optimization run. */
        auto result = layout.clone();
        post_layout_optimization(result, post_params);
        return result;
    };
    REQUIRE(equivalence_checking(network, optimize_layout()) != eq_type::NO);
    BENCHMARK("post_layout_optimization: parity, max_gate_relocations=8")
    {
        return optimize_layout();
    };

    /** @brief Repeatable sequential GOLD search parameters. */
    graph_oriented_layout_design_params gold_params{};
    gold_params.seed         = 1340;
    gold_params.return_first = true;
    gold_params.mode         = graph_oriented_layout_design_params::effort_mode::HIGH_EFFORT;
    /** @brief GOLD result checked outside the timed region. */
    const auto gold_layout = graph_oriented_layout_design(network, gold_params);
    REQUIRE(gold_layout.has_value());
    REQUIRE(equivalence_checking(network, *gold_layout) != eq_type::NO);
    BENCHMARK("graph_oriented_layout_design: parity, seed 1340")
    {
        return graph_oriented_layout_design(network, gold_params);
    };
}

TEST_CASE("Benchmark A* open grid", "[benchmark]")
{
    /** @brief Large frontier workload independent of gate placement. */
    const cartesian_layout layout{{256, 256}};
    REQUIRE(a_star(layout, {{0, 0}, {255, 255}}).size() == 511);
    BENCHMARK("a_star: open 256 by 256 grid")
    {
        return a_star(layout, {{0, 0}, {255, 255}});
    };
}
