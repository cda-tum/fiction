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
#include <stdexcept>
#include <utility>

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
 * The cube coordinate of a lattice site in a Cartesian SiDB cell-level layout, the inverse of `to_lattice_site`.
 *
 * @param s Lattice site.
 * @return Cube coordinate at column `x` and single-SiDB row `2 * y + z`.
 * @throws std::out_of_range if the row exceeds the cube coordinate range.
 */
[[nodiscard]] constexpr layouts::coords::cube to_cube(const lattice_site& s)
{
    if (!std::in_range<int32_t>(row_of(s)))
    {
        throw std::out_of_range("Lattice-site row exceeds the cube coordinate range");
    }
    return {s.x, row_of(s), 0};
}
/**
 * The coordinate of a lattice site in a given Cartesian SiDB cell-level layout type: the column and the single-SiDB
 * row.
 *
 * @tparam CellLyt SiDB cell-level layout type.
 * @param s Lattice site.
 * @return The cell of `CellLyt` at `s`.
 */
template <typename CellLyt>
[[nodiscard]] constexpr cell<CellLyt> to_cell(const lattice_site& s) noexcept
{
    return {s.x, row_of(s)};
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

    lyt.foreach_cell([&](const auto& c) { result.assign_cell_type(to_lattice_site(c), lyt.get_cell_type(c)); });

    return result;
}
/**
 * Converts an `sidb::layout` into a Cartesian SiDB cell-level layout: cell types, inputs, outputs, and the layout name
 * carry over. The layout's lattice and its defects are not represented in the cell-level type and are dropped. This is
 * the inverse of `to_sidb_layout`.
 *
 * @tparam CellLyt SiDB cell-level layout type to create.
 * @param lyt The layout to convert.
 * @return The cell-level layout.
 */
template <typename CellLyt>
[[nodiscard]] CellLyt to_cell_level_layout(const layout& lyt)
{
    static_assert(is_cell_level_layout_v<CellLyt>, "CellLyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<CellLyt>, "CellLyt is not an SiDB layout");

    CellLyt result{};

    if constexpr (has_set_layout_name_v<CellLyt>)
    {
        result.set_layout_name(lyt.get_layout_name());
    }

    lyt.foreach_cell([&result, &lyt](const auto& s)
                     { result.assign_cell_type(to_cell<CellLyt>(s), lyt.get_cell_type(s)); });

    return result;
}

}  // namespace fiction::sidb
