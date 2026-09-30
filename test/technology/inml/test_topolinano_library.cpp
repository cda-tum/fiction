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
 * @brief Tests for `fiction/technology/inml/topolinano_library.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author OpenAI Codex
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "fiction/layouts/io/print_layout.hpp"
#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/arrangement.hpp>
#include <fiction/technology/inml/layout.hpp>
#include <fiction/technology/inml/topolinano_library.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <iostream>
#include <stdexcept>
#include <type_traits>

using namespace fiction;
using namespace fiction::inml;
using namespace fiction::layouts;
using namespace fiction::layouts::io;

TEST_CASE("ToPoliNano library traits", "[inml-topolinano-library]")
{
    CHECK(std::is_same_v<topolinano_library::layout, inml::layout>);
    CHECK(!has_get_functional_implementations_v<topolinano_library>);
    CHECK(!has_get_gate_ports_v<topolinano_library>);
}

TEST_CASE("ToPoliNano rejects row-shifted layouts", "[inml-topolinano-library]")
{
    const auto                a = GENERATE(arrangement::ODD_ROW, arrangement::EVEN_ROW);
    shifted_cart_gate_clk_lyt lyt{a, {1, 1}};
    const auto                x = lyt.create_pi("x", {0, 0});
    const auto                y = lyt.create_pi("y", {1, 0});
    lyt.create_and(x, y, {1, 1});

    CHECK_THROWS_AS(topolinano_library::set_up_gate(lyt, {1, 1}), std::invalid_argument);
    CHECK_THROWS_AS(topolinano_library::set_up_gate(lyt, {0, 0}), std::invalid_argument);
}

TEST_CASE("Setting up input ports, gates, and wires", "[inml-topolinano-library]")
{
    const auto layout =
        blueprints::shifted_cart_and_or_inv_gate_layout<shifted_cart_gate_clk_lyt>(arrangement::ODD_COLUMN);

    print_gate_level_layout(std::cout, layout);

    // clang-format off

    static constexpr const topolinano_library::gate lower_pi{
        topolinano_library::cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {'i', 'x', 'x', 'x'},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const topolinano_library::gate upper_pi{
        topolinano_library::cell_list_to_gate<char>(
    {{
        {'i', 'x', 'x', 'x'},
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const topolinano_library::gate conjunction{
        topolinano_library::cell_list_to_gate<char>(
    {{
        {'d', ' ', ' ', ' '},
        {'d', 'x', 'x', 'x'},
        {'d', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const topolinano_library::gate disjunction{
        topolinano_library::cell_list_to_gate<char>(
    {{
        {'u', ' ', ' ', ' '},
        {'u', 'x', 'x', 'x'},
        {'u', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const topolinano_library::gate bottom_up_bent_wire{
        topolinano_library::cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', 'x'},
        {'x', 'x', 'x', 'x'},
        {'x', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const topolinano_library::gate bottom_up_bent_inverter{
        topolinano_library::cell_list_to_gate<char>(
    {{
        {'n', 'n', 'n', 'n'},
        {'x', ' ', ' ', ' '},
        {'x', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const topolinano_library::gate lower_wire{
        topolinano_library::cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {'x', 'x', 'x', 'x'},
        {'x', ' ', ' ', ' '}
    }})};

    static constexpr const topolinano_library::gate lower_po{
        topolinano_library::cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {'x', 'x', 'x', 'o'}
    }})};

    // clang-format on

    CHECK(topolinano_library::set_up_gate(layout, {0, 0}) == lower_pi);
    CHECK(topolinano_library::set_up_gate(layout, {0, 1}) == upper_pi);
    CHECK(topolinano_library::set_up_gate(layout, {0, 2}) == upper_pi);

    CHECK(topolinano_library::set_up_gate(layout, {1, 0}) == conjunction);
    CHECK(topolinano_library::set_up_gate(layout, {1, 1}) == bottom_up_bent_wire);
    CHECK(topolinano_library::set_up_gate(layout, {2, 0}) == lower_wire);
    CHECK(topolinano_library::set_up_gate(layout, {2, 1}) == bottom_up_bent_inverter);
    CHECK(topolinano_library::set_up_gate(layout, {3, 0}) == disjunction);

    CHECK(topolinano_library::set_up_gate(layout, {4, 0}) == lower_po);
}
