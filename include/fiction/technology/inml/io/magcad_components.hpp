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
 * @brief The MagCAD components that iNML magnets form, shared by the QCC and QLL writers.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/technology/inml/layout.hpp"

#include <array>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>

namespace fiction::inml::io::detail
{

/**
 * Names of the MagCAD components, indexed by the values of `COMPONENT_SELECTOR`.
 */
inline constexpr const std::array<const char*, 6> COMPONENTS{"Magnet", "Coupler",  "Cross Wire",
                                                             "And",    "Inverter", "Or"};
/**
 * The MagCAD component, as an index into `COMPONENTS`, that a magnet type starts.
 */
inline const std::unordered_map<magnet_type, uint8_t> COMPONENT_SELECTOR{
    {magnet_type::NORMAL, 0},           {magnet_type::INPUT, 0},
    {magnet_type::OUTPUT, 0},           {magnet_type::FANOUT_COUPLER_MAGNET, 1},
    {magnet_type::CROSSWIRE_MAGNET, 2}, {magnet_type::SLANTED_EDGE_DOWN_MAGNET, 3},
    {magnet_type::INVERTER_MAGNET, 4},  {magnet_type::SLANTED_EDGE_UP_MAGNET, 5}};
/**
 * Adds to `skip` the magnets that belong to the MagCAD component starting at `c`, so that a writer that scans the
 * layout row by row emits every component once. The positions follow the ToPoliNano gate library.
 *
 * @param type Type of the magnet at `c`.
 * @param c Position of the magnet that starts the component.
 * @param skip Positions to skip.
 */
inline void skip_component_magnets(const magnet_type type, const layout::cell& c,
                                   std::unordered_set<layout::cell>& skip)
{
    // AND and OR structures span the next two magnets in southern direction
    if (type == magnet_type::SLANTED_EDGE_UP_MAGNET || type == magnet_type::SLANTED_EDGE_DOWN_MAGNET)
    {
        skip.insert({c.x, c.y + 1});
        skip.insert({c.x, c.y + 2});
    }
    // couplers span the magnets of the fan-out structure
    else if (type == magnet_type::FANOUT_COUPLER_MAGNET)
    {
        skip.insert({c.x, c.y + 1});
        skip.insert({c.x, c.y + 2});
        skip.insert({c.x + 1, c.y});
        skip.insert({c.x + 1, c.y + 2});
    }
    // cross wires span the magnets of the crossing structure
    else if (type == magnet_type::CROSSWIRE_MAGNET)
    {
        skip.insert({c.x + 2, c.y});
        skip.insert({c.x, c.y + 2});
        skip.insert({c.x + 1, c.y + 1});
        skip.insert({c.x + 2, c.y + 2});
    }
    // inverters are single structures of 4 magnets in the library
    else if (type == magnet_type::INVERTER_MAGNET)
    {
        skip.insert({c.x + 1, c.y});
        skip.insert({c.x + 2, c.y});
        skip.insert({c.x + 3, c.y});
    }
}

}  // namespace fiction::inml::io::detail
