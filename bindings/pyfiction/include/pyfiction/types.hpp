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
using py_cartesian_layout = fiction::cartesian_layout<py_offset_coordinate>;
/**
 * Shifted Cartesian layout.
 */
using py_shifted_cartesian_layout =
    fiction::shifted_cartesian_layout<py_offset_coordinate, fiction::odd_column_cartesian>;
/**
 * Hexagonal layout.
 */
using py_hexagonal_layout = fiction::hexagonal_layout<py_offset_coordinate, fiction::even_row_hex>;
/**
 * Cartesian clocked layout.
 */
using py_cartesian_clocked_layout = fiction::clocked_layout<fiction::tile_based_layout<py_cartesian_layout>>;
/**
 * Shifted Cartesian clocked layout.
 */
using py_shifted_cartesian_clocked_layout =
    fiction::clocked_layout<fiction::tile_based_layout<py_shifted_cartesian_layout>>;
/**
 * Hexagonal clocked layout.
 */
using py_hexagonal_clocked_layout = fiction::clocked_layout<fiction::tile_based_layout<py_hexagonal_layout>>;
/**
 * Cartesian gate layout.
 */
using py_cartesian_gate_layout = fiction::gate_level_layout<py_cartesian_clocked_layout>;
/**
 * Shifted Cartesian gate layout.
 */
using py_shifted_cartesian_gate_layout = fiction::gate_level_layout<py_shifted_cartesian_clocked_layout>;
/**
 * Hexagonal gate layout.
 */
using py_hexagonal_gate_layout = fiction::gate_level_layout<py_hexagonal_clocked_layout>;
/**
 * Cartesian gate-level obstruction layout.
 */
using py_cartesian_obstruction_layout = fiction::obstruction_layout<py_cartesian_gate_layout>;
/**
 * Shifted Cartesian gate-level obstruction layout.
 */
using py_shifted_cartesian_obstruction_layout = fiction::obstruction_layout<py_shifted_cartesian_gate_layout>;
/**
 * Hexagonal gate-level obstruction layout.
 */
using py_hexagonal_obstruction_layout = fiction::obstruction_layout<py_hexagonal_gate_layout>;
/**
 * Cartesian cell layout.
 */
template <typename Technology>
using py_cartesian_cell_layout = fiction::cell_level_layout<Technology, py_cartesian_clocked_layout>;
/**
 * QCA cell layout.
 */
using py_qca_layout = py_cartesian_cell_layout<fiction::qca_technology>;
/**
 * iNML cell layout.
 */
using py_inml_layout = py_cartesian_cell_layout<fiction::inml_technology>;
/**
 * SiDB cell layout.
 */
using py_sidb_layout = py_cartesian_cell_layout<fiction::sidb_technology>;
/**
 * Charge distribution surface. This is a special SiDB cell-level layout that is used for the SiDB simulation
 * algorithms. It is based on SiQAD coordinates.
 */
using py_charge_distribution_surface = fiction::charge_distribution_surface<fiction::cell_level_layout<
    fiction::sidb_technology, fiction::clocked_layout<fiction::cartesian_layout<fiction::siqad::coord_t>>>>;
/**
 * The base type of py_charge_distribution_surface to prevent double inference of charge_distribution_surface in the
 * template parameter of many functions, which would yield a type not registered in pyfiction. This type is not
 * explicitly exposed. Don't use it on its own elsewhere unless you know what you are doing.
 */
using py_cds_base =
    fiction::cell_level_layout<fiction::sidb_technology,
                               fiction::clocked_layout<fiction::cartesian_layout<fiction::siqad::coord_t>>>;

}  // namespace pyfiction

#endif  // PYFICTION_TYPES_HPP
