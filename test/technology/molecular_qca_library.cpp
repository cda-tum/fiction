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
#include <fiction/technology/molecular_qca_library.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEST_CASE("QCA ONE library traits", "[molecular-qca-library]")
{
    CHECK(has_post_layout_optimization_v<molecular_qca_library, mol_qca_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<molecular_qca_library, qca_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<molecular_qca_library, inml_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<molecular_qca_library, sidb_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<molecular_qca_library, cart_gate_clk_lyt>);
    CHECK(!has_get_functional_implementations_v<molecular_qca_library>);
    CHECK(!has_get_gate_ports_v<molecular_qca_library>);
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

    // clang-format on

    CHECK(molecular_qca_library::set_up_gate(layout, {0, 1}) == molecular_qca_library::rotate_90(primary_input_port));
    CHECK(molecular_qca_library::set_up_gate(layout, {1, 0}) == molecular_qca_library::rotate_180(primary_input_port));
    CHECK(molecular_qca_library::set_up_gate(layout, {1, 1}) == molecular_qca_library::rotate_90(disjunction));
    CHECK(molecular_qca_library::set_up_gate(layout, {2, 2}) == molecular_qca_library::rotate_270(primary_output_port));
}

TEST_CASE("Setting up wires", "[molecular-qca-library]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto layout = blueprints::three_wire_paths_gate_layout<gate_layout>();

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

    static constexpr const molecular_qca_library::fcn_gate wire{
        molecular_qca_library::cell_list_to_gate<char>(
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
