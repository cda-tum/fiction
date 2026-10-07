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
 * @brief The molQCA layout: planar molecular QCA cells whose types carry their clock phase.
 * @author Marcel Walter (marcelwa)
 * @author Benjamin Hien (hibenj)
 */

#pragma once

#include "fiction/layouts/cell_grid.hpp"

#include <cstdint>
#include <string>
#include <utility>

namespace fiction::mol_qca
{

/**
 * Types of molecular Quantum-dot Cellular Automata (molQCA) cells. The enumerators carry the symbols of their ASCII
 * representation. A regular cell is clocked individually: `NORMAL1` to `NORMAL4` place it in clock phase 0 to 3, which
 * is how the SCERPA simulator addresses clock regions and how SIM(7) gates arrange several clock regions in one tile.
 */
enum class cell_type : uint8_t
{
    /**
     * No cell.
     */
    EMPTY = ' ',
    /**
     * Regular cell in clock phase 0.
     */
    NORMAL1 = 'a',
    /**
     * Regular cell in clock phase 1.
     */
    NORMAL2 = 'b',
    /**
     * Regular cell in clock phase 2.
     */
    NORMAL3 = 'c',
    /**
     * Regular cell in clock phase 3.
     */
    NORMAL4 = 'd',
    /**
     * Primary input cell.
     */
    INPUT = 'i',
    /**
     * Primary output cell.
     */
    OUTPUT = 'o',
    /**
     * Cell with a fixed polarization of logic 0.
     */
    CONST_0 = '0',
    /**
     * Cell with a fixed polarization of logic 1.
     */
    CONST_1 = '1'
};
/**
 * Whether a molQCA cell is a regular cell of any clock phase.
 *
 * @param ct Cell type.
 * @return `true` iff `ct` is one of `NORMAL1` to `NORMAL4`.
 */
[[nodiscard]] constexpr bool is_normal(const cell_type ct) noexcept
{
    return ct == cell_type::NORMAL1 || ct == cell_type::NORMAL2 || ct == cell_type::NORMAL3 || ct == cell_type::NORMAL4;
}
/**
 * Whether a molQCA cell has a fixed polarization.
 *
 * @param ct Cell type.
 * @return `true` iff `ct` is `CONST_0` or `CONST_1`.
 */
[[nodiscard]] constexpr bool is_constant(const cell_type ct) noexcept
{
    return ct == cell_type::CONST_0 || ct == cell_type::CONST_1;
}
/**
 * The clock phase of a regular molQCA cell.
 *
 * @param ct Cell type.
 * @return Clock phase 0 to 3 of `NORMAL1` to `NORMAL4`; 0 for every other type.
 */
[[nodiscard]] constexpr uint8_t clock_number(const cell_type ct) noexcept
{
    switch (ct)
    {
        case cell_type::NORMAL2: return 1u;
        case cell_type::NORMAL3: return 2u;
        case cell_type::NORMAL4: return 3u;
        default: return 0u;
    }
}

/**
 * A molQCA layout: molecular QCA cells on a planar Cartesian grid. Each cell carries a type, which includes the clock
 * phase of regular cells, and, for inputs and outputs, a name. The layout has no crossing layer, no cell modes, and no
 * tile-based clocking, since molQCA crossings are coplanar and every cell names its own clock phase. The layout has
 * value semantics; copies are independent.
 */
class layout : public layouts::cell_grid<cell_type>
{
  public:
    /**
     * Cell width in nm, as in SIM(7).
     */
    static constexpr double CELL_WIDTH = 2.0;
    /**
     * Cell height in nm, as in SIM(7).
     */
    static constexpr double CELL_HEIGHT = 2.0;
    /**
     * Horizontal spacing between two cells in nm.
     */
    static constexpr double CELL_HSPACE = 0.0;
    /**
     * Vertical spacing between two cells in nm.
     */
    static constexpr double CELL_VSPACE = 0.0;
    /**
     * Creates an empty layout.
     *
     * @param ar Highest cell position; its z-coordinate is ignored because the layout is planar.
     * @param name Layout name.
     */
    explicit layout(const aspect_ratio& ar = {}, std::string name = "") :
            cell_grid{aspect_ratio{ar.x, ar.y, 0}, std::move(name)}
    {}
    /**
     * Compares two layouts: same dimensions, cells, and names.
     *
     * @param other Right-hand side layout.
     * @return `true` iff both layouts are identical.
     */
    [[nodiscard]] bool operator==(const layout& other) const noexcept = default;
};

}  // namespace fiction::mol_qca
