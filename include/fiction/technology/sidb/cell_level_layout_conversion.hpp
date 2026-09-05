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

#include "fiction/layouts/coordinates.hpp"
#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/traits.hpp"

#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace fiction::sidb
{

/**
 * The lattice site a cell-level layout coordinate refers to. SiQAD coordinates map one-to-one; Cartesian coordinates
 * count single SiDB rows, so row `y` becomes unit cell `y / 2`, basis site `y mod 2`.
 *
 * @tparam Coordinate Coordinate type: `layouts::coords::offset`, `cube`, or `siqad`.
 * @param c Coordinate.
 * @return The lattice site of `c`.
 */
template <typename Coordinate>
[[nodiscard]] constexpr lattice_site to_lattice_site(const Coordinate& c)
{
    if constexpr (std::is_same_v<Coordinate, layouts::coords::siqad>)
    {
        return {c.x, c.y, c.z};
    }
    else
    {
        return site_at_row(static_cast<int32_t>(c.x), static_cast<int32_t>(c.y));
    }
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
 * The coordinate of a lattice site in a given SiDB cell-level layout type: the site itself for SiQAD coordinates,
 * the column and single-SiDB row otherwise.
 *
 * @tparam CellLyt SiDB cell-level layout type.
 * @param s Lattice site.
 * @return The cell of `CellLyt` at `s`.
 */
template <typename CellLyt>
[[nodiscard]] constexpr cell<CellLyt> to_cell(const lattice_site& s) noexcept
{
    if constexpr (has_siqad_coord_v<CellLyt>)
    {
        return {s.x, s.y, s.z};
    }
    else
    {
        return {s.x, row_of(s)};
    }
}
/**
 * The lattice a cell-level layout type is tagged with: H-Si(111)-1x1 for a `lattice<lattice_111, …>` layout and
 * H-Si(100)-2x1 for every other SiDB layout.
 *
 * @tparam CellLyt SiDB cell-level layout type.
 * @return The lattice of `CellLyt`.
 */
template <typename CellLyt>
[[nodiscard]] constexpr lattice lattice_of() noexcept
{
    static_assert(has_sidb_technology_v<CellLyt>, "CellLyt is not an SiDB layout");

    if constexpr (is_sidb_lattice_111_v<CellLyt>)
    {
        return lattice::si_111_1x1();
    }
    else
    {
        return lattice::si_100_2x1();
    }
}
/**
 * Converts an SiDB cell-level layout into an `sidb::layout` on the given lattice. Cell types, inputs, outputs, the
 * layout name, and surface defects carry over; cell names, cell modes, tile sizes, and clocking do not.
 *
 * @tparam CellLyt SiDB cell-level layout type.
 * @param lyt Layout to convert.
 * @param lat Lattice of the resulting layout.
 * @return The SiDB layout.
 */
template <typename CellLyt>
[[nodiscard]] layout to_sidb_layout(const CellLyt& lyt, const lattice& lat)
{
    static_assert(is_cell_level_layout_v<CellLyt>, "CellLyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<CellLyt>, "CellLyt is not an SiDB layout");

    layout result{lat, lyt.get_layout_name()};

    lyt.foreach_cell([&](const auto& c) { result.assign_cell_type(to_lattice_site(c), lyt.get_cell_type(c)); });

    if constexpr (is_sidb_defect_surface_v<CellLyt>)
    {
        lyt.foreach_sidb_defect([&](const auto& cd) { result.assign_defect(to_lattice_site(cd.first), cd.second); });
    }

    return result;
}
/**
 * Converts an SiDB cell-level layout into an `sidb::layout` on the lattice the layout type is tagged with, see
 * `lattice_of`.
 *
 * @tparam CellLyt SiDB cell-level layout type.
 * @param lyt Layout to convert.
 * @return The SiDB layout.
 */
template <typename CellLyt>
[[nodiscard]] layout to_sidb_layout(const CellLyt& lyt)
{
    return to_sidb_layout(lyt, lattice_of<CellLyt>());
}

/**
 * Converts a `sidb::layout` back into a Cartesian SiDB cell-level layout: cell types, inputs, outputs, the layout
 * name, and, for defect surfaces, the surface defects carry over. The layout's lattice is not represented in the
 * cell-level type and is dropped. This is the inverse of `to_sidb_layout` for the algorithms that still hand out
 * cell-level layouts.
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

    if constexpr (is_sidb_defect_surface_v<CellLyt>)
    {
        lyt.foreach_defect([&result](const auto& sd) { result.assign_defect(to_cell<CellLyt>(sd.first), sd.second); });
    }

    return result;
}

}  // namespace fiction::sidb
