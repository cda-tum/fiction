//
// Created by marcel on 18.05.21.
//

#ifndef FICTION_TYPES_HPP
#define FICTION_TYPES_HPP

#include "layouts/cartesian_layout.hpp"
#include "layouts/cell_level_layout.hpp"
#include "layouts/clocked_layout.hpp"
#include "layouts/gate_level_layout.hpp"
#include "layouts/synchronization_element_layout.hpp"
#include "layouts/tile_based_layout.hpp"
#include "networks/topology_network.hpp"
#include "technology/cell_technologies.hpp"

#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/io/write_dot.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/views/names_view.hpp>

#include <memory>
#include <type_traits>
#include <variant>

namespace fiction
{
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

constexpr const char* aig_name = "AIG";

using mig_nt  = mockturtle::names_view<mockturtle::mig_network>;
using mig_ptr = std::shared_ptr<mig_nt>;

constexpr const char* mig_name = "MIG";

using top_nt  = mockturtle::names_view<fiction::topology_network>;
using top_ptr = std::shared_ptr<top_nt>;

constexpr const char* top_name = "TOP";

using logic_network_t = std::variant<aig_ptr, mig_ptr, top_ptr>;

template <class Ntk>
inline constexpr const char* ntk_type_name = std::is_same_v<std::decay_t<Ntk>, aig_nt> ? aig_name :
                                             std::is_same_v<std::decay_t<Ntk>, mig_nt> ? mig_name :
                                             std::is_same_v<std::decay_t<Ntk>, top_nt> ? top_name :
                                                                                         "?";

/**
 * FCN gate-level layouts.
 */

using gate_clk_lyt = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<cartesian::ucoord_t>>>>;

using gate_clk_lyt_ptr = std::shared_ptr<gate_clk_lyt>;

using gate_layout_t = std::variant<gate_clk_lyt_ptr>;

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

/**
 * FCN cell-level layouts.
 */
using qca_cell_clk_lyt =
    cell_level_layout<qca_technology,
                      synchronization_element_layout  // se_layouts have only been investigated for QCA technologies
                      <clocked_layout<cartesian_layout<cartesian::ucoord_t>>>>;
using qca_cell_clk_lyt_ptr = std::shared_ptr<qca_cell_clk_lyt>;

using inml_cell_clk_lyt     = cell_level_layout<inml_technology, clocked_layout<cartesian_layout<cartesian::ucoord_t>>>;
using inml_cell_clk_lyt_ptr = std::shared_ptr<inml_cell_clk_lyt>;

using sidb_cell_clk_lyt     = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<cartesian::ucoord_t>>>;
using sidb_cell_clk_lyt_ptr = std::shared_ptr<sidb_cell_clk_lyt>;

using cell_layout_t = std::variant<qca_cell_clk_lyt_ptr, inml_cell_clk_lyt_ptr, sidb_cell_clk_lyt_ptr>;

}  // namespace fiction

#endif  // FICTION_TYPES_HPP
