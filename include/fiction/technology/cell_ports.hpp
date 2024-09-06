//
// Created by marcel on 29.09.18.
//

#ifndef FICTION_CELL_PORTS_HPP
#define FICTION_CELL_PORTS_HPP

#include "fiction/utils/hash.hpp"

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <set>
#include <utility>

namespace fiction
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

    /**
     * Positions.
     */
    uint16_t x{}, y{};
    /**
     * Primary input port.
     */
    bool pi = false;
    /**
     * Primary output port.
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
    enum cardinal : uint8_t
    {
        NORTH = 0,
        NORTH_EAST,
        EAST,
        SOUTH_EAST,
        SOUTH,
        SOUTH_WEST,
        WEST,
        NORTH_WEST
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
     */
    uint8_t dir{};
    /**
     * Primary input port.
     */
    bool pi = false;
    /**
     * Primary output port.
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
    /**
     * Input and output positions.
     */
    std::set<PortType> inp{}, out{};
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

}  // namespace fiction

namespace std
{

// hashing for port_position
template <>
struct hash<fiction::port_position>
{
    std::size_t operator()(const fiction::port_position& port) const noexcept
    {
        std::size_t h = port.x ^ port.y;
        fiction::hash_combine(h, port.x, port.y);

        return h;
    }
};
// hashing for port_direction
template <>
struct hash<fiction::port_direction>
{
    std::size_t operator()(const fiction::port_direction& port) const noexcept
    {
        return std::hash<std::size_t>{}(static_cast<std::size_t>(port.dir));
    }
};
// hashing for port_list
template <typename PortType>
struct hash<fiction::port_list<PortType>>
{
    std::size_t operator()(const fiction::port_list<PortType>& port_list) const noexcept
    {
        std::size_t h = 0;
        fiction::hash_combine(h, port_list.inp, port_list.out);

        return h;
    }
};

}  // namespace std

namespace fmt
{
// make port_position compatible with fmt::format
template <>
struct formatter<fiction::port_position>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const fiction::port_position& p, FormatContext& ctx) const
    {
        return format_to(ctx.out(), "({},{})", p.x, p.y);
    }
};
// make port_direction compatible with fmt::format
template <>
struct formatter<fiction::port_direction>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const fiction::port_direction& p, FormatContext& ctx) const
    {
        return format_to(ctx.out(), p.dir == fiction::port_direction::NORTH      ? "N" :
                                    p.dir == fiction::port_direction::NORTH_EAST ? "NE" :
                                    p.dir == fiction::port_direction::EAST       ? "E" :
                                    p.dir == fiction::port_direction::SOUTH_EAST ? "SE" :
                                    p.dir == fiction::port_direction::SOUTH      ? "S" :
                                    p.dir == fiction::port_direction::SOUTH_WEST ? "SW" :
                                    p.dir == fiction::port_direction::WEST       ? "W" :
                                    p.dir == fiction::port_direction::NORTH_WEST ? "NW" :
                                                                                   "?");
    }
};
// make port_list compatible with fmt::format
template <typename PortType>
struct formatter<fiction::port_list<PortType>>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const fiction::port_list<PortType>& pl, FormatContext& ctx) const
    {
        return format_to(ctx.out(), "inp: {}, out: {}", join(pl.inp, ", "), join(pl.out, ", "));
    }
};
}  // namespace fmt

#endif  // FICTION_CELL_PORTS_HPP
