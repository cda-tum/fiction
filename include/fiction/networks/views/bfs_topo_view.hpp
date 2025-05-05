//
// Created by benjamin on 08.04.24.
//

#ifndef FICTION_BFS_TOPO_VIEW_HPP
#define FICTION_BFS_TOPO_VIEW_HPP

#include <mockturtle/networks/detail/foreach.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/immutable_view.hpp>

#include <cassert>
#include <cstdint>
#include <queue>
#include <unordered_map>
#include <vector>

namespace fiction
{

/**
 * Computes a breadth-first topological order for all nodes reachable from the outputs of a logic network.
 *
 * Overrides the methods `foreach_node`, `foreach_gate`, `size`, and `num_gates`.
 * The topological order is computed once upon construction and remains fixed for the lifetime of the view.
 *
 * All constant nodes and primary inputs are included in the traversal, even if they are not reachable from the outputs.
 * Constant nodes precede primary inputs, and primary inputs are ordered according to their creation order.
 *
 * Modifications to the network are not allowed after construction, as the order is immutable.
 * Only reachable nodes are included in `foreach_node` and `foreach_gate`, which may exclude some network nodes.
 *
 * Required network functions:
 * - `get_constant`
 * - `foreach_pi`
 * - `foreach_fanin`
 * - `incr_trav_id`
 * - `set_visited`
 * - `trav_id`
 * - `visited`
 *
 * @tparam Ntk Logic network type.
 * @tparam sorted Internal tag to indicate if the network is already topologically sorted (default: false).
 */
template <class Ntk, bool sorted = mockturtle::is_topologically_sorted_v<Ntk>>
class bfs_topo_view
{};

/**
 * Specialization for non-topologically sorted networks.
 *
 * @tparam Ntk Logic network type.
 */
template <typename Ntk>
class bfs_topo_view<Ntk, false> : public mockturtle::immutable_view<Ntk>
{
  public:
    using storage                                 = typename Ntk::storage;
    using node                                    = typename Ntk::node;
    using signal                                  = typename Ntk::signal;
    static constexpr bool is_topologically_sorted = true;

    /**
     * Constructs a breadth-first topological view from a given network.
     *
     * @param ntk Logic network to be wrapped with a topological view.
     */
    explicit bfs_topo_view(Ntk const& ntk) : mockturtle::immutable_view<Ntk>(ntk), fanout_ntk(ntk)
    {
        static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
        static_assert(mockturtle::has_size_v<Ntk>, "Ntk does not implement the size method");
        static_assert(mockturtle::has_get_constant_v<Ntk>, "Ntk does not implement the get_constant method");
        static_assert(mockturtle::has_foreach_pi_v<Ntk>, "Ntk does not implement the foreach_pi method");
        static_assert(mockturtle::has_foreach_fanin_v<Ntk>, "Ntk does not implement the foreach_fanin method");
        static_assert(mockturtle::has_incr_trav_id_v<Ntk>, "Ntk does not implement the incr_trav_id method");
        static_assert(mockturtle::has_set_visited_v<Ntk>, "Ntk does not implement the set_visited method");
        static_assert(mockturtle::has_trav_id_v<Ntk>, "Ntk does not implement the trav_id method");
        static_assert(mockturtle::has_visited_v<Ntk>, "Ntk does not implement the visited method");

        update_bfs_topo();
    }

    /**
     * Returns the number of nodes in the topological view.
     *
     * @return Total number of visited nodes in the topological order.
     */
    auto size() const
    {
        return static_cast<uint32_t>(topo_order.size());
    }

    /**
     * Returns the number of gates in the topological view.
     *
     * @return Number of gates excluding constants and primary inputs.
     */
    auto num_gates() const
    {
        uint32_t const offset = 1u + this->num_pis() +
                                (this->get_node(this->get_constant(true)) != this->get_node(this->get_constant(false)));
        return static_cast<uint32_t>(topo_order.size() - offset);
    }

    /**
     * Returns the topological index of a node.
     *
     * @param n Node whose index is to be determined.
     * @return Index of the node in the topological order.
     */
    uint32_t node_to_index(node const& n) const
    {
        return std::distance(std::begin(topo_order), std::find(std::begin(topo_order), std::end(topo_order), n));
    }

    /**
     * Returns the node at a given topological index.
     *
     * @param index Index into the topological order.
     * @return Node at the specified index.
     */
    node index_to_node(uint32_t index) const
    {
        return topo_order.at(index);
    }

    /**
     * Iterates over all nodes in topological order.
     *
     * @tparam Fn Callable function or lambda to be applied to each node.
     * @param fn Function to apply to each node.
     */
    template <typename Fn>
    void foreach_node(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(topo_order.begin(), topo_order.end(), fn);
    }

    /**
     * Iterates over all nodes in reverse topological order.
     *
     * @tparam Fn Callable function or lambda to be applied to each node.
     * @param fn Function to apply to each node.
     */
    template <typename Fn>
    void foreach_node_reverse(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(topo_order.rbegin(), topo_order.rend(), fn);
    }

    /**
     * Iterates over all gates (excluding constants and primary inputs) in topological order.
     *
     * @tparam Fn Callable function or lambda to be applied to each gate.
     * @param fn Function to apply to each gate.
     */
    template <typename Fn>
    void foreach_gate(Fn&& fn) const
    {
        uint32_t const offset = 1u + this->num_pis() +
                                (this->get_node(this->get_constant(true)) != this->get_node(this->get_constant(false)));
        mockturtle::detail::foreach_element(topo_order.begin() + offset, topo_order.end(), fn);
    }

    /**
     * Iterates over all gates (excluding constants and primary inputs) in reverse topological order.
     *
     * @tparam Fn Callable function or lambda to be applied to each gate.
     * @param fn Function to apply to each gate.
     */
    template <typename Fn>
    void foreach_gate_reverse(Fn&& fn) const
    {
        uint32_t const offset = 1u + this->num_pis() +
                                (this->get_node(this->get_constant(true)) != this->get_node(this->get_constant(false)));
        mockturtle::detail::foreach_element(topo_order.rbegin(), topo_order.rend() - offset, fn);
    }

    /**
     * Updates the breadth-first topological order of the network.
     * Called once during construction.
     */
    void update_bfs_topo()
    {
        fanout_ntk.incr_trav_id();
        topo_order.clear();
        topo_order.reserve(fanout_ntk.size());

        std::queue<node>                   q;
        std::unordered_map<node, uint32_t> unvisited_fanin_count;

        const auto c0 = fanout_ntk.get_node(fanout_ntk.get_constant(false));
        q.push(c0);
        fanout_ntk.set_visited(c0, fanout_ntk.trav_id());
        topo_order.push_back(c0);

        const auto c1 = fanout_ntk.get_node(fanout_ntk.get_constant(true));
        if (fanout_ntk.visited(c1) != fanout_ntk.trav_id())
        {
            q.push(c1);
            fanout_ntk.set_visited(c1, fanout_ntk.trav_id());
            topo_order.push_back(c1);
        }

        fanout_ntk.foreach_node(
            [&](auto n)
            {
                unvisited_fanin_count[n] = 0;
                fanout_ntk.foreach_fanin(n, [&](auto const&  /*f*/) { ++unvisited_fanin_count[n]; });
            });

        fanout_ntk.foreach_ci(
            [&](auto n)
            {
                if (fanout_ntk.visited(n) != fanout_ntk.trav_id())
                {
                    fanout_ntk.set_visited(n, fanout_ntk.trav_id());
                    topo_order.push_back(n);
                    q.push(n);
                }
            });

        while (!q.empty())
        {
            const auto n = q.front();
            q.pop();

            fanout_ntk.foreach_fanout(n,
                                      [&](auto const& child)
                                      {
                                          if (fanout_ntk.visited(child) == fanout_ntk.trav_id())
                                          {
                                              return;
                                          }

                                          if (--unvisited_fanin_count[child] == 0)
                                          {
                                              fanout_ntk.set_visited(child, fanout_ntk.trav_id());
                                              topo_order.push_back(child);
                                              q.push(child);
                                          }
                                      });
        }
    }

  private:
    /**
     * Stores nodes in topological order from constants and PIs to reachable gates.
     */
    std::vector<node> topo_order;

    /**
     * Fanout-augmented view of the original network for efficient traversal.
     */
    mockturtle::fanout_view<Ntk> fanout_ntk;
};

/**
 * Specialization for already topologically sorted networks.
 * Simply forwards the underlying network without reordering.
 *
 * @tparam Ntk Logic network type.
 */
template <typename Ntk>
class bfs_topo_view<Ntk, true> : public Ntk
{
  public:
    /**
     * Constructs a topological view from a network that is already sorted.
     *
     * @param ntk Topologically sorted network.
     */
    explicit bfs_topo_view(Ntk const& ntk) : Ntk(ntk) {}
};

/**
 * Deduction guide for bfs_topo_view.
 *
 * @tparam T Logic network type.
 */
template <class T>
bfs_topo_view(T const&) -> bfs_topo_view<T>;

/**
 * Deduction guide for bfs_topo_view from a network and signal.
 *
 * @tparam T Logic network type.
 */
template <class T>
bfs_topo_view(T const&, typename T::signal const&) -> bfs_topo_view<T>;

}  // namespace fiction

#endif  // FICTION_BFS_TOPO_VIEW_HPP
