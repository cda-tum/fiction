//
// Created by marcel on 31.03.21.
//

#ifndef FICTION_TILE_BASED_LAYOUT_HPP
#define FICTION_TILE_BASED_LAYOUT_HPP

#include <map>
#include <ostream>
#include <set>
#include <vector>

#include <fmt/format.h>
#include <mockturtle/traits.hpp>
#include <mockturtle/networks/detail/foreach.hpp>

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
        if (z < other.z) return true;

        if (z == other.z)
        {
            if (y < other.y) return true;

            if (y == other.y) { return x < other.x; }
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
            ratio{aspect_ratio},
            l_storage{std::make_shared<layout_storage>()}
    {
        static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
    }

#pragma endregion

#pragma region Primary I / O and constants

    signal create_pi(const tile& t, std::string const& name = std::string())
    {
        static_assert(mockturtle::has_create_pi_v<Ntk>, "Ntk does not implement the create_pi function");

        auto s = Ntk::create_pi(name);
        assign_node(t, Ntk::get_node(s));
        l_storage->inputs.emplace_back(t);

        return s;
    }

    void create_po(const tile& t, signal const& f, std::string const& name = std::string())
    {
        static_assert(mockturtle::has_create_po_v<Ntk>, "Ntk does not implement the create_po function");

        Ntk::create_po(f, name);
        assign_signal(t, f);
        l_storage->outputs.emplace_back(t);
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

#pragma endregion

  private:
    aspect_ratio ratio;

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
};

}  // namespace fiction

#endif  // FICTION_TILE_BASED_LAYOUT_HPP
