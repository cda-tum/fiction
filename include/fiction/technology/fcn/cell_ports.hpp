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
 * @brief Port positions, directions, and port lists that describe a gate's tile interface.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#pragma once

#include "fiction/utils/stl/hash.hpp"

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <set>
#include <utility>

namespace fiction::fcn
{
/**
 * A port position is a relative location of a cell within a tile.
 */
struct port_position
{
    /**
     * Default constructor.
     */
    constexpr port_position() = default;

    // NOLINTBEGIN(readability-identifier-naming)

    /**
     * Standard constructor.
     */
    constexpr port_position(const uint16_t x_, const uint16_t y_, const bool pi_ = false, const bool po_ = false) :
            x{x_},
            y{y_},
            pi{pi_},
            po{po_}
    {}

    // NOLINTEND(readability-identifier-naming)

    /** @brief Horizontal port coordinate. */
    uint16_t x{};
    /** @brief Vertical port coordinate. */
    uint16_t y{};
    /**
     * Primary input port.
     * Defaults to `false`.
     */
    bool pi = false;
    /**
     * Primary output port.
     * Defaults to `false`.
     */
    bool po = false;
    /**
     * Comparator for set insertion.
     *
     * @param p Port to compare to.
     * @return `true` iff this port goes before `p` in set.
     */
    constexpr bool operator<(const port_position& p) const
    {
        return (this->x < p.x) || ((this->x == p.x) && (this->y < p.y));
    }
    /**
     * Comparator for equality tests.
     *
     * @param p Port to compare to.
     * @return `true` iff this port is equal to given port `p`.
     */
    constexpr bool operator==(const port_position& p) const
    {
        return (this->x == p.x) && (this->y == p.y);
    }
};
/**
 * A port direction is a relative (cardinal) direction of a port within a tile.
 * Useful, when no exact port locations within a tile are needed.
 */
struct port_direction
{
    /**
     * Cardinal direction.
     */
    // `dir` below is stored and compared as a plain uint8_t at ~300 call sites across the code base;
    // converting this to an enum class is a real module-modernization task, not a same-file cleanup,
    // and is deferred to a follow-up PR.
    // NOLINTNEXTLINE(cppcoreguidelines-use-enum-class)
    enum cardinal : uint8_t
    {
        /**
         * North direction.
         */
        NORTH = 0,
        /**
         * North-East direction.
         */
        NORTH_EAST,
        /**
         * East direction.
         */
        EAST,
        /**
         * South-East direction.
         */
        SOUTH_EAST,
        /**
         * South direction.
         */
        SOUTH,
        /**
         * South-West direction.
         */
        SOUTH_WEST,
        /**
         * West direction.
         */
        WEST,
        /**
         * North-West direction.
         */
        NORTH_WEST,
        /**
         * None direction.
         */
        NONE
    };
    /**
     * Default constructor.
     */
    constexpr port_direction() = default;

    // NOLINTBEGIN(readability-identifier-naming)

    /**
     * Standard constructor.
     */
    constexpr explicit port_direction(const cardinal d, const bool pi_ = false, const bool po_ = false) :
            dir{d},
            pi{pi_},
            po{po_}
    {}

    // NOLINTEND(readability-identifier-naming)

    /**
     * Direction.
     * Defaults to `NORTH` (0).
     */
    uint8_t dir{};
    /**
     * Primary input port.
     * Defaults to `false`.
     */
    bool pi = false;
    /**
     * Primary output port.
     * Defaults to `false`.
     */
    bool po = false;
    /**
     * Comparator for set insertion.
     *
     * @param p Port to compare to.
     * @return `true` iff this port goes before `p` in set.
     */
    constexpr bool operator<(const port_direction& p) const
    {
        return (this->dir < p.dir);
    }
    /**
     * Comparator for equality tests.
     *
     * @param p Port to compare to.
     * @return `true` iff this port is equal to given port `p`.
     */
    constexpr bool operator==(const port_direction& p) const
    {
        return (this->dir == p.dir);
    }
};
/**
 * Port lists are collections of input and output ports.
 *
 * @tparam PortType A port type, e.g., port_position or port_direction.
 */
template <typename PortType>
struct port_list
{
    /**
     * Exposing the underlying port type.
     */
    using port_type = PortType;
    /**
     * Default constructor.
     */
    constexpr port_list() = default;
    /**
     * Standard constructor.
     */
    port_list(std::set<PortType> input_ports, std::set<PortType> output_ports) :
            inp{std::move(input_ports)},
            out{std::move(output_ports)}
    {}
    /** @brief Input ports. */
    std::set<PortType> inp{};
    /** @brief Output ports. */
    std::set<PortType> out{};
    /**
     * Comparator for port lists.
     *
     * @param p Ports to compare to.
     * @return `true` iff these ports are equal to `p`.
     */
    bool operator==(const port_list<PortType>& p) const
    {
        return this->inp == p.inp && this->out == p.out;
    }
    /**
     * Merges two `port_list` objects together. The given `port_list` might be altered.
     *
     * @param p Ports to merge.
     * @return Merged port lists.
     */
    port_list<PortType>& operator+=(port_list<PortType>& p)
    {
        this->inp.merge(p.inp);
        this->out.merge(p.out);

        return *this;
    }
};

}  // namespace fiction::fcn
namespace std
{

/** @brief Hashes `fiction::fcn::port_position` values. */
template <>
struct hash<fiction::fcn::port_position>
{
    /** @brief Hashes the horizontal and vertical port coordinates. */
    std::size_t operator()(const fiction::fcn::port_position& port) const noexcept
    {
        std::size_t h = port.x ^ port.y;
        fiction::utils::stl::hash_combine(h, port.x, port.y);

        return h;
    }
};
/** @brief Hashes `fiction::fcn::port_direction` values. */
template <>
struct hash<fiction::fcn::port_direction>
{
    /** @brief Hashes the cardinal port direction. */
    std::size_t operator()(const fiction::fcn::port_direction& port) const noexcept
    {
        return std::hash<std::size_t>{}(static_cast<std::size_t>(port.dir));
    }
};
/** @brief Hashes `fiction::fcn::port_list` values. */
template <typename PortType>
struct hash<fiction::fcn::port_list<PortType>>
{
    /** @brief Hashes the input and output port sets. */
    std::size_t operator()(const fiction::fcn::port_list<PortType>& port_list) const noexcept
    {
        std::size_t h = 0;
        fiction::utils::stl::hash_combine(h, port_list.inp, port_list.out);

        return h;
    }
};

}  // namespace std

namespace fmt
{
/** @brief Formats `fiction::fcn::port_position` values. */
template <>
struct formatter<fiction::fcn::port_position>
{
    /** @brief Parses the format specification. */
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    /** @brief Writes the port representation to the format context. */
    template <typename FormatContext>
    auto format(const fiction::fcn::port_position& p, FormatContext& ctx) const
    {
        return format_to(ctx.out(), runtime("({},{})"), p.x, p.y);
    }
};
/** @brief Formats `fiction::fcn::port_direction` values. */
template <>
struct formatter<fiction::fcn::port_direction>
{
    /** @brief Parses the format specification. */
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    /** @brief Writes the port representation to the format context. */
    template <typename FormatContext>
    auto format(const fiction::fcn::port_direction& p, FormatContext& ctx) const
    {
        const auto* dir = "?";

        switch (p.dir)
        {
            case fiction::fcn::port_direction::NORTH:
            {
                dir = "N";
                break;
            }
            case fiction::fcn::port_direction::NORTH_EAST:
            {
                dir = "NE";
                break;
            }
            case fiction::fcn::port_direction::EAST:
            {
                dir = "E";
                break;
            }
            case fiction::fcn::port_direction::SOUTH_EAST:
            {
                dir = "SE";
                break;
            }
            case fiction::fcn::port_direction::SOUTH:
            {
                dir = "S";
                break;
            }
            case fiction::fcn::port_direction::SOUTH_WEST:
            {
                dir = "SW";
                break;
            }
            case fiction::fcn::port_direction::WEST:
            {
                dir = "W";
                break;
            }
            case fiction::fcn::port_direction::NORTH_WEST:
            {
                dir = "NW";
                break;
            }
            default:
            {
                break;
            }
        }

        return format_to(ctx.out(), fmt::runtime(dir));
    }
};
/** @brief Formats `fiction::fcn::port_list` values. */
template <typename PortType>
struct formatter<fiction::fcn::port_list<PortType>>
{
    /** @brief Parses the format specification. */
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    /** @brief Writes the port representation to the format context. */
    template <typename FormatContext>
    auto format(const fiction::fcn::port_list<PortType>& pl, FormatContext& ctx) const
    {
        return format_to(ctx.out(), runtime("inp: {}, out: {}"), join(pl.inp, ", "), join(pl.out, ", "));
    }
};
}  // namespace fmt
