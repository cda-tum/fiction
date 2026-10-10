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
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/arrangement.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/layouts/layout_utils.hpp>
#include <fiction/layouts/shifted_cartesian_layout.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/verification/count_gate_types.hpp>

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/xag.hpp>

#include <initializer_list>

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

TEMPLATE_TEST_CASE("Network gate counts exclude inputs and constants", "[count-gate-types]", technology_network,
                   mockturtle::aig_network, mockturtle::xag_network)
{
    TestType   ntk{};
    const auto a = ntk.create_pi();
    const auto b = ntk.create_pi();
    ntk.create_po(ntk.create_and(a, b));
    count_gate_types_stats stats{};
    count_gate_types(ntk, &stats);
    CHECK(stats.num_and2 == 1);
    CHECK(stats.num_buf == 0);
    CHECK(stats.num_other == 0);
}

TEMPLATE_TEST_CASE("Native gate counts classify supported functions and generic LUTs", "[count-gate-types]",
                   (gate_level_layout<cartesian_layout>), (gate_level_layout<shifted_cartesian_layout>),
                   (gate_level_layout<hexagonal_layout>))
{
    auto       lyt    = make_gate_level_layout<TestType>(arrangement::ODD_ROW, {18, 2}, clocking::open());
    const auto a      = lyt.create_pi("a", {0, 0});
    const auto b      = lyt.create_pi("b", {1, 0});
    const auto c      = lyt.create_pi("c", {2, 0});
    const auto buffer = lyt.create_buf(a, {3, 0});
    const auto fanout = lyt.create_buf(a, {4, 0});
    lyt.create_po(buffer, "buffer", {3, 1});
    lyt.create_not(a, {5, 0});
    lyt.create_and(fanout, b, {6, 0});
    lyt.create_or(a, b, {7, 0});
    lyt.create_nand(fanout, b, {8, 0});
    lyt.create_nor(a, b, {9, 0});
    lyt.create_xor(a, b, {10, 0});
    lyt.create_xnor(a, b, {11, 0});
    lyt.create_lt(a, b, {12, 0});
    lyt.create_gt(a, b, {13, 0});
    lyt.create_le(a, b, {14, 0});
    lyt.create_ge(a, b, {15, 0});
    lyt.create_maj(a, b, c, {16, 0});
    kitty::dynamic_truth_table and3{3};
    kitty::create_from_hex_string(and3, "80");
    lyt.create_gate({a, b, c}, and3, {17, 0});

    count_gate_types_stats stats{};
    count_gate_types(lyt, &stats);
    CHECK(stats.num_fanout == 1);
    CHECK(stats.num_buf == 1);
    CHECK(stats.num_inv == 1);
    CHECK(stats.num_and2 == 1);
    CHECK(stats.num_or2 == 1);
    CHECK(stats.num_nand2 == 1);
    CHECK(stats.num_nor2 == 1);
    CHECK(stats.num_xor2 == 1);
    CHECK(stats.num_xnor2 == 1);
    CHECK(stats.num_lt2 == 1);
    CHECK(stats.num_gt2 == 1);
    CHECK(stats.num_le2 == 1);
    CHECK(stats.num_ge2 == 1);
    CHECK(stats.num_maj3 == 1);
    CHECK(stats.num_other == 1);
    CHECK(stats.num_and3 == 0);
    CHECK(stats.num_xor_and == 0);
    CHECK(stats.num_or_and == 0);
    CHECK(stats.num_onehot == 0);
    CHECK(stats.num_gamble == 0);
    CHECK(stats.num_dot == 0);
    CHECK(stats.num_mux == 0);
    CHECK(stats.num_and_xor == 0);
}

TEST_CASE("Technology network gate counts retain ternary classifications", "[count-gate-types]")
{
    technology_network ntk{};
    const auto         a = ntk.create_pi();
    const auto         b = ntk.create_pi();
    const auto         c = ntk.create_pi();
    ntk.create_maj(a, b, c);
    ntk.create_ite(a, b, c);
    ntk.create_dot(a, b, c);
    for (const auto* hex : {"80", "28", "a8", "16", "81", "6a"})
    {
        kitty::dynamic_truth_table function{3};
        kitty::create_from_hex_string(function, hex);
        ntk.create_node({a, b, c}, function);
    }
    count_gate_types_stats stats{};
    count_gate_types(ntk, &stats);
    CHECK(stats.num_maj3 == 1);
    CHECK(stats.num_mux == 1);
    CHECK(stats.num_dot == 1);
    CHECK(stats.num_and3 == 1);
    CHECK(stats.num_xor_and == 1);
    CHECK(stats.num_or_and == 1);
    CHECK(stats.num_onehot == 1);
    CHECK(stats.num_gamble == 1);
    CHECK(stats.num_and_xor == 1);
    CHECK(stats.num_other == 0);
}
