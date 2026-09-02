//
// Created by marcel on 17.01.22.
//

#include <catch2/catch_test_macros.hpp>

#include "fiction/layouts/io/print_layout.hpp"
#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/technology/inml/topolinano_library.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEST_CASE("ToPoliNano library traits", "[inml-topolinano-library]")
{
    CHECK(!has_post_layout_optimization_v<inml::topolinano_library, qca_cell_clk_lyt>);
    CHECK(has_post_layout_optimization_v<inml::topolinano_library, inml_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<inml::topolinano_library, sidb_cell_clk_lyt>);
    CHECK(!has_post_layout_optimization_v<inml::topolinano_library, cart_gate_clk_lyt>);
    CHECK(!has_get_functional_implementations_v<inml::topolinano_library>);
    CHECK(!has_get_gate_ports_v<inml::topolinano_library>);
}

TEST_CASE("Setting up input ports, gates, and wires", "[inml-topolinano-library]")
{
    const auto layout = blueprints::shifted_cart_and_or_inv_gate_layout<fiction::cart_odd_col_gate_clk_lyt>();

    layouts::io::print_gate_level_layout(std::cout, layout);

    // clang-format off

    static constexpr const inml::topolinano_library::gate lower_pi{
        inml::topolinano_library::cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {'i', 'x', 'x', 'x'},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const inml::topolinano_library::gate upper_pi{
        inml::topolinano_library::cell_list_to_gate<char>(
    {{
        {'i', 'x', 'x', 'x'},
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const inml::topolinano_library::gate conjunction{
        inml::topolinano_library::cell_list_to_gate<char>(
    {{
        {'d', ' ', ' ', ' '},
        {'d', 'x', 'x', 'x'},
        {'d', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const inml::topolinano_library::gate disjunction{
        inml::topolinano_library::cell_list_to_gate<char>(
    {{
        {'u', ' ', ' ', ' '},
        {'u', 'x', 'x', 'x'},
        {'u', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const inml::topolinano_library::gate bottom_up_bent_wire{
        inml::topolinano_library::cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', 'x'},
        {'x', 'x', 'x', 'x'},
        {'x', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const inml::topolinano_library::gate bottom_up_bent_inverter{
        inml::topolinano_library::cell_list_to_gate<char>(
    {{
        {'n', 'n', 'n', 'n'},
        {'x', ' ', ' ', ' '},
        {'x', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '}
    }})};

    static constexpr const inml::topolinano_library::gate lower_wire{
        inml::topolinano_library::cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {'x', 'x', 'x', 'x'},
        {'x', ' ', ' ', ' '}
    }})};

    static constexpr const inml::topolinano_library::gate lower_po{
        inml::topolinano_library::cell_list_to_gate<char>(
    {{
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' '},
        {'x', 'x', 'x', 'o'}
    }})};

    // clang-format on

    CHECK(inml::topolinano_library::set_up_gate(layout, {0, 0}) == lower_pi);
    CHECK(inml::topolinano_library::set_up_gate(layout, {0, 1}) == upper_pi);
    CHECK(inml::topolinano_library::set_up_gate(layout, {0, 2}) == upper_pi);

    CHECK(inml::topolinano_library::set_up_gate(layout, {1, 0}) == conjunction);
    CHECK(inml::topolinano_library::set_up_gate(layout, {1, 1}) == bottom_up_bent_wire);
    CHECK(inml::topolinano_library::set_up_gate(layout, {2, 0}) == lower_wire);
    CHECK(inml::topolinano_library::set_up_gate(layout, {2, 1}) == bottom_up_bent_inverter);
    CHECK(inml::topolinano_library::set_up_gate(layout, {3, 0}) == disjunction);

    CHECK(inml::topolinano_library::set_up_gate(layout, {4, 0}) == lower_po);
}
