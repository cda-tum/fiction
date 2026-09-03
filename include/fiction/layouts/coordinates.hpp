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
 * @brief Offset, cube, and SiQAD coordinate types and the conversions between them.
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 * @author Jan Drewniok (Drewniok)
 * @author Willem Lambooy (wlambooy)
 */

#pragma once

#include "fiction/utils/math/math_utils.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>
#include <type_traits>

// data types cannot properly be converted to bit field types
#pragma GCC diagnostic push
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wuseless-cast"
#endif
#pragma GCC diagnostic ignored "-Wconversion"

namespace fiction::layouts::coords
{

/**
 * Provides offset coordinates. An offset coordinate is a coordinate that defines a location via an offset from a fixed
 * point (origin). Cartesian coordinates are offset coordinates.
 */

/**
 * Unsigned offset coordinates.
 *
 * This implementation is optimized for memory-efficiency and fits within 64 bits. Coordinates span from \f$(0, 0, 0)\f$
 * to \f$(2^{31} - 1, 2^{31} - 1, 1)\f$. Each coordinate has a dead indicator `d` that can be used to represent
 * that it is not in use.
 */
struct offset
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
    constexpr offset() noexcept :
            d{static_cast<decltype(d)>(1u)},  // default-constructed offset coordinates are dead
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
    constexpr offset(X x_, Y y_, Z z_) noexcept :
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
    constexpr offset(X x_, Y y_) noexcept :
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
    constexpr explicit offset(const uint64_t t) noexcept :
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
    [[nodiscard]] constexpr offset get_dead() const noexcept
    {
        return offset{static_cast<uint64_t>(*this) | static_cast<uint64_t>(offset{})};
    }
    /**
     * Wraps the coordinate within the inclusive bounds from `minimum` to `maximum`. If x exceeds its maximum, x is
     * reset to its minimum and y is increased. If y exceeds its maximum, y is reset to its minimum and z is increased.
     * If z exceeds its maximum, the coordinate becomes a dead copy of `maximum`.
     *
     * @param maximum Maximum coordinate to wrap against.
     * @param minimum Minimum coordinate to wrap to. Defaults to the origin.
     */
    void wrap(const offset& maximum, const offset& minimum = {0, 0, 0}) noexcept
    {
        if (x > maximum.x)
        {
            x = minimum.x;

            if (y >= maximum.y)
            {
                if (z >= maximum.z)
                {
                    *this = maximum.get_dead();
                    return;
                }

                y = minimum.y;
                ++z;
            }
            else
            {
                ++y;
            }
        }

        if (y > maximum.y)
        {
            if (z >= maximum.z)
            {
                *this = maximum.get_dead();
            }
            else
            {
                y = minimum.y;
                ++z;
            }
        }

        if (z > maximum.z)
        {
            *this = maximum.get_dead();
        }
    }
    /**
     * Compares against another coordinate for equality. Respects the dead indicator.
     *
     * @param other Right-hand side coordinate.
     * @return `true` iff both coordinates are identical.
     */
    // this is written out rather than defaulted. g++-11, the project's C++20 floor compiler and part of the Ubuntu CI
    // matrix, miscompiles a defaulted comparison over this struct's bit-field members and produces wrong
    // layout-printing output, consistent with a known class of GCC bugs in early C++20 support. The same applies to
    // `siqad` below. Revisit once g++-11 is dropped from the matrix
    constexpr bool operator==(const offset& other) const noexcept
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
    constexpr bool operator!=(const offset& other) const noexcept
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
    constexpr bool operator<(const offset& other) const noexcept
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
    constexpr bool operator>(const offset& other) const noexcept
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
    constexpr bool operator<=(const offset& other) const noexcept
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
    constexpr bool operator>=(const offset& other) const noexcept
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

inline std::ostream& operator<<(std::ostream& os, const offset& t)
{
    os << t.str();
    return os;
}

/**
 * Provides cube coordinates. Cube coordinates are used as a way to identify faces in a hexagonal grid. A wonderful
 * resource on the topic is: https://www.redblobgames.com/grids/hexagons/#coordinates-cube
 */

/**
 * Signed cube coordinates.
 *
 * This implementation allows for negative coordinate values and offers a balance between memory consumption and range
 * of values. Coordinates span from \f$(-2^{31}, -2^{31}, -2^{31})\f$ to \f$(2^{31} - 1, 2^{31} - 1, 2^{31} - 1)\f$.
 * Each coordinate has a dead indicator `d` that can be used to represent that it is not in use.
 */
struct cube
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
    constexpr cube() noexcept :
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
    constexpr cube(X x_, Y y_, Z z_) noexcept :
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
    constexpr cube(X x_, Y y_) noexcept :
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
    [[nodiscard]] constexpr cube get_dead() const noexcept
    {
        auto dead_coord{*this};
        dead_coord.d = true;
        return dead_coord;
    }
    /**
     * Wraps the coordinate within the inclusive bounds from `minimum` to `maximum`. If x exceeds its maximum, x is
     * reset to its minimum and y is increased. If y exceeds its maximum, y is reset to its minimum and z is increased.
     * If z exceeds its maximum, the coordinate becomes a dead copy of `maximum`.
     *
     * @param maximum Maximum coordinate to wrap against.
     * @param minimum Minimum coordinate to wrap to. Defaults to the origin.
     */
    void wrap(const cube& maximum, const cube& minimum = {0, 0, 0}) noexcept
    {
        if (x > maximum.x)
        {
            x = minimum.x;

            if (y >= maximum.y)
            {
                if (z >= maximum.z)
                {
                    *this = maximum.get_dead();
                    return;
                }

                y = minimum.y;
                ++z;
            }
            else
            {
                ++y;
            }
        }

        if (y > maximum.y)
        {
            y = minimum.y;

            if (z >= maximum.z)
            {
                *this = maximum.get_dead();
                return;
            }

            ++z;
        }

        if (z > maximum.z)
        {
            *this = maximum.get_dead();
        }
    }
    /**
     * Compares against another coordinate for equality. Respects the dead indicator.
     *
     * @param other Right-hand side coordinate.
     * @return `true` iff both coordinates are identical.
     */
    constexpr bool operator==(const cube& other) const noexcept
    {
        return d == other.d && z == other.z && y == other.y && x == other.x;
    }
    /**
     * Compares against another coordinate for inequality. Respects the dead indicator.
     *
     * @param other Right-hand side coordinate.
     * @return `true` iff both coordinates are not identical.
     */
    constexpr bool operator!=(const cube& other) const noexcept
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
    constexpr bool operator<(const cube& other) const noexcept
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
    constexpr bool operator>(const cube& other) const noexcept
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
    constexpr bool operator<=(const cube& other) const noexcept
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
    constexpr bool operator>=(const cube& other) const noexcept
    {
        return !(*this < other);
    }
    /**
     * Adds another coordinate to this one and returns the result. Does not modify this coordinate.
     *
     * @param other Coordinate to add.
     * @return Sum of both coordinates.
     */
    constexpr cube operator+(const cube& other) const noexcept
    {
        return cube{x + other.x, y + other.y, z + other.z};
    }
    /**
     * Subtracts another coordinate from this one and returns the result. Does not modify this coordinate.
     *
     * @param other Coordinate to subtract.
     * @return Difference of both coordinates.
     */
    constexpr cube operator-(const cube& other) const noexcept
    {
        return cube{x - other.x, y - other.y, z - other.z};
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

/**
 * Provides SiQAD coordinates. SiQAD coordinates are used to describe locations of Silicon Dangling Bonds on the
 * H-Si(100) 2x1 surface were dimer columns and rows are identified by x and y values, respectively, while the z value
 * (0,1) points to the top or bottom Si atom in the dimer. The coordinates are originally used in the SiQAD simulator
 * (https://github.com/siqad).
 */

/**
 * SiQAD coordinates.
 *
 * Coordinates span from \f$(-2^{31}, -2^{31}, 0)\f$ to \f$(2^{31} - 1 , 2^{31} - 1, 1)\f$. `x` is the SiDB's
 * x-coordinate, `y` is the dimer pair's row number, and `z` represents the two possible SiDB positions in one SiDB
 * dimer pair. Each coordinate has a dead indicator `d` that can be used to represent that it is not in use.
 */
struct siqad
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
    constexpr siqad() noexcept :
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
    constexpr siqad(X x_, Y y_, Z z_) noexcept :
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
    constexpr siqad(X x_, Y y_) noexcept :
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
    [[nodiscard]] constexpr siqad get_dead() const noexcept
    {
        auto dead_coord{*this};
        dead_coord.d = true;
        return dead_coord;
    }
    /**
     * Wraps the coordinate within the inclusive bounds from `minimum` to `maximum` in SiQAD row order. If x exceeds its
     * maximum, it is reset to its minimum before advancing to the next dimer position. If the resulting coordinate is
     * outside the y- or z-bounds, it becomes a dead copy of `maximum`.
     *
     * @param maximum Maximum coordinate to wrap against.
     * @param minimum Minimum coordinate to wrap to. Defaults to the origin.
     */
    void wrap(const siqad& maximum, const siqad& minimum = {0, 0, 0}) noexcept
    {
        if (x > maximum.x)
        {
            x = minimum.x;

            if (minimum.z == maximum.z)
            {
                z = minimum.z;

                if (y >= maximum.y)
                {
                    *this = maximum.get_dead();
                    return;
                }

                ++y;
            }
            else if (z >= maximum.z)
            {
                z = minimum.z;

                if (y >= maximum.y)
                {
                    *this = maximum.get_dead();
                    return;
                }

                ++y;
            }
            else
            {
                z = maximum.z;
            }
        }

        if (z < minimum.z || z > maximum.z)
        {
            *this = maximum.get_dead();
        }

        if (y > maximum.y)
        {
            *this = maximum.get_dead();
        }
    }
    /**
     * Compares against another coordinate for equality. Respects the dead indicator.
     *
     * @param other Right-hand side coordinate.
     * @return True iff both coordinates are identical.
     */
    constexpr bool operator==(const siqad& other) const noexcept
    {
        return d == other.d && z == other.z && y == other.y && x == other.x;
    }
    /**
     * Compares against another coordinate for inequality. Respects the dead indicator.
     *
     * @param other Right-hand side coordinate.
     * @return True iff both coordinates are not identical.
     */
    constexpr bool operator!=(const siqad& other) const noexcept
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
    constexpr bool operator<(const siqad& other) const noexcept
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
    constexpr bool operator>(const siqad& other) const noexcept
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
    constexpr bool operator<=(const siqad& other) const noexcept
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
    constexpr bool operator>=(const siqad& other) const noexcept
    {
        return !(*this < other);
    }
    /**
     * Adds another coordinate to this one and returns the result. Does not modify this coordinate.
     *
     * @param other Coordinate to add.
     * @return Sum of both coordinates.
     */
    constexpr siqad operator+(const siqad& other) const noexcept
    {
        return siqad{x + other.x, y + other.y + static_cast<decltype(y)>(z && other.z), z ^ other.z};
    }
    /**
     * Subtracts another coordinate from this one and returns the result. Does not modify this coordinate.
     *
     * @param other Coordinate to subtract.
     * @return Difference of both coordinates.
     */
    constexpr siqad operator-(const siqad& other) const noexcept
    {
        return siqad{x - other.x, y - other.y - static_cast<decltype(y)>(!z && other.z), z - other.z};
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
constexpr CoordinateType from_siqad(const siqad& coord) noexcept
{
    if (!coord.is_dead())
    {
        if ((2 * static_cast<double>(coord.y)) + static_cast<double>(coord.z) >
            static_cast<double>(std::numeric_limits<decltype(siqad::y)>::max()))
        {
            return {coord.x, std::numeric_limits<decltype(siqad::y)>::max()};
        }

        if ((2 * static_cast<double>(coord.y)) + static_cast<double>(coord.z) <
            static_cast<double>(std::numeric_limits<decltype(siqad::y)>::min()))
        {
            return {coord.x, std::numeric_limits<decltype(siqad::y)>::min()};
        }

        return {coord.x, (coord.y * 2) + coord.z};
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
constexpr siqad to_siqad(const CoordinateType& coord) noexcept
{
    if (coord.y >= 0)
    {
        return {coord.x, (coord.y - (coord.y % 2)) / 2, coord.y % 2};
    }
    return {coord.x, (coord.y + (coord.y % 2)) / 2, (coord.y % 2 == 0 ? 0 : 1)};
}

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
constexpr cube to_cube(const offset& coord) noexcept
{
    assert(coord.x <= std::numeric_limits<int32_t>::max() && coord.y <= std::numeric_limits<int32_t>::max() &&
           coord.z <= std::numeric_limits<int32_t>::max() && "Coordinate is out-of-range and cannot be transformed");

    if (coord.is_dead())
    {
        return cube{};
    }

    return {static_cast<decltype(cube::x)>(coord.x), static_cast<decltype(cube::y)>(coord.y),
            static_cast<decltype(cube::z)>(coord.z)};
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
uint64_t area_of(const CoordinateType& coord) noexcept
{
    if constexpr (std::is_same_v<CoordinateType, siqad>)
    {
        return (static_cast<uint64_t>(fiction::utils::math::integral_abs(coord.x)) + 1) *
               ((2 * static_cast<uint64_t>(fiction::utils::math::integral_abs(coord.y))) +
                static_cast<uint64_t>(fiction::utils::math::integral_abs(coord.z)) + 1);
    }

    return (static_cast<uint64_t>(fiction::utils::math::integral_abs(coord.x)) + 1) *
           (static_cast<uint64_t>(fiction::utils::math::integral_abs(coord.y)) + 1);
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
uint64_t volume_of(const CoordinateType& coord) noexcept
{
    if constexpr (std::is_same_v<CoordinateType, siqad>)
    {
        return area_of(coord);
    }

    return (static_cast<uint64_t>(fiction::utils::math::integral_abs(coord.x)) + 1) *
           (static_cast<uint64_t>(fiction::utils::math::integral_abs(coord.y)) + 1) *
           (static_cast<uint64_t>(fiction::utils::math::integral_abs(coord.z)) + 1);
}

/**
 * An iterator type that allows to enumerate coordinates in order within a boundary.
 *
 * @note Only `offset`, `cube`, and `siqad` are supported. This is enforced on the
 * boundary-and-start constructor via a `requires` clause rather than on the class itself, so that the
 * default constructor (required for `std::semiregular`) remains usable for any `CoordinateType`.
 *
 * @tparam CoordinateType Type of coordinate to enumerate.
 */
template <typename CoordinateType>
class coordinate_iterator
{
  public:
    using value_type = CoordinateType;
    /**
     * Default constructor. Required so that iterator satisfies `std::semiregular`, which in turn is required
     * for it to serve as its own `std::sentinel_for` (e.g., for `std::ranges::subrange` CTAD).
     */
    constexpr coordinate_iterator() noexcept = default;
    /**
     * Standard constructor. Initializes the iterator with a starting position and inclusive boundaries within to
     * enumerate.
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
     * The iterator is compatible with the STL forward_iterator category.
     *
     * @param maximum Maximum boundary within to enumerate. Iteration wraps at its limits.
     * @param start Starting coordinate to enumerate first.
     * @param minimum Minimum boundary to wrap to. Defaults to the origin.
     */
    constexpr explicit coordinate_iterator(const CoordinateType& maximum, const CoordinateType& start,
                                           const CoordinateType& minimum = {0, 0, 0}) noexcept
        requires std::same_as<CoordinateType, offset> || std::same_as<CoordinateType, cube> ||
                     std::same_as<CoordinateType, siqad>
            : maximum{maximum}, minimum{minimum}, coord{start}
    {
        assert(minimum.x <= maximum.x && minimum.y <= maximum.y && minimum.z <= maximum.z &&
               "Minimum coordinate must not exceed maximum coordinate");

        // Make sure the start iterator is within the lower boundary.
        coord.x = std::max(coord.x, minimum.x);
        coord.y = std::max(coord.y, minimum.y);
        coord.z = std::max(coord.z, minimum.z);

        // Then handle coordinates that are beyond the upper boundary.
        coord.wrap(maximum, minimum);
    }
    /**
     * Increments the iterator, while keeping it within the boundary. Also defined on iterators that are out of bounds.
     *
     * @return Reference to the incremented iterator.
     */
    constexpr coordinate_iterator& operator++() noexcept
    {
        if (coord == maximum)
        {
            coord = coord.get_dead();
            return *this;
        }

        if (coord.x < maximum.x)
        {
            ++coord.x;
            return *this;
        }

        coord.x = minimum.x;

        if constexpr (std::is_same_v<CoordinateType, siqad>)
        {
            if (minimum.z != maximum.z && coord.z < maximum.z)
            {
                ++coord.z;
            }
            else if (coord.y < maximum.y)
            {
                coord.z = minimum.z;
                ++coord.y;
            }
            else
            {
                coord = maximum.get_dead();
            }
        }
        else if (coord.y < maximum.y)
        {
            ++coord.y;
        }
        else if (coord.z < maximum.z)
        {
            coord.y = minimum.y;
            ++coord.z;
        }
        else
        {
            coord = maximum.get_dead();
        }

        return *this;
    }

    constexpr coordinate_iterator operator++(int) noexcept
    {
        const auto result{*this};

        ++(*this);

        return result;
    }

    constexpr CoordinateType operator*() const noexcept
    {
        return coord;
    }

    constexpr bool operator==(const coordinate_iterator& other) const noexcept
    {
        return (coord == other.coord);
    }

    constexpr bool operator!=(const coordinate_iterator& other) const noexcept
    {
        return !(*this == other);
    }

    constexpr bool operator<(const coordinate_iterator& other) const noexcept
    {
        return (coord < other.coord);
    }

    constexpr bool operator<=(const coordinate_iterator& other) const noexcept
    {
        return (coord <= other.coord);
    }

  private:
    /**
     * Maximum boundary within to enumerate. Not `const`: `std::input_or_output_iterator` requires `iterator` to be
     * `std::movable`, which in turn requires it to be assignable.
     */
    CoordinateType maximum;

    /** Minimum boundary to wrap to. */
    CoordinateType minimum;

    CoordinateType coord;
};

}  // namespace fiction::layouts::coords
// NOLINTBEGIN(cert-dcl58-cpp)

namespace std
{

// define std::hash overload for offset
template <>
struct hash<fiction::layouts::coords::offset>
{
    std::size_t operator()(const fiction::layouts::coords::offset& c) const noexcept
    {
        return static_cast<std::size_t>(std::hash<uint64_t>{}(static_cast<uint64_t>(c)));
    }
};
// define std::hash overload for cube
template <>
struct hash<fiction::layouts::coords::cube>
{
    // based on: https://stackoverflow.com/questions/25649342/hash-function-for-3d-integer-coordinates
    std::size_t operator()(const fiction::layouts::coords::cube& c) const noexcept
    {
        return static_cast<std::size_t>((c.x * 18397ll) + (c.y * 20483ll) + (c.z * 29303ll) + static_cast<int>(c.d));
    }
};
// define std::hash overload for siqad
template <>
struct hash<fiction::layouts::coords::siqad>
{
    // based on: https://stackoverflow.com/questions/25649342/hash-function-for-3d-integer-coordinates
    std::size_t operator()(const fiction::layouts::coords::siqad& c) const noexcept
    {
        return static_cast<std::size_t>((c.x * 18397ll) + (c.y * 20483ll) + (c.z * 29303ll) + static_cast<int>(c.d));
    }
};

/**
 * Makes `iterator` compatible with STL iterator categories. `reference` and `difference_type` are required
 * for `iterator` to satisfy `std::input_or_output_iterator` (e.g., for `std::ranges::subrange` CTAD).
 *
 * @tparam Coordinate Coordinate type enumerated by the `iterator`.
 */
template <typename Coordinate>
struct iterator_traits<fiction::layouts::coords::coordinate_iterator<Coordinate>>
{
    using iterator_category = std::forward_iterator_tag;
    using value_type        = Coordinate;
    using reference         = Coordinate;
    using difference_type   = std::ptrdiff_t;
};

}  // namespace std

// NOLINTEND(cert-dcl58-cpp)

namespace fmt
{

// make offset compatible with fmt::format
template <>
struct formatter<fiction::layouts::coords::offset>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const fiction::layouts::coords::offset& c, FormatContext& ctx) const
    {
        return format_to(ctx.out(), runtime("({},{},{})"), c.x, c.y, c.z);
    }
};
// make cube compatible with fmt::format
template <>
struct formatter<fiction::layouts::coords::cube>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const fiction::layouts::coords::cube& c, FormatContext& ctx) const
    {
        return format_to(ctx.out(), runtime("({},{},{})"), c.x, c.y, c.z);
    }
};
// make siqad compatible with fmt::format
template <>
struct formatter<fiction::layouts::coords::siqad>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const fiction::layouts::coords::siqad& c, FormatContext& ctx) const
    {
        return format_to(ctx.out(), runtime("({},{},{})"), c.x, c.y, c.z);
    }
};

}  // namespace fmt

#pragma GCC diagnostic pop
