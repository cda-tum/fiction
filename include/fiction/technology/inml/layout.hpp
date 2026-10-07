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
 * @brief The iNML layout: planar in-plane nanomagnets, clocked by tiles.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/layouts/cell_grid.hpp"
#include "fiction/layouts/tile_clocking.hpp"

#include <cstdint>
#include <string>
#include <utility>

namespace fiction::inml
{

/**
 * Types of in-plane Nanomagnet Logic (iNML) magnets. The enumerators carry the symbols of their ASCII representation.
 */
enum class magnet_type : uint8_t
{
    /**
     * No magnet.
     */
    EMPTY = ' ',
    /**
     * Regular magnet.
     */
    NORMAL = 'x',
    /**
     * Primary input magnet.
     */
    INPUT = 'i',
    /**
     * Primary output magnet.
     */
    OUTPUT = 'o',
    /**
     * Magnet with an upper slanted edge.
     */
    SLANTED_EDGE_UP_MAGNET = 'u',
    /**
     * Magnet with a lower slanted edge.
     */
    SLANTED_EDGE_DOWN_MAGNET = 'd',
    /**
     * Inverter magnet.
     */
    INVERTER_MAGNET = 'n',
    /**
     * Magnet of a coplanar cross wire.
     */
    CROSSWIRE_MAGNET = 'c',
    /**
     * Coupler (fan-out) magnet.
     */
    FANOUT_COUPLER_MAGNET = 'f'
};

/**
 * An iNML layout: in-plane nanomagnets on a planar Cartesian grid. Each position holds a magnet type and, for inputs
 * and outputs, a name. Clock zones are tiles of magnets. Signal crossings are coplanar cross-wire magnets, so the
 * layout has no crossing layer. The layout has value semantics; copies are independent.
 */
class layout : public layouts::cell_grid<magnet_type>, public layouts::tile_clocking
{
  public:
    /**
     * Magnet width in nm, the NMLSim default.
     */
    static constexpr double CELL_WIDTH = 50.0;
    /**
     * Magnet height in nm, the NMLSim default.
     */
    static constexpr double CELL_HEIGHT = 100.0;
    /**
     * Horizontal spacing between two magnets in nm, the NMLSim default.
     */
    static constexpr double CELL_HSPACE = 10.0;
    /**
     * Vertical spacing between two magnets in nm, the NMLSim default.
     */
    static constexpr double CELL_VSPACE = 25.0;
    /**
     * Creates an empty layout with open clocking.
     *
     * @param ar Highest magnet position; its z-coordinate is ignored because the layout is planar.
     * @param name Layout name.
     * @param tile_size_x Clock-zone width in magnets.
     * @param tile_size_y Clock-zone height in magnets.
     * @throws std::invalid_argument if either clock-zone dimension is zero.
     */
    explicit layout(const aspect_ratio& ar = {}, std::string name = "", const uint16_t tile_size_x = 1u,
                    const uint16_t tile_size_y = 1u) :
            cell_grid{aspect_ratio{ar.x, ar.y, 0}, std::move(name)},
            tile_clocking{tile_size_x, tile_size_y}
    {}
    /**
     * Creates an empty layout clocked by the given scheme.
     *
     * @param ar Highest magnet position; its z-coordinate is ignored because the layout is planar.
     * @param scheme Clocking scheme over clock zones.
     * @param name Layout name.
     * @param tile_size_x Clock-zone width in magnets.
     * @param tile_size_y Clock-zone height in magnets.
     * @throws std::invalid_argument if either clock-zone dimension is zero.
     */
    layout(const aspect_ratio& ar, const clocking_scheme_t& scheme, std::string name = "",
           const uint16_t tile_size_x = 1u, const uint16_t tile_size_y = 1u) :
            layout{ar, std::move(name), tile_size_x, tile_size_y}
    {
        replace_clocking_scheme(scheme);
    }
    /**
     * Compares two layouts: same magnets, names, tile size, and clocking scheme name.
     *
     * @param other Right-hand side layout.
     * @return `true` iff both layouts are identical.
     */
    [[nodiscard]] bool operator==(const layout& other) const noexcept
    {
        return cell_grid::operator==(other) && tile_clocking::operator==(other);
    }
};

}  // namespace fiction::inml
