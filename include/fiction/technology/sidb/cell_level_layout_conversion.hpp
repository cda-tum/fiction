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
 * @brief Conversion between Cartesian SiDB cell-level layouts and `sidb::layout`, the boundary to physical design.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/traits.hpp"

#include <cstdint>

namespace fiction::sidb
{

/**
 * The lattice site a Cartesian cell-level layout coordinate refers to: the coordinate counts single SiDB rows, so row
 * `y` becomes unit cell `y / 2`, basis site `y mod 2`.
 *
 * @tparam Coordinate Coordinate type: `layouts::coords::offset` or `layouts::coords::cube`.
 * @param c Coordinate.
 * @return The lattice site of `c`.
 */
template <typename Coordinate>
[[nodiscard]] constexpr lattice_site to_lattice_site(const Coordinate& c)
{
    return site_at_row(static_cast<int32_t>(c.x), static_cast<int32_t>(c.y));
}
/**
 * Converts a Cartesian SiDB cell-level layout, as produced by placement and routing, into an `sidb::layout` on the
 * given lattice. Cell types, inputs, outputs, and the layout name carry over; cell names, cell modes, tile sizes, and
 * clocking do not.
 *
 * @tparam CellLyt SiDB cell-level layout type.
 * @param lyt Layout to convert.
 * @param lat Lattice of the resulting layout; H-Si(100) 2x1 by default.
 * @return The SiDB layout.
 */
template <typename CellLyt>
[[nodiscard]] layout to_sidb_layout(const CellLyt& lyt, const lattice& lat = lattice::si_100_2x1())
{
    static_assert(is_cell_level_layout_v<CellLyt>, "CellLyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<CellLyt>, "CellLyt is not an SiDB layout");

    layout result{lat, lyt.get_layout_name()};

    lyt.foreach_cell([&](const auto& c) { result.assign_sidb(to_lattice_site(c), lyt.get_cell_type(c)); });

    return result;
}

}  // namespace fiction::sidb
