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
#include <cassert>
#include <cstdint>
#include <functional>
#include <memory>
#include <ranges>
#include <stdexcept>
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

    using layout_base::aspect_ratio;
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
        int32_t x;
        /**
         * y coordinate.
         */
        int32_t y;
        /**
         * z coordinate.
         */
        int32_t z;
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
        constexpr cube_coordinate(const int32_t cube_x, const int32_t cube_y, const int32_t cube_z) noexcept :
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

    /**
     * State that all copies of a layout share.
     */
    struct hexagonal_layout_storage
    {
        /**
         * Creates the storage of a layout.
         *
         * @param ar Highest possible position in the layout.
         * @param a Arrangement of the shifted rows or columns.
         */
        hexagonal_layout_storage(const aspect_ratio& ar, const layouts::arrangement a) noexcept :
                dimension{ar},
                shift{a}
        {}

        /**
         * Highest possible position in the layout.
         */
        aspect_ratio dimension;
        /**
         * Arrangement of the shifted rows or columns.
         */
        layouts::arrangement shift;
    };

    static constexpr auto min_fanin_size = 0u;  // NOLINT(readability-identifier-naming): mockturtle requirement
    static constexpr auto max_fanin_size = 5u;  // NOLINT(readability-identifier-naming): mockturtle requirement

    using base_type = hexagonal_layout;

    using storage = std::shared_ptr<hexagonal_layout_storage>;

    /**
     * Standard constructor. The given aspect ratio points to the highest possible coordinate in the layout. That means
     * in the `arrangement::EVEN_COLUMN` ASCII layout representation above `ar = (3,2)`. Consequently, with
     * `ar = (0,0)`, the layout has exactly one coordinate.
     *
     * @param a Arrangement of the shifted rows or columns. It cannot change after construction.
     * @param ar Highest possible position in the layout.
     * @throws std::invalid_argument If an axis of `ar` is negative.
     */
    explicit hexagonal_layout(const layouts::arrangement a, const aspect_ratio& ar = {0, 0}) :
            strg{std::make_shared<hexagonal_layout_storage>(checked(ar), a)}
    {}
    /**
     * Constructor that takes ownership of an existing storage, so that the new layout shares the coordinates of the
     * one the storage came from.
     *
     * @param s Storage to adopt.
     */
    explicit hexagonal_layout(std::shared_ptr<hexagonal_layout_storage> s) : strg{std::move(s)} {}
    /**
     * Clones the layout returning a deep copy.
     *
     * @return Deep copy of the layout.
     */
    [[nodiscard]] hexagonal_layout clone() const noexcept
    {
        return hexagonal_layout(std::make_shared<hexagonal_layout_storage>(*strg));
    }
    /**
     * Returns the arrangement of the shifted rows or columns.
     *
     * @return Arrangement fixed at construction.
     */
    [[nodiscard]] layouts::arrangement get_arrangement() const noexcept
    {
        return strg->shift;
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
     */
    template <typename X, typename Y, typename Z = uint64_t>
    constexpr coordinate coord(const X x, const Y y, const Z z = 0ul) const noexcept
    {
        return coordinate(x, y, z);
    }

#pragma endregion

#pragma region Structural properties
    /**
     * Returns the layout's x-dimension, i.e., returns the biggest x-value that still belongs to the layout.
     *
     * @return x-dimension.
     */
    [[nodiscard]] int32_t x() const noexcept
    {
        return strg->dimension.x;
    }
    /**
     * Returns the layout's y-dimension, i.e., returns the biggest y-value that still belongs to the layout.
     *
     * @return y-dimension.
     */
    [[nodiscard]] int32_t y() const noexcept
    {
        return strg->dimension.y;
    }
    /**
     * Returns the layout's z-dimension, i.e., returns the biggest z-value that still belongs to the layout.
     *
     * @return z-dimension.
     */
    [[nodiscard]] int32_t z() const noexcept
    {
        return strg->dimension.z;
    }
    /**
     * Returns the layout's number of faces depending on the coordinate type.
     *
     * @return Area of layout.
     */
    [[nodiscard]] uint64_t area() const noexcept
    {
        return fiction::layouts::area_of(strg->dimension);
    }
    /**
     * Updates the layout's dimensions, effectively resizing it.
     *
     * @param ar New aspect ratio.
     * @throws std::invalid_argument If an axis of `ar` is negative.
     */
    void resize(const aspect_ratio& ar)
    {
        strg->dimension = checked(ar);
    }

#pragma endregion

#pragma region row / column detection
    // The neighbor and border queries below do not read the layout, but every layout type exposes them as members: the
    // generic algorithms and `is_coordinate_layout_v` call them on a layout instance.
    // NOLINTBEGIN(readability-convert-member-functions-to-static)
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
     * Returns the coordinate that is directly adjacent in northern direction of a given coordinate `c`, i.e., the face
     * whose y-dimension is lower by 1. If `c`'s y-dimension is already at minimum, `c` is returned instead.
     *
     * @param c Coordinate whose northern counterpart is desired.
     * @return Coordinate adjacent and north of `c`.
     */
    [[nodiscard]] constexpr coordinate north(const coordinate& c) const noexcept
    {
        if (c.y <= 0)
        {
            return c;
        }

        auto nc = c;
        --nc.y;

        return nc;
    }
    /**
     * Returns the coordinate that is located in north-eastern direction of a given coordinate `c`. Depending on the
     * arrangement of the layout, the dimension values of the returned coordinate may differ.
     *
     * @param c Coordinate whose north-eastern counterpart is desired.
     * @return Coordinate directly north-eastern of `c`.
     */
    [[nodiscard]] constexpr coordinate north_east(const coordinate& c) const noexcept
    {
        if (!c.is_valid())
        {
            return c;
        }

        auto ne = to_offset_coordinate(to_cube_coordinate(c) + cube_coordinate{+1, 0, -1});

        ne.z = c.z;

        return is_within_bounds(ne) ? ne : c;
    }
    /**
     * Returns the coordinate that is directly adjacent in eastern direction of a given coordinate `c`, i.e., the face
     * whose x-dimension is higher by 1. If `c`'s x-dimension is already at maximum, `c` is returned instead.
     *
     * @param c Coordinate whose eastern counterpart is desired.
     * @return Coordinate adjacent and east of `c`.
     */
    [[nodiscard]] coordinate east(const coordinate& c) const noexcept
    {
        auto ec = c;

        if (c.x < 0 || c.x > x())
        {
            return coordinate{};
        }

        if (c.x < x())
        {
            ++ec.x;
        }

        return ec;
    }
    /**
     * Returns the coordinate that is located in south-eastern direction of a given coordinate `c`. Depending on the
     * arrangement of the layout, the dimension values of the returned coordinate may differ.
     *
     * @param c Coordinate whose south-eastern counterpart is desired.
     * @return Coordinate directly south-eastern of `c`.
     */
    [[nodiscard]] constexpr coordinate south_east(const coordinate& c) const noexcept
    {
        if (!c.is_valid())
        {
            return c;
        }

        const auto step =
            is_row_arrangement(get_arrangement()) ? cube_coordinate{0, -1, +1} : cube_coordinate{+1, -1, 0};

        auto se = to_offset_coordinate(to_cube_coordinate(c) + step);

        se.z = c.z;

        return is_within_bounds(se) ? se : c;
    }
    /**
     * Returns the coordinate that is directly adjacent in southern direction of a given coordinate `c`, i.e., the face
     * whose y-dimension is higher by 1. If `c`'s y-dimension is already at maximum, `c` is returned instead.
     *
     * @param c Coordinate whose southern counterpart is desired.
     * @return Coordinate adjacent and south of `c`.
     */
    [[nodiscard]] coordinate south(const coordinate& c) const noexcept
    {
        auto sc = c;

        if (c.y < 0 || c.y > y())
        {
            return coordinate{};
        }

        if (c.y < y())
        {
            ++sc.y;
        }

        return sc;
    }
    /**
     * Returns the coordinate that is located in south-western direction of a given coordinate `c`. Depending on the
     * arrangement of the layout, the dimension values of the returned coordinate may differ.
     *
     * @param c Coordinate whose south-western counterpart is desired.
     * @return Coordinate directly south-western of `c`.
     */
    [[nodiscard]] constexpr coordinate south_west(const coordinate& c) const noexcept
    {
        if (!c.is_valid())
        {
            return c;
        }

        auto sw = to_offset_coordinate(to_cube_coordinate(c) + cube_coordinate{-1, 0, +1});

        sw.z = c.z;

        return is_within_bounds(sw) ? sw : c;
    }
    /**
     * Returns the coordinate that is directly adjacent in western direction of a given coordinate `c`, i.e., the face
     * whose x-dimension is lower by 1. If `c`'s x-dimension is already at minimum, `c` is returned instead.
     *
     * @param c Coordinate whose western counterpart is desired.
     * @return Coordinate adjacent and west of `c`.
     */
    [[nodiscard]] constexpr coordinate west(const coordinate& c) const noexcept
    {
        if (c.x <= 0)
        {
            return c;
        }

        auto wc = c;
        --wc.x;

        return wc;
    }
    /**
     * Returns the coordinate that is located in north-western direction of a given coordinate `c`. Depending on the
     * arrangement of the layout, the dimension values of the returned coordinate may differ.
     *
     * @param c Coordinate whose north-western counterpart is desired.
     * @return Coordinate directly north-western of `c`.
     */
    [[nodiscard]] constexpr coordinate north_west(const coordinate& c) const noexcept
    {
        if (!c.is_valid())
        {
            return c;
        }

        const auto step =
            is_row_arrangement(get_arrangement()) ? cube_coordinate{0, +1, -1} : cube_coordinate{-1, +1, 0};

        auto nw = to_offset_coordinate(to_cube_coordinate(c) + step);

        nw.z = c.z;

        return is_within_bounds(nw) ? nw : c;
    }
    /**
     * Returns the coordinate that is directly above a given coordinate `c`, i.e., the face whose z-dimension is higher
     * by 1. If `c`'s z-dimension is already at maximum, `c` is returned instead.
     *
     * @param c Coordinate whose above counterpart is desired.
     * @return Coordinate directly above `c`.
     */
    [[nodiscard]] coordinate above(const coordinate& c) const noexcept
    {
        auto ac = c;

        if (c.z < 0 || c.z > z())
        {
            return coordinate{};
        }

        if (c.z < z())
        {
            ++ac.z;
        }

        return ac;
    }
    /**
     * Returns the coordinate that is directly below a given coordinate `c`, i.e., the face whose z-dimension is lower
     * by 1. If `c`'s z-dimension is already at minimum, `c` is returned instead.
     *
     * @param c Coordinate whose below counterpart is desired.
     * @return Coordinate directly below `c`.
     */
    [[nodiscard]] constexpr coordinate below(const coordinate& c) const noexcept
    {
        if (c.z <= 0)
        {
            return c;
        }

        auto bc = c;
        --bc.z;

        return bc;
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
        return c1 != c2 && north(c1) == c2;
    }
    /**
     * Returns `true` iff coordinate `c2` is directly east of coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is directly east of `c1`.
     */
    [[nodiscard]] bool is_east_of(const coordinate& c1, const coordinate& c2) const noexcept
    {
        return c1 != c2 && east(c1) == c2;
    }
    /**
     * Returns `true` iff coordinate `c2` is directly south of coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is directly south of `c1`.
     */
    [[nodiscard]] bool is_south_of(const coordinate& c1, const coordinate& c2) const noexcept
    {
        return c1 != c2 && south(c1) == c2;
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
        return c1 != c2 && west(c1) == c2;
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
        bool is_adjacent = false;

        foreach_adjacent_coordinate(c1,
                                    [&c2, &is_adjacent](const auto& ac1)
                                    {
                                        if (ac1 == c2)
                                        {
                                            is_adjacent = true;
                                        }
                                    });

        return is_adjacent;
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
        return is_adjacent_of(c1, c2) || is_adjacent_of(above(c1), c2) || is_adjacent_of(below(c1), c2);
    }
    /**
     * Returns `true` iff coordinate `c2` is directly above coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is directly above `c1`.
     */
    [[nodiscard]] bool is_above(const coordinate& c1, const coordinate& c2) const noexcept
    {
        return c1 != c2 && above(c1) == c2;
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
        return c1 != c2 && below(c1) == c2;
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
    [[nodiscard]] constexpr bool is_at_northern_border(const coordinate& c) const noexcept
    {
        return c.y == 0;
    }
    /**
     * Returns whether the given coordinate is located at the layout's eastern border where x is maximal.
     *
     * @param c Coordinate to check for border location.
     * @return `true` iff `c` is located at the layout's northern border.
     */
    [[nodiscard]] bool is_at_eastern_border(const coordinate& c) const noexcept
    {
        return c.x == x();
    }
    /**
     * Returns whether the given coordinate is located at the layout's southern border where y is maximal.
     *
     * @param c Coordinate to check for border location.
     * @return `true` iff `c` is located at the layout's southern border.
     */
    [[nodiscard]] bool is_at_southern_border(const coordinate& c) const noexcept
    {
        return c.y == y();
    }
    /**
     * Returns whether the given coordinate is located at the layout's western border where x is minimal.
     *
     * @param c Coordinate to check for border location.
     * @return `true` iff `c` is located at the layout's western border.
     */
    [[nodiscard]] constexpr bool is_at_western_border(const coordinate& c) const noexcept
    {
        return c.x == 0;
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
     * Returns the coordinate with the same x and z values as a given coordinate but that is located at the layout's
     * northern border.
     *
     * @param c Coordinate whose border counterpart is desired.
     * @return The northern border equivalent of `c`.
     */
    [[nodiscard]] coordinate northern_border_of(const coordinate& c) const noexcept
    {
        return {c.x, 0, c.z};
    }
    /**
     * Returns the coordinate with the same y and z values as a given coordinate but that is located at the layout's
     * eastern border.
     *
     * @param c Coordinate whose border counterpart is desired.
     * @return The eastern border equivalent of `c`.
     */
    [[nodiscard]] coordinate eastern_border_of(const coordinate& c) const noexcept
    {
        return {x(), c.y, c.z};
    }
    /**
     * Returns the coordinate with the same x and z values as a given coordinate but that is located at the layout's
     * southern border.
     *
     * @param c Coordinate whose border counterpart is desired.
     * @return The southern border equivalent of `c`.
     */
    [[nodiscard]] coordinate southern_border_of(const coordinate& c) const noexcept
    {
        return {c.x, y(), c.z};
    }
    /**
     * Returns the coordinate with the same y and z values as a given coordinate but that is located at the layout's
     * western border.
     *
     * @param c Coordinate whose border counterpart is desired.
     * @return The western border equivalent of `c`.
     */
    [[nodiscard]] coordinate western_border_of(const coordinate& c) const noexcept
    {
        return {0, c.y, c.z};
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
    [[nodiscard]] constexpr bool is_within_bounds(const coordinate& c) const noexcept
    {
        return c.x >= 0 && c.x <= x() && c.y >= 0 && c.y <= y() && c.z >= 0 && c.z <= z();
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
    [[nodiscard]] auto coordinates(const coordinate& start = {}, const coordinate& stop = {}) const
    {
        return std::ranges::subrange{coordinate_iterator{strg->dimension, !start.is_valid() ? coordinate{0, 0} : start},
                                     coordinate_iterator{strg->dimension, !stop.is_valid() ? coordinate{} : stop}};
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
    void foreach_coordinate(Fn&& fn, const coordinate& start = {}, const coordinate& stop = {}) const
    {
        mockturtle::detail::foreach_element(
            coordinate_iterator{strg->dimension, !start.is_valid() ? coordinate{0, 0} : start},
            coordinate_iterator{strg->dimension, !stop.is_valid() ? coordinate{} : stop}, std::forward<Fn>(fn));
    }
    /**
     * Returns a range of all coordinates accessible in the layout's ground layer between `start` and `stop`. The
     * iteration order is the same as for the coordinates function but without the z dimension.
     *
     * @param start First coordinate to include in the range of all ground coordinates.
     * @param stop Last coordinate (exclusive) to include in the range of all ground coordinates.
     * @return An iterator range from `start` to `stop`. If they are not provided, the first/last coordinate in the
     * ground layer is used as a default.
     */
    [[nodiscard]] auto ground_coordinates(const coordinate& start = {}, const coordinate& stop = {}) const
    {
        assert((!start.is_valid() || start.z == 0) && (!stop.is_valid() || stop.z == 0));

        auto ground_layer = aspect_ratio{x(), y(), 0};

        return std::ranges::subrange{coordinate_iterator{ground_layer, !start.is_valid() ? coordinate{0, 0} : start},
                                     coordinate_iterator{ground_layer, !stop.is_valid() ? coordinate{} : stop}};
    }
    /**
     * Applies a function to all coordinates accessible in the layout's ground layer between `start` and `stop`. The
     * iteration order is the same as for the ground_coordinates function.
     *
     * @tparam Fn Functor type that has to comply with the restrictions imposed by `mockturtle::foreach_element`.
     * @param fn Functor to apply to each coordinate in the range.
     * @param start First coordinate to include in the range of all ground coordinates.
     * @param stop Last coordinate (exclusive) to include in the range of all ground coordinates.
     */
    template <typename Fn>
    void foreach_ground_coordinate(Fn&& fn, const coordinate& start = {}, const coordinate& stop = {}) const
    {
        assert((!start.is_valid() || start.z == 0) && (!stop.is_valid() || stop.z == 0));

        auto ground_layer = aspect_ratio{x(), y(), 0};

        mockturtle::detail::foreach_element(
            coordinate_iterator{ground_layer, !start.is_valid() ? coordinate{0, 0} : start},
            coordinate_iterator{ground_layer, !stop.is_valid() ? coordinate{} : stop}, std::forward<Fn>(fn));
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

        foreach_adjacent_coordinate(c, [&cnt](const auto& ac) { cnt.push_back(ac); });

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
        if (!c.is_valid())
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
                                  // convert given coordinate to the cube system, add direction, and convert back to
                                  // offset
                                  auto neighbor = to_offset_coordinate(to_cube_coordinate(c) + dir);
                                  // since cube coordinates don't carry the layer information, it has to be manually
                                  // added
                                  neighbor.z = c.z;

                                  // add neighboring coordinate if there was no over-/underflow
                                  if (is_within_bounds(neighbor))
                                  {
                                      std::invoke(std::forward<Fn>(fn), std::move(neighbor));
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

        foreach_adjacent_opposite_coordinates(c, [&cnt](const auto& cp) { cnt.push_back(cp); });

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
        const auto apply_if_not_c = [&c, &fn](auto cardinal1, auto cardinal2) noexcept
        {
            if (cardinal1 != c && cardinal2 != c)
            {
                std::invoke(std::forward<Fn>(fn), std::make_pair(std::move(cardinal1), std::move(cardinal2)));
            }
        };

        if (is_row_arrangement(get_arrangement()))
        {
            apply_if_not_c(east(c), west(c));
        }
        else  // column arrangement, flat top
        {
            apply_if_not_c(north(c), south(c));
        }

        apply_if_not_c(north_east(c), south_west(c));
        apply_if_not_c(north_west(c), south_east(c));
    }

#pragma endregion

// data types cannot properly be converted to bit field types
#pragma GCC diagnostic push
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wuseless-cast"
#endif
#pragma GCC diagnostic ignored "-Wconversion"

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
        cube_coordinate cube_coord{0, 0, 0};

        const auto offset = is_odd_arrangement(get_arrangement()) ? -1 : 1;

        if (is_row_arrangement(get_arrangement()))
        {
            cube_coord.x = offset_coord.x - static_cast<decltype(cube_coord.x)>(
                                                (offset_coord.y + (offset_coord.y % 2 != 0 ? offset : 0)) / 2);
            cube_coord.z = offset_coord.y;
            cube_coord.y = -cube_coord.x - cube_coord.z;
        }
        else
        {
            cube_coord.x = offset_coord.x;
            cube_coord.z = offset_coord.y - static_cast<decltype(cube_coord.z)>(
                                                (offset_coord.x + (offset_coord.x % 2 != 0 ? offset : 0)) / 2);
            cube_coord.y = -cube_coord.x - cube_coord.z;
        }

        return cube_coord;
    }
    /**
     * Converts a cube coordinate to an offset coordinate.
     *
     * This implementation is adapted from https://www.redblobgames.com/grids/hexagons/codegen/output/lib.cpp
     *
     * @param cube_coord Cube coordinate to convert.
     * @return Offset coordinate representing `cube_coord` in the layout's arrangement.
     */
    [[nodiscard]] coordinate to_offset_coordinate(const cube_coordinate& cube_coord) const noexcept
    {
        // the generated coordinate will be in ground layer
        coordinate offset_coord{0, 0};

        const auto offset = is_odd_arrangement(get_arrangement()) ? -1 : 1;
        if (is_row_arrangement(get_arrangement()))
        {
            offset_coord.x = static_cast<decltype(offset_coord.x)>(
                cube_coord.x + static_cast<int64_t>((cube_coord.z + (cube_coord.z % 2 != 0 ? offset : 0)) / 2));
            offset_coord.y = static_cast<decltype(offset_coord.y)>(cube_coord.z);
        }
        else
        {
            offset_coord.x = static_cast<decltype(offset_coord.x)>(cube_coord.x);
            offset_coord.y = static_cast<decltype(offset_coord.y)>(
                cube_coord.z + static_cast<int64_t>((cube_coord.x + (cube_coord.x % 2 != 0 ? offset : 0)) / 2));
        }

        return offset_coord;
    }

#pragma endregion

#pragma GCC diagnostic pop

  private:
    /**
     * Returns an aspect ratio after checking that it describes a layout. An invalid aspect ratio describes the layout
     * with exactly one coordinate.
     *
     * @param ar Aspect ratio to check.
     * @return `ar`, or (0, 0, 0) if `ar` is invalid.
     * @throws std::invalid_argument If an axis of `ar` is negative.
     */
    static aspect_ratio checked(const aspect_ratio& ar)
    {
        if (!ar.is_valid())
        {
            return aspect_ratio{0, 0, 0};
        }

        if (ar.x < 0 || ar.y < 0 || ar.z < 0)
        {
            throw std::invalid_argument("The aspect ratio of a layout must not be negative");
        }

        return ar;
    }
    /**
     * Shared storage for the layout dimensions and arrangement.
     */
    storage strg;
};

}  // namespace fiction::layouts
