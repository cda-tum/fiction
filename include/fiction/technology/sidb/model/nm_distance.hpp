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
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/technology/sidb/model/nm_position.hpp"
#include "fiction/traits.hpp"

#include <cmath>

namespace fiction::sidb::model
{
/**
 * Computes the distance between two SiDB cells in nanometers (unit: nm).
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param source The source cell.
 * @param target The target cell.
 * @return The distance between the two cells in nanometers (unit: nm).
 */
template <typename Lyt>
[[nodiscard]] constexpr double nm_distance(const Lyt& lyt, const coordinate<Lyt>& source,
                                           const coordinate<Lyt>& target) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not based on SiDB technology");

    if (source == target)
    {
        return 0.0;
    }

    const auto pos_c1 = nm_position<Lyt>(lyt, source);
    const auto pos_c2 = nm_position<Lyt>(lyt, target);

    const auto x = pos_c1.first - pos_c2.first;
    const auto y = pos_c1.second - pos_c2.second;

    return std::hypot(x, y);
}

}  // namespace fiction::sidb::model
