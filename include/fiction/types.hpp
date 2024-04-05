//
// Created by marcel on 18.05.21.
//

#ifndef FICTION_TYPES_HPP
#define FICTION_TYPES_HPP

#include "fiction/layouts/cartesian_layout.hpp"
#include "fiction/layouts/cell_level_layout.hpp"
#include "fiction/layouts/clocked_layout.hpp"
#include "fiction/layouts/coordinates.hpp"
#include "fiction/layouts/gate_level_layout.hpp"
#include "fiction/layouts/hexagonal_layout.hpp"
#include "fiction/layouts/shifted_cartesian_layout.hpp"
#include "fiction/layouts/synchronization_element_layout.hpp"
#include "fiction/layouts/tile_based_layout.hpp"
#include "fiction/networks/technology_network.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/sidb_defect_surface.hpp"
#include "fiction/technology/sidb_lattice.hpp"
#include "fiction/technology/sidb_lattice_orientations.hpp"

#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/io/write_dot.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/views/names_view.hpp>

#include <memory>
#include <type_traits>
#include <variant>

namespace fiction
{
/**
 * This file provides preset types that can be used as parameter types to run fiction's algorithms. They are furthermore
 * utilized in fiction's CLI. Naturally, this selection is neither complete nor final and can be extended at any time by
 * custom types.
 */

/**
 * Truth tables.
 */
using tt     = kitty::dynamic_truth_table;
using tt_ptr = std::shared_ptr<tt>;

// using truth_table_t = std:variant<tt_ptr>;
using truth_table_t = tt_ptr;

/**
 * Logic networks.
 */
using aig_nt  = mockturtle::names_view<mockturtle::aig_network>;
using aig_ptr = std::shared_ptr<aig_nt>;

inline constexpr const char* aig_name = "AIG";

using xag_nt  = mockturtle::names_view<mockturtle::xag_network>;
using xag_ptr = std::shared_ptr<xag_nt>;

inline constexpr const char* xag_name = "XAG";

using mig_nt  = mockturtle::names_view<mockturtle::mig_network>;
using mig_ptr = std::shared_ptr<mig_nt>;

inline constexpr const char* mig_name = "MIG";

using tec_nt  = mockturtle::names_view<fiction::technology_network>;
using tec_ptr = std::shared_ptr<tec_nt>;

inline constexpr const char* tec_name = "TEC";

using logic_network_t = std::variant<aig_ptr, xag_ptr, mig_ptr, tec_ptr>;

template <class Ntk>
inline constexpr const char* ntk_type_name = std::is_same_v<std::decay_t<Ntk>, aig_nt> ? aig_name :
                                             std::is_same_v<std::decay_t<Ntk>, xag_nt> ? xag_name :
                                             std::is_same_v<std::decay_t<Ntk>, mig_nt> ? mig_name :
                                             std::is_same_v<std::decay_t<Ntk>, tec_nt> ? tec_name :
                                                                                         "?";

/**
 * FCN gate-level layouts.
 */
using cart_gate_clk_lyt = gate_level_layout<
    synchronization_element_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>>;
using cart_gate_clk_lyt_ptr = std::shared_ptr<cart_gate_clk_lyt>;

using cart_odd_row_gate_clk_lyt =
    gate_level_layout<clocked_layout<tile_based_layout<shifted_cartesian_layout<offset::ucoord_t, odd_row_cartesian>>>>;
using cart_odd_row_gate_clk_lyt_ptr = std::shared_ptr<cart_odd_row_gate_clk_lyt>;

using cart_even_row_gate_clk_lyt = gate_level_layout<
    clocked_layout<tile_based_layout<shifted_cartesian_layout<offset::ucoord_t, even_row_cartesian>>>>;
using cart_even_row_gate_clk_lyt_ptr = std::shared_ptr<cart_even_row_gate_clk_lyt>;

using cart_odd_col_gate_clk_lyt = gate_level_layout<
    clocked_layout<tile_based_layout<shifted_cartesian_layout<offset::ucoord_t, odd_column_cartesian>>>>;
using cart_odd_col_gate_clk_lyt_ptr = std::shared_ptr<cart_odd_col_gate_clk_lyt>;

using cart_even_col_gate_clk_lyt = gate_level_layout<
    clocked_layout<tile_based_layout<shifted_cartesian_layout<offset::ucoord_t, even_column_cartesian>>>>;
using cart_even_col_gate_clk_lyt_ptr = std::shared_ptr<cart_even_col_gate_clk_lyt>;

using hex_odd_row_gate_clk_lyt =
    gate_level_layout<clocked_layout<tile_based_layout<hexagonal_layout<offset::ucoord_t, odd_row_hex>>>>;
using hex_odd_row_gate_clk_lyt_ptr = std::shared_ptr<hex_odd_row_gate_clk_lyt>;

using hex_even_row_gate_clk_lyt =
    gate_level_layout<clocked_layout<tile_based_layout<hexagonal_layout<offset::ucoord_t, even_row_hex>>>>;
using hex_even_row_gate_clk_lyt_ptr = std::shared_ptr<hex_even_row_gate_clk_lyt>;

using hex_odd_col_gate_clk_lyt =
    gate_level_layout<clocked_layout<tile_based_layout<hexagonal_layout<offset::ucoord_t, odd_column_hex>>>>;
using hex_odd_col_gate_clk_lyt_ptr = std::shared_ptr<hex_odd_col_gate_clk_lyt>;

using hex_even_col_gate_clk_lyt =
    gate_level_layout<clocked_layout<tile_based_layout<hexagonal_layout<offset::ucoord_t, even_column_hex>>>>;
using hex_even_col_gate_clk_lyt_ptr = std::shared_ptr<hex_even_col_gate_clk_lyt>;

using gate_layout_t =
    std::variant<cart_gate_clk_lyt_ptr, cart_odd_row_gate_clk_lyt_ptr, cart_even_row_gate_clk_lyt_ptr,
                 cart_odd_col_gate_clk_lyt_ptr, cart_even_col_gate_clk_lyt_ptr, hex_odd_row_gate_clk_lyt_ptr,
                 hex_even_row_gate_clk_lyt_ptr, hex_odd_col_gate_clk_lyt_ptr, hex_even_col_gate_clk_lyt_ptr>;

/**
 * FCN technologies.
 */
constexpr const char* qca_name  = "QCA";
constexpr const char* inml_name = "iNML";
constexpr const char* sidb_name = "SiDB";

template <class Tech>
inline constexpr const char* tech_impl_name = std::is_same_v<std::decay_t<Tech>, qca_technology>  ? qca_name :
                                              std::is_same_v<std::decay_t<Tech>, inml_technology> ? inml_name :
                                              std::is_same_v<std::decay_t<Tech>, sidb_technology> ? sidb_name :
                                                                                                    "?";

constexpr const char* qca_cell_name  = "cells";
constexpr const char* inml_cell_name = "magnets";
constexpr const char* sidb_cell_name = "dots";

template <class Tech>
inline constexpr const char* tech_cell_name = std::is_same_v<std::decay_t<Tech>, qca_technology>  ? qca_cell_name :
                                              std::is_same_v<std::decay_t<Tech>, inml_technology> ? inml_cell_name :
                                              std::is_same_v<std::decay_t<Tech>, sidb_technology> ? sidb_cell_name :
                                                                                                    "?";

/**
 * FCN cell-level layouts.
 */
using qca_cell_clk_lyt =
    cell_level_layout<qca_technology,
                      synchronization_element_layout  // se_layouts have only been investigated for QCA technologies
                      <clocked_layout<cartesian_layout<offset::ucoord_t>>>>;
using qca_cell_clk_lyt_ptr = std::shared_ptr<qca_cell_clk_lyt>;

using stacked_qca_cell_clk_lyt     = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<cube::coord_t>>>;
using stacked_qca_cell_clk_lyt_ptr = std::shared_ptr<stacked_qca_cell_clk_lyt>;

using inml_cell_clk_lyt     = cell_level_layout<inml_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;
using inml_cell_clk_lyt_ptr = std::shared_ptr<inml_cell_clk_lyt>;

using sidb_cell_clk_lyt     = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;
using sidb_cell_clk_lyt_ptr = std::shared_ptr<sidb_cell_clk_lyt>;

using sidb_cell_clk_lyt_siqad = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>;
using sidb_cell_clk_lyt_siqad_ptr = std::shared_ptr<sidb_cell_clk_lyt_siqad>;

using sidb_cell_clk_lyt_cube     = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<cube::coord_t>>>;
using sidb_cell_clk_lyt_cube_ptr = std::shared_ptr<sidb_cell_clk_lyt_cube>;

using sidb_100_cell_clk_lyt     = sidb_lattice<sidb_100_lattice, sidb_cell_clk_lyt>;
using sidb_100_cell_clk_lyt_ptr = std::shared_ptr<sidb_100_cell_clk_lyt>;

using sidb_100_cell_clk_lyt_siqad     = sidb_lattice<sidb_100_lattice, sidb_cell_clk_lyt_siqad>;
using sidb_100_cell_clk_lyt_siqad_ptr = std::shared_ptr<sidb_100_cell_clk_lyt_siqad>;

using sidb_100_cell_clk_lyt_cube     = sidb_lattice<sidb_100_lattice, sidb_cell_clk_lyt_cube>;
using sidb_100_cell_clk_lyt_cube_ptr = std::shared_ptr<sidb_100_cell_clk_lyt_cube>;

using sidb_111_cell_clk_lyt     = sidb_lattice<sidb_111_lattice, sidb_cell_clk_lyt>;
using sidb_111_cell_clk_lyt_ptr = std::shared_ptr<sidb_111_cell_clk_lyt>;

using sidb_111_cell_clk_lyt_siqad     = sidb_lattice<sidb_111_lattice, sidb_cell_clk_lyt_siqad>;
using sidb_111_cell_clk_lyt_siqad_ptr = std::shared_ptr<sidb_111_cell_clk_lyt>;

using sidb_111_cell_clk_lyt_cube     = sidb_lattice<sidb_111_lattice, sidb_cell_clk_lyt_cube>;
using sidb_111_cell_clk_lyt_cube_ptr = std::shared_ptr<sidb_111_cell_clk_lyt_cube>;

using cds_sidb_100_cell_clk_lyt     = charge_distribution_surface<sidb_100_cell_clk_lyt>;
using cds_sidb_100_cell_clk_lyt_ptr = std::shared_ptr<cds_sidb_100_cell_clk_lyt>;

using cds_sidb_cell_clk_lyt     = charge_distribution_surface<sidb_cell_clk_lyt>;
using cds_sidb_cell_clk_lyt_ptr = std::shared_ptr<cds_sidb_cell_clk_lyt>;

using cds_sidb_cell_clk_lyt_siqad     = charge_distribution_surface<sidb_cell_clk_lyt_siqad>;
using cds_sidb_cell_clk_lyt_siqad_ptr = std::shared_ptr<cds_sidb_cell_clk_lyt_siqad>;

using cds_sidb_cell_clk_lyt_cube     = charge_distribution_surface<sidb_cell_clk_lyt_siqad>;
using cds_sidb_cell_clk_lyt_cube_ptr = std::shared_ptr<cds_sidb_cell_clk_lyt_cube>;

using cds_sidb_100_cell_clk_lyt_siqad     = charge_distribution_surface<sidb_100_cell_clk_lyt_siqad>;
using cds_sidb_100_cell_clk_lyt_siqad_ptr = std::shared_ptr<cds_sidb_100_cell_clk_lyt_siqad>;

using cds_sidb_100_cell_clk_lyt_cube     = charge_distribution_surface<sidb_100_cell_clk_lyt_cube>;
using cds_sidb_100_cell_clk_lyt_cube_ptr = std::shared_ptr<cds_sidb_100_cell_clk_lyt_cube>;

using cds_sidb_111_cell_clk_lyt     = charge_distribution_surface<sidb_111_cell_clk_lyt>;
using cds_sidb_111_cell_clk_lyt_ptr = std::shared_ptr<cds_sidb_111_cell_clk_lyt>;

using cds_sidb_111_cell_clk_lyt_siqad     = charge_distribution_surface<sidb_111_cell_clk_lyt_siqad>;
using cds_sidb_111_cell_clk_lyt_siqad_ptr = std::shared_ptr<sidb_111_cell_clk_lyt_siqad>;

using cds_sidb_111_cell_clk_lyt_cube     = charge_distribution_surface<sidb_111_cell_clk_lyt_cube>;
using cds_sidb_111_cell_clk_lyt_cube_ptr = std::shared_ptr<sidb_111_cell_clk_lyt_cube>;

using sidb_defect_cell_clk_lyt     = sidb_defect_surface<sidb_cell_clk_lyt>;
using sidb_defect_cell_clk_lyt_ptr = std::shared_ptr<sidb_defect_cell_clk_lyt>;

using sidb_defect_cell_clk_lyt_siqad     = sidb_defect_surface<sidb_cell_clk_lyt_siqad>;
using sidb_defect_cell_clk_lyt_siqad_ptr = std::shared_ptr<sidb_defect_cell_clk_lyt_siqad>;

using sidb_defect_cell_clk_lyt_cube     = sidb_defect_surface<sidb_cell_clk_lyt_cube>;
using sidb_defect_cell_clk_lyt_cube_ptr = std::shared_ptr<sidb_defect_cell_clk_lyt_cube>;

using sidb_defect_100_cell_clk_lyt     = sidb_defect_surface<sidb_100_cell_clk_lyt>;
using sidb_defect_100_cell_clk_lyt_ptr = std::shared_ptr<sidb_defect_100_cell_clk_lyt>;

using sidb_defect_100_cell_clk_lyt_siqad     = sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>;
using sidb_defect_100_cell_clk_lyt_siqad_ptr = std::shared_ptr<sidb_defect_100_cell_clk_lyt_siqad>;

using sidb_defect_100_cell_clk_lyt_cube     = sidb_defect_surface<sidb_100_cell_clk_lyt_cube>;
using sidb_defect_100_cell_clk_lyt_cube_ptr = std::shared_ptr<sidb_defect_100_cell_clk_lyt_cube>;

using cds_sidb_defect_100_cell_clk_lyt     = charge_distribution_surface<sidb_defect_surface<sidb_100_cell_clk_lyt>>;
using cds_sidb_defect_100_cell_clk_lyt_ptr = std::shared_ptr<cds_sidb_defect_100_cell_clk_lyt>;

using cds_sidb_defect_100_cell_clk_lyt_siqad =
    charge_distribution_surface<sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>>;
using cds_sidb_defect_100_cell_clk_lyt_siqad_ptr = std::shared_ptr<cds_sidb_defect_100_cell_clk_lyt_siqad>;

using cds_sidb_defect_100_cell_clk_lyt_cube =
    charge_distribution_surface<sidb_defect_surface<sidb_100_cell_clk_lyt_cube>>;
using cds_sidb_defect_100_cell_clk_lyt_cube_ptr = std::shared_ptr<cds_sidb_defect_100_cell_clk_lyt_cube>;

using cell_layout_t =
    std::variant<qca_cell_clk_lyt_ptr, stacked_qca_cell_clk_lyt_ptr, inml_cell_clk_lyt_ptr, sidb_100_cell_clk_lyt_ptr,
                 sidb_111_cell_clk_lyt_ptr, cds_sidb_100_cell_clk_lyt_ptr, cds_sidb_111_cell_clk_lyt_ptr>;

}  // namespace fiction

#endif  // FICTION_TYPES_HPP
