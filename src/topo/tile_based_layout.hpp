//
// Created by marcel on 31.03.21.
//

#ifndef FICTION_TILE_BASED_LAYOUT_HPP
#define FICTION_TILE_BASED_LAYOUT_HPP

#include "clocking_scheme.hpp"
#include "coordinate.hpp"
#include "range.h"

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/networks/detail/foreach.hpp>
#include <mockturtle/networks/storage.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/truth_table_cache.hpp>

#include <cstdint>
#include <iterator>
#include <optional>
#include <ostream>
#include <set>
#include <unordered_map>
#include <vector>

namespace fiction
{

class tile_based_layout
{
  public:
#pragma region Types and constructors

    using tile         = coord_t;
    using aspect_ratio = coord_t;

    template <class AspectRatio, class Node, class Tile>
    struct tile_based_layout_storage_data
    {
        AspectRatio dimension;

        std::shared_ptr<clocking_scheme<coord_t>> clocking;

        mockturtle::truth_table_cache<kitty::dynamic_truth_table> cache;

        std::unordered_map<Tile, Node> tile_node_map{};
        std::unordered_map<Node, Tile> node_tile_map{};

        uint32_t trav_id = 0u;

        const Tile const0{0x8000000000000000ull};
        const Tile const1{0xc000000000000000ull};
    };

    /*! \brief tile-based layout node
     *
     * `data[0].h1`: Fan-out size
     * `data[0].h2`: Application-specific value
     * `data[1].h1`: Function literal in truth table cache
     * `data[2].h2`: Visited flags
     */
    struct tile_based_layout_storage_node : mockturtle::mixed_fanin_node<2>
    {
        bool operator==(tile_based_layout_storage_node const& other) const
        {
            return data[1].h1 == other.data[1].h1 && children == other.children;
        }
    };

    static constexpr auto min_fanin_size = 1;
    static constexpr auto max_fanin_size = 3;

    using base_type = tile_based_layout;
    using node      = uint64_t;
    using signal    = uint64_t;

    /*! \brief tile-based layout storage container */
    using tile_based_layout_storage =
        mockturtle::storage<tile_based_layout_storage_node, tile_based_layout_storage_data<aspect_ratio, node, signal>>;

    using storage = std::shared_ptr<tile_based_layout_storage>;

    explicit tile_based_layout(const aspect_ratio& aspect_ratio) : strg{std::make_shared<tile_based_layout_storage>()}
    {
        strg->data.dimension = aspect_ratio;
        strg->data.clocking  = std::make_shared<clocking_scheme<coord_t>>(open_4_clocking);
        initialize_truth_table_cache();
    }

    tile_based_layout(const aspect_ratio& aspect_ratio, const clocking_scheme<tile>& scheme) :
            strg{std::make_shared<tile_based_layout_storage>()}
    {
        strg->data.dimension = aspect_ratio;
        strg->data.clocking  = std::make_shared<clocking_scheme<coord_t>>(scheme);
        initialize_truth_table_cache();
    }

    explicit tile_based_layout(std::shared_ptr<tile_based_layout_storage> storage) : strg{std::move(storage)} {}

#pragma endregion

#pragma region Primary I / O and constants

    [[nodiscard]] signal get_constant(bool value = false) const noexcept
    {
        // const0 = (1,0,0,0), const1 = (1,1,0,0)
        return value ? strg->data.const1 : strg->data.const0;
    }

    [[nodiscard]] static bool is_constant(node const& n)
    {
        return n <= 1;
    }

    signal create_pi([[maybe_unused]] const std::string& name = std::string(), const tile& t = {})
    {
        const auto n = strg->nodes.size();
        strg->nodes.emplace_back();     // empty node data
        strg->nodes[n].data[1].h1 = 2;  // assign identity function
        strg->inputs.emplace_back(n);
        assign_node(t, n);

        return static_cast<signal>(t);
    }

    void create_po(const signal& f, [[maybe_unused]] const std::string& name = std::string(), const tile& t = {})
    {
        const auto n = strg->nodes.size();
        strg->nodes.emplace_back();     // empty node data
        strg->nodes[n].data[1].h1 = 2;  // assign identity function
        strg->outputs.emplace_back(n);  // TODO this line currently saves the created output node instead of its tile
                                        // TODO (signal). This might break stuff in mockturtle's algorithms.
                                        // TODO Reevaluate storing the tile instead!
        assign_node(t, n);

        /* increase ref-count to children */
        strg->nodes[get_node(f)].data[0].h1++;
    }

    [[nodiscard]] bool is_pi(const node& n) const
    {
        bool found = false;
        mockturtle::detail::foreach_element(strg->inputs.begin(), strg->inputs.end(),
                                            [&found, &n](auto const& node)
                                            {
                                                if (node == n)
                                                {
                                                    found = true;
                                                    return false;
                                                }
                                                return true;
                                            });
        return found;
    }

#pragma endregion

#pragma region Create function tiles

    signal create_buf(signal const& a, const tile& t = {})
    {
        return create_node_from_literal({a}, 2, t);
    }

    signal create_not(signal const& a, const tile& t = {})
    {
        return create_node_from_literal({a}, 3, t);
    }

    signal create_and(signal a, signal b, const tile& t = {})
    {
        return create_node_from_literal({a, b}, 4, t);
    }

    signal create_or(signal a, signal b, const tile& t = {})
    {
        return create_node_from_literal({a, b}, 6, t);
    }

    signal create_xor(signal a, signal b, const tile& t = {})
    {
        return create_node_from_literal({a, b}, 8, t);
    }

    signal create_maj(signal a, signal b, signal c, const tile& t = {})
    {
        return create_node_from_literal({a, b, c}, 10, t);
    }

    signal create_node(const std::vector<signal>& children, const kitty::dynamic_truth_table& function,
                       const tile& t = {})
    {
        if (children.empty())
        {
            return {};
        }
        return create_node_from_literal(children, strg->data.cache.insert(function), t);
    }

#pragma endregion

#pragma region Functional properties

    [[nodiscard]] kitty::dynamic_truth_table node_function(const node& n) const
    {
        return strg->data.cache[strg->nodes[n].data[1].h1];
    }

#pragma endregion

#pragma region Structural properties

    [[nodiscard]] auto size() const
    {
        return static_cast<uint32_t>(strg->nodes.size() - 2);
    }

    [[nodiscard]] auto num_pis() const
    {
        return strg->inputs.size();
    }

    [[nodiscard]] auto num_pos() const
    {
        return strg->outputs.size();
    }

    // TODO num_gates und num_wires mitführen
    [[nodiscard]] auto num_gates() const
    {
        return static_cast<uint32_t>(strg->data.tile_node_map.size() - strg->inputs.size() - strg->outputs.size());
    }

#pragma endregion

#pragma region Nodes and signals

    [[nodiscard]] node get_node(const signal& f) const
    {
        if (auto it = strg->data.tile_node_map.find(f); it != strg->data.tile_node_map.end())
        {
            return it->second;
        }

        return 0;
    }

    [[nodiscard]] node get_node(const tile& t) const
    {
        return get_node(static_cast<signal>(t));
    }

    [[nodiscard]] tile get_tile(const node& n) const noexcept
    {
        if (auto it = strg->data.node_tile_map.find(n); it != strg->data.node_tile_map.end())
        {
            return static_cast<tile>(it->second);
        }

        return {};
    }

    [[nodiscard]] static bool is_complemented([[maybe_unused]] const signal& f)
    {
        return false;
    }

    // TODO if no PI / PO assigned and function literal > 2
    [[nodiscard]] bool is_gate_tile(const tile& t) const noexcept
    {
        if (auto it = strg->data.tile_node_map.find(static_cast<signal>(t)); it != strg->data.tile_node_map.end())
        {
            return strg->nodes[it->second].data[1].h1 > 2;
        }
        else
        {
            return false;
        }
    }

    // TODO if no PI / PO assigned and function literal == 2
    [[nodiscard]] bool is_wire_tile(const tile& t) const noexcept
    {
        if (auto it = strg->data.tile_node_map.find(static_cast<signal>(t)); it != strg->data.tile_node_map.end())
        {
            return strg->nodes[it->second].data[1].h1 == 2;
        }
        else
        {
            return false;
        }
    }

    [[nodiscard]] bool is_empty_tile(const tile& t) const noexcept
    {
        return !is_gate_tile(t) && !is_wire_tile(t);
    }

#pragma endregion

#pragma region tile iterators

    template <typename Fn>
    void foreach_pi(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(strg->inputs.cbegin(), strg->inputs.cend(), fn);
    }

    template <typename Fn>
    void foreach_po(Fn&& fn) const
    {
        using IteratorType = decltype(strg->outputs.begin());
        mockturtle::detail::foreach_element_transform<IteratorType, uint32_t>(
            strg->outputs.begin(), strg->outputs.end(), [](const auto& o) { return o.index; }, fn);
    }

    [[nodiscard]] auto tiles(const tile& start = {}, const tile& stop = {}) const
    {
        return range_t{
            std::make_pair(coord_iterator{strg->data.dimension, start}, coord_iterator{strg->data.dimension, stop})};
    }

    [[nodiscard]] auto ground_tiles(const tile& start = {}, const tile& stop = {}) const
    {
        assert(start.z == 0 && stop.z == 0);

        auto ground_layer = aspect_ratio{strg->data.dimension.x, strg->data.dimension.y, 1};

        return range_t{std::make_pair(coord_iterator{ground_layer, start}, coord_iterator{ground_layer, stop})};
    }

#pragma endregion

#pragma region clocking

    void assign_clocking(const tile& t, const clocking_scheme<tile>::zone z) noexcept
    {
        strg->data.clocking->override_clock_zone(t, z);
    }

    [[nodiscard]] clocking_scheme<tile>::zone tile_clocking(const tile& t) const noexcept
    {
        return (*strg->data.clocking)(t);
    }

    [[nodiscard]] clocking_scheme<tile>::number num_clocks() const noexcept
    {
        return strg->data.clocking->num_clocks;
    }

    [[nodiscard]] bool is_regularly_clocked() const noexcept
    {
        return strg->data.clocking->is_regular();
    }

    [[nodiscard]] bool is_clocking(std::string&& name) const noexcept
    {
        return *strg->data.clocking == name;
    }

#pragma endregion

#pragma region Custom node values

    void clear_values() const
    {
        std::for_each(strg->nodes.begin(), strg->nodes.end(), [](auto& n) { n.data[0].h2 = 0; });
    }

    [[nodiscard]] uint32_t value(node const& n) const
    {
        return strg->nodes[n].data[0].h2;
    }

    void set_value(node const& n, uint32_t v) const
    {
        strg->nodes[n].data[0].h2 = v;
    }

    uint32_t incr_value(node const& n) const
    {
        return static_cast<uint32_t>(strg->nodes[n].data[0].h2++);
    }

    uint32_t decr_value(node const& n) const
    {
        return static_cast<uint32_t>(--strg->nodes[n].data[0].h2);
    }

#pragma endregion

#pragma region Visited flags

    void clear_visited() const
    {
        std::for_each(strg->nodes.begin(), strg->nodes.end(), [](auto& n) { n.data[1].h2 = 0; });
    }

    [[nodiscard]] auto visited(node const& n) const
    {
        return strg->nodes[n].data[1].h2;
    }

    void set_visited(node const& n, uint32_t v) const
    {
        strg->nodes[n].data[1].h2 = v;
    }

    [[nodiscard]] uint32_t trav_id() const
    {
        return strg->data.trav_id;
    }

    void incr_trav_id() const
    {
        ++strg->data.trav_id;
    }

#pragma endregion

  private:
    storage strg;

    inline void initialize_truth_table_cache()
    {
        /* reserve the second node for constant 1 */
        strg->nodes.emplace_back();

        kitty::dynamic_truth_table tt_zero(0);
        strg->data.cache.insert(tt_zero);

        strg->nodes[0].data[1].h1 = 0;
        strg->nodes[1].data[1].h1 = 1;

        /* reserve some truth tables for nodes */
        const auto create_and_cache = [this](const auto& literal, auto n)
        {
            kitty::dynamic_truth_table tt(n);
            kitty::create_from_words(tt, &literal, &literal + 1);
            strg->data.cache.insert(tt);
        };

        static constexpr const uint64_t lit_not = 0x1, lit_and = 0x8, lit_or = 0xe, lit_xor = 0x6, lit_maj = 0xe8;

        create_and_cache(lit_not, 1);  // since NOT is not normal, its complement, i.e., the identity, is stored
        create_and_cache(lit_and, 2);
        create_and_cache(lit_or, 2);
        create_and_cache(lit_xor, 2);
        create_and_cache(lit_maj, 3);
    }

    void assign_node(const tile& t, const node& n)
    {
        if (!t.is_dead())
        {
            strg->data.tile_node_map[static_cast<signal>(t)] = n;

            strg->data.node_tile_map[n] = static_cast<signal>(t);
        }
    }

    signal create_node_from_literal(const std::vector<signal>& children, uint32_t literal, const tile& t)
    {
        storage::element_type::node_type node_data;
        std::copy(children.begin(), children.end(), std::back_inserter(node_data.children));
        node_data.data[1].h1 = literal;

        if (auto it = strg->hash.find(node_data); it != strg->hash.end())
        {
            return static_cast<signal>(get_tile(it->second));
        }

        const auto n = strg->nodes.size();
        strg->nodes.push_back(node_data);
        strg->hash[node_data] = n;

        /* increase ref-count to children */
        for (const auto& c : children) { strg->nodes[get_node(c)].data[0].h1++; }

        set_value(n, 0);

        assign_node(t, n);

        //        for (auto const& fn : _events->on_add) { fn(n); }

        return static_cast<signal>(t);
    }

    void clear_tile(const tile& t)
    {
        if (auto it = strg->data.tile_node_map.find(static_cast<signal>(t)); it != strg->data.tile_node_map.end())
        {
            // remove node-tile
            strg->data.node_tile_map.erase(it->second);
            // remove tile-node
            strg->data.tile_node_map.erase(it);
        }
    }
};

}  // namespace fiction

#endif  // FICTION_TILE_BASED_LAYOUT_HPP
