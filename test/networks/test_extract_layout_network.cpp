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
 * @brief Tests for extraction of layout logic.
 */
#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/networks/extract_layout_network.hpp>

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <kitty/operations.hpp>
#include <kitty/print.hpp>
#include <mockturtle/algorithms/simulation.hpp>

#include <array>
#include <cstdint>
#include <stdexcept>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::networks;

/** @brief Placed Cartesian objects used by extraction tests. */
using extraction_layout = gate_level_layout<cartesian_layout>;

TEST_CASE("Extraction preserves interface order and truth-table argument indices", "[extract-layout-network]")
{
    extraction_layout          lyt{{1, 1}, "ordered"};
    const auto                 unused = lyt.create_pi("unused", {10, 0});
    const auto                 a      = lyt.create_pi("a", {0, 0});
    const auto                 b      = lyt.create_pi("b", {7, 0});
    kitty::dynamic_truth_table function{2};
    kitty::create_from_hex_string(function, "4");  // !a & b
    const auto gate = lyt.create_node({a, b}, function, {20, 0});
    lyt.set_name(gate, "gate");
    const auto wire   = lyt.create_buf(gate, {30, 0});
    const auto result = lyt.create_po(wire, "result", {40, 0});
    const auto pass   = lyt.create_po(a, "pass", {50, 0});
    lyt.set_input_order(std::array{b.object, a.object, unused.object});
    lyt.set_output_order(std::array{pass.object, result.object});

    const auto ntk = extract_layout_network(lyt);
    REQUIRE(ntk.num_pis() == 3);
    REQUIRE(ntk.num_pos() == 2);
    CHECK(ntk.get_network_name() == "ordered");
    CHECK(ntk.get_name(ntk.make_signal(ntk.pi_at(0))) == "b");
    CHECK(ntk.get_name(ntk.make_signal(ntk.pi_at(1))) == "a");
    CHECK(ntk.get_name(ntk.make_signal(ntk.pi_at(2))) == "unused");
    CHECK(ntk.get_output_name(0) == "pass");
    CHECK(ntk.get_output_name(1) == "result");
    const auto tables = mockturtle::simulate<kitty::dynamic_truth_table>(
        ntk, mockturtle::default_simulator<kitty::dynamic_truth_table>{ntk.num_pis()});
    CHECK(kitty::to_hex(tables[0]) == "cc");
    CHECK(kitty::to_hex(tables[1]) == "22");
}

TEST_CASE("Extraction validates only dependencies of primary outputs", "[extract-layout-network]")
{
    extraction_layout lyt{};
    const auto        a     = lyt.create_pi("a", {0, 0});
    const auto        cycle = lyt.create_buf({1, 0});
    lyt.connect(cycle, {cycle.object, 0});
    kitty::dynamic_truth_table function{2};
    kitty::create_from_hex_string(function, "8");
    const auto gate = lyt.create_node({a}, function, {2, 0});
    lyt.disconnect({gate.object, 0});
    lyt.connect(a, {gate.object, 1});
    const auto po = lyt.create_po(a, "result", {3, 0});
    CHECK_NOTHROW(extract_layout_network(lyt));

    lyt.connect(gate, {po.object, 0});
    CHECK_THROWS_AS(extract_layout_network(lyt), std::invalid_argument);
    lyt.connect(a, {gate.object, 0});
    CHECK_NOTHROW(extract_layout_network(lyt));
    lyt.connect(cycle, {po.object, 0});
    CHECK_THROWS_AS(extract_layout_network(lyt), std::invalid_argument);
    lyt.disconnect({po.object, 0});
    CHECK_THROWS_AS(extract_layout_network(lyt), std::invalid_argument);
}

TEST_CASE("Extraction accepts explicitly placed constant functions", "[extract-layout-network]")
{
    extraction_layout          lyt{};
    kitty::dynamic_truth_table function{0};
    kitty::create_from_hex_string(function, "1");
    const auto constant = lyt.create_node({}, function, {0, 0});
    lyt.create_po(constant, "one", {1, 0});
    const auto ntk = extract_layout_network(lyt);
    REQUIRE(ntk.num_pis() == 0);
    REQUIRE(ntk.num_pos() == 1);
    CHECK(ntk.po_at(0) == ntk.get_constant(true));
}

TEST_CASE("Extraction traverses a million wires without recursive stack growth", "[extract-layout-network]")
{
    extraction_layout lyt{};
    auto              wire = lyt.create_pi("a", {0, 0});
    for (uint32_t index = 1; index <= 1'000'000; ++index)
    {
        wire = lyt.create_buf(wire, {index, 0});
    }
    lyt.create_po(wire, "result", {1'000'001, 0});
    const auto ntk = extract_layout_network(lyt);
    REQUIRE(ntk.num_pis() == 1);
    REQUIRE(ntk.num_pos() == 1);
    CHECK(ntk.po_at(0) == ntk.make_signal(ntk.pi_at(0)));
}

TEST_CASE("Extraction follows connections across allocation order and reused identities", "[extract-layout-network]")
{
    extraction_layout lyt{};
    const auto        removed = lyt.create_pi("removed", {0, 0});
    lyt.remove(removed.object);
    const auto                 po = lyt.create_po("result", {0, 0});
    kitty::dynamic_truth_table function{2};
    kitty::create_from_hex_string(function, "4");  // !a & b
    const auto gate = lyt.create_node({}, function, {1, 0});
    const auto b    = lyt.create_pi("b", {2, 0});
    const auto a    = lyt.create_pi("a", {3, 0});
    lyt.connect(a, {gate.object, 0});
    lyt.connect(b, {gate.object, 1});
    lyt.connect(gate, {po.object, 0});
    const auto ntk = extract_layout_network(lyt);
    REQUIRE(ntk.num_pis() == 2);
    const auto tables = mockturtle::simulate<kitty::dynamic_truth_table>(
        ntk, mockturtle::default_simulator<kitty::dynamic_truth_table>{ntk.num_pis()});
    CHECK(kitty::to_hex(tables[0]) == "2");
}
