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
 * @brief Tests extracted layout simulation and mockturtle technology mapping.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/layouts/gate_layout.hpp>
#include <fiction/networks/extract_layout_network.hpp>
#include <fiction/networks/mockturtle_utils.hpp>
#include <fiction/synthesis/technology_mapping_library.hpp>

#include <kitty/bit_operations.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <kitty/print.hpp>
#include <lorina/common.hpp>
#include <lorina/genlib.hpp>
#include <mockturtle/algorithms/mapper.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <mockturtle/io/genlib_reader.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/tech_library.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::networks;
using namespace fiction::synthesis;

/**
 * @brief Checks whether a callback supports filtered element traversal.
 * @tparam Fn Callback type to test.
 */
template <typename Fn>
concept filtered_callback = requires(int* first, Fn fn) {
    mockturtle::detail::foreach_element_if_transform<int*, int>(
        first, first, [](int) { return true; }, [](int value) { return value; }, fn);
};

static_assert(filtered_callback<decltype([](int) {})>);
static_assert(filtered_callback<decltype([](int, uint32_t) {})>);
static_assert(filtered_callback<decltype([](int) { return true; })>);
static_assert(filtered_callback<decltype([](int, uint32_t) { return true; })>);
static_assert(!filtered_callback<int>);

TEST_CASE("Filtered traversal transforms elements and stops on false", "[mockturtle]")
{
    /** Source values traversed through raw-pointer iterators. */
    std::array       values{1, 2, 3, 4, 5, 6};
    const auto       even   = [](const int value) { return value % 2 == 0; };
    const auto       triple = [](const int value) { return 3 * value; };
    std::vector<int> visited{};
    mockturtle::detail::foreach_element_if_transform<int*, int>(
        values.data(), std::next(values.data(), std::ssize(values)), even, triple,
        [&visited](const int value) { visited.push_back(value); });
    CHECK(visited == std::vector<int>{6, 12, 18});

    visited.clear();
    std::vector<uint32_t> indices{};
    auto* const           stopped = mockturtle::detail::foreach_element_if_transform<int*, int>(
        values.data(), std::next(values.data(), std::ssize(values)), even, triple,
        [&visited, &indices](const int value, const uint32_t index)
        {
            visited.push_back(value);
            indices.push_back(index);
            return value < 12;
        },
        5);
    CHECK(stopped == std::next(values.data(), 4));
    CHECK(visited == std::vector<int>{6, 12});
    CHECK(indices == std::vector<uint32_t>{5, 6});
}

TEST_CASE("Extracted layout simulation", "[mockturtle]")
{
    STATIC_REQUIRE(mockturtle::has_compute_v<decltype(extract_layout_network(cartesian_gate_layout{})),
                                             kitty::dynamic_truth_table>);
    SECTION("AND and OR outputs")
    {
        const auto lyt    = blueprints::and_or_gate_layout<cartesian_gate_layout>();
        const auto tables = mockturtle::simulate<kitty::dynamic_truth_table>(
            extract_layout_network(lyt), mockturtle::default_simulator<kitty::dynamic_truth_table>{lyt.num_pis()});
        REQUIRE(tables.size() == 2);
        CHECK(kitty::to_hex(tables[0]) == "8");
        CHECK(kitty::to_hex(tables[1]) == "e");
    }
    SECTION("Majority and XOR outputs")
    {
        const auto lyt    = blueprints::xor_maj_gate_layout<cartesian_gate_layout>();
        const auto tables = mockturtle::simulate<kitty::dynamic_truth_table>(
            extract_layout_network(lyt), mockturtle::default_simulator<kitty::dynamic_truth_table>{lyt.num_pis()});
        REQUIRE(tables.size() == 2);
        CHECK(kitty::to_hex(tables[0]) == "e8");
        CHECK(kitty::to_hex(tables[1]) == "66");  // Two-input XOR in three variables.
    }
    SECTION("Crossing outputs")
    {
        const auto lyt    = blueprints::crossing_layout<cartesian_gate_layout>();
        const auto tables = mockturtle::simulate<kitty::dynamic_truth_table>(
            extract_layout_network(lyt), mockturtle::default_simulator<kitty::dynamic_truth_table>{lyt.num_pis()});
        REQUIRE(tables.size() == 2);
        CHECK(kitty::to_hex(tables[0]) == "8888");  // AND of the first two inputs.
        CHECK(kitty::to_hex(tables[1]) == "f000");  // AND of the last two inputs.
    }
    SECTION("Synchronization elements")
    {
        const auto lyt    = blueprints::se_gate_layout<cartesian_gate_layout>();
        const auto tables = mockturtle::simulate<kitty::dynamic_truth_table>(
            extract_layout_network(lyt), mockturtle::default_simulator<kitty::dynamic_truth_table>{lyt.num_pis()});
        REQUIRE(tables.size() == 1);
        CHECK(kitty::to_hex(tables[0]) == "e");
    }
}

/**
 * @brief Checks that mapping preserves a network's output functions.
 * @tparam Ntk Source network type.
 * @tparam Lib Technology library type.
 * @param ntk Source network.
 * @param lib Technology library.
 */
template <typename Ntk, typename Lib>
void check(const Ntk& ntk, const Lib& lib)
{
    check_eq(ntk, mockturtle::map(ntk, lib));
}

/**
 * @brief Checks majority, n-ary, and full-adder networks against a library.
 * @tparam Lib Technology library type.
 * @param lib Technology library.
 */
template <typename Lib>
void check_all(const Lib& lib)
{
    check(blueprints::maj4_network<mockturtle::aig_network>(), lib);
    check(blueprints::nary_operation_network<mockturtle::aig_network>(), lib);
    check(blueprints::full_adder_network<mockturtle::aig_network>(), lib);
}

TEST_CASE("Technology mapping", "[mockturtle]")
{
    // instantiate a technology mapping library
    std::stringstream library_stream{};

    std::vector<mockturtle::gate> gates{};

    SECTION("QCA ONE library")
    {
        library_stream << GATE_ZERO << GATE_ONE << GATE_BUF << GATE_INV << GATE_AND2 << GATE_OR2 << GATE_MAJ3
                       << DECAY_MAJ3;

        const auto read_genlib_result = lorina::read_genlib(library_stream, mockturtle::genlib_reader{gates});
        REQUIRE(read_genlib_result == lorina::return_code::success);
        const mockturtle::tech_library<3> gate_lib{gates};

        check_all(gate_lib);
    }
    SECTION("Bestagon library")
    {
        library_stream << GATE_ZERO << GATE_ONE << GATE_BUF << GATE_INV << GATE_AND2 << GATE_NAND2 << GATE_OR2
                       << GATE_NOR2 << GATE_XOR2 << GATE_XNOR2;

        const auto read_genlib_result = lorina::read_genlib(library_stream, mockturtle::genlib_reader{gates});
        REQUIRE(read_genlib_result == lorina::return_code::success);
        const mockturtle::tech_library<2> gate_lib{gates};

        check_all(gate_lib);
    }
    SECTION("Marakkalage 3-input library")
    {
        library_stream << GATE_ZERO << GATE_ONE << GATE_BUF << GATE_INV << GATE_AND3 << GATE_XOR_AND << GATE_OR_AND
                       << GATE_ONEHOT << GATE_MAJ3 << GATE_GAMBLE << GATE_DOT << GATE_MUX << GATE_AND_XOR << DECAY_AND3
                       << DECAY_XOR_AND << DECAY_OR_AND << DECAY_ONEHOT << DECAY_MAJ3 << DECAY_GAMBLE << DECAY_DOT
                       << DECAY_MUX << DECAY_AND_XOR;

        const auto read_genlib_result = lorina::read_genlib(library_stream, mockturtle::genlib_reader{gates});
        REQUIRE(read_genlib_result == lorina::return_code::success);
        const mockturtle::tech_library<3> gate_lib{gates};

        check_all(gate_lib);
    }
}

TEST_CASE("Half-adder blueprint names its sum and carry functions", "[mockturtle]")
{
    /** @brief Half-adder outputs evaluated independently of their declared order. */
    const auto network = blueprints::half_adder_network<mockturtle::aig_network>();
    /** @brief Complete output functions for the two primary inputs. */
    const auto functions = mockturtle::simulate<kitty::dynamic_truth_table>(
        network, mockturtle::default_simulator<kitty::dynamic_truth_table>{2});
    REQUIRE(network.num_pos() == 2);
    REQUIRE(functions.size() == 2);

    /** @brief Output names sorted only to check completeness and uniqueness. */
    std::array names{network.get_output_name(0), network.get_output_name(1)};
    std::ranges::sort(names);
    CHECK(names == std::array{std::string{"carry"}, std::string{"sum"}});

    for (uint32_t input{}; input < 4; ++input)
    {
        CAPTURE(input);
        /** @brief Independent arithmetic sum of the two input bits. */
        const auto sum = (input & 1u) + ((input >> 1u) & 1u);
        for (uint32_t output{}; output < network.num_pos(); ++output)
        {
            CAPTURE(output, network.get_output_name(output));
            /** @brief Named bit of the arithmetic sum. */
            const auto expected = network.get_output_name(output) == "sum" ? (sum & 1u) != 0 : (sum & 2u) != 0;
            CHECK(static_cast<bool>(kitty::get_bit(functions[output], input)) == expected);
        }
    }
}
