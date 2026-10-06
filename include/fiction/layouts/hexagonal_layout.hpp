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
 * @brief Hexagonal grid layout in the four pointy- and flat-top offset arrangements.
 * @author Marcel Walter (marcelwa)
 * @author Willem Lambooy (wlambooy)
 * @author Simon Hofmann (simon1hofmann)
 */

#pragma once

#include "fiction/layouts/arrangement.hpp"
#include "fiction/layouts/layout_base.hpp"

#include <mockturtle/networks/detail/foreach.hpp>

#include <algorithm>
#include <array>
#include <concepts>
#include <cstdint>
#include <functional>
#include <limits>
#include <optional>
#include <ranges>
#include <utility>
#include <vector>

namespace fiction::layouts
{

/**
 * A layout type that utilizes offset coordinates to represent a hexagonal grid. Its faces are organized in an offset
 * coordinate system as provided. The arrangement fixed at construction selects which rows or columns are shifted. Row
 * arrangements yield pointy-top hexagons, column arrangements flat-top hexagons. The four arrangements look as follows.
 *
 * `arrangement::ODD_ROW`:
 * \verbatim
         / \     / \     / \
       /     \ /     \ /     \
      | (0,0) | (1,0) | (2,0) |
      |       |       |       |
       \     / \     / \     / \
         \ /     \ /     \ /     \
          | (0,1) | (1,1) | (2,1) |
          |       |       |       |
         / \     / \     / \     /
       /     \ /     \ /     \ /
      | (0,2) | (1,2) | (2,2) |
      |       |       |       |
       \     / \     / \     /
         \ /     \ /     \ /
  \endverbatim
 *
 * `arrangement::EVEN_ROW`:
 * \verbatim
         / \     / \     / \
       /     \ /     \ /     \
      | (0,0) | (1,0) | (2,0) |
      |       |       |       |
     / \     / \     / \     /
   /     \ /     \ /     \ /
  | (0,1) | (1,1) | (2,1) |
  |       |       |       |
   \     / \     / \     / \
     \ /     \ /     \ /     \
      | (0,2) | (1,2) | (2,2) |
      |       |       |       |
       \     / \     / \     /
         \ /     \ /     \ /
  \endverbatim
 *
 * `arrangement::ODD_COLUMN`:
 * \verbatim
     _____         _____
    /     \       /     \
   / (0,0) \_____/ (2,0) \_____
   \       /     \       /     \
    \_____/ (1,0) \_____/ (3,0) \
    /     \       /     \       /
   / (0,1) \_____/ (2,1) \_____/
   \       /     \       /     \
    \_____/ (1,1) \_____/ (3,1) \
    /     \       /     \       /
   / (0,2) \_____/ (2,2) \_____/
   \       /     \       /
    \_____/       \_____/
  \endverbatim
 *
 * `arrangement::EVEN_COLUMN`:
 * \verbatim
            _____         _____
           /     \       /     \
     _____/ (1,0) \_____/ (3,0) \
    /     \       /     \       /
   / (0,0) \_____/ (2,0) \_____/
   \       /     \       /     \
    \_____/ (1,1) \_____/ (3,1) \
    /     \       /     \       /
   / (0,1) \_____/ (2,1) \_____/
   \       /     \       /     \
    \_____/ (1,2) \_____/ (3,2) \
          \       /     \       /
           \_____/       \_____/
  \endverbatim
 *
 * Other representations would be using cube or axial coordinates for instance, but since we want the layouts to be
 * rectangular-ish, offset coordinates make the most sense here.
 *
 * https://www.redblobgames.com/grids/hexagons/ is a wonderful resource on the topic.
 *
 */
class hexagonal_layout : public layout_base
{
  public:
#pragma region Types and constructors

    /** Axis sizes. */
    using layout_base::extent;
    /** Signed coordinate values. */
    using layout_base::coordinate;

    /**
     * Cube coordinates identify faces of the hexagonal grid with three signed axes that sum to zero. The layout uses
     * them internally for neighbor calculations. A wonderful resource on the topic is
     * https://www.redblobgames.com/grids/hexagons/#coordinates-cube
     */
    struct cube_coordinate
    {
        /**
         * x coordinate.
         */
        int64_t x;
        /**
         * y coordinate.
         */
        int64_t y;
        /**
         * z coordinate.
         */
        int64_t z;
        /**
         * Creates a cube coordinate at the origin.
         */
        constexpr cube_coordinate() noexcept : x{0}, y{0}, z{0} {}
        /**
         * Creates a cube coordinate from its three axes.
         *
         * @param cube_x x coordinate.
         * @param cube_y y coordinate.
         * @param cube_z z coordinate.
         */
        constexpr cube_coordinate(const int64_t cube_x, const int64_t cube_y, const int64_t cube_z) noexcept :
                x{cube_x},
                y{cube_y},
                z{cube_z}
        {}
        /**
         * Compares against another cube coordinate for equality.
         *
         * @param other Right-hand side coordinate.
         * @return `true` iff all axes are equal.
         */
        constexpr bool operator==(const cube_coordinate& other) const noexcept
        {
            return x == other.x && y == other.y && z == other.z;
        }
        /**
         * Adds another cube coordinate axis by axis.
         *
         * @param other Right-hand side coordinate.
         * @return The sum of both coordinates.
         */
        [[nodiscard]] constexpr cube_coordinate operator+(const cube_coordinate& other) const noexcept
        {
            return {x + other.x, y + other.y, z + other.z};
        }
    };

    /** Minimum number of incoming neighbors. */
    static constexpr auto min_fanin_size = 0u;  // NOLINT(readability-identifier-naming): mockturtle requirement
    /** Maximum number of incoming neighbors. */
    static constexpr auto max_fanin_size = 5u;  // NOLINT(readability-identifier-naming): mockturtle requirement

    /** Geometry base type. */
    using base_type = hexagonal_layout;

    /**
     * Creates geometry with half-open, zero-origin bounds. The default extent is empty.
     * @param a Arrangement of shifted rows or columns.
     * @param size Axis sizes.
     * @throws std::invalid_argument If a size exceeds the coordinate domain.
     */
    explicit hexagonal_layout(const layouts::arrangement a, const extent& size = {}) :
            dimension{checked(size)},
            shift{a}
    {}
    /** @return Independent copy of the geometry. */
    [[nodiscard]] hexagonal_layout clone() const noexcept
    {
        return *this;
    }
    /**
     * Returns the arrangement of the shifted rows or columns.
     *
     * @return Arrangement fixed at construction.
     */
    [[nodiscard]] layouts::arrangement get_arrangement() const noexcept
    {
        return shift;
    }
    /**
     * Creates and returns a coordinate in the layout from the given x-, y-, and z-values.
     *
     * @note This function is equivalent to calling `coordinate(x, y, z)`.
     *
     * @tparam X x-type.
     * @tparam Y y-type.
     * @tparam Z z-type.
     * @param x x-value.
     * @param y y-value.
     * @param z z-value.
     * @return A coordinate in the layout of type `coordinate`.
     * @throws std::overflow_error If an axis is outside the signed 32-bit range.
     */
    template <std::integral X, std::integral Y, std::integral Z = uint64_t>
    constexpr coordinate coord(const X x, const Y y, const Z z = 0ul) const
    {
        return coordinate(x, y, z);
    }

#pragma endregion

#pragma region Structural properties
    /** @return Number of coordinates along x. */
    [[nodiscard]] uint32_t width() const noexcept
    {
        return dimension.width;
    }
    /** @return Number of coordinates along y. */
    [[nodiscard]] uint32_t height() const noexcept
    {
        return dimension.height;
    }
    /** @return Number of layers. */
    [[nodiscard]] uint32_t layers() const noexcept
    {
        return dimension.layers;
    }
    /** @return Independent value of the axis sizes. */
    [[nodiscard]] extent dimensions() const noexcept
    {
        return dimension;
    }
    /** @return Width times height. */
    [[nodiscard]] uint64_t area() const noexcept
    {
        return area_of(dimension);
    }
    /** @return Volume. @throws std::overflow_error If the volume exceeds `uint64_t`. */
    [[nodiscard]] uint64_t volume() const
    {
        return volume_of(dimension);
    }
    /**
     * Changes the geometry's axis sizes.
     * @param size Axis sizes.
     * @throws std::invalid_argument If a size exceeds the coordinate domain.
     */
    void resize(const extent& size)
    {
        dimension = checked(size);
    }
    /** @return Last coordinate in iteration order, or no value for empty geometry. */
    [[nodiscard]] std::optional<coordinate> last_coordinate() const noexcept
    {
        if (width() == 0 || height() == 0 || layers() == 0)
        {
            return std::nullopt;
        }
        return coordinate{width() - 1, height() - 1, layers() - 1};
    }
#pragma endregion

#pragma region row / column detection
    // Coordinate predicates belong to the layout interface used by generic algorithms.
    // NOLINTBEGIN(readability-convert-member-functions-to-static): generic algorithms use the layout member interface
    /**
     * Checks if the given coordinate is located in a row with an odd index.
     *
     * @param c Coordinate to check.
     * @return `true` iff `c` is located in an odd row.
     */
    [[nodiscard]] bool is_in_odd_row(const coordinate& c) const noexcept
    {
        return c.y % 2 != 0;
    }
    /**
     * Checks if the given coordinate is located in a row with an even index.
     *
     * @param c Coordinate to check.
     * @return `true` iff `c` is located in an even row.
     */
    [[nodiscard]] bool is_in_even_row(const coordinate& c) const noexcept
    {
        return c.y % 2 == 0;
    }
    /**
     * Checks if the given coordinate is located in a column with an odd index.
     *
     * @param c Coordinate to check.
     * @return `true` iff `c` is located in an odd column.
     */
    [[nodiscard]] bool is_in_odd_column(const coordinate& c) const noexcept
    {
        return c.x % 2 != 0;
    }
    /**
     * Checks if the given coordinate is located in a column with an even index.
     *
     * @param c Coordinate to check.
     * @return `true` iff `c` is located in an even column.
     */
    [[nodiscard]] bool is_in_even_column(const coordinate& c) const noexcept
    {
        return c.x % 2 == 0;
    }

#pragma endregion

#pragma region Cardinal operations
    /**
     * Returns the north neighbor when both coordinates lie inside the geometry.
     * @param c Base coordinate.
     * @return Neighbor, or no value at a boundary or outside the geometry.
     */
    [[nodiscard]] std::optional<coordinate> north(const coordinate& c) const noexcept
    {
        return bounded_neighbor(c, 0, -1, 0);
    }
    /**
     * Returns the north-east neighbor when both coordinates lie inside the geometry.
     * @param c Base coordinate.
     * @return Neighbor, or no value at a boundary or outside the geometry.
     */
    [[nodiscard]] std::optional<coordinate> north_east(const coordinate& c) const noexcept
    {
        if (!contains_coordinate(c))
        {
            return std::nullopt;
        }
        const auto step = cube_coordinate{+1, 0, -1};
        return bounded_offset(offset_axes(to_cube_coordinate(c) + step), c.z);
    }
    /**
     * Returns the east neighbor when both coordinates lie inside the geometry.
     * @param c Base coordinate.
     * @return Neighbor, or no value at a boundary or outside the geometry.
     */
    [[nodiscard]] std::optional<coordinate> east(const coordinate& c) const noexcept
    {
        return bounded_neighbor(c, 1, 0, 0);
    }
    /**
     * Returns the south-east neighbor when both coordinates lie inside the geometry.
     * @param c Base coordinate.
     * @return Neighbor, or no value at a boundary or outside the geometry.
     */
    [[nodiscard]] std::optional<coordinate> south_east(const coordinate& c) const noexcept
    {
        if (!contains_coordinate(c))
        {
            return std::nullopt;
        }
        const auto step =
            is_row_arrangement(get_arrangement()) ? cube_coordinate{0, -1, +1} : cube_coordinate{+1, -1, 0};
        return bounded_offset(offset_axes(to_cube_coordinate(c) + step), c.z);
    }
    /**
     * Returns the south neighbor when both coordinates lie inside the geometry.
     * @param c Base coordinate.
     * @return Neighbor, or no value at a boundary or outside the geometry.
     */
    [[nodiscard]] std::optional<coordinate> south(const coordinate& c) const noexcept
    {
        return bounded_neighbor(c, 0, 1, 0);
    }
    /**
     * Returns the south-west neighbor when both coordinates lie inside the geometry.
     * @param c Base coordinate.
     * @return Neighbor, or no value at a boundary or outside the geometry.
     */
    [[nodiscard]] std::optional<coordinate> south_west(const coordinate& c) const noexcept
    {
        if (!contains_coordinate(c))
        {
            return std::nullopt;
        }
        const auto step = cube_coordinate{-1, 0, +1};
        return bounded_offset(offset_axes(to_cube_coordinate(c) + step), c.z);
    }
    /**
     * Returns the west neighbor when both coordinates lie inside the geometry.
     * @param c Base coordinate.
     * @return Neighbor, or no value at a boundary or outside the geometry.
     */
    [[nodiscard]] std::optional<coordinate> west(const coordinate& c) const noexcept
    {
        return bounded_neighbor(c, -1, 0, 0);
    }
    /**
     * Returns the north-west neighbor when both coordinates lie inside the geometry.
     * @param c Base coordinate.
     * @return Neighbor, or no value at a boundary or outside the geometry.
     */
    [[nodiscard]] std::optional<coordinate> north_west(const coordinate& c) const noexcept
    {
        if (!contains_coordinate(c))
        {
            return std::nullopt;
        }
        const auto step =
            is_row_arrangement(get_arrangement()) ? cube_coordinate{0, +1, -1} : cube_coordinate{-1, +1, 0};
        return bounded_offset(offset_axes(to_cube_coordinate(c) + step), c.z);
    }
    /**
     * Returns the above neighbor when both coordinates lie inside the geometry.
     * @param c Base coordinate.
     * @return Neighbor, or no value at a boundary or outside the geometry.
     */
    [[nodiscard]] std::optional<coordinate> above(const coordinate& c) const noexcept
    {
        return bounded_neighbor(c, 0, 0, 1);
    }
    /**
     * Returns the below neighbor when both coordinates lie inside the geometry.
     * @param c Base coordinate.
     * @return Neighbor, or no value at a boundary or outside the geometry.
     */
    [[nodiscard]] std::optional<coordinate> below(const coordinate& c) const noexcept
    {
        return bounded_neighbor(c, 0, 0, -1);
    }
    /**
     * Returns `true` iff coordinate `c2` is directly north of coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is directly north of `c1`.
     */
    [[nodiscard]] constexpr bool is_north_of(const coordinate& c1, const coordinate& c2) const noexcept
    {
        return static_cast<int64_t>(c2.x) - c1.x == 0 && static_cast<int64_t>(c2.y) - c1.y == -1 &&
               static_cast<int64_t>(c2.z) - c1.z == 0;
    }
    /**
     * Returns `true` iff coordinate `c2` is directly east of coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is directly east of `c1`.
     */
    [[nodiscard]] constexpr bool is_east_of(const coordinate& c1, const coordinate& c2) const noexcept
    {
        return static_cast<int64_t>(c2.x) - c1.x == 1 && static_cast<int64_t>(c2.y) - c1.y == 0 &&
               static_cast<int64_t>(c2.z) - c1.z == 0;
    }
    /**
     * Returns `true` iff coordinate `c2` is directly south of coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is directly south of `c1`.
     */
    [[nodiscard]] constexpr bool is_south_of(const coordinate& c1, const coordinate& c2) const noexcept
    {
        return static_cast<int64_t>(c2.x) - c1.x == 0 && static_cast<int64_t>(c2.y) - c1.y == 1 &&
               static_cast<int64_t>(c2.z) - c1.z == 0;
    }
    /**
     * Returns `true` iff coordinate `c2` is directly west of coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is directly west of `c1`.
     */
    [[nodiscard]] constexpr bool is_west_of(const coordinate& c1, const coordinate& c2) const noexcept
    {
        return static_cast<int64_t>(c2.x) - c1.x == -1 && static_cast<int64_t>(c2.y) - c1.y == 0 &&
               static_cast<int64_t>(c2.z) - c1.z == 0;
    }
    /**
     * Returns `true` iff coordinate `c2` is either north, north-east, east, south-east, south, south-west, west, or
     * north-west of coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is directly adjacent to `c1` in one of the six different ordinal directions possible for
     * the layout's arrangement.
     */
    [[nodiscard]] bool is_adjacent_of(const coordinate& c1, const coordinate& c2) const noexcept
    {
        const auto a = to_cube_coordinate(c1);
        const auto b = to_cube_coordinate(c2);
        return c1.z == c2.z && std::max({std::abs(a.x - b.x), std::abs(a.y - b.y), std::abs(a.z - b.z)}) == 1;
    }
    /**
     * Similar to is_adjacent_of but also considers `c1`'s elevation, i.e., if `c2` is adjacent to `above(c1)` or
     * `below(c1)`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is either adjacent of `c1` or `c1`'s elevations.
     */
    [[nodiscard]] bool is_adjacent_elevation_of(const coordinate& c1, const coordinate& c2) const noexcept
    {
        const auto dz = static_cast<int64_t>(c2.z) - c1.z;
        return dz >= -1 && dz <= 1 && is_adjacent_of(c1, coordinate{c2.x, c2.y, c1.z});
    }
    /**
     * Returns `true` iff coordinate `c2` is directly above coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is directly above `c1`.
     */
    [[nodiscard]] constexpr bool is_above(const coordinate& c1, const coordinate& c2) const noexcept
    {
        return static_cast<int64_t>(c2.x) - c1.x == 0 && static_cast<int64_t>(c2.y) - c1.y == 0 &&
               static_cast<int64_t>(c2.z) - c1.z == 1;
    }
    /**
     * Returns `true` iff coordinate `c2` is directly below coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is directly below `c1`.
     */
    [[nodiscard]] constexpr bool is_below(const coordinate& c1, const coordinate& c2) const noexcept
    {
        return static_cast<int64_t>(c2.x) - c1.x == 0 && static_cast<int64_t>(c2.y) - c1.y == 0 &&
               static_cast<int64_t>(c2.z) - c1.z == -1;
    }
    /**
     * Returns `true` iff coordinate `c2` is somewhere north of coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is somewhere north of `c1`.
     */
    [[nodiscard]] constexpr bool is_northwards_of(const coordinate& c1, const coordinate& c2) const noexcept
    {
        return (c1.z == c2.z) && (c1.y > c2.y) && (c1.x == c2.x);
    }
    /**
     * Returns `true` iff coordinate `c2` is somewhere east of coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is somewhere east of `c1`.
     */
    [[nodiscard]] constexpr bool is_eastwards_of(const coordinate& c1, const coordinate& c2) const noexcept
    {
        return (c1.z == c2.z) && (c1.y == c2.y) && (c1.x < c2.x);
    }
    /**
     * Returns `true` iff coordinate `c2` is somewhere south of coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is somewhere south of `c1`.
     */
    [[nodiscard]] constexpr bool is_southwards_of(const coordinate& c1, const coordinate& c2) const noexcept
    {
        return (c1.z == c2.z) && (c1.y < c2.y) && (c1.x == c2.x);
    }
    /**
     * Returns `true` iff coordinate `c2` is somewhere west of coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is somewhere west of `c1`.
     */
    [[nodiscard]] constexpr bool is_westwards_of(const coordinate& c1, const coordinate& c2) const noexcept
    {
        return (c1.z == c2.z) && (c1.y == c2.y) && (c1.x > c2.x);
    }
    /**
     * Returns whether the given coordinate is located at the layout's northern border where y is minimal.
     *
     * @param c Coordinate to check for border location.
     * @return `true` iff `c` is located at the layout's northern border.
     */
    [[nodiscard]] bool is_at_northern_border(const coordinate& c) const noexcept
    {
        return contains_coordinate(c) && c.y == 0;
    }
    /**
     * Returns whether the given coordinate is located at the layout's eastern border where x is maximal.
     *
     * @param c Coordinate to check for border location.
     * @return `true` iff `c` is located at the layout's northern border.
     */
    [[nodiscard]] bool is_at_eastern_border(const coordinate& c) const noexcept
    {
        return contains_coordinate(c) && static_cast<uint32_t>(c.x) + 1 == width();
    }
    /**
     * Returns whether the given coordinate is located at the layout's southern border where y is maximal.
     *
     * @param c Coordinate to check for border location.
     * @return `true` iff `c` is located at the layout's southern border.
     */
    [[nodiscard]] bool is_at_southern_border(const coordinate& c) const noexcept
    {
        return contains_coordinate(c) && static_cast<uint32_t>(c.y) + 1 == height();
    }
    /**
     * Returns whether the given coordinate is located at the layout's western border where x is minimal.
     *
     * @param c Coordinate to check for border location.
     * @return `true` iff `c` is located at the layout's western border.
     */
    [[nodiscard]] bool is_at_western_border(const coordinate& c) const noexcept
    {
        return contains_coordinate(c) && c.x == 0;
    }
    /**
     * Returns whether the given coordinate is located at any of the layout's borders where x or y are either minimal or
     * maximal.
     *
     * @param c Coordinate to check for border location.
     * @return `true` iff `c` is located at any of the layout's borders.
     */
    [[nodiscard]] bool is_at_any_border(const coordinate& c) const noexcept
    {
        return is_at_northern_border(c) || is_at_eastern_border(c) || is_at_southern_border(c) ||
               is_at_western_border(c);
    }
    /**
     * Projects a coordinate to the northern border.
     * @param c Coordinate to project.
     * @return Projection, or no value if the projection lies outside the geometry.
     */
    [[nodiscard]] std::optional<coordinate> northern_border_of(const coordinate& c) const noexcept
    {
        if (!last_coordinate())
        {
            return std::nullopt;
        }
        const coordinate projected{c.x, 0, c.z};
        return contains_coordinate(projected) ? std::optional{projected} : std::nullopt;
    }
    /**
     * Projects a coordinate to the eastern border.
     * @param c Coordinate to project.
     * @return Projection, or no value if the projection lies outside the geometry.
     */
    [[nodiscard]] std::optional<coordinate> eastern_border_of(const coordinate& c) const noexcept
    {
        if (!last_coordinate())
        {
            return std::nullopt;
        }
        const coordinate projected{static_cast<int64_t>(width()) - 1, c.y, c.z};
        return contains_coordinate(projected) ? std::optional{projected} : std::nullopt;
    }
    /**
     * Projects a coordinate to the southern border.
     * @param c Coordinate to project.
     * @return Projection, or no value if the projection lies outside the geometry.
     */
    [[nodiscard]] std::optional<coordinate> southern_border_of(const coordinate& c) const noexcept
    {
        if (!last_coordinate())
        {
            return std::nullopt;
        }
        const coordinate projected{c.x, static_cast<int64_t>(height()) - 1, c.z};
        return contains_coordinate(projected) ? std::optional{projected} : std::nullopt;
    }
    /**
     * Projects a coordinate to the western border.
     * @param c Coordinate to project.
     * @return Projection, or no value if the projection lies outside the geometry.
     */
    [[nodiscard]] std::optional<coordinate> western_border_of(const coordinate& c) const noexcept
    {
        if (!last_coordinate())
        {
            return std::nullopt;
        }
        const coordinate projected{0, c.y, c.z};
        return contains_coordinate(projected) ? std::optional{projected} : std::nullopt;
    }
    /**
     * Returns whether the given coordinate is located in the ground layer where z is minimal.
     *
     * @param c Coordinate to check for elevation.
     * @return `true` iff `c` is in ground layer.
     */
    [[nodiscard]] constexpr bool is_ground_layer(const coordinate& c) const noexcept
    {
        return c.z == 0;
    }
    /**
     * Returns whether the given coordinate is located in a crossing layer where z is not minimal.
     *
     * @param c Coordinate to check for elevation.
     * @return `true` iff `c` is in a crossing layer.
     */
    [[nodiscard]] constexpr bool is_crossing_layer(const coordinate& c) const noexcept
    {
        return c.z > 0;
    }
    // NOLINTEND(readability-convert-member-functions-to-static)
    /**
     * Returns whether the given coordinate is located within the layout bounds.
     *
     * @param c Coordinate to check for boundary.
     * @return `true` iff `c` is located within the layout bounds.
     */
    [[nodiscard]] bool contains_coordinate(const coordinate& c) const noexcept
    {
        return c.x >= 0 && static_cast<uint32_t>(c.x) < width() && c.y >= 0 && static_cast<uint32_t>(c.y) < height() &&
               c.z >= 0 && static_cast<uint32_t>(c.z) < layers();
    }
    /** @param c Coordinate. @return Whether the geometry contains the coordinate. */
    [[nodiscard]] bool is_within_bounds(const coordinate& c) const noexcept
    {
        return contains_coordinate(c);
    }

#pragma endregion

#pragma region Iteration
    /**
     * Returns a range of all coordinates accessible in the layout between `start` and `stop`. If no values are
     * provided, all coordinates in the layout will be included. The returned iterator range points to the first and
     * last coordinate, respectively. The range object can be used within a for-each loop. Incrementing the iterator is
     * equivalent to nested for loops in the order z, y, x. Consequently, the iteration will happen inside out, i.e., x
     * will be iterated first, then y, then z.
     *
     * @param start First coordinate to include in the range of all coordinates.
     * @param stop Last coordinate (exclusive) to include in the range of all coordinates.
     * @return An iterator range from `start` to `stop`. If they are not provided, the first/last coordinate is used as
     * a default.
     */
    [[nodiscard]] auto coordinates(const std::optional<coordinate> start = std::nullopt,
                                   const std::optional<coordinate> stop  = std::nullopt) const
    {
        const coordinate_iterator first{dimension, start.value_or(coordinate{})};
        const coordinate_iterator last{dimension, stop};
        return std::ranges::subrange{first < last ? first : last, last};
    }
    /**
     * Applies a function to all coordinates accessible in the layout between `start` and `stop`. The iteration order is
     * the same as for the coordinates function.
     *
     * @tparam Fn Functor type that has to comply with the restrictions imposed by `mockturtle::foreach_element`.
     * @param fn Functor to apply to each coordinate in the range.
     * @param start First coordinate to include in the range of all coordinates.
     * @param stop Last coordinate (exclusive) to include in the range of all coordinates.
     */
    template <typename Fn>
    void foreach_coordinate(Fn&& fn, const std::optional<coordinate> start = std::nullopt,
                            const std::optional<coordinate> stop = std::nullopt) const
    {
        const auto range = coordinates(start, stop);
        mockturtle::detail::foreach_element(range.begin(), range.end(), std::forward<Fn>(fn));
    }
    /**
     * Returns a range of all coordinates accessible in the layout's ground layer between `start` and `stop`. The
     * iteration order is the same as for the coordinates function but without the z dimension.
     *
     * @param start First coordinate to include in the range of all ground coordinates.
     * @param stop Last coordinate (exclusive) to include in the range of all ground coordinates.
     * @return An iterator range from `start` to `stop`. If they are not provided, the first/last coordinate in the
     * ground layer is used as a default.
     * @throws std::invalid_argument If a range bound lies outside layer zero.
     */
    [[nodiscard]] auto ground_coordinates(const std::optional<coordinate> start = std::nullopt,
                                          const std::optional<coordinate> stop  = std::nullopt) const
    {
        if ((start && start->z != 0) || (stop && stop->z != 0))
        {
            throw std::invalid_argument("A ground coordinate range requires layer zero");
        }
        const extent              ground_layer{width(), height(), layers() == 0 ? 0u : 1u};
        const coordinate_iterator first{ground_layer, start.value_or(coordinate{})};
        const coordinate_iterator last{ground_layer, stop};
        return std::ranges::subrange{first < last ? first : last, last};
    }
    /**
     * Applies a function to all coordinates accessible in the layout's ground layer between `start` and `stop`. The
     * iteration order is the same as for the ground_coordinates function.
     *
     * @tparam Fn Functor type that has to comply with the restrictions imposed by `mockturtle::foreach_element`.
     * @param fn Functor to apply to each coordinate in the range.
     * @param start First coordinate to include in the range of all ground coordinates.
     * @param stop Last coordinate (exclusive) to include in the range of all ground coordinates.
     * @throws std::invalid_argument If a range bound lies outside layer zero.
     */
    template <typename Fn>
    void foreach_ground_coordinate(Fn&& fn, const std::optional<coordinate> start = std::nullopt,
                                   const std::optional<coordinate> stop = std::nullopt) const
    {
        const auto range = ground_coordinates(start, stop);
        mockturtle::detail::foreach_element(range.begin(), range.end(), std::forward<Fn>(fn));
    }
    /**
     * Returns a container that contains all coordinates that are adjacent to a given one. Thereby, cardinal and ordinal
     * directions are being considered, i.e., the container will contain all coordinates `ac` for which `is_adjacent(c,
     * ac)` returns `true`.
     *
     * Coordinates that are outside of the layout bounds are not considered. Thereby, the size of the returned container
     * is at most 6.
     *
     * @param c Coordinate whose adjacent ones are desired.
     * @return A container that contains all of `c`'s adjacent coordinates.
     */
    [[nodiscard]] auto adjacent_coordinates(const coordinate& c) const noexcept
    {
        std::vector<coordinate> cnt{};
        cnt.reserve(max_fanin_size + 1);  // reserve memory

        foreach_adjacent_coordinate(c, [&cnt](const auto& ac) noexcept { cnt.push_back(ac); });

        return cnt;
    }
    /**
     * Applies a function to all coordinates adjacent to a given one in accordance with `adjacent_coordinates`. Thereby,
     * cardinal and ordinal directions are being considered, i.e., the given function is applied to all coordinates ac
     * for which `is_adjacent(c, ac)` returns `true`.
     *
     * Coordinates that are outside of the layout bounds are not considered. Thereby, at most 6 coordinates are touched.
     *
     * @tparam Fn Functor type.
     * @param c Coordinate whose adjacent ones are desired.
     * @param fn Functor to apply to each of `c`'s adjacent coordinates.
     */
    template <typename Fn>
    void foreach_adjacent_coordinate(const coordinate& c, Fn&& fn) const
    {
        if (!contains_coordinate(c))
        {
            return;
        }

        // six possible directions in cube coordinates
        static constexpr const std::array<cube_coordinate, 6> cube_directions{
            {{+1, -1, 0}, {+1, 0, -1}, {0, +1, -1}, {-1, +1, 0}, {-1, 0, +1}, {0, -1, +1}}};

        // for each direction
        std::ranges::for_each(cube_directions,
                              [this, &c, &fn](const auto& dir)
                              {
                                  auto neighbor = bounded_offset(offset_axes(to_cube_coordinate(c) + dir), c.z);
                                  if (neighbor)
                                  {
                                      std::invoke(std::forward<Fn>(fn), *neighbor);
                                  }
                              });
    }
    /**
     * Returns a container that contains all coordinates pairs of opposing adjacent coordinates with
     * respect to a given one. In this hexagonal layout, the container content depends on the arrangement.
     *
     * In case of a row arrangement (pointy-top), the container will contain (`east(c)`, `west(c)`), (`north_east(c)`,
     * `south_west(c)`), (`north_west(c)`, `south_east(c)`). In case of a column arrangement (flat-top), the container
     * will contain (`north(c)`, `south(c)`), (`north_east(c)`, `south_west(c)`), (`north_west(c)`, `south_east(c)`)
     * instead.
     *
     * This function comes in handy when straight lines on the layout are to be examined.
     *
     * Coordinates outside of the layout bounds are not being considered.
     *
     * @param c Coordinate whose opposite ones are desired.
     * @return A container that contains pairs of `c`'s opposing coordinates.
     */
    [[nodiscard]] auto adjacent_opposite_coordinates(const coordinate& c) const noexcept
    {
        std::vector<std::pair<coordinate, coordinate>> cnt{};
        cnt.reserve((max_fanin_size + 1) / 2);  // reserve memory

        foreach_adjacent_opposite_coordinates(c, [&cnt](const auto& cp) noexcept { cnt.push_back(cp); });

        return cnt;
    }
    /**
     * Applies a function to all opposing coordinate pairs adjacent to a given one. In this hexagonal layout, the
     * function application depends on the arrangement.
     *
     * In case of a row arrangement (pointy-top), the function will apply to (`east(c)`, `west(c)`), (`north_east(c)`,
     * `south_west(c)`), (`north_west(c)`, `south_east(c)`). In case of a column arrangement (flat-top), the function
     * will apply to (`north(c)`, `south(c)`), (`north_east(c)`, `south_west(c)`), (`north_west(c)`, `south_east(c)`)
     * instead.
     *
     * This function comes in handy when straight lines on the layout are to be examined.
     *
     * Coordinates outside of the layout bounds are not being considered.
     *
     * @tparam Fn Functor type.
     * @param c Coordinate whose opposite adjacent ones are desired.
     * @param fn Functor to apply to each of `c`'s opposite adjacent coordinate pairs.
     */
    template <typename Fn>
    void foreach_adjacent_opposite_coordinates(const coordinate& c, Fn&& fn) const
    {
        const auto apply_if_present = [&fn](auto cardinal1, auto cardinal2) noexcept
        {
            if (cardinal1 && cardinal2)
            {
                std::invoke(std::forward<Fn>(fn), std::make_pair(*cardinal1, *cardinal2));
            }
        };

        if (is_row_arrangement(get_arrangement()))
        {
            apply_if_present(east(c), west(c));
        }
        else  // column arrangement, flat top
        {
            apply_if_present(north(c), south(c));
        }

        apply_if_present(north_east(c), south_west(c));
        apply_if_present(north_west(c), south_east(c));
    }

#pragma endregion

#pragma region coordinates
    /**
     * Converts an offset coordinate to a cube coordinate.
     *
     * This implementation is adapted from https://www.redblobgames.com/grids/hexagons/codegen/output/lib.cpp
     *
     * @param offset_coord Offset coordinate to convert.
     * @return Cube coordinate representing `offset_coord` in the layout's arrangement.
     */
    [[nodiscard]] cube_coordinate to_cube_coordinate(const coordinate& offset_coord) const noexcept
    {
        // 64-bit arithmetic keeps the conversion free of overflow for every 32-bit coordinate
        const int64_t offset = is_odd_arrangement(get_arrangement()) ? -1 : 1;
        const int64_t ox     = offset_coord.x;
        const int64_t oy     = offset_coord.y;

        cube_coordinate cube_coord{0, 0, 0};

        if (is_row_arrangement(get_arrangement()))
        {
            cube_coord.x = ox - half_shifted(oy, offset);
            cube_coord.z = oy;
        }
        else
        {
            cube_coord.x = ox;
            cube_coord.z = oy - half_shifted(ox, offset);
        }

        cube_coord.y = -cube_coord.x - cube_coord.z;

        return cube_coord;
    }
    /**
     * Converts a cube coordinate to an offset coordinate. The result lies in the ground layer.
     *
     * This implementation is adapted from https://www.redblobgames.com/grids/hexagons/codegen/output/lib.cpp
     *
     * @param cube_coord Cube coordinate to convert.
     * @return Offset coordinate representing `cube_coord`, or no value if an axis exceeds 32 bits.
     */
    [[nodiscard]] std::optional<coordinate> to_offset_coordinate(const cube_coordinate& cube_coord) const noexcept
    {
        const bool row          = is_row_arrangement(get_arrangement());
        const auto fixed_axis   = row ? cube_coord.z : cube_coord.x;
        const auto shifted_axis = row ? cube_coord.x : cube_coord.z;
        // The shift can cancel at most half of the unchanged 32-bit axis.
        if (!std::in_range<int32_t>(fixed_axis) ||
            shifted_axis < 2 * static_cast<int64_t>(std::numeric_limits<int32_t>::min()) ||
            shifted_axis > 2 * static_cast<int64_t>(std::numeric_limits<int32_t>::max()))
        {
            return {};
        }
        const auto [x, y] = offset_axes(cube_coord);
        if (!std::in_range<int32_t>(x) || !std::in_range<int32_t>(y))
        {
            return {};
        }
        return coordinate{static_cast<int32_t>(x), static_cast<int32_t>(y)};
    }

#pragma endregion

  private:
    /**
     * Halves an axis value after moving an odd value by `offset`, which is the shift between neighboring rows or
     * columns in offset coordinates.
     *
     * @param value Axis value.
     * @param offset Shift of odd values, -1 for odd and +1 for even arrangements.
     * @return `(value + offset) / 2` for an odd value and `value / 2` for an even one.
     */
    [[nodiscard]] static constexpr int64_t half_shifted(const int64_t value, const int64_t offset) noexcept
    {
        return (value + (value % 2 != 0 ? offset : 0)) / 2;
    }
    /**
     * Converts cube coordinates to offset axes without narrowing.
     *
     * @param cube_coord Cube coordinate.
     * @return Signed 64-bit x and y offset axes.
     */
    [[nodiscard]] std::pair<int64_t, int64_t> offset_axes(const cube_coordinate& cube_coord) const noexcept
    {
        const int64_t offset = is_odd_arrangement(get_arrangement()) ? -1 : 1;
        auto          x      = cube_coord.x;
        auto          y      = cube_coord.z;
        if (is_row_arrangement(get_arrangement()))
        {
            x += half_shifted(y, offset);
        }
        else
        {
            y += half_shifted(x, offset);
        }
        return {x, y};
    }
    /**
     * Checks layout bounds before narrowing offset axes.
     *
     * @param axes Signed 64-bit offset axes.
     * @param layer Coordinate layer.
     * @return Coordinate in the layout, or no value.
     */
    [[nodiscard]] std::optional<coordinate> bounded_offset(const std::pair<int64_t, int64_t>& axes,
                                                           const int32_t                      layer) const noexcept
    {
        const auto [nx, ny] = axes;
        if (nx < 0 || nx >= width() || ny < 0 || ny >= height() || layer < 0 ||
            static_cast<uint32_t>(layer) >= layers())
        {
            return {};
        }
        return coordinate{nx, ny, layer};
    }
    /**
     * Computes a Cartesian step with wide arithmetic and checks both positions against the geometry.
     * @param c Base coordinate.
     * @param dx x step.
     * @param dy y step.
     * @param dz z step.
     * @return Neighbor, or no value outside the geometry.
     */
    [[nodiscard]] std::optional<coordinate> bounded_neighbor(const coordinate& c, const int32_t dx, const int32_t dy,
                                                             const int32_t dz) const noexcept
    {
        if (!contains_coordinate(c))
        {
            return std::nullopt;
        }
        const auto nx = static_cast<int64_t>(c.x) + dx;
        const auto ny = static_cast<int64_t>(c.y) + dy;
        const auto nz = static_cast<int64_t>(c.z) + dz;
        if (nx < 0 || nx >= width() || ny < 0 || ny >= height() || nz < 0 || nz >= layers())
        {
            return std::nullopt;
        }
        return coordinate{nx, ny, nz};
    }
    /** Independent axis sizes. */
    extent dimension{};
    /** Arrangement of shifted rows or columns. */
    layouts::arrangement shift;
};

}  // namespace fiction::layouts
