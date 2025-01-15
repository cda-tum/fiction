//
// Created by benjamin on 13.12.24.
//

#ifndef FICTION_DEPTH_VIEW_HPP
#define FICTION_DEPTH_VIEW_HPP

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/cost_functions.hpp>

#include <cassert>
#include <cstdint>

#include <parallel_hashmap/phmap.h>

namespace fiction
{

/**
 * Parameters for depth view.
 */
struct depth_view_params
{
    /**
     * Take complemented edges into account for depth computation.
     */
    bool count_complements{false};
    /**
     * Whether PIs have costs.
     */
    bool pi_cost{false};
};

/**
 * Provides `depth` and `level` methods for networks, similar to `mockturtle::depth_view()`.
 * Unlike the `mockturtle` implementation, this version uses flat hashmaps to store `level` data
 * instead of `mockturtle::node_map`. Additionally, the `on_add` functionality has been removed.
 * As a result, if the underlying network changes, the `update_levels` method must be called to
 * refresh the `depth_view` information. These modifications address performance issues encountered
 * with `mockturtle::depth_view`.
 *
 * This view computes the level of each node and the overall depth of the network. The `level` and
 * `depth` methods adhere to the network interface. Levels are calculated during construction and
 * can be updated later by invoking the `update_levels` method.
 *
 * @tparam Ntk The network type.
 * @tparam NodeCostFn Optional cost function applied to nodes. Default is `mockturtle::unit_cost<Ntk>`.
 * @tparam has_depth_interface Boolean flag checked compile-time, determines if the provided Ntk supports the depth
 * interface.
 */
template <typename Ntk, typename NodeCostFn = mockturtle::unit_cost<Ntk>,
          bool has_depth_interface =
              mockturtle::has_depth_v<Ntk> && mockturtle::has_level_v<Ntk> && mockturtle::has_update_levels_v<Ntk>>
class depth_view
{};

/**
 * A specialization of `depth_view` for networks where `has_depth_interface` is `true`.
 * When this condition is met, constructing a new depth view is unnecessary.
 * @tparam Ntk The type of the network.
 * @tparam NodeCostFn A function to compute the costs associated with nodes.
 */
template <typename Ntk, typename NodeCostFn>
class depth_view<Ntk, NodeCostFn, true> : public Ntk
{
  public:
    explicit depth_view(Ntk const& ntk, depth_view_params const& ps = {}) : Ntk(ntk)
    {
        (void)ps;
    }
};

/**
 * A specialization of `depth_view` for networks where `has_depth_interface` is `false`.
 * When this condition is met, a depth view is constructed.
 * @tparam Ntk The type of the network.
 * @tparam NodeCostFn A function to compute the costs associated with nodes.
 */
template <typename Ntk, typename NodeCostFn>
class depth_view<Ntk, NodeCostFn, false> : public Ntk
{
  public:
    using storage = typename Ntk::storage;
    using node    = typename Ntk::node;
    using signal  = typename Ntk::signal;

    /**
     * Default constructor for `fiction::depth_view`.
     * Initializes an empty `fiction::depth_view` object, sets up base class properties,
     * and ensures that the network type (Ntk) satisfies required interface methods.
     *
     * @param cost_fn Optional cost function to compute node costs.
     * @param ps Optional parameters for depth view construction
     */
    explicit depth_view(NodeCostFn const& cost_fn = {}, depth_view_params const& ps = {}) :
            Ntk(),
            _ps(ps),
            _cost_fn(cost_fn)
    {
        static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
        static_assert(mockturtle::has_size_v<Ntk>, "Ntk does not implement the size method");
        static_assert(mockturtle::has_get_node_v<Ntk>, "Ntk does not implement the get_node method");
        static_assert(mockturtle::has_is_complemented_v<Ntk>, "Ntk does not implement the is_complemented method");
        static_assert(mockturtle::has_visited_v<Ntk>, "Ntk does not implement the visited method");
        static_assert(mockturtle::has_set_visited_v<Ntk>, "Ntk does not implement the set_visited method");
        static_assert(mockturtle::has_foreach_po_v<Ntk>, "Ntk does not implement the foreach_po method");
        static_assert(mockturtle::has_foreach_fanin_v<Ntk>, "Ntk does not implement the foreach_fanin method");

        _levels.rehash(this->size());
    }

    /**
     * Constructs an `fiction::depth_view` from an existing network.
     * Invokes the base class constructor with the provided network, initializes class-specific members,
     * and registers necessary network events. Ensures that the network type (Ntk) satisfies required
     * interface methods.
     *
     * @param ntk The network on which to construct the depth view.
     * @param cost_fn Optional function to compute node costs.
     * @param ps Optional parameters for depth view construction.
     */
    explicit depth_view(Ntk const& ntk, NodeCostFn const& cost_fn = {}, depth_view_params const& ps = {}) :
            Ntk(ntk),
            _ps(ps),
            _cost_fn(cost_fn)
    {
        static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
        static_assert(mockturtle::has_size_v<Ntk>, "Ntk does not implement the size method");
        static_assert(mockturtle::has_get_node_v<Ntk>, "Ntk does not implement the get_node method");
        static_assert(mockturtle::has_is_complemented_v<Ntk>, "Ntk does not implement the is_complemented method");
        static_assert(mockturtle::has_visited_v<Ntk>, "Ntk does not implement the visited method");
        static_assert(mockturtle::has_set_visited_v<Ntk>, "Ntk does not implement the set_visited method");
        static_assert(mockturtle::has_foreach_po_v<Ntk>, "Ntk does not implement the foreach_po method");
        static_assert(mockturtle::has_foreach_fanin_v<Ntk>, "Ntk does not implement the foreach_fanin method");

        _levels.rehash(this->size());
        update_levels();
    }

    /**
     * Copy constructor creates a new `fiction::depth_view` by copying the content of another `fiction::depth_view`.
     *
     * @param other The other `fiction::depth_view` object to be copied.
     */
    depth_view(depth_view<Ntk, NodeCostFn, false> const& other) :
            Ntk(other),
            _ps(other._ps),
            _levels(other._levels),
            _crit_path(other._crit_path),
            _depth(other._depth),
            _cost_fn(other._cost_fn)
    {}

    /**
     * Overloaded assignment operator for copying `fiction::depth_view` content of another `fiction::depth_view` object.
     *
     * @param other The source `fiction::depth_view` object whose contents are being copied.
     * @return A reference to the current object, enabling chain assignments.
     */
    depth_view<Ntk, NodeCostFn, false>& operator=(depth_view<Ntk, NodeCostFn, false> const& other)
    {
        // Check for self-assignment
        if (this == &other)
        {
            return *this;
        }

        // update the base class
        this->_storage = other._storage;
        this->_events  = other._events;

        // update the virtual storage
        if constexpr (has_is_virtual_pi_v<Ntk>)
        {
            this->v_storage = other.v_storage;
        }

        // copy
        _ps        = other._ps;
        _levels    = other._levels;
        _crit_path = other._crit_path;
        _depth     = other._depth;
        _cost_fn   = other._cost_fn;

        // Return the current object
        return *this;
    }

    /**
     * Destructor for `fiction::depth_view`.
     */
    ~depth_view() = default;

    /**
     * @return The depth of the network.
     */
    [[nodiscard]] uint32_t depth() const
    {
        return _depth;
    }

    /**
     * @return The level of a node.
     */
    uint32_t level(node const& n) const
    {
        return _levels.at(n);
    }

    /**
     * @return If a node is on the critical path.
     */
    bool is_on_critical_path(node const& n) const
    {
        return _crit_path.contains(n);
    }

    /**
     * Set the level of a node.
     */
    void set_level(node const& n, uint32_t level)
    {
        _levels[n] = level;
    }

    /**
     * Set the depth of the network.
     */
    void set_depth(uint32_t level)
    {
        _depth = level;
    }

    /**
     * Update the levels and the critical path of the network.
     */
    void update_levels()
    {
        _levels.clear();
        _levels.rehash(this->size());
        _crit_path.clear();
        _crit_path.rehash(this->size());

        this->incr_trav_id();
        compute_levels();
    }

    /*
     * Add a node to the `depth_view` and update its level.
     *
     * @param n Node added to the `depth_view`.
     */
    void on_add(node const& n)
    {
        _levels.rehash(this->size());

        uint32_t level{0};
        this->foreach_fanin(n,
                            [&](auto const& f)
                            {
                                auto clevel = _levels[f];
                                if (_ps.count_complements && this->is_complemented(f))
                                {
                                    clevel++;
                                }
                                level = std::max(level, clevel);
                            });
        if (this->is_pi(n))
        {
            _levels[n] = level;
        }
        else
        {
            _levels[n] = level + _cost_fn(*this, n);
        }
    }

  private:
    /**
     * Compute the level of a nodes.
     */
    uint32_t compute_levels(node const& n)
    {
        if (this->visited(n) == this->trav_id())
        {
            return _levels[n];
        }
        this->set_visited(n, this->trav_id());

        if (this->is_constant(n))
        {
            return _levels[n] = 0;
        }
        if (this->is_ci(n))
        {
            assert(!_ps.pi_cost || _cost_fn(*this, n) >= 1);
            return _levels[n] = _ps.pi_cost ? _cost_fn(*this, n) - 1 : 0;
        }

        uint32_t level{0};
        this->foreach_fanin(n,
                            [&](auto const& f)
                            {
                                auto clevel = compute_levels(this->get_node(f));
                                if (_ps.count_complements && this->is_complemented(f))
                                {
                                    clevel++;
                                }
                                level = std::max(level, clevel);
                            });

        return _levels[n] = level + _cost_fn(*this, n);
    }

    /**
     * Compute the levels and depth of the network.
     */
    void compute_levels()
    {
        _depth = 0;
        this->foreach_po(
            [&](auto const& f)
            {
                auto clevel = compute_levels(this->get_node(f));
                if (_ps.count_complements && this->is_complemented(f))
                {
                    clevel++;
                }
                _depth = std::max(_depth, clevel);
            });

        if constexpr (mockturtle::has_foreach_ri_v<Ntk>)
        {
            this->foreach_ri(
                [&](auto const& f)
                {
                    auto clevel = compute_levels(this->get_node(f));
                    if (_ps.count_complements && this->is_complemented(f))
                    {
                        clevel++;
                    }
                    _depth = std::max(_depth, clevel);
                });
        }

        this->foreach_po(
            [&](auto const& f)
            {
                const auto n = this->get_node(f);
                if (_levels[n] == _depth)
                {
                    set_critical_path(n);
                }
            });

        if constexpr (mockturtle::has_foreach_ri_v<Ntk>)
        {
            this->foreach_ri(
                [&](auto const& f)
                {
                    const auto n = this->get_node(f);
                    if (_levels[n] == _depth)
                    {
                        set_critical_path(n);
                    }
                });
        }
    }

    /**
     * Set nodes on the critical path.
     */
    void set_critical_path(node const& n)
    {
        _crit_path[n] = true;
        if (!this->is_constant(n) && !(_ps.pi_cost && this->is_pi(n)))
        {
            const auto lvl = _levels[n];
            this->foreach_fanin(n,
                                [&](auto const& f)
                                {
                                    const auto cn     = this->get_node(f);
                                    auto       offset = _cost_fn(*this, n);
                                    if (_ps.count_complements && this->is_complemented(f))
                                    {
                                        offset++;
                                    }
                                    if (_levels[cn] + offset == lvl && !_crit_path[cn])
                                    {
                                        set_critical_path(cn);
                                    }
                                });
        }
    }

    /**
     * Depth view parameters.
     */
    depth_view_params _ps;
    /**
     * Hashmap assigning levels to nodes.
     */
    phmap::flat_hash_map<node, uint32_t> _levels;
    /**
     * Hashmap assigning nodes on the critical path.
     */
    phmap::flat_hash_map<node, uint32_t> _crit_path;
    /**
     * The depth of the network.
     */
    uint32_t _depth{};
    /**
     * The cost function.
     */
    NodeCostFn _cost_fn;
};

/**
 * Deduction guide for `fiction::depth_view'.
 *
 * @tparam T Network type deduced from the construction context of `fiction::depth_view`.
 */
template <typename T>
depth_view(T const&) -> depth_view<T>;

/**
 * Deduction guide for `fiction::depth_view` with two constructor arguments
 *
 * @tparam T Network type deduced from the construction context of `fiction::depth_view`.
 */
template <typename T, typename NodeCostFn = mockturtle::unit_cost<T>>
depth_view(T const&, NodeCostFn const&, depth_view_params const&) -> depth_view<T, NodeCostFn>;

}  // namespace fiction

#endif  // FICTION_DEPTH_VIEW_HPP
