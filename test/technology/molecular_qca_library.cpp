//
// Created by benjamin on 15.07.25.
//

#include <catch2/catch_test_macros.hpp>

#include "../utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/molecular_qca_library.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <exception>

using namespace fiction;

TEST_CASE("Molecular QCA library traits", "[molecular-qca-library]")
{
    CHECK(!has_post_layout_optimization_v<molecular_qca_library, mol_qca_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<molecular_qca_library, qca_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<molecular_qca_library, inml_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<molecular_qca_library, sidb_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<molecular_qca_library, cart_gate_clk_lyt>);
    CHECK(!has_get_functional_implementations_v<molecular_qca_library>);
    CHECK(!has_get_gate_ports_v<molecular_qca_library>);
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
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto layout = blueprints::or_not_gate_layout<gate_layout>();

    // clang-format off

    static constexpr const molecular_qca_library::fcn_gate primary_input_port{
    molecular_qca_library::cell_list_to_gate<char>(
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

    static constexpr const molecular_qca_library::fcn_gate primary_output_port{
    molecular_qca_library::cell_list_to_gate<char>(
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

    static constexpr const molecular_qca_library::fcn_gate disjunction{
    molecular_qca_library::cell_list_to_gate<char>(
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

    static constexpr const molecular_qca_library::fcn_gate bent_inverter{
    molecular_qca_library::cell_list_to_gate<char>(
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

    CHECK(molecular_qca_library::set_up_gate(layout, {0, 1}) == molecular_qca_library::rotate_90(primary_input_port));
    CHECK(molecular_qca_library::set_up_gate(layout, {1, 0}) == molecular_qca_library::rotate_180(primary_input_port));
    CHECK(molecular_qca_library::set_up_gate(layout, {1, 1}) == molecular_qca_library::rotate_90(disjunction));
    CHECK(molecular_qca_library::set_up_gate(layout, {1, 2}) == bent_inverter);
    CHECK(molecular_qca_library::set_up_gate(layout, {2, 2}) == molecular_qca_library::rotate_270(primary_output_port));
}

TEST_CASE("Setting up wires", "[molecular-qca-library]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto layout = blueprints::three_wire_paths_gate_layout<gate_layout>();

    static constexpr const molecular_qca_library::fcn_gate primary_input_port{
        molecular_qca_library::cell_list_to_gate<char>({{{' ', ' ', ' ', ' ', 'i', 'i', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}}})};

    static constexpr const molecular_qca_library::fcn_gate primary_output_port{
        molecular_qca_library::cell_list_to_gate<char>({{{' ', ' ', ' ', ' ', 'o', 'o', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}}})};

    static constexpr const molecular_qca_library::fcn_gate wire{
        molecular_qca_library::cell_list_to_gate<char>({{{' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', 'b', 'b', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', 'b', 'b', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', 'c', 'c', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', 'c', 'c', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
                                                         {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '}}})};

    // clang-format on

    CHECK(molecular_qca_library::set_up_gate(layout, {0, 0}) == molecular_qca_library::rotate_90(primary_input_port));
    CHECK(molecular_qca_library::set_up_gate(layout, {0, 2}) == molecular_qca_library::rotate_90(primary_input_port));
    CHECK(molecular_qca_library::set_up_gate(layout, {0, 4}) == molecular_qca_library::rotate_90(primary_input_port));
    CHECK(molecular_qca_library::set_up_gate(layout, {1, 0}) == molecular_qca_library::rotate_270(wire));
    CHECK(molecular_qca_library::set_up_gate(layout, {1, 2}) == molecular_qca_library::rotate_270(wire));
    CHECK(molecular_qca_library::set_up_gate(layout, {1, 4}) == molecular_qca_library::rotate_270(wire));
    CHECK(molecular_qca_library::set_up_gate(layout, {2, 0}) == molecular_qca_library::rotate_270(wire));
    CHECK(molecular_qca_library::set_up_gate(layout, {2, 2}) == molecular_qca_library::rotate_270(wire));
    CHECK(molecular_qca_library::set_up_gate(layout, {2, 4}) == molecular_qca_library::rotate_270(wire));
    CHECK(molecular_qca_library::set_up_gate(layout, {3, 0}) == molecular_qca_library::rotate_270(wire));
    CHECK(molecular_qca_library::set_up_gate(layout, {3, 2}) == molecular_qca_library::rotate_270(wire));
    CHECK(molecular_qca_library::set_up_gate(layout, {3, 4}) == molecular_qca_library::rotate_270(wire));
    CHECK(molecular_qca_library::set_up_gate(layout, {4, 0}) == molecular_qca_library::rotate_270(primary_output_port));
    CHECK(molecular_qca_library::set_up_gate(layout, {4, 2}) == molecular_qca_library::rotate_270(primary_output_port));
    CHECK(molecular_qca_library::set_up_gate(layout, {4, 4}) == molecular_qca_library::rotate_270(primary_output_port));
}

TEST_CASE("Setting up fanouts", "[molecular-qca-library]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto layout = blueprints::fanout_layout<gate_layout>();

    // clang-format off

    static constexpr const molecular_qca_library::fcn_gate primary_input_port{
    molecular_qca_library::cell_list_to_gate<char>(
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

    static constexpr const molecular_qca_library::fcn_gate primary_output_port{
    molecular_qca_library::cell_list_to_gate<char>(
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

    static constexpr const molecular_qca_library::fcn_gate fanout{
    molecular_qca_library::cell_list_to_gate<char>(
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

    static constexpr const molecular_qca_library::fcn_gate fanout_d{
    molecular_qca_library::cell_list_to_gate<char>(
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

    static constexpr const molecular_qca_library::fcn_gate bent_wire{
    molecular_qca_library::cell_list_to_gate<char>(
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

    CHECK(molecular_qca_library::set_up_gate(layout, {0, 1}) == molecular_qca_library::rotate_90(primary_input_port));
    CHECK(molecular_qca_library::set_up_gate(layout, {1, 0}) == molecular_qca_library::rotate_180(primary_output_port));
    CHECK(molecular_qca_library::set_up_gate(layout, {2, 0}) == molecular_qca_library::rotate_180(primary_output_port));
    CHECK(molecular_qca_library::set_up_gate(layout, {1, 2}) == molecular_qca_library::rotate_90(primary_output_port));
    CHECK(molecular_qca_library::set_up_gate(layout, {1, 1}) == molecular_qca_library::rotate_180(fanout));
    CHECK(molecular_qca_library::set_up_gate(layout, {2, 1}) == molecular_qca_library::rotate_90(fanout_d));
    CHECK(molecular_qca_library::set_up_gate(layout, {2, 2}) == molecular_qca_library::rotate_270(bent_wire));
}

TEST_CASE("Setting up fanout-3 rotations", "[molecular-qca-library]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    static constexpr const molecular_qca_library::fcn_gate fanout_1_3{
        molecular_qca_library::cell_list_to_gate<char>(
            {{{' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
              {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
              {' ', ' ', ' ', ' ', 'a', 'a', ' ', ' ', ' ', ' '},
              {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
              {'d', 'd', 'c', 'b', 'b', 'b', 'b', 'c', 'd', 'd'},
              {'d', 'd', 'c', 'b', 'b', 'b', 'b', 'c', 'd', 'd'},
              {' ', ' ', ' ', 'b', 'b', 'b', 'b', ' ', ' ', ' '},
              {' ', ' ', ' ', ' ', 'c', 'c', ' ', ' ', ' ', ' '},
              {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '},
              {' ', ' ', ' ', ' ', 'd', 'd', ' ', ' ', ' ', ' '}}})};

    auto northern_input_layout = gate_layout{gate_layout::aspect_ratio{2, 2, 0}};
    const auto north_pi        = northern_input_layout.create_pi("x", {1, 0});
    const auto north_fanout    = northern_input_layout.create_buf(north_pi, {1, 1});
    northern_input_layout.create_po(north_fanout, "e", {2, 1});
    northern_input_layout.create_po(north_fanout, "s", {1, 2});
    northern_input_layout.create_po(north_fanout, "w", {0, 1});

    auto eastern_input_layout = gate_layout{gate_layout::aspect_ratio{2, 2, 0}};
    const auto east_pi        = eastern_input_layout.create_pi("x", {2, 1});
    const auto east_fanout    = eastern_input_layout.create_buf(east_pi, {1, 1});
    eastern_input_layout.create_po(east_fanout, "s", {1, 2});
    eastern_input_layout.create_po(east_fanout, "w", {0, 1});
    eastern_input_layout.create_po(east_fanout, "n", {1, 0});

    auto southern_input_layout = gate_layout{gate_layout::aspect_ratio{2, 2, 0}};
    const auto south_pi        = southern_input_layout.create_pi("x", {1, 2});
    const auto south_fanout    = southern_input_layout.create_buf(south_pi, {1, 1});
    southern_input_layout.create_po(south_fanout, "w", {0, 1});
    southern_input_layout.create_po(south_fanout, "n", {1, 0});
    southern_input_layout.create_po(south_fanout, "e", {2, 1});

    auto western_input_layout = gate_layout{gate_layout::aspect_ratio{2, 2, 0}};
    const auto west_pi        = western_input_layout.create_pi("x", {0, 1});
    const auto west_fanout    = western_input_layout.create_buf(west_pi, {1, 1});
    western_input_layout.create_po(west_fanout, "n", {1, 0});
    western_input_layout.create_po(west_fanout, "e", {2, 1});
    western_input_layout.create_po(west_fanout, "s", {1, 2});

    CHECK(molecular_qca_library::set_up_gate(northern_input_layout, {1, 1}) == fanout_1_3);
    CHECK(molecular_qca_library::set_up_gate(eastern_input_layout, {1, 1}) ==
          molecular_qca_library::rotate_90(fanout_1_3));
    CHECK(molecular_qca_library::set_up_gate(southern_input_layout, {1, 1}) ==
          molecular_qca_library::rotate_180(fanout_1_3));
    CHECK(molecular_qca_library::set_up_gate(western_input_layout, {1, 1}) ==
          molecular_qca_library::rotate_270(fanout_1_3));
}

TEST_CASE("Setting up majority gate", "[molecular-qca-library]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto layout = blueprints::res_maj_gate_layout<gate_layout>();

    // clang-format off

    static constexpr const molecular_qca_library::fcn_gate primary_input_port{
    molecular_qca_library::cell_list_to_gate<char>(
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

    static constexpr const molecular_qca_library::fcn_gate primary_output_port{
    molecular_qca_library::cell_list_to_gate<char>(
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

    static constexpr const molecular_qca_library::fcn_gate majority{
    molecular_qca_library::cell_list_to_gate<char>(
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

    CHECK(molecular_qca_library::set_up_gate(layout, {0, 1}) == molecular_qca_library::rotate_90(primary_input_port));
    CHECK(molecular_qca_library::set_up_gate(layout, {1, 0}) == molecular_qca_library::rotate_180(primary_input_port));
    CHECK(molecular_qca_library::set_up_gate(layout, {2, 1}) == molecular_qca_library::rotate_270(primary_input_port));
    CHECK(molecular_qca_library::set_up_gate(layout, {1, 1}) == majority);
    CHECK(molecular_qca_library::set_up_gate(layout, {1, 2}) == primary_output_port);
}

TEST_CASE("Setting up and or inv", "[molecular-qca-library]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto layout = blueprints::and_or_inv_gate_layout<gate_layout>();

    // Expected 10x10 MolQCA gate patterns for each logical tile type used by the blueprint.
    // clang-format off

    // Boundary tiles are represented by dedicated primary input and output port templates.
    static constexpr const molecular_qca_library::fcn_gate primary_input_port{
    molecular_qca_library::cell_list_to_gate<char>(
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

    static constexpr const molecular_qca_library::fcn_gate primary_output_port{
    molecular_qca_library::cell_list_to_gate<char>(
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
    static constexpr const molecular_qca_library::fcn_gate straight_inverter{
    molecular_qca_library::cell_list_to_gate<char>(
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

    static constexpr const molecular_qca_library::fcn_gate bent_inverter{
    molecular_qca_library::cell_list_to_gate<char>(
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
    static constexpr const molecular_qca_library::fcn_gate conjunction{
    molecular_qca_library::cell_list_to_gate<char>(
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

    static constexpr const molecular_qca_library::fcn_gate conjunction_r{
    molecular_qca_library::cell_list_to_gate<char>(
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

    static constexpr const molecular_qca_library::fcn_gate disjunction{
    molecular_qca_library::cell_list_to_gate<char>(
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
    static constexpr const molecular_qca_library::fcn_gate fanout{
    molecular_qca_library::cell_list_to_gate<char>(
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

    static constexpr const molecular_qca_library::fcn_gate bent_wire{
    molecular_qca_library::cell_list_to_gate<char>(
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
    CHECK(molecular_qca_library::set_up_gate(layout, {0, 2}) == molecular_qca_library::rotate_90(primary_input_port));
    CHECK(molecular_qca_library::set_up_gate(layout, {1, 0}) == molecular_qca_library::rotate_180(primary_input_port));
    CHECK(molecular_qca_library::set_up_gate(layout, {2, 0}) == molecular_qca_library::rotate_90(primary_input_port));

    // Internal tiles must resolve to the exact gate template and rotation dictated by their port routing.
    CHECK(molecular_qca_library::set_up_gate(layout, {1, 2}) == molecular_qca_library::rotate_270(straight_inverter));
    CHECK(molecular_qca_library::set_up_gate(layout, {1, 1}) == bent_wire);
    CHECK(molecular_qca_library::set_up_gate(layout, {2, 1}) == fanout);
    CHECK(molecular_qca_library::set_up_gate(layout, {3, 0}) == molecular_qca_library::rotate_180(bent_inverter));
    CHECK(molecular_qca_library::set_up_gate(layout, {2, 2}) == molecular_qca_library::rotate_180(disjunction));
    CHECK(molecular_qca_library::set_up_gate(layout, {3, 1}) == molecular_qca_library::rotate_90(conjunction_r));
    CHECK(molecular_qca_library::set_up_gate(layout, {3, 2}) == molecular_qca_library::rotate_180(conjunction));
    CHECK(molecular_qca_library::set_up_gate(layout, {4, 2}) == molecular_qca_library::rotate_270(primary_output_port));
}

TEST_CASE("Check unsupported gate type", "[molecular-qca-library]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto layout = blueprints::row_clocked_and_xor_gate_layout<gate_layout>();

    REQUIRE_THROWS_AS(molecular_qca_library::set_up_gate(layout, {1, 2}), std::exception);
}
