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
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 * @author Willem Lambooy (wlambooy)
 */

#pragma once

#include "fiction/layouts/arrangement.hpp"
#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/technology/fcn/cell_ports.hpp"
#include "fiction/traits.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <optional>
#include <random>
#include <stdexcept>
#include <utility>

namespace fiction::layouts
{

/**
 * Rejects a missing arrangement for gate-level layout types that need one.
 *
 * @tparam Lyt Gate-level layout type.
 * @param a Arrangement of the shifted rows or columns the caller provides.
 * @throws std::invalid_argument If `Lyt` is a shifted Cartesian or hexagonal layout and `a` is empty.
 */
template <typename Lyt>
void require_arrangement([[maybe_unused]] const std::optional<arrangement>& a)
{
    if constexpr (!is_cartesian_layout_v<Lyt>)
    {
        if (!a.has_value())
        {
            throw std::invalid_argument("An arrangement is required for shifted Cartesian and hexagonal layouts");
        }
    }
}

/**
 * Creates an empty gate-level layout of type `Lyt`. Cartesian layouts ignore the arrangement.
 *
 * @tparam Lyt Gate-level layout type.
 * @param a Arrangement of the shifted rows or columns. Shifted Cartesian and hexagonal layouts require it.
 * @param ar Highest possible position in the layout.
 * @param scheme Clocking scheme to apply to the layout.
 * @return Empty layout.
 * @throws std::invalid_argument If `Lyt` is a shifted Cartesian or hexagonal layout and `a` is empty.
 */
template <typename Lyt>
[[nodiscard]] Lyt make_gate_level_layout([[maybe_unused]] const std::optional<arrangement>& a,
                                         const typename Lyt::aspect_ratio& ar, const clocking::scheme& scheme)
{
    require_arrangement<Lyt>(a);

    if constexpr (is_cartesian_layout_v<Lyt>)
    {
        return Lyt{ar, scheme};
    }
    else
    {
        return Lyt{*a, ar, scheme};
    }
}

/**
 * Returns the number of adjacent coordinates of a given one. This is not a constant value because `c` could be located
 * at a layout border.
 *
 * @tparam Lyt Layout type.
 * @param lyt Layout.
 * @param c Coordinate whose number of adjacencies are required.
 * @return Number of `c`'s adjacent coordinates.
 */
template <typename Lyt>
[[nodiscard]] uint8_t num_adjacent_coordinates(const Lyt& lyt, const coordinate<Lyt>& c) noexcept
{
    static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");

    return static_cast<uint8_t>(lyt.adjacent_coordinates(c).size());
}

/**
 * Converts a relative cell position within a tile to an absolute cell position within a layout. To compute the absolute
 * position, the layout topology is taken into account.
 *
 * @tparam GateSizeX Horizontal tile size.
 * @tparam GateSizeY Vertical tile size.
 * @tparam GateLyt Gate-level layout type.
 * @tparam Coordinate Cell coordinate type, e.g., `layout_base::coordinate`. Hexagonal tiles can yield negative
 * positions.
 * @param gate_lyt The gate-level layout whose tiles are to be considered.
 * @param t Tile within gate_lyt.
 * @param relative_c Relative cell position within t.
 * @return Absolute cell position in a layout.
 * @throws std::invalid_argument If the relative cell lies outside the tile.
 * @throws std::overflow_error If the absolute cell is outside the signed 32-bit coordinate range.
 */
template <uint16_t GateSizeX, uint16_t GateSizeY, typename GateLyt, typename Coordinate>
[[nodiscard]] Coordinate relative_to_absolute_cell_position(const GateLyt& gate_lyt, const tile<GateLyt>& t,
                                                            const Coordinate& relative_c)
{
    static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt is not a gate-level layout");

    if (relative_c.x < 0 || relative_c.x >= GateSizeX || relative_c.y < 0 || relative_c.y >= GateSizeY)
    {
        throw std::invalid_argument("The relative cell must be within the bounds of a single tile");
    }

    int64_t x = static_cast<int64_t>(t.x) * GateSizeX;
    int64_t y = static_cast<int64_t>(t.y) * GateSizeY;

    // Cartesian layouts
    if constexpr (is_cartesian_layout_v<GateLyt>)
    {
        // Cartesian tiles use the full gate size in both directions.
    }
    // shifted Cartesian and hexagonal layouts
    else if constexpr (is_shifted_cartesian_layout_v<GateLyt> || is_hexagonal_layout_v<GateLyt>)
    {
        // hexagons nest into each other, so their tiles are 3/4 as far apart perpendicular to the shift
        constexpr auto step_x = is_hexagonal_layout_v<GateLyt> ? GateSizeX * 3 / 4 : GateSizeX;
        constexpr auto step_y = is_hexagonal_layout_v<GateLyt> ? GateSizeY * 3 / 4 : GateSizeY;

        const auto a   = gate_lyt.get_arrangement();
        const auto odd = is_odd_arrangement(a);

        if (is_row_arrangement(a))
        {
            y = static_cast<int64_t>(t.y) * step_y;

            if (odd ? gate_lyt.is_in_odd_row(t) : gate_lyt.is_in_even_row(t))
            {
                // shifted rows move in by width / 2
                x += static_cast<int64_t>(static_cast<double>(GateSizeX) / 2.0);
            }
        }
        else
        {
            x = static_cast<int64_t>(t.x) * step_x;

            if (odd ? gate_lyt.is_in_odd_column(t) : gate_lyt.is_in_even_column(t))
            {
                // shifted columns move in by height / 2
                y += static_cast<int64_t>(static_cast<double>(GateSizeY) / 2.0);
            }
        }
    }
    // more gate-level layout types go here
    else
    {
        assert(false && "unknown gate-level layout type");
    }

    return Coordinate{x + relative_c.x, y + relative_c.y, t.z};
}

/**
 * Port directions address coordinates relative to each other by specifying cardinal directions. This function converts
 * such a relative direction to an absolute coordinate when given a layout and a coordinate therein to consider. That
 * is, when presented with, e.g., a `NORTH_EAST` direction, it will return the coordinate that is to the `NORTH_EAST` of
 * the given coordinate `c` in the layout `lyt`.
 *
 * @tparam Lyt Coordinate layout type.
 * @param lyt Coordinate layout.
 * @param c Coordinate to consider.
 * @param port Port direction.
 * @return Absolute coordinate specified by a coordinate `c` in layout `lyt` and a port direction.
 */
template <typename Lyt>
[[nodiscard]] coordinate<Lyt> port_direction_to_coordinate(const Lyt& lyt, const coordinate<Lyt>& c,
                                                           const fcn::port_direction& port) noexcept
{
    static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");

    switch (port.dir)
    {
        case fcn::port_direction::cardinal::NORTH:
        {
            return lyt.north(c);
        }
        case fcn::port_direction::cardinal::NORTH_EAST:
        {
            return lyt.north_east(c);
        }
        case fcn::port_direction::cardinal::EAST:
        {
            return lyt.east(c);
        }
        case fcn::port_direction::cardinal::SOUTH_EAST:
        {
            return lyt.south_east(c);
        }
        case fcn::port_direction::cardinal::SOUTH:
        {
            return lyt.south(c);
        }
        case fcn::port_direction::cardinal::SOUTH_WEST:
        {
            return lyt.south_west(c);
        }
        case fcn::port_direction::cardinal::WEST:
        {
            return lyt.west(c);
        }
        case fcn::port_direction::cardinal::NORTH_WEST:
        {
            return lyt.north_west(c);
        }
        default:
        {
            assert(false && "Given port does not specify a cardinal direction");
        }
    }

    return {};
}

/**
 * Returns a copy of the given cell grid layout whose cells are shifted towards the origin, so that the smallest
 * occupied x- and y-coordinates become 0. Cell types, names, and, where the layout has them, cell modes move with their
 * cells; layers, the layout name, and the clocking stay unchanged. The dimensions shrink by the shift.
 *
 * @tparam Lyt Cell grid layout type, e.g., `qca::layout`, `mol_qca::layout`, or `inml::layout`.
 * @param lyt The layout to normalize.
 * @return Normalized copy of `lyt`.
 */
template <typename Lyt>
[[nodiscard]] Lyt normalize_layout_coordinates(const Lyt& lyt)
{
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

    if (lyt.is_empty())
    {
        return lyt;
    }

    auto x_offset = lyt.x();
    auto y_offset = lyt.y();

    lyt.foreach_cell(
        [&x_offset, &y_offset](const auto& c)
        {
            x_offset = std::min(x_offset, static_cast<decltype(x_offset)>(c.x));
            y_offset = std::min(y_offset, static_cast<decltype(y_offset)>(c.y));
        });

    constexpr bool has_cell_modes =
        requires(Lyt& l, const coordinate<Lyt>& c) { l.assign_cell_mode(c, l.get_cell_mode(c)); };

    Lyt normalized{lyt};

    lyt.foreach_cell([&normalized](const auto& c) { normalized.assign_cell_type(c, Lyt::cell_type::EMPTY); });

    normalized.resize({lyt.x() - x_offset, lyt.y() - y_offset, lyt.z()});

    lyt.foreach_cell(
        [&normalized, &lyt, x_offset, y_offset](const auto& c)
        {
            const coordinate<Lyt> shifted{c.x - x_offset, c.y - y_offset, c.z};

            normalized.assign_cell_type(shifted, lyt.get_cell_type(c));
            normalized.assign_cell_name(shifted, lyt.get_cell_name(c));

            if constexpr (has_cell_modes)
            {
                normalized.assign_cell_mode(shifted, lyt.get_cell_mode(c));
            }
        });

    return normalized;
}
/**
 * Generates a random coordinate within the region spanned by two given coordinates. The two given coordinates form the
 * top left corner and the bottom right corner of the spanned region.
 *
 * @tparam CoordinateType The coordinate implementation to be used.
 * @param coordinate1 Top left Coordinate.
 * @param coordinate2 Bottom right Coordinate (coordinate order is not important, automatically swapped if
 * necessary).
 * @return Randomly generated coordinate.
 */
template <typename CoordinateType>
CoordinateType random_coordinate(CoordinateType coordinate1, CoordinateType coordinate2) noexcept
{
    static std::mt19937_64 generator(std::random_device{}());

    if (coordinate1 > coordinate2)
    {
        std::swap(coordinate1, coordinate2);
    }

    std::uniform_int_distribution<> dist_x(coordinate1.x, coordinate2.x);
    std::uniform_int_distribution<> dist_y(coordinate1.y, coordinate2.y);
    std::uniform_int_distribution<> dist_z(coordinate1.z, coordinate2.z);

    return {dist_x(generator), dist_y(generator), dist_z(generator)};
}

}  // namespace fiction::layouts
