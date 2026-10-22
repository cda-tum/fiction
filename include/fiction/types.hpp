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
#include "fiction/synthesis/types.hpp"

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/views/names_view.hpp>

namespace fiction::networks
{

/**
 * @brief AND-inverter graph with network, signal, and output names.
 */
using named_aig_network = mockturtle::names_view<mockturtle::aig_network>;

/**
 * @brief XOR-AND-inverter graph with network, signal, and output names.
 */
using named_xag_network = mockturtle::names_view<mockturtle::xag_network>;

/**
 * @brief Majority-inverter graph with network, signal, and output names.
 */
using named_mig_network = mockturtle::names_view<mockturtle::mig_network>;

/**
 * @brief Technology network with network, signal, and output names.
 */
using named_technology_network = mockturtle::names_view<technology_network>;

}  // namespace fiction::networks
