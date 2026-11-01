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
 * @brief Tests for `fiction/technology/mol_qca/sim7_mol_library.hpp`.
 * @author Benjamin Hien (hibenj)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/layouts/layout_base.hpp>
#include <fiction/technology/fcn/cell_ports.hpp>
#include <fiction/technology/fcn/gate_library.hpp>
#include <fiction/technology/mol_qca/layout.hpp>
#include <fiction/technology/mol_qca/sim7_mol_library.hpp>

#include <array>
#include <type_traits>

using namespace fiction;
using namespace fiction::fcn;
using namespace fiction::layouts;
using namespace fiction::mol_qca;

// clang-format off

/** @brief Expected primary input cells before orientation. */
static constexpr const sim7_mol_library::gate PRIMARY_INPUT_PORT{
sim7_mol_library::cell_list_to_gate<char>(
{{
    {' ', ' ', ' ', ' ', 'i', 'i', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}
}})};

/** @brief Expected primary output cells before orientation. */
static constexpr const sim7_mol_library::gate PRIMARY_OUTPUT_PORT{
sim7_mol_library::cell_list_to_gate<char>(
{{
    {' ', ' ', ' ', ' ', 'o', 'o', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}
}})};

// clang-format on

TEST_CASE("Molecular QCA library traits", "[molecular-qca-library]")
{
    STATIC_REQUIRE(std::is_same_v<sim7_mol_library::layout, mol_qca::layout>);
}

TEST_CASE("Setting up input ports and gates", "[molecular-qca-library]")
{
    auto layout = blueprints::or_not_gate_layout<cartesian_gate_layout>();

    // clang-format off

    static constexpr const sim7_mol_library::gate disjunction{
    sim7_mol_library::cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', '1', '1', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {'a', 'a', 'a', 'b', 'b', 'b', 'b', 'c', 'd', 'd'},
        {'a', 'a', 'a', 'b', 'b', 'b', 'b', 'c', 'd', 'd'},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const sim7_mol_library::gate bent_inverter{
    sim7_mol_library::cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', 'b', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', 'b', ' ', 'd', 'd'},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', 'c', 'c', 'd'},
        {' ', ' ', ' ', ' ', 'b', 'b', ' ', 'c', 'c', 'd'},
        {' ', ' ', ' ', ' ', 'b', 'b', 'b', ' ', 'd', 'd'},
        {' ', ' ', ' ', ' ', 'b', 'b', 'b', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}
    }})};

    // clang-format on

    CHECK(sim7_mol_library::set_up_gate(layout, {0, 1}) == sim7_mol_library::rotate_90(PRIMARY_INPUT_PORT));
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 0}) == sim7_mol_library::rotate_180(PRIMARY_INPUT_PORT));
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 1}) == sim7_mol_library::rotate_90(disjunction));
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 2}) == bent_inverter);
    CHECK(sim7_mol_library::set_up_gate(layout, {2, 2}) == sim7_mol_library::rotate_270(PRIMARY_OUTPUT_PORT));
}

TEST_CASE("Setting up wires", "[molecular-qca-library]")
{
    auto layout = blueprints::three_wire_paths_gate_layout<cartesian_gate_layout>();

    // clang-format off

    static constexpr const sim7_mol_library::gate wire{
    sim7_mol_library::cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'b', 'b', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'b', 'b', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'c', 'c', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'c', 'c', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '}
    }})};

    // clang-format on

    CHECK(sim7_mol_library::set_up_gate(layout, {0, 0}) == sim7_mol_library::rotate_90(PRIMARY_INPUT_PORT));
    CHECK(sim7_mol_library::set_up_gate(layout, {0, 2}) == sim7_mol_library::rotate_90(PRIMARY_INPUT_PORT));
    CHECK(sim7_mol_library::set_up_gate(layout, {0, 4}) == sim7_mol_library::rotate_90(PRIMARY_INPUT_PORT));
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 0}) == sim7_mol_library::rotate_270(wire));
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 2}) == sim7_mol_library::rotate_270(wire));
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 4}) == sim7_mol_library::rotate_270(wire));
    CHECK(sim7_mol_library::set_up_gate(layout, {2, 0}) == sim7_mol_library::rotate_270(wire));
    CHECK(sim7_mol_library::set_up_gate(layout, {2, 2}) == sim7_mol_library::rotate_270(wire));
    CHECK(sim7_mol_library::set_up_gate(layout, {2, 4}) == sim7_mol_library::rotate_270(wire));
    CHECK(sim7_mol_library::set_up_gate(layout, {3, 0}) == sim7_mol_library::rotate_270(wire));
    CHECK(sim7_mol_library::set_up_gate(layout, {3, 2}) == sim7_mol_library::rotate_270(wire));
    CHECK(sim7_mol_library::set_up_gate(layout, {3, 4}) == sim7_mol_library::rotate_270(wire));
    CHECK(sim7_mol_library::set_up_gate(layout, {4, 0}) == sim7_mol_library::rotate_270(PRIMARY_OUTPUT_PORT));
    CHECK(sim7_mol_library::set_up_gate(layout, {4, 2}) == sim7_mol_library::rotate_270(PRIMARY_OUTPUT_PORT));
    CHECK(sim7_mol_library::set_up_gate(layout, {4, 4}) == sim7_mol_library::rotate_270(PRIMARY_OUTPUT_PORT));
}

TEST_CASE("Setting up fanouts", "[molecular-qca-library]")
{
    auto layout = blueprints::fanout_layout<cartesian_gate_layout>();

    // clang-format off

    static constexpr const sim7_mol_library::gate fanout{
    sim7_mol_library::cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {'d', 'd', 'd', 'd', 'c', 'c', 'b', 'b', 'a', 'a'},
        {'d', 'd', 'd', 'd', 'c', 'c', 'b', 'b', 'a', 'a'},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const sim7_mol_library::gate fanout_d{
    sim7_mol_library::cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {'d', 'd', 'd', 'd', 'c', 'c', 'd', 'd', 'd', 'd'},
        {'d', 'd', 'd', 'd', 'c', 'c', 'd', 'd', 'd', 'd'},
        {' ', ' ', ' ', ' ', 'b', 'b', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'b', 'b', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const sim7_mol_library::gate bent_wire{
    sim7_mol_library::cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'c', 'c', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'c', 'c', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'b', 'b', 'b', 'a', 'a', 'a'},
        {' ', ' ', ' ', ' ', 'b', 'b', 'b', 'a', 'a', 'a'},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}
    }})};

    // clang-format on

    CHECK(sim7_mol_library::set_up_gate(layout, {0, 1}) == sim7_mol_library::rotate_90(PRIMARY_INPUT_PORT));
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 0}) == sim7_mol_library::rotate_180(PRIMARY_OUTPUT_PORT));
    CHECK(sim7_mol_library::set_up_gate(layout, {2, 0}) == sim7_mol_library::rotate_180(PRIMARY_OUTPUT_PORT));
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 2}) == sim7_mol_library::rotate_90(PRIMARY_OUTPUT_PORT));
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 1}) == sim7_mol_library::rotate_180(fanout));
    CHECK(sim7_mol_library::set_up_gate(layout, {2, 1}) == sim7_mol_library::rotate_90(fanout_d));
    CHECK(sim7_mol_library::set_up_gate(layout, {2, 2}) == sim7_mol_library::rotate_270(bent_wire));
}

TEST_CASE("Setting up fanout-3 rotations", "[molecular-qca-library]")
{
    using orientation_exception = unsupported_gate_orientation_exception<layout_base::coordinate, port_position>;

    // clang-format off

    static constexpr const sim7_mol_library::gate fanout_1_3{
    sim7_mol_library::cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {'d', 'd', 'c', 'b', 'b', 'b', 'b', 'c', 'd', 'd'},
        {'d', 'd', 'c', 'b', 'b', 'b', 'b', 'c', 'd', 'd'},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'c', 'c', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '}
    }})};

    // clang-format on

    SECTION("all input directions")
    {
        const auto [direction, input, expected] =
            GENERATE_COPY(Catch::Generators::table<const char*, layout_base::coordinate, sim7_mol_library::gate>({
                {"north", {1, 0}, fanout_1_3},
                {"east", {2, 1}, sim7_mol_library::rotate_90(fanout_1_3)},
                {"south", {1, 2}, sim7_mol_library::rotate_180(fanout_1_3)},
                {"west", {0, 1}, sim7_mol_library::rotate_270(fanout_1_3)},
            }));
        CAPTURE(direction);
        cartesian_gate_layout lyt{{3, 3, 1}};
        lyt.assign_clock_number(input, 0);
        lyt.assign_clock_number({1, 1}, 1);
        const auto pi     = lyt.create_pi("x", input);
        const auto fanout = lyt.create_buf(pi, {1, 1});
        for (const auto output : std::array<layout_base::coordinate, 4>{{{1, 0}, {2, 1}, {1, 2}, {0, 1}}})
        {
            if (output != input)
            {
                lyt.assign_clock_number(output, 2);
                lyt.create_po(fanout, "branch", output);
            }
        }
        CHECK(sim7_mol_library::set_up_gate(lyt, {1, 1}) == expected);
    }

    SECTION("clocking does not hide a physical branch")
    {
        auto clocked_layout   = cartesian_gate_layout{cartesian_gate_layout::extent{3, 3, 1}, clocking::twoddwave()};
        const auto clocked_pi = clocked_layout.create_pi("x", {1, 0});
        const auto clocked_fanout = clocked_layout.create_buf(clocked_pi, {1, 1});
        clocked_layout.create_po(clocked_fanout, "e", {2, 1});
        clocked_layout.create_po(clocked_fanout, "s", {1, 2});
        clocked_layout.create_po(clocked_fanout, "w", {0, 1});

        CHECK(clocked_layout.is_fanout(clocked_fanout));
        CHECK(clocked_layout.outgoing_data_flow({1, 1}).size() == 2u);
        CHECK(clocked_layout.fanout_size(clocked_fanout) == 3u);
        CHECK(sim7_mol_library::set_up_gate(clocked_layout, {1, 1}) == fanout_1_3);
    }

    SECTION("missing adjacent input")
    {
        auto missing_input_layout =
            cartesian_gate_layout{cartesian_gate_layout::extent{3, 3, 1}, clocking::twoddwave()};
        const auto non_adjacent_pi      = missing_input_layout.create_pi("x", {0, 0});
        const auto missing_input_fanout = missing_input_layout.create_buf(non_adjacent_pi, {1, 1});
        missing_input_layout.create_po(missing_input_fanout, "e", {2, 1});
        missing_input_layout.create_po(missing_input_fanout, "s", {1, 2});
        missing_input_layout.create_po(missing_input_fanout, "w", {0, 1});

        CHECK(missing_input_layout.is_fanout(missing_input_fanout));
        CHECK(missing_input_layout.fanout_size(missing_input_fanout) == 3u);
        CHECK_THROWS_AS((sim7_mol_library::set_up_gate(missing_input_layout, {1, 1})), orientation_exception);
    }
}

TEST_CASE("Setting up majority gate", "[molecular-qca-library]")
{
    auto layout = blueprints::res_maj_gate_layout<cartesian_gate_layout>();

    // clang-format off

    static constexpr const sim7_mol_library::gate majority{
    sim7_mol_library::cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {'a', 'a', 'a', 'b', 'b', 'b', 'b', 'a', 'a', 'a'},
        {'a', 'a', 'a', 'b', 'b', 'b', 'b', 'a', 'a', 'a'},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'c', 'c', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '}
    }})};

    // clang-format on

    CHECK(sim7_mol_library::set_up_gate(layout, {0, 1}) == sim7_mol_library::rotate_90(PRIMARY_INPUT_PORT));
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 0}) == sim7_mol_library::rotate_180(PRIMARY_INPUT_PORT));
    CHECK(sim7_mol_library::set_up_gate(layout, {2, 1}) == sim7_mol_library::rotate_270(PRIMARY_INPUT_PORT));
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 1}) == majority);
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 2}) == PRIMARY_OUTPUT_PORT);
}

TEST_CASE("Setting up and or inv", "[molecular-qca-library]")
{
    auto layout = blueprints::and_or_inv_gate_layout<cartesian_gate_layout>();

    // Expected 10x10 MolQCA gate patterns for each logical tile type used by the blueprint.
    // clang-format off

    // Inverters can appear in straight and bent forms depending on the local tile neighborhood.
    static constexpr const sim7_mol_library::gate straight_inverter{
    sim7_mol_library::cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', 'b', 'b', 'a', 'a', 'b', 'b', ' ', ' '},
        {' ', ' ', 'b', 'b', 'a', 'a', 'b', 'b', ' ', ' '},
        {' ', ' ', 'b', 'b', ' ', ' ', 'b', 'b', ' ', ' '},
        {' ', ' ', ' ', ' ', 'c', 'c', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', 'd', 'c', 'c', 'd', ' ', ' ', ' '},
        {' ', ' ', ' ', 'd', 'd', 'd', 'd', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const sim7_mol_library::gate bent_inverter_r{
    sim7_mol_library::cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', 'd', 'd', 'd', 'd', ' ', ' ', ' '},
        {' ', ' ', ' ', 'd', 'c', 'c', 'd', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'c', 'c', ' ', ' ', ' ', ' '},
        {' ', ' ', 'b', 'b', ' ', ' ', 'b', 'b', ' ', ' '},
        {' ', ' ', 'b', 'b', 'b', 'a', 'a', 'a', 'a', 'a'},
        {' ', ' ', 'b', 'b', 'b', 'a', 'a', 'a', 'a', 'a'},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}
    }})};

    // AND and OR gates are encoded as majority gates with fixed 0 and 1 polarization inputs, respectively.
    static constexpr const sim7_mol_library::gate conjunction{
    sim7_mol_library::cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', '0', '0', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {'d', 'd', 'c', 'b', 'b', 'b', 'b', 'a', 'a', 'a'},
        {'d', 'd', 'c', 'b', 'b', 'b', 'b', 'a', 'a', 'a'},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const sim7_mol_library::gate conjunction_r{
    sim7_mol_library::cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', '0', '0', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {'a', 'a', 'a', 'b', 'b', 'b', 'b', 'c', 'd', 'd'},
        {'a', 'a', 'a', 'b', 'b', 'b', 'b', 'c', 'd', 'd'},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const sim7_mol_library::gate disjunction{
    sim7_mol_library::cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', '1', '1', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {'d', 'd', 'c', 'b', 'b', 'b', 'b', 'a', 'a', 'a'},
        {'d', 'd', 'c', 'b', 'b', 'b', 'b', 'a', 'a', 'a'},
        {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '}
    }})};

    // Fanout and bent-wire templates cover the remaining internal routing cases in this blueprint.
    static constexpr const sim7_mol_library::gate fanout{
    sim7_mol_library::cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {'a', 'a', 'b', 'b', 'c', 'c', 'd', 'd', 'd', 'd'},
        {'a', 'a', 'b', 'b', 'c', 'c', 'd', 'd', 'd', 'd'},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '}
    }})};

    static constexpr const sim7_mol_library::gate bent_wire{
    sim7_mol_library::cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'b', 'b', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', 'b', 'b', 'c', 'c', 'd', 'd'},
        {' ', ' ', ' ', ' ', 'b', 'b', 'c', 'c', 'd', 'd'},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}
    }})};

    // clang-format on

    // Primary I/O ports are matched with the rotation implied by their border orientation.
    CHECK(sim7_mol_library::set_up_gate(layout, {0, 2}) == sim7_mol_library::rotate_90(PRIMARY_INPUT_PORT));
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 0}) == sim7_mol_library::rotate_180(PRIMARY_INPUT_PORT));
    CHECK(sim7_mol_library::set_up_gate(layout, {2, 0}) == sim7_mol_library::rotate_90(PRIMARY_INPUT_PORT));

    // Internal tiles must resolve to the exact gate template and rotation dictated by their port routing.
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 2}) == sim7_mol_library::rotate_270(straight_inverter));
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 1}) == bent_wire);
    CHECK(sim7_mol_library::set_up_gate(layout, {2, 1}) == fanout);
    CHECK(sim7_mol_library::set_up_gate(layout, {3, 0}) == sim7_mol_library::rotate_180(bent_inverter_r));
    CHECK(sim7_mol_library::set_up_gate(layout, {2, 2}) == sim7_mol_library::rotate_180(disjunction));
    CHECK(sim7_mol_library::set_up_gate(layout, {3, 1}) == sim7_mol_library::rotate_90(conjunction_r));
    CHECK(sim7_mol_library::set_up_gate(layout, {3, 2}) == sim7_mol_library::rotate_180(conjunction));
    CHECK(sim7_mol_library::set_up_gate(layout, {4, 2}) == sim7_mol_library::rotate_270(PRIMARY_OUTPUT_PORT));
}

TEST_CASE("Check unsupported gate type", "[molecular-qca-library]")
{
    auto layout = blueprints::row_clocked_and_xor_gate_layout<cartesian_gate_layout>();

    REQUIRE_THROWS_AS(sim7_mol_library::set_up_gate(layout, {1, 2}),
                      unsupported_gate_type_exception<layout_base::coordinate>);
}
