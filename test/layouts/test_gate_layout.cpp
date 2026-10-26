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
 * @brief Tests for `fiction/layouts/gate_layout.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#include <fiction/layouts/gate_layout.hpp>

// Test dependencies follow the defining header to expose missing direct includes.
#include <catch2/catch_message.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include "utils/allocation_failure.hpp"
#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/arrangement.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/layouts/layout_base.hpp>
#include <fiction/layouts/shifted_cartesian_layout.hpp>

#include <kitty/bit_operations.hpp>
#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
#include <set>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

using namespace fiction;
using namespace fiction::test;
using namespace fiction::layouts;

using Catch::Matchers::RangeEquals;
using Catch::Matchers::UnorderedRangeEquals;

/** @brief Coordinate geometries covered by gate-layout fixture tests. */
using gate_geometries = std::tuple<cartesian_layout, shifted_cartesian_layout, hexagonal_layout>;

/** @brief Checks whether a geometry can back a gate layout. @tparam Geometry Coordinate geometry. */
template <typename Geometry>
concept gate_geometry = requires { typename gate_layout<Geometry>; };

static_assert(gate_geometry<cartesian_layout>);
static_assert(!gate_geometry<cartesian_gate_layout>);
static_assert(!gate_geometry<hexagonal_gate_layout>);

/** @brief Two independently named inputs shared by connectivity and ownership cases. @tparam Geometry Base geometry. */
template <typename Geometry>
struct gate_layout_fixture
{
    /** @brief Layout with room for placement and connectivity edits. */
    gate_layout<Geometry> lyt = blueprints::make_layout<gate_layout<Geometry>>(arrangement::EVEN_ROW, {8, 8});
    /** @brief First logical input. */
    const layout_object_id a = lyt.create_pi("a", {0, 0});
    /** @brief Second logical input. */
    const layout_object_id b = lyt.create_pi("b", {1, 0});
};

TEST_CASE("Canonical gate layouts expose their geometry", "[gate-layout]")
{
    STATIC_REQUIRE(std::is_same_v<gate_layout<>, cartesian_gate_layout>);
    STATIC_REQUIRE(std::is_same_v<cartesian_gate_layout, gate_layout<cartesian_layout>>);
    STATIC_REQUIRE(std::is_same_v<hexagonal_gate_layout, gate_layout<hexagonal_layout>>);
    STATIC_REQUIRE(std::is_same_v<shifted_cartesian_gate_layout, gate_layout<shifted_cartesian_layout>>);
    STATIC_REQUIRE(std::is_default_constructible_v<cartesian_gate_layout>);
    STATIC_REQUIRE_FALSE(std::is_default_constructible_v<hexagonal_gate_layout>);
    STATIC_REQUIRE_FALSE(std::is_default_constructible_v<shifted_cartesian_gate_layout>);

    SECTION("Cartesian extent and placement")
    {
        gate_layout<> lyt{{2, 2, 1}};
        STATIC_REQUIRE(std::is_same_v<decltype(lyt.clone()), cartesian_gate_layout>);
        CHECK(lyt.width() == 2);
        CHECK(lyt.height() == 2);
        CHECK(lyt.layers() == 1);
        const auto input = lyt.create_pi("a", {1, 1});
        CHECK(lyt.get_tile(input) == layout_base::coordinate{1, 1});
    }
    SECTION("Explicit arrangement")
    {
        const auto a =
            GENERATE(arrangement::EVEN_ROW, arrangement::ODD_ROW, arrangement::EVEN_COLUMN, arrangement::ODD_COLUMN);
        CAPTURE(a);
        CHECK(hexagonal_gate_layout{a}.get_arrangement() == a);
        CHECK(shifted_cartesian_gate_layout{a}.get_arrangement() == a);
    }
}

TEMPLATE_LIST_TEST_CASE_METHOD(gate_layout_fixture, "Elementary truth tables retain logical input order",
                               "[gate-layout]", gate_geometries)
{
    auto&      lyt = this->lyt;
    const auto a   = this->a;
    const auto b   = this->b;
    /** @brief Constructor of a binary gate with ordered logical inputs. */
    using binary_creator =
        layout_object_id (gate_layout<TestType>::*)(layout_object_id, layout_object_id, const layout_base::coordinate&);
    const auto [name, create, literal] = GENERATE((Catch::Generators::table<const char*, binary_creator, uint64_t>(
        {{"AND", &gate_layout<TestType>::create_and, 0x8},
         {"NAND", &gate_layout<TestType>::create_nand, 0x7},
         {"OR", &gate_layout<TestType>::create_or, 0xe},
         {"NOR", &gate_layout<TestType>::create_nor, 0x1},
         {"LT", &gate_layout<TestType>::create_lt, 0x2},
         {"GE", &gate_layout<TestType>::create_ge, 0xd},
         {"GT", &gate_layout<TestType>::create_gt, 0x4},
         {"LE", &gate_layout<TestType>::create_le, 0xb},
         {"XOR", &gate_layout<TestType>::create_xor, 0x6},
         {"XNOR", &gate_layout<TestType>::create_xnor, 0x9}})));
    DYNAMIC_SECTION(name)
    {
        CAPTURE(literal);
        const auto                 gate = (lyt.*create)(a, b, {1, 1});
        kitty::dynamic_truth_table expected{2};
        const std::array           words{literal};
        kitty::create_from_words(expected, words.cbegin(), words.cend());
        CHECK(lyt.object_function(gate) == expected);
        CHECK(lyt.source({gate, 0}) == a);
        CHECK(lyt.source({gate, 1}) == b);
        std::vector<layout_object_id> gates{};
        lyt.foreach_gate([&](const auto id) { gates.push_back(id); });
        CHECK_THAT(gates, RangeEquals(std::array{gate}));
        CHECK(lyt.num_gates() == 1);
    }
}

TEMPLATE_LIST_TEST_CASE_METHOD(gate_layout_fixture, "Gate iteration excludes identity functions and terminals",
                               "[gate-layout]", gate_geometries)
{
    auto&      lyt  = this->lyt;
    const auto wire = lyt.create_buf(this->a, {0, 2});
    const auto inv  = lyt.create_not(this->a, {1, 2});
    const auto maj  = lyt.create_maj(this->a, this->b, wire, {2, 2});
    CHECK(lyt.is_buf(wire));
    CHECK(lyt.is_inv(inv));
    CHECK(lyt.is_maj(maj));
    kitty::dynamic_truth_table identity{1};
    kitty::create_nth_var(identity, 0);
    const auto generic_wire = lyt.create_gate({this->a}, identity, {3, 2});
    CHECK(lyt.is_wire(generic_wire));
    CHECK_FALSE(lyt.is_gate(generic_wire));
    std::vector<layout_object_id> gates{};
    lyt.foreach_gate([&](const auto id) { gates.push_back(id); });
    CHECK_THAT(gates, UnorderedRangeEquals(std::array{inv, maj}));
    CHECK(lyt.num_gates() == 2);
}

TEMPLATE_TEST_CASE("Connections expose declared ports despite physical violations", "[gate-layout]", cartesian_layout,
                   shifted_cartesian_layout, hexagonal_layout)
{
    auto lyt     = blueprints::make_layout<gate_layout<TestType>>(arrangement::EVEN_ROW, {1, 1}, clocking::twoddwave());
    const auto a = lyt.create_pi("a", {-5, 0});
    const auto gate = lyt.create_buf(a, {99, 0, 3});
    CHECK(lyt.source({gate, 0}) == a);
    std::vector<typename gate_layout<TestType>::input_port> sinks{};
    lyt.foreach_sink(a, [&](const auto port) { sinks.push_back(port); });
    CHECK(sinks == std::vector{typename gate_layout<TestType>::input_port{gate, 0}});
    CHECK_THROWS_AS(lyt.connect(a, {gate, 1}), std::out_of_range);
    CHECK(lyt.source({gate, 0}) == a);
    CHECK_FALSE(lyt.find_object({0, 0}).has_value());
}

TEST_CASE("Connection visitors preserve ports while unrelated storage grows", "[gate-layout]")
{
    /** @brief Layout edited by the connection visitors. */
    gate_layout<> lyt{};
    /** @brief Source connected to the first gate input and a buffer. */
    const auto a = lyt.create_pi("a", {0, 0});
    /** @brief Source connected to the final gate input. */
    const auto b = lyt.create_pi("b", {1, 0});
    /** @brief Five-input parity function. */
    kitty::dynamic_truth_table parity{5};
    kitty::create_from_hex_string(parity, "96696996");
    /** @brief Gate with disconnected middle inputs. */
    const auto gate = lyt.create_gate({a}, parity, {2, 0});
    lyt.connect(b, {gate, 4});
    /** @brief Second sink driven by the first source. */
    const auto wire = lyt.create_buf(a, {3, 0});

    /** @brief Declared sources and logical indices recorded by the visitor. */
    std::vector<std::pair<layout_object_id, uint32_t>> sources{};
    lyt.foreach_fanin(gate,
                      [&](const auto source, const auto input)
                      {
                          sources.emplace_back(source, input);
                          /** @brief Column of the unrelated buffer pair. */
                          for (int32_t x{}; x < 256; ++x)
                          {
                              /** @brief Source of an unrelated connection created during traversal. */
                              const auto unrelated = lyt.create_buf({x, static_cast<int32_t>(input) + 1});
                              lyt.create_buf(unrelated, {x, static_cast<int32_t>(input) + 10});
                          }
                      });
    CHECK((sources == std::vector<std::pair<layout_object_id, uint32_t>>{{a, 0}, {b, 4}}));

    /** @brief Visited sink membership without an ordering requirement. */
    std::set<layout_input_port> sinks{};
    /** @brief Free row for each sink callback to create unrelated objects. */
    int32_t row{20};
    lyt.foreach_sink(a,
                     [&](const auto sink)
                     {
                         sinks.insert(sink);
                         /** @brief Column of the unrelated buffer pair. */
                         for (int32_t x{}; x < 256; ++x)
                         {
                             /** @brief Source of an unrelated connection created during traversal. */
                             const auto unrelated = lyt.create_buf({x, row});
                             lyt.create_buf(unrelated, {x, row + 1});
                         }
                         row += 2;
                     });
    CHECK((sinks == std::set<layout_input_port>{{gate, 0}, {wire, 0}}));
    CHECK(lyt.source({gate, 0}) == a);
    CHECK(lyt.source({gate, 4}) == b);
}

TEMPLATE_LIST_TEST_CASE_METHOD(gate_layout_fixture, "Object identity survives placement and stale IDs reject reuse",
                               "[gate-layout]", gate_geometries)
{
    auto&      lyt  = this->lyt;
    const auto a    = this->a;
    const auto b    = this->b;
    const auto gate = lyt.create_lt(a, b, {1, 1});
    SECTION("Placement preserves declared connectivity outside the frame")
    {
        lyt.move_object(a, {-2, 3, 7});
        CHECK(lyt.get_tile(a) == layout_base::coordinate{-2, 3, 7});
        CHECK(lyt.find_object({-2, 3, 7}) == a);
        CHECK(lyt.is_empty_tile({0, 0}));
        CHECK_FALSE(lyt.contains_coordinate(lyt.get_tile(a)));
        CHECK_FALSE(lyt.is_empty_tile({1, 1}));
        CHECK(lyt.source({gate, 0}) == a);
        CHECK(lyt.source({gate, 1}) == b);
        const auto output = lyt.create_po(gate, "f", {2, 2});
        CHECK(lyt.num_pis() == 2);
        CHECK(lyt.num_pos() == 1);
        CHECK(lyt.move_object(gate, {10, 0, 2}) == gate);
        CHECK(lyt.find_object({10, 0, 2}) == gate);
        CHECK(lyt.is_empty_tile({1, 1}));
        CHECK(lyt.source({output, 0}) == gate);
        CHECK(lyt.source({gate, 0}) == a);
        CHECK(lyt.source({gate, 1}) == b);
    }
    SECTION("Occupied placement rejects without mutation")
    {
        CHECK_THROWS_AS(lyt.move_object(gate, lyt.get_tile(b)), std::invalid_argument);
        CHECK(lyt.get_tile(gate) == layout_base::coordinate{1, 1});
        CHECK_THROWS_AS(lyt.create_pi("occupied", {1, 1}), std::invalid_argument);
        CHECK(lyt.size() == 3);
        CHECK(lyt.source({gate, 0}) == a);
        CHECK(lyt.source({gate, 1}) == b);
    }
    SECTION("Removed identities cannot address a reused slot")
    {
        lyt.move_object(a, {-2, 3, 7});
        lyt.move_object(gate, {10, 0, 2});
        lyt.remove(a);
        CHECK(lyt.is_empty_tile({-2, 3, 7}));
        CHECK_FALSE(lyt.source({gate, 0}).has_value());
        CHECK(lyt.source({gate, 1}) == b);
        const auto replacement = lyt.create_pi("replacement", {0, 0});
        CHECK(replacement != a);
        CHECK_FALSE(lyt.contains(a));
        CHECK_THROWS_AS(lyt.connect(a, {gate, 1}), std::invalid_argument);
        CHECK(lyt.source({gate, 1}) == b);
        lyt.connect(replacement, {gate, 0});
        CHECK(lyt.source({gate, 0}) == replacement);
    }
}

TEMPLATE_TEST_CASE("Physical wire counts exclude terminals through removal and reuse", "[gate-layout]",
                   cartesian_layout, shifted_cartesian_layout, hexagonal_layout)
{
    auto       lyt  = blueprints::make_layout<gate_layout<TestType>>(arrangement::EVEN_ROW, {5, 1});
    const auto pi   = lyt.create_pi("a", {0, 0});
    const auto wire = lyt.create_buf(pi, {1, 0});
    const auto po   = lyt.create_po(wire, "f", {2, 0});
    const auto inv  = lyt.create_not(pi, {3, 0});

    CHECK(lyt.is_buf(pi));
    CHECK(lyt.is_buf(po));
    CHECK(lyt.is_buf(wire));
    CHECK(lyt.is_fanout(pi));
    CHECK_FALSE(lyt.is_wire_tile({0, 0}));
    CHECK_FALSE(lyt.is_wire_tile({2, 0}));
    CHECK(lyt.num_wires() == 1);
    CHECK(lyt.num_gates() == 1);
    std::vector<typename gate_layout<TestType>::object_id> wires{};
    lyt.foreach_wire([&](const auto id) { wires.push_back(id); });
    CHECK(wires == std::vector{wire});

    lyt.remove(pi);
    lyt.remove(po);
    CHECK(lyt.num_wires() == 1);
    CHECK(lyt.num_gates() == 1);
    lyt.remove(wire);
    CHECK(lyt.num_wires() == 0);
    CHECK(lyt.num_gates() == 1);
    lyt.create_pi("replacement", {1, 0});
    CHECK(lyt.num_wires() == 0);
    CHECK(lyt.num_gates() == 1);
    lyt.remove(inv);
    CHECK(lyt.num_gates() == 0);
    CHECK_FALSE(lyt.is_empty());
}

TEMPLATE_TEST_CASE("Removing terminals updates declared order and sparse names", "[gate-layout]", cartesian_layout,
                   shifted_cartesian_layout, hexagonal_layout)
{
    auto       lyt    = blueprints::make_layout<gate_layout<TestType>>(arrangement::EVEN_ROW, {5, 5});
    const auto a      = lyt.create_pi("a", {0, 0});
    const auto b      = lyt.create_pi("b", {1, 0});
    const auto c      = lyt.create_pi("c", {2, 0});
    const auto output = lyt.create_po(c, "result", {2, 1});
    lyt.set_input_order(std::vector{c, a, b});
    lyt.remove(a);
    CHECK(lyt.num_pis() == 2);
    CHECK(lyt.pi_at(0) == c);
    CHECK(lyt.pi_at(1) == b);
    lyt.set_name(b, "");
    CHECK_FALSE(lyt.has_name(b));
    lyt.remove(output);
    CHECK(lyt.num_pos() == 0);
    CHECK(lyt.fanout_size(c) == 0);
}

TEMPLATE_TEST_CASE("Deletion disconnects self loops and duplicate destination inputs", "[gate-layout]",
                   cartesian_layout, shifted_cartesian_layout, hexagonal_layout)
{
    auto       lyt   = blueprints::make_layout<gate_layout<TestType>>(arrangement::EVEN_ROW, {4, 4});
    const auto input = lyt.create_pi("a", {0, 0});
    const auto gate  = lyt.create_and(input, input, {1, 0});
    CHECK(lyt.fanout_size(input) == 2);
    lyt.disconnect({gate, 0});
    CHECK(lyt.fanout_size(input) == 1);
    CHECK(lyt.source({gate, 1}) == input);
    lyt.connect(gate, {gate, 0});
    lyt.remove(gate);
    CHECK(lyt.fanout_size(input) == 0);
    CHECK(lyt.size() == 1);
}

TEMPLATE_LIST_TEST_CASE_METHOD(gate_layout_fixture, "Copies isolate geometry, terminals, and connectivity",
                               "[gate-layout]", gate_geometries)
{
    auto&      original = this->lyt;
    const auto a        = this->a;
    const auto b        = this->b;
    const auto gate     = original.create_and(a, b, {1, 1});
    original.assign_clock_number({1, 1}, 3);
    original.assign_synchronization_element({1, 1}, 2);
    original.obstruct_coordinate({2, 2});
    original.obstruct_connection({0, 0}, {1, 0});
    auto copy = original;
    SECTION("Placement and connectivity")
    {
        copy.move_object(gate, {2, 2});
        copy.disconnect({gate, 0});
        copy.resize({9, 9});
        CHECK(original.get_tile(gate) == layout_base::coordinate{1, 1});
        CHECK(original.source({gate, 0}) == a);
        CHECK(original.width() == 8);
        CHECK(copy.width() == 9);
    }
    SECTION("Interface order and rejected permutations")
    {
        copy.set_input_order(std::vector{b, a});
        CHECK(original.pi_at(0) == a);
        CHECK(copy.pi_at(0) == b);
        const auto [reason, invalid] =
            GENERATE_COPY((Catch::Generators::table<const char*, std::vector<layout_object_id>>(
                {{"duplicate", {a, a}}, {"missing", {a}}, {"non-terminal", {a, gate}}})));
        CAPTURE(reason);
        CHECK_THROWS_AS(copy.set_input_order(invalid), std::invalid_argument);
        CHECK(copy.pi_at(0) == b);
        CHECK(copy.pi_at(1) == a);
    }
    SECTION("Clocking and obstructions")
    {
        copy.assign_clock_number({1, 1}, 0);
        copy.assign_synchronization_element({1, 1}, 4);
        copy.clear_obstructed_coordinates();
        copy.clear_obstructed_connections();
        CHECK(original.get_clock_number({1, 1}) == 3);
        CHECK(original.get_synchronization_element({1, 1}) == 2);
        CHECK(original.is_obstructed_coordinate({2, 2}));
        CHECK(original.is_obstructed_connection({0, 0}, {1, 0}));
    }
}

TEMPLATE_TEST_CASE("Moved layouts leave reusable empty sources", "[gate-layout]", cartesian_layout,
                   shifted_cartesian_layout, hexagonal_layout)
{
    auto       source  = blueprints::make_layout<gate_layout<TestType>>(arrangement::EVEN_ROW, {4, 4});
    const auto removed = source.create_pi("removed", {0, 0});
    source.create_pi("kept", {1, 0});
    source.remove(removed);
    auto destination = std::move(source);
    // The layout contract permits moved-from reuse.
    // NOLINTNEXTLINE(*-use-after-move,*.Move,*-invalid-access-moved)
    REQUIRE(source.is_empty());
    const auto reused = source.create_pi("reused", {0, 0});
    CHECK(source.contains(reused));
    CHECK(destination.num_pis() == 1);
    source = std::move(destination);
    // The layout contract permits moved-from reuse.
    // NOLINTNEXTLINE(*-use-after-move,*.Move,*-invalid-access-moved)
    REQUIRE(destination.is_empty());
    CHECK(destination.create_pi("new", {2, 0}).generation != 0);
    CHECK(source.get_input_name(0) == "kept");
}

TEST_CASE("Clock zone assignment", "[gate-layout]")
{
    cartesian_gate_layout layout{cartesian_gate_layout::extent{2, 2, 1}, clocking::twoddwave()};

    SECTION("2DDWave Clocking")
    {
        CHECK(layout.is_clocking_scheme(clocking::TWODDWAVE_NAME));
        CHECK_FALSE(layout.is_clocking_scheme(clocking::RES_NAME));
        CHECK(layout.is_regularly_clocked());
        CHECK(layout.num_clocks() == 4);

        CHECK(layout.get_clock_number({0, 0}) == 0);
        CHECK(layout.get_clock_number({1, 0}) == 1);
        CHECK(layout.get_clock_number({0, 1}) == 1);
        CHECK(layout.get_clock_number({1, 1}) == 2);

        CHECK(layout.is_incoming_clocked({1, 0}, {0, 0}));
        CHECK(layout.is_incoming_clocked({0, 1}, {0, 0}));
        CHECK(layout.is_incoming_clocked({1, 1}, {0, 1}));
        CHECK(layout.is_incoming_clocked({1, 1}, {1, 0}));
        CHECK_FALSE(layout.is_incoming_clocked({1, 1}, {0, 0}));
        CHECK_FALSE(layout.is_incoming_clocked({1, 1}, {1, 1}));

        CHECK(layout.is_outgoing_clocked({0, 0}, {1, 0}));
        CHECK(layout.is_outgoing_clocked({0, 0}, {0, 1}));
        CHECK(layout.is_outgoing_clocked({0, 1}, {1, 1}));
        CHECK(layout.is_outgoing_clocked({1, 0}, {1, 1}));
        CHECK_FALSE(layout.is_outgoing_clocked({0, 0}, {1, 1}));
        CHECK_FALSE(layout.is_outgoing_clocked({1, 1}, {1, 1}));

        layout.assign_clock_number({1, 0}, 2);
        layout.assign_clock_number({0, 1}, 2);
        layout.assign_clock_number({1, 1}, 3);

        CHECK_FALSE(layout.is_regularly_clocked());

        CHECK(layout.get_clock_number({0, 0}) == 0);
        CHECK(layout.get_clock_number({1, 0}) == 2);
        CHECK(layout.get_clock_number({0, 1}) == 2);
        CHECK(layout.get_clock_number({1, 1}) == 3);

        // a clock zone spans every layer of its tile
        CHECK(layout.get_clock_number({1, 1, 1}) == 3);
        layout.assign_clock_number({0, 1, 1}, 3);
        CHECK(layout.get_clock_number({0, 1, 0}) == 3);
        layout.assign_clock_number({0, 1}, 2);

        CHECK(layout.is_incoming_clocked({1, 1}, {1, 0}));
        CHECK(layout.is_incoming_clocked({1, 1}, {0, 1}));
        CHECK_FALSE(layout.is_incoming_clocked({1, 0}, {0, 0}));
        CHECK_FALSE(layout.is_incoming_clocked({1, 1}, {0, 0}));

        CHECK(layout.is_outgoing_clocked({1, 0}, {1, 1}));
        CHECK(layout.is_outgoing_clocked({0, 1}, {1, 1}));
        CHECK_FALSE(layout.is_outgoing_clocked({0, 0}, {1, 0}));
        CHECK_FALSE(layout.is_outgoing_clocked({0, 0}, {0, 1}));
    }

    SECTION("Replace with USE")
    {
        layout.replace_clocking_scheme(clocking::use());

        CHECK_FALSE(layout.is_clocking_scheme(clocking::TWODDWAVE_NAME));
        CHECK(layout.is_clocking_scheme(clocking::USE_NAME));
        CHECK(layout.is_regularly_clocked());

        CHECK(layout.get_clock_number({0, 0}) == 0);
        CHECK(layout.get_clock_number({1, 0}) == 1);
        CHECK(layout.get_clock_number({0, 1}) == 3);
        CHECK(layout.get_clock_number({1, 1}) == 2);

        CHECK(layout.is_incoming_clocked({0, 1}, {1, 1}));
        CHECK(layout.is_incoming_clocked({1, 1}, {1, 0}));
        CHECK(layout.is_incoming_clocked({1, 0}, {0, 0}));
        CHECK_FALSE(layout.is_incoming_clocked({1, 1}, {0, 0}));
        CHECK_FALSE(layout.is_incoming_clocked({1, 1}, {1, 1}));

        CHECK(layout.is_outgoing_clocked({0, 0}, {1, 0}));
        CHECK(layout.is_outgoing_clocked({0, 1}, {0, 0}));
        CHECK(layout.is_outgoing_clocked({1, 0}, {1, 1}));
        CHECK_FALSE(layout.is_outgoing_clocked({0, 0}, {0, 1}));
        CHECK_FALSE(layout.is_outgoing_clocked({0, 0}, {1, 1}));
        CHECK_FALSE(layout.is_outgoing_clocked({1, 1}, {1, 1}));

        layout.assign_clock_number({1, 0}, 2);
        layout.assign_clock_number({0, 1}, 2);
        layout.assign_clock_number({1, 1}, 3);

        CHECK_FALSE(layout.is_regularly_clocked());

        CHECK(layout.get_clock_number({0, 0}) == 0);
        CHECK(layout.get_clock_number({1, 0}) == 2);
        CHECK(layout.get_clock_number({0, 1}) == 2);
        CHECK(layout.get_clock_number({1, 1}) == 3);
    }
}

TEMPLATE_TEST_CASE("Clones isolate clock overrides, schemes, and geometry", "[gate-layout]", cartesian_layout,
                   shifted_cartesian_layout, hexagonal_layout)
{
    auto original =
        blueprints::make_layout<gate_layout<TestType>>(arrangement::EVEN_ROW, {6, 6, 1}, clocking::twoddwave());
    original.assign_clock_number({0, 0}, 3);
    original.assign_synchronization_element({0, 0}, 1);
    original.assign_synchronization_element({1, 0}, 2);
    original.create_pi("a", {2, 2});

    auto copy = original.clone();

    CHECK(copy.get_clock_number({0, 0}) == 3);
    original.assign_clock_number({0, 0}, 1);
    CHECK(copy.get_clock_number({0, 0}) == 3);
    copy.assign_clock_number({0, 0}, 2);
    CHECK(original.get_clock_number({0, 0}) == 1);

    copy.assign_synchronization_element({0, 0}, 2);
    copy.assign_synchronization_element({1, 0}, 3);
    CHECK(original.get_synchronization_element({0, 0}) == 1);
    CHECK(original.get_synchronization_element({1, 0}) == 2);
    CHECK(copy.get_synchronization_element({0, 0}) == 2);
    CHECK(copy.get_synchronization_element({1, 0}) == 3);
    copy.resize({11, 11, 2});
    copy.replace_clocking_scheme(clocking::use());

    CHECK(original.width() == 6);
    CHECK(original.height() == 6);
    CHECK(original.layers() == 1);
    CHECK(original.is_clocking_scheme(clocking::TWODDWAVE_NAME));

    CHECK(copy.width() == 11);
    CHECK(copy.height() == 11);
    CHECK(copy.layers() == 2);
    CHECK(copy.is_clocking_scheme(clocking::USE_NAME));
}

TEST_CASE("Borrowed clocking schemes observe overrides and isolate clones", "[gate-layout]")
{
    cartesian_gate_layout original{{6, 6, 1}, clocking::twoddwave()};
    const auto&           scheme = original.get_clocking_scheme();
    STATIC_REQUIRE(
        std::is_same_v<decltype(original.get_clocking_scheme()), const cartesian_gate_layout::clocking_scheme_t&>);
    STATIC_REQUIRE(noexcept(original.get_clocking_scheme()));

    auto copy = original.clone();
    original.assign_clock_number({0, 0}, 3);
    CHECK(scheme(0, 0) == 3);
    CHECK(copy.get_clocking_scheme()(0, 0) == 0);
    copy.assign_clock_number({0, 0}, 2);
    CHECK(scheme(0, 0) == 3);
    CHECK(copy.get_clocking_scheme()(0, 0) == 2);
}

TEST_CASE("Iteration over clocking zones", "[gate-layout]")
{
    const cartesian_gate_layout layout{{3, 3, 1}, clocking::twoddwave()};
    const auto                  incoming = GENERATE(true, false);
    CAPTURE(incoming);
    const auto boundary = incoming ? layout_base::coordinate{0, 0} : layout_base::coordinate{2, 2};
    CHECK((incoming ? layout.incoming_clocked_zones(boundary) : layout.outgoing_clocked_zones(boundary)).empty());
    const auto expected = incoming ? std::vector<layout_base::coordinate>{{1, 0}, {0, 1}} :
                                     std::vector<layout_base::coordinate>{{1, 2}, {2, 1}};
    CHECK_THAT((incoming ? layout.incoming_clocked_zones({1, 1}) : layout.outgoing_clocked_zones({1, 1})),
               UnorderedRangeEquals(expected));
    std::vector<layout_base::coordinate> visited{};
    const auto                           collect = [&](const auto& zone) { visited.push_back(zone); };
    if (incoming)
    {
        layout.foreach_incoming_clocked_zone({1, 1}, collect);
    }
    else
    {
        layout.foreach_outgoing_clocked_zone({1, 1}, collect);
    }
    CHECK_THAT(visited, UnorderedRangeEquals(expected));
}

TEST_CASE("Clocked neighbor degrees", "[gate-layout]")
{
    const auto                  use_clocking = GENERATE(false, true);
    const cartesian_gate_layout layout{{3, 3, 1}, use_clocking ? clocking::use() : clocking::twoddwave()};
    const auto [zone, in_wave, out_wave, in_use, out_use, degree] =
        GENERATE((Catch::Generators::table<layout_base::coordinate, unsigned, unsigned, unsigned, unsigned, unsigned>(
            {{{0, 0}, 0, 2, 1, 1, 2},
             {{1, 0}, 1, 2, 1, 2, 3},
             {{2, 0}, 1, 1, 2, 0, 2},
             {{1, 1}, 2, 2, 2, 2, 4},
             {{0, 2}, 1, 1, 0, 2, 2},
             {{1, 2}, 2, 1, 2, 1, 3},
             {{2, 2}, 2, 0, 1, 1, 2}})));
    CAPTURE(use_clocking, zone);
    CHECK(layout.in_degree(zone) == (use_clocking ? in_use : in_wave));
    CHECK(layout.out_degree(zone) == (use_clocking ? out_use : out_wave));
    CHECK(layout.degree(zone) == degree);
}

TEST_CASE("Synchronization element layout properties", "[gate-layout]")
{
    cartesian_gate_layout layout{{3, 3, 1}, clocking::twoddwave()};
    CHECK(layout.num_se() == 0);
    layout.assign_synchronization_element({0, 0}, 0);
    CHECK(layout.num_se() == 0);
    layout.assign_synchronization_element({0, 1}, 1);
    CHECK(layout.num_se() == 1);
    layout.assign_synchronization_element({1, 0}, 1);
    CHECK(layout.num_se() == 2);
    layout.assign_synchronization_element({1, 2}, 2);
    CHECK(layout.num_se() == 3);
    layout.assign_synchronization_element({2, 1}, 2);
    CHECK(layout.num_se() == 4);

    const auto [zone, delay, degree] =
        GENERATE((Catch::Generators::table<layout_base::coordinate, unsigned, unsigned>({{{0, 0}, 0, 2},
                                                                                         {{1, 0}, 1, 3},
                                                                                         {{2, 0}, 0, 2},
                                                                                         {{0, 1}, 1, 3},
                                                                                         {{1, 1}, 0, 4},
                                                                                         {{2, 1}, 2, 3},
                                                                                         {{0, 2}, 0, 2},
                                                                                         {{1, 2}, 2, 3},
                                                                                         {{2, 2}, 0, 2}})));
    CAPTURE(zone, delay);
    CHECK(layout.is_synchronization_element(zone) == (delay != 0));
    CHECK(layout.get_synchronization_element(zone) == delay);
    CHECK(layout.in_degree(zone) == degree);
    CHECK(layout.out_degree(zone) == degree);
    CHECK(layout.degree(zone) == degree);
}

TEST_CASE("Shifted clocking with synchronization elements", "[gate-layout]")
{
    cartesian_gate_layout layout{cartesian_gate_layout::extent{3, 3, 1}, clocking::twoddwave()};

    layout.assign_synchronization_element({1, 1}, 1);

    CHECK(layout.is_clocking_scheme(clocking::TWODDWAVE_NAME));
    CHECK(layout.is_regularly_clocked());
    CHECK(layout.num_clocks() == 4);

    CHECK(layout.get_clock_number({0, 0}) == 0);
    CHECK(layout.get_clock_number({1, 0}) == 1);
    CHECK(layout.get_clock_number({0, 1}) == 1);
    CHECK(layout.get_clock_number({1, 1}) == 2);
    CHECK(layout.get_clock_number({2, 1}) == 3);
    CHECK(layout.get_clock_number({1, 2}) == 3);
    CHECK(layout.get_clock_number({2, 2}) == 0);

    CHECK(layout.is_incoming_clocked({1, 0}, {0, 0}));
    CHECK(layout.is_incoming_clocked({0, 1}, {0, 0}));
    CHECK(layout.is_incoming_clocked({1, 1}, {0, 1}));
    CHECK(layout.is_incoming_clocked({1, 1}, {1, 0}));
    CHECK(layout.is_incoming_clocked({1, 1}, {2, 1}));
    CHECK(layout.is_incoming_clocked({1, 1}, {1, 2}));

    CHECK(layout.is_outgoing_clocked({0, 0}, {1, 0}));
    CHECK(layout.is_outgoing_clocked({0, 0}, {0, 1}));
    CHECK(layout.is_outgoing_clocked({0, 1}, {1, 1}));
    CHECK(layout.is_outgoing_clocked({1, 0}, {1, 1}));
    CHECK(layout.is_outgoing_clocked({2, 1}, {1, 1}));
    CHECK(layout.is_outgoing_clocked({1, 2}, {1, 1}));
}

TEST_CASE("Iteration over synchronization elements", "[gate-layout]")
{
    cartesian_gate_layout layout{cartesian_gate_layout::extent{3, 3, 1}, clocking::twoddwave()};

    layout.assign_synchronization_element({0, 1}, 1);
    layout.assign_synchronization_element({1, 0}, 1);
    layout.assign_synchronization_element({1, 2}, 1);
    layout.assign_synchronization_element({2, 1}, 1);

    CHECK(layout.incoming_clocked_zones({0, 0}).size() == 2);
    CHECK(layout.outgoing_clocked_zones({2, 2}).size() == 2);

    const std::array expected{layout_base::coordinate{1, 0}, layout_base::coordinate{0, 1},
                              layout_base::coordinate{1, 2}, layout_base::coordinate{2, 1}};
    CHECK_THAT(layout.incoming_clocked_zones({1, 1}), UnorderedRangeEquals(expected));
    CHECK_THAT(layout.outgoing_clocked_zones({1, 1}), UnorderedRangeEquals(expected));
}

TEST_CASE("Sparse clock metadata enumerates assigned zones independently of frame area", "[gate-layout]")
{
    gate_layout<cartesian_layout> lyt{{1'000'000, 1'000'000}};
    lyt.assign_clock_number({999'999, 999'999}, 2);
    lyt.assign_synchronization_element({-1, 4}, 7);
    uint32_t clocks{};
    lyt.get_clocking_scheme().foreach_override(
        [&](const auto x, const auto y, const auto number)
        {
            CHECK(x == 999'999);
            CHECK(y == 999'999);
            CHECK(number == 2);
            ++clocks;
        });
    uint32_t delays{};
    lyt.foreach_synchronization_element(
        [&](const auto& zone, const auto delay)
        {
            CHECK(zone == layout_base::coordinate{-1, 4});
            CHECK(delay == 7);
            ++delays;
        });
    CHECK(clocks == 1);
    CHECK(delays == 1);
}

TEMPLATE_TEST_CASE("Crossings count routing wires rather than identity terminals", "[gate-layout]", cartesian_layout,
                   shifted_cartesian_layout, hexagonal_layout)
{
    auto lyt = blueprints::make_layout<gate_layout<TestType>>(arrangement::EVEN_ROW, {4, 2, 2});
    for (int64_t x{}; x < 4; ++x)
    {
        lyt.create_buf({x, 0, 0});
    }
    const auto pi = lyt.create_pi("a", {0, 0, 1});
    const auto po = lyt.create_po("f", {1, 0, 1});
    lyt.create_buf({2, 0, 1});
    kitty::dynamic_truth_table identity{1};
    kitty::create_from_hex_string(identity, "2");
    const auto gate = lyt.create_gate({}, identity, {3, 0, 1});
    CHECK_FALSE(lyt.is_wire(pi));
    CHECK_FALSE(lyt.is_wire(po));
    CHECK(lyt.is_wire(gate));
    CHECK(lyt.num_wires() == 6);
    CHECK(lyt.num_crossings() == 2);
}

TEMPLATE_TEST_CASE("Crossing counts follow edits on both layers and value copies", "[gate-layout]", cartesian_layout,
                   shifted_cartesian_layout, hexagonal_layout)
{
    auto       lyt   = blueprints::make_layout<gate_layout<TestType>>(arrangement::EVEN_ROW, {4, 2, 2});
    const auto upper = lyt.create_buf({0, 0, 1});
    CHECK(lyt.num_crossings() == 0);
    const auto lower = lyt.create_pi("a", {0, 0, 0});
    CHECK(lyt.num_crossings() == 1);
    const auto third = lyt.create_buf({0, 0, 2});
    CHECK(lyt.num_crossings() == 1);
    lyt.move_object(third, {1, 0, 0});
    CHECK(lyt.num_crossings() == 1);
    lyt.move_object(upper, {1, 0, 1});
    CHECK(lyt.num_crossings() == 1);
    lyt.move_object(third, {1, 0, 2});
    CHECK(lyt.num_crossings() == 0);
    lyt.move_object(third, {1, 0, 0});
    CHECK(lyt.num_crossings() == 1);
    CHECK_THROWS_AS(lyt.move_object(lower, {1, 0, 0}), std::invalid_argument);
    CHECK(lyt.num_crossings() == 1);
    auto copy = lyt;
    lyt.remove(upper);
    CHECK(lyt.num_crossings() == 0);
    CHECK(copy.num_crossings() == 1);
    auto moved = std::move(copy);
    CHECK(moved.num_crossings() == 1);
    moved.remove(third);
    CHECK(moved.num_crossings() == 0);
    lyt = moved;
    CHECK(lyt.num_crossings() == 0);
}

TEMPLATE_TEST_CASE("Five-input objects keep ordered ports", "[gate-layout]", cartesian_layout, shifted_cartesian_layout,
                   hexagonal_layout)
{
    auto lyt = blueprints::make_layout<gate_layout<TestType>>(arrangement::EVEN_ROW, {6, 6});
    std::vector<typename gate_layout<TestType>::object_id> pis{};
    pis.reserve(5);
    for (uint32_t i = 0; i < 5; ++i)
    {
        pis.push_back(lyt.create_pi("pi" + std::to_string(i), {static_cast<int64_t>(i), 0}));
    }
    kitty::dynamic_truth_table parity{5};
    kitty::create_from_hex_string(parity, "96696996");
    const auto gate = lyt.create_gate(pis, parity, {2, 2});
    CHECK(lyt.input_count(gate) == 5);
    CHECK(lyt.fanin_size(gate) == 5);
    for (uint32_t i = 0; i < 5; ++i)
    {
        CHECK(lyt.source({gate, i}) == pis[i]);
    }
    lyt.disconnect({gate, 3});
    CHECK(lyt.fanin_size(gate) == 4);
    CHECK_FALSE(lyt.source({gate, 3}).has_value());
    CHECK(lyt.source({gate, 4}) == pis[4]);
    CHECK_THROWS_AS(lyt.source({gate, 5}), std::out_of_range);
    /** @brief Sources and their declared indices across a disconnected input port. */
    std::vector<layout_object_id> fanins{};
    std::vector<uint32_t>         input_indices{};
    lyt.foreach_fanin(gate,
                      [&](const auto source, const auto index)
                      {
                          fanins.push_back(source);
                          input_indices.push_back(index);
                      });
    CHECK_THAT(fanins, RangeEquals(std::array{pis[0], pis[1], pis[2], pis[4]}));
    CHECK_THAT(input_indices, RangeEquals(std::array{0u, 1u, 2u, 4u}));

    lyt.connect(pis[0], {gate, 3});
    CHECK(lyt.fanout_size(pis[0]) == 2);
    /** @brief Destination input ports driven by the first primary input. */
    std::vector<typename gate_layout<TestType>::input_port> sinks{};
    lyt.foreach_sink(pis[0], [&](const auto port) { sinks.push_back(port); });
    CHECK_THAT(sinks, UnorderedRangeEquals(std::array{layout_input_port{gate, 0}, layout_input_port{gate, 3}}));
    /** @brief Destination objects visited once per connected input port. */
    std::vector<typename gate_layout<TestType>::object_id> destinations{};
    lyt.foreach_fanout(pis[0], [&](const auto destination) { destinations.push_back(destination); });
    CHECK((destinations == std::vector{gate, gate}));
    lyt.move_object(pis[0], {2, 1});
    CHECK((lyt.template outgoing_data_flow<false>({2, 1}) ==
           std::vector{layout_base::coordinate{2, 2}, layout_base::coordinate{2, 2}}));
    lyt.assign_clock_number({2, 1}, 0);
    lyt.assign_clock_number({2, 2}, 1);
    CHECK((lyt.template outgoing_data_flow<true>({2, 1}) ==
           std::vector{layout_base::coordinate{2, 2}, layout_base::coordinate{2, 2}}));
    lyt.assign_clock_number({2, 2}, 0);
    CHECK(lyt.template outgoing_data_flow<true>({2, 1}).empty());
}

TEMPLATE_LIST_TEST_CASE_METHOD(gate_layout_fixture,
                               "Large-arity objects preserve disconnected ports through copying, moving, and removal",
                               "[gate-layout]", gate_geometries)
{
    auto&                      original = this->lyt;
    const auto                 a        = this->a;
    const auto                 b        = this->b;
    kitty::dynamic_truth_table parity{5};
    kitty::create_from_hex_string(parity, "96696996");
    const auto gate = original.create_gate({a}, parity, {2, 2});
    original.connect(b, {gate, 4});
    auto copy = original;
    copy.disconnect({gate, 0});
    CHECK(original.source({gate, 0}) == a);
    CHECK_FALSE(copy.source({gate, 0}).has_value());
    auto moved = std::move(copy);
    CHECK(moved.input_count(gate) == 5);
    CHECK_FALSE(moved.source({gate, 3}).has_value());
    CHECK(moved.source({gate, 4}) == b);
    moved.remove(gate);
    CHECK(moved.fanout_size(a) == 0);
    CHECK(moved.fanout_size(b) == 0);
    const auto replacement = moved.create_gate({b}, parity, {2, 2});
    CHECK_FALSE(moved.contains(gate));
    CHECK(moved.source({replacement, 0}) == b);
    CHECK_FALSE(moved.source({replacement, 4}).has_value());
    moved.remove(replacement);
    const auto wire = moved.create_buf(a, {2, 2});
    CHECK(moved.input_count(wire) == 1);
    CHECK(moved.source({wire, 0}) == a);
}

TEMPLATE_TEST_CASE("Object visitors accept move-only lvalues and temporaries", "[gate-layout]", cartesian_layout,
                   shifted_cartesian_layout, hexagonal_layout)
{
    /** @brief Layout with enough live objects to test early termination. */
    auto lyt = blueprints::make_layout<gate_layout<TestType>>(arrangement::EVEN_ROW, {3, 1, 1});
    lyt.create_pi("a", {0, 0});
    lyt.create_pi("b", {1, 0});
    lyt.create_pi("c", {2, 0});

    /** @brief Counts visits and stops after two objects. */
    struct move_only_visitor
    {
        /** @brief Owns the call count. */
        std::unique_ptr<uint32_t> calls;
        /** @brief Reports calls externally. */
        uint32_t& observed;
        /**
         * @brief Visits as an lvalue and stops after two objects.
         * @return Whether another object should be visited.
         */
        bool operator()(layout_object_id) &
        {
            observed = ++*calls;
            return observed < 2;
        }
    };

    /** @brief Visits reported by either callback value category. */
    uint32_t observed{};  // NOLINT(misc-const-correctness): The visitor writes through its reference member.
    SECTION("Lvalue")
    {
        move_only_visitor visitor{.calls = std::make_unique<uint32_t>(0), .observed = observed};
        lyt.foreach_object(visitor);
        CHECK(observed == 2);
    }
    SECTION("Temporary")
    {
        lyt.foreach_object(move_only_visitor{.calls = std::make_unique<uint32_t>(0), .observed = observed});
        CHECK(observed == 2);
    }
}

TEST_CASE("Clocked neighbor visitors accept move-only lvalues and temporaries", "[gate-layout]")
{
    /** @brief Layout with two incoming and two outgoing neighbors of the center. */
    const cartesian_gate_layout lyt{{3, 3, 1}, clocking::twoddwave()};
    /** @brief Collects neighbors through an lvalue-qualified move-only callback. */
    struct visitor
    {
        /** @brief Owns the callback state. */
        std::unique_ptr<uint32_t> calls;
        /** @brief Reports visited neighbors. */
        std::vector<cartesian_gate_layout::coordinate>& observed;
        /** @brief Records a clocked neighbor. @param coordinate Visited neighbor. */
        void operator()(const cartesian_gate_layout::coordinate& coordinate) &
        {
            ++*calls;
            observed.push_back(coordinate);
        }
    };
    /** @brief Incoming neighbors visited through both callback value categories. */
    std::vector<cartesian_gate_layout::coordinate> incoming{};
    /** @brief Move-only lvalue callback. */
    visitor incoming_visitor{.calls = std::make_unique<uint32_t>(0), .observed = incoming};
    lyt.foreach_incoming_clocked_zone({1, 1}, incoming_visitor);
    CHECK_THAT(incoming, UnorderedRangeEquals(std::vector<layout_base::coordinate>{{0, 1}, {1, 0}}));
    incoming.clear();
    lyt.foreach_incoming_clocked_zone({1, 1}, visitor{.calls = std::make_unique<uint32_t>(0), .observed = incoming});
    CHECK_THAT(incoming, UnorderedRangeEquals(std::vector<layout_base::coordinate>{{0, 1}, {1, 0}}));
    /** @brief Outgoing neighbors visited through both callback value categories. */
    std::vector<cartesian_gate_layout::coordinate> outgoing{};
    /** @brief Move-only lvalue callback. */
    visitor outgoing_visitor{.calls = std::make_unique<uint32_t>(0), .observed = outgoing};
    lyt.foreach_outgoing_clocked_zone({1, 1}, outgoing_visitor);
    CHECK_THAT(outgoing, UnorderedRangeEquals(std::vector<layout_base::coordinate>{{1, 2}, {2, 1}}));
    outgoing.clear();
    lyt.foreach_outgoing_clocked_zone({1, 1}, visitor{.calls = std::make_unique<uint32_t>(0), .observed = outgoing});
    CHECK_THAT(outgoing, UnorderedRangeEquals(std::vector<layout_base::coordinate>{{1, 2}, {2, 1}}));
}

TEMPLATE_TEST_CASE("Object iteration preserves sparse membership through reuse and ownership changes", "[gate-layout]",
                   cartesian_layout, shifted_cartesian_layout, hexagonal_layout)
{
    auto                          lyt = blueprints::make_layout<gate_layout<TestType>>(arrangement::EVEN_ROW, {});
    std::vector<layout_object_id> objects{};
    std::set<layout_object_id>    expected{};
    for (int32_t x{}; x < 130; ++x)
    {
        objects.push_back(lyt.create_buf({x, 0}));
    }
    for (uint32_t index{}; index < objects.size(); ++index)
    {
        if (index == 0 || index == 63 || index == 64 || index == 127 || index == 129)
        {
            expected.insert(objects[index]);
        }
        else
        {
            lyt.remove(objects[index]);
        }
    }
    const auto replacement = lyt.create_buf({1, 0});
    expected.insert(replacement);
    /** @brief Checks live membership and contiguous visitor indices without assuming traversal order. */
    const auto check_membership = [&](const auto& layout)
    {
        std::vector<layout_object_id> visited{};
        uint32_t                      count{};
        layout.foreach_object(
            [&](const auto id, const auto index)
            {
                CHECK(index == count++);
                visited.push_back(id);
            });
        std::ranges::sort(visited);
        CHECK_THAT(visited, RangeEquals(expected));
        CHECK(count == layout.size());
        count = 0;
        layout.foreach_object([&](const auto) { return ++count < 2; });
        CHECK(count == 2);
    };
    check_membership(lyt);
    auto copied = lyt;
    check_membership(copied);
    auto moved = std::move(copied);
    check_membership(moved);
    copied = lyt;
    check_membership(copied);
    copied = std::move(moved);
    check_membership(copied);
    lyt.remove(replacement);
    CHECK(copied.contains(replacement));
}

TEST_CASE("Name presence queries do not allocate", "[gate-layout]")
{
    require_allocation_failure_support();
    /** Layout with long names that cannot use short-string storage. */
    gate_layout<cartesian_layout> layout{{2, 1}};
    /** Named input. */
    const auto input = layout.create_pi(std::string(128, 'a'), {0, 0});
    layout.create_po(input, std::string(128, 'b'), {1, 0});
    /** Results collected before Catch2 performs its own allocations. */
    bool object_named{}, input_named{}, output_named{};
    allocation_budget = 0;
    try
    {
        object_named = layout.has_name(input);
        input_named  = layout.has_input_name(0);
        output_named = layout.has_output_name(0);
    }
    catch (...)
    {
        allocation_budget.reset();
        throw;
    }
    allocation_budget.reset();
    CHECK(object_named);
    CHECK(input_named);
    CHECK(output_named);
    CHECK_THROWS_AS(layout.has_name(gate_layout<cartesian_layout>::object_id{999}), std::invalid_argument);
    CHECK_THROWS_AS(layout.has_input_name(1), std::out_of_range);
    CHECK_THROWS_AS(layout.has_output_name(1), std::out_of_range);
    layout.set_name(input, "");
    CHECK_FALSE(layout.has_name(input));
    CHECK_FALSE(layout.has_input_name(0));
}

TEMPLATE_TEST_CASE("Layout moves preserve objects and clocking without allocating", "[gate-layout]", cartesian_layout,
                   shifted_cartesian_layout, hexagonal_layout)
{
    STATIC_REQUIRE(std::is_nothrow_move_constructible_v<gate_layout<TestType>>);
    STATIC_REQUIRE(std::is_nothrow_move_assignable_v<gate_layout<TestType>>);

    require_allocation_failure_support();
    auto source = blueprints::make_layout<gate_layout<TestType>>(arrangement::EVEN_ROW, {3, 2}, clocking::twoddwave());
    const auto input = source.create_pi("input", {0, 0});
    source.assign_synchronization_element({0, 0}, 2);
    auto destination  = blueprints::make_layout<gate_layout<TestType>>(arrangement::EVEN_ROW, {});
    allocation_budget = 0;
    auto moved        = std::move(source);
    destination       = std::move(moved);
    allocation_budget.reset();
    CHECK(destination.contains(input));
    CHECK(destination.is_clocking_scheme(clocking::TWODDWAVE_NAME));
    CHECK(destination.get_synchronization_element({0, 0}) == 2);
    // The layout contract permits inspecting and reusing moved-from layouts.
    // NOLINTNEXTLINE(*-use-after-move,*.Move,*-invalid-access-moved)
    CHECK(source.get_clocking_scheme() == clocking::open());
}

/** @brief Cartesian geometry whose independent clone reports a failure. */
struct throwing_clone_geometry : cartesian_layout
{
    /** @brief Constructs ordinary Cartesian geometry. */
    using cartesian_layout::cartesian_layout;
    /** @brief Reports a failed geometry clone. @throws std::runtime_error Always. */
    [[nodiscard]] static throwing_clone_geometry clone()
    {
        throw std::runtime_error("Geometry clone failed");
    }
};

TEST_CASE("Geometry clone failures preserve layout move operands", "[gate-layout]")
{
    STATIC_REQUIRE_FALSE(std::is_nothrow_move_constructible_v<gate_layout<throwing_clone_geometry>>);
    STATIC_REQUIRE_FALSE(std::is_nothrow_move_assignable_v<gate_layout<throwing_clone_geometry>>);
    gate_layout<throwing_clone_geometry> source{{3, 2}};
    const auto                           input = source.create_pi("source", {0, 0});
    gate_layout<throwing_clone_geometry> destination{{2, 1}};
    const auto                           retained = destination.create_pi("destination", {1, 0});

    SECTION("Move construction")
    {
        REQUIRE_THROWS_AS(gate_layout<throwing_clone_geometry>{std::move(source)}, std::runtime_error);
    }
    SECTION("Move assignment")
    {
        // Catch2 runs these sections independently; the clone failure preserves the source.
        REQUIRE_THROWS_AS(destination = std::move(source),  // NOLINT(*-use-after-move,*-invalid-access-moved)
                          std::runtime_error);
    }
    // A failed geometry clone preserves the source before ownership transfer.
    REQUIRE(source.contains(input));  // NOLINT(*-use-after-move,*-invalid-access-moved)
    CHECK(source.get_tile(input) == layout_base::coordinate{0, 0});
    CHECK(source.get_name(input) == "source");
    CHECK(source.num_pis() == 1);
    REQUIRE(destination.contains(retained));
    CHECK(destination.get_tile(retained) == layout_base::coordinate{1, 0});
    CHECK(destination.get_name(retained) == "destination");
    CHECK(destination.num_pis() == 1);
}

/** @brief Cartesian geometry whose move assignment changes geometry and reports a failure. */
struct throwing_assignment_geometry : cartesian_layout
{
    /** @brief Constructs ordinary Cartesian geometry. */
    using cartesian_layout::cartesian_layout;
    /** @brief Copies the geometry. */
    throwing_assignment_geometry(const throwing_assignment_geometry&) = default;
    /** @brief Moves the geometry. */
    throwing_assignment_geometry(throwing_assignment_geometry&&) noexcept = default;
    /** @brief Copies the geometry. */
    throwing_assignment_geometry& operator=(const throwing_assignment_geometry&) = default;
    /** @brief Changes the extent and reports assignment failure. @throws std::runtime_error Always. */
    // NOLINTNEXTLINE(bugprone-exception-escape): Intentionally fails to exercise layout-state rollback.
    throwing_assignment_geometry& operator=(throwing_assignment_geometry&& other) noexcept(false)
    {
        cartesian_layout::operator=(std::move(other));
        throw std::runtime_error("Geometry assignment failed");
    }
    /** @brief Releases the geometry. */
    ~throwing_assignment_geometry() = default;
    /** @brief Returns an independent geometry copy. */
    [[nodiscard]] throwing_assignment_geometry clone() const noexcept
    {
        return *this;
    }
};

TEST_CASE("Geometry assignment failures preserve owned layout state", "[gate-layout]")
{
    STATIC_REQUIRE(std::is_nothrow_move_constructible_v<gate_layout<throwing_assignment_geometry>>);
    STATIC_REQUIRE_FALSE(std::is_nothrow_move_assignable_v<gate_layout<throwing_assignment_geometry>>);
    gate_layout<throwing_assignment_geometry> source{{3, 2}};
    const auto                                input = source.create_pi("source", {0, 0});
    source.assign_synchronization_element({0, 0}, 2);
    gate_layout<throwing_assignment_geometry> destination{{2, 1}};
    const auto                                retained = destination.create_pi("destination", {1, 0});
    destination.assign_synchronization_element({1, 0}, 1);

    REQUIRE_THROWS_AS(destination = std::move(source), std::runtime_error);
    // A failed geometry assignment restores the source's owned state.
    REQUIRE(source.contains(input));  // NOLINT(*-use-after-move,*-invalid-access-moved)
    CHECK(source.get_name(input) == "source");
    CHECK(source.get_tile(input) == layout_base::coordinate{0, 0});
    CHECK(source.get_synchronization_element({0, 0}) == 2);
    CHECK(source.num_pis() == 1);
    REQUIRE(destination.contains(retained));
    CHECK(destination.get_name(retained) == "destination");
    CHECK(destination.get_tile(retained) == layout_base::coordinate{1, 0});
    CHECK(destination.get_synchronization_element({1, 0}) == 1);
    CHECK(destination.num_pis() == 1);
    // The geometry's assignment contract controls changes made before it throws.
}

TEST_CASE("Crossing counts survive failed connection allocation", "[gate-layout]")
{
    require_allocation_failure_support();
    for (std::size_t failure{};; ++failure)
    {
        CAPTURE(failure);
        REQUIRE(failure < ALLOCATION_FAILURE_ATTEMPT_LIMIT);
        gate_layout<cartesian_layout> lyt{{2, 2, 2}};
        const auto                    source = lyt.create_pi("a", {0, 0});
        bool                          created{};
        allocation_budget = failure;
        try
        {
            lyt.create_buf(source, {0, 0, 1});
            created = true;
        }
        catch (const std::bad_alloc&)
        {
            allocation_budget.reset();
        }
        catch (...)
        {
            allocation_budget.reset();
            throw;
        }
        allocation_budget.reset();
        CHECK(lyt.num_crossings() == static_cast<uint32_t>(created));
        CHECK(lyt.size() == (created ? 2u : 1u));
        if (created)
        {
            CHECK(failure > 0);
            break;
        }
    }
}

TEST_CASE("Failed large-arity creation leaves no object or connections", "[gate-layout]")
{
    require_allocation_failure_support();
    const bool reuse = GENERATE(false, true);
    INFO("Creating a five-input object with slot reuse = " << reuse);
    for (std::size_t failure = 0;; ++failure)
    {
        CAPTURE(failure);
        REQUIRE(failure < ALLOCATION_FAILURE_ATTEMPT_LIMIT);
        gate_layout<cartesian_layout> lyt{{8, 8}};
        const auto                    a = lyt.create_pi("a", {0, 0});
        kitty::dynamic_truth_table    parity{5};
        kitty::create_from_hex_string(parity, "96696996");
        lyt.remove(lyt.create_gate({}, parity, {2, 2}));
        if (!reuse)
        {
            lyt.create_pi("extra", {3, 3});
        }
        const auto initial_size = lyt.size();
        bool       created{};
        allocation_budget = failure;
        try
        {
            lyt.create_gate({a, a, a, a, a}, parity, {2, 2});
            created = true;
        }
        catch (const std::bad_alloc&)
        {
            created = false;
        }
        catch (...)
        {
            allocation_budget.reset();
            throw;
        }
        allocation_budget.reset();
        if (created)
        {
            CHECK(failure > 0);
            break;
        }
        CHECK(lyt.size() == initial_size);
        CHECK_FALSE(lyt.find_object({2, 2}).has_value());
        CHECK(lyt.fanout_size(a) == 0);
        const auto recovered = lyt.create_gate({a}, parity, {2, 2});
        CHECK(lyt.source({recovered, 0}) == a);
        CHECK_FALSE(lyt.source({recovered, 4}).has_value());
    }
}

TEST_CASE("Moved-from layouts remain reusable after allocation failure", "[gate-layout]")
{
    require_allocation_failure_support();
    for (std::size_t failure = 0;; ++failure)
    {
        CAPTURE(failure);
        REQUIRE(failure < ALLOCATION_FAILURE_ATTEMPT_LIMIT);
        cartesian_gate_layout source{{4, 4}};
        const auto            original = source.create_pi("original", {0, 0});
        /** @brief Destination that retains the original object after moving the layout. */
        const cartesian_gate_layout destination{std::move(source)};
        bool                        created{};
        allocation_budget = failure;
        try
        {
            // The layout contract permits moved-from reuse.
            // NOLINTNEXTLINE(*-use-after-move,*.Move,*-invalid-access-moved)
            source.create_pi("first", {0, 0});
            created = true;
        }
        catch (const std::bad_alloc&)
        {
            created = false;
        }
        catch (...)
        {
            allocation_budget.reset();
            throw;
        }
        allocation_budget.reset();
        source.clear_tile({0, 0});
        const auto a    = source.create_pi("a", {0, 0});
        const auto b    = source.create_pi("b", {1, 0});
        const auto gate = source.create_and(a, b, {1, 1});
        CHECK(source.object_function(a).num_vars() == 1);
        CHECK(source.object_function(gate).num_vars() == 2);
        CHECK(kitty::get_bit(source.object_function(gate), 3));
        CHECK_FALSE(kitty::get_bit(source.object_function(gate), 0));
        CHECK(destination.get_name(original) == "original");
        if (created)
        {
            CHECK(failure > 0);
            break;
        }
    }
}

TEST_CASE("Failed creation preserves object iteration across storage growth", "[gate-layout]")
{
    require_allocation_failure_support();
    const auto initial_count = GENERATE(0, 64, 128);
    CAPTURE(initial_count);
    for (std::size_t failure{};; ++failure)
    {
        CAPTURE(failure);
        REQUIRE(failure < ALLOCATION_FAILURE_ATTEMPT_LIMIT);
        gate_layout<cartesian_layout> lyt{};
        std::set<layout_object_id>    expected{};
        for (int32_t x{}; x < initial_count; ++x)
        {
            expected.insert(lyt.create_buf({x, 0}));
        }
        bool             created{};
        layout_object_id added{};
        allocation_budget = failure;
        try
        {
            added   = lyt.create_buf({initial_count, 0});
            created = true;
        }
        catch (const std::bad_alloc&)
        {
            allocation_budget.reset();
        }
        catch (...)
        {
            allocation_budget.reset();
            throw;
        }
        allocation_budget.reset();
        if (created)
        {
            expected.insert(added);
        }
        std::vector<layout_object_id> visited{};
        lyt.foreach_object([&](const auto id) { visited.push_back(id); });
        std::ranges::sort(visited);
        CHECK_THAT(visited, RangeEquals(expected));
        CHECK(lyt.size() == expected.size());
        if (created)
        {
            CHECK(failure > 0);
            break;
        }
        CHECK_FALSE(lyt.find_object({initial_count, 0}).has_value());
        const auto recovered = lyt.create_buf({initial_count, 0});
        CHECK(lyt.contains(recovered));
        expected.insert(recovered);
        visited.clear();
        lyt.foreach_object([&](const auto id) { visited.push_back(id); });
        std::ranges::sort(visited);
        CHECK_THAT(visited, RangeEquals(expected));
    }
}

TEST_CASE("Failed name assignment preserves name presence", "[gate-layout]")
{
    require_allocation_failure_support();
    /** Long name whose assignment requires string storage. */
    const std::string name(128, 'a');
    for (std::size_t failure{};; ++failure)
    {
        CAPTURE(failure);
        REQUIRE(failure < ALLOCATION_FAILURE_ATTEMPT_LIMIT);
        /** Unnamed object with no existing name-map entry. */
        gate_layout<cartesian_layout> layout{{1, 1}};
        /** Input whose name assignment may fail. */
        const auto input = layout.create_pi("", {0, 0});
        /** Whether the name assignment completed. */
        bool named{};
        allocation_budget = failure;
        try
        {
            layout.set_name(input, name);
            named = true;
        }
        catch (const std::bad_alloc&)
        {
            allocation_budget.reset();
        }
        catch (...)
        {
            allocation_budget.reset();
            throw;
        }
        allocation_budget.reset();
        CHECK(layout.has_name(input) == named);
        CHECK(layout.has_input_name(0) == named);
        CHECK(layout.get_name(input) == (named ? name : std::string{}));
        if (named)
        {
            CHECK(failure > 0);
            break;
        }
    }
}
