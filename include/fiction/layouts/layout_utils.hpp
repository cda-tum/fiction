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

#include "fiction/layouts/coordinates.hpp"
#include "fiction/technology/fcn/cell_ports.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/stl/hash.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <random>
#include <type_traits>
#include <utility>
#include <vector>

namespace fiction::layouts
{

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
 * @tparam CellLyt Cell-level layout type.
 * @param gate_lyt The gate-level layout whose tiles are to be considered.
 * @param t Tile within gate_lyt.
 * @param relative_c Relative cell position within t.
 * @return Absolute cell position in a layout.
 */
template <uint16_t GateSizeX, uint16_t GateSizeY, typename GateLyt, typename CellLyt>
[[nodiscard]] cell<CellLyt> relative_to_absolute_cell_position(const GateLyt& gate_lyt, const tile<GateLyt>& t,
                                                               const cell<CellLyt>& relative_c) noexcept
{
    static_assert(is_cell_level_layout_v<CellLyt>, "CellLyt is not a cell-level layout");
    static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt is not a gate-level layout");

    assert(relative_c.x < GateSizeX && relative_c.y < GateSizeY &&
           "relative_c must be within the bounds of a single tile");

    cell<CellLyt> absolute_c{};

    // Cartesian layouts
    if constexpr (is_cartesian_layout_v<GateLyt>)
    {
        absolute_c = {t.x * GateSizeX, t.y * GateSizeY, t.z};
    }
    // shifted Cartesian layouts
    else if constexpr (is_shifted_cartesian_layout_v<GateLyt>)
    {
        if constexpr (has_horizontally_shifted_cartesian_orientation_v<GateLyt>)
        {
            absolute_c = {t.x * GateSizeX, static_cast<decltype(absolute_c.y)>(t.y * (GateSizeY)), t.z};
        }
        else if constexpr (has_vertically_shifted_cartesian_orientation_v<GateLyt>)
        {
            absolute_c = {static_cast<decltype(absolute_c.x)>(t.x * (GateSizeX)), t.y * (GateSizeY), t.z};
        }

        if constexpr (has_odd_row_cartesian_arrangement_v<GateLyt>)
        {
            if (gate_lyt.is_in_odd_row(t))
            {
                // odd rows are shifted in by width / 2
                absolute_c.x += static_cast<decltype(absolute_c.x)>(static_cast<double>(GateSizeX) / 2.0);
            }
        }
        else if constexpr (has_even_row_cartesian_arrangement_v<GateLyt>)
        {
            if (gate_lyt.is_in_even_row(t))
            {
                // even rows are shifted in by width / 2
                absolute_c.x += static_cast<decltype(absolute_c.x)>(static_cast<double>(GateSizeX) / 2.0);
            }
        }
        else if constexpr (has_odd_column_cartesian_arrangement_v<GateLyt>)
        {
            if (gate_lyt.is_in_odd_column(t))
            {
                // odd columns are shifted in by height / 2
                absolute_c.y += static_cast<decltype(absolute_c.y)>(static_cast<double>(GateSizeY) / 2.0);
            }
        }
        else if constexpr (has_even_column_cartesian_arrangement_v<GateLyt>)
        {
            if (gate_lyt.is_in_even_column(t))
            {
                // even columns are shifted in by height / 2
                absolute_c.y += static_cast<decltype(absolute_c.y)>(static_cast<double>(GateSizeY) / 2.0);
            }
        }
    }
    // hexagonal layouts
    else if constexpr (is_hexagonal_layout_v<GateLyt>)
    {
        if constexpr (has_pointy_top_hex_orientation_v<GateLyt>)
        {
            // vertical distance between pointy top hexagons is height * 3/4
            absolute_c = {t.x * GateSizeX, static_cast<decltype(absolute_c.y)>(t.y * (GateSizeY * 3 / 4)), t.z};
        }
        else if constexpr (has_flat_top_hex_orientation_v<GateLyt>)
        {
            // horizontal distance between flat top hexagons is width * 3/4
            absolute_c = {static_cast<decltype(absolute_c.x)>(t.x * (GateSizeX * 3 / 4)), t.y * (GateSizeY), t.z};
        }

        if constexpr (has_odd_row_hex_arrangement_v<GateLyt>)
        {
            if (gate_lyt.is_in_odd_row(t))
            {
                // odd rows are shifted in by width / 2
                absolute_c.x += static_cast<decltype(absolute_c.x)>(static_cast<double>(GateSizeX) / 2.0);
            }
        }
        else if constexpr (has_even_row_hex_arrangement_v<GateLyt>)
        {
            if (gate_lyt.is_in_even_row(t))
            {
                // even rows are shifted in by width / 2
                absolute_c.x += static_cast<decltype(absolute_c.x)>(static_cast<double>(GateSizeX) / 2.0);
            }
        }
        else if constexpr (has_odd_column_hex_arrangement_v<GateLyt>)
        {
            if (gate_lyt.is_in_odd_column(t))
            {
                // odd columns are shifted in by height / 2
                absolute_c.y += static_cast<decltype(absolute_c.y)>(static_cast<double>(GateSizeY) / 2.0);
            }
        }
        else if constexpr (has_even_column_hex_arrangement_v<GateLyt>)
        {
            if (gate_lyt.is_in_even_column(t))
            {
                // even columns are shifted in by height / 2
                absolute_c.y += static_cast<decltype(absolute_c.y)>(static_cast<double>(GateSizeY) / 2.0);
            }
        }
    }
    // more gate-level layout types go here
    else
    {
        assert(false && "unknown gate-level layout type");
    }

    absolute_c.x += relative_c.x;
    absolute_c.y += relative_c.y;

    return absolute_c;
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
 * A new layout is constructed and returned that is equivalent to the given cell-level layout. However, its coordinates
 * are normalized, i.e., start at `(0, 0)` and are all positive. To this end, all existing coordinates are shifted by an
 * x and y offset.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt The layout which is to be normalized.
 * @return New normalized equivalent layout.
 */
template <typename Lyt>
Lyt normalize_layout_coordinates(const Lyt& lyt) noexcept
{
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");

    auto x_offset = std::numeric_limits<decltype(lyt.x())>::max();
    auto y_offset = std::numeric_limits<decltype(lyt.y())>::max();

    lyt.foreach_cell(
        [&x_offset, &y_offset](const auto& c)
        {
            if (c.y < y_offset)
            {
                y_offset = c.y;
            }
            if (c.x < x_offset)
            {
                x_offset = c.x;
            }
        });

    Lyt lyt_new{};

    assert(lyt.x() - x_offset >= 0 && "x_offset is too large");
    assert(lyt.y() - y_offset >= 0 && "y_offset is too large");

    lyt_new.resize(
        {static_cast<std::size_t>(lyt.x() - x_offset), static_cast<std::size_t>(lyt.y() - y_offset), lyt.z()});

    lyt_new.set_layout_name(lyt.get_layout_name());
    lyt_new.set_tile_size_x(lyt.get_tile_size_x());
    lyt_new.set_tile_size_y(lyt.get_tile_size_y());

    lyt.foreach_cell(
        [&lyt_new, &lyt, &x_offset, &y_offset](const auto& c)
        {
            lyt_new.assign_cell_type({c.x - x_offset, c.y - y_offset}, lyt.get_cell_type(c));
            lyt_new.assign_cell_mode({c.x - x_offset, c.y - y_offset}, lyt.get_cell_mode(c));
            lyt_new.assign_cell_name({c.x - x_offset, c.y - y_offset}, lyt.get_cell_name(c));
        });

    return lyt_new;
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
/**
 * This function checks whether the given layouts `first_lyt` and `second_lyt` are identical by comparing the number
 * of cells and the types of cells.
 *
 * @Note The aspect ratios of the cell-level layouts are not compared.
 *
 * @tparam Lyt The layout type. Must be a cell-level layout.
 * @param first_lyt The first layout to compare.
 * @param second_lyt The second layout to compare.
 * @return `true` if the layouts are identical, `false` otherwise.
 */
template <typename Lyt>
[[nodiscard]] inline bool are_cell_layouts_identical(const Lyt& first_lyt, const Lyt& second_lyt) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");

    if (first_lyt.num_cells() != second_lyt.num_cells())
    {
        return false;
    }

    bool different_cells = false;

    first_lyt.foreach_cell(
        [&first_lyt, &second_lyt, &different_cells](const auto& c)
        {
            if (first_lyt.get_cell_type(c) != second_lyt.get_cell_type(c))
            {
                different_cells = true;
                return false;  // abort
            }
            return true;  // keep looping
        });

    if (different_cells)
    {
        return false;
    }

    return true;
}
/**
 * @brief Computes a digest of the given cell-level layout that respects the equality
 * `are_cell_layouts_identical` implements.
 *
 * Identical layouts always share a digest, so layouts with different digests are never identical. That makes the
 * digest a cheap filter in front of `are_cell_layouts_identical`. Different layouts may share a digest, so a
 * digest match still has to be confirmed with `are_cell_layouts_identical`.
 *
 * The digest covers the cells and their types. Following `are_cell_layouts_identical`, it ignores the layout's aspect
 * ratio.
 *
 * @tparam Lyt The layout type. Must be a cell-level layout.
 * @param lyt The layout to digest.
 * @return Hash value that identifies `lyt` up to `are_cell_layouts_identical`.
 */
template <typename Lyt>
[[nodiscard]] inline std::size_t cell_layout_digest(const Lyt& lyt) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");

    // cells live in a hash map, so two layouts holding the same elements do not agree on the order in which
    // foreach_cell visits them. hash_combine_unordered is commutative and therefore
    // independent of that order. An ordered container is not an option here: the coordinate types order by x, y,
    // and z while comparing and hashing the dead indicator as well, so a sorted fold would merge a cell with its
    // dead twin and lose one of the two
    std::size_t cell_fold{0};

    lyt.foreach_cell(
        [&lyt, &cell_fold](const auto& c)
        {
            std::size_t cell_hash{0};
            fiction::utils::stl::hash_combine(cell_hash, c, lyt.get_cell_type(c));

            fiction::utils::stl::hash_combine_unordered(cell_fold, cell_hash);
        });

    std::size_t digest{0};
    fiction::utils::stl::hash_combine(digest, lyt.num_cells(), cell_fold);

    return digest;
}

}  // namespace fiction::layouts
