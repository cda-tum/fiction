//
// Created by marcel on 31.03.21.
//

#ifndef FICTION_TILE_BASED_LAYOUT_HPP
#define FICTION_TILE_BASED_LAYOUT_HPP

#include <mockturtle/traits.hpp>

namespace fiction
{
    struct tile
    {
        // could use MSB for dead indicator
        uint64_t z: 2;
        uint64_t y: 31;
        uint64_t x: 31;

        template<class X, class Y, class Z>
        constexpr tile(X x, Y y, Z z) : z{z}, y{y}, x{x} {}

        template<class X, class Y>
        constexpr tile(X x, Y y) : z{0}, y{y}, x{x} {}

        constexpr bool operator==(const tile& other) const
        {
            return x == other.x && y == other.y && z == other.z;
        }
    };

    using aspect_ratio = tile;

    template <class Ntk>
    struct layout_node : public Ntk::node
    {
        tile t{0, 0, 0};

        bool operator==(layout_node const& other) const
        {
            t.x == other.t.x &&
            t.y == other.t.y &&
            t.z == other.t.z &&
            this->Ntk::node == other.Ntk::node;
        }
    };

    template<class Ntk>
    class tile_based_layout : public Ntk
    {
    public:
#pragma region Types and constructors

        using base_type = tile_based_layout<Ntk>;
        using storage = typename Ntk::storage;
        using node = layout_node<Ntk>;
        using signal = typename Ntk::signal;

        explicit tile_based_layout(const aspect_ratio& aspect_ratio)
                :
                Ntk(),
                aspect_ratio{aspect_ratio}
        {
            static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
        }

#pragma endregion

#pragma region Primary I / O and constants

    private:
        aspect_ratio aspect_ratio;
    };
}

#endif //FICTION_TILE_BASED_LAYOUT_HPP
