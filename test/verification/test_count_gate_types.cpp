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
 * @brief Tests for counting logic gates and placed wire objects.
 */
#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/verification/count_gate_types.hpp>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::networks;
using namespace fiction::verification;

TEST_CASE("Gate counts exclude terminals and classify placed wires", "[count-gate-types]")
{
    gate_level_layout<cartesian_layout> lyt{{4, 2}};
    const auto                          pi   = lyt.create_pi("a", {0, 0});
    const auto                          wire = lyt.create_buf(pi, {1, 0});
    const auto                          gate = lyt.create_and(wire, pi, {2, 0});
    lyt.create_po(gate, "result", {3, 0});
    lyt.create_po(wire, "wire", {1, 1});
    count_gate_types_stats stats{};
    count_gate_types(lyt, &stats);
    CHECK(stats.num_fanout == 1);
    CHECK(stats.num_buf == 0);
    CHECK(stats.num_and2 == 1);
    CHECK(stats.num_other == 0);
}

TEST_CASE("Network gate counts exclude inputs and constants", "[count-gate-types]")
{
    technology_network ntk{};
    const auto         a = ntk.create_pi();
    const auto         b = ntk.create_pi();
    ntk.create_po(ntk.create_and(a, b));
    count_gate_types_stats stats{};
    count_gate_types(ntk, &stats);
    CHECK(stats.num_and2 == 1);
    CHECK(stats.num_buf == 0);
    CHECK(stats.num_other == 0);
}
