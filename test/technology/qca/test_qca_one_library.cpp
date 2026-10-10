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

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/technology/fcn/gate_library.hpp>
#include <fiction/technology/qca/layout.hpp>
#include <fiction/technology/qca/qca_one_library.hpp>
#include <fiction/traits.hpp>

#include <cstdint>
#include <type_traits>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::qca;

TEST_CASE("QCA ONE library traits", "[qca-one-library]")
{
    CHECK(std::is_same_v<qca_one_library::layout, qca::layout>);
    CHECK(!has_get_functional_implementations_v<qca_one_library>);
    CHECK(!has_get_gate_ports_v<qca_one_library>);
}

TEST_CASE("Setting up input ports and gates", "[qca-one-library]")
{
    using gate_layout = gate_level_layout<cartesian_layout>;

    auto layout = blueprints::or_not_gate_layout<gate_layout>();

    // clang-format off

    static constexpr const qca_one_library::gate primary_input_port{
        qca_one_library::cell_list_to_gate<char>({{{' ', ' ', 'x', ' ', ' '},
                                                   {' ', ' ', 'x', ' ', ' '},
                                                   {' ', ' ', 'i', ' ', ' '},
                                                   {' ', ' ', ' ', ' ', ' '},
                                                   {' ', ' ', ' ', ' ', ' '}}})};

    static constexpr const qca_one_library::gate primary_output_port{
        qca_one_library::cell_list_to_gate<char>({{{' ', ' ', 'x', ' ', ' '},
                                                   {' ', ' ', 'x', ' ', ' '},
                                                   {' ', ' ', 'o', ' ', ' '},
                                                   {' ', ' ', ' ', ' ', ' '},
                                                   {' ', ' ', ' ', ' ', ' '}}})};

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

    CHECK(qca_one_library::set_up_gate(layout, {0, 1}) == qca_one_library::rotate_90(primary_input_port));
    CHECK(qca_one_library::set_up_gate(layout, {1, 0}) == qca_one_library::rotate_180(primary_input_port));
    CHECK(qca_one_library::set_up_gate(layout, {1, 1}) == qca_one_library::rotate_90(disjunction));
    CHECK(qca_one_library::set_up_gate(layout, {1, 2}) == bent_inverter);
    CHECK(qca_one_library::set_up_gate(layout, {2, 2}) == qca_one_library::rotate_270(primary_output_port));
}

TEST_CASE("Setting up wires", "[qca-one-library]")
{
    using gate_layout = gate_level_layout<cartesian_layout>;

    auto layout = blueprints::crossing_layout<gate_layout>();

    // clang-format off

    static constexpr const qca_one_library::gate primary_input_port{
        qca_one_library::cell_list_to_gate<char>({{{' ', ' ', 'x', ' ', ' '},
                                                   {' ', ' ', 'x', ' ', ' '},
                                                   {' ', ' ', 'i', ' ', ' '},
                                                   {' ', ' ', ' ', ' ', ' '},
                                                   {' ', ' ', ' ', ' ', ' '}}})};

    static constexpr const qca_one_library::gate primary_output_port{
        qca_one_library::cell_list_to_gate<char>({{{' ', ' ', 'x', ' ', ' '},
                                                   {' ', ' ', 'x', ' ', ' '},
                                                   {' ', ' ', 'o', ' ', ' '},
                                                   {' ', ' ', ' ', ' ', ' '},
                                                   {' ', ' ', ' ', ' ', ' '}}})};

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

    CHECK(qca_one_library::set_up_gate(layout, {0, 1}) == qca_one_library::rotate_90(primary_input_port));
    CHECK(qca_one_library::set_up_gate(layout, {0, 2}) == qca_one_library::rotate_90(primary_input_port));
    CHECK(qca_one_library::set_up_gate(layout, {1, 0}) == qca_one_library::rotate_180(primary_input_port));
    CHECK(qca_one_library::set_up_gate(layout, {2, 0}) == qca_one_library::rotate_180(primary_input_port));
    CHECK(qca_one_library::set_up_gate(layout, {1, 1}) == qca_one_library::rotate_180(conjunction));
    CHECK(qca_one_library::set_up_gate(layout, {2, 2}) == qca_one_library::rotate_180(conjunction));
    CHECK(qca_one_library::set_up_gate(layout, {2, 1}) == wire);
    CHECK(qca_one_library::set_up_gate(layout, {1, 2}) == qca_one_library::rotate_90(wire));
    CHECK(qca_one_library::set_up_gate(layout, {2, 1, 1}) == qca_one_library::rotate_90(wire));
    CHECK(qca_one_library::set_up_gate(layout, {3, 1}) == qca_one_library::rotate_270(primary_output_port));
    CHECK(qca_one_library::set_up_gate(layout, {3, 2}) == qca_one_library::rotate_270(primary_output_port));
}

TEST_CASE("Setting up fanouts", "[qca-one-library]")
{
    using gate_layout = gate_level_layout<cartesian_layout>;

    auto layout = blueprints::fanout_layout<gate_layout>();

    // clang-format off

    static constexpr const qca_one_library::gate primary_input_port{
        qca_one_library::cell_list_to_gate<char>({{{' ', ' ', 'x', ' ', ' '},
                                                   {' ', ' ', 'x', ' ', ' '},
                                                   {' ', ' ', 'i', ' ', ' '},
                                                   {' ', ' ', ' ', ' ', ' '},
                                                   {' ', ' ', ' ', ' ', ' '}}})};

    static constexpr const qca_one_library::gate primary_output_port{
        qca_one_library::cell_list_to_gate<char>({{{' ', ' ', 'x', ' ', ' '},
                                                   {' ', ' ', 'x', ' ', ' '},
                                                   {' ', ' ', 'o', ' ', ' '},
                                                   {' ', ' ', ' ', ' ', ' '},
                                                   {' ', ' ', ' ', ' ', ' '}}})};

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

    CHECK(qca_one_library::set_up_gate(layout, {0, 1}) == qca_one_library::rotate_90(primary_input_port));
    CHECK(qca_one_library::set_up_gate(layout, {1, 0}) == qca_one_library::rotate_180(primary_output_port));
    CHECK(qca_one_library::set_up_gate(layout, {2, 0}) == qca_one_library::rotate_180(primary_output_port));
    CHECK(qca_one_library::set_up_gate(layout, {1, 2}) == qca_one_library::rotate_90(primary_output_port));
    CHECK(qca_one_library::set_up_gate(layout, {1, 1}) == qca_one_library::rotate_180(fanout));
    CHECK(qca_one_library::set_up_gate(layout, {2, 1}) == qca_one_library::rotate_90(fanout));
    CHECK(qca_one_library::set_up_gate(layout, {2, 2}) == qca_one_library::rotate_270(bent_wire));
}

TEST_CASE("QCA ONE rejects an unoccupied tile", "[qca-one-library]")
{
    const gate_level_layout<cartesian_layout> layout{{2, 2}};
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
        CHECK(layout.get_cell_mode({x, 5, 1}) == cell_mode::VERTICAL);
        CHECK(layout.get_cell_type({x, 5, 0}) == cell_type::NORMAL);
        CHECK(layout.get_cell_mode({x, 5, 0}) == cell_mode::VERTICAL);
    }
}
