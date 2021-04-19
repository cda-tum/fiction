//
// Created by marcel on 31.03.21.
//

#ifndef FICTION_TILE_BASED_LAYOUT_HPP
#define FICTION_TILE_BASED_LAYOUT_HPP

#include "range.h"

#include <map>
#include <optional>
#include <ostream>
#include <set>
#include <vector>

#include <fmt/format.h>
#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/networks/detail/foreach.hpp>
#include <mockturtle/traits.hpp>

namespace fiction
{
struct tile
{
    // could use MSB for dead indicator
    uint64_t z : 2;
    uint64_t y : 31;
    uint64_t x : 31;

    constexpr tile() : z{static_cast<uint64_t>(0ul)}, y{static_cast<uint64_t>(0ul)}, x{static_cast<uint64_t>(0ul)} {}

    template <class X, class Y, class Z>
    constexpr tile(X x, Y y, Z z) :
            z{static_cast<uint64_t>(z)},
            y{static_cast<uint64_t>(y)},
            x{static_cast<uint64_t>(x)}
    {}

    template <class X, class Y>
    constexpr tile(X x, Y y) : z{static_cast<uint64_t>(0)}, y{static_cast<uint64_t>(y)}, x{static_cast<uint64_t>(x)}
    {}

    constexpr bool operator==(const tile& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

    constexpr bool operator!=(const tile& other) const
    {
        return !(*this == other);
    }

    constexpr bool operator<(const tile& other) const
    {
        if (z < other.z)
            return true;

        if (z == other.z)
        {
            if (y < other.y)
                return true;

            if (y == other.y)
            {
                return x < other.x;
            }
        }

        return false;
    }

    constexpr bool operator>(const tile& other) const
    {
        return other < *this;
    }

    constexpr bool operator<=(const tile& other) const
    {
        return !(*this > other);
    }

    constexpr bool operator>=(const tile& other) const
    {
        return !(*this < other);
    }
};

std::ostream& operator<<(std::ostream& os, const tile& t)
{
    os << fmt::format("({},{},{})", t.x, t.y, t.z);
    return os;
}

template <class Ntk>
class tile_based_layout : public Ntk
{
  public:
#pragma region Types and constructors

    using base_type    = tile_based_layout<Ntk>;
    using storage      = typename Ntk::storage;
    using node         = typename Ntk::node;
    using signal       = typename Ntk::signal;
    using aspect_ratio = tile;

    explicit tile_based_layout(const aspect_ratio& aspect_ratio) :
            Ntk(),
            dimension{aspect_ratio},
            l_storage{std::make_shared<layout_storage>()}
    {
        static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
    }

#pragma endregion

#pragma region Primary I / O and constants

    signal create_pi_tile(const tile& t, std::string const& name = std::string())
    {
        static_assert(mockturtle::has_create_pi_v<Ntk>, "Ntk does not implement the create_pi function");

        auto s = Ntk::create_pi(name);
        assign_node(t, Ntk::get_node(s));
        l_storage->inputs.emplace_back(t);

        return s;
    }

    void create_po_tile(const tile& t, signal const& f, std::string const& name = std::string())
    {
        static_assert(mockturtle::has_create_po_v<Ntk>, "Ntk does not implement the create_po function");

        Ntk::create_po(f, name);
        assign_signal(t, f);
        l_storage->outputs.emplace_back(t);
    }

#pragma endregion

#pragma region Create unary functions

    signal create_buf_tile(const tile& t, signal const& a)
    {
        static_assert(mockturtle::has_create_buf_v<Ntk>, "Ntk does not implement the create_buf function");

        auto s = Ntk::create_buf(a);
        assign_signal(t, s);

        return s;
    }

    signal create_not_tile(const tile& t, signal const& a)
    {
        static_assert(mockturtle::has_create_not_v<Ntk>, "Ntk does not implement the create_not function");

        auto s = Ntk::create_not(a);
        assign_signal(t, s);

        return s;
    }

#pragma endregion

#pragma region Create binary functions

    signal create_and_tile(const tile& t, signal a, signal b)
    {
        static_assert(mockturtle::has_create_and_v<Ntk>, "Ntk does not implement the create_and function");

        auto s = Ntk::create_and(a, b);
        assign_node(t, Ntk::get_node(s));

        return s;
    }

    signal create_or_tile(const tile& t, signal a, signal b)
    {
        static_assert(mockturtle::has_create_or_v<Ntk>, "Ntk does not implement the create_or function");

        auto s = Ntk::create_or(a, b);
        assign_node(t, Ntk::get_node(s));

        return s;
    }

#pragma endregion

#pragma region Createy ternary functions

    signal create_maj_tile(const tile& t, signal a, signal b, signal c)
    {
        static_assert(mockturtle::has_create_maj_v<Ntk>, "Ntk does not implement the create_maj function");

        auto s = Ntk::create_maj(a, b, c);
        assign_node(t, Ntk::get_node(s));

        return s;
    }

#pragma endregion

#pragma region Functional properties

    [[nodiscard]] std::optional<kitty::dynamic_truth_table> tile_function(const tile& t) const
    {
        static_assert(mockturtle::has_node_function_v<Ntk>, "Ntk does not implement the node_function function");

        if (auto n = get_tile_node(t); n)
        {
            return Ntk::node_function(*n);
        }

        return std::nullopt;
    }

#pragma endregion

#pragma region Nodes and signals

    [[nodiscard]] std::optional<node> get_tile_node(const tile& t) const
    {
        if (auto it = l_storage->tile_node_map.find(t); it != l_storage->tile_node_map.end())
        {
            return it->second;
        }

        return std::nullopt;
    }

    [[nodiscard]] std::optional<tile> get_node_tile(const node& n) const
    {
        if (auto it = l_storage->node_tile_map.find(n); it != l_storage->node_tile_map.end())
        {
            return it->second;
        }

        return std::nullopt;
    }

    [[nodiscard]] std::set<signal> get_tile_signals(const tile& t) const
    {
        if (auto it = l_storage->tile_signal_map.find(t); it != l_storage->tile_signal_map.end())
        {
            return it->second;
        }

        return {};
    }

#pragma endregion

#pragma region tile iterators

    template <typename Fn>
    void foreach_pi_tile(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(l_storage->inputs.cbegin(), l_storage->inputs.cend(), fn);
    }

    template <typename Fn>
    void foreach_po_tile(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(l_storage->outputs.cbegin(), l_storage->outputs.cend(), fn);
    }

    [[nodiscard]] auto tiles(const tile& start = {}, const tile& stop = {}) const
    {
        return range_t{std::make_pair(tile_iterator{dimension, start}, tile_iterator{dimension, stop})};
    }

    [[nodiscard]] auto ground_tiles(const tile& start = {}, const tile& stop = {}) const
    {
        assert(start.z == 0 && stop.z == 0);

        auto ground_layer = aspect_ratio{dimension.x, dimension.y, 1};

        return range_t{std::make_pair(tile_iterator{ground_layer, start}, tile_iterator{ground_layer, stop})};
    }

#pragma endregion

  private:
    aspect_ratio dimension;

    struct layout_storage
    {
        std::vector<tile> inputs{}, outputs{};

        std::map<tile, node> tile_node_map{};
        std::map<node, tile> node_tile_map{};

        std::map<tile, std::set<signal>> tile_signal_map{};
    };

    std::shared_ptr<layout_storage> l_storage;

    void assign_node(const tile& t, const node& n)
    {
        l_storage->tile_node_map[t] = n;
        l_storage->node_tile_map[n] = t;
    }

    void assign_signal(const tile& t, const signal& s)
    {
        l_storage->tile_signal_map[t].insert(s);
    }

    // TODO think about PI/PO tiles
    void clear_tile(const tile& t)
    {
        if (auto it = l_storage->tile_node_map.find(t); it != l_storage->tile_signal_map.end())
        {
            // remove node-tile
            l_storage->node_tile_map.erase(it->second);
            // remove tile-node
            l_storage->tile_node_map.erase(it);
        }

        // remove signals
        l_storage->tile_signal_map.erase(t);
    }

    class tile_iterator
    {
      public:
        constexpr explicit tile_iterator(const aspect_ratio& dimension, const tile& t = {}) noexcept :
                dimension{dimension},
                t{t}
        {}

        constexpr tile_iterator& operator++() noexcept
        {
            ++t.x;

            if (t.x >= dimension.x)
            {
                t.x = 0;

                ++t.y;
                if (t.y >= dimension.y)
                {
                    t.y = 0;

                    ++t.z;
                    if (t.z >= dimension.z)
                    {
                        t = dimension;
                    }
                }
            }

            return *this;
        }

        constexpr tile_iterator operator++(int) noexcept
        {
            auto result{*this};

            ++(*this);

            return result;
        }

        constexpr tile operator*() const
        {
            return t;
        }

        constexpr bool operator==(const tile_iterator& other) const
        {
            return (t == other.t);
        }

        constexpr bool operator!=(const tile_iterator& other) const
        {
            return !(*this == other);
        }

        constexpr bool operator<(const tile_iterator& other) const
        {
            return (t < other.t);
        }

        constexpr bool operator<=(const tile_iterator& other) const
        {
            return (t <= other.t);
        }

      private:
        const aspect_ratio dimension;

        tile t;
    };
};

}  // namespace fiction

#endif  // FICTION_TILE_BASED_LAYOUT_HPP
