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
 * This function connects gates that aren't affected by the inverter substitution. It means that all the gates without
 * any fan-ins or fan-outs impacted by the inverter substitution retain their functionality and are subsequently
 * connected to their children.
 *
 * @tparam Ntk Type of the input logic network.
 * @tparam NtkDest Type of the returned logic network.
 * @param ntk Input network.
 * @param ntk_dest Output network.
 * @param old2new `node_map` to assign the nodes of the old network to the new network.
 * @param children Children of the current gate.
 * @param g Currently viewed gate.
 * @return 'true' iff the assignment was successful.
 */
template <typename Ntk, typename NtkDest>
bool connect_children_to_gates(const Ntk& ntk, NtkDest& ntk_dest,
                               mockturtle::node_map<mockturtle::signal<Ntk>, Ntk>& old2new,
                               const mockturtle::node<Ntk>& g, const std::vector<typename Ntk::signal>& children)
{
    if constexpr (mockturtle::has_is_and_v<Ntk> && mockturtle::has_create_and_v<NtkDest>)
    {
        if (ntk.is_and(g))
        {
            old2new[g] = ntk_dest.create_and(children[0], children[1]);
            return true;  // keep looping
        }
    }
    if constexpr (mockturtle::has_is_or_v<Ntk> && mockturtle::has_create_or_v<NtkDest>)
    {
        if (ntk.is_or(g))
        {
            old2new[g] = ntk_dest.create_or(children[0], children[1]);
            return true;  // keep looping
        }
    }
    if constexpr (mockturtle::has_is_xor_v<Ntk> && mockturtle::has_create_xor_v<NtkDest>)
    {
        if (ntk.is_xor(g))
        {
            old2new[g] = ntk_dest.create_xor(children[0], children[1]);
            return true;  // keep looping
        }
    }
    if constexpr (mockturtle::has_is_maj_v<Ntk> && mockturtle::has_create_maj_v<NtkDest>)
    {
        if (ntk.is_maj(g))
        {
            old2new[g] = ntk_dest.create_maj(children[0], children[1], children[2]);
            return true;  // keep looping
        }
    }
    if constexpr (mockturtle::has_is_nary_and_v<Ntk> && mockturtle::has_create_nary_and_v<NtkDest>)
    {
        if (ntk.is_nary_and(g))
        {
            old2new[g] = ntk_dest.create_nary_and(children);
            return true;  // keep looping
        }
    }
    if constexpr (mockturtle::has_is_nary_or_v<Ntk> && mockturtle::has_create_nary_or_v<NtkDest>)
    {
        if (ntk.is_nary_or(g))
        {
            old2new[g] = ntk_dest.create_nary_or(children);
            return true;  // keep looping
        }
    }
    if constexpr (mockturtle::has_is_nary_xor_v<Ntk> && mockturtle::has_create_nary_xor_v<NtkDest>)
    {
        if (ntk.is_nary_xor(g))
        {
            old2new[g] = ntk_dest.create_nary_xor(children);
            return true;  // keep looping
        }
    }
    if constexpr (mockturtle::has_node_function_v<Ntk> && mockturtle::has_create_node_v<NtkDest>)
    {
        old2new[g] = ntk_dest.create_node(children, ntk.node_function(g));
        return true;  // keep looping
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
                if constexpr (fiction::has_is_inv_v<TopoNtkSrc> && mockturtle::has_create_buf_v<Ntk>)
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
                            preserved_po.push_back(m_inv[static_cast<std::vector<uint64_t>::size_type>(index)]);
                        }
                        return true;  // keep looping
                    }
                }
                if constexpr (fiction::has_is_buf_v<TopoNtkSrc> && mockturtle::has_create_not_v<Ntk>)
                {
                    if (ntk.is_buf(g) && std::find(blc_fos.cbegin(), blc_fos.cend(), g) != blc_fos.cend())
                    {
                        old2new[g] = ntk_dest.create_not(children[0]);
                        return true;  // keep looping
                    }
                }
                // map all unaffected nodes
                if (connect_children_to_gates(ntk, ntk_dest, old2new, g, children))
                {
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
     * Alias for a topological view of a network.
     */
    using TopoNtkSrc = mockturtle::topo_view<Ntk>;
    /**
     * A topologically ordered input logic network.
     */
    TopoNtkSrc ntk;
    /**
     * This is a fan-out view of the network 'ntk'.
     */
    mockturtle::fanout_view<TopoNtkSrc> fo_ntk{ntk};
    /**
     * A collection of inverter nodes that are set to be deleted.
     */
    std::vector<mockturtle::node<Ntk>> x_inv{};
    /**
     * A collection of inverter nodes that are set to be moved to fan-in position.
     */
    std::vector<mockturtle::node<Ntk>> m_inv{};
    /**
     * These are fan-out nodes where balancing is applied.
     */
    std::vector<mockturtle::node<Ntk>> blc_fos{};
    /**
     * Collection of nodes where primary outputs need to be preserved.
     */
    std::vector<mockturtle::node<Ntk>> preserved_po{};
    /**
     * An indicator to check if optimizations can be applied or not.
     */
    bool rerun{true};
};

}  // namespace detail

/**
 * A network optimization algorithm that substitutes inverters at the outputs of all fan-out nodes with one single
 * inverter at their inputs.
 * Thereby, the total number of inverters is reduced.
 * This is part of the Signal Distribution Networks I: Input Ordering.
 *
 * @tparam Ntk Logic network type.
 * @param ntk The input logic network whose inverter count is to be optimized.
 * @return A network that is logically equivalent to `ntk`, but with an optimized inverter count.
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
