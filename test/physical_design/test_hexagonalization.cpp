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
 * @brief Tests for `fiction/physical_design/hexagonalization.hpp`.
 * @author Simon Hofmann (simon1hofmann)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"
#include "utils/progress_recorder.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/layouts/layout_base.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/physical_design/hexagonalization.hpp>
#include <fiction/physical_design/orthogonal.hpp>
#include <fiction/synthesis/network_conversion.hpp>
#include <fiction/types.hpp>

#include <mockturtle/networks/aig.hpp>

#include <cstdint>
#include <optional>
#include <vector>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::networks;
using namespace fiction::physical_design;

/**
 * @brief Checks that a hexagonal layout carries no explicit obstructions, i.e., that only occupied tiles and existing
 * signal connections are obstructed.
 * @tparam HexLyt Hexagonal gate layout type.
 * @param hex_layout Layout to check.
 */
template <typename HexLyt>
static void check_no_explicit_obstructions(const HexLyt& hex_layout)
{
    hex_layout.foreach_coordinate(
        [&hex_layout](const auto& src)
        {
            CHECK(hex_layout.is_obstructed_coordinate(src) == !hex_layout.is_empty_tile(src));

            hex_layout.foreach_adjacent_coordinate(
                src,
                [&hex_layout, &src](const auto& tgt)
                {
                    for (const auto& t : {std::optional{tgt}, hex_layout.above(tgt)})
                    {
                        if (!t)
                        {
                            continue;
                        }
                        CHECK(hex_layout.is_obstructed_connection(src, *t) ==
                              (hex_layout.is_incoming_signal(*t, src) || hex_layout.is_outgoing_signal(src, t)));
                    }
                });
        });
}

/**
 * @brief Checks hexagonalization equivalence and terminal extension positions.
 * @tparam Lyt Source gate-level layout type.
 * @tparam Ntk Logic network type.
 * @param ntk Network to place and map.
 */
template <typename Lyt, typename Ntk>
static void check_mapping_equiv(const Ntk& ntk)
{
    /** @brief Supported terminal extension modes. */
    using mode                         = hexagonalization_params::io_pin_extension_mode;
    const auto [name, inputs, outputs] = GENERATE(
        (Catch::Generators::table<const char*, mode, mode>({{"none", mode::NONE, mode::NONE},
                                                            {"inputs", mode::EXTEND, mode::NONE},
                                                            {"outputs", mode::NONE, mode::EXTEND},
                                                            {"inputs and outputs", mode::EXTEND, mode::EXTEND}})));
    CAPTURE(name);

    const auto layout = orthogonal<typename Lyt::coordinate_layout>(
        fiction::synthesis::convert_network<named_technology_network>(ntk), {});

    hexagonalization_stats        stats{};
    const hexagonalization_params params{.input_pin_extension = inputs, .output_pin_extension = outputs};
    const auto                    hex_layout = hexagonalization(layout, params, &stats);

    check_eq(ntk, layout);
    check_eq(ntk, hex_layout);
    check_eq(layout, hex_layout);

    CHECK(hex_layout.num_pis() == layout.num_pis());
    CHECK(hex_layout.num_pos() == layout.num_pos());
    if (inputs != mode::NONE)
    {
        hex_layout.foreach_pi([&hex_layout](const auto gate) { CHECK(hex_layout.get_tile(gate).y == 0); });
    }
    if (outputs != mode::NONE)
    {
        check_no_explicit_obstructions(hex_layout);
        hex_layout.foreach_po([&hex_layout](const auto gate)
                              { CHECK(hex_layout.get_tile(gate).y == static_cast<int32_t>(hex_layout.height() - 1)); });
    }
}

/**
 * @brief Checks hexagonalization equivalence, names, and terminal extension positions.
 * @tparam Lyt Source gate-level layout type.
 * @param lyt Layout to map.
 */
template <typename Lyt>
static void check_mapping_equiv_layout(const Lyt& lyt)
{
    /** @brief Supported terminal extension modes. */
    using mode                         = hexagonalization_params::io_pin_extension_mode;
    const auto [name, inputs, outputs] = GENERATE((Catch::Generators::table<const char*, mode, mode>(
        {{"none", mode::NONE, mode::NONE},
         {"inputs", mode::EXTEND, mode::NONE},
         {"outputs", mode::NONE, mode::EXTEND},
         {"inputs and outputs", mode::EXTEND, mode::EXTEND},
         {"planar inputs", mode::EXTEND_PLANAR, mode::NONE},
         {"planar outputs", mode::NONE, mode::EXTEND_PLANAR},
         {"planar inputs and outputs", mode::EXTEND_PLANAR, mode::EXTEND_PLANAR}})));
    CAPTURE(name);

    hexagonalization_stats        stats{};
    const hexagonalization_params params{.input_pin_extension = inputs, .output_pin_extension = outputs};
    const auto                    hex_layout = hexagonalization(lyt, params, &stats);

    check_eq(lyt, hex_layout);
    CHECK(lyt.get_layout_name() == hex_layout.get_layout_name());
    CHECK(hex_layout.num_pis() == lyt.num_pis());
    CHECK(hex_layout.num_pos() == lyt.num_pos());
    if (inputs != mode::NONE)
    {
        hex_layout.foreach_pi([&hex_layout](const auto gate) { CHECK(hex_layout.get_tile(gate).y == 0); });
    }
    if (outputs != mode::NONE)
    {
        hex_layout.foreach_po([&hex_layout](const auto gate)
                              { CHECK(hex_layout.get_tile(gate).y == static_cast<int32_t>(hex_layout.height() - 1)); });
    }
}

/** @brief Checks the network and layout blueprints independently. @tparam Lyt Source layout type. */
template <typename Lyt>
static void check_mapping_equiv_all()
{
    SECTION("maj1_network")
    {
        check_mapping_equiv<Lyt>(blueprints::maj1_network<mockturtle::aig_network>());
    }
    SECTION("maj4_network")
    {
        check_mapping_equiv<Lyt>(blueprints::maj4_network<mockturtle::aig_network>());
    }
    SECTION("unbalanced_and_inv_network")
    {
        check_mapping_equiv<Lyt>(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>());
    }
    SECTION("and_or_network")
    {
        check_mapping_equiv<Lyt>(blueprints::and_or_network<technology_network>());
    }
    SECTION("nary_operation_network")
    {
        check_mapping_equiv<Lyt>(blueprints::nary_operation_network<technology_network>());
    }
    SECTION("constant_gate_input_maj_network")
    {
        check_mapping_equiv<Lyt>(blueprints::constant_gate_input_maj_network<technology_network>());
    }
    SECTION("half_adder_network")
    {
        check_mapping_equiv<Lyt>(blueprints::half_adder_network<technology_network>());
    }
    SECTION("full_adder_network")
    {
        check_mapping_equiv<Lyt>(blueprints::full_adder_network<technology_network>());
    }
    SECTION("mux21_network")
    {
        check_mapping_equiv<Lyt>(blueprints::mux21_network<technology_network>());
    }
    SECTION("se_coloring_corner_case_network")
    {
        check_mapping_equiv<Lyt>(blueprints::se_coloring_corner_case_network<technology_network>());
    }
    SECTION("fanout_substitution_corner_case_network")
    {
        check_mapping_equiv<Lyt>(blueprints::fanout_substitution_corner_case_network<technology_network>());
    }
    SECTION("inverter_network")
    {
        check_mapping_equiv<Lyt>(blueprints::inverter_network<technology_network>());
    }
    SECTION("clpl")
    {
        check_mapping_equiv<Lyt>(blueprints::clpl<technology_network>());
    }
    SECTION("one_to_five_path_difference_network")
    {
        check_mapping_equiv<Lyt>(blueprints::one_to_five_path_difference_network<technology_network>());
    }
    SECTION("nand_xnor_network")
    {
        check_mapping_equiv<Lyt>(blueprints::nand_xnor_network<technology_network>());
    }
    SECTION("straight_wire_gate_layout")
    {
        check_mapping_equiv_layout(blueprints::straight_wire_gate_layout<cartesian_gate_layout>());
    }
    SECTION("or_not_gate_layout")
    {
        check_mapping_equiv_layout(blueprints::or_not_gate_layout<cartesian_gate_layout>());
    }
    SECTION("crossing_layout")
    {
        check_mapping_equiv_layout(blueprints::crossing_layout<cartesian_gate_layout>());
    }
    SECTION("tautology_gate_layout")
    {
        check_mapping_equiv_layout(blueprints::tautology_gate_layout<cartesian_gate_layout>());
    }
    SECTION("ge_gt_le_lt_layout")
    {
        check_mapping_equiv_layout(blueprints::ge_gt_le_lt_layout<cartesian_gate_layout>());
    }
    SECTION("po_extension_corner_case_layout")
    {
        check_mapping_equiv_layout(blueprints::po_extension_corner_case_layout<cartesian_gate_layout>());
    }
}

TEST_CASE("Cartesian to hexagonal", "[hexagonalization]")
{
    /** @brief Coordinates in the source and result geometries. */
    using coordinate = layout_base::coordinate;
    const auto [source, expected] =
        GENERATE((Catch::Generators::table<coordinate, coordinate>({{{0, 0, 0}, {1, 0, 0}},
                                                                    {{0, 0, 1}, {1, 0, 1}},
                                                                    {{1, 0, 0}, {2, 1, 0}},
                                                                    {{1, 0, 1}, {2, 1, 1}},
                                                                    {{2, 0, 0}, {2, 2, 0}},
                                                                    {{2, 0, 1}, {2, 2, 1}},
                                                                    {{0, 1, 0}, {1, 1, 0}},
                                                                    {{0, 1, 1}, {1, 1, 1}},
                                                                    {{1, 1, 0}, {1, 2, 0}},
                                                                    {{1, 1, 1}, {1, 2, 1}},
                                                                    {{2, 1, 0}, {2, 3, 0}},
                                                                    {{2, 1, 1}, {2, 3, 1}},
                                                                    {{0, 2, 0}, {0, 2, 0}},
                                                                    {{0, 2, 1}, {0, 2, 1}},
                                                                    {{1, 2, 0}, {1, 3, 0}},
                                                                    {{1, 2, 1}, {1, 3, 1}},
                                                                    {{2, 2, 0}, {1, 4, 0}},
                                                                    {{2, 2, 1}, {1, 4, 1}}})));
    CAPTURE(source.x, source.y, source.z);
    CHECK(physical_design::detail::to_hex(source, 3) == expected);
}

TEST_CASE("Layout equivalence", "[hexagonalization]")
{
    SECTION("Cartesian layouts")
    {
        check_mapping_equiv_all<cartesian_gate_layout>();
    }
}

TEST_CASE("Hexagonalization preserves declared input holes and terminal order", "[hexagonalization]")
{
    cartesian_gate_layout layout{{4, 3, 2}, clocking::twoddwave()};
    const auto            b    = layout.create_pi("b", {0, 1});
    const auto            a    = layout.create_pi("a", {1, 0});
    const auto            gate = layout.create_lt(a, b, {1, 1});
    layout.disconnect({gate, 0});
    layout.create_po(gate, "f", {3, 1});
    layout.set_input_order(std::vector{a, b});
    const auto hex = hexagonalization(layout);
    REQUIRE(hex.num_pis() == 2);
    REQUIRE(hex.num_pos() == 1);
    REQUIRE(hex.num_gates() == 1);
    CHECK(hex.get_input_name(0) == "a");
    CHECK(hex.get_input_name(1) == "b");
    CHECK(hex.get_output_name(0) == "f");
    hex.foreach_gate(
        [&](const auto id)
        {
            CHECK(hex.is_lt(id));
            CHECK_FALSE(hex.source({id, 0}));
            REQUIRE(hex.source({id, 1}));
            CHECK(hex.get_name(*hex.source({id, 1})) == "b");
        });
}

TEST_CASE("Hexagonalization reports progress", "[hexagonalization]")
{
    const auto ntk    = blueprints::mux21_network<technology_network>();
    const auto layout = orthogonal<cartesian_layout>(ntk);

    progress_recorder             rec{};
    const hexagonalization_params params{.input_pin_extension  = hexagonalization_params::io_pin_extension_mode::EXTEND,
                                         .output_pin_extension = hexagonalization_params::io_pin_extension_mode::EXTEND,
                                         .on_progress          = rec.callback()};

    const auto hex_layout = hexagonalization(layout, params);

    check_eq(ntk, hex_layout);

    CHECK(rec.is_consistent("objects"));
    CHECK(rec.final_count("objects") == layout.size());
    CHECK(rec.is_consistent("input pins"));
    CHECK(rec.is_consistent("output pins"));
}

TEST_CASE("Hexagonalization preserves the crossing layer", "[hexagonalization]")
{
    /** Sparse layout with one input on the crossing layer. */
    cartesian_gate_layout layout{{1, 1, 2}, clocking::twoddwave()};
    layout.create_pi("a", {0, 0, 1});

    /** Converted geometry retains both layers. */
    const auto converted = hexagonalization(layout);

    CHECK(converted.layers() == 2);
    REQUIRE(converted.num_pis() == 1);
    CHECK(converted.get_tile(converted.pi_at(0)).z == 1);
}
