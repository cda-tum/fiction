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
 * @brief Preset network and truth-table types that instantiate the algorithms.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 * @author Benjamin Hien (hibenj)
 */

#pragma once

#include "fiction/networks/technology_network.hpp"

#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/views/names_view.hpp>

#include <memory>
#include <type_traits>

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

using truth_table_t = tt_ptr;

/**
 * Logic networks.
 */
using aig_nt  = mockturtle::names_view<mockturtle::aig_network>;
using aig_ptr = std::shared_ptr<aig_nt>;

using xag_nt  = mockturtle::names_view<mockturtle::xag_network>;
using xag_ptr = std::shared_ptr<xag_nt>;

using mig_nt  = mockturtle::names_view<mockturtle::mig_network>;
using mig_ptr = std::shared_ptr<mig_nt>;

using tec_nt  = mockturtle::names_view<fiction::networks::technology_network>;
using tec_ptr = std::shared_ptr<tec_nt>;

/**
 * Every `*_ptr` alias points at the type its name says (`aig_ptr` at `aig_nt`, and so on). The
 * aliases above are written by hand, so this pins each pair.
 */
static_assert(std::is_same_v<tt_ptr::element_type, tt>);
static_assert(std::is_same_v<aig_ptr::element_type, aig_nt>);
static_assert(std::is_same_v<xag_ptr::element_type, xag_nt>);
static_assert(std::is_same_v<mig_ptr::element_type, mig_nt>);
static_assert(std::is_same_v<tec_ptr::element_type, tec_nt>);

}  // namespace fiction
