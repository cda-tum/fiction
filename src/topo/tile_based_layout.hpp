//
// Created by marcel on 31.03.21.
//

#ifndef FICTION_TILE_BASED_LAYOUT_HPP
#define FICTION_TILE_BASED_LAYOUT_HPP

#include <memory>

#include <mockturtle/utils/truth_table_cache.hpp>
#include <mockturtle/networks/events.hpp>
#include <mockturtle/networks/storage.hpp>

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>

namespace fiction
{
    struct layout_storage_data
    {
        mockturtle::truth_table_cache <kitty::dynamic_truth_table> cache{};
        uint32_t num_pis = 0u;
        uint32_t num_pos = 0u;
        uint32_t trav_id = 0u;
    };

    /*! \brief layout node
     *
     * `data[0].h1`: x-position
     * `data[0].h2`: y-position
     * `data[1].h1`: z-position
     * `data[1].h2`: Application-specific value
     * `data[2].h1`: Function literal in truth table cache
     * `data[2].h2`: Visited flags
     */
    struct layout_node : mockturtle::mixed_fanin_node<3>
    {
        bool operator==(layout_node const& other) const
        {
            return data[0].h1 == other.data[0].h1 && // x-pos
                   data[0].h2 == other.data[0].h2 && // y-pos
                   data[1].h1 == other.data[1].h1 && // z-pos
                   data[2].h1 == other.data[2].h1 && // function literals
                   children == other.children;
        }
    };

    using layout_storage = mockturtle::storage<layout_node, layout_storage_data>;

    template<class Ntk>
    class tile_based_layout
    {
    public:
#pragma region Types and constructors
        static constexpr auto min_fanin_size = 1;
        static constexpr auto max_fanin_size = 3;

        using base_type = tile_based_layout<Ntk>;
        using storage = std::shared_ptr<layout_storage>;
        using node = uint64_t;
        using signal = uint64_t;

        tile_based_layout()
                :
                strg{std::make_shared<layout_storage>()},
                evn{std::make_shared<mockturtle::network_events<base_type>>()}
        {
            init();
            evn->on_add.push_back({});
        }

        explicit tile_based_layout(std::shared_ptr<layout_storage> s)
                :
                strg{s},
                evn{std::make_shared<mockturtle::network_events<base_type>>()}
        {
            init();
        }

    private:
        inline void init()
        {
            // reserve the second node for constant 1
            strg->nodes.emplace_back();

            // reserve some truth tables for nodes
            kitty::dynamic_truth_table tt_zero(0);
            strg->data.cache.insert(tt_zero);

            static const auto tt_insert = [this](const auto f, const auto size) -> void
            {
                kitty::dynamic_truth_table tt(size);
                kitty::create_from_words(tt, &f, &f + 1);
                strg->data.cache.insert(tt);
            };

            static constexpr uint64_t inv = 0x1;
            static constexpr uint64_t and2 = 0x8;
            static constexpr uint64_t or2 = 0xe;
//            static constexpr uint64_t xor2 = 0x6;
            static constexpr uint64_t maj3 = 0xe8;

            tt_insert(inv, 1);
            tt_insert(and2, 2);
            tt_insert(or2, 2);
//            tt_insert(xor2, 2);
            tt_insert(maj3, 3);

            // truth tables for constants
            strg->nodes[0].data[1].h1 = 0;
            strg->nodes[1].data[1].h1 = 1;
        }
#pragma endregion


#pragma region General methods
        auto& events() const
        {
            return *evn;
        }
#pragma endregion

    private:
        std::shared_ptr<layout_storage> strg{};
        std::shared_ptr<mockturtle::network_events<base_type>> evn{};
    };
}

#endif //FICTION_TILE_BASED_LAYOUT_HPP
