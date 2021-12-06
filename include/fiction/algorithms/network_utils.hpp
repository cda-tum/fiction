//
// Created by marcel on 13.10.21.
//

#ifndef FICTION_NETWORK_UTILS_HPP
#define FICTION_NETWORK_UTILS_HPP

#include <mockturtle/traits.hpp>

#include <optional>
#include <type_traits>

namespace mockturtle
{
/**
 * Container for an edge between two network nodes.
 *
 * @tparam Ntk mockturtle network type.
 */
template <typename Ntk>
struct edge
{
    const node<Ntk> source, target;
};
}  // namespace mockturtle

namespace fiction
{
/**
 * Applies a function to all edges in a mockturtle network.
 *
 * @tparam Ntk mockturtle network type.
 * @tparam Fn Unary function type that takes a mockturtle::edge<Ntk> object as parameter.
 * @param ntk Network to iterate over.
 * @param fn Function object to apply to each edge in ntk.
 */
template <typename Ntk, typename Fn>
void foreach_edge(const Ntk& ntk, Fn&& fn)
{
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
 * Applies a function to all outgoing edges in a mockturtle network.
 *
 * @tparam Ntk mockturtle network type.
 * @tparam Fn Unary function type that takes a mockturtle::edge<Ntk> object as parameter.
 * @param ntk Network to iterate over.
 * @param n Node of ntk whose outgoing edges are to be considered.
 * @param fn Function object to apply to each outgoing edge of n in ntk.
 */
template <typename Ntk, typename Fn>
void foreach_outgoing_edge(const Ntk& ntk, const mockturtle::node<Ntk>& n, Fn&& fn)
{
    ntk.foreach_fanout(n,
                       [&ntk, &fn, &n](const mockturtle::signal<Ntk>& fo)
                       {
                           mockturtle::edge<Ntk> e{n, ntk.get_node(fo)};

                           fn(e);
                       });
}
/**
 * Applies a function to all incoming edges in a mockturtle network.
 *
 * @tparam Ntk mockturtle network type.
 * @tparam Fn Unary function type that takes a mockturtle::edge<Ntk> object as parameter.
 * @param ntk Network to iterate over.
 * @param n Node of ntk whose incoming edges are to be considered.
 * @param fn Function object to apply to each incoming edge of n in ntk.
 */
template <typename Ntk, typename Fn>
void foreach_incoming_edge(const Ntk& ntk, const mockturtle::node<Ntk>& n, Fn&& fn)
{
    ntk.foreach_fanin(n,
                      [&ntk, &fn, &n](const mockturtle::signal<Ntk>& fi)
                      {
                          mockturtle::edge<Ntk> e{ntk.get_node(fi), n};

                          fn(e);
                      });
}

template <typename Ntk>
struct fanin_container
{
    std::vector<mockturtle::node<Ntk>> fanin_nodes{};

    std::optional<bool> constant_fanin{std::nullopt};
};

template <typename Ntk>
fanin_container<Ntk> fanins(const Ntk& ntk, const mockturtle::node<Ntk>& n) noexcept
{
    fanin_container<Ntk> fc{};

    ntk.foreach_fanin(n,
                      [&ntk, &fc](const auto& f)
                      {
                          if (const auto fn = ntk.get_node(f); ntk.is_constant(fn))
                          {
                              assert(!fc.constant_fanin.has_value());  // there can only be one constant input
                              fc.constant_fanin = ntk.constant_value(fn);
                          }
                          else
                          {
                              fc.fanin_nodes.push_back(fn);
                          }
                      });

    return fc;
}

template <typename Ntk>
uint32_t num_constant_fanins(const Ntk& ntk, const mockturtle::node<Ntk>& n) noexcept
{
    uint32_t num_const_fi{0};

    ntk.foreach_fanin(n,
                      [&ntk, &num_const_fi](const auto& f)
                      {
                          if (ntk.is_constant(ntk.get_node(f)))
                          {
                              ++num_const_fi;
                          }
                      });

    return num_const_fi;
}

class high_degree_fanin_exception : public std::exception
{
  public:
    explicit high_degree_fanin_exception() : std::exception() {}

    [[nodiscard]] const char* what() const noexcept override
    {
        return "network contains nodes that exceed the supported non-constant fanin size";
    }
};

template <typename Ntk>
bool has_high_degree_fanin_nodes(const Ntk& ntk, const uint32_t threshold = 2) noexcept
{
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

template <typename Ntk>
bool has_incoming_primary_input(const Ntk& ntk, const mockturtle::node<Ntk>& n) noexcept
{
    bool in_pi = false;

    ntk.foreach_fanin(n,
                      [&ntk, &in_pi](const auto& fi)
                      {
                          // skip constants
                          if (const auto fn = ntk.get_node(fi); !ntk.is_constant(fn))
                          {
                              if (ntk.is_pi(fn))
                              {
                                  in_pi = true;
                              }
                          }

                          return !in_pi;  // keep looping if no PI has yet been encountered
                      });

    return in_pi;
}

template <typename Ntk>
using edge_path = std::vector<mockturtle::edge<Ntk>>;
/**
 * Returns a vector of all possible paths to reach the given node from the primary inputs within the given network.
 *
 * Each vertex without predecessors is considered a terminal and a path is defined as a sequence of edges.
 *
 * @param n Node whose paths are desired.
 * @return A vector of all possible edge paths leading from terminals to v.
 */
template <typename Ntk>
std::vector<edge_path<Ntk>> all_incoming_edge_paths(const Ntk& ntk, const mockturtle::node<Ntk>& n) noexcept
{
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

                                  for (auto& p : ps) { p.push_back(e); }

                                  paths.insert(paths.end(), std::make_move_iterator(ps.begin()),
                                               std::make_move_iterator(ps.end()));
                              }
                          });

    return paths;
}

}  // namespace fiction

#endif  // FICTION_NETWORK_UTILS_HPP
