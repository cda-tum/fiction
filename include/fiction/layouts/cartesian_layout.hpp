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
 * @brief Cartesian grid layout addressed by coordinates.
 * @author Marcel Walter (marcelwa)
 * @author Willem Lambooy (wlambooy)
 * @author Simon Hofmann (simon1hofmann)
 */

#pragma once

#include "fiction/layouts/coordinates.hpp"

#include <mockturtle/networks/detail/foreach.hpp>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <memory>
#include <ranges>
#include <type_traits>
#include <utility>
#include <vector>

namespace fiction::layouts
{

/**
 * A layout type that utilizes coordinates to represent a Cartesian grid. Its faces are organized in the following way:
 *
 * \verbatim
   +-------+-------+-------+-------+
   |       |       |       |       |
   | (0,0) | (1,0) | (2,0) | (3,0) |
   |       |       |       |       |
   +-------+-------+-------+-------+
   |       |       |       |       |
   | (0,1) | (1,1) | (2,1) | (3,1) |
   |       |       |       |       |
   +-------+-------+-------+-------+
   |       |       |       |       |
   | (0,2) | (1,2) | (2,2) | (3,2) |
   |       |       |       |       |
   +-------+-------+-------+-------+
   \endverbatim
 *
 * @tparam OffsetCoordinateType The coordinate implementation to be used.
 */
template <typename OffsetCoordinateType = coords::offset>
class cartesian_layout
{
  public:
#pragma region Types and constructors

    using coordinate   = OffsetCoordinateType;
    using aspect_ratio = OffsetCoordinateType;

    struct cartesian_layout_storage
    {
        /**
         * Creates storage whose minimum is the origin and whose maximum is `ar`.
         *
         * @param ar Maximum coordinate.
         */
        explicit cartesian_layout_storage(const aspect_ratio& ar) noexcept : minimum{0, 0, 0}, dimension{ar}
        {
            assert(minimum.x <= dimension.x && minimum.y <= dimension.y && minimum.z <= dimension.z &&
                   "Layout maximum must not be below the origin");
        }

        /** Minimum coordinate. */
        coordinate minimum;
        /** Maximum coordinate. */
        aspect_ratio dimension;
    };

    static constexpr auto min_fanin_size = 0u;  // NOLINT(readability-identifier-naming): mockturtle requirement
    static constexpr auto max_fanin_size = 3u;  // NOLINT(readability-identifier-naming): mockturtle requirement

    using base_type = cartesian_layout;

    using storage = std::shared_ptr<cartesian_layout_storage>;

    /**
     * Standard constructor. The given aspect ratio points to the highest possible coordinate in the layout. That means
     * in the ASCII layout above `ar = (3,2)`. Consequently, with `ar = (0,0)`, the layout has exactly one coordinate.
     *
     * @param ar Highest possible position in the layout.
     * @pre No component of `ar` is below the origin.
     */
    explicit cartesian_layout(const aspect_ratio& ar = {}) :
            strg{std::make_shared<cartesian_layout_storage>(initialize_dimension(ar))}
    {}
    /**
     * Copy constructor from another layout's storage.
     *
     * @param s Storage of another cartesian_layout.
     */
    explicit cartesian_layout(std::shared_ptr<cartesian_layout_storage> s) : strg{std::move(s)} {}
    /**
     * Clones the layout returning a deep copy.
     *
     * @return Deep copy of the layout.
     */
    [[nodiscard]] cartesian_layout clone() const noexcept
    {
        return cartesian_layout(std::make_shared<cartesian_layout_storage>(*strg));
    }
    /**
     * Creates and returns a coordinate in the layout from the given x-, y-, and z-values.
     *
     * @note This function is equivalent to calling `OffsetCoordinateType(x, y, z)`.
     *
     * @tparam X x-type.
     * @tparam Y y-type.
     * @tparam Z z-type.
     * @param x x-value.
     * @param y y-value.
     * @param z z-value.
     * @return A coordinate in the layout of type `OffsetCoordinateType`.
     */
    template <typename X, typename Y, typename Z = uint64_t>
    constexpr OffsetCoordinateType coord(const X x, const Y y, const Z z = 0ul) const noexcept
    {
        return OffsetCoordinateType(x, y, z);
    }

#pragma endregion

#pragma region Structural properties
    /**
     * Returns the layout's minimum x-coordinate.
     *
     * @return Minimum x-coordinate.
     */
    [[nodiscard]] auto x_min() const noexcept
    {
        return strg->minimum.x;
    }
    /**
     * Returns the layout's minimum y-coordinate.
     *
     * @return Minimum y-coordinate.
     */
    [[nodiscard]] auto y_min() const noexcept
    {
        return strg->minimum.y;
    }
    /**
     * Returns the layout's minimum z-coordinate.
     *
     * @return Minimum z-coordinate.
     */
    [[nodiscard]] auto z_min() const noexcept
    {
        return strg->minimum.z;
    }
    /**
     * Returns the layout's x-dimension, i.e., returns the biggest x-value that still belongs to the layout.
     *
     * @return x-dimension.
     */
    [[nodiscard]] auto x() const noexcept
    {
        return strg->dimension.x;
    }
    /**
     * Returns the layout's y-dimension, i.e., returns the biggest y-value that still belongs to the layout.
     *
     * @return y-dimension.
     */
    [[nodiscard]] auto y() const noexcept
    {
        return strg->dimension.y;
    }
    /**
     * Returns the layout's z-dimension, i.e., returns the biggest z-value that still belongs to the layout.
     *
     * @return z-dimension.
     */
    [[nodiscard]] auto z() const noexcept
    {
        return strg->dimension.z;
    }
    /**
     * Returns the distance between the minimum and maximum x-coordinates.
     *
     * @return Layout size in x-direction.
     */
    [[nodiscard]] uint64_t x_size() const noexcept
    {
        return dimension_size(x_min(), x());
    }
    /**
     * Returns the distance between the minimum and maximum y-coordinates.
     *
     * @return Layout size in y-direction.
     */
    [[nodiscard]] uint64_t y_size() const noexcept
    {
        return dimension_size(y_min(), y());
    }
    /**
     * Returns the distance between the minimum and maximum z-coordinates.
     *
     * @return Layout size in z-direction.
     */
    [[nodiscard]] uint64_t z_size() const noexcept
    {
        return dimension_size(z_min(), z());
    }
    /**
     * Returns the layout's number of faces depending on the coordinate type.
     *
     * @return Area of layout.
     */
    [[nodiscard]] auto area() const noexcept
    {
        if constexpr (std::is_same_v<coordinate, coords::siqad>)
        {
            return (x_size() + 1) * ((2 * y_size()) + z_size() + 1);
        }

        return (x_size() + 1) * (y_size() + 1);
    }
    /**
     * Returns the layout's number of coordinate positions.
     *
     * @return Volume of layout.
     */
    [[nodiscard]] auto volume() const noexcept
    {
        if constexpr (std::is_same_v<coordinate, coords::siqad>)
        {
            return area();
        }

        return area() * (z_size() + 1);
    }
    /**
     * Updates the layout's dimensions, effectively resizing it.
     *
     * @param ar New aspect ratio.
     * @pre No component of `ar` is below the origin.
     */
    void resize(const aspect_ratio& ar) noexcept
    {
        resize(coordinate{0, 0, 0}, ar);
    }
    /**
     * Updates the layout's inclusive minimum and maximum coordinates, effectively resizing it.
     *
     * @param minimum New minimum coordinate.
     * @param maximum New maximum coordinate.
     * @pre No component of `minimum` exceeds the corresponding component of `maximum`.
     */
    void resize(const coordinate& minimum, const coordinate& maximum) noexcept
    {
        const auto initialized_minimum = initialize_minimum(minimum);
        const auto initialized_maximum = initialize_dimension(maximum);

        assert(initialized_minimum.x <= initialized_maximum.x && initialized_minimum.y <= initialized_maximum.y &&
               initialized_minimum.z <= initialized_maximum.z &&
               "Minimum coordinate must not exceed maximum coordinate");

        strg->minimum   = initialized_minimum;
        strg->dimension = initialized_maximum;
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
    [[nodiscard]] constexpr OffsetCoordinateType north(const OffsetCoordinateType& c) const noexcept
    {
        auto nc = c;

        if (c.y <= y_min())
        {
            if (c.y < y_min())
            {
                nc.d = 1;
            }

            return nc;
        }

        --nc.y;

        return nc;
    }
    /**
     * Returns the coordinate that is located in north-eastern direction of a given coordinate `c`, i.e., the face
     * whose x-dimension is higher by 1 and whose y-dimension is lower by 1. If `c`'s x-dimension is already at maximum
     * or `c`'s y-dimension is already at minimum, `c` is returned instead.
     *
     * @param c Coordinate whose north-eastern counterpart is desired.
     * @return Coordinate directly north-eastern of `c`.
     */
    [[nodiscard]] constexpr OffsetCoordinateType north_east(const OffsetCoordinateType& c) const noexcept
    {
        auto nec = c;

        if (c.x >= x() || c.y <= y_min())
        {
            if (c.x > x() || c.y < y_min())
            {
                nec.d = 1;
            }

            return nec;
        }

        ++nec.x;
        --nec.y;

        return nec;
    }
    /**
     * Returns the coordinate that is directly adjacent in eastern direction of a given coordinate `c`, i.e., the face
     * whose x-dimension is higher by 1. If `c`'s x-dimension is already at maximum, `c` is returned instead.
     *
     * @param c Coordinate whose eastern counterpart is desired.
     * @return Coordinate adjacent and east of `c`.
     */
    [[nodiscard]] OffsetCoordinateType east(const OffsetCoordinateType& c) const noexcept
    {
        auto ec = c;

        if (c.x > x())
        {
            ec.d = 1;
        }
        else if (c.x < x())
        {
            ++ec.x;
        }

        return ec;
    }
    /**
     * Returns the coordinate that is located in south-eastern direction of a given coordinate `c`, i.e., the face
     * whose x-dimension and y-dimension are higher by 1. If `c`'s x-dimension or y-dimension are already at maximum,
     * `c` is returned instead.
     *
     * @param c Coordinate whose south-eastern counterpart is desired.
     * @return Coordinate directly south-eastern of `c`.
     */
    [[nodiscard]] OffsetCoordinateType south_east(const OffsetCoordinateType& c) const noexcept
    {
        auto sec = c;

        if (c.x > x() || c.y > y())
        {
            sec.d = 1;
        }
        else if (c.x < x() && c.y < y())
        {
            ++sec.x;
            ++sec.y;
        }

        return sec;
    }
    /**
     * Returns the coordinate that is directly adjacent in southern direction of a given coordinate `c`, i.e., the face
     * whose y-dimension is higher by 1. If `c`'s y-dimension is already at maximum, `c` is returned instead.
     *
     * @param c Coordinate whose southern counterpart is desired.
     * @return Coordinate adjacent and south of `c`.
     */
    [[nodiscard]] OffsetCoordinateType south(const OffsetCoordinateType& c) const noexcept
    {
        auto sc = c;

        if (c.y > y())
        {
            sc.d = 1;
        }
        else if (c.y < y())
        {
            ++sc.y;
        }

        return sc;
    }
    /**
     * Returns the coordinate that is located in south-western direction of a given coordinate `c`, i.e., the face
     * whose x-dimension is lower by 1 and whose y-dimension is higher by 1. If `c`'s x-dimension is already at minimum
     * or `c`'s y-dimension is already at maximum, `c` is returned instead.
     *
     * @param c Coordinate whose south-western counterpart is desired.
     * @return Coordinate directly south-western of `c`.
     */
    [[nodiscard]] OffsetCoordinateType south_west(const OffsetCoordinateType& c) const noexcept
    {
        auto swc = c;

        if (c.x <= x_min() || c.y >= y())
        {
            if (c.x < x_min() || c.y > y())
            {
                swc.d = 1;
            }

            return swc;
        }

        --swc.x;
        ++swc.y;

        return swc;
    }
    /**
     * Returns the coordinate that is directly adjacent in western direction of a given coordinate `c`, i.e., the face
     * whose x-dimension is lower by 1. If `c`'s x-dimension is already at minimum, `c` is returned instead.
     *
     * @param c Coordinate whose western counterpart is desired.
     * @return Coordinate adjacent and west of `c`.
     */
    [[nodiscard]] constexpr OffsetCoordinateType west(const OffsetCoordinateType& c) const noexcept
    {
        auto wc = c;

        if (c.x <= x_min())
        {
            if (c.x < x_min())
            {
                wc.d = 1;
            }

            return wc;
        }

        --wc.x;

        return wc;
    }
    /**
     * Returns the coordinate that is located in north-western direction of a given coordinate `c`, i.e., the face
     * whose x-dimension and y-dimension are lower by 1. If `c`'s x-dimension or y-dimension are already at minimum, `c`
     * is returned instead.
     *
     * @param c Coordinate whose north-western counterpart is desired.
     * @return Coordinate directly north-western of `c`.
     */
    [[nodiscard]] constexpr OffsetCoordinateType north_west(const OffsetCoordinateType& c) const noexcept
    {
        auto nwc = c;

        if (c.x <= x_min() || c.y <= y_min())
        {
            if (c.x < x_min() || c.y < y_min())
            {
                nwc.d = 1;
            }

            return nwc;
        }

        --nwc.x;
        --nwc.y;

        return nwc;
    }
    /**
     * Returns the coordinate that is directly above a given coordinate `c`, i.e., the face whose z-dimension is higher
     * by 1. If `c`'s z-dimension is already at maximum, `c` is returned instead.
     *
     * @param c Coordinate whose above counterpart is desired.
     * @return Coordinate directly above `c`.
     */
    [[nodiscard]] OffsetCoordinateType above(const OffsetCoordinateType& c) const noexcept
    {
        auto ac = c;

        if (c.z > z())
        {
            ac.d = 1;
        }
        else if (c.z < z())
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
    [[nodiscard]] constexpr OffsetCoordinateType below(const OffsetCoordinateType& c) const noexcept
    {
        auto bc = c;

        if (c.z <= z_min())
        {
            if (c.z < z_min())
            {
                bc.d = 1;
            }

            return bc;
        }

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
    [[nodiscard]] constexpr bool is_north_of(const OffsetCoordinateType& c1,
                                             const OffsetCoordinateType& c2) const noexcept
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
    [[nodiscard]] bool is_east_of(const OffsetCoordinateType& c1, const OffsetCoordinateType& c2) const noexcept
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
    [[nodiscard]] bool is_south_of(const OffsetCoordinateType& c1, const OffsetCoordinateType& c2) const noexcept
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
    [[nodiscard]] constexpr bool is_west_of(const OffsetCoordinateType& c1,
                                            const OffsetCoordinateType& c2) const noexcept
    {
        return c1 != c2 && west(c1) == c2;
    }
    /**
     * Returns `true` iff coordinate `c2` is either directly north, east, south, or west of coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is either directly north, east, south, or west of `c1`.
     */
    [[nodiscard]] bool is_adjacent_of(const OffsetCoordinateType& c1, const OffsetCoordinateType& c2) const noexcept
    {
        return is_north_of(c1, c2) || is_east_of(c1, c2) || is_south_of(c1, c2) || is_west_of(c1, c2);
    }
    /**
     * Similar to `is_adjacent_of` but also considers `c1`'s elevation, i.e., if `c2` is adjacent to `above(c1)` or
     * `below(c1)`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is either directly north, east, south, or west of `c1` or `c1`'s elevations.
     */
    [[nodiscard]] bool is_adjacent_elevation_of(const OffsetCoordinateType& c1,
                                                const OffsetCoordinateType& c2) const noexcept
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
    [[nodiscard]] bool is_above(const OffsetCoordinateType& c1, const OffsetCoordinateType& c2) const noexcept
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
    [[nodiscard]] constexpr bool is_below(const OffsetCoordinateType& c1, const OffsetCoordinateType& c2) const noexcept
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
    [[nodiscard]] constexpr bool is_northwards_of(const OffsetCoordinateType& c1,
                                                  const OffsetCoordinateType& c2) const noexcept
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
    [[nodiscard]] constexpr bool is_eastwards_of(const OffsetCoordinateType& c1,
                                                 const OffsetCoordinateType& c2) const noexcept
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
    [[nodiscard]] constexpr bool is_southwards_of(const OffsetCoordinateType& c1,
                                                  const OffsetCoordinateType& c2) const noexcept
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
    [[nodiscard]] constexpr bool is_westwards_of(const OffsetCoordinateType& c1,
                                                 const OffsetCoordinateType& c2) const noexcept
    {
        return (c1.z == c2.z) && (c1.y == c2.y) && (c1.x > c2.x);
    }
    /**
     * Returns whether the given coordinate is located at the layout's northern border where y is minimal.
     *
     * @param c Coordinate to check for border location.
     * @return `true` iff `c` is located at the layout's northern border.
     */
    [[nodiscard]] constexpr bool is_at_northern_border(const OffsetCoordinateType& c) const noexcept
    {
        return c.y == y_min();
    }
    /**
     * Returns whether the given coordinate is located at the layout's eastern border where x is maximal.
     *
     * @param c Coordinate to check for border location.
     * @return `true` iff `c` is located at the layout's northern border.
     */
    [[nodiscard]] bool is_at_eastern_border(const OffsetCoordinateType& c) const noexcept
    {
        return c.x == x();
    }
    /**
     * Returns whether the given coordinate is located at the layout's southern border where y is maximal.
     *
     * @param c Coordinate to check for border location.
     * @return `true` iff `c` is located at the layout's southern border.
     */
    [[nodiscard]] bool is_at_southern_border(const OffsetCoordinateType& c) const noexcept
    {
        return c.y == y();
    }
    /**
     * Returns whether the given coordinate is located at the layout's western border where x is minimal.
     *
     * @param c Coordinate to check for border location.
     * @return `true` iff `c` is located at the layout's western border.
     */
    [[nodiscard]] constexpr bool is_at_western_border(const OffsetCoordinateType& c) const noexcept
    {
        return c.x == x_min();
    }
    /**
     * Returns whether the given coordinate is located at any of the layout's borders where x or y are either minimal or
     * maximal.
     *
     * @param c Coordinate to check for border location.
     * @return `true` iff `c` is located at any of the layout's borders.
     */
    [[nodiscard]] bool is_at_any_border(const OffsetCoordinateType& c) const noexcept
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
    [[nodiscard]] OffsetCoordinateType northern_border_of(const OffsetCoordinateType& c) const noexcept
    {
        return {c.x, y_min(), c.z};
    }
    /**
     * Returns the coordinate with the same y and z values as a given coordinate but that is located at the layout's
     * eastern border.
     *
     * @param c Coordinate whose border counterpart is desired.
     * @return The eastern border equivalent of `c`.
     */
    [[nodiscard]] OffsetCoordinateType eastern_border_of(const OffsetCoordinateType& c) const noexcept
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
    [[nodiscard]] OffsetCoordinateType southern_border_of(const OffsetCoordinateType& c) const noexcept
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
    [[nodiscard]] OffsetCoordinateType western_border_of(const OffsetCoordinateType& c) const noexcept
    {
        return {x_min(), c.y, c.z};
    }
    /**
     * Returns whether the given coordinate is located in the ground layer where z is minimal.
     *
     * @param c Coordinate to check for elevation.
     * @return `true` iff `c` is in ground layer.
     */
    [[nodiscard]] constexpr bool is_ground_layer(const OffsetCoordinateType& c) const noexcept
    {
        return c.z == z_min();
    }
    /**
     * Returns whether the given coordinate is located in a crossing layer where z is not minimal.
     *
     * @param c Coordinate to check for elevation.
     * @return `true` iff `c` is in a crossing layer.
     */
    [[nodiscard]] constexpr bool is_crossing_layer(const OffsetCoordinateType& c) const noexcept
    {
        return c.z > z_min();
    }
    /**
     * Returns whether the given coordinate is located within the layout bounds.
     *
     * @param c Coordinate to check for boundary.
     * @return `true` iff `c` is located within the layout bounds.
     */
    [[nodiscard]] constexpr bool is_within_bounds(const OffsetCoordinateType& c) const noexcept
    {
        return x_min() <= c.x && c.x <= x() && y_min() <= c.y && c.y <= y() && z_min() <= c.z && c.z <= z();
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
    [[nodiscard]] auto coordinates(const OffsetCoordinateType& start = {}, const OffsetCoordinateType& stop = {}) const
    {
        return std::ranges::subrange{
            coords::coordinate_iterator{strg->dimension, start.is_dead() ? strg->minimum : start, strg->minimum},
            coords::coordinate_iterator{strg->dimension, stop.is_dead() ? strg->dimension.get_dead() : stop,
                                        strg->minimum}};
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
    void foreach_coordinate(Fn&& fn, const OffsetCoordinateType& start = {},
                            const OffsetCoordinateType& stop = {}) const
    {
        mockturtle::detail::foreach_element(
            coords::coordinate_iterator{strg->dimension, start.is_dead() ? strg->minimum : start, strg->minimum},
            coords::coordinate_iterator{strg->dimension, stop.is_dead() ? strg->dimension.get_dead() : stop,
                                        strg->minimum},
            std::forward<Fn>(fn));
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
    [[nodiscard]] auto ground_coordinates(const OffsetCoordinateType& start = {},
                                          const OffsetCoordinateType& stop  = {}) const
    {
        assert((start.is_dead() || start.z == z_min()) && (stop.is_dead() || stop.z == z_min()));

        const auto ground_min = coordinate{x_min(), y_min(), z_min()};
        const auto ground_max = coordinate{x(), y(), z_min()};

        return std::ranges::subrange{
            coords::coordinate_iterator{ground_max, start.is_dead() ? ground_min : start, ground_min},
            coords::coordinate_iterator{ground_max, stop.is_dead() ? ground_max.get_dead() : stop, ground_min}};
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
    void foreach_ground_coordinate(Fn&& fn, const OffsetCoordinateType& start = {},
                                   const OffsetCoordinateType& stop = {}) const
    {
        assert((start.is_dead() || start.z == z_min()) && (stop.is_dead() || stop.z == z_min()));

        const auto ground_min = coordinate{x_min(), y_min(), z_min()};
        const auto ground_max = coordinate{x(), y(), z_min()};

        mockturtle::detail::foreach_element(
            coords::coordinate_iterator{ground_max, start.is_dead() ? ground_min : start, ground_min},
            coords::coordinate_iterator{ground_max, stop.is_dead() ? ground_max.get_dead() : stop, ground_min},
            std::forward<Fn>(fn));
    }
    /**
     * Returns a container that contains all coordinates that are adjacent to a given one. Thereby, only cardinal
     * directions are being considered, i.e., the container contains all coordinates `ac` for which `is_adjacent(c, ac)`
     * returns `true`.
     *
     * Coordinates that are outside of the layout bounds are not considered. Thereby, the size of the returned container
     * is at most 4.
     *
     * @param c Coordinate whose adjacent ones are desired.
     * @return A container that contains all of `c`'s adjacent coordinates.
     */
    auto adjacent_coordinates(const OffsetCoordinateType& c) const noexcept
    {
        std::vector<OffsetCoordinateType> cnt{};
        cnt.reserve(max_fanin_size + 1);  // reserve memory

        foreach_adjacent_coordinate(c, [&cnt](const auto& ac) noexcept { cnt.push_back(ac); });

        return cnt;
    }
    /**
     * Applies a function to all coordinates adjacent to a given one. Thereby, only cardinal directions are being
     * considered, i.e., the function is applied to all coordinates `ac` for which `is_adjacent(c, ac)` returns `true`.
     *
     * Coordinates that are outside of the layout bounds are not considered. Thereby, at most 4 coordinates are touched.
     *
     * @tparam Fn Functor type.
     * @param c Coordinate whose adjacent ones are desired.
     * @param fn Functor to apply to each of `c`'s adjacent coordinates.
     */
    template <typename Fn>
    void foreach_adjacent_coordinate(const OffsetCoordinateType& c, Fn&& fn) const
    {
        const auto apply_if_not_c = [&c, &fn](const auto& cardinal) noexcept
        {
            if (cardinal != c)
            {
                std::invoke(std::forward<Fn>(fn), cardinal);
            }
        };

        apply_if_not_c(north(c));
        apply_if_not_c(east(c));
        apply_if_not_c(south(c));
        apply_if_not_c(west(c));
    }
    /**
     * Returns a container that contains all coordinates pairs of opposing adjacent coordinates with respect to a given
     * one. In this Cartesian layout, the container will contain (`north(c)`, `south(c)`) and (`east(c)`, `west(c)`).
     *
     * This function comes in handy when straight lines on the layout are to be examined.
     *
     * Coordinates outside of the layout bounds are not being considered.
     *
     * @param c Coordinate whose opposite ones are desired.
     * @return A container that contains pairs of `c`'s opposing coordinates.
     */
    auto adjacent_opposite_coordinates(const OffsetCoordinateType& c) const noexcept
    {
        std::vector<std::pair<OffsetCoordinateType, OffsetCoordinateType>> cnt{};
        cnt.reserve((max_fanin_size + 1) / 2);  // reserve memory

        foreach_adjacent_opposite_coordinates(c, [&cnt](const auto& cp) noexcept { cnt.push_back(cp); });

        return cnt;
    }
    /**
     * Applies a function to all opposing coordinate pairs adjacent to a given one. In this Cartesian layout, the
     * function will be applied to (`north(c)`, `south(c)`) and (`east(c)`, `west(c)`).
     *
     * @tparam Fn Functor type.
     * @param c Coordinate whose opposite adjacent ones are desired.
     * @param fn Functor to apply to each of `c`'s opposite adjacent coordinate pairs.
     */
    template <typename Fn>
    void foreach_adjacent_opposite_coordinates(const OffsetCoordinateType& c, Fn&& fn) const
    {
        const auto apply_if_not_c = [&c, &fn](auto cardinal1, auto cardinal2) noexcept
        {
            if (cardinal1 != c && cardinal2 != c)
            {
                std::invoke(std::forward<Fn>(fn), std::make_pair(std::move(cardinal1), std::move(cardinal2)));
            }
        };

        apply_if_not_c(north(c), south(c));
        apply_if_not_c(east(c), west(c));
    }

#pragma endregion

  private:
    storage strg;

    /**
     * Returns the distance between a minimum and maximum coordinate component.
     *
     * @tparam CoordinateValue Coordinate component type.
     * @param minimum Minimum component value.
     * @param maximum Maximum component value.
     * @return Distance between `minimum` and `maximum`.
     */
    template <typename CoordinateValue>
    [[nodiscard]] static constexpr uint64_t dimension_size(const CoordinateValue minimum,
                                                           const CoordinateValue maximum) noexcept
    {
        return static_cast<uint64_t>(static_cast<int64_t>(maximum) - static_cast<int64_t>(minimum));
    }

    /**
     * Initializes a Cartesian layout's minimum coordinate. For SiQAD coordinates, the z-value is set to 0 such that
     * only complete dimer rows are considered.
     *
     * @param coord Minimum coordinate to initialize.
     * @return Initialized minimum coordinate.
     */
    constexpr OffsetCoordinateType initialize_minimum(const OffsetCoordinateType& coord) const
    {
        if constexpr (std::is_same_v<OffsetCoordinateType, coords::siqad>)
        {
            return OffsetCoordinateType{coord.x, coord.y, 0};
        }

        return OffsetCoordinateType{coord.x, coord.y, coord.z};
    }

    /*
     * Initializer for a cartesian layout dimension. When using SiQAD coordinates, it will default the z value to 1,
     * such that only complete dimer rows are considered.
     */
    constexpr OffsetCoordinateType initialize_dimension(const OffsetCoordinateType& coord) const
    {
        if constexpr (std::is_same_v<OffsetCoordinateType, coords::siqad>)
        {
            return OffsetCoordinateType{coord.x, coord.y, 1};
        }

        return OffsetCoordinateType{coord.x, coord.y, coord.z};
    }
};

}  // namespace fiction::layouts
