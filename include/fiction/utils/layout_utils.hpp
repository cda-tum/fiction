//
// Created by marcel on 14.10.21.
//

#ifndef FICTION_LAYOUT_UTILS_HPP
#define FICTION_LAYOUT_UTILS_HPP

#include "fiction/technology/cell_ports.hpp"
#include "fiction/traits.hpp"

#include <cassert>
#include <cstdint>
#include <set>

namespace fiction
{

/**
 * Returns the number of adjacent coordinates of a given one. This is not a constant value because c could be located at
 * a layout border.
 *
 * @tparam Lyt Layout type.
 * @param lyt Layout.
 * @param c Coordinate whose number of adjacencies are required.
 * @return Number of c's adjacent coordinates.
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
 * is, when presented with, e.g., a NORTH_EAST direction, it will return the coordinate that is to the NORTH_EAST of the
 * given coordinate c in the layout lyt.
 *
 * @tparam Lyt Coordinate layout type.
 * @param lyt Coordinate layout.
 * @param c Coordinate to consider.
 * @param port Port direction.
 * @return Absolute coordinate specified by a coordinate c in layout lyt and a port direction.
 */
template <typename Lyt>
[[nodiscard]] coordinate<Lyt> port_direction_to_coordinate(const Lyt& lyt, const coordinate<Lyt>& c,
                                                           const port_direction& port) noexcept
{
    static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");

    switch (port.dir)
    {
        case port_direction::cardinal::NORTH:
        {
            return lyt.north(c);
        }
        case port_direction::cardinal::NORTH_EAST:
        {
            return lyt.north_east(c);
        }
        case port_direction::cardinal::EAST:
        {
            return lyt.east(c);
        }
        case port_direction::cardinal::SOUTH_EAST:
        {
            return lyt.south_east(c);
        }
        case port_direction::cardinal::SOUTH:
        {
            return lyt.south(c);
        }
        case port_direction::cardinal::SOUTH_WEST:
        {
            return lyt.south_west(c);
        }
        case port_direction::cardinal::WEST:
        {
            return lyt.west(c);
        }
        case port_direction::cardinal::NORTH_WEST:
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

}  // namespace fiction

#endif  // FICTION_LAYOUT_UTILS_HPP
