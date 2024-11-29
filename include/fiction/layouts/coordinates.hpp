//
// Created by marcel on 01.05.21.
//

#ifndef FICTION_COORDINATES_HPP
#define FICTION_COORDINATES_HPP

#include "fiction/utils/math_utils.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <type_traits>

// data types cannot properly be converted to bit field types
#pragma GCC diagnostic push
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wuseless-cast"
#endif
#pragma GCC diagnostic ignored "-Wconversion"

namespace fiction
{

/**
 * Provides offset coordinates. An offset coordinate is a coordinate that defines a location via an offset from a fixed
 * point (origin). Cartesian coordinates are offset coordinates.
 */
namespace offset
{

/**
 * Unsigned offset coordinates.
 *
 * This implementation is optimized for memory-efficiency and fits within 64 bits. Coordinates span from \f$(0, 0, 0)\f$
 * to \f$(2^{31} - 1, 2^{31} - 1, 1)\f$. Each coordinate has a dead indicator `d` that can be used to represent
 * that it is not in use.
 */
struct ucoord_t
{
    /**
     * MSB acts as dead indicator.
     */
    uint64_t d : 1;
    /**
     * 1 bit for the z coordinate.
     */
    uint64_t z : 1;
    /**
     * 31 bit for the y coordinate.
     */
    uint64_t y : 31;
    /**
     * 31 bit for the x coordinate.
     */
    uint64_t x : 31;

    // NOLINTBEGIN(readability-identifier-naming)

    /**
     * Default constructor. Creates a dead coordinate at (0, 0, 0).
     */
    constexpr ucoord_t() noexcept :
            d{static_cast<decltype(d)>(1u)},  // default-constructed ucoord_ts are dead
            z{static_cast<decltype(z)>(0u)},
            y{static_cast<decltype(y)>(0u)},
            x{static_cast<decltype(x)>(0u)}
    {}
    /**
     * Standard constructor. Creates a non-dead coordinate at (x_, y_, z_).
     *
     * @tparam X Type of x.
     * @tparam Y Type of y.
     * @tparam Z Type of z.
     * @param x_ x position.
     * @param y_ y position.
     * @param z_ z position.
     */
    template <class X, class Y, class Z>
    constexpr ucoord_t(X x_, Y y_, Z z_) noexcept :
            d{static_cast<decltype(d)>(0u)},
            z{static_cast<decltype(z)>(z_)},
            y{static_cast<decltype(y)>(y_)},
            x{static_cast<decltype(x)>(x_)}
    {}
    /**
     * Standard constructor. Creates a non-dead coordinate at (x_, y_, 0).
     *
     * @tparam X Type of x.
     * @tparam Y Type of y.
     * @param x_ x position.
     * @param y_ y position.
     */
    template <class X, class Y>
    constexpr ucoord_t(X x_, Y y_) noexcept :
            d{static_cast<decltype(d)>(0u)},
            z{static_cast<decltype(z)>(0u)},
            y{static_cast<decltype(y)>(y_)},
            x{static_cast<decltype(x)>(x_)}
    {}
    /**
     * Standard constructor. Instantiates a coordinate from an uint64_t, where the positions are encoded in the
     * following four parts of the unsigned 64-bit integer (from MSB to LSB):
     *  - 1 bit for the dead indicator
     *  - 1 bit for the z position
     *  - 31 bit for the y position
     *  - 31 bit for the x position
     *
     * @param t Unsigned 64-bit integer to instantiate the coordinate from.
     */
    constexpr explicit ucoord_t(const uint64_t t) noexcept :
            d{static_cast<decltype(d)>(t >> 63ull)},
            z{static_cast<decltype(z)>((t << 1ull) >> 63ull)},
            y{static_cast<decltype(y)>((t << 2ull) >> 33ull)},
            x{static_cast<decltype(x)>((t << 33ull) >> 33ull)}
    {}

    // NOLINTEND(readability-identifier-naming)

    /**
     * Allows explicit conversion to `uint64_t`. Segments an unsigned 64-bit integer into four parts (from MSB to LSB):
     *  - 1 bit for the dead indicator
     *  - 1 bit for the z position
     *  - 31 bit for the y position
     *  - 31 bit for the x position
     */
    explicit constexpr operator uint64_t() const noexcept
    {
        return (((((((static_cast<uint64_t>(d)) << 1ull) | z) << 31ull) | y) << 31ull) | x);
    }
    /**
     * Returns whether the coordinate is dead.
     *
     * @return `true` iff coordinate is dead.
     */
    [[nodiscard]] constexpr bool is_dead() const noexcept
    {
        return static_cast<bool>(d);
    }
    /**
     * Returns a dead copy of the coordinate, i.e., (1, x, y, z).
     *
     * @return A dead copy of the coordinate.
     */
    [[nodiscard]] constexpr ucoord_t get_dead() const noexcept
    {
        return ucoord_t{static_cast<uint64_t>(*this) | static_cast<uint64_t>(ucoord_t{})};
    }
    /**
     * Wraps the coordinate with respect to the given aspect ratio by iterating over the dimensions in the order defined
     * by the coordinate type. For any dimension of the coordinate that is strictly larger than the associated dimension
     * of the aspect ratio, this dimension will be wrapped to zero, and the next dimension is increased. The resulting
     * coordinate becomes a dead copy of the aspect ratio if it is not contained in the aspect ratio after iterating.
     * An example use case of this function is the coordinate iterator, which implements iterator advancing by first
     * incrementing the x dimension, then wrapping the coordinate to the boundary within to enumerate.
     *
     * @param aspect_ratio Aspect ratio to wrap the coordinate to.
     */
    void wrap(const ucoord_t& aspect_ratio) noexcept
    {
        if (x > aspect_ratio.x)
        {
            x = 0;
            ++y;
        }

        if (y > aspect_ratio.y)
        {
            if (z == 1)
            {
                *this = aspect_ratio.get_dead();
            }
            else
            {
                y = 0;
                z = 1;
            }
        }

        if (z > aspect_ratio.z)
        {
            *this = aspect_ratio.get_dead();
        }
    }
    /**
     * Compares against another coordinate for equality. Respects the dead indicator.
     *
     * @param other Right-hand side coordinate.
     * @return `true` iff both coordinates are identical.
     */
    constexpr bool operator==(const ucoord_t& other) const noexcept
    {
        return d == other.d && z == other.z && y == other.y && x == other.x;
    }
    /**
     * Compares against another coordinate's `uint64_t` representation for equality. Respects the dead indicator.
     *
     * @param other Right-hand side coordinate representation in `uint64_t` format.
     * @return `true` iff this coordinate is equal to the converted one.
     */
    constexpr bool operator==(const uint64_t& other) const noexcept
    {
        return static_cast<uint64_t>(*this) == other;
    }
    /**
     * Compares against another coordinate for inequality. Respects the dead indicator.
     *
     * @param other Right-hand side coordinate.
     * @return `true` iff both coordinates are not identical.
     */
    constexpr bool operator!=(const ucoord_t& other) const noexcept
    {
        return !(*this == other);
    }
    /**
     * Determine whether this coordinate is "less than" another one. This is the case if z is smaller, or if z is equal
     * but y is smaller, or if z and y are equal but x is smaller.
     *
     * @param other Right-hand side coordinate.
     * @return `true` iff this coordinate is "less than" the other coordinate.
     */
    constexpr bool operator<(const ucoord_t& other) const noexcept
    {
        if (z < other.z)
        {
            return true;
        }

        if (z == other.z)
        {
            if (y < other.y)
            {
                return true;
            }

            if (y == other.y)
            {
                return x < other.x;
            }
        }

        return false;
    }
    /**
     * Determine whether this coordinate is "greater than" another one. This is the case if the other one is "less
     * than".
     *
     * @param other Right-hand side coordinate.
     * @return `true` iff this coordinate is "greater than" the other coordinate.
     */
    constexpr bool operator>(const ucoord_t& other) const noexcept
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
    constexpr bool operator<=(const ucoord_t& other) const noexcept
    {
        return !(*this > other);
    }
    /**
     * Determine whether this coordinate is "greater than or equal to" another one. This is the case if this one is not
     * "less than" the other.
     *
     * @param other Right-hand side coordinate.
     * @return `true` iff this coordinate is "greater than or equal to" the other coordinate.
     */
    constexpr bool operator>=(const ucoord_t& other) const noexcept
    {
        return !(*this < other);
    }
    /**
     * Returns a string representation of the coordinate of the form `"(x, y, z)"` that does not respect the dead
     * indicator.
     *
     * @return String representation of the form `"(x, y, z)"`.
     */
    [[nodiscard]] std::string str() const
    {
        return fmt::format("({},{},{})", x, y, z);
    }
};

inline std::ostream& operator<<(std::ostream& os, const ucoord_t& t)
{
    os << t.str();
    return os;
}

}  // namespace offset

/**
 * Provides cube coordinates. Cube coordinates are used as a way to identify faces in a hexagonal grid. A wonderful
 * resource on the topic is: https://www.redblobgames.com/grids/hexagons/#coordinates-cube
 */
namespace cube
{

/**
 * Signed cube coordinates.
 *
 * This implementation allows for negative coordinate values and offers a balance between memory consumption and range
 * of values. Coordinates span from \f$(-2^{31}, -2^{31}, -2^{31})\f$ to \f$(2^{31} - 1, 2^{31} - 1, 2^{31} - 1)\f$.
 * Each coordinate has a dead indicator `d` that can be used to represent that it is not in use.
 */
struct coord_t
{
    /**
     * Dead indicator.
     */
    bool d{true};  // default-constructed coordinates are dead
    /**
     * z coordinate.
     */
    int32_t z;
    /**
     * y coordinate.
     */
    int32_t y;
    /**
     * x coordinate.
     */
    int32_t x;

    // NOLINTBEGIN(readability-identifier-naming)

    /**
     * Default constructor. Creates a dead coordinate at (0, 0, 0).
     */
    constexpr coord_t() noexcept :
            z{static_cast<decltype(z)>(0)},
            y{static_cast<decltype(y)>(0)},
            x{static_cast<decltype(x)>(0)}
    {}
    /**
     * Standard constructor. Creates a non-dead coordinate at (x_, y_, z_).
     *
     * @tparam X Type of x.
     * @tparam Y Type of y.
     * @tparam Z Type of z.
     * @param x_ x position.
     * @param y_ y position.
     * @param z_ z position.
     */
    template <class X, class Y, class Z>
    constexpr coord_t(X x_, Y y_, Z z_) noexcept :
            d{false},
            z{static_cast<decltype(z)>(z_)},
            y{static_cast<decltype(y)>(y_)},
            x{static_cast<decltype(x)>(x_)}
    {}
    /**
     * Standard constructor. Creates a non-dead coordinate at (x_, y_, 0).
     *
     * @tparam X Type of x.
     * @tparam Y Type of y.
     * @param x_ x position.
     * @param y_ y position.
     */
    template <class X, class Y>
    constexpr coord_t(X x_, Y y_) noexcept :
            d{false},
            z{static_cast<decltype(z)>(0)},
            y{static_cast<decltype(y)>(y_)},
            x{static_cast<decltype(x)>(x_)}
    {}

    // NOLINTEND(readability-identifier-naming)

    /**
     * Returns whether the coordinate is dead.
     *
     * @return `true` iff coordinate is dead.
     */
    [[nodiscard]] constexpr bool is_dead() const noexcept
    {
        return static_cast<bool>(d);
    }
    /**
     * Returns a dead copy of the coordinate, i.e., (1, x, y, z).
     *
     * @return A dead copy of the coordinate.
     */
    [[nodiscard]] constexpr coord_t get_dead() const noexcept
    {
        auto dead_coord{*this};
        dead_coord.d = true;
        return dead_coord;
    }
    /**
     * Wraps the coordinate with respect to the given aspect ratio by iterating over the dimensions in the order defined
     * by the coordinate type. For any dimension of the coordinate that is strictly larger than the associated dimension
     * of the aspect ratio, this dimension will be wrapped to zero, and the next dimension is increased. The resulting
     * coordinate becomes a dead copy of the aspect ratio if it is not contained in the aspect ratio after iterating.
     * An example use case of this function is the coordinate iterator, which implements iterator advancing by first
     * incrementing the x dimension, then wrapping the coordinate to the boundary within to enumerate.
     *
     * @param aspect_ratio Aspect ratio to wrap the coordinate to.
     */
    void wrap(const coord_t& aspect_ratio) noexcept
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
            *this = aspect_ratio.get_dead();
        }
    }
    /**
     * Compares against another coordinate for equality. Respects the dead indicator.
     *
     * @param other Right-hand side coordinate.
     * @return `true` iff both coordinates are identical.
     */
    constexpr bool operator==(const coord_t& other) const noexcept
    {
        return d == other.d && z == other.z && y == other.y && x == other.x;
    }
    /**
     * Compares against another coordinate for inequality. Respects the dead indicator.
     *
     * @param other Right-hand side coordinate.
     * @return `true` iff both coordinates are not identical.
     */
    constexpr bool operator!=(const coord_t& other) const noexcept
    {
        return !(*this == other);
    }
    /**
     * Determine whether this coordinate is "less than" another one. This is the case if z is smaller, or if z is equal
     * but y is smaller, or if z and y are equal but x is smaller.
     *
     * @param other Right-hand side coordinate.
     * @return `true` iff this coordinate is "less than" the other coordinate.
     */
    constexpr bool operator<(const coord_t& other) const noexcept
    {
        if (z < other.z)
        {
            return true;
        }

        if (z == other.z)
        {
            if (y < other.y)
            {
                return true;
            }

            if (y == other.y)
            {
                return x < other.x;
            }
        }

        return false;
    }
    /**
     * Determine whether this coordinate is "greater than" another one. This is the case if the other one is "less
     * than".
     *
     * @param other Right-hand side coordinate.
     * @return `true` iff this coordinate is "greater than" the other coordinate.
     */
    constexpr bool operator>(const coord_t& other) const noexcept
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
    constexpr bool operator<=(const coord_t& other) const noexcept
    {
        return !(*this > other);
    }
    /**
     * Determine whether this coordinate is "greater than or equal to" another one. This is the case if this one is not
     * "less than" the other.
     *
     * @param other Right-hand side coordinate.
     * @return `true` iff this coordinate is "greater than or equal to" the other coordinate.
     */
    constexpr bool operator>=(const coord_t& other) const noexcept
    {
        return !(*this < other);
    }
    /**
     * Adds another coordinate to this one and returns the result. Does not modify this coordinate.
     *
     * @param other Coordinate to add.
     * @return Sum of both coordinates.
     */
    constexpr coord_t operator+(const coord_t& other) const noexcept
    {
        return coord_t{x + other.x, y + other.y, z + other.z};
    }
    /**
     * Subtracts another coordinate from this one and returns the result. Does not modify this coordinate.
     *
     * @param other Coordinate to subtract.
     * @return Difference of both coordinates.
     */
    constexpr coord_t operator-(const coord_t& other) const noexcept
    {
        return coord_t{x - other.x, y - other.y, z - other.z};
    }
    /**
     * Returns a string representation of the coordinate of the form `"(x, y, z)"` that does not respect the dead
     * indicator.
     *
     * @return String representation of the form `"(x, y, z)"`.
     */
    [[nodiscard]] std::string str() const
    {
        return fmt::format("({},{},{})", x, y, z);
    }
};

}  // namespace cube

/**
 * Provides SiQAD coordinates. SiQAD coordinates are used to describe locations of Silicon Dangling Bonds on the
 * H-Si(100) 2x1 surface were dimer columns and rows are identified by x and y values, respectively, while the z value
 * (0,1) points to the top or bottom Si atom in the dimer. The coordinates are originally used in the SiQAD simulator
 * (https://github.com/siqad).
 */
namespace siqad
{

/**
 * SiQAD coordinates.
 *
 * Coordinates span from \f$(-2^{31}, -2^{31}, 0)\f$ to \f$(2^{31} - 1 , 2^{31} - 1, 1)\f$. `x` is the SiDB's
 * x-coordinate, `y` is the dimer pair's row number, and `z` represents the two possible SiDB positions in one SiDB
 * dimer pair. Each coordinate has a dead indicator `d` that can be used to represent that it is not in use.
 */
struct coord_t
{
    /**
     * MSB acts as dead indicator.
     */
    bool d{true};
    /**
     * 1 bit for the z coordinate.
     */
    uint8_t z : 1;
    /**
     * 31 bit for the y coordinate.
     */
    int32_t y;
    /**
     * 31 bit for the x coordinate.
     */
    int32_t x;

    // NOLINTBEGIN(readability-identifier-naming)

    /**
     * Default constructor. Creates a dead coordinate at (0, 0, 0).
     */
    constexpr coord_t() noexcept :
            z{static_cast<decltype(z)>(0)},
            y{static_cast<decltype(y)>(0)},
            x{static_cast<decltype(x)>(0)}
    {}
    /**
     * Standard constructor. Creates a non-dead coordinate at (x_, y_, z_).
     *
     * @tparam X Type of x.
     * @tparam Y Type of y.
     * @tparam Z Type of z.
     * @param x_ x position.
     * @param y_ y position.
     * @param z_ z position.
     */
    template <class X, class Y, class Z>
    constexpr coord_t(X x_, Y y_, Z z_) noexcept :
            d{false},
            z{static_cast<decltype(z)>(z_)},
            y{static_cast<decltype(y)>(y_)},
            x{static_cast<decltype(x)>(x_)}
    {}
    /**
     * Standard constructor. Creates a non-dead coordinate at (x_, y_, 0).
     *
     * @tparam X Type of x.
     * @tparam Y Type of y.
     * @param x_ x position.
     * @param y_ y position.
     */
    template <class X, class Y>
    constexpr coord_t(X x_, Y y_) noexcept :
            d{false},
            z{static_cast<decltype(z)>(0)},
            y{static_cast<decltype(y)>(y_)},
            x{static_cast<decltype(x)>(x_)}
    {}

    // NOLINTEND(readability-identifier-naming)

    /**
     * Returns whether the coordinate is dead.
     *
     * @return True iff coordinate is dead.
     */
    [[nodiscard]] constexpr bool is_dead() const noexcept
    {
        return static_cast<bool>(d);
    }
    /**
     * Returns a dead copy of the coordinate, i.e., (1, x, y, z).
     *
     * @return A dead copy of the coordinate.
     */
    [[nodiscard]] constexpr coord_t get_dead() const noexcept
    {
        auto dead_coord{*this};
        dead_coord.d = true;
        return dead_coord;
    }
    /**
     * Wraps the coordinate with respect to the given aspect ratio by iterating over the dimensions in the order defined
     * by the coordinate type. For any dimension of the coordinate that is strictly larger than the associated dimension
     * of the aspect ratio, this dimension will be wrapped to zero, and the next dimension is increased. The resulting
     * coordinate becomes a dead copy of the aspect ratio if it is not contained in the aspect ratio after iterating.
     * An example use case of this function is the coordinate iterator, which implements iterator advancing by first
     * incrementing the x dimension, then wrapping the coordinate to the boundary within to enumerate.
     *
     * @param aspect_ratio Aspect ratio to wrap the coordinate to.
     */
    void wrap(const coord_t& aspect_ratio) noexcept
    {
        if (x > aspect_ratio.x)
        {
            x = 0;
            y += z;
            z = !z;
        }

        if (z > aspect_ratio.z)
        {
            *this = aspect_ratio.get_dead();
        }

        if (y > aspect_ratio.y)
        {
            *this = aspect_ratio.get_dead();
        }
    }
    /**
     * Compares against another coordinate for equality. Respects the dead indicator.
     *
     * @param other Right-hand side coordinate.
     * @return True iff both coordinates are identical.
     */
    constexpr bool operator==(const coord_t& other) const noexcept
    {
        return d == other.d && z == other.z && y == other.y && x == other.x;
    }
    /**
     * Compares against another coordinate for inequality. Respects the dead indicator.
     *
     * @param other Right-hand side coordinate.
     * @return True iff both coordinates are not identical.
     */
    constexpr bool operator!=(const coord_t& other) const noexcept
    {
        return !(*this == other);
    }
    /**
     * Determine whether this coordinate is "less than" another one. This is the case if y is smaller, or if y is equal
     * but z is smaller, or if z and y are equal but x is smaller.
     *
     * @param other Right-hand side coordinate.
     * @return True iff this coordinate is "less than" the other coordinate.
     */
    constexpr bool operator<(const coord_t& other) const noexcept
    {

        if (y != other.y)
        {
            return y < other.y;
        }
        if (z != other.z)
        {
            return z < other.z;
        }

        return x < other.x;
    }
    /**
     * Determine whether this coordinate is "greater than" another one. This is the case if the other one is "less
     * than".
     *
     * @param other Right-hand side coordinate.
     * @return True iff this coordinate is "greater than" the other coordinate.
     */
    constexpr bool operator>(const coord_t& other) const noexcept
    {
        return other < *this;
    }
    /**
     * Determine whether this coordinate is "less than or equal to" another one. This is the case if this one is not
     * "greater than" the other.
     *
     * @param other Right-hand side coordinate.
     * @return True iff this coordinate is "less than or equal to" the other coordinate.
     */
    constexpr bool operator<=(const coord_t& other) const noexcept
    {
        return !(*this > other);
    }
    /**
     * Determine whether this coordinate is "greater than or equal to" another one. This is the case if this one is not
     * "less than" the other.
     *
     * @param other Right-hand side coordinate.
     * @return True iff this coordinate is "greater than or equal to" the other coordinate.
     */
    constexpr bool operator>=(const coord_t& other) const noexcept
    {
        return !(*this < other);
    }
    /**
     * Adds another coordinate to this one and returns the result. Does not modify this coordinate.
     *
     * @param other Coordinate to add.
     * @return Sum of both coordinates.
     */
    constexpr coord_t operator+(const coord_t& other) const noexcept
    {
        return coord_t{x + other.x, y + other.y + static_cast<decltype(y)>(z && other.z), z ^ other.z};
    }
    /**
     * Subtracts another coordinate from this one and returns the result. Does not modify this coordinate.
     *
     * @param other Coordinate to subtract.
     * @return Difference of both coordinates.
     */
    constexpr coord_t operator-(const coord_t& other) const noexcept
    {
        return coord_t{x - other.x, y - other.y - static_cast<decltype(y)>(!z && other.z), z - other.z};
    }
    /**
     * Returns a string representation of the coordinate of the form "(x, y, z)" that does not respect the dead
     * indicator.
     *
     * @return String representation of the form "(x, y, z)".
     */
    [[nodiscard]] std::string str() const
    {
        return fmt::format("({},{},{})", x, y, z);
    }
};

/**
 * Converts SiQAD coordinates to other coordinates (offset, cube).
 *
 * @tparam CoordinateType The desired coordinate type.
 * @param coord SiQAD coordinate to convert.
 * @return Coordinate of type `CoordinateType`.
 */
template <typename CoordinateType>
constexpr CoordinateType to_fiction_coord(const siqad::coord_t& coord) noexcept
{
    if (!coord.is_dead())
    {
        if (2 * static_cast<double>(coord.y) + static_cast<double>(coord.z) >
            static_cast<double>(std::numeric_limits<decltype(siqad::coord_t::y)>::max()))
        {
            return {coord.x, std::numeric_limits<decltype(siqad::coord_t::y)>::max()};
        }

        if (2 * static_cast<double>(coord.y) + static_cast<double>(coord.z) <
            static_cast<double>(std::numeric_limits<decltype(siqad::coord_t::y)>::min()))
        {
            return {coord.x, std::numeric_limits<decltype(siqad::coord_t::y)>::min()};
        }

        return {coord.x, coord.y * 2 + coord.z};
    }

    return CoordinateType{};
}
/**
 * Converts any coordinate type to SiQAD coordinates.
 *
 * @tparam CoordinateType Coordinate type to convert.
 * @param coord Coordinate to convert.
 * @return SiQAD coordinate representation of `coord`.
 *
 */
template <typename CoordinateType>
constexpr coord_t to_siqad_coord(const CoordinateType& coord) noexcept
{
    if (coord.y >= 0)
    {
        return {coord.x, (coord.y - coord.y % 2) / 2, coord.y % 2};
    }
    return {coord.x, (coord.y + coord.y % 2) / 2, (-coord.y - 1) % 2 + 1};
}

}  // namespace siqad

/**
 * Converts offset coordinates to cube coordinates.
 *
 * @note This function assumes that the input coordinates are within the valid range for cube coordinates. Specifically,
 * the x, y, and z coordinates should be within the range of \f$(0, 0, 0)\f$ to \f$(2^{31} - 1, 2^{31} - 1, 1)\f$. If
 * the input coordinates are outside this range, the behavior of the function is undefined. If the input coordinate is
 * dead, a dead cube coordinate is returned.
 *
 * @param coord Offset coordinate to convert to a cube coordinate.
 * @return Cube coordinate equivalent to `coord`.
 */
constexpr cube::coord_t offset_to_cube_coord(const offset::ucoord_t& coord) noexcept
{
    assert(coord.x <= std::numeric_limits<int32_t>::max() && coord.y <= std::numeric_limits<int32_t>::max() &&
           coord.z <= std::numeric_limits<int32_t>::max() && "Coordinate is out-of-range and cannot be transformed");

    if (coord.is_dead())
    {
        return cube::coord_t{};
    }

    return {static_cast<decltype(cube::coord_t::x)>(coord.x), static_cast<decltype(cube::coord_t::y)>(coord.y),
            static_cast<decltype(cube::coord_t::z)>(coord.z)};
}
/**
 * Computes the area of a given coordinate assuming its origin is (0, 0, 0). Calculates \f$(|x| + 1) \cdot (|y| + 1)\f$
 * by default. The exception is SiQAD coordinates, for which it computes \f$(|x| + 1) \cdot (2 \cdot |y| + |z| + 1)\f$.
 *
 * @tparam CoordinateType Coordinate type.
 * @param coord Coordinate.
 * @return Area of coord.
 */
template <typename CoordinateType>
uint64_t area(const CoordinateType& coord) noexcept
{
    if constexpr (std::is_same_v<CoordinateType, siqad::coord_t>)
    {
        return (static_cast<uint64_t>(integral_abs(coord.x)) + 1) *
               (2 * static_cast<uint64_t>(integral_abs(coord.y)) + static_cast<uint64_t>(integral_abs(coord.z)) + 1);
    }

    return (static_cast<uint64_t>(integral_abs(coord.x)) + 1) * (static_cast<uint64_t>(integral_abs(coord.y)) + 1);
}
/**
 * Computes the volume of a given coordinate assuming its origin is (0, 0, 0). Calculates \f$(|x| + 1) \cdot (|y| + 1)
 * \cdot (|z| + 1)\f$ by default. For SiQAD coordinates, which are planar by definition, the area is returned.
 *
 * @tparam CoordinateType Coordinate type.
 * @param coord Coordinate.
 * @return Volume of coord.
 */
template <typename CoordinateType>
uint64_t volume(const CoordinateType& coord) noexcept
{
    if constexpr (std::is_same_v<CoordinateType, siqad::coord_t>)
    {
        return area(coord);
    }

    return (static_cast<uint64_t>(integral_abs(coord.x)) + 1) * (static_cast<uint64_t>(integral_abs(coord.y)) + 1) *
           (static_cast<uint64_t>(integral_abs(coord.z)) + 1);
}

/**
 * An iterator type that allows to enumerate coordinates in order within a boundary.
 *
 * @tparam CoordinateType Type of coordinate to enumerate.
 */
template <typename CoordinateType>
class coord_iterator
{
  public:
    using value_type = CoordinateType;
    /**
     * Standard constructor. Initializes the iterator with a starting position and the boundary within to enumerate.
     *
     * With `dimension = (1, 2, 1)` and `start = (0, 0, 0)`, the following order would be enumerated for offset or cubic
     * coordinates:
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
     * For SiQAD coordinates with the same parameters, we have the following order of enumeration:
     *
     * - (0, 0, 0)
     * - (1, 0, 0)
     * - (0, 0, 1)
     * - (1, 0, 1)
     * - (0, 1, 0)
     * - (1, 2, 0)
     * - (0, 1, 1)
     * - (1, 1, 1)
     * - (1, 1, 0)
     * - (0, 2, 0)
     * - (0, 2, 1)
     * - (1, 2, 1)
     *
     * coord_iterator is compatible with the STL forward_iterator category. Does not iterate over negative coordinates.
     *
     * @param dimension Boundary within to enumerate. Iteration wraps at its limits.
     * @param start Starting coordinate to enumerate first.
     */
    constexpr explicit coord_iterator(const CoordinateType& dimension, const CoordinateType& start) noexcept :
            aspect_ratio{dimension},
            coord{start}
    {
        static_assert(std::is_same_v<CoordinateType, offset::ucoord_t> ||
                          std::is_same_v<CoordinateType, cube::coord_t> ||
                          std::is_same_v<CoordinateType, siqad::coord_t>,
                      "CoordinateType must be a supported coordinate");

        // Make sure the start iterator is within the given boundary; first handle negative coordinates ...
        coord.x = std::max(coord.x, static_cast<decltype(coord.x)>(0));
        coord.y = std::max(coord.y, static_cast<decltype(coord.y)>(0));
        coord.z = std::max(coord.z, static_cast<decltype(coord.z)>(0));

        // ... then handle coordinates that are beyond the given boundary.
        coord.wrap(aspect_ratio);
    }
    /**
     * Increments the iterator, while keeping it within the boundary. Also defined on iterators that are out of bounds.
     *
     * @return Reference to the incremented iterator.
     */
    constexpr coord_iterator& operator++() noexcept
    {
        if (coord != aspect_ratio)
        {
            ++coord.x;

            coord.wrap(aspect_ratio);
        }
        else
        {
            coord = coord.get_dead();
        }

        return *this;
    }

    constexpr coord_iterator operator++(int) noexcept
    {
        const auto result{*this};

        ++(*this);

        return result;
    }

    constexpr CoordinateType operator*() const noexcept
    {
        return coord;
    }

    constexpr bool operator==(const coord_iterator& other) const noexcept
    {
        return (coord == other.coord);
    }

    constexpr bool operator!=(const coord_iterator& other) const noexcept
    {
        return !(*this == other);
    }

    constexpr bool operator<(const coord_iterator& other) const noexcept
    {
        return (coord < other.coord);
    }

    constexpr bool operator<=(const coord_iterator& other) const noexcept
    {
        return (coord <= other.coord);
    }

  private:
    const CoordinateType aspect_ratio;

    CoordinateType coord;
};

}  // namespace fiction

// NOLINTBEGIN(cert-dcl58-cpp)

namespace std
{

// define std::hash overload for offset::ucoord_t
template <>
struct hash<fiction::offset::ucoord_t>
{
    std::size_t operator()(const fiction::offset::ucoord_t& c) const noexcept
    {
        return static_cast<std::size_t>(std::hash<uint64_t>{}(static_cast<uint64_t>(c)));
    }
};
// define std::hash overload for cube::coord_t
template <>
struct hash<fiction::cube::coord_t>
{
    // based on: https://stackoverflow.com/questions/25649342/hash-function-for-3d-integer-coordinates
    std::size_t operator()(const fiction::cube::coord_t& c) const noexcept
    {
        return static_cast<std::size_t>((c.x * 18397ll) + (c.y * 20483ll) + (c.z * 29303ll) + static_cast<int>(c.d));
    }
};
// define std::hash overload for siqad::coord_t
template <>
struct hash<fiction::siqad::coord_t>
{
    // based on: https://stackoverflow.com/questions/25649342/hash-function-for-3d-integer-coordinates
    std::size_t operator()(const fiction::siqad::coord_t& c) const noexcept
    {
        return static_cast<std::size_t>((c.x * 18397ll) + (c.y * 20483ll) + (c.z * 29303ll) + static_cast<int>(c.d));
    }
};

// make coord_iterator compatible with STL iterator categories
template <typename Coordinate>
struct iterator_traits<fiction::coord_iterator<Coordinate>>
{
    using iterator_category = std::forward_iterator_tag;
    using value_type        = Coordinate;
};

}  // namespace std

// NOLINTEND(cert-dcl58-cpp)

namespace fmt
{

// make offset::ucoord_t compatible with fmt::format
template <>
struct formatter<fiction::offset::ucoord_t>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const fiction::offset::ucoord_t& c, FormatContext& ctx) const
    {
        return format_to(ctx.out(), runtime("({},{},{})"), c.x, c.y, c.z);
    }
};
// make cube::coord_t compatible with fmt::format
template <>
struct formatter<fiction::cube::coord_t>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const fiction::cube::coord_t& c, FormatContext& ctx) const
    {
        return format_to(ctx.out(), runtime("({},{},{})"), c.x, c.y, c.z);
    }
};
// make siqad::coord_t compatible with fmt::format
template <>
struct formatter<fiction::siqad::coord_t>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const fiction::siqad::coord_t& c, FormatContext& ctx) const
    {
        return format_to(ctx.out(), runtime("({},{},{})"), c.x, c.y, c.z);
    }
};

}  // namespace fmt

#pragma GCC diagnostic pop

#endif  // FICTION_COORDINATES_HPP
