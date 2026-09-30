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
 * @brief Base class of all layouts that hosts the signed coordinate type shared by every layout topology.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 * @author Willem Lambooy (wlambooy)
 */

#pragma once

#include "fiction/utils/math/math_utils.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>

namespace fiction::layouts
{

/**
 * Base class of all layouts. It defines the signed coordinate type that every layout topology (Cartesian, shifted
 * Cartesian, and hexagonal) exposes under the same API.
 */
class layout_base
{
  public:
    /**
     * Signed coordinates.
     *
     * An coordinate coordinate defines a location via an coordinate from a fixed point (origin). Each axis is a signed
     * 32-bit integer. The default-constructed coordinate is invalid; it has all axes set to `INVALID_AXIS` and stands
     * for "no coordinate", e.g., a neighbor outside of a layout or the tile of a node that is not placed.
     *
     * Gate-level layouts pack a coordinate into a 64-bit signal with `explicit operator uint64_t`. This encoding holds
     * 31-bit signed x and y values and a single z bit.
     */
    struct coordinate
    {
        /**
         * Value of every axis of the invalid coordinate.
         */
        static constexpr int32_t INVALID_AXIS = std::numeric_limits<int32_t>::min();
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

        // NOLINTBEGIN(readability-identifier-naming)

        /**
         * Default constructor. Creates the invalid coordinate.
         */
        constexpr coordinate() noexcept : x{INVALID_AXIS}, y{INVALID_AXIS}, z{INVALID_AXIS} {}
        /**
         * Standard constructor. Creates a coordinate at (x_, y_, z_).
         *
         * @tparam X Type of x.
         * @tparam Y Type of y.
         * @tparam Z Type of z.
         * @param x_ x position.
         * @param y_ y position.
         * @param z_ z position.
         */
        template <class X, class Y, class Z>
        constexpr coordinate(X x_, Y y_, Z z_) noexcept :
                x{static_cast<int32_t>(x_)},
                y{static_cast<int32_t>(y_)},
                z{static_cast<int32_t>(z_)}
        {}
        /**
         * Standard constructor. Creates a coordinate at (x_, y_, 0).
         *
         * @tparam X Type of x.
         * @tparam Y Type of y.
         * @param x_ x position.
         * @param y_ y position.
         */
        template <class X, class Y>
        constexpr coordinate(X x_, Y y_) noexcept : x{static_cast<int32_t>(x_)}, y{static_cast<int32_t>(y_)}, z{0}
        {}
        /**
         * Standard constructor. Instantiates a coordinate from the 64-bit encoding of a gate-level signal, where the
         * positions are encoded in the following four parts (from MSB to LSB):
         *  - 1 bit for the invalid indicator
         *  - 1 bit for the z position
         *  - 31 bit for the y position in two's complement
         *  - 31 bit for the x position in two's complement
         *
         * A set invalid indicator yields the invalid coordinate.
         *
         * @param t Unsigned 64-bit integer to instantiate the coordinate from.
         */
        constexpr explicit coordinate(const uint64_t t) noexcept : coordinate{}
        {
            if ((t >> 63ull) == 0ull)
            {
                x = sign_extend_31(t & AXIS_MASK);
                y = sign_extend_31((t >> 31ull) & AXIS_MASK);
                z = static_cast<int32_t>((t >> 62ull) & 1ull);
            }
        }

        // NOLINTEND(readability-identifier-naming)

        /**
         * Allows explicit conversion to `uint64_t`, the encoding of gate-level signals. See the constructor for the
         * encoding. For non-negative x and y, it equals the concatenation of the bits `0`, `z`, `y`, and `x`. An
         * invalid coordinate encodes as `0x8000000000000000`. Coordinates outside of the representable range (x and y
         * in
         * \f$[-2^{30}, 2^{30} - 1]\f$, z in \f$\{0, 1\}\f$) lose their higher bits.
         */
        explicit constexpr operator uint64_t() const noexcept
        {
            if (!is_valid())
            {
                return INVALID_CODE;
            }

            return ((static_cast<uint64_t>(z) & 1ull) << 62ull) | ((static_cast<uint64_t>(y) & AXIS_MASK) << 31ull) |
                   (static_cast<uint64_t>(x) & AXIS_MASK);
        }
        /**
         * Returns whether the coordinate is valid, i.e., whether it differs from the default-constructed coordinate.
         *
         * @return `true` iff the coordinate is valid.
         */
        [[nodiscard]] constexpr bool is_valid() const noexcept
        {
            return x != INVALID_AXIS;
        }
        /**
         * Returns whether the coordinate fits the 64-bit signal encoding, i.e., x and y are 31-bit signed values and z
         * is either 0 or 1.
         *
         * @return `true` iff the signal encoding of the coordinate can be decoded to the coordinate itself.
         */
        [[nodiscard]] constexpr bool fits_signal() const noexcept
        {
            constexpr auto max_axis = static_cast<int32_t>((1ull << 30ull) - 1ull);
            constexpr auto min_axis = -max_axis - 1;

            return x >= min_axis && x <= max_axis && y >= min_axis && y <= max_axis && (z == 0 || z == 1);
        }
        /**
         * Wraps the coordinate with respect to the given aspect ratio by iterating over the dimensions in the order x,
         * y, z. For any dimension of the coordinate that is strictly larger than the associated dimension of the aspect
         * ratio, this dimension will be wrapped to zero, and the next dimension is increased. The resulting coordinate
         * becomes invalid if it is not contained in the aspect ratio after iterating. An example use case of this
         * function is the coordinate iterator, which implements iterator advancing by first incrementing the x
         * dimension, then wrapping the coordinate to the boundary within to enumerate.
         *
         * @param aspect_ratio Aspect ratio to wrap the coordinate to.
         */
        void wrap(const coordinate& aspect_ratio) noexcept
        {
            if (x > aspect_ratio.x)
            {
                x = 0;
                ++y;
            }

            if (y > aspect_ratio.y)
            {
                y = 0;
                ++z;
            }

            if (z > aspect_ratio.z)
            {
                *this = coordinate{};
            }
        }
        /**
         * Compares against another coordinate for equality. All invalid coordinates are equal.
         *
         * @param other Right-hand side coordinate.
         * @return `true` iff both coordinates are identical.
         */
        constexpr bool operator==(const coordinate& other) const noexcept
        {
            // a user-provided comparison keeps standard libraries from treating the 12-byte coordinate as bitwise
            // comparable and vectorizing `std::find` for it, which MSVC's library rejects for this size
            return x == other.x && y == other.y && z == other.z;
        }
        /**
         * Compares against another coordinate for inequality.
         *
         * @param other Right-hand side coordinate.
         * @return `true` iff both coordinates are not identical.
         */
        constexpr bool operator!=(const coordinate& other) const noexcept
        {
            return !(*this == other);
        }
        /**
         * Determine whether this coordinate is "less than" another one. This is the case if z is smaller, or if z is
         * equal but y is smaller, or if z and y are equal but x is smaller.
         *
         * @param other Right-hand side coordinate.
         * @return `true` iff this coordinate is "less than" the other coordinate.
         */
        constexpr bool operator<(const coordinate& other) const noexcept
        {
            if (z != other.z)
            {
                return z < other.z;
            }

            if (y != other.y)
            {
                return y < other.y;
            }

            return x < other.x;
        }
        /**
         * Determine whether this coordinate is "greater than" another one. This is the case if the other one is "less
         * than".
         *
         * @param other Right-hand side coordinate.
         * @return `true` iff this coordinate is "greater than" the other coordinate.
         */
        constexpr bool operator>(const coordinate& other) const noexcept
        {
            return other < *this;
        }
        /**
         * Determine whether this coordinate is "less than or equal to" another one. This is the case if this one is not
         * "greater than" the other.
         *
         * @param other Right-hand side coordinate.
         * @return `true` iff this coordinate is "less than or equal to" the other coordinate.
         */
        constexpr bool operator<=(const coordinate& other) const noexcept
        {
            return !(*this > other);
        }
        /**
         * Determine whether this coordinate is "greater than or equal to" another one. This is the case if this one is
         * not "less than" the other.
         *
         * @param other Right-hand side coordinate.
         * @return `true` iff this coordinate is "greater than or equal to" the other coordinate.
         */
        constexpr bool operator>=(const coordinate& other) const noexcept
        {
            return !(*this < other);
        }
        /**
         * Returns a string representation of the coordinate of the form `"(x, y, z)"`.
         *
         * @return String representation of the form `"(x, y, z)"`.
         */
        [[nodiscard]] std::string str() const
        {
            return fmt::format("({},{},{})", x, y, z);
        }

      private:
        /**
         * Mask of the 31 bits of one axis in the signal encoding.
         */
        static constexpr uint64_t AXIS_MASK = (1ull << 31ull) - 1ull;
        /**
         * Signal encoding of the invalid coordinate.
         */
        static constexpr uint64_t INVALID_CODE = 1ull << 63ull;
        /**
         * Sign-extends a 31-bit two's complement value.
         *
         * @param v Value with its 31 low bits set.
         * @return The represented signed value.
         */
        static constexpr int32_t sign_extend_31(const uint64_t v) noexcept
        {
            return static_cast<int32_t>((v ^ (1ull << 30ull)) - (1ull << 30ull));
        }
    };

    /**
     * Aspect ratios use the coordinate type. An aspect ratio names the highest coordinate that belongs to a layout.
     */
    using aspect_ratio = coordinate;

    /**
     * An iterator type that allows to enumerate coordinates in order within a boundary.
     */
    class coordinate_iterator
    {
      public:
        using value_type = coordinate;
        /**
         * Default constructor. Required so that iterator satisfies `std::semiregular`, which in turn is required
         * for it to serve as its own `std::sentinel_for` (e.g., for `std::ranges::subrange` CTAD).
         */
        constexpr coordinate_iterator() noexcept = default;
        /**
         * Standard constructor. Initializes the iterator with a starting position and the boundary within to enumerate.
         *
         * With `dimension = (1, 2, 1)` and `start = (0, 0, 0)`, the following order would be enumerated:
         *
         * - (0, 0, 0)
         * - (1, 0, 0)
         * - (0, 1, 0)
         * - (1, 1, 0)
         * - (0, 2, 0)
         * - (1, 2, 0)
         * - (0, 0, 1)
         * - (1, 0, 1)
         * - (0, 1, 1)
         * - (1, 1, 1)
         * - (0, 2, 1)
         * - (1, 2, 1)
         *
         * iterator is compatible with the STL forward_iterator category. Does not iterate over negative coordinates.
         *
         * @param dimension Boundary within to enumerate. Iteration wraps at its limits.
         * @param start Starting coordinate to enumerate first.
         */
        constexpr explicit coordinate_iterator(const coordinate& dimension, const coordinate& start) noexcept :
                bound{dimension},
                current{start}
        {
            // an invalid start marks the end of the enumeration
            if (!current.is_valid())
            {
                return;
            }

            // Make sure the start iterator is within the given boundary; first handle negative coordinates ...
            current.x = std::max(current.x, 0);
            current.y = std::max(current.y, 0);
            current.z = std::max(current.z, 0);

            // ... then handle coordinates that are beyond the given boundary.
            current.wrap(bound);
        }
        /**
         * Increments the iterator, while keeping it within the boundary. Also defined on iterators that are out of
         * bounds.
         *
         * @return Reference to the incremented iterator.
         */
        constexpr coordinate_iterator& operator++() noexcept
        {
            if (current != bound)
            {
                ++current.x;

                current.wrap(bound);
            }
            else
            {
                current = coordinate{};
            }

            return *this;
        }

        constexpr coordinate_iterator operator++(int) noexcept
        {
            const auto result{*this};

            ++(*this);

            return result;
        }

        constexpr coordinate operator*() const noexcept
        {
            return current;
        }

        constexpr bool operator==(const coordinate_iterator& other) const noexcept
        {
            return (current == other.current);
        }

        constexpr bool operator!=(const coordinate_iterator& other) const noexcept
        {
            return !(*this == other);
        }

        constexpr bool operator<(const coordinate_iterator& other) const noexcept
        {
            return (current < other.current);
        }

        constexpr bool operator<=(const coordinate_iterator& other) const noexcept
        {
            return (current <= other.current);
        }

      private:
        /**
         * Boundary within to enumerate. Not `const`: `std::input_or_output_iterator` requires `iterator` to be
         * `std::movable`, which in turn requires it to be assignable.
         */
        coordinate bound;

        coordinate current;
    };
};

inline std::ostream& operator<<(std::ostream& os, const layout_base::coordinate& t)
{
    os << t.str();
    return os;
}

/**
 * Computes the area of a given coordinate assuming its origin is (0, 0, 0). Calculates \f$(|x| + 1) \cdot (|y| + 1)\f$.
 *
 * @tparam CoordinateType Coordinate type.
 * @param coord Coordinate.
 * @return Area of coord.
 */
template <typename CoordinateType>
uint64_t area_of(const CoordinateType& coord) noexcept
{
    return (static_cast<uint64_t>(fiction::utils::math::integral_abs(coord.x)) + 1) *
           (static_cast<uint64_t>(fiction::utils::math::integral_abs(coord.y)) + 1);
}
/**
 * Computes the volume of a given coordinate assuming its origin is (0, 0, 0). Calculates \f$(|x| + 1) \cdot (|y| + 1)
 * \cdot (|z| + 1)\f$.
 *
 * @tparam CoordinateType Coordinate type.
 * @param coord Coordinate.
 * @return Volume of coord.
 */
template <typename CoordinateType>
uint64_t volume_of(const CoordinateType& coord) noexcept
{
    return (static_cast<uint64_t>(fiction::utils::math::integral_abs(coord.x)) + 1) *
           (static_cast<uint64_t>(fiction::utils::math::integral_abs(coord.y)) + 1) *
           (static_cast<uint64_t>(fiction::utils::math::integral_abs(coord.z)) + 1);
}

}  // namespace fiction::layouts

// NOLINTBEGIN(cert-dcl58-cpp)

namespace std
{

// define std::hash overload for coordinates
template <>
struct hash<fiction::layouts::layout_base::coordinate>
{
    std::size_t operator()(const fiction::layouts::layout_base::coordinate& c) const noexcept
    {
        return std::hash<uint64_t>{}(static_cast<uint64_t>(c));
    }
};

/**
 * Makes `coordinate_iterator` compatible with STL iterator categories. `reference` and `difference_type` are required
 * for the iterator to satisfy `std::input_or_output_iterator` (e.g., for `std::ranges::subrange` CTAD).
 */
template <>
struct iterator_traits<fiction::layouts::layout_base::coordinate_iterator>
{
    using iterator_category = std::forward_iterator_tag;
    using value_type        = fiction::layouts::layout_base::coordinate;
    using reference         = fiction::layouts::layout_base::coordinate;
    using difference_type   = std::ptrdiff_t;
};

}  // namespace std

// NOLINTEND(cert-dcl58-cpp)

namespace fmt
{

// make coordinates compatible with fmt::format
template <>
struct formatter<fiction::layouts::layout_base::coordinate>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const fiction::layouts::layout_base::coordinate& c, FormatContext& ctx) const
    {
        return format_to(ctx.out(), runtime("({},{},{})"), c.x, c.y, c.z);
    }
};

}  // namespace fmt
