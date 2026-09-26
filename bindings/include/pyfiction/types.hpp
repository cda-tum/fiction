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
 * @brief The concrete network and layout types the Python bindings instantiate.
 * @author Marcel Walter (marcelwa)
 * @author Benjamin Hien (hibenj)
 */

#pragma once

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/layouts/shifted_cartesian_layout.hpp>
#include <fiction/networks/technology_network.hpp>

#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/views/names_view.hpp>

#include <cstdint>

#include <nanobind/nanobind.h>

namespace pyfiction
{

/**
 * Truth table.
 */
using py_tt = kitty::dynamic_truth_table;
/**
 * @brief Technology network used by physical design and verification bindings.
 */
using py_tec_network = mockturtle::names_view<fiction::networks::technology_network>;
/**
 * @brief Named AND-inverter graph.
 */
using py_aig_network = mockturtle::names_view<mockturtle::aig_network>;
/**
 * @brief Named XOR-AND-inverter graph.
 */
using py_xag_network = mockturtle::names_view<mockturtle::xag_network>;
/**
 * @brief Named majority-inverter graph.
 */
using py_mig_network = mockturtle::names_view<mockturtle::mig_network>;
/**
 * Coordinates.
 */
using py_offset_coordinate = fiction::layouts::coords::offset;
using py_cube_coordinate   = fiction::layouts::coords::cube;
/**
 * Cartesian layout.
 */
using py_cartesian_layout = fiction::layouts::cartesian_layout<py_offset_coordinate>;
/**
 * Shifted Cartesian layout.
 */
using py_shifted_cartesian_layout =
    fiction::layouts::shifted_cartesian_layout<py_offset_coordinate, fiction::layouts::odd_column_cartesian>;
/**
 * Hexagonal layout.
 */
using py_hexagonal_layout = fiction::layouts::hexagonal_layout<py_offset_coordinate, fiction::layouts::even_row_hex>;
/**
 * odd row cartesian layout.
 */
using py_odd_row_cartesian_layout =
    fiction::layouts::shifted_cartesian_layout<py_offset_coordinate, fiction::layouts::odd_row_cartesian>;
/**
 * even row cartesian layout.
 */
using py_even_row_cartesian_layout =
    fiction::layouts::shifted_cartesian_layout<py_offset_coordinate, fiction::layouts::even_row_cartesian>;
/**
 * even column cartesian layout.
 */
using py_even_column_cartesian_layout =
    fiction::layouts::shifted_cartesian_layout<py_offset_coordinate, fiction::layouts::even_column_cartesian>;
/**
 * odd row hex layout.
 */
using py_odd_row_hex_layout = fiction::layouts::hexagonal_layout<py_offset_coordinate, fiction::layouts::odd_row_hex>;
/**
 * odd column hex layout.
 */
using py_odd_column_hex_layout =
    fiction::layouts::hexagonal_layout<py_offset_coordinate, fiction::layouts::odd_column_hex>;
/**
 * even column hex layout.
 */
using py_even_column_hex_layout =
    fiction::layouts::hexagonal_layout<py_offset_coordinate, fiction::layouts::even_column_hex>;
/**
 * Cartesian layout with unrestricted stacked coordinates.
 */
using py_stacked_cartesian_layout = fiction::layouts::cartesian_layout<py_cube_coordinate>;
/**
 * Gate-level odd row cartesian layout.
 */
using py_odd_row_cartesian_gate_layout = fiction::layouts::gate_level_layout<py_odd_row_cartesian_layout>;
/**
 * Gate-level even row cartesian layout.
 */
using py_even_row_cartesian_gate_layout = fiction::layouts::gate_level_layout<py_even_row_cartesian_layout>;
/**
 * Gate-level even column cartesian layout.
 */
using py_even_column_cartesian_gate_layout = fiction::layouts::gate_level_layout<py_even_column_cartesian_layout>;
/**
 * Gate-level odd row hex layout.
 */
using py_odd_row_hex_gate_layout = fiction::layouts::gate_level_layout<py_odd_row_hex_layout>;
/**
 * Gate-level odd column hex layout.
 */
using py_odd_column_hex_gate_layout = fiction::layouts::gate_level_layout<py_odd_column_hex_layout>;
/**
 * Gate-level even column hex layout.
 */
using py_even_column_hex_gate_layout = fiction::layouts::gate_level_layout<py_even_column_hex_layout>;
/**
 * Cartesian gate layout.
 */
using py_cartesian_gate_layout = fiction::layouts::gate_level_layout<py_cartesian_layout>;
/**
 * Shifted Cartesian gate layout.
 */
using py_shifted_cartesian_gate_layout = fiction::layouts::gate_level_layout<py_shifted_cartesian_layout>;
/**
 * Hexagonal gate layout.
 */
using py_hexagonal_gate_layout = fiction::layouts::gate_level_layout<py_hexagonal_layout>;
}  // namespace pyfiction

namespace nanobind::detail
{

/**
 * @brief Describes offset coordinate tuple inputs while retaining the registered class caster.
 */
template <>
struct type_caster<pyfiction::py_offset_coordinate> : type_caster_base<pyfiction::py_offset_coordinate>
{
    /**
     * @brief Accepted Python inputs and the concrete Python return type.
     */
    // NOLINTNEXTLINE(readability-identifier-naming): nanobind requires the member name Name.
    static constexpr auto Name =
        const_name<pyfiction::py_offset_coordinate>() + io_name(" | tuple[int, int] | tuple[int, int, int]", "");
};

/**
 * @brief Describes cube coordinate tuple inputs while retaining the registered class caster.
 */
template <>
struct type_caster<pyfiction::py_cube_coordinate> : type_caster_base<pyfiction::py_cube_coordinate>
{
    /**
     * @brief Accepted Python inputs and the concrete Python return type.
     */
    // NOLINTNEXTLINE(readability-identifier-naming): nanobind requires the member name Name.
    static constexpr auto Name =
        const_name<pyfiction::py_cube_coordinate>() + io_name(" | tuple[int, int] | tuple[int, int, int]", "");
};

/**
 * @brief Keeps constructor self arguments as a single registered class.
 *
 * nanobind recognizes constructors by the exact self descriptor, without an input union.
 * @tparam Coordinate Coordinate type constructed by the Python call.
 */
template <typename Coordinate>
struct coordinate_constructor_caster
{
    NB_TYPE_CASTER(nanobind::pointer_and_handle<Coordinate>, const_name<Coordinate>())

    /**
     * @brief Converts the constructor self argument, including uninitialized instances.
     * @param src Python instance being constructed.
     * @param flags Constructor conversion flags.
     * @param cleanup Temporary conversion storage.
     * @return Whether the instance belongs to the coordinate class.
     */
    bool from_python(nanobind::handle src, std::uint32_t flags, cleanup_list* cleanup) noexcept
    {
        type_caster_base<Coordinate> caster;
        if (!caster.from_python(src, flags, cleanup))
        {
            return false;
        }
        value = {caster.operator Coordinate*(), src};
        return true;
    }
};

/**
 * @brief Preserves offset coordinate constructor dispatch.
 */
template <>
struct type_caster<nanobind::pointer_and_handle<pyfiction::py_offset_coordinate>>
        : coordinate_constructor_caster<pyfiction::py_offset_coordinate>
{};

/**
 * @brief Preserves cube coordinate constructor dispatch.
 */
template <>
struct type_caster<nanobind::pointer_and_handle<pyfiction::py_cube_coordinate>>
        : coordinate_constructor_caster<pyfiction::py_cube_coordinate>
{};

}  // namespace nanobind::detail
