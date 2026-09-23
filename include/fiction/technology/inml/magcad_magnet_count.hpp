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
 * @brief Counts the magnets of an iNML layout the way MagCAD does.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/technology/inml/layout.hpp"

#include <cstdint>

namespace fiction::inml
{
/**
 * Calculates the number of magnets for an iNML layout the way MagCAD (https://topolinano.polito.it/) would do it.
 * That is, counting chains of 4 inverters as a single entity.
 *
 * @param lyt The iNML layout whose magnets are counted.
 * @return Number of magnets as counted by MagCAD.
 */
[[nodiscard]] inline uint64_t magcad_magnet_count(const layout& lyt) noexcept
{
    uint64_t num_inv_cells{0ull};

    // count inverter magnets
    lyt.foreach_cell(
        [&lyt, &num_inv_cells](const auto& c)
        {
            if (lyt.get_cell_type(c) == magnet_type::INVERTER_MAGNET)
            {
                ++num_inv_cells;
            }
        });

    return lyt.num_cells() + (num_inv_cells / 4);
}

}  // namespace fiction::inml
