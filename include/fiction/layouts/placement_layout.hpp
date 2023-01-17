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
                node_to_coordinate{network},
                random_node_functor{ntk.get_constant(false) == ntk.get_constant(true) ? 1u : 2u, ntk.size()}
        {}
        /**
         * The associated network
         */
        const Ntk& ntk;
        /**
         * Maps network nodes to their layout coordinates.
         */
        mockturtle::node_map<coordinate<GateLyt>, Ntk> node_to_coordinate;
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
     * Standard constructor for empty layouts.
     *
     * @param ntk The network whose nodes are to be placed.
     */
    explicit placement_layout(const Ntk& ntk) : GateLyt(), strg{std::make_shared<placement_layout_storage>(ntk)}
    {
        static_assert(is_gate_level_layout_v<GateLyt>, "GateLyt is not a gate level layout");
        static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");

        strg->random_x_functor = std::uniform_int_distribution<uint64_t>(0, this->x());
        strg->random_y_functor = std::uniform_int_distribution<uint64_t>(0, this->y());
    }
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

        strg->random_x_functor = std::uniform_int_distribution<uint64_t>(0, this->x());
        strg->random_y_functor = std::uniform_int_distribution<uint64_t>(0, this->y());
    }

    void resize(const typename GateLyt::aspect_ratio& ar) noexcept
    {
        GateLyt::resize(ar);

        strg->random_x_functor = std::uniform_int_distribution<uint64_t>(0, this->x());
        strg->random_y_functor = std::uniform_int_distribution<uint64_t>(0, this->y());
    }

    /**
     * Randomly places all nodes of the associated network on tiles of this layout. The function assumes that the
     * layout is empty prior to this call. It furthermore assumes that the network has at most as many nodes as the
     * layout has tiles.
     */
    void initialize_random_placement()
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

                    strg->node_to_coordinate[n] = {x, y};
                }

                ++i;
            });
    }

    tile random_tile() const
    {
        return {strg->random_x_functor(strg->generator), strg->random_x_functor(strg->generator)};
    }

    mockturtle::node<Ntk> random_node() const
    {
        return strg->ntk.index_to_node(strg->random_node_functor(strg->generator));
    }

    template <typename Fn>
    void foreach_placed_node(Fn&& fn) const
    {
        strg->ntk.foreach_node(
            [this, &fn](auto const& n)
            {
                if (!strg->ntk.is_constant(n))
                {
                    if (const auto coord = strg->node_to_coordinate[n]; !coord.is_dead())
                    {
                        std::invoke(std::forward<Fn>(fn), n, coord);
                    }
                }
            });
    }

  private:
    storage strg;
};

template <class T, class U>
placement_layout(const T&, const U&) -> placement_layout<T, U>;

}  // namespace fiction

#endif  // FICTION_PLACEMENT_LAYOUT_HPP
