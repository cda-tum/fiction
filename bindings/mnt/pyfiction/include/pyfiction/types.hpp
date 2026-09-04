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
 * @author Simon Hofmann (simon1hofmann)
 */

#pragma once

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/layouts/obstruction_layout.hpp>
#include <fiction/layouts/shifted_cartesian_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/technology/inml/technology.hpp>
#include <fiction/technology/qca/technology.hpp>
#include <fiction/technology/sidb/surfaces/charge_distribution_surface.hpp>
#include <fiction/technology/sidb/surfaces/lattice.hpp>
#include <fiction/technology/sidb/surfaces/lattice_orientations.hpp>
#include <fiction/technology/sidb/technology.hpp>

#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/views/names_view.hpp>

namespace pyfiction
{

/**
 * Truth table.
 */
using py_tt = kitty::dynamic_truth_table;
/**
 * Logic network.
 */
using py_logic_network = mockturtle::names_view<fiction::networks::technology_network>;
/**
 * Coordinates.
 */
using py_offset_coordinate = fiction::layouts::coords::offset;
using py_cube_coordinate   = fiction::layouts::coords::cube;
using py_siqad_coordinate  = fiction::layouts::coords::siqad;
/**
 * Cartesian layout.
 */
using py_cartesian_layout = fiction::layouts::cartesian_layout<py_offset_coordinate>;
/**
 * Cartesian layout with cube coordinates.
 */
using py_cartesian_layout_cube = fiction::layouts::cartesian_layout<py_cube_coordinate>;
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
 * Cartesian clocked layout.
 */
using py_cartesian_clocked_layout =
    fiction::layouts::clocked_layout<fiction::layouts::tile_based_layout<py_cartesian_layout>>;
/**
 * Cartesian clocked layout with cube coordinates.
 */
using py_cartesian_clocked_layout_cube =
    fiction::layouts::clocked_layout<fiction::layouts::tile_based_layout<py_cartesian_layout_cube>>;
/**
 * Shifted Cartesian clocked layout.
 */
using py_shifted_cartesian_clocked_layout =
    fiction::layouts::clocked_layout<fiction::layouts::tile_based_layout<py_shifted_cartesian_layout>>;
/**
 * Hexagonal clocked layout.
 */
using py_hexagonal_clocked_layout =
    fiction::layouts::clocked_layout<fiction::layouts::tile_based_layout<py_hexagonal_layout>>;
/**
 * Cartesian gate layout.
 */
using py_cartesian_gate_layout = fiction::layouts::gate_level_layout<py_cartesian_clocked_layout>;
/**
 * Shifted Cartesian gate layout.
 */
using py_shifted_cartesian_gate_layout = fiction::layouts::gate_level_layout<py_shifted_cartesian_clocked_layout>;
/**
 * Hexagonal gate layout.
 */
using py_hexagonal_gate_layout = fiction::layouts::gate_level_layout<py_hexagonal_clocked_layout>;
/**
 * Cartesian gate-level obstruction layout.
 */
using py_cartesian_obstruction_layout = fiction::layouts::obstruction_layout<py_cartesian_gate_layout>;
/**
 * Shifted Cartesian gate-level obstruction layout.
 */
using py_shifted_cartesian_obstruction_layout = fiction::layouts::obstruction_layout<py_shifted_cartesian_gate_layout>;
/**
 * Hexagonal gate-level obstruction layout.
 */
using py_hexagonal_obstruction_layout = fiction::layouts::obstruction_layout<py_hexagonal_gate_layout>;
/**
 * Cartesian cell layout.
 */
template <typename Technology, typename Coordinate = py_offset_coordinate>
using py_cartesian_cell_layout = fiction::layouts::cell_level_layout<
    Technology, fiction::layouts::clocked_layout<
                    fiction::layouts::tile_based_layout<fiction::layouts::cartesian_layout<Coordinate>>>>;
/**
 * QCA cell layout.
 */
using py_qca_layout = py_cartesian_cell_layout<fiction::qca::qca_technology>;
/**
 * iNML cell layout.
 */
using py_inml_layout = py_cartesian_cell_layout<fiction::inml::inml_technology>;
/**
 * MolQCA cell layout.
 */
using py_mol_qca_layout = py_cartesian_cell_layout<fiction::qca::mol_qca_technology>;
/**
 * SiDB cell layout.
 */
using py_sidb_layout = py_cartesian_cell_layout<fiction::sidb::sidb_technology>;
/**
 * SiDB cell layout with cube coordinates.
 */
using py_sidb_layout_cube = py_cartesian_cell_layout<fiction::sidb::sidb_technology, py_cube_coordinate>;

/**
 * SiDB lattice layout.
 */
template <typename LatticeOrientation, typename SidbLayout = py_sidb_layout>
using py_sidb_lattice = fiction::sidb::surfaces::lattice<LatticeOrientation, SidbLayout>;
/**
 * SiDB cell layout (with specified H-Si(100)-2x1 lattice orientation).
 */
using py_sidb_100_lattice = py_sidb_lattice<fiction::sidb::surfaces::lattice_100>;
/**
 * SiDB cell layout (with specified H-Si(111)-1x1 lattice orientation).
 */
using py_sidb_111_lattice = py_sidb_lattice<fiction::sidb::surfaces::lattice_111>;

/**
 * Charge distribution surface. This is a special SiDB cell-level layout that is used for the SiDB simulation
 * algorithms.
 */

template <typename Lyt>
using py_charge_distribution_surface_layout = fiction::sidb::surfaces::charge_distribution_surface<Lyt>;

using py_charge_distribution_surface = fiction::sidb::surfaces::charge_distribution_surface<py_sidb_layout>;

/**
 * Charge distribution surface with underlying H-Si(100)-2x1 lattice.
 */
using py_charge_distribution_surface_100 = py_charge_distribution_surface_layout<py_sidb_100_lattice>;
/**
 * Charge distribution surface with underlying H-Si(111)-1x1 lattice.
 */
using py_charge_distribution_surface_111 = py_charge_distribution_surface_layout<py_sidb_111_lattice>;

}  // namespace pyfiction
