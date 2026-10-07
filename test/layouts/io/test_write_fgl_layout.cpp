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
 * @brief Tests for `fiction/layouts/io/write_fgl_layout.hpp`.
 * @author Simon Hofmann (simon1hofmann)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/layouts/arrangement.hpp>
#include <fiction/layouts/bounding_box.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/io/read_fgl_layout.hpp>
#include <fiction/layouts/io/write_fgl_layout.hpp>
#include <fiction/layouts/layout_base.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/physical_design/orthogonal.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/networks/aig.hpp>

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::layouts::io;
using namespace fiction::networks;
using namespace fiction::physical_design;

/**
 * @brief Compares serialized placement, functions, ports, and interfaces.
 * @tparam WLyt Written layout type.
 * @tparam RLyt Restored layout type.
 * @param wlyt Written layout.
 * @param rlyt Restored layout.
 */
template <typename WLyt, typename RLyt>
void compare_written_and_read_layout(const WLyt& wlyt, const RLyt& rlyt)
{
    CHECK(wlyt.get_layout_name() == rlyt.get_layout_name());

    const bounding_box_2d<WLyt> wbb{wlyt};
    const bounding_box_2d<RLyt> rbb{rlyt};

    CHECK(wbb.get_min() == rbb.get_min());
    CHECK(wbb.get_max() == rbb.get_max());

    CHECK(wbb.get_x_size() == rbb.get_x_size());
    CHECK(wbb.get_y_size() == rbb.get_y_size());

    CHECK(wlyt.num_pis() == rlyt.num_pis());
    CHECK(wlyt.num_pos() == rlyt.num_pos());
    CHECK(wlyt.width() == rlyt.width());
    CHECK(wlyt.height() == rlyt.height());
    CHECK(wlyt.layers() == rlyt.layers());
    CHECK(wlyt.size() == rlyt.size());
    for (uint32_t i = 0; i < wlyt.num_pis(); ++i)
    {
        CHECK(wlyt.get_input_name(i) == rlyt.get_input_name(i));
    }
    for (uint32_t i = 0; i < wlyt.num_pos(); ++i)
    {
        CHECK(wlyt.get_output_name(i) == rlyt.get_output_name(i));
    }
    wlyt.foreach_node(
        [&](const auto id)
        {
            const auto restored = rlyt.find_object(wlyt.get_tile(id));
            REQUIRE(restored.has_value());
            if (!restored.has_value())
            {
                return;
            }
            CHECK(wlyt.get_name(id) == rlyt.get_name(*restored));
            CHECK(wlyt.is_pi(id) == rlyt.is_pi(*restored));
            CHECK(wlyt.is_po(id) == rlyt.is_po(*restored));
            CHECK(wlyt.is_buf(id) == rlyt.is_buf(*restored));
            CHECK(wlyt.input_count(id) == rlyt.input_count(*restored));
            CHECK(wlyt.node_function(id) == rlyt.node_function(*restored));
            for (uint32_t i = 0; i < wlyt.input_count(id); ++i)
            {
                /** @brief Input connection in the written layout. */
                const auto written_source = wlyt.source({id, i});
                /** @brief Corresponding connection in the restored layout. */
                const auto restored_source = rlyt.source({*restored, i});
                CHECK((written_source.has_value() && restored_source.has_value() &&
                       wlyt.get_tile(*written_source) == rlyt.get_tile(*restored_source)));
            }
        });
}

/** @brief Round-trip a physically designed network. */
template <typename Lyt, typename Ntk>
void check_parsing_equiv(const Ntk& ntk)
{
    const auto layout = orthogonal<Lyt>(ntk, {});

    std::stringstream layout_stream{};
    write_fgl_layout(layout, layout_stream);

    const auto read_layout = read_fgl_layout<Lyt>(layout_stream, layout.get_layout_name());

    compare_written_and_read_layout(layout, read_layout);

    check_eq(ntk, layout);
    check_eq(ntk, read_layout);
    check_eq(layout, read_layout);
}

/** @brief Round-trip a finished layout. */
template <typename Lyt>
void check_parsing_equiv_layout(const Lyt& lyt)
{
    std::stringstream layout_stream{};
    write_fgl_layout(lyt, layout_stream);
    const auto read_layout = read_fgl_layout<Lyt>(layout_stream, lyt.get_layout_name());

    compare_written_and_read_layout(lyt, read_layout);

    if constexpr (!is_cartesian_layout_v<Lyt>)
    {
        CHECK(read_layout.get_arrangement() == lyt.get_arrangement());
    }

    check_eq(lyt, read_layout);
    CHECK(lyt.get_layout_name() == read_layout.get_layout_name());
}

/** @brief Check the network round-trip fixtures. */
template <typename Lyt>
void check_parsing_equiv_all()
{
    check_parsing_equiv<Lyt>(blueprints::maj1_network<mockturtle::aig_network>());
    check_parsing_equiv<Lyt>(blueprints::maj4_network<mockturtle::aig_network>());
    check_parsing_equiv<Lyt>(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>());
    check_parsing_equiv<Lyt>(blueprints::and_or_network<technology_network>());
    check_parsing_equiv<Lyt>(blueprints::nary_operation_network<technology_network>());
    check_parsing_equiv<Lyt>(blueprints::constant_gate_input_maj_network<technology_network>());
    check_parsing_equiv<Lyt>(blueprints::half_adder_network<technology_network>());
    check_parsing_equiv<Lyt>(blueprints::full_adder_network<technology_network>());
    check_parsing_equiv<Lyt>(blueprints::mux21_network<technology_network>());
    check_parsing_equiv<Lyt>(blueprints::se_coloring_corner_case_network<technology_network>());
    check_parsing_equiv<Lyt>(blueprints::fanout_substitution_corner_case_network<technology_network>());
    check_parsing_equiv<Lyt>(blueprints::inverter_network<technology_network>());
    check_parsing_equiv<Lyt>(blueprints::clpl<technology_network>());
    check_parsing_equiv<Lyt>(blueprints::one_to_five_path_difference_network<technology_network>());
    check_parsing_equiv<Lyt>(blueprints::nand_xnor_network<technology_network>());
}

/** @brief Check layout round-trip fixtures for all topologies. */
void check_parsing_equiv_layout_all()
{
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::straight_wire_gate_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::three_wire_paths_gate_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::xor_maj_gate_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::single_input_tautology_gate_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::tautology_gate_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::and_or_gate_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::and_not_gate_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::or_not_gate_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::use_and_gate_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::res_maj_gate_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::res_tautology_gate_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::crossing_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::fanout_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::unbalanced_and_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(blueprints::optimization_layout<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(
        blueprints::optimization_layout_corner_case_outputs_1<cart_gate_clk_lyt>());
    check_parsing_equiv_layout<cart_gate_clk_lyt>(
        blueprints::optimization_layout_corner_case_outputs_2<cart_gate_clk_lyt>());

    for (const auto a :
         {arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN})
    {
        check_parsing_equiv_layout<shifted_cart_gate_clk_lyt>(
            blueprints::and_or_gate_layout<shifted_cart_gate_clk_lyt>(a));
        check_parsing_equiv_layout<hex_gate_clk_lyt>(blueprints::and_or_gate_layout<hex_gate_clk_lyt>(a));
    }

    check_parsing_equiv_layout<hex_gate_clk_lyt>(
        blueprints::open_tautology_gate_layout<hex_gate_clk_lyt>(arrangement::EVEN_ROW));
    check_parsing_equiv_layout<shifted_cart_gate_clk_lyt>(
        blueprints::shifted_cart_and_or_inv_gate_layout<shifted_cart_gate_clk_lyt>(arrangement::ODD_COLUMN));
    check_parsing_equiv_layout<shifted_cart_gate_clk_lyt>(
        blueprints::row_clocked_and_xor_gate_layout<shifted_cart_gate_clk_lyt>(arrangement::EVEN_ROW));
}

TEST_CASE("Write empty gate_level layout", "[write-fgl-layout]")
{
    using gate_layout = gate_level_layout<cartesian_layout>;
    const gate_layout layout{{}, "empty"};

    std::stringstream layout_stream{};
    write_fgl_layout(layout, layout_stream);
    const auto read_layout = read_fgl_layout<gate_layout>(layout_stream, "empty");

    compare_written_and_read_layout(layout, read_layout);
}

TEST_CASE("Write and read layouts", "[write-fgl-layout]")
{
    using gate_layout = gate_level_layout<cartesian_layout>;

    check_parsing_equiv_all<gate_layout>();
    check_parsing_equiv_layout_all();
}

TEMPLATE_TEST_CASE("FGL preserves clock phases and zone assignments", "[write-fgl-layout]", cart_gate_clk_lyt,
                   shifted_cart_gate_clk_lyt, hex_gate_clk_lyt)
{
    const auto a =
        GENERATE(arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN);

    for (const auto* const name : {"OPEN3", "OPEN4", "COLUMNAR3", "COLUMNAR4", "ROW3", "ROW4", "2DDWAVE3", "2DDWAVE4",
                                   "2DDWAVEHEX3", "2DDWAVEHEX4", "BANCS"})
    {
        if constexpr (!is_hexagonal_layout_v<TestType>)
        {
            if (std::string_view{name}.starts_with("2DDWAVEHEX"))
            {
                continue;
            }
        }
        INFO(name);
        const auto scheme =
            clocking::get_scheme(name, is_hexagonal_layout_v<TestType> ? std::optional{a} : std::nullopt);
        if (!scheme.has_value())
        {
            FAIL("Unknown clocking scheme");
            return;
        }
        auto original = blueprints::make_layout<TestType>(a, {4, 3, 1}, *scheme);
        original.set_layout_name("clock phases");
        if (!scheme->is_regular())
        {
            original.assign_clock_number({1, 1, 0}, 2);
        }
        original.create_pi("a", {0, 0, 0});
        std::stringstream stream{};
        write_fgl_layout(original, stream);
        const auto restored = read_fgl_layout<TestType>(stream);
        CHECK(restored.num_clocks() == original.num_clocks());
        CHECK(restored.get_clocking_scheme().name() == original.get_clocking_scheme().name());
        CHECK(restored.get_clocking_scheme().is_regular() == original.get_clocking_scheme().is_regular());
        original.foreach_coordinate(
            [&](const auto& coordinate)
            { CHECK(restored.get_clock_number(coordinate) == original.get_clock_number(coordinate)); });
        compare_written_and_read_layout(original, restored);
    }
}

TEST_CASE("FGL preserves clock numbers on crossing layers", "[write-fgl-layout]")
{
    using gate_layout = gate_level_layout<cartesian_layout>;

    gate_layout original{{2, 2, 2}, clocking::open(), "crossing clocks"};
    original.assign_clock_number({1, 1, 1}, 2);

    std::stringstream stream{};
    write_fgl_layout(original, stream);
    const auto restored = read_fgl_layout<gate_layout>(stream);

    CHECK(restored.get_clock_number({1, 1, 0}) == 2);
    CHECK(restored.get_clock_number({1, 1, 1}) == 2);
}

TEST_CASE("FGL refuses nodes it cannot place", "[write-fgl-layout]")
{
    using gate_layout = gate_level_layout<cartesian_layout>;

    std::stringstream stream{};

    SECTION("Incomplete object")
    {
        gate_layout lyt{{2, 2, 1}, clocking::twoddwave()};
        lyt.create_buf({0, 0, 0});

        CHECK_THROWS_AS(write_fgl_layout(lyt, stream), std::invalid_argument);
    }
    SECTION("Node on a negative tile")
    {
        gate_layout lyt{{2, 2, 1}, clocking::twoddwave()};
        lyt.create_pi("a", {-1, 0, 0});

        CHECK_THROWS_AS(write_fgl_layout(lyt, stream), std::invalid_argument);
    }
    SECTION("Placed nodes")
    {
        gate_layout lyt{{2, 2, 1}, clocking::twoddwave()};
        lyt.create_pi("a", {0, 0, 0});

        CHECK_NOTHROW(write_fgl_layout(lyt, stream));
    }
}

TEST_CASE("Versioned FGL keeps interface order and dangling objects", "[write-fgl-layout]")
{
    cart_gate_clk_lyt layout{{3, 3, 1}, clocking::twoddwave()};
    const auto        b     = layout.create_pi("b", {1, 0, 0});
    const auto        a     = layout.create_pi("a", {0, 1, 0});
    const auto        gate  = layout.create_lt(a, b, {1, 1, 0});
    const auto        other = layout.create_po(a, "pass", {0, 2, 0});
    const auto        po    = layout.create_po(gate, "f", {2, 1, 0});
    layout.set_output_order(std::array{po, other});
    layout.set_input_order(std::array{a, b});
    kitty::dynamic_truth_table constant{0};
    kitty::create_from_hex_string(constant, "1");
    layout.create_node({}, constant, {0, 0, 0});
    std::stringstream stream{};
    write_fgl_layout(layout, stream);
    CHECK(stream.str().find("<fgl version=\"2\">") != std::string::npos);
    const auto restored = read_fgl_layout<cart_gate_clk_lyt>(stream);
    CHECK(restored.width() == 3);
    CHECK(restored.height() == 3);
    CHECK(restored.layers() == 1);
    CHECK(restored.size() == layout.size());
    CHECK(restored.get_input_name(0) == "a");
    CHECK(restored.get_input_name(1) == "b");
    /** @brief Restored gate whose input indices must be preserved. */
    const auto rg = restored.find_object({1, 1, 0});
    REQUIRE(rg.has_value());
    if (!rg.has_value())
    {
        return;
    }
    /** @brief First restored gate input. */
    const auto first_input = restored.source({*rg, 0});
    /** @brief Second restored gate input. */
    const auto second_input = restored.source({*rg, 1});
    CHECK((first_input.has_value() && restored.get_tile(*first_input) == layout.get_tile(a)));
    CHECK((second_input.has_value() && restored.get_tile(*second_input) == layout.get_tile(b)));
    /** @brief Placed zero-input function restored from the file. */
    const auto constant_object = restored.find_object({0, 0, 0});
    CHECK((constant_object.has_value() && restored.node_function(*constant_object).num_vars() == 0));
    CHECK(restored.get_output_name(0) == layout.get_name(po));
    CHECK(restored.get_output_name(1) == "pass");
    compare_written_and_read_layout(layout, restored);
}

TEST_CASE("FGL validates every object before changing the stream", "[write-fgl-layout]")
{
    cart_gate_clk_lyt layout{{4, 2, 1}, clocking::twoddwave()};
    layout.create_pi("unused", {0, 0, 0});
    const auto        wire = layout.create_buf({1, 0, 0});
    std::stringstream stream{};
    stream << "sentinel";
    CHECK_THROWS_AS(write_fgl_layout(layout, stream), std::invalid_argument);
    CHECK(stream.str() == "sentinel");
    layout.connect(wire, {wire, 0});
    CHECK_THROWS_AS(write_fgl_layout(layout, stream), std::invalid_argument);
    CHECK(stream.str() == "sentinel");
}

TEST_CASE("FGL rejects invalid finished placement and metadata", "[write-fgl-layout]")
{
    cart_gate_clk_lyt layout{{3, 1, 1}, clocking::twoddwave()};
    const auto        source = layout.create_pi("a", {0, 0, 0});
    SECTION("Nonadjacent connection")
    {
        layout.create_po(source, "f", {2, 0, 0});
    }
    SECTION("Wrong clock")
    {
        layout.create_po(source, "f", {1, 0, 0});
        layout.assign_clock_number({1, 0, 0}, 0);
    }
    SECTION("Outside extent")
    {
        layout.move_node(source, {-1, 0, 0});
    }
    SECTION("Unsupported clocking name")
    {
        layout.replace_clocking_scheme(clocking::scheme{"CUSTOM", {{0}}, 4, 1, 1});
    }
    SECTION("Unsupported base under a known name")
    {
        layout.replace_clocking_scheme(clocking::scheme{clocking::TWODDWAVE_NAME, {{0}}, 4, 3, 3});
    }
    SECTION("Outside clock override")
    {
        layout.assign_clock_number({3, 0, 0}, 0);
    }
    SECTION("Outside synchronization element")
    {
        layout.assign_synchronization_element({3, 0, 0}, 1);
    }
    std::stringstream stream{};
    stream << "kept";
    CHECK_THROWS_AS(write_fgl_layout(layout, stream), std::invalid_argument);
    CHECK(stream.str() == "kept");
}

TEST_CASE("FGL keeps sparse metadata in a large multilayer extent", "[write-fgl-layout]")
{
    cart_gate_clk_lyt layout{{1000000000, 1000000000, 3}, clocking::twoddwave()};
    layout.assign_clock_number({999999999, 999999999, 2}, 2);
    layout.assign_synchronization_element({999999999, 999999999, 2}, 7);
    std::stringstream stream{};
    write_fgl_layout(layout, stream);
    CHECK(stream.str().size() < 2000);
    const auto restored = read_fgl_layout<cart_gate_clk_lyt>(stream);
    CHECK(restored.width() == 1000000000);
    CHECK(restored.height() == 1000000000);
    CHECK(restored.layers() == 3);
    CHECK(restored.get_clock_number({999999999, 999999999, 2}) == 2);
    CHECK(restored.get_clock_number({0, 1, 0}) == layout.get_clock_number({0, 1, 0}));
    CHECK(restored.get_synchronization_element({999999999, 999999999, 2}) == 7);
}

TEMPLATE_TEST_CASE("FGL preserves manual obstructions independently of occupancy", "[write-fgl-layout]",
                   cart_gate_clk_lyt, shifted_cart_gate_clk_lyt, hex_gate_clk_lyt)
{
    TestType layout = []
    {
        if constexpr (is_cartesian_layout_v<TestType>)
        {
            return TestType{{3, 2, 1}, clocking::twoddwave()};
        }
        else
        {
            return TestType{arrangement::EVEN_ROW, {3, 2, 1}, clocking::twoddwave()};
        }
    }();
    const auto input = layout.create_pi("a", {0, 0});
    const auto wire  = layout.create_buf(input, {1, 0});
    layout.create_po(wire, "f", {2, 0});
    layout.obstruct_coordinate({1, 0});
    layout.obstruct_coordinate({1, 1});
    layout.obstruct_coordinate({-1, -2, -3});
    layout.obstruct_coordinate({7, 8, 9});
    layout.obstruct_connection({0, 0}, {1, 0});
    layout.obstruct_connection({-1, -2, -3}, {7, 8, 9});

    std::stringstream stream{};
    write_fgl_layout(layout, stream);
    auto restored = read_fgl_layout<TestType>(stream);
    CHECK(restored.is_obstructed_coordinate({1, 1}));
    CHECK(restored.is_obstructed_coordinate({-1, -2, -3}));
    CHECK(restored.is_obstructed_coordinate({7, 8, 9}));
    CHECK(restored.is_obstructed_connection({-1, -2, -3}, {7, 8, 9}));
    CHECK_FALSE(restored.is_obstructed_connection({7, 8, 9}, {-1, -2, -3}));

    /** @brief Occupied tile whose manual obstruction must survive object removal. */
    const auto occupied = restored.find_object({1, 0});
    REQUIRE(occupied.has_value());
    if (occupied.has_value())
    {
        restored.remove(*occupied);
    }
    CHECK(restored.is_obstructed_coordinate({1, 0}));
    CHECK(restored.is_obstructed_connection({0, 0}, {1, 0}));
    restored.clear_obstructed_coordinates();
    restored.clear_obstructed_connections();
    CHECK_FALSE(restored.is_obstructed_coordinate({1, 0}));
    CHECK_FALSE(restored.is_obstructed_coordinate({-1, -2, -3}));
    CHECK_FALSE(restored.is_obstructed_connection({0, 0}, {1, 0}));
    CHECK(restored.is_obstructed_coordinate({2, 0}));
}

TEST_CASE("FGL preserves XML whitespace in layout and object names", "[write-fgl-layout]")
{
    cart_gate_clk_lyt layout{{3, 1, 1}, clocking::twoddwave()};
    const std::string label{"first\r\nsecond\t<&>"};
    layout.set_layout_name(label);
    const auto input = layout.create_pi(label, {0, 0});
    const auto wire  = layout.create_buf(input, {1, 0});
    layout.set_name(wire, label);
    layout.create_po(wire, label, {2, 0});
    std::stringstream stream{};
    write_fgl_layout(layout, stream);
    const auto restored = read_fgl_layout<cart_gate_clk_lyt>(stream);
    CHECK(restored.get_layout_name() == label);
    restored.foreach_node([&](const auto id) { CHECK(restored.get_name(id) == label); });
}

TEST_CASE("FGL rejects illegal XML controls before changing output", "[write-fgl-layout]")
{
    const auto        control = GENERATE(char{0}, char{1}, char{8}, char{11}, char{12}, char{14}, char{31});
    cart_gate_clk_lyt layout{{3, 1, 1}, clocking::twoddwave()};
    const auto        input  = layout.create_pi("a", {0, 0});
    const auto        wire   = layout.create_buf(input, {1, 0});
    const auto        output = layout.create_po(wire, "f", {2, 0});
    const std::string label  = std::string{"first"} + control + "second";
    SECTION("Layout name")
    {
        layout.set_layout_name(label);
    }
    SECTION("PI name")
    {
        layout.set_name(input, label);
    }
    SECTION("Wire name")
    {
        layout.set_name(wire, label);
    }
    SECTION("PO name")
    {
        layout.set_name(output, label);
    }
    std::stringstream stream{};
    stream << "sentinel";
    CHECK_THROWS_AS(write_fgl_layout(layout, stream), std::invalid_argument);
    CHECK(stream.str() == "sentinel");

    const auto filename =
        std::filesystem::temp_directory_path() / ("fiction-fgl-text-" + std::to_string(std::random_device{}()));
    {
        std::ofstream original{filename};
        original << "sentinel";
    }
    CHECK_THROWS_AS(write_fgl_layout(layout, filename.string()), std::invalid_argument);
    std::ifstream original{filename};
    std::string   contents{};
    std::getline(original, contents);
    CHECK(contents == "sentinel");
    original.close();
    std::filesystem::remove(filename);
}
