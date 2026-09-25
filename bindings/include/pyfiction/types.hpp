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
