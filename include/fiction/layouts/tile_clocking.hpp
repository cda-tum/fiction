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
 * @brief Tile-based clock zones for cell grids whose clocking electrodes each govern a tile of cells.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/layouts/clocking_state.hpp"
#include "fiction/layouts/coordinates.hpp"

#include <cstdint>
#include <stdexcept>
#include <string_view>

namespace fiction::layouts
{

/**
 * Clock zones of a cell grid. A clock zone is a tile, i.e., a region of `get_tile_size_x()` by `get_tile_size_y()`
 * cells that one clock signal governs on every layer. Clock zones are addressed by their tile position on layer 0, and
 * every clock query takes a cell and looks up the zone that `get_clock_zone` returns.
 *
 * Fabricable designs group cells into tiles large enough to be addressed by individual clocking electrodes buried in
 * the substrate. Tiles of \f$1 \times 1\f$ cells are most likely not fabricable.
 *
 * Layouts of technologies with tile-based clocking derive from this class next to `cell_grid`.
 */
class tile_clocking
{
  public:
    /**
     * Coordinate identifying a clock zone, i.e., a tile.
     */
    using clock_zone = coords::offset;
    /**
     * Clocking scheme over clock zones.
     */
    using clocking_scheme_t = clocking::scheme;
    /**
     * Clock phase index.
     */
    using clock_number_t = typename clocking_scheme_t::clock_number;
    /**
     * Creates open clocking with four clocks and tiles of the given size.
     *
     * @param tile_size_x Tile width in cells.
     * @param tile_size_y Tile height in cells.
     * @throws std::invalid_argument if either dimension is zero.
     */
    explicit tile_clocking(const uint16_t tile_size_x = 1u, const uint16_t tile_size_y = 1u) :
            tile_x{checked(tile_size_x)},
            tile_y{checked(tile_size_y)}
    {}
    /**
     * Compares tile sizes and clocking scheme names. Overridden clock numbers and synchronization elements are not
     * compared.
     *
     * @param other Right-hand side clocking.
     * @return `true` iff both have the same tile size and scheme name.
     */
    [[nodiscard]] bool operator==(const tile_clocking& other) const noexcept
    {
        return tile_x == other.tile_x && tile_y == other.tile_y &&
               clocking.get_clocking_scheme().name() == other.clocking.get_clocking_scheme().name();
    }
    /**
     * Tile width in cells.
     *
     * @return Tile width.
     */
    [[nodiscard]] uint16_t get_tile_size_x() const noexcept
    {
        return tile_x;
    }
    /**
     * Tile height in cells.
     *
     * @return Tile height.
     */
    [[nodiscard]] uint16_t get_tile_size_y() const noexcept
    {
        return tile_y;
    }
    /**
     * Sets the tile width.
     *
     * @param tile_size_x Tile width in cells.
     * @throws std::invalid_argument if `tile_size_x` is zero.
     */
    void set_tile_size_x(const uint16_t tile_size_x)
    {
        tile_x = checked(tile_size_x);
    }
    /**
     * Sets the tile height.
     *
     * @param tile_size_y Tile height in cells.
     * @throws std::invalid_argument if `tile_size_y` is zero.
     */
    void set_tile_size_y(const uint16_t tile_size_y)
    {
        tile_y = checked(tile_size_y);
    }
    /**
     * The clock zone that contains a cell: its tile on layer 0.
     *
     * @param c Cell position.
     * @return Clock zone of `c`.
     */
    [[nodiscard]] clock_zone get_clock_zone(const coords::offset& c) const noexcept
    {
        return {c.x / tile_x, c.y / tile_y};
    }
    /**
     * Replaces the clocking scheme.
     *
     * @param scheme New clocking scheme over clock zones.
     */
    void replace_clocking_scheme(const clocking_scheme_t& scheme) noexcept
    {
        clocking.replace_clocking_scheme(scheme);
    }
    /**
     * Overrides the clock number of a clock zone.
     *
     * @param cz Clock zone.
     * @param cn Clock number.
     */
    void assign_clock_number(const clock_zone& cz, const clock_number_t cn) noexcept
    {
        clocking.assign_clock_number(cz, cn);
    }
    /**
     * The clock number of the clock zone that contains a cell.
     *
     * @param c Cell position.
     * @return Clock number of `get_clock_zone(c)`.
     */
    [[nodiscard]] clock_number_t get_clock_number(const coords::offset& c) const noexcept
    {
        return clocking.get_clock_number(get_clock_zone(c));
    }
    /**
     * Number of clock phases.
     *
     * @return Number of clock phases.
     */
    [[nodiscard]] clock_number_t num_clocks() const noexcept
    {
        return clocking.num_clocks();
    }
    /**
     * Whether a regular scheme clocks the layout with no overridden zones.
     *
     * @return `true` iff the clocking is regular.
     */
    [[nodiscard]] bool is_regularly_clocked() const noexcept
    {
        return clocking.is_regularly_clocked();
    }
    /**
     * Compares the clocking scheme against a name. Predefined names are constants in `fiction::layouts::clocking`.
     *
     * @param name Clocking scheme name.
     * @return `true` iff the scheme is called `name`.
     */
    [[nodiscard]] bool is_clocking_scheme(const std::string_view& name) const noexcept
    {
        return clocking.is_clocking_scheme(name);
    }
    /**
     * A copy of the clocking scheme.
     *
     * @return Clocking scheme.
     */
    [[nodiscard]] clocking_scheme_t get_clocking_scheme() const noexcept
    {
        return clocking.get_clocking_scheme();
    }

  protected:
    /**
     * Scheme, overridden clock numbers, and synchronization delays per clock zone.
     */
    clocking::state<clock_zone> clocking{clocking::open()};

  private:
    /**
     * Tile width in cells.
     */
    uint16_t tile_x;
    /**
     * Tile height in cells.
     */
    uint16_t tile_y;
    /**
     * Returns `size` if it is nonzero.
     */
    static uint16_t checked(const uint16_t size)
    {
        if (size == 0)
        {
            throw std::invalid_argument("Clock-zone dimensions must be positive");
        }

        return size;
    }
};

}  // namespace fiction::layouts
