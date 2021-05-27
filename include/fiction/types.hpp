//
// Created by marcel on 18.05.21.
//

#ifndef FICTION_TYPES_HPP
#define FICTION_TYPES_HPP

#include "layouts/clocked_layout.hpp"
#include "layouts/gate_level_layout.hpp"
#include "layouts/tile_based_layout.hpp"
#include "networks/topology_network.hpp"

#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/io/write_dot.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/views/names_view.hpp>

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
inline constexpr const char* ntk_type_name = std::is_same_v<Ntk, fiction::aig_nt> ? fiction::aig_name :
                                             std::is_same_v<Ntk, fiction::mig_nt> ? fiction::mig_name :
                                             std::is_same_v<Ntk, fiction::top_nt> ? fiction::top_name :
                                                                                    "?";

/**
 * FCN gate-level layouts.
 */
using tile_clk_lyt =
    mockturtle::names_view<fiction::gate_level_layout<fiction::clocked_layout<fiction::tile_based_layout>>>;
using tile_clk_lyt_ptr = std::shared_ptr<tile_clk_lyt>;

using gate_layout_t = std::variant<tile_clk_lyt_ptr>;

}  // namespace fiction

#endif  // FICTION_TYPES_HPP
