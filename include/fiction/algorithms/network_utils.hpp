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
    node<Ntk> source, target;
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

}  // namespace fiction

#endif  // FICTION_NETWORK_UTILS_HPP
