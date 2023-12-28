//
// Created by marcel on 16.01.23.
//

#ifndef FICTION_PLACEMENT_LAYOUT_HPP
#define FICTION_PLACEMENT_LAYOUT_HPP

#include "fiction/algorithms/path_finding/distance.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/name_utils.hpp"
#include "fiction/utils/placement_utils.hpp"

#include <fmt/format.h>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/views/fanout_view.hpp>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iterator>
#include <memory>
#include <random>
#include <type_traits>
#include <utility>
#include <valarray>
#include <vector>

namespace fiction
{

template <typename GateLyt, typename Ntk, typename Dist = uint64_t,
          bool has_placement_interface = std::conjunction_v<has_initialize_random_placement<GateLyt>,
                                                            has_random_tile<GateLyt>, has_random_node<GateLyt>>>
class placement_layout : public GateLyt
{};

template <typename GateLyt, typename Ntk, typename Dist>
class placement_layout<GateLyt, Ntk, Dist, true> : public GateLyt
{
  public:
    placement_layout(const GateLyt& lyt, const Ntk&,
                     const distance_functor<GateLyt, Dist>& = manhattan_distance_functor<GateLyt, uint64_t>{}) :
            GateLyt(lyt)
    {}
};

template <typename GateLyt, typename Ntk, typename Dist>
class placement_layout<GateLyt, Ntk, Dist, false> : public GateLyt
{
  public:
    using tile = typename GateLyt::tile;

    struct placement_layout_storage
    {
        /**
         * Default constructor.
         */
        placement_layout_storage(const Ntk& network, const distance_functor<GateLyt, Dist>& dist) :
                ntk{network},
                dist_fn{dist},
                node_to_tile{network},
                net_costs(Dist{0}, node_to_tile.size()),
                random_node_functor{ntk.num_pis() + ntk.num_gates() == 0u             ? 0u :
                                    ntk.get_constant(false) == ntk.get_constant(true) ? 1u :
                                                                                        2u,
                                    ntk.size() - 1u}
        {}

        placement_layout_storage(const placement_layout_storage& other) :
                ntk{other.ntk},
                dist_fn{other.dist_fn},
                node_to_tile{other.node_to_tile},
                tile_index_to_node{other.tile_index_to_node},
                net_costs{other.net_costs},
                generator{other.generator},
                random_x_functor{other.random_x_functor},
                random_y_functor{other.random_y_functor},
                random_border_tile_functor{other.random_border_tile_functor},
                random_node_functor{other.random_node_functor}
        {}

        placement_layout_storage(placement_layout_storage&& other) noexcept :
                ntk{std::move(other.ntk)},
                dist_fn{std::move(other.dist_fn)},
                node_to_tile{std::move(other.node_to_tile)},
                tile_index_to_node{std::move(other.tile_index_to_node)},
                net_costs{std::move(other.net_costs)},
                generator{std::move(other.generator)},
                random_x_functor{std::move(other.random_x_functor)},
                random_y_functor{std::move(other.random_y_functor)},
                random_border_tile_functor{std::move(other.random_border_tile_functor)},
                random_node_functor{std::move(other.random_node_functor)}
        {}

        placement_layout_storage& operator=(const placement_layout_storage& other)
        {
            if (this != &other)
            {
                ntk                        = other.ntk;
                dist_fn                    = other.dist_fn;
                node_to_tile               = other.node_to_tile;
                tile_index_to_node         = other.tile_index_to_node;
                net_costs                  = other.net_costs;
                generator                  = other.generator;
                random_x_functor           = other.random_x_functor;
                random_y_functor           = other.random_y_functor;
                random_border_tile_functor = other.random_border_tile_functor;
                random_node_functor        = other.random_node_functor;
            }

            return *this;
        }

        placement_layout_storage& operator=(placement_layout_storage&& other) noexcept
        {
            if (this != &other)
            {
                ntk                        = std::move(other.ntk);
                dist_fn                    = std::move(other.dist_fn);
                node_to_tile               = std::move(other.node_to_tile);
                tile_index_to_node         = std::move(other.tile_index_to_node);
                net_costs                  = std::move(other.net_costs);
                generator                  = std::move(other.generator);
                random_x_functor           = std::move(other.random_x_functor);
                random_y_functor           = std::move(other.random_y_functor);
                random_border_tile_functor = std::move(other.random_border_tile_functor);
                random_node_functor        = std::move(other.random_node_functor);
            }

            return *this;
        }

        ~placement_layout_storage() noexcept = default;

        /**
         * The associated network wrapped in a fanout view to enable the foreach_fanout function.
         */
        const mockturtle::fanout_view<Ntk> ntk;
        /**
         * Distance cost functor to estimate wire costs.
         */
        const distance_functor<GateLyt, Dist> dist_fn;
        /**
         * Maps network nodes to their layout coordinates.
         */
        mockturtle::node_map<tile, Ntk> node_to_tile;
        /**
         * Maps layout coordinate indices to network nodes.
         */
        std::vector<mockturtle::node<Ntk>> tile_index_to_node;
        /**
         * Stores the approximated net costs of each node.
         */
        std::valarray<Dist> net_costs;
        /**
         * Stores the most recently swapped node-tile pair.
         */
        std::pair<mockturtle::node<GateLyt>, tile> last_swap{};
        /**
         * A random-number generator.
         */
        std::mt19937_64 generator{std::random_device{}()};
        /**
         * Random functor for the layout's x-dimension.
         */
        std::uniform_int_distribution<uint64_t> random_x_functor;
        /**
         * Random functor for the layout's y-dimension.
         */
        std::uniform_int_distribution<uint64_t> random_y_functor;
        /**
         * Random functor for the layout's border tiles.
         */
        std::uniform_int_distribution<uint64_t> random_border_tile_functor;
        /**
         * Random functor for the network's nodes.
         */
        std::uniform_int_distribution<uint32_t> random_node_functor;
    };

    using storage = std::shared_ptr<placement_layout_storage>;

    /**
     * Standard constructor that layers the placement interface on top of an existing layout.
     *
     * @param lyt Existing layout that is to be extended by a placement interface.
     * @param ntk The network whose nodes are to be placed.
     */
    placement_layout(const GateLyt& lyt, const Ntk& ntk,
                     const distance_functor<GateLyt, Dist>& dist_fn = manhattan_distance_functor<GateLyt, uint64_t>{}) :
            GateLyt(lyt),
            strg{std::make_shared<placement_layout_storage>(ntk, dist_fn)}
    {
        static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt is not a gate level layout");
        static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");

        strg->random_x_functor = std::uniform_int_distribution<uint64_t>(0, this->x());
        strg->random_y_functor = std::uniform_int_distribution<uint64_t>(0, this->y());

        strg->random_border_tile_functor =
            std::uniform_int_distribution<uint64_t>(0, 2 * this->x() + 2 * this->y() - 4);

        strg->tile_index_to_node = std::vector<mockturtle::node<Ntk>>(this->area());
    }
    /**
     * Copy constructor. Creates a deep-copy of the placement layout and its associated storage.
     *
     * @param other The placement layout to be copied.
     */
    placement_layout(const placement_layout& other) :
            GateLyt(other),
            strg{std::make_shared<placement_layout_storage>(*other.strg)}
    {}
    /**
     * Move constructor. Creates a new layout storage and moves the contents.
     *
     * @param other The placement layout to be moved.
     */
    placement_layout(placement_layout&& other) noexcept :
            GateLyt(other),
            strg{std::make_shared<placement_layout_storage>(std::move(*other.strg))}
    {
        assert(strg->ntk.num_pis() + strg->ntk.num_gates() <= this->area() &&
               "The network has too many nodes for this layout");
    }
    /**
     * Copy assignment operator. Creates a deep-copy of the placement layout and its associated storage.
     *
     * @param other The placement layout to be copied.
     * @return A reference to this placement layout.
     */
    placement_layout& operator=(const placement_layout& other)
    {
        if (this != &other)
        {
            GateLyt::operator=(other);
            strg = std::make_shared<placement_layout_storage>(*other.strg);
        }

        return *this;
    }
    /**
     * Move assignment operator. Creates a new layout storage and moves the contents.
     *
     * @param other The placement layout to be moved.
     * @return A reference to this placement layout.
     */
    placement_layout& operator=(placement_layout&& other) noexcept
    {
        if (this != &other)
        {
            GateLyt::operator=(other);
            strg = std::make_shared<placement_layout_storage>(std::move(*other.strg));
        }

        return *this;
    }
    /**
     * Default destructor.
     */
    ~placement_layout() noexcept = default;
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

        std::vector<tile> available_tiles{};
        available_tiles.reserve(this->area());
        this->foreach_ground_coordinate([&available_tiles](const auto& c) { available_tiles.push_back(c); });

        std::vector<tile> sampled_tiles{};
        sampled_tiles.reserve(strg->ntk.size());

        std::sample(available_tiles.begin(), available_tiles.end(), std::back_inserter(sampled_tiles), strg->ntk.size(),
                    strg->generator);

        // place nodes
        strg->ntk.foreach_node(
            [this, &sampled_tiles, i = 0u](auto const& n) mutable
            {
                if (!strg->ntk.is_constant(n))
                {
                    const auto t = sampled_tiles[i];

                    // place node
                    strg->node_to_tile[n] = t;
                    // store node position
                    strg->tile_index_to_node[tile_to_index(t)] = n;
                }

                ++i;
            });

        initialize_net_costs();
    }
    /**
     * Assign node `n` to tile `t`.
     *
     * @param n Node to assign to `t`.
     * @param t Tile to be assigned `n`.
     */
    void place(const mockturtle::node<Ntk>& n, const tile& t) noexcept
    {
        assert(this->is_within_bounds(t) && "Tile is not within layout bounds");

        if (strg->ntk.is_constant(n))
        {
            return;
        }

        // place node
        strg->node_to_tile[n] = t;
        // store node position
        strg->tile_index_to_node[tile_to_index(t)] = n;

        //        update_net_costs(n);

        initialize_net_costs();
    }
    /**
     * Returns a random tile of this layout.
     *
     * @return A random tile.
     */
    [[nodiscard]] tile random_tile() const noexcept
    {
        return {strg->random_x_functor(strg->generator), strg->random_y_functor(strg->generator)};
    }
    /**
     * Returns a random tile at the border of this layout.
     *
     * @return A random border tile.
     */
    [[nodiscard]] tile random_border_tile() const noexcept
    {
        const auto border_tile_index = strg->random_border_tile_functor(strg->generator);

        if (border_tile_index < this->x())
        {
            return {border_tile_index, 0};
        }
        if (border_tile_index < this->x() + this->y() - 1)
        {
            return {this->x(), border_tile_index - this->x() + 1};
        }
        if (border_tile_index < 2 * this->x() + this->y() - 2)
        {
            return {2 * this->x() + this->y() - 2 - border_tile_index, this->y()};
        }

        return {0, 2 * this->x() + 2 * this->y() - 3 - border_tile_index};
    }
    /**
     * Returns a random node of the associated network.
     *
     * @return A random node of the associated network.
     */
    [[nodiscard]] mockturtle::node<Ntk> random_node() const noexcept
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
        assert(this->is_within_bounds(t) && "Tile is not within layout bounds");

        const auto n_tile = strg->node_to_tile[n];
        const auto t_node = strg->tile_index_to_node[tile_to_index(t)];

        // swap node positions
        strg->node_to_tile[n]      = t;
        strg->node_to_tile[t_node] = n_tile;

        // swap tile contents
        strg->tile_index_to_node[tile_to_index(t)]      = n;
        strg->tile_index_to_node[tile_to_index(n_tile)] = t_node;

        // store the given node and its original tile for potential undo
        strg->last_swap = {n, n_tile};

        // update costs
        //        update_net_costs(n);
        //        update_net_costs(t_node);

        initialize_net_costs();
    }
    /**
     * Swaps the position of a random node with the contents of a random tile. If the tile is empty, the node is
     * merely relocated.
     */
    void swap_random_node_and_random_tile() noexcept
    {
        const auto n = random_node();
        const auto t = random_tile();

        swap_node_and_tile(n, t);
    }
    /**
     * Undoes the most recent swap.
     */
    void undo_swap() noexcept
    {
        swap_node_and_tile(strg->last_swap.first, strg->last_swap.second);
    }
    /**
     * Returns the tile of a given node.
     *
     * @param n The node whose tile is to be returned.
     * @return The tile of the given node.
     */
    [[nodiscard]] tile get_node_tile(const mockturtle::node<Ntk>& n) const noexcept
    {
        return strg->node_to_tile[n];
    }
    /**
     * Returns the node at a given tile.
     *
     * @param t The tile whose node is to be returned.
     * @return The node at the given tile.
     */
    [[nodiscard]] mockturtle::node<Ntk> get_tile_node(const tile& t) const noexcept
    {
        assert(this->is_within_bounds(t) && "Tile is not within layout bounds");

        return strg->tile_index_to_node[tile_to_index(t)];
    }
    /**
     * Returns the net cost of a given node. The net cost are defined as the sum of distances to all predecessors and
     * successors in the layout.
     *
     * @param n The node whose net cost is to be returned.
     * @return The net cost of the given node.
     */
    [[nodiscard]] Dist net_cost(const mockturtle::node<Ntk>& n) const noexcept
    {
        return strg->net_costs[n];
    }
    /**
     * Returns the accumulated net cost of all placed nodes.
     *
     * @return The accumulated net cost of all placed nodes.
     */
    [[nodiscard]] Dist net_cost() const noexcept
    {
        return strg->net_costs.sum();
    }
    /**
     * Applies a given function to each node of the associated network and its tile. Therefore, the function must
     * accept two arguments: a node and a tile.
     *
     * @tparam Fn The type of the function to be applied.
     * @param fn The function to be applied.
     */
    template <typename Fn>
    void foreach_placed_node(Fn&& fn) const
    {
        strg->ntk.foreach_node(
            [this, &fn](auto const& n)
            {
                if (strg->ntk.is_constant(n))
                {
                    return;
                }

                if (const auto t = strg->node_to_tile[n]; !t.is_dead())
                {
                    assert(t == strg->node_to_tile[n] && "node placement data is out of sync");

                    std::invoke(std::forward<Fn>(fn), n, t);
                }
            });
    }
    /**
     * Applies the current placement to the underlying layout type and returns that layout.
     *
     * Since the `placement_layout` data type is build to swap gates and determine their costs as efficiently as
     * possible, it does neglects some functionality of the underlying layout type. This function should be called after
     * a sufficient placement has been determined.
     *
     * @return The underlying layout type with the current placement applied.
     */
    GateLyt apply_placement() noexcept
    {
        GateLyt lyt{{this->x(), this->y()}, this->get_clocking_scheme(), this->get_layout_name()};

        // place PIs
        strg->ntk.foreach_pi(
            [&lyt, this](auto const& pi)
            {
                const auto t = strg->node_to_tile[pi];
                assert(this->is_within_bounds(t) && "Tile is not within layout bounds");

                fiction::place(lyt, t, strg->ntk, pi);
            });

        // place gates
        strg->ntk.foreach_gate(
            [&lyt, this, po_counter = 0u](auto const& n) mutable noexcept
            {
                const auto t = strg->node_to_tile[n];
                assert(this->is_within_bounds(t) && "Tile is not within layout bounds");

                // 1-input gates
                if (strg->ntk.fanin_size(n) == 1)
                {
                    if (strg->ntk.is_po(n))  // special case for primary outputs
                    {
                        lyt.create_po(mockturtle::signal<GateLyt>{},
                                      strg->ntk.has_output_name(po_counter) ? strg->ntk.get_output_name(po_counter++) :
                                                                              fmt::format("po{}", po_counter++),
                                      t);
                    }
                    else
                    {
                        fiction::place(lyt, t, strg->ntk, n, mockturtle::signal<GateLyt>{});
                    }
                }
                // 2-input gates
                else if (strg->ntk.fanin_size(n) == 2)
                {
                    fiction::place(lyt, t, strg->ntk, n, mockturtle::signal<GateLyt>{}, mockturtle::signal<GateLyt>{});
                }
                // 3-input gates
                else if (strg->ntk.fanin_size(n) == 3)
                {
                    fiction::place(lyt, t, strg->ntk, n, mockturtle::signal<GateLyt>{}, mockturtle::signal<GateLyt>{},
                                   mockturtle::signal<GateLyt>{});
                }
                else
                {
                    assert(false && "unsupported fanin size");
                }
            });

        restore_names(strg->ntk, lyt);

        return lyt;
    }

    /**
     * Initializes the net costs of all placed nodes.
     */
    void initialize_net_costs() noexcept
    {
        this->foreach_placed_node([this](const auto& n, const auto&)
                                  { strg->net_costs[strg->ntk.node_to_index(n)] = compute_net_cost(n); });
    }

  private:
    /**
     * The storage of the placement layout.
     */
    storage strg;

    /**
     * Converts a tile to an index.
     *
     * @param t The tile to be converted.
     * @return The index of the given tile.
     */
    [[nodiscard]] std::size_t tile_to_index(const tile& t) const noexcept
    {
        assert(this->is_within_bounds(t) && "Tile is not within layout bounds");

        return t.x + t.y * (this->x() + 1);
    }
    /**
     * Recomputes the net cost of the given node.
     *
     * @param n The node whose net cost is to be recomputed.
     * @return The net cost of the given node.
     */
    [[nodiscard]] Dist compute_net_cost(const mockturtle::node<Ntk>& n) const noexcept
    {
        Dist cost{0};

        if (strg->ntk.is_constant(n))
        {
            return cost;
        }

        const auto n_t = strg->node_to_tile[n];

        strg->ntk.foreach_fanin(n, [this, &cost, &n_t](auto const& fi)
                                { cost += strg->dist_fn(*this, strg->node_to_tile[strg->ntk.get_node(fi)], n_t); });

        strg->ntk.foreach_fanout(n, [this, &cost, &n_t](auto const& fon)
                                 { cost += strg->dist_fn(*this, n_t, strg->node_to_tile[fon]); });

        return cost;
    }
    /**
     * Updates the net costs depending on node `n`. This function assumes `n` has been moved. Thereby, not only its net
     * cost are updated, but also the net costs of all nodes that are connected to `n`.
     *
     * @param n The node whose net cost is to be updated.
     */
    void update_net_costs(const mockturtle::node<Ntk>& n) const noexcept
    {
        Dist cost{0};

        if (strg->ntk.is_constant(n))
        {
            return;
        }

        const auto n_t = strg->node_to_tile[n];

        strg->ntk.foreach_fanin(n,
                                [this, &cost, &n_t](auto const& fi)
                                {
                                    const auto fin = strg->ntk.get_node(fi);

                                    strg->net_costs[strg->ntk.node_to_index(fin)] = compute_net_cost(fin);

                                    cost += strg->dist_fn(*this, strg->node_to_tile[fin], n_t);
                                });

        strg->ntk.foreach_fanout(n,
                                 [this, &cost, &n_t](auto const& fon)
                                 {
                                     strg->net_costs[strg->ntk.node_to_index(fon)] = compute_net_cost(fon);

                                     cost += strg->dist_fn(*this, n_t, strg->node_to_tile[fon]);
                                 });

        strg->net_costs[strg->ntk.node_to_index(n)] = cost;
    }
};

template <class T, class U>
placement_layout(const T&, const U&) -> placement_layout<T, U>;

}  // namespace fiction

#endif  // FICTION_PLACEMENT_LAYOUT_HPP
