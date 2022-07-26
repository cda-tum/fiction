//
// Created by marcel on 29.09.18.
//

#ifndef FICTION_CELL_PORTS_HPP
#define FICTION_CELL_PORTS_HPP

#include "fiction/traits.hpp"
#include "fiction/utils/hash.hpp"

#include <fmt/format.h>

#include <cassert>
#include <cstdint>
#include <functional>
#include <set>
#include <unordered_map>
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
    /**
     * Standard constructor.
     */
    constexpr port_position(const uint16_t x_, const uint16_t y_, const bool pi_ = false, const bool po_ = false) :
            x{x_},
            y{y_},
            pi{pi_},
            po{po_}
    {}
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
     * @return True iff this port goes before p in set.
     */
    constexpr bool operator<(const port_position& p) const
    {
        return (this->x < p.x) || ((this->x == p.x) && (this->y < p.y));
    }
    /**
     * Comparator for equality tests.
     *
     * @param p Port to compare to.
     * @return True iff this port is equal to given port p.
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
    /**
     * Standard constructor.
     */
    constexpr explicit port_direction(const cardinal d, const bool pi_ = false, const bool po_ = false) :
            dir{d},
            pi{pi_},
            po{po_}
    {}
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
     * @return True iff this port goes before p in set.
     */
    constexpr bool operator<(const port_direction& p) const
    {
        return (this->dir < p.dir);
    }
    /**
     * Comparator for equality tests.
     *
     * @param p Port to compare to.
     * @return True iff this port is equal to given port p.
     */
    constexpr bool operator==(const port_direction& p) const
    {
        return (this->dir == p.dir);
    }
};
/**
 * Port directions address coordinates relative to each other by specifying cardinal directions. This function converts
 * such a relative direction to an absolute coordinate when given a layout and a coordinate therein to consider. That
 * is, when presented with, e.g., a NORTH_EAST direction, it will return the coordinate that is to the NORTH_EAST of the
 * given coordinate c in the layout lyt.
 *
 * @tparam Lyt Coordinate layout type.
 * @param lyt Coordinate layout.
 * @param c Coordinate to consider.
 * @param port Port direction.
 * @return Absolute coordinate specified by a coordinate c in layout lyt and a port direction.
 */
template <typename Lyt>
[[nodiscard]] coordinate<Lyt> port_direction_to_coordinate(const Lyt& lyt, const coordinate<Lyt>& c,
                                                           const port_direction& port) noexcept
{
    static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");

    switch (port.dir)
    {
        case port_direction::cardinal::NORTH:
        {
            return lyt.north(c);
        }
        case port_direction::cardinal::NORTH_EAST:
        {
            return lyt.north_east(c);
        }
        case port_direction::cardinal::EAST:
        {
            return lyt.east(c);
        }
        case port_direction::cardinal::SOUTH_EAST:
        {
            return lyt.south_east(c);
        }
        case port_direction::cardinal::SOUTH:
        {
            return lyt.south(c);
        }
        case port_direction::cardinal::SOUTH_WEST:
        {
            return lyt.south_west(c);
        }
        case port_direction::cardinal::WEST:
        {
            return lyt.west(c);
        }
        case port_direction::cardinal::NORTH_WEST:
        {
            return lyt.north_west(c);
        }
        default:
        {
            assert(false && "Given port does not specify a cardinal direction");
        }
    }
}
/**
 * Port lists are collections of input and output ports.
 *
 * @tparam PortType A port type, e.g., port_position or port_direction.
 */
template <typename PortType>
struct port_list
{
    /**
     * Default constructor.
     */
    constexpr port_list() = default;
    /**
     * Standard constructor.
     */
    port_list(std::set<PortType> inp_, std::set<PortType> out_) : inp{std::move(inp_)}, out{std::move(out_)} {}
    /**
     * Input and output positions.
     */
    std::set<PortType> inp{}, out{};
    /**
     * Comparator for unordered_set/map.
     *
     * @param p Ports to compare to.
     * @return True iff these ports are equal to p.
     */
    bool operator==(const port_list<PortType>& p) const
    {
        return this->inp == p.inp && this->out == p.out;
    }
    /**
     * Merges two port_list objects together. The given port_list might be altered.
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
    auto format(const fiction::port_position& p, FormatContext& ctx)
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
    auto format(const fiction::port_direction& p, FormatContext& ctx)
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
    auto format(const fiction::port_list<PortType>& pl, FormatContext& ctx)
    {
        return format_to(ctx.out(), "inp: {}, out: {}", join(pl.inp, ", "), join(pl.out, ", "));
    }
};
}  // namespace fmt

#endif  // FICTION_CELL_PORTS_HPP
