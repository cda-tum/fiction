//
// Created by marcel on 15.12.22.
//

#ifndef PYFICTION_TYPES_HPP
#define PYFICTION_TYPES_HPP

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
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/technology/sidb_lattice.hpp>
#include <fiction/technology/sidb_lattice_orientations.hpp>

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
using py_logic_network = mockturtle::names_view<fiction::technology_network>;
/**
 * Coordinates.
 */
using py_offset_coordinate = fiction::offset::ucoord_t;
using py_cube_coordinate   = fiction::cube::coord_t;
using py_siqad_coordinate  = fiction::siqad::coord_t;
/**
 * Cartesian layout.
 */
template <typename OffsetCoordinateType = py_offset_coordinate>
using py_cartesian_layout = fiction::cartesian_layout<OffsetCoordinateType>;
/**
 * Shifted Cartesian layout.
 */
template <typename OffsetCoordinateType = py_offset_coordinate>
using py_shifted_cartesian_layout =
    fiction::shifted_cartesian_layout<OffsetCoordinateType, fiction::odd_column_cartesian>;
/**
 * Hexagonal layout.
 */
template <typename OffsetCoordinateType = py_offset_coordinate>
using py_hexagonal_layout = fiction::hexagonal_layout<OffsetCoordinateType, fiction::even_row_hex>;
/**
 * Cartesian clocked layout.
 */
template <typename OffsetCoordinateType = py_offset_coordinate>
using py_cartesian_clocked_layout =
    fiction::clocked_layout<fiction::tile_based_layout<py_cartesian_layout<OffsetCoordinateType>>>;
/**
 * Shifted Cartesian clocked layout.
 */
template <typename OffsetCoordinateType = py_offset_coordinate>
using py_shifted_cartesian_clocked_layout =
    fiction::clocked_layout<fiction::tile_based_layout<py_shifted_cartesian_layout<OffsetCoordinateType>>>;
/**
 * Hexagonal clocked layout.
 */
template <typename OffsetCoordinateType = py_offset_coordinate>
using py_hexagonal_clocked_layout =
    fiction::clocked_layout<fiction::tile_based_layout<py_hexagonal_layout<OffsetCoordinateType>>>;
/**
 * Cartesian gate layout.
 */
template <typename OffsetCoordinateType = py_offset_coordinate>
using py_cartesian_gate_layout = fiction::gate_level_layout<py_cartesian_clocked_layout<OffsetCoordinateType>>;
/**
 * Shifted Cartesian gate layout.
 */
template <typename OffsetCoordinateType = py_offset_coordinate>
using py_shifted_cartesian_gate_layout =
    fiction::gate_level_layout<py_shifted_cartesian_clocked_layout<OffsetCoordinateType>>;
/**
 * Hexagonal gate layout.
 */
template <typename OffsetCoordinateType = py_offset_coordinate>
using py_hexagonal_gate_layout = fiction::gate_level_layout<py_hexagonal_clocked_layout<OffsetCoordinateType>>;
/**
 * Cartesian gate-level obstruction layout.
 */
using py_cartesian_obstruction_layout = fiction::obstruction_layout<py_cartesian_gate_layout<py_offset_coordinate>>;
/**
 * Shifted Cartesian gate-level obstruction layout.
 */
using py_shifted_cartesian_obstruction_layout =
    fiction::obstruction_layout<py_shifted_cartesian_gate_layout<py_offset_coordinate>>;
/**
 * Hexagonal gate-level obstruction layout.
 */
using py_hexagonal_obstruction_layout = fiction::obstruction_layout<py_hexagonal_gate_layout<py_offset_coordinate>>;
/**
 * Cartesian cell layout.
 */
template <typename Technology, typename OffsetCoordinateType = py_offset_coordinate>
using py_cartesian_cell_layout =
    fiction::cell_level_layout<Technology, py_cartesian_clocked_layout<OffsetCoordinateType>>;
/**
 * QCA cell layout.
 */
template <typename OffsetCoordinateType = py_offset_coordinate>
using py_qca_layout = py_cartesian_cell_layout<fiction::qca_technology, OffsetCoordinateType>;
/**
 * iNML cell layout.
 */
template <typename OffsetCoordinateType = py_offset_coordinate>
using py_inml_layout = py_cartesian_cell_layout<fiction::inml_technology, OffsetCoordinateType>;
/**
 * SiDB cell layout.
 */
template <typename OffsetCoordinateType = py_offset_coordinate>
using py_sidb_layout = py_cartesian_cell_layout<fiction::sidb_technology, OffsetCoordinateType>;
/**
 * SiDB lattice layout.
 */
template <typename LatticeOrientation, typename OffsetCoordinateType = py_offset_coordinate>
using py_sidb_lattice = fiction::sidb_lattice<LatticeOrientation, py_sidb_layout<OffsetCoordinateType>>;
/**
 * SiDB cell layout (with specified H-Si(100)-2x1 lattice orientation).
 */
template <typename OffsetCoordinateType = py_offset_coordinate>
using py_sidb_100_lattice = py_sidb_lattice<fiction::sidb_100_lattice, OffsetCoordinateType>;
/**
 * SiDB cell layout (with specified H-Si(111)-1x1 lattice orientation).
 */
template <typename OffsetCoordinateType = py_offset_coordinate>
using py_sidb_111_lattice = py_sidb_lattice<fiction::sidb_111_lattice, OffsetCoordinateType>;
/**
 * Charge distribution surface. This is a special SiDB cell-level layout that is used for the SiDB simulation
 * algorithms.
 */
template <typename Lyt>
using py_charge_distribution_surface_layout = fiction::charge_distribution_surface<Lyt>;
using py_charge_distribution_surface = fiction::charge_distribution_surface<py_sidb_layout<py_offset_coordinate>>;
/**
 * Charge distribution surface with underlying H-Si(100)-2x1 lattice.
 */
using py_charge_distribution_surface_100 =
    py_charge_distribution_surface_layout<py_sidb_100_lattice<py_offset_coordinate>>;
/**
 * Charge distribution surface with underlying H-Si(111)-1x1 lattice.
 */
using py_charge_distribution_surface_111 =
    py_charge_distribution_surface_layout<py_sidb_111_lattice<py_offset_coordinate>>;

}  // namespace pyfiction

#endif  // PYFICTION_TYPES_HPP
