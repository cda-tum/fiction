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
 * @brief The QCA layout: QCA cells with modes on a ground and a crossing layer, clocked by tiles.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/layouts/cell_grid.hpp"
#include "fiction/layouts/clocking_state.hpp"
#include "fiction/layouts/tile_clocking.hpp"

#include <phmap.h>

#include <cstdint>
#include <string>
#include <utility>

namespace fiction::qca
{

/**
 * Types of Quantum-dot Cellular Automata (QCA) cells. The enumerators carry the symbols of their ASCII representation.
 */
enum class cell_type : uint8_t
{
    /**
     * No cell.
     */
    EMPTY = ' ',
    /**
     * Regular cell.
     */
    NORMAL = 'x',
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
 * Whether a QCA cell has a fixed polarization.
 *
 * @param ct Cell type.
 * @return `true` iff `ct` is `CONST_0` or `CONST_1`.
 */
[[nodiscard]] constexpr bool is_constant(const cell_type ct) noexcept
{
    return ct == cell_type::CONST_0 || ct == cell_type::CONST_1;
}
/**
 * Modes of QCA cells as known from QCADesigner.
 */
enum class cell_mode : uint8_t
{
    /**
     * Regular cell.
     */
    NORMAL = 0u,
    /**
     * Cell rotated by 45°.
     */
    ROTATED,
    /**
     * Via cell that connects the ground and the crossing layer.
     */
    VERTICAL,
    /**
     * Cell of a multilayer crossing.
     */
    CROSSOVER
};

/**
 * A QCA layout: QCA cells on a Cartesian grid with a ground layer (`z = 0`) and a crossing layer (`z = 1`). Cells carry
 * a type, a mode, and, for inputs and outputs, a name. Clock zones are tiles of cells, and each clock zone can be a
 * synchronization element that extends its Hold phase. The layout has value semantics; copies are independent.
 */
class layout : public layouts::cell_grid<cell_type>, public layouts::tile_clocking
{
  public:
    /**
     * Hold-phase extension in full clock cycles.
     */
    using sync_elem_t = typename layouts::clocking::state<clock_zone>::sync_elem_t;
    /**
     * Cell width in nm, the QCADesigner default.
     */
    static constexpr double CELL_WIDTH = 18.0;
    /**
     * Cell height in nm, the QCADesigner default.
     */
    static constexpr double CELL_HEIGHT = 18.0;
    /**
     * Horizontal spacing between two cells in nm, the QCADesigner default.
     */
    static constexpr double CELL_HSPACE = 2.0;
    /**
     * Vertical spacing between two cells in nm, the QCADesigner default.
     */
    static constexpr double CELL_VSPACE = 2.0;
    /**
     * Creates an empty layout with open clocking.
     *
     * @param ar Highest cell position; `ar.z = 1` enables the crossing layer.
     * @param name Layout name.
     * @param tile_size_x Clock-zone width in cells.
     * @param tile_size_y Clock-zone height in cells.
     * @throws std::invalid_argument if either clock-zone dimension is zero.
     */
    explicit layout(const aspect_ratio& ar = {}, std::string name = "", const uint16_t tile_size_x = 1u,
                    const uint16_t tile_size_y = 1u) :
            cell_grid{ar, std::move(name)},
            tile_clocking{tile_size_x, tile_size_y}
    {}
    /**
     * Creates an empty layout clocked by the given scheme.
     *
     * @param ar Highest cell position; `ar.z = 1` enables the crossing layer.
     * @param scheme Clocking scheme over clock zones.
     * @param name Layout name.
     * @param tile_size_x Clock-zone width in cells.
     * @param tile_size_y Clock-zone height in cells.
     * @throws std::invalid_argument if either clock-zone dimension is zero.
     */
    layout(const aspect_ratio& ar, const clocking_scheme_t& scheme, std::string name = "",
           const uint16_t tile_size_x = 1u, const uint16_t tile_size_y = 1u) :
            layout{ar, std::move(name), tile_size_x, tile_size_y}
    {
        replace_clocking_scheme(scheme);
    }
    /**
     * Compares two layouts: same cells, names, modes, tile size, and clocking scheme name.
     *
     * @param other Right-hand side layout.
     * @return `true` iff both layouts are identical.
     */
    [[nodiscard]] bool operator==(const layout& other) const noexcept
    {
        return cell_grid::operator==(other) && tile_clocking::operator==(other) && cell_modes == other.cell_modes;
    }
    /**
     * Assigns a cell type to a position. Assigning `EMPTY` removes the cell with its name and mode.
     *
     * @param c Cell position.
     * @param ct Cell type.
     */
    void assign_cell_type(const cell& c, const cell_type ct)
    {
        if (ct == cell_type::EMPTY)
        {
            cell_modes.erase(c);
        }

        cell_grid::assign_cell_type(c, ct);
    }
    /**
     * Assigns a mode to a cell. `cell_mode::NORMAL` removes a stored mode.
     *
     * @param c Cell position.
     * @param m Cell mode.
     */
    void assign_cell_mode(const cell& c, const cell_mode m)
    {
        if (m == cell_mode::NORMAL)
        {
            cell_modes.erase(c);
        }
        else
        {
            cell_modes[c] = m;
        }
    }
    /**
     * The mode of a cell.
     *
     * @param c Cell position.
     * @return Mode of the cell at `c`, `cell_mode::NORMAL` if none is stored.
     */
    [[nodiscard]] cell_mode get_cell_mode(const cell& c) const noexcept
    {
        if (const auto it = cell_modes.find(c); it != cell_modes.cend())
        {
            return it->second;
        }

        return cell_mode::NORMAL;
    }
    /**
     * Turns a clock zone into a synchronization element.
     *
     * @param cz Clock zone.
     * @param se Number of full clock cycles to extend the Hold phase of `cz` by; 0 turns `cz` back into a normal clock
     * zone.
     */
    void assign_synchronization_element(const clock_zone& cz, const sync_elem_t se) noexcept
    {
        clocking.assign_synchronization_element(cz, se);
    }
    /**
     * Whether the clock zone that contains a cell is a synchronization element.
     *
     * @param c Cell position.
     * @return `true` iff `get_clock_zone(c)` is a synchronization element.
     */
    [[nodiscard]] bool is_synchronization_element(const cell& c) const noexcept
    {
        return clocking.is_synchronization_element(get_clock_zone(c));
    }
    /**
     * The Hold-phase extension of the clock zone that contains a cell.
     *
     * @param c Cell position.
     * @return Hold-phase extension of `get_clock_zone(c)` in full clock cycles.
     */
    [[nodiscard]] sync_elem_t get_synchronization_element(const cell& c) const noexcept
    {
        return clocking.get_synchronization_element(get_clock_zone(c));
    }
    /**
     * Number of synchronization elements.
     *
     * @return Number of clock zones with a nonzero Hold-phase extension.
     */
    [[nodiscard]] uint32_t num_se() const noexcept
    {
        return clocking.num_se();
    }

  private:
    /**
     * Modes of the cells whose mode is not `cell_mode::NORMAL`.
     */
    phmap::flat_hash_map<cell, cell_mode> cell_modes{};
};

}  // namespace fiction::qca
