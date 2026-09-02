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
 * @brief Tests for `fiction/technology/qca/sim7_mol_library.hpp`.
 * @author Benjamin Hien (hibenj)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/technology/fcn/cell_ports.hpp>
#include <fiction/technology/fcn/gate_library.hpp>
#include <fiction/technology/qca/sim7_mol_library.hpp>
#include <fiction/technology/qca/technology.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

using namespace fiction;
using namespace fiction::fcn;
using namespace fiction::layouts;
using namespace fiction::qca;

TEST_CASE("Molecular QCA library traits", "[molecular-qca-library]")
{
    CHECK(!has_post_layout_optimization_v<sim7_mol_library, mol_qca_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<sim7_mol_library, qca_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<sim7_mol_library, inml_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<sim7_mol_library, sidb_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<sim7_mol_library, cart_gate_clk_lyt>);
    CHECK(!has_get_functional_implementations_v<sim7_mol_library>);
    CHECK(!has_get_gate_ports_v<sim7_mol_library>);
}

TEST_CASE("Molecular QCA technology helpers", "[molecular-qca-library]")
{
    CHECK(mol_qca_technology::is_empty_cell(mol_qca_technology::cell_type::EMPTY));
    CHECK(mol_qca_technology::is_normal_cell(mol_qca_technology::cell_type::NORMAL1));
    CHECK(mol_qca_technology::is_normal_cell(mol_qca_technology::cell_type::NORMAL2));
    CHECK(mol_qca_technology::is_normal_cell(mol_qca_technology::cell_type::NORMAL3));
    CHECK(mol_qca_technology::is_normal_cell(mol_qca_technology::cell_type::NORMAL4));
    CHECK(!mol_qca_technology::is_normal_cell(mol_qca_technology::cell_type::INPUT));

    CHECK(mol_qca_technology::cell_clock_number(mol_qca_technology::cell_type::NORMAL1) == 0);
    CHECK(mol_qca_technology::cell_clock_number(mol_qca_technology::cell_type::NORMAL2) == 1);
    CHECK(mol_qca_technology::cell_clock_number(mol_qca_technology::cell_type::NORMAL3) == 2);
    CHECK(mol_qca_technology::cell_clock_number(mol_qca_technology::cell_type::NORMAL4) == 3);
    CHECK(mol_qca_technology::cell_clock_number(mol_qca_technology::cell_type::OUTPUT) == 0);

    CHECK(mol_qca_technology::is_input_cell(mol_qca_technology::cell_type::INPUT));
    CHECK(mol_qca_technology::is_output_cell(mol_qca_technology::cell_type::OUTPUT));
    CHECK(mol_qca_technology::is_const_0_cell(mol_qca_technology::cell_type::CONST_0));
    CHECK(mol_qca_technology::is_const_1_cell(mol_qca_technology::cell_type::CONST_1));
    CHECK(mol_qca_technology::is_constant_cell(mol_qca_technology::cell_type::CONST_0));
    CHECK(mol_qca_technology::is_constant_cell(mol_qca_technology::cell_type::CONST_1));

    CHECK(mol_qca_technology::is_normal_cell_mode(mol_qca_technology::cell_mode::NORMAL));
    CHECK(mol_qca_technology::is_rotated_cell_mode(mol_qca_technology::cell_mode::ROTATED));
    CHECK(mol_qca_technology::is_vertical_cell_mode(mol_qca_technology::cell_mode::VERTICAL));
    CHECK(mol_qca_technology::is_crossover_cell_mode(mol_qca_technology::cell_mode::CROSSOVER));
}

TEST_CASE("Setting up input ports and gates", "[molecular-qca-library]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;

    auto layout = blueprints::or_not_gate_layout<gate_layout>();

    // clang-format off

    static constexpr const sim7_mol_library::gate primary_input_port{
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

    static constexpr const sim7_mol_library::gate primary_output_port{
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

    CHECK(sim7_mol_library::set_up_gate(layout, {0, 1}) == sim7_mol_library::rotate_90(primary_input_port));
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 0}) == sim7_mol_library::rotate_180(primary_input_port));
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 1}) == sim7_mol_library::rotate_90(disjunction));
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 2}) == bent_inverter);
    CHECK(sim7_mol_library::set_up_gate(layout, {2, 2}) == sim7_mol_library::rotate_270(primary_output_port));
}

TEST_CASE("Setting up wires", "[molecular-qca-library]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;

    auto layout = blueprints::three_wire_paths_gate_layout<gate_layout>();

    // clang-format off

    static constexpr const sim7_mol_library::gate primary_input_port{
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

    static constexpr const sim7_mol_library::gate primary_output_port{
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

    CHECK(sim7_mol_library::set_up_gate(layout, {0, 0}) == sim7_mol_library::rotate_90(primary_input_port));
    CHECK(sim7_mol_library::set_up_gate(layout, {0, 2}) == sim7_mol_library::rotate_90(primary_input_port));
    CHECK(sim7_mol_library::set_up_gate(layout, {0, 4}) == sim7_mol_library::rotate_90(primary_input_port));
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 0}) == sim7_mol_library::rotate_270(wire));
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 2}) == sim7_mol_library::rotate_270(wire));
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 4}) == sim7_mol_library::rotate_270(wire));
    CHECK(sim7_mol_library::set_up_gate(layout, {2, 0}) == sim7_mol_library::rotate_270(wire));
    CHECK(sim7_mol_library::set_up_gate(layout, {2, 2}) == sim7_mol_library::rotate_270(wire));
    CHECK(sim7_mol_library::set_up_gate(layout, {2, 4}) == sim7_mol_library::rotate_270(wire));
    CHECK(sim7_mol_library::set_up_gate(layout, {3, 0}) == sim7_mol_library::rotate_270(wire));
    CHECK(sim7_mol_library::set_up_gate(layout, {3, 2}) == sim7_mol_library::rotate_270(wire));
    CHECK(sim7_mol_library::set_up_gate(layout, {3, 4}) == sim7_mol_library::rotate_270(wire));
    CHECK(sim7_mol_library::set_up_gate(layout, {4, 0}) == sim7_mol_library::rotate_270(primary_output_port));
    CHECK(sim7_mol_library::set_up_gate(layout, {4, 2}) == sim7_mol_library::rotate_270(primary_output_port));
    CHECK(sim7_mol_library::set_up_gate(layout, {4, 4}) == sim7_mol_library::rotate_270(primary_output_port));
}

TEST_CASE("Setting up fanouts", "[molecular-qca-library]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;

    auto layout = blueprints::fanout_layout<gate_layout>();

    // clang-format off

    static constexpr const sim7_mol_library::gate primary_input_port{
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

    static constexpr const sim7_mol_library::gate primary_output_port{
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

    CHECK(sim7_mol_library::set_up_gate(layout, {0, 1}) == sim7_mol_library::rotate_90(primary_input_port));
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 0}) == sim7_mol_library::rotate_180(primary_output_port));
    CHECK(sim7_mol_library::set_up_gate(layout, {2, 0}) == sim7_mol_library::rotate_180(primary_output_port));
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 2}) == sim7_mol_library::rotate_90(primary_output_port));
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 1}) == sim7_mol_library::rotate_180(fanout));
    CHECK(sim7_mol_library::set_up_gate(layout, {2, 1}) == sim7_mol_library::rotate_90(fanout_d));
    CHECK(sim7_mol_library::set_up_gate(layout, {2, 2}) == sim7_mol_library::rotate_270(bent_wire));
}

TEST_CASE("Setting up fanout-3 rotations", "[molecular-qca-library]")
{
    using gate_layout    = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;
    using clock_number_t = gate_layout::clock_number_t;
    using orientation_exception = unsupported_gate_orientation_exception<coords::offset, port_position>;

    static constexpr auto input_clock  = static_cast<clock_number_t>(0);
    static constexpr auto fanout_clock = static_cast<clock_number_t>(1);
    static constexpr auto output_clock = static_cast<clock_number_t>(2);

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

    auto northern_input_layout = gate_layout{gate_layout::aspect_ratio{2, 2, 0}};
    northern_input_layout.assign_clock_number({1, 0}, input_clock);
    northern_input_layout.assign_clock_number({1, 1}, fanout_clock);
    northern_input_layout.assign_clock_number({2, 1}, output_clock);
    northern_input_layout.assign_clock_number({1, 2}, output_clock);
    northern_input_layout.assign_clock_number({0, 1}, output_clock);
    const auto north_pi     = northern_input_layout.create_pi("x", {1, 0});
    const auto north_fanout = northern_input_layout.create_buf(north_pi, {1, 1});
    northern_input_layout.create_po(north_fanout, "e", {2, 1});
    northern_input_layout.create_po(north_fanout, "s", {1, 2});
    northern_input_layout.create_po(north_fanout, "w", {0, 1});

    auto eastern_input_layout = gate_layout{gate_layout::aspect_ratio{2, 2, 0}};
    eastern_input_layout.assign_clock_number({2, 1}, input_clock);
    eastern_input_layout.assign_clock_number({1, 1}, fanout_clock);
    eastern_input_layout.assign_clock_number({1, 2}, output_clock);
    eastern_input_layout.assign_clock_number({0, 1}, output_clock);
    eastern_input_layout.assign_clock_number({1, 0}, output_clock);
    const auto east_pi     = eastern_input_layout.create_pi("x", {2, 1});
    const auto east_fanout = eastern_input_layout.create_buf(east_pi, {1, 1});
    eastern_input_layout.create_po(east_fanout, "s", {1, 2});
    eastern_input_layout.create_po(east_fanout, "w", {0, 1});
    eastern_input_layout.create_po(east_fanout, "n", {1, 0});

    auto southern_input_layout = gate_layout{gate_layout::aspect_ratio{2, 2, 0}};
    southern_input_layout.assign_clock_number({1, 2}, input_clock);
    southern_input_layout.assign_clock_number({1, 1}, fanout_clock);
    southern_input_layout.assign_clock_number({0, 1}, output_clock);
    southern_input_layout.assign_clock_number({1, 0}, output_clock);
    southern_input_layout.assign_clock_number({2, 1}, output_clock);
    const auto south_pi     = southern_input_layout.create_pi("x", {1, 2});
    const auto south_fanout = southern_input_layout.create_buf(south_pi, {1, 1});
    southern_input_layout.create_po(south_fanout, "w", {0, 1});
    southern_input_layout.create_po(south_fanout, "n", {1, 0});
    southern_input_layout.create_po(south_fanout, "e", {2, 1});

    auto western_input_layout = gate_layout{gate_layout::aspect_ratio{2, 2, 0}};
    western_input_layout.assign_clock_number({0, 1}, input_clock);
    western_input_layout.assign_clock_number({1, 1}, fanout_clock);
    western_input_layout.assign_clock_number({1, 0}, output_clock);
    western_input_layout.assign_clock_number({2, 1}, output_clock);
    western_input_layout.assign_clock_number({1, 2}, output_clock);
    const auto west_pi     = western_input_layout.create_pi("x", {0, 1});
    const auto west_fanout = western_input_layout.create_buf(west_pi, {1, 1});
    western_input_layout.create_po(west_fanout, "n", {1, 0});
    western_input_layout.create_po(west_fanout, "e", {2, 1});
    western_input_layout.create_po(west_fanout, "s", {1, 2});

    CHECK(sim7_mol_library::set_up_gate(northern_input_layout, {1, 1}) == fanout_1_3);
    CHECK(sim7_mol_library::set_up_gate(eastern_input_layout, {1, 1}) == sim7_mol_library::rotate_90(fanout_1_3));
    CHECK(sim7_mol_library::set_up_gate(southern_input_layout, {1, 1}) == sim7_mol_library::rotate_180(fanout_1_3));
    CHECK(sim7_mol_library::set_up_gate(western_input_layout, {1, 1}) == sim7_mol_library::rotate_270(fanout_1_3));

    auto       clocked_layout = gate_layout{gate_layout::aspect_ratio{2, 2, 0}, clocking::twoddwave<gate_layout>()};
    const auto clocked_pi     = clocked_layout.create_pi("x", {1, 0});
    const auto clocked_fanout = clocked_layout.create_buf(clocked_pi, {1, 1});
    clocked_layout.create_po(clocked_fanout, "e", {2, 1});
    clocked_layout.create_po(clocked_fanout, "s", {1, 2});
    clocked_layout.create_po(clocked_fanout, "w", {0, 1});

    const auto clocked_fanout_node = clocked_layout.get_node({1, 1});
    CHECK(clocked_layout.is_fanout(clocked_fanout_node));
    CHECK(clocked_layout.fanout_size(clocked_fanout_node) == 2u);
    CHECK(clocked_layout.template fanout_size<false>(clocked_fanout_node) == 3u);
    CHECK(sim7_mol_library::set_up_gate(clocked_layout, {1, 1}) == fanout_1_3);

    auto missing_input_layout  = gate_layout{gate_layout::aspect_ratio{2, 2, 0}, clocking::twoddwave<gate_layout>()};
    const auto non_adjacent_pi = missing_input_layout.create_pi("x", {0, 0});
    const auto missing_input_fanout = missing_input_layout.create_buf(non_adjacent_pi, {1, 1});
    missing_input_layout.create_po(missing_input_fanout, "e", {2, 1});
    missing_input_layout.create_po(missing_input_fanout, "s", {1, 2});
    missing_input_layout.create_po(missing_input_fanout, "w", {0, 1});

    const auto missing_input_fanout_node = missing_input_layout.get_node({1, 1});
    CHECK(missing_input_layout.is_fanout(missing_input_fanout_node));
    CHECK(missing_input_layout.template fanout_size<false>(missing_input_fanout_node) == 3u);
    CHECK_THROWS_AS((sim7_mol_library::set_up_gate(missing_input_layout, {1, 1})), orientation_exception);
}

TEST_CASE("Setting up majority gate", "[molecular-qca-library]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;

    auto layout = blueprints::res_maj_gate_layout<gate_layout>();

    // clang-format off

    static constexpr const sim7_mol_library::gate primary_input_port{
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

    static constexpr const sim7_mol_library::gate primary_output_port{
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

    CHECK(sim7_mol_library::set_up_gate(layout, {0, 1}) == sim7_mol_library::rotate_90(primary_input_port));
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 0}) == sim7_mol_library::rotate_180(primary_input_port));
    CHECK(sim7_mol_library::set_up_gate(layout, {2, 1}) == sim7_mol_library::rotate_270(primary_input_port));
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 1}) == majority);
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 2}) == primary_output_port);
}

TEST_CASE("Setting up and or inv", "[molecular-qca-library]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;

    auto layout = blueprints::and_or_inv_gate_layout<gate_layout>();

    // Expected 10x10 MolQCA gate patterns for each logical tile type used by the blueprint.
    // clang-format off

    // Boundary tiles are represented by dedicated primary input and output port templates.
    static constexpr const sim7_mol_library::gate primary_input_port{
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

    static constexpr const sim7_mol_library::gate primary_output_port{
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
    CHECK(sim7_mol_library::set_up_gate(layout, {0, 2}) == sim7_mol_library::rotate_90(primary_input_port));
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 0}) == sim7_mol_library::rotate_180(primary_input_port));
    CHECK(sim7_mol_library::set_up_gate(layout, {2, 0}) == sim7_mol_library::rotate_90(primary_input_port));

    // Internal tiles must resolve to the exact gate template and rotation dictated by their port routing.
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 2}) == sim7_mol_library::rotate_270(straight_inverter));
    CHECK(sim7_mol_library::set_up_gate(layout, {1, 1}) == bent_wire);
    CHECK(sim7_mol_library::set_up_gate(layout, {2, 1}) == fanout);
    CHECK(sim7_mol_library::set_up_gate(layout, {3, 0}) == sim7_mol_library::rotate_180(bent_inverter_r));
    CHECK(sim7_mol_library::set_up_gate(layout, {2, 2}) == sim7_mol_library::rotate_180(disjunction));
    CHECK(sim7_mol_library::set_up_gate(layout, {3, 1}) == sim7_mol_library::rotate_90(conjunction_r));
    CHECK(sim7_mol_library::set_up_gate(layout, {3, 2}) == sim7_mol_library::rotate_180(conjunction));
    CHECK(sim7_mol_library::set_up_gate(layout, {4, 2}) == sim7_mol_library::rotate_270(primary_output_port));
}

TEST_CASE("Check unsupported gate type", "[molecular-qca-library]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<coords::offset>>>>;

    auto layout = blueprints::row_clocked_and_xor_gate_layout<gate_layout>();

    REQUIRE_THROWS_AS(sim7_mol_library::set_up_gate(layout, {1, 2}), unsupported_gate_type_exception<coords::offset>);
}
