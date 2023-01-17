//
// Created by marcel on 16.01.23.
//

#ifndef FICTION_PLACEMENT_LAYOUT_HPP
#define FICTION_PLACEMENT_LAYOUT_HPP

#include "fiction/traits.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <memory>
#include <random>
#include <type_traits>
#include <utility>
#include <vector>

namespace fiction
{

template <typename GateLyt, typename Ntk,
          bool has_placement_interface = std::conjunction_v<has_initialize_random_placement<GateLyt>,
                                                            has_random_tile<GateLyt>, has_random_node<GateLyt>>>
class placement_layout : public GateLyt
{};

template <typename GateLyt, typename Ntk>
class placement_layout<GateLyt, Ntk, true> : public GateLyt
{
  public:
    explicit placement_layout(const Ntk&) : GateLyt() {}
    placement_layout(const GateLyt& lyt, const Ntk&) : GateLyt(lyt) {}
};

template <typename GateLyt, typename Ntk>
class placement_layout<GateLyt, Ntk, false> : public GateLyt
{
  public:
    using tile = typename GateLyt::tile;

    struct placement_layout_storage
    {
        /**
         * Default constructor.
         */
        explicit placement_layout_storage(const Ntk& network) :
                ntk{network},
                node_to_tile{network},
                random_node_functor{ntk.get_constant(false) == ntk.get_constant(true) ? 1u : 2u, ntk.size() - 1u}
        {}
        /**
         * The associated network
         */
        const Ntk& ntk;
        /**
         * Maps network nodes to their layout coordinates.
         */
        mockturtle::node_map<coordinate<GateLyt>, Ntk> node_to_tile;
        /**
         * Maps layout coordinate indices to network nodes.
         */
        std::vector<mockturtle::node<Ntk>> tile_index_to_node;
        /**
         * A random-number generator.
         */
        std::mt19937_64 generator{std::random_device{}()};

        std::uniform_int_distribution<uint64_t> random_x_functor;
        std::uniform_int_distribution<uint64_t> random_y_functor;
        std::uniform_int_distribution<uint32_t> random_node_functor;
    };

    using storage = std::shared_ptr<placement_layout_storage>;

    /**
     * Standard constructor that layers the placement interface on top of an existing layout.
     *
     * @param lyt Existing layout that is to be extended by a placement interface.
     * @param ntk The network whose nodes are to be placed.
     */
    placement_layout(const GateLyt& lyt, const Ntk& ntk) :
            GateLyt(lyt),
            strg{std::make_shared<placement_layout_storage>(ntk)}
    {
        static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt is not a gate level layout");
        static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");

        strg->random_x_functor   = std::uniform_int_distribution<uint64_t>(0, this->x());
        strg->random_y_functor   = std::uniform_int_distribution<uint64_t>(0, this->y());
        strg->tile_index_to_node = std::vector<mockturtle::node<Ntk>>(this->area());
    }
    /**
     * The resize function is not available.
     */
    void resize(const typename GateLyt::aspect_ratio& ar) = delete;
    /**
     * Randomly places all nodes of the associated network on tiles of this layout. The function assumes that the
     * layout is empty prior to this call. It furthermore assumes that the network has at most as many nodes as the
     * layout has tiles.
     */
    void initialize_random_placement() noexcept
    {
        assert(this->is_empty() && "Layout is not empty");
        assert(strg->ntk.num_pis() + strg->ntk.num_gates() <= this->area() &&
               "The network has too many nodes for this layout");

        // generate an index for each tile
        std::vector<std::size_t> indices(this->area());
        std::iota(indices.begin(), indices.end(), 0);
        // shuffle the indices
        std::shuffle(indices.begin(), indices.end(), strg->generator);

        // place nodes
        strg->ntk.foreach_node(
            [this, &indices, i = 0u](auto const& n) mutable
            {
                if (!strg->ntk.is_constant(n))
                {
                    const auto y = static_cast<uint64_t>(indices[i] / (this->y() + 1));
                    const auto x = static_cast<uint64_t>(indices[i] % (this->y() + 1));

                    const auto t = tile{x, y};
                    // place node
                    strg->node_to_tile[n] = t;
                    // store node position
                    strg->tile_index_to_node[tile_to_index(t)] = n;
                }

                ++i;
            });
    }
    /**
     * Returns a random tile of this layout.
     *
     * @return A random tile.
     */
    tile random_tile() const noexcept
    {
        return {strg->random_x_functor(strg->generator), strg->random_x_functor(strg->generator)};
    }
    /**
     * Returns a random node of the associated network.
     *
     * @return A random node of the associated network.
     */
    mockturtle::node<Ntk> random_node() const noexcept
    {
        return strg->ntk.index_to_node(strg->random_node_functor(strg->generator));
    }
    /**
     * Swaps the position of the given node with the contents of the given tile. If the tile is empty, the node is
     * merely relocated.
     *
     * @param n The node to be moved.
     * @param t The tile to be moved to. If `t` is not empty, the node at `t` is moved to the position of `n`.
     */
    void swap_node_and_tile(const mockturtle::node<Ntk>& n, const tile& t) noexcept
    {
        const auto n_tile = strg->node_to_tile[n];
        const auto t_node = strg->tile_index_to_node[tile_to_index(t)];

        // swap node positions
        strg->node_to_tile[n]      = t;
        strg->node_to_tile[t_node] = n_tile;

        // swap tile contents
        strg->tile_index_to_node[tile_to_index(t)]      = n;
        strg->tile_index_to_node[tile_to_index(n_tile)] = t_node;

        // TODO think about how you can use std::swap here
    }
    //    void swap_random_tiles() noexcept
    //    {
    //        const auto t1 = random_tile();
    //        const auto t2 = random_tile();
    //
    //        std::swap(strg->tile_index_to_node[tile_to_index(t1)], strg->tile_index_to_node[tile_to_index(t2)]);
    //        std::swap(strg->node_to_tile[strg->tile_index_to_node[tile_to_index(t1)]],
    //                  strg->node_to_tile[strg->tile_index_to_node[tile_to_index(t2)]]);
    //    }
    /**
     * Returns the tile of a given node.
     *
     * @param n The node whose tile is to be returned.
     * @return The tile of the given node.
     */
    tile get_node_tile(const mockturtle::node<Ntk>& n) const noexcept
    {
        return strg->node_to_tile[n];
    }
    /**
     * Returns the node at a given tile.
     *
     * @param t The tile whose node is to be returned.
     * @return The node at the given tile.
     */
    mockturtle::node<Ntk> get_tile_node(const tile& t) const noexcept
    {
        return strg->tile_index_to_node[tile_to_index(t)];
    }
    /**
     *
     * @tparam Fn
     * @param fn
     */
    template <typename Fn>
    void foreach_placed_node(Fn&& fn) const
    {
        strg->ntk.foreach_node(
            [this, &fn](auto const& n)
            {
                if (!strg->ntk.is_constant(n))
                {
                    if (const auto t = strg->node_to_tile[n]; !t.is_dead())
                    {
                        assert(t == strg->node_to_tile[n] && "node placement data is out of sync");

                        std::invoke(std::forward<Fn>(fn), n, t);
                    }
                }
            });
    }

  private:
    storage strg;

    std::size_t tile_to_index(const tile& t) const noexcept
    {
        return t.y * (this->y() + 1) + t.x;
    }
};

template <class T, class U>
placement_layout(const T&, const U&) -> placement_layout<T, U>;

}  // namespace fiction

#endif  // FICTION_PLACEMENT_LAYOUT_HPP
