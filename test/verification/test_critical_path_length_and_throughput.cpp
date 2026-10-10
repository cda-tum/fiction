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
 * @brief Tests for `fiction/verification/critical_path_length_and_throughput.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/layout_base.hpp>
#include <fiction/verification/critical_path_length_and_throughput.hpp>

#include <cstdint>
#include <stdexcept>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::verification;

/**
 * @brief Checks physical path length and throughput against declared expectations.
 * @tparam Lyt Gate layout type.
 * @param lyt Layout to analyze.
 * @param length Expected longest path in objects.
 * @param throughput Expected throughput denominator.
 */
template <typename Lyt>
void check(const Lyt& lyt, const uint64_t length, const uint64_t throughput)
{
    const auto result = critical_path_length_and_throughput(lyt);
    CHECK(result.critical_path_length == length);
    CHECK(result.throughput == throughput);
}

TEST_CASE("Balanced layout", "[throughput]")
{
    using gate_layout = gate_level_layout<cartesian_layout>;

    check(blueprints::and_or_gate_layout<gate_layout>(), 3, 1);
    check(blueprints::xor_maj_gate_layout<gate_layout>(), 3, 1);
    check(blueprints::or_not_gate_layout<gate_layout>(), 4, 1);
    check(blueprints::fanout_layout<gate_layout>(), 5, 1);
    check(blueprints::crossing_layout<gate_layout>(), 4, 1);

    SECTION("Synchronization Elements")
    {
        using se_gate_layout = gate_level_layout<cartesian_layout>;

        check(blueprints::se_gate_layout<se_gate_layout>(), 4, 1);
    }
}

TEST_CASE("Unbalanced layout", "[throughput]")
{
    using gate_layout = gate_level_layout<cartesian_layout>;

    check(blueprints::unbalanced_and_layout<gate_layout>(), 6, 2);
}

TEST_CASE("Critical path analysis handles long routes", "[throughput]")
{
    using gate_layout = gate_level_layout<cartesian_layout>;

    constexpr int64_t length{1'000'000};
    gate_layout       layout{{length + 1, 2}, clocking::twoddwave()};
    auto              signal = layout.create_pi("in", {0, 0});
    auto              branch = signal;
    for (int64_t x = 1; x < length; ++x)
    {
        signal = x == length / 2 ? layout.create_not(signal, {x, 0}) : layout.create_buf(signal, {x, 0});
        if (x == length / 2)
        {
            branch = signal;
        }
    }
    layout.create_po(signal, "out", {length, 0});
    layout.create_po(branch, "branch", {length / 2, 1});

    const auto result = critical_path_length_and_throughput(layout);
    CHECK(result.critical_path_length == static_cast<uint64_t>(length + 1));
    CHECK(result.throughput == 1);
}

TEST_CASE("Timing rejects required holes and cycles without rejecting dangling objects", "[throughput][placed-objects]")
{
    gate_level_layout<cartesian_layout> lyt{{1, 4}};
    const auto                          pi       = lyt.create_pi("a", {0, 0});
    const auto                          wire     = lyt.create_buf(pi, {0, 1});
    const auto                          po       = lyt.create_po(wire, "result", {0, 2});
    const auto                          dangling = lyt.create_buf({0, 3});
    lyt.connect(dangling, {dangling, 0});
    const auto result = critical_path_length_and_throughput(lyt);
    CHECK(result.critical_path_length == 3);
    CHECK(result.throughput == 1);
    lyt.disconnect({wire, 0});
    CHECK_THROWS_AS(critical_path_length_and_throughput(lyt), std::invalid_argument);
    lyt.connect(po, {wire, 0});
    CHECK_THROWS_AS(critical_path_length_and_throughput(lyt), std::invalid_argument);
}

TEST_CASE("Timing counts declared wire paths independently of placement", "[throughput][placed-objects]")
{
    gate_level_layout<cartesian_layout> lyt{{1, 1}};
    const auto                          pi   = lyt.create_pi("a", {10, 0});
    auto                                path = pi;
    for (int64_t x = 11; x < 15; ++x)
    {
        path = lyt.create_buf(path, {x, 0});
    }
    const auto gate = lyt.create_and(pi, path, {15, 0});
    lyt.create_po(gate, "result", {16, 0});
    const auto result = critical_path_length_and_throughput(lyt);
    CHECK(result.critical_path_length == 7);
    CHECK(result.throughput == 2);
}
