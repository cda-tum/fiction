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

#include <fmt/format.h>

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

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
     * A coordinate defines a location relative to a fixed point (origin). Each axis is a signed 32-bit integer. The
     * default-constructed coordinate is the origin. Every signed 32-bit axis value identifies a position.
     */
    struct coordinate
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
         * Default constructor. Creates the origin.
         */
        constexpr coordinate() noexcept : x{0}, y{0}, z{0} {}
        /**
         * Standard constructor. Creates a coordinate at (coordinate_x, coordinate_y, coordinate_z).
         *
         * @tparam X Type of x.
         * @tparam Y Type of y.
         * @tparam Z Type of z.
         * @param coordinate_x x position.
         * @param coordinate_y y position.
         * @param coordinate_z z position.
         * @throws std::overflow_error If an axis is outside the signed 32-bit range.
         */
        template <std::integral X, std::integral Y, std::integral Z>
        constexpr coordinate(X coordinate_x, Y coordinate_y, Z coordinate_z) :
                x{checked_axis(coordinate_x)},
                y{checked_axis(coordinate_y)},
                z{checked_axis(coordinate_z)}
        {}
        /**
         * Standard constructor. Creates a coordinate at (coordinate_x, coordinate_y, 0).
         *
         * @tparam X Type of x.
         * @tparam Y Type of y.
         * @param coordinate_x x position.
         * @param coordinate_y y position.
         * @throws std::overflow_error If an axis is outside the signed 32-bit range.
         */
        template <std::integral X, std::integral Y>
        constexpr coordinate(X coordinate_x, Y coordinate_y) :
                x{checked_axis(coordinate_x)},
                y{checked_axis(coordinate_y)},
                z{0}
        {}
        /**
         * Compares against another coordinate for equality, axis by axis.
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
         * Converts an integral axis without narrowing.
         *
         * @tparam Axis Integral input type.
         * @param value Axis value.
         * @return Signed 32-bit axis.
         * @throws std::overflow_error If `value` is outside the signed 32-bit range.
         */
        template <std::integral Axis>
        static constexpr int32_t checked_axis(const Axis value)
        {
            if constexpr (std::numeric_limits<Axis>::digits > std::numeric_limits<int32_t>::digits)
            {
                if (value > static_cast<Axis>(std::numeric_limits<int32_t>::max()) ||
                    (std::signed_integral<Axis> && value < static_cast<Axis>(std::numeric_limits<int32_t>::min())))
                {
                    throw std::overflow_error("A coordinate axis is outside the signed 32-bit range");
                }
            }
            return static_cast<int32_t>(value);
        }
    };

    /**
     * Nonnegative axis sizes of a zero-origin layout. Each size is at most `INT32_MAX + 1`, so every contained
     * coordinate fits the signed coordinate domain. A zero size on any axis makes the geometry empty.
     */
    struct extent
    {
        /** Width in coordinates. */
        uint32_t width{0};
        /** Height in coordinates. */
        uint32_t height{0};
        /** Number of layers. */
        uint32_t layers{0};
        /** Creates an empty extent. */
        constexpr extent() noexcept = default;
        /**
         * Creates an extent from axis sizes. Two axis sizes describe one layer.
         * @tparam W Width type.
         * @tparam H Height type.
         * @tparam L Layer count type.
         * @param w Width.
         * @param h Height.
         * @param l Number of layers.
         * @throws std::invalid_argument If a size is negative or exceeds `INT32_MAX + 1`.
         */
        template <std::integral W, std::integral H, std::integral L = uint32_t>
        constexpr extent(const W w, const H h, const L l = 1) :
                width{checked_size(w)},
                height{checked_size(h)},
                layers{checked_size(l)}
        {}
        /** Compares all axis sizes. */
        constexpr bool operator==(const extent&) const noexcept = default;

      private:
        /**
         * Checks one size against the coordinate domain.
         * @tparam Axis Size type.
         * @param value Size to check.
         * @return Checked size.
         * @throws std::invalid_argument If the size is negative or exceeds `INT32_MAX + 1`.
         */
        template <std::integral Axis>
        static constexpr uint32_t checked_size(const Axis value)
        {
            if constexpr (std::signed_integral<Axis>)
            {
                if (value < 0)
                {
                    throw std::invalid_argument("A layout size must not be negative");
                }
            }
            if (static_cast<uint64_t>(value) > uint64_t{std::numeric_limits<int32_t>::max()} + 1)
            {
                throw std::invalid_argument("A layout size exceeds the signed coordinate domain");
            }
            return static_cast<uint32_t>(value);
        }
    };

  protected:
    /**
     * Validates extent values, including public axes edited after construction.
     * @param size Sizes to check.
     * @return Checked extent.
     * @throws std::invalid_argument If a size exceeds `INT32_MAX + 1`.
     */
    static constexpr extent checked(const extent& size)
    {
        return {size.width, size.height, size.layers};
    }

  public:
    /**
     * Forward iterator over half-open, zero-origin bounds. The end state is separate from coordinate values.
     */
    class coordinate_iterator
    {
      public:
        /** Iterator value type. */
        using value_type = coordinate;
        /** Creates an end iterator. */
        constexpr coordinate_iterator() noexcept = default;
        /**
         * Creates an iterator at a position, or the end when no position is given.
         * Negative axes clamp to zero. An axis beyond its size wraps to zero and advances the next axis once.
         * @param size Half-open bounds.
         * @param start First coordinate, or the end state.
         * @throws std::invalid_argument If a size exceeds the coordinate domain.
         */
        constexpr explicit coordinate_iterator(const extent&                   size,
                                               const std::optional<coordinate> start = std::nullopt) :
                bound{checked(size)},
                current{start.value_or(coordinate{})},
                ended{!start.has_value()}
        {
            if (ended || bound.width == 0 || bound.height == 0 || bound.layers == 0)
            {
                ended = true;
                return;
            }
            int64_t x = std::max(current.x, 0);
            int64_t y = std::max(current.y, 0);
            int64_t z = std::max(current.z, 0);
            if (std::cmp_greater_equal(x, bound.width))
            {
                x = 0;
                ++y;
            }
            if (std::cmp_greater_equal(y, bound.height))
            {
                y = 0;
                ++z;
            }
            ended = std::cmp_greater_equal(z, bound.layers);
            if (!ended)
            {
                current = coordinate{x, y, z};
            }
        }
        /** Advances the iterator. End iterators remain at the end. @return This iterator. */
        constexpr coordinate_iterator& operator++() noexcept
        {
            if (ended)
            {
                return *this;
            }
            if (static_cast<uint32_t>(current.x) + 1 < bound.width)
            {
                ++current.x;
            }
            else if (static_cast<uint32_t>(current.y) + 1 < bound.height)
            {
                current.x = 0;
                ++current.y;
            }
            else if (static_cast<uint32_t>(current.z) + 1 < bound.layers)
            {
                current.x = current.y = 0;
                ++current.z;
            }
            else
            {
                ended = true;
            }
            return *this;
        }
        /** Advances the iterator. @return Its value before advancing. */
        constexpr coordinate_iterator operator++(int) noexcept
        {
            const auto result{*this};
            ++(*this);
            return result;
        }
        /** Reads a live iterator. @return Current coordinate. */
        constexpr coordinate operator*() const noexcept
        {
            return current;
        }
        /** Compares iterator positions, including the explicit end state. */
        constexpr bool operator==(const coordinate_iterator& other) const noexcept
        {
            return ended == other.ended && (ended || (bound == other.bound && current == other.current));
        }
        /** Orders positions in iteration order, with the end after every live position. */
        constexpr bool operator<(const coordinate_iterator& other) const noexcept
        {
            return !ended && (other.ended || current < other.current);
        }

      private:
        /** Half-open bounds. */
        extent bound{};
        /** Current coordinate value. */
        coordinate current{};
        /** Whether this iterator denotes the end. */
        bool ended{true};
    };
};

/** Writes a coordinate. @param os Stream. @param t Coordinate. @return Stream. */

inline std::ostream& operator<<(std::ostream& os, const layout_base::coordinate& t)
{
    os << t.str();
    return os;
}

/**
 * Computes width times height.
 * @param size Axis sizes.
 * @return Area.
 */
constexpr uint64_t area_of(const layout_base::extent& size) noexcept
{
    return static_cast<uint64_t>(size.width) * size.height;
}
/**
 * Computes width times height times layers with checked multiplication.
 * @param size Axis sizes.
 * @return Volume.
 * @throws std::overflow_error If the volume exceeds `uint64_t`.
 */
constexpr uint64_t volume_of(const layout_base::extent& size)
{
    const auto area = area_of(size);
    if (size.layers != 0 && area > std::numeric_limits<uint64_t>::max() / size.layers)
    {
        throw std::overflow_error("The layout volume exceeds the unsigned 64-bit range");
    }
    return area * size.layers;
}

}  // namespace fiction::layouts

namespace std
{

/** Hashes every bit of all three coordinate axes. */
template <>
struct hash<fiction::layouts::layout_base::coordinate>
{
    /** @param c Coordinate. @return Hash value. */
    std::size_t operator()(const fiction::layouts::layout_base::coordinate& c) const noexcept
    {
        std::size_t seed{0};
        for (const auto axis : {c.x, c.y, c.z})
        {
            seed ^= std::hash<int32_t>{}(axis) + 0x9e3779b9u + (seed << 6u) + (seed >> 2u);
        }
        return seed;
    }
};

/**
 * Makes `coordinate_iterator` compatible with STL iterator categories. `reference` and `difference_type` are required
 * for the iterator to satisfy `std::input_or_output_iterator` (e.g., for `std::ranges::subrange` CTAD).
 */
template <>
struct iterator_traits<fiction::layouts::layout_base::coordinate_iterator>
{
    /** Iterator type information. */
    using iterator_category = std::forward_iterator_tag;
    /** Iterator type information. */
    using value_type = fiction::layouts::layout_base::coordinate;
    /** Iterator type information. */
    using reference = fiction::layouts::layout_base::coordinate;
    /** Iterator type information. */
    using difference_type = std::ptrdiff_t;
};

}  // namespace std

namespace fmt
{

/** Formats a coordinate. */
template <>
struct formatter<fiction::layouts::layout_base::coordinate>
{
    /** Parses coordinate formatting. @param ctx Parse context. @return Parse position. */
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    /** Formats a coordinate. @param c Coordinate. @param ctx Format context. @return Output iterator. */
    template <typename FormatContext>
    auto format(const fiction::layouts::layout_base::coordinate& c, FormatContext& ctx) const
    {
        return format_to(ctx.out(), runtime("({},{},{})"), c.x, c.y, c.z);
    }
};

}  // namespace fmt
