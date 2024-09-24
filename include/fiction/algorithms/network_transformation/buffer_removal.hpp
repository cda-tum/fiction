//
// Created by benjamin on 9/24/24.
//

#ifndef FICTION_BUFFER_REMOVAL_HPP
#define FICTION_BUFFER_REMOVAL_HPP

#include "fiction/traits.hpp"
#include "fiction/utils/name_utils.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/views/topo_view.hpp>

#include <cassert>
#include <utility>
#include <vector>

#if (PROGRESS_BARS)
#include <mockturtle/utils/progress_bar.hpp>
#endif

namespace fiction
{

template <class NtkDest, class NtkSrc>
std::pair<NtkDest, mockturtle::node_map<mockturtle::signal<NtkDest>, NtkSrc>>
initialize_copy_network_v(NtkSrc const& src)
{
    static_assert(mockturtle::is_network_type_v<NtkDest>, "NtkDest is not a network type");
    static_assert(mockturtle::is_network_type_v<NtkSrc>, "NtkSrc is not a network type");

    static_assert(mockturtle::has_get_constant_v<NtkDest>, "NtkDest does not implement the get_constant method");
    static_assert(mockturtle::has_create_pi_v<NtkDest>, "NtkDest does not implement the create_pi method");
    static_assert(mockturtle::has_get_constant_v<NtkSrc>, "NtkSrc does not implement the get_constant method");
    static_assert(mockturtle::has_get_node_v<NtkSrc>, "NtkSrc does not implement the get_node method");
    static_assert(mockturtle::has_foreach_pi_v<NtkSrc>, "NtkSrc does not implement the foreach_pi method");

    mockturtle::node_map<mockturtle::signal<NtkDest>, NtkSrc> old2new(src);
    NtkDest                                                   dest;
    old2new[src.get_constant(false)] = dest.get_constant(false);
    if (src.get_node(src.get_constant(true)) != src.get_node(src.get_constant(false)))
    {
        old2new[src.get_constant(true)] = dest.get_constant(true);
    }
    if constexpr (fiction::has_foreach_real_pi_v<NtkSrc>)
    {
        src.foreach_real_pi([&](auto const& n) { old2new[n] = dest.create_pi(); });
        src.foreach_virtual_pi([&](auto const& n) { old2new[n] = dest.create_virtual_pi(src.get_real_pi(n)); });
    }
    else
    {
        src.foreach_pi([&](auto const& n) { old2new[n] = dest.create_pi(); });
    }

    return {dest, old2new};
}

namespace detail
{

template <typename Ntk>
class remove_buffer_impl
{
  public:
    using signal     = typename Ntk::signal;
    using node       = typename Ntk::node;
    using TopoNtkSrc = mockturtle::topo_view<Ntk>;

    explicit remove_buffer_impl(const Ntk& ntk_src) : ntk{ntk_src} {}

    signal skip_buffer_chain_rec(node n, signal s, mockturtle::node_map<signal, Ntk>& old2new)
    {
        // Buffer only have one fan-in && make sure it is not a fan-out node
        if (ntk.is_buf(n) && ntk.fanout_size(n) == 1)
        {
            const auto fc = fanins(ntk, n);
            assert(fc.fanin_nodes.size() == 1 && "Error: Node is not a buffer -> check the conditions for this path");
            const auto fn         = fc.fanin_nodes[0];
            const auto tgt_signal = old2new[fn];

            // Recursive call: Traverse upstream if the fanin node is also a buffer
            return skip_buffer_chain_rec(fn, tgt_signal, old2new);
        }

        // If the node 'n' is not a buffer, return the current signal 's'
        return s;
    }

    Ntk run()
    {
        auto  init     = fiction::initialize_copy_network_v<Ntk>(ntk);
        auto& ntk_dest = init.first;
        auto& old2new  = init.second;

        const auto gather_fanin_signals = [this, &ntk_dest, &old2new](const auto& n)
        {
            std::vector<typename Ntk::signal> children{};

            ntk.foreach_fanin(n,
                              [this, &ntk_dest, &old2new, &children](const auto& f)
                              {
                                  const auto fn         = ntk.get_node(f);
                                  const auto tgt_signal = old2new[fn];

                                  // skip buffer chains
                                  const auto tgt_signal_wo_buf = skip_buffer_chain_rec(fn, tgt_signal, old2new);

                                  children.emplace_back(ntk.is_complemented(f) ?
                                                            ntk_dest.create_not(tgt_signal_wo_buf) :
                                                            tgt_signal_wo_buf);
                              });

            return children;
        };

#if (PROGRESS_BARS)
        // initialize a progress bar
        mockturtle::progress_bar bar{static_cast<uint32_t>(ntk.num_gates()), "[i] network conversion: |{0}|"};
#endif

        ntk.foreach_gate(
            [&, this](const auto& g, [[maybe_unused]] auto i)
            {
                // skip buffer nodes
                if (ntk.is_buf(g) && ntk.fanout_size(g) == 1)
                {
                    return true;
                }

                auto children = gather_fanin_signals(g);

#if (PROGRESS_BARS)
                // update progress
                bar(i);
#endif

                if constexpr (mockturtle::has_is_and_v<TopoNtkSrc> && mockturtle::has_create_and_v<Ntk>)
                {
                    if (ntk.is_and(g))
                    {
                        old2new[g] = ntk_dest.create_and(children[0], children[1]);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_or_v<TopoNtkSrc> && mockturtle::has_create_or_v<Ntk>)
                {
                    if (ntk.is_or(g))
                    {
                        old2new[g] = ntk_dest.create_or(children[0], children[1]);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_xor_v<TopoNtkSrc> && mockturtle::has_create_xor_v<Ntk>)
                {
                    if (ntk.is_xor(g))
                    {
                        old2new[g] = ntk_dest.create_xor(children[0], children[1]);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_maj_v<TopoNtkSrc> && mockturtle::has_create_maj_v<Ntk>)
                {
                    if (ntk.is_maj(g))
                    {
                        old2new[g] = ntk_dest.create_maj(children[0], children[1], children[2]);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_nary_and_v<TopoNtkSrc> && mockturtle::has_create_nary_and_v<Ntk>)
                {
                    if (ntk.is_nary_and(g))
                    {
                        old2new[g] = ntk_dest.create_nary_and(children);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_nary_or_v<TopoNtkSrc> && mockturtle::has_create_nary_or_v<Ntk>)
                {
                    if (ntk.is_nary_or(g))
                    {
                        old2new[g] = ntk_dest.create_nary_or(children);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_is_nary_xor_v<TopoNtkSrc> && mockturtle::has_create_nary_xor_v<Ntk>)
                {
                    if (ntk.is_nary_xor(g))
                    {
                        old2new[g] = ntk_dest.create_nary_xor(children);
                        return true;
                    }
                }
                if constexpr (fiction::has_is_buf_v<TopoNtkSrc> && mockturtle::has_create_buf_v<Ntk>)
                {
                    if (ntk.is_buf(g))
                    {
                        old2new[g] = ntk_dest.create_buf(children[0]);
                        return true;
                    }
                }
                if constexpr (mockturtle::has_node_function_v<TopoNtkSrc> && mockturtle::has_create_node_v<Ntk>)
                {
                    old2new[g] = ntk_dest.create_node(children, ntk.node_function(g));
                    return true;
                }
                assert(false);
                return true;
            });

        ntk.foreach_po(
            [this, &ntk_dest, &old2new](const auto& po)
            {
                const auto tgt_signal = old2new[ntk.get_node(po)];

                // skip buffer chains
                const auto tgt_signal_wo_buf = skip_buffer_chain_rec(ntk.get_node(po), tgt_signal, old2new);

                const auto tgt_po =
                    ntk.is_complemented(po) ? ntk_dest.create_not(tgt_signal_wo_buf) : tgt_signal_wo_buf;

                ntk_dest.create_po(tgt_po);
            });

        // restore signal names if applicable
        fiction::restore_names(ntk, ntk_dest, old2new);

        return ntk_dest;
    }

  private:
    Ntk ntk;
};
}  // namespace detail

template <typename Ntk>
bool constains_buffer(Ntk ntk)
{
    bool has_buf = false;
    ntk.foreach_gate(
        [&ntk, &has_buf](const auto& g)
        {
            if (ntk.is_buf(g))
            {
                has_buf = true;
            }
        });
    return has_buf;
}

/**
 * Converts a logic network into an equivalent one of another type. Thereby, this function is very similar to
 * `mockturtle::cleanup_dangling`. However, it supports real buffer nodes used for fanouts and path balancing in
 * fiction.
 *
 * @note If `NtkDest` and `NtkSrc` are of the same type, this function returns `ntk` cleaned using
 * `mockturtle::cleanup_dangling`.
 *
 * @tparam NtkDest Type of the returned logic network.
 * @tparam NtkSrc Type of the input logic network.
 * @param ntk The input logic network.
 * @return A logic network of type `NtkDest` that is logically equivalent to `ntk`.
 */
template <typename Ntk>
Ntk remove_buffer(const Ntk& ntk)
{
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");

    static_assert(mockturtle::has_is_buf_v<Ntk>, "Ntk does not implement the is_buf function");
    static_assert(mockturtle::has_get_node_v<Ntk>, "Ntk does not implement the get_node function");
    static_assert(mockturtle::has_is_complemented_v<Ntk>, "Ntk does not implement the is_complemented function");
    static_assert(mockturtle::has_foreach_pi_v<Ntk>, "Ntk does not implement the foreach_pi function");
    static_assert(mockturtle::has_foreach_gate_v<Ntk>, "Ntk does not implement the foreach_gate function");
    static_assert(mockturtle::has_foreach_po_v<Ntk>, "Ntk does not implement the foreach_po function");
    static_assert(mockturtle::has_foreach_fanin_v<Ntk>, "Ntk does not implement the foreach_fanin function");

    static_assert(mockturtle::has_get_constant_v<Ntk>, "Ntk does not implement the get_constant function");

    static_assert(mockturtle::has_create_pi_v<Ntk>, "Ntk does not implement the create_pi function");
    static_assert(mockturtle::has_create_po_v<Ntk>, "Ntk does not implement the create_po function");
    static_assert(mockturtle::has_create_not_v<Ntk>, "Ntk does not implement the create_not function");
    static_assert(mockturtle::has_create_and_v<Ntk>, "Ntk does not implement the create_and function");
    static_assert(mockturtle::has_create_or_v<Ntk>, "Ntk does not implement the create_or function");
    static_assert(mockturtle::has_create_xor_v<Ntk>, "Ntk does not implement the create_xor function");
    static_assert(mockturtle::has_create_maj_v<Ntk>, "Ntk does not implement the create_maj function");
    // TODO handle ci/ro/etc...

    assert(ntk.is_combinational() && "Network has to be combinational");

    if (!constains_buffer(ntk))
    {
        return ntk;
    }

    detail::remove_buffer_impl<Ntk> p{ntk};

    auto result = p.run();

    return result;
}

}  // namespace fiction

#endif  // FICTION_BUFFER_REMOVAL_HPP
