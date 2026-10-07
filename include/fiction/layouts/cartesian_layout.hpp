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
 * @brief Cartesian grid layout addressed by offset coordinates.
 * @author Marcel Walter (marcelwa)
 * @author Willem Lambooy (wlambooy)
 * @author Simon Hofmann (simon1hofmann)
 */

#pragma once

#include "fiction/layouts/layout_base.hpp"

#include <mockturtle/networks/detail/foreach.hpp>

#include <concepts>
#include <cstdint>
#include <functional>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <utility>
#include <vector>

namespace fiction::layouts
{

/**
 * A layout type that utilizes signed offset coordinates to represent a Cartesian grid. Its faces are organized in the
 * following way:
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
 */
class cartesian_layout : public layout_base
{
  public:
#pragma region Types and constructors

    /** Axis sizes. */
    using layout_base::extent;
    /** Signed coordinate values. */
    using layout_base::coordinate;

    /** Minimum number of incoming neighbors. */
    static constexpr auto min_fanin_size = 0u;  // NOLINT(readability-identifier-naming): mockturtle requirement
    /** Maximum number of incoming neighbors. */
    static constexpr auto max_fanin_size = 3u;  // NOLINT(readability-identifier-naming): mockturtle requirement

    /** Geometry base type. */
    using base_type = cartesian_layout;

    /**
     * Creates geometry with half-open, zero-origin bounds. The default extent is empty.
     * @param size Axis sizes.
     * @throws std::invalid_argument If a size exceeds the coordinate domain.
     * @throws std::out_of_range If the layer count exceeds two.
     */
    explicit cartesian_layout(const extent& size = {}) : dimension{checked(size)} {}
    /** @return Independent copy of the geometry. */
    [[nodiscard]] cartesian_layout clone() const noexcept
    {
        return *this;
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
     * @throws std::out_of_range If the layer count exceeds two. The dimensions remain unchanged.
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

#pragma region Cardinal operations
    // Coordinate predicates belong to the layout interface used by generic algorithms.
    // NOLINTBEGIN(readability-convert-member-functions-to-static): generic algorithms use the layout member interface
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
        return bounded_neighbor(c, 1, -1, 0);
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
        return bounded_neighbor(c, 1, 1, 0);
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
        return bounded_neighbor(c, -1, 1, 0);
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
        return bounded_neighbor(c, -1, -1, 0);
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
     * Returns `true` iff coordinate `c2` is either directly north, east, south, or west of coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is either directly north, east, south, or west of `c1`.
     */
    [[nodiscard]] bool is_adjacent_of(const coordinate& c1, const coordinate& c2) const noexcept
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
        const coordinate projected{static_cast<int32_t>(width() - 1), c.y, c.z};
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
        const coordinate projected{c.x, static_cast<int32_t>(height() - 1), c.z};
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
        return c.z == decltype(c.z){0};
    }
    /**
     * Returns whether the given coordinate is located in the crossing layer at z = 1.
     *
     * @param c Coordinate to check for elevation.
     * @return `true` iff `c.z` is 1.
     */
    [[nodiscard]] constexpr bool is_crossing_layer(const coordinate& c) const noexcept
    {
        return c.z == 1;
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
    [[nodiscard]] auto adjacent_coordinates(const coordinate& c) const
    {
        std::vector<coordinate> cnt{};
        cnt.reserve(max_fanin_size + 1);  // reserve memory

        foreach_adjacent_coordinate(c, [&cnt](const auto& ac) { cnt.push_back(ac); });

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
     * @param fn Functor invoked as an lvalue for each of `c`'s adjacent coordinates.
     */
    template <typename Fn>
    // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward): repeated calls require an lvalue callback.
    void foreach_adjacent_coordinate(const coordinate& c, Fn&& fn) const
    {
        const auto apply_if_present = [&fn](const auto& cardinal)
        {
            if (cardinal)
            {
                std::invoke(fn, *cardinal);
            }
        };

        apply_if_present(north(c));
        apply_if_present(east(c));
        apply_if_present(south(c));
        apply_if_present(west(c));
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
    [[nodiscard]] auto adjacent_opposite_coordinates(const coordinate& c) const
    {
        std::vector<std::pair<coordinate, coordinate>> cnt{};
        cnt.reserve((max_fanin_size + 1) / 2);  // reserve memory

        foreach_adjacent_opposite_coordinates(c, [&cnt](const auto& cp) { cnt.push_back(cp); });

        return cnt;
    }
    /**
     * Applies a function to all opposing coordinate pairs adjacent to a given one. In this Cartesian layout, the
     * function will be applied to (`north(c)`, `south(c)`) and (`east(c)`, `west(c)`).
     *
     * @tparam Fn Functor type.
     * @param c Coordinate whose opposite adjacent ones are desired.
     * @param fn Functor invoked as an lvalue for each of `c`'s opposite adjacent coordinate pairs.
     */
    template <typename Fn>
    // NOLINTNEXTLINE(cppcoreguidelines-missing-std-forward): repeated calls require an lvalue callback.
    void foreach_adjacent_opposite_coordinates(const coordinate& c, Fn&& fn) const
    {
        const auto apply_if_present = [&fn](auto cardinal1, auto cardinal2)
        {
            if (cardinal1 && cardinal2)
            {
                std::invoke(fn, std::make_pair(*cardinal1, *cardinal2));
            }
        };

        apply_if_present(north(c), south(c));
        apply_if_present(east(c), west(c));
    }

#pragma endregion

  private:
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
        if (nx < 0 || std::cmp_greater_equal(nx, width()) || ny < 0 || std::cmp_greater_equal(ny, height()) || nz < 0 ||
            std::cmp_greater_equal(nz, layers()))
        {
            return std::nullopt;
        }
        return coordinate{nx, ny, nz};
    }
    /** Independent axis sizes. */
    extent dimension{};
};

}  // namespace fiction::layouts
