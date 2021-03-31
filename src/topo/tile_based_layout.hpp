//
// Created by marcel on 31.03.21.
//

#ifndef FICTION_TILE_BASED_LAYOUT_HPP
#define FICTION_TILE_BASED_LAYOUT_HPP


namespace fiction
{
    union lcoord64_t
    {
        struct
        {
            uint64_t z: 2;
            uint64_t y: 31;
            uint64_t x: 31;
        };
    };

    constexpr bool operator==(const lcoord64_t& coord1, const lcoord64_t& coord2)
    {
        return coord1.x == coord2.x && coord1.y == coord2.y && coord1.z == coord2.z;
    }

    template <class Ntk>
    struct layout_node : public Ntk::node
    {
        lcoord64_t coord;

        bool operator==(layout_node const& other) const
        {
            coord.x == other.coord.x &&
            coord.y == other.coord.y &&
            coord.z == other.coord.z &&
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

        template<class X, class Y, class Z>
        constexpr lcoord64_t tile(X x, Y y, Z z) const
        {
            lcoord64_t coord{};
            coord.x = x;
            coord.y = y;
            coord.z = z;

            return coord;
        }

        template<class X, class Y>
        constexpr lcoord64_t tile(X x, Y y) const
        {
            return tile(x, y, 0);
        }

        tile_based_layout()
                :
                Ntk()
        {
        }

#pragma endregion

#pragma region Primary I / O and constants

    private:
    };
}

#endif //FICTION_TILE_BASED_LAYOUT_HPP
