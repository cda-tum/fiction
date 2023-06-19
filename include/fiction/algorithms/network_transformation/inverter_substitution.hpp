//
// Created by benjamin on 13.04.23.
//

#ifndef FICTION_INVERTER_SUBSTITUTION_HPP
#define FICTION_INVERTER_SUBSTITUTION_HPP

#include "fiction/algorithms/properties/count_gate_types.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/name_utils.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/topo_view.hpp>

#include <algorithm>
#include <cstdint>
#include <type_traits>
#include <vector>

#if (PROGRESS_BARS)
#include <mockturtle/utils/progress_bar.hpp>
#endif

namespace fiction
{

namespace detail
{

/**
 * Connect all gates, that are not affected by the inverter substitution.
 *
 * @param ntk input network.
 * @param ntk_dest output network.
 * @param old2new node_map to assign the nodes of the old network to the new network.
 * @param children children of the current node.
 * @param g currently viewed node.
 * @return 'true' iff the assignment was successful.
 */
template <typename Ntk, typename NtkDest>
bool connect_children_to_gates(const Ntk& ntk, const NtkDest& ntk_dest,
                               mockturtle::node_map<mockturtle::signal<Ntk>, Ntk>& old2new,
                               const std::vector<typename Ntk::signal>& children, const mockturtle::node<Ntk>& g)
{
    if constexpr (mockturtle::has_is_and_v<Ntk> && mockturtle::has_create_and_v<Ntk>)
    {
        if (ntk.is_and(g))
        {
            old2new[g] = ntk_dest.create_and(children[0], children[1]);
            return true;  // keep looping
        }
    }
    if constexpr (mockturtle::has_is_or_v<Ntk> && mockturtle::has_create_or_v<Ntk>)
    {
        if (ntk.is_or(g))
        {
            old2new[g] = ntk_dest.create_or(children[0], children[1]);
            return true;  // keep looping
        }
    }
    if constexpr (mockturtle::has_is_xor_v<Ntk> && mockturtle::has_create_xor_v<Ntk>)
    {
        if (ntk.is_xor(g))
        {
            old2new[g] = ntk_dest.create_xor(children[0], children[1]);
            return true;  // keep looping
        }
    }
    if constexpr (mockturtle::has_is_maj_v<Ntk> && mockturtle::has_create_maj_v<Ntk>)
    {
        if (ntk.is_maj(g))
        {
            old2new[g] = ntk_dest.create_maj(children[0], children[1], children[2]);
            return true;  // keep looping
        }
    }
    if constexpr (mockturtle::has_is_nary_and_v<Ntk> && mockturtle::has_create_nary_and_v<Ntk>)
    {
        if (ntk.is_nary_and(g))
        {
            old2new[g] = ntk_dest.create_nary_and(children);
            return true;  // keep looping
        }
    }
    if constexpr (mockturtle::has_is_nary_or_v<Ntk> && mockturtle::has_create_nary_or_v<Ntk>)
    {
        if (ntk.is_nary_or(g))
        {
            old2new[g] = ntk_dest.create_nary_or(children);
            return true;  // keep looping
        }
    }
    if constexpr (mockturtle::has_is_nary_xor_v<Ntk> && mockturtle::has_create_nary_xor_v<Ntk>)
    {
        if (ntk.is_nary_xor(g))
        {
            old2new[g] = ntk_dest.create_nary_xor(children);
            return true;  // keep looping
        }
    }
    return false;  // gate type not supported
}

template <typename Ntk>
class inverter_substitution_impl
{
  public:
    explicit inverter_substitution_impl(const Ntk& ntk_src) : ntk{ntk_src}
    {
        // compute maximum sizes for vectors
        count_gate_types_stats st{};
        count_gate_types(fo_ntk, &st);
        x_inv.reserve(st.num_fanout);
        m_inv.reserve(st.num_fanout);
        blc_fos.reserve(st.num_fanout);

        // enable premature termination, when no optimizations are possible
        if (st.num_fanout == 0 || st.num_inv < 1)
        {
            rerun = false;
        }
    }

    // check if optimizations were made
    [[nodiscard]] bool is_rerun() const
    {
        return rerun;
    }

    Ntk run()
    {
        auto  init     = mockturtle::initialize_copy_network<Ntk>(ntk);
        auto& ntk_dest = init.first;
        auto& old2new  = init.second;

        const auto gather_fanin_signals = [this, &old2new](const auto& n)
        {
            std::vector<typename Ntk::signal> children{};
            // store affected nodes
            if (fo_ntk.is_fanout(n))
            {
                const auto fanout_inv    = fanouts(fo_ntk, n);
                const auto do_substitute = std::all_of(fanout_inv.cbegin(), fanout_inv.cend(),
                                                       [this](const auto& fo_node) { return fo_ntk.is_inv(fo_node); });
                if (do_substitute && fanout_inv.size() > 1)
                {
                    x_inv.emplace_back(fanout_inv[1]);
                    blc_fos.emplace_back(n);
                    m_inv.emplace_back(fanout_inv[0]);
                }
            }
            // compute children of affected nodes
            ntk.foreach_fanin(n,
                              [this, &old2new, &children](const auto& f)
                              {
                                  auto fn = ntk.get_node(f);

                                  for (std::size_t i = 0; i < blc_fos.size(); ++i)
                                  {
                                      if (x_inv[i] == fn)
                                      {
                                          const auto fis = fanins(fo_ntk, fn);
                                          fn             = m_inv[i];
                                      }
                                  }

                                  const auto tgt_signal = old2new[fn];
                                  children.emplace_back(tgt_signal);
                              });

            return children;
        };

#if (PROGRESS_BARS)
        // initialize a progress bar
        mockturtle::progress_bar bar{static_cast<uint32_t>(ntk.num_gates()), "[i] inverter substitution: |{0}|"};
#endif

        ntk.foreach_gate(
            [&, this](const auto& g, [[maybe_unused]] auto i)
            {
                const auto children = gather_fanin_signals(g);

#if (PROGRESS_BARS)
                // update progress
                bar(i);
#endif
                // map all affected nodes
                if constexpr (mockturtle::has_create_buf_v<Ntk>)
                {
                    if (ntk.is_inv(g) && std::find(m_inv.cbegin(), m_inv.cend(), g) != m_inv.cend())
                    {
                        old2new[g] = ntk_dest.create_buf(children[0]);
                        return true;  // keep looping
                    }
                    if (const auto po_it = std::find(x_inv.cbegin(), x_inv.cend(), g);
                        ntk.is_inv(g) && po_it != x_inv.cend())
                    {
                        if (ntk.is_po(g))
                        {
                            // Preserve Outputs
                            const auto index = po_it - x_inv.cbegin();
                            preserved_po.push_back(
                                m_inv[static_cast<std::vector<long unsigned int>::size_type>(index)]);
                        }
                        return true;  // keep looping
                    }
                }
                if constexpr (mockturtle::has_create_not_v<Ntk>)
                {
                    if (ntk.is_buf(g) && std::find(blc_fos.cbegin(), blc_fos.cend(), g) != blc_fos.cend())
                    {
                        old2new[g] = ntk_dest.create_not(children[0]);
                        return true;  // keep looping
                    }
                }
                // map all unaffected nodes
                if (connect_children_to_gates(ntk, ntk_dest, old2new, children, g))
                {
                    return true;  // keep looping
                }
                if constexpr (mockturtle::has_node_function_v<TopoNtkSrc> && mockturtle::has_create_node_v<Ntk>)
                {
                    old2new[g] = ntk_dest.create_node(children, ntk.node_function(g));
                    return true;  // keep looping
                }

                return true;  // keep looping
            });

        // create the POs of the network, POs of deleted inverters need to be preserved
        ntk.foreach_po(
            [this, &ntk_dest, &old2new](const auto& po)
            {
                auto tgt_signal = old2new[ntk.get_node(po)];
                auto tgt_po     = ntk.is_complemented(po) ? ntk_dest.create_not(tgt_signal) : tgt_signal;
                if (tgt_po == mockturtle::signal<Ntk>{} && !ntk.is_constant(po))
                {
                    tgt_signal = old2new[ntk.get_node(preserved_po[0])];
                    preserved_po.erase(preserved_po.cbegin());
                    tgt_po = ntk.is_complemented(po) ? ntk_dest.create_not(tgt_signal) : tgt_signal;
                }
                ntk_dest.create_po(tgt_po);
            });

        // restore signal names if applicable
        fiction::restore_names(ntk, ntk_dest, old2new);

        // check if any optimizations were made
        if (blc_fos.empty())
        {
            rerun = false;
        }

        return ntk_dest;
    }

  private:
    /**
     * @param ntk topologically ordered input logic network.
     * @param fo_ntk fan-out view of ntk.
     * @param x_inv inverter nodes, which get deleted.
     * @param m_inv inverter nodes, which get moved to fan-in position.
     * @param blc_fos fo nodes, where balancing is applied.
     * @param preserved_po nodes where pos need to be preserved.
     * @param rerun indicate if optimizations can be made.
     */
    using TopoNtkSrc = mockturtle::topo_view<Ntk>;
    TopoNtkSrc                          ntk;
    mockturtle::fanout_view<TopoNtkSrc> fo_ntk{ntk};
    std::vector<mockturtle::node<Ntk>>  x_inv{};
    std::vector<mockturtle::node<Ntk>>  m_inv{};
    std::vector<mockturtle::node<Ntk>>  blc_fos{};
    std::vector<mockturtle::node<Ntk>>  preserved_po{};
    bool                                rerun{true};
};

}  // namespace detail

/**
 * Substitutes two inverters at the fan-outs of a fan-out node with one inverter at the fan-in of this node.
 * Therefore one inverter gets deleted and one inverter gets moved to the fan-in of the fan-out node.
 * All nodes, with fan-ins affected by deletion or moving of nodes are taken into account by the algorithm.
 * This is part of the Distribution Network I: Input_Ordering
 *
 * @tparam Ntk A logic network of type `Ntk` with optimized inverter count.
 * @param ntk The input logic network.
 * @return A logic network of type Ntk.
 */
template <typename Ntk>
Ntk inverter_substitution(const Ntk& ntk)
{
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
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

    static_assert(has_is_fanout_v<Ntk>, "Ntk does not implement the has_is_fanout function");

    assert(ntk.is_combinational() && "Network has to be combinational");

    bool run    = true;
    auto result = ntk;
    while (run)
    {
        detail::inverter_substitution_impl<Ntk> p{result};
        if (!p.is_rerun())
        {
            break;  // premature termination
        }
        result = p.run();
        run    = p.is_rerun();
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_INVERTER_SUBSTITUTION_HPP
