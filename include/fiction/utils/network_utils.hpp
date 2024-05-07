//
// Created by marcel on 13.10.21.
//

#ifndef FICTION_NETWORK_UTILS_HPP
#define FICTION_NETWORK_UTILS_HPP

#include "fiction/utils/hash.hpp"

#include <mockturtle/traits.hpp>

#include <algorithm>
#include <cstdint>
#include <functional>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace mockturtle
{
/**
 * Representation of an edge between two network nodes.
 *
 * @tparam Ntk `mockturtle` network type.
 */
template <typename Ntk>
struct edge
{
    const node<Ntk> source, target;
    /**
     * Equality operator.
     *
     * @param other Edge to compare to.
     * @return `true` iff both sources and targets match.
     */
    bool operator==(const edge<Ntk>& other) const
    {
        return source == other.source && target == other.target;
    }
    /**
     * Inequality operator.
     *
     * @param other Edge to compare to.
     * @return `true` iff this edge is not equal to other.
     */
    bool operator!=(const edge<Ntk>& other) const
    {
        return !(*this == other);
    }
};
}  // namespace mockturtle

namespace std
{
/**
 * Provides a hash implementation for `mockturtle::edge<Ntk>`.
 *
 * @tparam Ntk Network type of edge.
 */
template <typename Ntk>
struct hash<mockturtle::edge<Ntk>>
{
    std::size_t operator()(const mockturtle::edge<Ntk>& e) const noexcept
    {
        std::size_t h = 0;
        fiction::hash_combine(h, e.source, e.target);

        return h;
    }
};
}  // namespace std

namespace fiction
{
/**
 * Applies a function to all edges in a `mockturtle` network.
 *
 * @tparam Ntk `mockturtle` network type.
 * @tparam Fn Unary function type that takes a `mockturtle::edge<Ntk>` object as parameter.
 * @param ntk Network to iterate over.
 * @param fn Function object to apply to each edge in `ntk`.
 */
template <typename Ntk, typename Fn>
void foreach_edge(const Ntk& ntk, Fn&& fn)
{
    static_assert(mockturtle::has_foreach_node_v<Ntk>, "Ntk does not implement the foreach_node function.");
    static_assert(mockturtle::has_foreach_fanin_v<Ntk>, "Ntk does not implement the foreach_fanin function.");
    static_assert(mockturtle::has_get_node_v<Ntk>, "Ntk does not implement the get_node function.");

    ntk.foreach_node(
        [&ntk, &fn](const mockturtle::node<Ntk>& n)
        {
            ntk.foreach_fanin(n,
                              [&ntk, &fn, &n](const mockturtle::signal<Ntk>& fi)
                              {
                                  mockturtle::edge<Ntk> e{ntk.get_node(fi), n};

                                  fn(e);
                              });
        });
}
/**
 * Applies a function to all outgoing edges in a `mockturtle` network.
 *
 * @tparam Ntk `mockturtle` network type.
 * @tparam Fn Unary function type that takes a `mockturtle::edge<Ntk>` object as parameter.
 * @param ntk Network to iterate over.
 * @param n Node of `ntk` whose outgoing edges are to be considered.
 * @param fn Function object to apply to each outgoing edge of `n` in `ntk`.
 */
template <typename Ntk, typename Fn>
void foreach_outgoing_edge(const Ntk& ntk, const mockturtle::node<Ntk>& n, Fn&& fn)
{
    static_assert(mockturtle::has_foreach_fanout_v<Ntk>, "Ntk does not implement the foreach_fanout function.");

    ntk.foreach_fanout(n,
                       [&fn, &n](const mockturtle::node<Ntk>& fon)
                       {
                           mockturtle::edge<Ntk> e{n, fon};

                           fn(e);
                       });
}
/**
 * Applies a function to all incoming edges in a `mockturtle` network.
 *
 * @tparam Ntk `mockturtle` network type.
 * @tparam Fn Unary function type that takes a `mockturtle::edge<Ntk>` object as parameter.
 * @param ntk Network to iterate over.
 * @param n Node of `ntk` whose incoming edges are to be considered.
 * @param fn Function object to apply to each incoming edge of `n` in `ntk`.
 */
template <typename Ntk, typename Fn>
void foreach_incoming_edge(const Ntk& ntk, const mockturtle::node<Ntk>& n, Fn&& fn)
{
    static_assert(mockturtle::has_foreach_fanin_v<Ntk>, "Ntk does not implement the foreach_fanin function.");
    static_assert(mockturtle::has_get_node_v<Ntk>, "Ntk does not implement the get_node function.");

    ntk.foreach_fanin(n,
                      [&ntk, &fn, &n](const mockturtle::signal<Ntk>& fi)
                      {
                          mockturtle::edge<Ntk> e{ntk.get_node(fi), n};

                          fn(e);
                      });
}
/**
 * Returns a vector of all fanout nodes of some given network node.
 *
 * @tparam Ntk `mockturtle` network type.
 * @param ntk Network in which the fanouts are to be gathered.
 * @param n Node whose fanouts are desired.
 * @return A vector of all outgoing nodes directly adjacent to `n` in `ntk`.
 */
template <typename Ntk>
std::vector<mockturtle::node<Ntk>> fanouts(const Ntk& ntk, const mockturtle::node<Ntk>& n) noexcept
{
    static_assert(mockturtle::has_foreach_fanout_v<Ntk>, "Ntk does not implement the foreach_fanout function.");

    std::vector<mockturtle::node<Ntk>> fos{};

    ntk.foreach_fanout(n, [&fos](const auto& fon) { fos.push_back(fon); });

    return fos;
}
/**
 * Container that stores fanins of a node in a network, including whether one of them is a constant.
 *
 * Note that this container assumes that each node has a maximum of one constant fanin.
 *
 * @tparam Ntk `mockturtle` network type.
 */
template <typename Ntk>
struct fanin_container
{
    /**
     * A vector of all fanin nodes except for constants.
     */
    std::vector<mockturtle::node<Ntk>> fanin_nodes{};
    /**
     * Has a value if a fanin node is constant. In that case, it represents the constant value.
     */
    std::optional<bool> constant_fanin{std::nullopt};
};
/**
 * Returns a fanin container filled with all fanin nodes of some given network node.
 *
 * Note that this function assumes that each node has a maximum of one constant fanin.
 *
 * @tparam Ntk `mockturtle` network type.
 * @param ntk Network in which the fanins are to be gathered.
 * @param n Node whose fanins are desired.
 * @return A container of all incoming nodes directly adjacent to `n` in `ntk`.
 */
template <typename Ntk>
fanin_container<Ntk> fanins(const Ntk& ntk, const mockturtle::node<Ntk>& n) noexcept
{
    static_assert(mockturtle::has_foreach_fanin_v<Ntk>, "Ntk does not implement the foreach_fanin function.");
    static_assert(mockturtle::has_get_node_v<Ntk>, "Ntk does not implement the get_node function.");
    static_assert(mockturtle::has_is_constant_v<Ntk>, "Ntk does not implement the is_constant function.");

    fanin_container<Ntk> fc{};

    ntk.foreach_fanin(n,
                      [&ntk, &fc](const auto& fi)
                      {
                          if (const auto fin = ntk.get_node(fi); ntk.is_constant(fin))
                          {
                              assert(!fc.constant_fanin.has_value());  // there can only be one constant input
                              fc.constant_fanin = ntk.constant_value(fin);
                          }
                          else
                          {
                              fc.fanin_nodes.push_back(fin);
                          }
                      });

    return fc;
}
/**
 * Computes the number of constant fanin nodes of some network node `n`.
 *
 * @tparam Ntk `mockturtle` network type.
 * @param ntk Network in which the constant fanins are to be counted.
 * @param n Node whose constant fanins are to be counted.
 * @return Number of constant fanins to `n` in `ntk`.
 */
template <typename Ntk>
uint32_t num_constant_fanins(const Ntk& ntk, const mockturtle::node<Ntk>& n) noexcept
{
    static_assert(mockturtle::has_foreach_fanin_v<Ntk>, "Ntk does not implement the foreach_fanin function.");
    static_assert(mockturtle::has_get_node_v<Ntk>, "Ntk does not implement the get_node function.");
    static_assert(mockturtle::has_is_constant_v<Ntk>, "Ntk does not implement the is_constant function.");

    uint32_t num_const_fi{0};

    ntk.foreach_fanin(n,
                      [&ntk, &num_const_fi](const auto& fi)
                      {
                          if (ntk.is_constant(ntk.get_node(fi)))
                          {
                              ++num_const_fi;
                          }
                      });

    return num_const_fi;
}
/**
 * Exception class that can be thrown if some network exceeds a legal number of fanins.
 */
class high_degree_fanin_exception : public std::invalid_argument
{
  public:
    high_degree_fanin_exception() : std::invalid_argument("network contains nodes that exceed the supported fanin size")
    {}
};
/**
 * Checks if a given network exceeds a given fanin threshold in any of its nodes.
 *
 * @tparam Ntk `mockturtle` network type.
 * @param ntk Network to check.
 * @param threshold Maximum number of legal fanins.
 * @return `true` iff any node in `ntk` exceeds `threshold` fanins.
 */
template <typename Ntk>
bool has_high_degree_fanin_nodes(const Ntk& ntk, const uint32_t threshold = 2) noexcept
{
    static_assert(mockturtle::has_foreach_node_v<Ntk>, "Ntk does not implement the foreach_node function.");
    static_assert(mockturtle::has_is_constant_v<Ntk>, "Ntk does not implement the is_constant function.");
    static_assert(mockturtle::has_fanin_size_v<Ntk>, "Ntk does not implement the fanin_size function.");

    bool result = false;

    ntk.foreach_node(
        [&ntk, &threshold, &result](const auto& n)
        {
            // skip constants
            if (!ntk.is_constant(n))
            {
                if ((ntk.fanin_size(n) - num_constant_fanins(ntk, n)) > threshold)
                {
                    result = true;
                }
            }

            return !result;  // keep looping if no high-degree fanin node has yet been encountered
        });

    return result;
}
/**
 * Container that stores fanin edges of a node in a network, including whether one of them is a constant.
 *
 * @tparam Ntk `mockturtle` network type.
 */
template <typename Ntk>
struct fanin_edge_container
{
    /**
     * A vector of all fanin edges excluding for constants.
     */
    std::vector<mockturtle::edge<Ntk>> fanin_edges{};
};
/**
 * Returns a fanin edge container filled with all fanin edges `(fanin, node)` of some given network node.
 *
 * @tparam Ntk `mockturtle` network type.
 * @param ntk Network in which the fanin edges are to be gathered.
 * @param n Node whose fanins are desired.
 * @return A container of all incoming edges `(fanin, n)` in `ntk`.
 */
template <typename Ntk>
fanin_edge_container<Ntk> fanin_edges(const Ntk& ntk, const mockturtle::node<Ntk>& n) noexcept
{
    static_assert(mockturtle::has_foreach_fanin_v<Ntk>, "Ntk does not implement the foreach_fanin function.");
    static_assert(mockturtle::has_get_node_v<Ntk>, "Ntk does not implement the get_node function.");
    static_assert(mockturtle::has_is_constant_v<Ntk>, "Ntk does not implement the is_constant function.");

    fanin_edge_container<Ntk> fec{};

    ntk.foreach_fanin(n,
                      [&ntk, &n, &fec](const auto& fi)
                      {
                          if (const auto fin = ntk.get_node(fi); !ntk.is_constant(fin))
                          {
                              fec.fanin_edges.push_back({fin, n});
                          }
                      });

    return fec;
}
/**
 * Checks if a given node in a given network has fanins that are primary inputs.
 *
 * @tparam Ntk `mockturtle` network type.
 * @param ntk Network to check in.
 * @param n Node to check.
 * @return `true` iff any of `n`'s fanins are primary inputs.
 */
template <typename Ntk>
bool has_incoming_primary_input(const Ntk& ntk, const mockturtle::node<Ntk>& n) noexcept
{
    static_assert(mockturtle::has_foreach_fanin_v<Ntk>, "Ntk does not implement the foreach_fanin function.");
    static_assert(mockturtle::has_get_node_v<Ntk>, "Ntk does not implement the get_node function.");
    static_assert(mockturtle::has_is_constant_v<Ntk>, "Ntk does not implement the is_constant function.");
    static_assert(mockturtle::has_is_pi_v<Ntk>, "Ntk does not implement the is_pi function.");

    bool in_pi = false;

    ntk.foreach_fanin(n,
                      [&ntk, &in_pi](const auto& fi)
                      {
                          // skip constants
                          if (const auto fin = ntk.get_node(fi); !ntk.is_constant(fin))
                          {
                              if (ntk.is_pi(fin))
                              {
                                  in_pi = true;
                              }
                          }

                          return !in_pi;  // keep looping if no PI has yet been encountered
                      });

    return in_pi;
}
/**
 * Represents a path in a network as a walk of edges.
 */
template <typename Ntk>
using edge_path = std::vector<mockturtle::edge<Ntk>>;
/**
 * Returns a vector of all possible paths to reach the given node from the primary inputs within the given network.
 *
 * Each node without predecessors is considered a terminal and a path is defined as a sequence of edges.
 *
 * @param n Node whose paths are desired.
 * @return A vector of all possible edge paths leading from terminals to `v`.
 */
template <typename Ntk>
std::vector<edge_path<Ntk>> all_incoming_edge_paths(const Ntk& ntk, const mockturtle::node<Ntk>& n) noexcept
{
    static_assert(mockturtle::has_is_constant_v<Ntk>, "Ntk does not implement the is_constant function.");
    static_assert(mockturtle::has_fanin_size_v<Ntk>, "Ntk does not implement the fanin_size function.");

    if (ntk.fanin_size(n) == 0u)
    {
        // this creates an empty path for each PI to built upon recursively; very important!
        return std::vector<edge_path<Ntk>>{edge_path<Ntk>{}};
    }

    std::vector<edge_path<Ntk>> paths{};
    foreach_incoming_edge(ntk, n,
                          [&ntk, &paths](const auto& e)
                          {
                              if (!ntk.is_constant(e.source))
                              {
                                  auto ps = all_incoming_edge_paths(ntk, e.source);

                                  for (auto& p : ps)
                                  {
                                      p.push_back(e);
                                  }

                                  paths.insert(paths.end(), std::make_move_iterator(ps.begin()),
                                               std::make_move_iterator(ps.end()));
                              }
                          });

    return paths;
}
/**
 * A clumsy implementation that returns the inverse level of each node in a given network. Its behavior is similar to
 * `mockturtle::depth_view` but starting from the primary outputs instead of the primary inputs. An implementation along
 * the lines of `inv_depth_view` would be a lot more sophisticated and desirable, but this quick hack does the job well
 * so far. If anyone wants to build an `inv_depth_view`, please be my guest.
 *
 * @tparam Ntk `mockturtle` network type.
 * @param ntk The network whose inverse levels are desired.
 * @return A vector of inverse levels for each node where `ntk.node_to_index(n)` is the position where `n`'s inverse
 * level is stored.
 */
template <typename Ntk>
std::vector<uint32_t> inverse_levels(const Ntk& ntk) noexcept
{
    static_assert(mockturtle::has_foreach_fanin_v<Ntk>, "Ntk does not implement the foreach_fanin function.");
    static_assert(mockturtle::has_get_node_v<Ntk>, "Ntk does not implement the get_node function.");
    static_assert(mockturtle::has_foreach_po_v<Ntk>, "Ntk does not implement the foreach_po function.");
    static_assert(mockturtle::has_node_to_index_v<Ntk>, "Ntk does not implement the node_to_index function.");

    // vector of all inverse levels by node index
    std::vector<uint32_t> inv_levels(ntk.size(), uint32_t{0});
    // vector to store a discovered flag to not visit nodes twice
    std::vector<bool> discovered(ntk.size(), false);

    // helper function to access a given node's inverse level
    const auto get_inv_level = [&inv_levels, &ntk](const auto& n) { return inv_levels[ntk.node_to_index(n)]; };
    // helper function to set a given node's inverse level
    const auto set_inv_level = [&inv_levels, &ntk](const auto& n, const auto& l)
    { inv_levels[ntk.node_to_index(n)] = l; };
    // helper function to check if a given node has already been discovered
    const auto is_discovered = [&discovered, &ntk](const auto& n) { return discovered[ntk.node_to_index(n)]; };
    // helper function to set a given node as discovered
    const auto set_discovered = [&discovered, &ntk](const auto& n) { discovered[ntk.node_to_index(n)] = true; };

    // inverse joint depth first search
    const std::function<void(const mockturtle::node<Ntk>& n)> inv_jdfs = [&](const auto& n)
    {
        auto fos = fanouts(ntk, n);
        // if all inverse predecessors are already discovered
        if (std::all_of(fos.cbegin(), fos.cend(), is_discovered))
        {
            set_discovered(n);

            // determine successor's maximum level
            const auto post_l = std::max_element(fos.cbegin(), fos.cend(), [&](const auto& n1, const auto& n2)
                                                 { return get_inv_level(n1) < get_inv_level(n2); });

            // if there are no successors, the level of current node is 0, else it is 1 higher than theirs
            set_inv_level(n, post_l != fos.cend() ? std::max(get_inv_level(n), get_inv_level(*post_l) + 1u) : 0u);

            ntk.foreach_fanin(n,
                              [&ntk, &is_discovered, &inv_jdfs](const auto& fi)
                              {
                                  if (const auto fin = ntk.get_node(fi); !is_discovered(fin))
                                  {
                                      inv_jdfs(fin);
                                  }
                              });
        }
    };

    // call inverse joint dfs for every PO node
    ntk.foreach_po([&ntk, &inv_jdfs](const auto& po) { inv_jdfs(ntk.get_node(po)); });

    return inv_levels;
}

}  // namespace fiction

#endif  // FICTION_NETWORK_UTILS_HPP
