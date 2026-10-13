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
 * @brief Coordinate conversion, normalization, and sampling helpers.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 * @author Willem Lambooy (wlambooy)
 */

#pragma once

#include "fiction/layouts/arrangement.hpp"
#include "fiction/layouts/cartesian_layout.hpp"
#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/layouts/gate_level_layout.hpp"
#include "fiction/layouts/hexagonal_layout.hpp"
#include "fiction/layouts/layout_base.hpp"
#include "fiction/layouts/shifted_cartesian_layout.hpp"
#include "fiction/technology/fcn/cell_ports.hpp"

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstdint>
#include <limits>
#include <optional>
#include <random>
#include <stdexcept>

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
    if constexpr (!std::derived_from<Lyt, fiction::layouts::cartesian_layout>)
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
 * @param ex Axis sizes of the layout.
 * @param scheme Clocking scheme to apply to the layout.
 * @return Empty layout.
 * @throws std::invalid_argument If `Lyt` is a shifted Cartesian or hexagonal layout and `a` is empty.
 */
template <typename Lyt>
[[nodiscard]] Lyt make_gate_level_layout([[maybe_unused]] const std::optional<arrangement>& a,
                                         const typename Lyt::extent& ex, const clocking::scheme& scheme)
{
    require_arrangement<Lyt>(a);

    if constexpr (std::derived_from<Lyt, fiction::layouts::cartesian_layout>)
    {
        return Lyt{ex, scheme};
    }
    else
    {
        return Lyt{*a, ex, scheme};
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
[[nodiscard]] uint8_t num_adjacent_coordinates(const Lyt&                                       lyt,
                                               const fiction::layouts::layout_base::coordinate& c) noexcept
{
    static_assert(std::derived_from<Lyt, fiction::layouts::layout_base>, "Lyt is not a coordinate layout");

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
[[nodiscard]] Coordinate relative_to_absolute_cell_position(const GateLyt&                                   gate_lyt,
                                                            const fiction::layouts::layout_base::coordinate& t,
                                                            const Coordinate&                                relative_c)
{
    static_assert(
        requires { requires std::same_as<typename GateLyt::object_id, fiction::layouts::layout_object_id>; },
        "GateLyt is not a gate-level layout");

    if (relative_c.x < 0 || relative_c.x >= GateSizeX || relative_c.y < 0 || relative_c.y >= GateSizeY)
    {
        throw std::invalid_argument("The relative cell must be within the bounds of a single tile");
    }

    int64_t x = static_cast<int64_t>(t.x) * GateSizeX;
    int64_t y = static_cast<int64_t>(t.y) * GateSizeY;

    if constexpr (std::derived_from<GateLyt, fiction::layouts::shifted_cartesian_layout> ||
                  (std::derived_from<GateLyt, fiction::layouts::hexagonal_layout> &&
                   !std::derived_from<GateLyt, fiction::layouts::shifted_cartesian_layout>))
    {
        // hexagons nest into each other, so their tiles are 3/4 as far apart perpendicular to the shift
        constexpr auto step_x = (std::derived_from<GateLyt, fiction::layouts::hexagonal_layout> &&
                                 !std::derived_from<GateLyt, fiction::layouts::shifted_cartesian_layout>) ?
                                    GateSizeX * 3 / 4 :
                                    GateSizeX;
        constexpr auto step_y = (std::derived_from<GateLyt, fiction::layouts::hexagonal_layout> &&
                                 !std::derived_from<GateLyt, fiction::layouts::shifted_cartesian_layout>) ?
                                    GateSizeY * 3 / 4 :
                                    GateSizeY;

        const auto a   = gate_lyt.get_arrangement();
        const auto odd = is_odd_arrangement(a);

        if (is_row_arrangement(a))
        {
            y = static_cast<int64_t>(t.y) * step_y;

            if (odd ? gate_lyt.is_in_odd_row(t) : gate_lyt.is_in_even_row(t))
            {
                // shifted rows move in by width / 2
                x += GateSizeX / 2;
            }
        }
        else
        {
            x = static_cast<int64_t>(t.x) * step_x;

            if (odd ? gate_lyt.is_in_odd_column(t) : gate_lyt.is_in_even_column(t))
            {
                // shifted columns move in by height / 2
                y += GateSizeY / 2;
            }
        }
    }
    else
    {
        static_assert(std::derived_from<GateLyt, fiction::layouts::cartesian_layout>,
                      "GateLyt does not have a supported geometry");
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
 * @return Adjacent coordinate, or no coordinate when the neighbor lies outside the layout.
 */
template <typename Lyt>
[[nodiscard]] std::optional<fiction::layouts::layout_base::coordinate>
port_direction_to_coordinate(const Lyt& lyt, const fiction::layouts::layout_base::coordinate& c,
                             const fcn::port_direction& port) noexcept
{
    static_assert(std::derived_from<Lyt, fiction::layouts::layout_base>, "Lyt is not a coordinate layout");

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
 * cells. Layer coordinates, the layout name, and the clocking stay unchanged. The extent preserves trailing padding
 * and encloses translated x/y coordinates and occupied nonnegative layers.
 *
 * @tparam Lyt Cell grid layout type, e.g., `qca::layout`, `mol_qca::layout`, or `inml::layout`.
 * @param lyt The layout to normalize.
 * @return Normalized copy of `lyt`.
 * @throws std::invalid_argument If the normalized extent exceeds the signed coordinate domain.
 */
template <typename Lyt>
[[nodiscard]] Lyt normalize_layout_coordinates(const Lyt& lyt)
{
    static_assert(std::derived_from<Lyt, fiction::layouts::cartesian_layout>, "Lyt is not a Cartesian layout");

    if (lyt.is_empty())
    {
        return lyt;
    }

    /** Smallest occupied column. */
    int64_t x_offset = std::numeric_limits<int64_t>::max();
    /** Smallest occupied row. */
    int64_t y_offset = std::numeric_limits<int64_t>::max();
    /** Exclusive upper column bound, including frame padding. */
    int64_t x_end = lyt.width();
    /** Exclusive upper row bound, including frame padding. */
    int64_t y_end = lyt.height();
    /** Exclusive upper layer bound, including frame padding. */
    int64_t z_end = lyt.layers();

    lyt.foreach_cell(
        [&](const auto& c)
        {
            x_offset = std::min(x_offset, static_cast<decltype(x_offset)>(c.x));
            y_offset = std::min(y_offset, static_cast<decltype(y_offset)>(c.y));
            x_end    = std::max(x_end, static_cast<int64_t>(c.x) + 1);
            y_end    = std::max(y_end, static_cast<int64_t>(c.y) + 1);
            z_end    = std::max(z_end, static_cast<int64_t>(c.z) + 1);
        });

    /** Whether this layout stores cell modes. */
    constexpr bool has_cell_modes = requires(Lyt& l, const fiction::layouts::layout_base::coordinate& c) {
        l.assign_cell_mode(c, l.get_cell_mode(c));
    };

    /** Copy retaining layout metadata while cells move. */
    Lyt normalized{lyt};

    lyt.foreach_cell([&normalized](const auto& c) { normalized.assign_cell_type(c, Lyt::cell_type::EMPTY); });

    normalized.resize({x_end - x_offset, y_end - y_offset, z_end});

    lyt.foreach_cell(
        [&normalized, &lyt, x_offset, y_offset](const auto& c)
        {
            /** Cell coordinate translated by the occupied minima. */
            const fiction::layouts::layout_base::coordinate shifted{c.x - x_offset, c.y - y_offset, c.z};

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
 * Generates a random coordinate with each axis inside the inclusive region spanned by two coordinates.
 *
 * @tparam CoordinateType Coordinate type to generate.
 * @param corner1 One corner of the region.
 * @param corner2 Opposite corner of the region; axes may appear in either order.
 * @return Random coordinate between the corresponding corner axes.
 */
template <typename CoordinateType>
CoordinateType random_coordinate(const CoordinateType corner1, const CoordinateType corner2) noexcept
{
    /** Pseudorandom generator seeded from the platform's random device. */
    static thread_local std::mt19937_64 generator(std::random_device{}());
    /** Inclusive column distribution. */
    std::uniform_int_distribution<> dist_x(std::min(corner1.x, corner2.x), std::max(corner1.x, corner2.x));
    /** Inclusive row distribution. */
    std::uniform_int_distribution<> dist_y(std::min(corner1.y, corner2.y), std::max(corner1.y, corner2.y));
    /** Inclusive layer distribution. */
    std::uniform_int_distribution<> dist_z(std::min(corner1.z, corner2.z), std::max(corner1.z, corner2.z));

    return {dist_x(generator), dist_y(generator), dist_z(generator)};
}

}  // namespace fiction::layouts
