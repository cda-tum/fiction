//
// Created by marcel on 29.09.18.
//

#ifndef FICTION_CELL_PORTS_HPP
#define FICTION_CELL_PORTS_HPP

#include "../utils/hash.hpp"

#include <cstdint>
#include <functional>
#include <set>
#include <unordered_map>

namespace fiction
{
/**
 * A port is a position of a cell relative to i.e. within a tile.
 */
struct port
{
    /**
     * Default constructor.
     */
    constexpr port() = default;
    /**
     * Standard constructor.
     */
    constexpr port(const uint16_t x_, const uint16_t y_, const bool pi_ = false, const bool po_ = false) :
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
    constexpr bool operator<(const port& p) const
    {
        return (this->x < p.x) || ((this->x == p.x) && (this->y < p.y));
    }
    /**
     * Comparator for equality tests.
     *
     * @param p Port to compare to.
     * @return True iff this port is equal to given port p.
     */
    constexpr bool operator==(const port& p) const
    {
        return (this->x == p.x) && (this->y == p.y);
    }
};
/**
 * Port lists are collections of input and output identifiers.
 */
struct port_list
{
    /**
     * Default constructor.
     */
    constexpr port_list() = default;
    /**
     * Standard constructor.
     */
    port_list(std::set<port> inp_, std::set<port> out_) : inp{std::move(inp_)}, out{std::move(out_)} {}
    /**
     * Input and output positions.
     */
    std::set<port> inp{}, out{};
    /**
     * Comparator for unordered_set/map.
     *
     * @param p Ports to compare to.
     * @return True iff these ports are equal to p.
     */
    bool operator==(const port_list& p) const
    {
        return this->inp == p.inp && this->out == p.out;
    }
};

}  // namespace fiction

namespace std
{

template <>
struct hash<fiction::port>
{
    std::size_t operator()(const fiction::port& port) const noexcept
    {
        std::size_t h = port.x ^ port.y;
        fiction::hash_combine(h, port.x, port.y);

        return h;
    }
};

template <>
struct hash<fiction::port_list>
{
    std::size_t operator()(const fiction::port_list& port_list) const noexcept
    {
        std::size_t h = 0;
        fiction::hash_combine(h, port_list.inp, port_list.out);

        return h;
    }
};

}  // namespace std

#endif  // FICTION_CELL_PORTS_HPP
