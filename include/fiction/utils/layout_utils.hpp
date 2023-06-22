//
// Created by marcel on 14.10.21.
//

#ifndef FICTION_LAYOUT_UTILS_HPP
#define FICTION_LAYOUT_UTILS_HPP

#include "fiction/layouts/cell_level_layout.hpp"
#include "fiction/layouts/coordinates.hpp"
#include "fiction/technology/cell_ports.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <limits>
#include <random>
#include <type_traits>
#include <utility>

namespace fiction
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
    static_assert(!has_siqad_coord_v<CellLyt>, "CellLyt cannot have SiQAD coordinates");
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

/**
 * A new layout is constructed and returned that is equivalent to the given cell-level layout. However, its coordinates
 * are normalized, i.e., start at `(0, 0)` and are all positive. To this end, all existing coordinates are shifted by an
 * x and y offset.
 *
 * @tparam Lyt Cell-level layout type.
 * @param lyt The layout which is to be normalized.
 * @return New normalized equivalent layout.
 */
template <typename Lyt>
Lyt normalize_layout_coordinates(const Lyt& lyt) noexcept
{
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");

    auto x_offset = std::numeric_limits<int32_t>::max();
    auto y_offset = std::numeric_limits<int32_t>::max();

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

    Lyt lyt_new{{lyt.x() - x_offset, lyt.y() - y_offset, lyt.z()},
                lyt.get_layout_name(),
                lyt.get_tile_size_x(),
                lyt.get_tile_size_y()};

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
 * Converts the coordinates of a given cell-level layout to SiQAD coordinates. A new equivalent layout based on SiQAD
 * coordinates is returned.
 *
 * @tparam Lyt Cell-level layout type based on fiction coordinates, e.g., `offset::ucoord_t` or `cube::coord_t`.
 * @param lyt The layout that is to be converted to a new layout based on SiQAD coordinates.
 * @return A new equivalent layout based on SiQAD coordinates.
 */
template <typename Lyt>
sidb_cell_clk_lyt_siqad convert_to_siqad_coordinates(const Lyt& lyt) noexcept
{
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    sidb_cell_clk_lyt_siqad lyt_new{{lyt.x(), (lyt.y() - lyt.y() % 2) / 2},
                                    lyt.get_layout_name(),
                                    lyt.get_tile_size_x(),
                                    lyt.get_tile_size_y()};

    lyt.foreach_cell(
        [&lyt_new, &lyt](const auto& c)
        {
            lyt_new.assign_cell_type(siqad::to_siqad_coord<cell<Lyt>>(c), lyt.get_cell_type(c));
            lyt_new.assign_cell_mode(siqad::to_siqad_coord<cell<Lyt>>(c), lyt.get_cell_mode(c));
            lyt_new.assign_cell_name(siqad::to_siqad_coord<cell<Lyt>>(c), lyt.get_cell_name(c));
        });

    return lyt_new;
}

/**
 * Converts the coordinates of a given cell-level layout to fiction coordinates, e.g., `offset::ucoord_t` or
 * `cube::coord_t`. A new equivalent layout based on fiction coordinates is returned.
 *
 * @tparam Lyt Cell-level layout type based on fiction coordinates.
 * @param lyt The layout that is to be converted to a new layout based on fiction coordinates.
 * @return A new equivalent layout based on fiction coordinates.
 */
template <typename Lyt>
Lyt convert_to_fiction_coordinates(const sidb_cell_clk_lyt_siqad& lyt) noexcept
{
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    Lyt lyt_new{{lyt.x(), 2 * lyt.y() + 1}, lyt.get_layout_name(), lyt.get_tile_size_x(), lyt.get_tile_size_y()};

    const auto assign_coordinates = [&lyt_new](const auto& base_lyt) noexcept
    {
        base_lyt.foreach_cell(
            [&lyt_new, &base_lyt](const auto& c)
            {
                lyt_new.assign_cell_type(siqad::to_fiction_coord<cell<Lyt>>(c), base_lyt.get_cell_type(c));
                lyt_new.assign_cell_mode(siqad::to_fiction_coord<cell<Lyt>>(c), base_lyt.get_cell_mode(c));
                lyt_new.assign_cell_name(siqad::to_fiction_coord<cell<Lyt>>(c), base_lyt.get_cell_name(c));
            });
    };

    if (has_offset_ucoord_v<Lyt> && !lyt.is_empty())
    {
        auto lyt_normalized = normalize_layout_coordinates<sidb_cell_clk_lyt_siqad>(lyt);
        assign_coordinates(lyt_normalized);
        lyt_new.resize({lyt_normalized.x(), 2 * lyt_normalized.y() + 1});
    }
    else
    {
        assign_coordinates(lyt);
    }

    return lyt_new;
}
/**
 * Generates a random coordinate within the region spanned by two given coordinates. The two given coordinates form the
 * top left corner and the bottom right corner of the spanned region.
 *
 * @OffsetCoordinateType The coordinate implementation to be used.
 * @param coordinate1 Top left Coordinate.
 * @param coordinate2 Bottom right Coordinate (coordinate order is not important, automatically swapped if
 * necessary).
 * @return Randomly generated coordinate.
 */
template <typename CoordinateType>
CoordinateType random_coordinate(CoordinateType coordinate1, CoordinateType coordinate2) noexcept
{
    static_assert(std::is_same_v<CoordinateType, offset::ucoord_t> || std::is_same_v<CoordinateType, cube::coord_t> ||
                      std::is_same_v<CoordinateType, siqad::coord_t>,
                  "CoordinateType is unknown");

    static std::mt19937_64 generator(std::random_device{}());

    if (coordinate1 > coordinate2)
    {
        std::swap(coordinate1, coordinate2);
    }

    if constexpr (std::is_same_v<CoordinateType, siqad::coord_t>)
    {
        std::uniform_int_distribution<> dist_x(coordinate1.x, coordinate2.x);
        std::uniform_int_distribution<> dist_y(coordinate1.y, coordinate2.y);
        std::uniform_int_distribution<> dist_z(0, 1);

        return std::clamp(siqad::coord_t{dist_x(generator), dist_y(generator), dist_z(generator)}, coordinate1,
                          coordinate2);
    }

    else
    {
        std::uniform_int_distribution<> dist_x(coordinate1.x, coordinate2.x);
        std::uniform_int_distribution<> dist_y(coordinate1.y, coordinate2.y);
        std::uniform_int_distribution<> dist_z(coordinate1.z, coordinate2.z);

        return {dist_x(generator), dist_y(generator), dist_z(generator)};
    }
}
/**
 * Calculates the north-west and south-east cells of a minimum-sized box around all non-empty cells in a given layout.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt The layout for which the two corner cells of the minimum-sized box are determined.
 * @return North-west and south-east cells of minimum-sized box.
 */
template <typename Lyt>
std::pair<typename Lyt::cell, typename Lyt::cell> bounding_box_siqad(const Lyt& lyt) noexcept
{
    static_assert(has_siqad_coord_v<Lyt>, "Layout is not based on siqad coordinates");

    if (lyt.num_cells() == 0)
    {
        return {typename Lyt::cell{}, typename Lyt::cell{}};
    }

    const auto converted_layout = convert_to_fiction_coordinates<
        cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<cube::coord_t>>>>(lyt);

    int32_t min_x = std::numeric_limits<int32_t>::max();
    int32_t max_x = std::numeric_limits<int32_t>::min();

    int32_t min_y = std::numeric_limits<int32_t>::max();
    int32_t max_y = std::numeric_limits<int32_t>::min();

    converted_layout.foreach_cell(
        [&converted_layout, &min_x, &max_x, &min_y, &max_y](const auto& c)
        {
            if (c.x < min_x)
            {
                min_x = c.x;
            }
            if (c.y < min_y)
            {
                min_y = c.y;
            }
            if (c.x > max_x)
            {
                max_x = c.x;
            }
            if (c.y > max_y)
            {
                max_y = c.y;
            }
        });

    return {siqad::to_siqad_coord(cube::coord_t{min_x, min_y}), siqad::to_siqad_coord(cube::coord_t{max_x, max_y})};
}

}  // namespace fiction

#endif  // FICTION_LAYOUT_UTILS_HPP
