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
