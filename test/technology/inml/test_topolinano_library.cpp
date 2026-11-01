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
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "fiction/layouts/layout_base.hpp"
#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/arrangement.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/physical_design/apply_gate_library.hpp>
#include <fiction/technology/fcn/gate_library.hpp>
#include <fiction/technology/inml/layout.hpp>
#include <fiction/technology/inml/topolinano_library.hpp>
#include <fiction/verification/design_rule_violations.hpp>

#include <sstream>
#include <stdexcept>
#include <type_traits>

using namespace fiction;
using namespace fiction::inml;
using namespace fiction::layouts;
using namespace fiction::physical_design;
using namespace fiction::verification;

static_assert(std::is_pointer_v<decltype(&topolinano_library::set_up_gate)>);

TEST_CASE("ToPoliNano library traits", "[inml-topolinano-library]")
{
    CHECK(std::is_same_v<topolinano_library::layout, inml::layout>);
}

TEST_CASE("Setting up input ports, gates, and wires", "[inml-topolinano-library]")
{
    const auto layout =
        blueprints::shifted_cart_and_or_inv_gate_layout<shifted_cartesian_gate_layout>(arrangement::ODD_COLUMN);

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

TEST_CASE("ToPoliNano rejects primary-input fanout", "[inml-topolinano-library]")
{
    shifted_cartesian_gate_layout lyt{arrangement::ODD_COLUMN, {2, 2}, clocking::columnar(clocking::num_clks::THREE)};
    const auto                    pi = lyt.create_pi("a", {0, 1});
    lyt.create_po(pi, "north", {1, 0});
    lyt.create_po(pi, "south", {1, 1});

    std::ostringstream          report{};
    const gate_level_drv_params ps{.out = &report};
    gate_level_drv_stats        st{};
    gate_level_drvs(lyt, ps, &st);
    INFO(report.str());
    REQUIRE(st.drvs == 0);

    CHECK_THROWS_AS(apply_gate_library<topolinano_library>(lyt),
                    fcn::unsupported_gate_type_exception<fiction::layouts::layout_base::coordinate>);
}

TEST_CASE("ToPoliNano rejects row-shifted layouts", "[inml-topolinano-library]")
{
    const auto                    a = GENERATE(arrangement::ODD_ROW, arrangement::EVEN_ROW);
    shifted_cartesian_gate_layout lyt{a, {2, 2}};
    const auto                    x = lyt.create_pi("x", {0, 0});
    const auto                    y = lyt.create_pi("y", {1, 0});
    lyt.create_and(x, y, {1, 1});

    CHECK_THROWS_AS(topolinano_library::set_up_gate(lyt, {1, 1}), std::invalid_argument);
    CHECK_THROWS_AS(topolinano_library::set_up_gate(lyt, {0, 0}), std::invalid_argument);
}

TEST_CASE("ToPoliNano straightens two-cell wire segments", "[inml-topolinano-library]")
{
    const auto   row = GENERATE(0u, 2u);
    inml::layout lyt{{3, 4}};
    lyt.assign_cell_type({0, row}, magnet_type::INPUT);
    lyt.assign_cell_name({0, row}, "x");
    lyt.assign_cell_type({1, row}, magnet_type::NORMAL);
    lyt.assign_cell_type({1, 1}, magnet_type::NORMAL);

    topolinano_library::post_layout_optimization(lyt);

    CHECK(lyt.get_cell_type({0, 1}) == magnet_type::INPUT);
    CHECK(lyt.get_cell_name({0, 1}) == "x");
    CHECK(lyt.get_cell_type({1, 1}) == magnet_type::NORMAL);
    CHECK(lyt.is_empty_cell({0, row}));
    CHECK(lyt.is_empty_cell({1, row}));
}

TEST_CASE("ToPoliNano preserves obstructed wire segments", "[inml-topolinano-library]")
{
    const auto   row = GENERATE(0u, 4u);
    inml::layout lyt{{4, 6}};
    for (const auto column : {0u, 1u, 2u})
    {
        lyt.assign_cell_type({column, row}, magnet_type::NORMAL);
    }
    const auto adjacent_row = row == 0 ? 1u : 3u;
    lyt.assign_cell_type({0, adjacent_row}, magnet_type::NORMAL);
    lyt.assign_cell_type({2, adjacent_row}, magnet_type::NORMAL);
    lyt.assign_cell_type({1, 2}, magnet_type::SLANTED_EDGE_UP_MAGNET);

    topolinano_library::post_layout_optimization(lyt);

    CHECK(lyt.get_cell_type({1, row}) == magnet_type::NORMAL);
    CHECK(lyt.is_empty_cell({1, adjacent_row}));
    CHECK(lyt.get_cell_type({1, 2}) == magnet_type::SLANTED_EDGE_UP_MAGNET);
}

TEST_CASE("ToPoliNano preserves wire segments without a neighboring row", "[inml-topolinano-library]")
{
    inml::layout lyt{{3, 1}};
    lyt.assign_cell_type({0, 0}, magnet_type::INPUT);
    lyt.assign_cell_name({0, 0}, "x");
    lyt.assign_cell_type({1, 0}, magnet_type::NORMAL);
    lyt.assign_cell_type({2, 0}, magnet_type::OUTPUT);

    topolinano_library::post_layout_optimization(lyt);

    CHECK(lyt.num_cells() == 3);
    CHECK(lyt.get_cell_type({0, 0}) == magnet_type::INPUT);
    CHECK(lyt.get_cell_name({0, 0}) == "x");
    CHECK(lyt.get_cell_type({1, 0}) == magnet_type::NORMAL);
    CHECK(lyt.get_cell_type({2, 0}) == magnet_type::OUTPUT);
}

/** @brief Vertical connections select the complete physical staircase wire. */
TEST_CASE("ToPoliNano maps vertical staircase wires", "[inml-topolinano-library]")
{
    /** @brief Direction of the wire's signal flow. */
    const auto down = GENERATE(true, false);
    CAPTURE(down);
    /** @brief Column-shifted layout with one clocked PI-wire-PO chain. */
    shifted_cartesian_gate_layout lyt{arrangement::ODD_COLUMN, {5, 5}};
    /** @brief Internal wire whose complete cell tile is checked. */
    const layout_base::coordinate middle{2, 2};
    /** @brief Incoming and outgoing tiles on opposite vertical sides. */
    const layout_base::coordinate incoming{2, down ? 1 : 3};
    /** @brief Outgoing tile on the opposite side from the input. */
    const layout_base::coordinate outgoing{2, down ? 3 : 1};
    /** @brief Primary input driving the staircase. */
    const auto input = lyt.create_pi("a", incoming);
    /** @brief Wire connecting the two vertical neighbors. */
    const auto wire = lyt.create_buf(input, middle);
    lyt.create_po(wire, "f", outgoing);
    lyt.assign_clock_number(incoming, 0);
    lyt.assign_clock_number(middle, 1);
    lyt.assign_clock_number(outgoing, 2);

    // clang-format off

    /** @brief Physical staircase from the upper left to the lower right connector. */
    static constexpr const topolinano_library::gate downward{
        topolinano_library::cell_list_to_gate<char>({{{'x', ' ', ' ', ' '},
                                                    {'x', 'x', 'x', ' '},
                                                    {' ', ' ', 'x', ' '},
                                                    {' ', ' ', 'x', 'x'}}})};
    /** @brief Physical staircase from the lower left to the upper right connector. */
    static constexpr const topolinano_library::gate upward{
        topolinano_library::cell_list_to_gate<char>({{{' ', ' ', 'x', 'x'},
                                                    {' ', ' ', 'x', ' '},
                                                    {'x', 'x', 'x', ' '},
                                                    {'x', ' ', ' ', ' '}}})};

    // clang-format on

    CHECK(topolinano_library::set_up_gate(lyt, middle) == (down ? downward : upward));
}

/** @brief A wire feeding a northern majority gate uses the short top-edge connection. */
TEST_CASE("ToPoliNano connects a wire to a northern majority gate", "[inml-topolinano-library]")
{
    /** @brief Clocked majority circuit with a separate wire on one input. */
    shifted_cartesian_gate_layout lyt{arrangement::ODD_COLUMN, {5, 6}};
    /** @brief Primary input driving the wire. */
    const auto a = lyt.create_pi("a", {1, 2});
    /** @brief Second input of the majority gate. */
    const auto b = lyt.create_pi("b", {1, 1});
    /** @brief Third input of the majority gate. */
    const auto c = lyt.create_pi("c", {3, 1});
    /** @brief Wire immediately south of the majority gate. */
    const auto wire = lyt.create_buf(a, {2, 3});
    /** @brief Majority gate with three adjacent inputs. */
    const auto majority = lyt.create_maj(wire, b, c, {2, 2});
    lyt.create_po(majority, "f", {2, 1});
    lyt.assign_clock_number({1, 2}, 0);
    lyt.assign_clock_number({2, 3}, 1);
    lyt.assign_clock_number({1, 1}, 1);
    lyt.assign_clock_number({3, 1}, 1);
    lyt.assign_clock_number({2, 2}, 2);
    lyt.assign_clock_number({2, 1}, 3);

    // clang-format off

    /** @brief Complete physical wire entering the majority gate from below. */
    static constexpr const topolinano_library::gate expected{
        topolinano_library::cell_list_to_gate<char>({{{'x', 'x', ' ', ' '},
                                                    {' ', ' ', ' ', ' '},
                                                    {' ', ' ', ' ', ' '},
                                                    {' ', ' ', ' ', ' '}}})};

    // clang-format on

    CHECK(topolinano_library::set_up_gate(lyt, {2, 3}) == expected);
}

/** @brief The inverter preserves the physical connection from a southwest AND gate. */
TEST_CASE("ToPoliNano connects an inverter to a southwest AND gate", "[inml-topolinano-library]")
{
    /** @brief Adjacent clocked AND-inverter circuit. */
    shifted_cartesian_gate_layout lyt{arrangement::ODD_COLUMN, {5, 5}};
    /** @brief First input of the AND gate. */
    const auto a = lyt.create_pi("a", {0, 2});
    /** @brief Second input of the AND gate. */
    const auto b = lyt.create_pi("b", {0, 3});
    /** @brief AND gate southwest of the inverter. */
    const auto conjunction = lyt.create_and(a, b, {1, 2});
    /** @brief Inverter driving the northeast primary output. */
    const auto inverter = lyt.create_not(conjunction, {2, 2});
    lyt.create_po(inverter, "f", {3, 1});
    lyt.assign_clock_number({0, 2}, 0);
    lyt.assign_clock_number({0, 3}, 0);
    lyt.assign_clock_number({1, 2}, 1);
    lyt.assign_clock_number({2, 2}, 2);
    lyt.assign_clock_number({3, 1}, 3);

    // clang-format off

    /** @brief Complete physical inverter from the lower-left to upper-right connector. */
    static constexpr const topolinano_library::gate expected{
        topolinano_library::cell_list_to_gate<char>({{{'n', 'n', 'n', 'n'},
                                                    {'x', ' ', ' ', ' '},
                                                    {'x', ' ', ' ', ' '},
                                                    {'x', ' ', ' ', ' '}}})};

    // clang-format on

    CHECK(topolinano_library::set_up_gate(lyt, {2, 2}) == expected);
}

/** @brief An adjacent clocked NAND gate has no ToPoliNano implementation. */
TEST_CASE("ToPoliNano rejects an unsupported NAND gate", "[inml-topolinano-library]")
{
    /** @brief Clocked NAND circuit with two adjacent primary inputs. */
    shifted_cartesian_gate_layout lyt{arrangement::ODD_COLUMN, {5, 5}};
    /** @brief First input northwest of the NAND gate. */
    const auto a = lyt.create_pi("a", {1, 1});
    /** @brief Second input southwest of the NAND gate. */
    const auto b = lyt.create_pi("b", {1, 2});
    /** @brief Gate coordinate reported by the rejection. */
    const layout_base::coordinate coordinate{2, 2};
    /** @brief Unsupported gate driving the primary output. */
    const auto nand = lyt.create_nand(a, b, coordinate);
    lyt.create_po(nand, "f", {3, 1});
    lyt.assign_clock_number({1, 1}, 0);
    lyt.assign_clock_number({1, 2}, 0);
    lyt.assign_clock_number(coordinate, 1);
    lyt.assign_clock_number({3, 1}, 2);

    try
    {
        static_cast<void>(topolinano_library::set_up_gate(lyt, coordinate));
        FAIL("The gate library has no NAND implementation");
    }
    catch (const fcn::unsupported_gate_type_exception<layout_base::coordinate>& error)
    {
        CHECK(error.where() == coordinate);
    }
}
