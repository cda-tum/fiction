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
#include <fiction/layouts/synchronization_element_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/technology/cell_technologies.hpp>

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
using py_coordinate = fiction::offset::ucoord_t;
/**
 * Cartesian layout.
 */
using py_cartesian_layout = fiction::cartesian_layout<py_coordinate>;
/**
 * Hexagonal layout.
 */
using py_hexagonal_layout = fiction::hexagonal_layout<py_coordinate, fiction::even_row_hex>;
/**
 * Cartesian clocked layout.
 */
using py_cartesian_clocked_layout =
    fiction::synchronization_element_layout<fiction::clocked_layout<fiction::tile_based_layout<py_cartesian_layout>>>;
/**
 * Hexagonal clocked layout.
 */
using py_hexagonal_clocked_layout =
    fiction::synchronization_element_layout<fiction::clocked_layout<fiction::tile_based_layout<py_hexagonal_layout>>>;
/**
 * Cartesian gate layout.
 */
using py_cartesian_gate_layout = fiction::gate_level_layout<py_cartesian_clocked_layout>;
/**
 * Hexagonal gate layout.
 */
using py_hexagonal_gate_layout = fiction::gate_level_layout<py_hexagonal_clocked_layout>;
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

}  // namespace pyfiction

#endif  // PYFICTION_TYPES_HPP
