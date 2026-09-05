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
 * @brief Preset network, layout, and technology types that instantiate the algorithms.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 * @author Benjamin Hien (hibenj)
 */

#pragma once

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
#include "fiction/technology/inml/technology.hpp"
#include "fiction/technology/qca/technology.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/simulation/result.hpp"
#include "fiction/technology/sidb/technology.hpp"

#include <kitty/dynamic_truth_table.hpp>
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

inline constexpr auto aig_name = "AIG";

using xag_nt  = mockturtle::names_view<mockturtle::xag_network>;
using xag_ptr = std::shared_ptr<xag_nt>;

inline constexpr auto xag_name = "XAG";

using mig_nt  = mockturtle::names_view<mockturtle::mig_network>;
using mig_ptr = std::shared_ptr<mig_nt>;

inline constexpr auto mig_name = "MIG";

using tec_nt  = mockturtle::names_view<fiction::networks::technology_network>;
using tec_ptr = std::shared_ptr<tec_nt>;

inline constexpr auto tec_name = "TEC";

using logic_network_t = std::variant<aig_ptr, xag_ptr, mig_ptr, tec_ptr>;

// NOLINTBEGIN(readability-avoid-nested-conditional-operator)
template <class Ntk>
constexpr const char* get_ntk_type_name()
{
    if constexpr (std::is_same_v<std::decay_t<Ntk>, aig_nt>)
    {
        return aig_name;
    }
    else if constexpr (std::is_same_v<std::decay_t<Ntk>, xag_nt>)
    {
        return xag_name;
    }
    else if constexpr (std::is_same_v<std::decay_t<Ntk>, mig_nt>)
    {
        return mig_name;
    }
    else if constexpr (std::is_same_v<std::decay_t<Ntk>, tec_nt>)
    {
        return tec_name;
    }
    else
    {
        return "?";
    }
}

template <class Ntk>
inline constexpr auto ntk_type_name = get_ntk_type_name<Ntk>();

/**
 * FCN gate-level layouts.
 */
using cart_gate_clk_lyt     = layouts::gate_level_layout<layouts::synchronization_element_layout<
    layouts::clocked_layout<layouts::tile_based_layout<layouts::cartesian_layout<layouts::coords::offset>>>>>;
using cart_gate_clk_lyt_ptr = std::shared_ptr<cart_gate_clk_lyt>;

using cart_odd_row_gate_clk_lyt     = layouts::gate_level_layout<layouts::clocked_layout<layouts::tile_based_layout<
    layouts::shifted_cartesian_layout<layouts::coords::offset, layouts::odd_row_cartesian>>>>;
using cart_odd_row_gate_clk_lyt_ptr = std::shared_ptr<cart_odd_row_gate_clk_lyt>;

using cart_even_row_gate_clk_lyt     = layouts::gate_level_layout<layouts::clocked_layout<layouts::tile_based_layout<
    layouts::shifted_cartesian_layout<layouts::coords::offset, layouts::even_row_cartesian>>>>;
using cart_even_row_gate_clk_lyt_ptr = std::shared_ptr<cart_even_row_gate_clk_lyt>;

using cart_odd_col_gate_clk_lyt     = layouts::gate_level_layout<layouts::clocked_layout<layouts::tile_based_layout<
    layouts::shifted_cartesian_layout<layouts::coords::offset, layouts::odd_column_cartesian>>>>;
using cart_odd_col_gate_clk_lyt_ptr = std::shared_ptr<cart_odd_col_gate_clk_lyt>;

using cart_even_col_gate_clk_lyt     = layouts::gate_level_layout<layouts::clocked_layout<layouts::tile_based_layout<
    layouts::shifted_cartesian_layout<layouts::coords::offset, layouts::even_column_cartesian>>>>;
using cart_even_col_gate_clk_lyt_ptr = std::shared_ptr<cart_even_col_gate_clk_lyt>;

using hex_odd_row_gate_clk_lyt     = layouts::gate_level_layout<layouts::clocked_layout<
    layouts::tile_based_layout<layouts::hexagonal_layout<layouts::coords::offset, layouts::odd_row_hex>>>>;
using hex_odd_row_gate_clk_lyt_ptr = std::shared_ptr<hex_odd_row_gate_clk_lyt>;

using hex_even_row_gate_clk_lyt     = layouts::gate_level_layout<layouts::clocked_layout<
    layouts::tile_based_layout<layouts::hexagonal_layout<layouts::coords::offset, layouts::even_row_hex>>>>;
using hex_even_row_gate_clk_lyt_ptr = std::shared_ptr<hex_even_row_gate_clk_lyt>;

using hex_odd_col_gate_clk_lyt     = layouts::gate_level_layout<layouts::clocked_layout<
    layouts::tile_based_layout<layouts::hexagonal_layout<layouts::coords::offset, layouts::odd_column_hex>>>>;
using hex_odd_col_gate_clk_lyt_ptr = std::shared_ptr<hex_odd_col_gate_clk_lyt>;

using hex_even_col_gate_clk_lyt     = layouts::gate_level_layout<layouts::clocked_layout<
    layouts::tile_based_layout<layouts::hexagonal_layout<layouts::coords::offset, layouts::even_column_hex>>>>;
using hex_even_col_gate_clk_lyt_ptr = std::shared_ptr<hex_even_col_gate_clk_lyt>;

using gate_layout_t =
    std::variant<cart_gate_clk_lyt_ptr, cart_odd_row_gate_clk_lyt_ptr, cart_even_row_gate_clk_lyt_ptr,
                 cart_odd_col_gate_clk_lyt_ptr, cart_even_col_gate_clk_lyt_ptr, hex_odd_row_gate_clk_lyt_ptr,
                 hex_even_row_gate_clk_lyt_ptr, hex_odd_col_gate_clk_lyt_ptr, hex_even_col_gate_clk_lyt_ptr>;

/**
 * FCN technologies.
 */
inline constexpr auto qca_name     = "QCA";
inline constexpr auto mol_qca_name = "molQCA";
inline constexpr auto inml_name    = "iNML";
inline constexpr auto sidb_name    = "SiDB";

template <class Tech>
constexpr const char* get_tech_impl_name()
{
    if constexpr (std::is_same_v<std::decay_t<Tech>, qca::qca_technology>)
    {
        return qca_name;
    }
    else if constexpr (std::is_same_v<std::decay_t<Tech>, qca::mol_qca_technology>)
    {
        return mol_qca_name;
    }
    else if constexpr (std::is_same_v<std::decay_t<Tech>, inml::inml_technology>)
    {
        return inml_name;
    }
    else if constexpr (std::is_same_v<std::decay_t<Tech>, sidb::sidb_technology>)
    {
        return sidb_name;
    }
    else
    {
        return "?";
    }
}

template <class Tech>
inline constexpr auto tech_impl_name = get_tech_impl_name<Tech>();

inline constexpr auto qca_cell_name     = "cells";
inline constexpr auto mol_qca_cell_name = "cells";
inline constexpr auto inml_cell_name    = "magnets";
inline constexpr auto sidb_cell_name    = "dots";

template <class Tech>
constexpr const char* get_tech_cell_name()
{
    if constexpr (std::is_same_v<std::decay_t<Tech>, qca::qca_technology>)
    {
        return qca_cell_name;
    }
    else if constexpr (std::is_same_v<std::decay_t<Tech>, qca::mol_qca_technology>)
    {
        return mol_qca_cell_name;
    }
    else if constexpr (std::is_same_v<std::decay_t<Tech>, inml::inml_technology>)
    {
        return inml_cell_name;
    }
    else if constexpr (std::is_same_v<std::decay_t<Tech>, sidb::sidb_technology>)
    {
        return sidb_cell_name;
    }
    else
    {
        return "?";
    }
}

template <class Tech>
inline constexpr const char* tech_cell_name = get_tech_cell_name<Tech>();

/**
 * FCN cell-level layouts.
 */
using qca_cell_clk_lyt =
    layouts::cell_level_layout<qca::qca_technology,
                               layouts::synchronization_element_layout  // se_layouts have only been investigated for
                                                                        // QCA technologies
                               <layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>>>;
using qca_cell_clk_lyt_ptr = std::shared_ptr<qca_cell_clk_lyt>;

using stacked_qca_cell_clk_lyt =
    layouts::cell_level_layout<qca::qca_technology,
                               layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::cube>>>;
using stacked_qca_cell_clk_lyt_ptr = std::shared_ptr<stacked_qca_cell_clk_lyt>;

using mol_qca_cell_clk_lyt =
    layouts::cell_level_layout<qca::mol_qca_technology,
                               layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>>;
using mol_qca_cell_clk_lyt_ptr = std::shared_ptr<mol_qca_cell_clk_lyt>;

using inml_cell_clk_lyt =
    layouts::cell_level_layout<inml::inml_technology,
                               layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>>;
using inml_cell_clk_lyt_ptr = std::shared_ptr<inml_cell_clk_lyt>;

using sidb_cell_clk_lyt =
    layouts::cell_level_layout<sidb::sidb_technology,
                               layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>>;
using sidb_cell_clk_lyt_ptr = std::shared_ptr<sidb_cell_clk_lyt>;

using sidb_cell_clk_lyt_cube =
    layouts::cell_level_layout<sidb::sidb_technology,
                               layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::cube>>>;
using sidb_cell_clk_lyt_cube_ptr = std::shared_ptr<sidb_cell_clk_lyt_cube>;
/**
 * SiDB layout over a crystal lattice.
 */
using sidb_layout_ptr = std::shared_ptr<sidb::layout>;
/**
 * Result of an SiDB simulation: the simulated layout plus its physically valid charge distributions.
 */
using sidb_sim_result_ptr = std::shared_ptr<sidb::simulation::result>;

using cell_layout_t = std::variant<qca_cell_clk_lyt_ptr, stacked_qca_cell_clk_lyt_ptr, mol_qca_cell_clk_lyt_ptr,
                                   inml_cell_clk_lyt_ptr, sidb_layout_ptr, sidb_sim_result_ptr>;

/**
 * Every `*_ptr` alias points at the type its name says (`aig_ptr` at `aig_nt`, and so on). The
 * aliases above are written by hand, so this pins each pair.
 */
static_assert(std::is_same_v<tt_ptr::element_type, tt>);
static_assert(std::is_same_v<aig_ptr::element_type, aig_nt>);
static_assert(std::is_same_v<xag_ptr::element_type, xag_nt>);
static_assert(std::is_same_v<mig_ptr::element_type, mig_nt>);
static_assert(std::is_same_v<tec_ptr::element_type, tec_nt>);
static_assert(std::is_same_v<cart_gate_clk_lyt_ptr::element_type, cart_gate_clk_lyt>);
static_assert(std::is_same_v<cart_odd_row_gate_clk_lyt_ptr::element_type, cart_odd_row_gate_clk_lyt>);
static_assert(std::is_same_v<cart_even_row_gate_clk_lyt_ptr::element_type, cart_even_row_gate_clk_lyt>);
static_assert(std::is_same_v<cart_odd_col_gate_clk_lyt_ptr::element_type, cart_odd_col_gate_clk_lyt>);
static_assert(std::is_same_v<cart_even_col_gate_clk_lyt_ptr::element_type, cart_even_col_gate_clk_lyt>);
static_assert(std::is_same_v<hex_odd_row_gate_clk_lyt_ptr::element_type, hex_odd_row_gate_clk_lyt>);
static_assert(std::is_same_v<hex_even_row_gate_clk_lyt_ptr::element_type, hex_even_row_gate_clk_lyt>);
static_assert(std::is_same_v<hex_odd_col_gate_clk_lyt_ptr::element_type, hex_odd_col_gate_clk_lyt>);
static_assert(std::is_same_v<hex_even_col_gate_clk_lyt_ptr::element_type, hex_even_col_gate_clk_lyt>);
static_assert(std::is_same_v<qca_cell_clk_lyt_ptr::element_type, qca_cell_clk_lyt>);
static_assert(std::is_same_v<stacked_qca_cell_clk_lyt_ptr::element_type, stacked_qca_cell_clk_lyt>);
static_assert(std::is_same_v<mol_qca_cell_clk_lyt_ptr::element_type, mol_qca_cell_clk_lyt>);
static_assert(std::is_same_v<inml_cell_clk_lyt_ptr::element_type, inml_cell_clk_lyt>);
static_assert(std::is_same_v<sidb_cell_clk_lyt_ptr::element_type, sidb_cell_clk_lyt>);
static_assert(std::is_same_v<sidb_cell_clk_lyt_cube_ptr::element_type, sidb_cell_clk_lyt_cube>);
static_assert(std::is_same_v<sidb_layout_ptr::element_type, sidb::layout>);
static_assert(std::is_same_v<sidb_sim_result_ptr::element_type, sidb::simulation::result>);

}  // namespace fiction
