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
 * @brief Tests for `fiction/technology/qca/qca_one_library.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/layouts/layout_base.hpp>
#include <fiction/physical_design/apply_gate_library.hpp>
#include <fiction/technology/fcn/cell_ports.hpp>
#include <fiction/technology/fcn/gate_library.hpp>
#include <fiction/technology/qca/layout.hpp>
#include <fiction/technology/qca/qca_one_library.hpp>
#include <fiction/verification/design_rule_violations.hpp>

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>

#include <cstdint>
#include <sstream>
#include <type_traits>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::physical_design;
using namespace fiction::verification;
using namespace fiction::qca;

// clang-format off

/** @brief Expected input port with its connector on the north edge. */
static constexpr const qca_one_library::gate PRIMARY_INPUT_PORT{
    qca_one_library::cell_list_to_gate<char>({{{' ', ' ', 'x', ' ', ' '},
                                               {' ', ' ', 'x', ' ', ' '},
                                               {' ', ' ', 'i', ' ', ' '},
                                               {' ', ' ', ' ', ' ', ' '},
                                               {' ', ' ', ' ', ' ', ' '}}})};

/** @brief Expected output port with its connector on the north edge. */
static constexpr const qca_one_library::gate PRIMARY_OUTPUT_PORT{
    qca_one_library::cell_list_to_gate<char>({{{' ', ' ', 'x', ' ', ' '},
                                               {' ', ' ', 'x', ' ', ' '},
                                               {' ', ' ', 'o', ' ', ' '},
                                               {' ', ' ', ' ', ' ', ' '},
                                               {' ', ' ', ' ', ' ', ' '}}})};

// clang-format on

TEST_CASE("QCA ONE library traits", "[qca-one-library]")
{
    STATIC_REQUIRE(std::is_same_v<qca_one_library::layout, qca::layout>);
}

TEST_CASE("Setting up input ports and gates", "[qca-one-library]")
{
    auto layout = blueprints::or_not_gate_layout<cartesian_gate_layout>();

    // clang-format off

    static constexpr const qca_one_library::gate disjunction{
        qca_one_library::cell_list_to_gate<char>({{{' ', ' ', '1', ' ', ' '},
                                                   {' ', ' ', 'x', ' ', ' '},
                                                   {'x', 'x', 'x', 'x', 'x'},
                                                   {' ', ' ', 'x', ' ', ' '},
                                                   {' ', ' ', 'x', ' ', ' '}}})};

    static constexpr const qca_one_library::gate bent_inverter{
        qca_one_library::cell_list_to_gate<char>({{{' ', ' ', 'x', ' ', ' '},
                                                   {' ', ' ', 'x', ' ', ' '},
                                                   {' ', ' ', ' ', 'x', 'x'},
                                                   {' ', ' ', ' ', ' ', ' '},
                                                   {' ', ' ', ' ', ' ', ' '}}})};

    // clang-format on

    CHECK(qca_one_library::set_up_gate(layout, {0, 1}) == qca_one_library::rotate_90(PRIMARY_INPUT_PORT));
    CHECK(qca_one_library::set_up_gate(layout, {1, 0}) == qca_one_library::rotate_180(PRIMARY_INPUT_PORT));
    CHECK(qca_one_library::set_up_gate(layout, {1, 1}) == qca_one_library::rotate_90(disjunction));
    CHECK(qca_one_library::set_up_gate(layout, {1, 2}) == bent_inverter);
    CHECK(qca_one_library::set_up_gate(layout, {2, 2}) == qca_one_library::rotate_270(PRIMARY_OUTPUT_PORT));
}

TEST_CASE("Setting up wires", "[qca-one-library]")
{
    auto layout = blueprints::crossing_layout<cartesian_gate_layout>();

    // clang-format off

    static constexpr const qca_one_library::gate conjunction{
        qca_one_library::cell_list_to_gate<char>({{{' ', ' ', '0', ' ', ' '},
                                                   {' ', ' ', 'x', ' ', ' '},
                                                   {'x', 'x', 'x', 'x', 'x'},
                                                   {' ', ' ', 'x', ' ', ' '},
                                                   {' ', ' ', 'x', ' ', ' '}}})};

    static constexpr const qca_one_library::gate wire{
        qca_one_library::cell_list_to_gate<char>({{{' ', ' ', 'x', ' ', ' '},
                                                   {' ', ' ', 'x', ' ', ' '},
                                                   {' ', ' ', 'x', ' ', ' '},
                                                   {' ', ' ', 'x', ' ', ' '},
                                                   {' ', ' ', 'x', ' ', ' '}}})};

    // clang-format on

    CHECK(qca_one_library::set_up_gate(layout, {0, 1}) == qca_one_library::rotate_90(PRIMARY_INPUT_PORT));
    CHECK(qca_one_library::set_up_gate(layout, {0, 2}) == qca_one_library::rotate_90(PRIMARY_INPUT_PORT));
    CHECK(qca_one_library::set_up_gate(layout, {1, 0}) == qca_one_library::rotate_180(PRIMARY_INPUT_PORT));
    CHECK(qca_one_library::set_up_gate(layout, {2, 0}) == qca_one_library::rotate_180(PRIMARY_INPUT_PORT));
    CHECK(qca_one_library::set_up_gate(layout, {1, 1}) == qca_one_library::rotate_180(conjunction));
    CHECK(qca_one_library::set_up_gate(layout, {2, 2}) == qca_one_library::rotate_180(conjunction));
    CHECK(qca_one_library::set_up_gate(layout, {2, 1}) == wire);
    CHECK(qca_one_library::set_up_gate(layout, {1, 2}) == qca_one_library::rotate_90(wire));
    CHECK(qca_one_library::set_up_gate(layout, {2, 1, 1}) == qca_one_library::rotate_90(wire));
    CHECK(qca_one_library::set_up_gate(layout, {3, 1}) == qca_one_library::rotate_270(PRIMARY_OUTPUT_PORT));
    CHECK(qca_one_library::set_up_gate(layout, {3, 2}) == qca_one_library::rotate_270(PRIMARY_OUTPUT_PORT));
}

TEST_CASE("Setting up fanouts", "[qca-one-library]")
{
    auto layout = blueprints::fanout_layout<cartesian_gate_layout>();

    // clang-format off

    static constexpr const qca_one_library::gate fanout{
        qca_one_library::cell_list_to_gate<char>({{{' ', ' ', ' ', ' ', ' '},
                                                   {' ', ' ', ' ', ' ', ' '},
                                                   {'x', 'x', 'x', 'x', 'x'},
                                                   {' ', ' ', 'x', ' ', ' '},
                                                   {' ', ' ', 'x', ' ', ' '}}})};

    static constexpr const qca_one_library::gate bent_wire{
        qca_one_library::cell_list_to_gate<char>({{{' ', ' ', 'x', ' ', ' '},
                                                   {' ', ' ', 'x', ' ', ' '},
                                                   {' ', ' ', 'x', 'x', 'x'},
                                                   {' ', ' ', ' ', ' ', ' '},
                                                   {' ', ' ', ' ', ' ', ' '}}})};

    // clang-format on

    CHECK(qca_one_library::set_up_gate(layout, {0, 1}) == qca_one_library::rotate_90(PRIMARY_INPUT_PORT));
    CHECK(qca_one_library::set_up_gate(layout, {1, 0}) == qca_one_library::rotate_180(PRIMARY_OUTPUT_PORT));
    CHECK(qca_one_library::set_up_gate(layout, {2, 0}) == qca_one_library::rotate_180(PRIMARY_OUTPUT_PORT));
    CHECK(qca_one_library::set_up_gate(layout, {1, 2}) == qca_one_library::rotate_90(PRIMARY_OUTPUT_PORT));
    CHECK(qca_one_library::set_up_gate(layout, {1, 1}) == qca_one_library::rotate_180(fanout));
    CHECK(qca_one_library::set_up_gate(layout, {2, 1}) == qca_one_library::rotate_90(fanout));
    CHECK(qca_one_library::set_up_gate(layout, {2, 2}) == qca_one_library::rotate_270(bent_wire));
}

TEST_CASE("QCA ONE rejects primary-input fanout", "[qca-one-library]")
{
    gate_layout<cartesian_layout> lyt{{2, 3}};
    const auto                    pi = lyt.create_pi("a", {0, 1});
    lyt.create_po(pi, "north", {0, 0});
    lyt.create_po(pi, "east", {1, 1});
    lyt.create_po(pi, "south", {0, 2});
    lyt.assign_clock_number({0, 1}, 0);
    lyt.assign_clock_number({0, 0}, 1);
    lyt.assign_clock_number({1, 1}, 1);
    lyt.assign_clock_number({0, 2}, 1);

    std::ostringstream          report{};
    const gate_level_drv_params ps{.out = &report};
    gate_level_drv_stats        st{};
    gate_level_drvs(lyt, ps, &st);
    INFO(report.str());
    REQUIRE(st.drvs == 0);

    CHECK_THROWS_AS(apply_gate_library<qca_one_library>(lyt),
                    fcn::unsupported_gate_type_exception<fiction::layouts::layout_base::coordinate>);
}

TEST_CASE("QCA ONE rejects an unoccupied tile", "[qca-one-library]")
{
    const gate_layout<cartesian_layout> layout{{2, 2}};
    CHECK_THROWS_AS(qca_one_library::set_up_gate(layout, {0, 0}),
                    fcn::unsupported_gate_type_exception<cartesian_layout::coordinate>);
}

TEST_CASE("QCA ONE vias in a sparse frame", "[qca-one-library]")
{
    /** Sparse geometry spanning the complete nonnegative x domain. */
    qca::layout layout{{uint32_t{1} << 31u, 8, 2}};
    layout.assign_cell_type({4, 1, 1}, cell_type::NORMAL);
    layout.assign_cell_type({4, 2, 1}, cell_type::NORMAL);
    layout.assign_cell_type({4, 3, 1}, cell_type::NORMAL);
    layout.assign_cell_type({4, 1, 0}, cell_type::OUTPUT);
    layout.assign_cell_type({-1, 1, 1}, cell_type::NORMAL);
    layout.assign_cell_type({4, 8, 1}, cell_type::OUTPUT);
    layout.assign_cell_type({4, 2, 3}, cell_type::NORMAL);

    for (int32_t x = 16; x < 136; x += 3)
    {
        layout.assign_cell_type({x, 5, 1}, cell_type::NORMAL);
    }

    qca_one_library::post_layout_optimization(layout);

    CHECK(layout.get_cell_mode({4, 1, 1}) == cell_mode::VERTICAL);
    CHECK(layout.get_cell_mode({4, 3, 1}) == cell_mode::VERTICAL);
    CHECK(layout.get_cell_mode({4, 2, 1}) == cell_mode::NORMAL);
    CHECK(layout.get_cell_type({4, 1, 0}) == cell_type::NORMAL);
    CHECK(layout.get_cell_type({4, 3, 0}) == cell_type::NORMAL);
    CHECK(layout.get_cell_mode({4, 1, 0}) == cell_mode::VERTICAL);
    CHECK(layout.get_cell_mode({4, 3, 0}) == cell_mode::VERTICAL);
    CHECK(layout.get_cell_mode({-1, 1, 1}) == cell_mode::NORMAL);
    CHECK(layout.get_cell_type({-1, 1, 0}) == cell_type::EMPTY);
    CHECK(layout.get_cell_mode({4, 8, 1}) == cell_mode::NORMAL);
    CHECK(layout.get_cell_type({4, 8, 1}) == cell_type::OUTPUT);
    CHECK(layout.get_cell_type({4, 8, 0}) == cell_type::EMPTY);
    CHECK(layout.get_cell_mode({4, 2, 3}) == cell_mode::NORMAL);
    CHECK(layout.get_cell_type({4, 2, 0}) == cell_type::EMPTY);
    for (int32_t x = 16; x < 136; x += 3)
    {
        CAPTURE(x);
        CHECK(layout.get_cell_mode({x, 5, 1}) == cell_mode::VERTICAL);
        CHECK(layout.get_cell_type({x, 5, 0}) == cell_type::NORMAL);
        CHECK(layout.get_cell_mode({x, 5, 0}) == cell_mode::VERTICAL);
    }
}

/** @brief An incomplete AND gate has no supported physical orientation. */
TEST_CASE("QCA ONE rejects a disconnected AND gate", "[qca-one-library]")
{
    cartesian_gate_layout lyt{{3, 3}};
    /** @brief AND truth table with both input slots disconnected. */
    kitty::dynamic_truth_table function{2};
    kitty::create_from_hex_string(function, "8");
    /** @brief Coordinate reported by the rejected mapping. */
    const layout_base::coordinate coordinate{1, 1};
    lyt.create_gate({}, function, coordinate);

    try
    {
        static_cast<void>(qca_one_library::set_up_gate(lyt, coordinate));
        FAIL("A disconnected AND gate has no supported orientation");
    }
    catch (const fcn::unsupported_gate_orientation_exception<layout_base::coordinate, fcn::port_position>& error)
    {
        CHECK(error.where() == coordinate);
    }
}
