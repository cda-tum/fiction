//
// Created by benjamin on 13.04.23.
//

#ifndef FICTION_INVERTER_BALANCING_HPP
#define FICTION_INVERTER_BALANCING_HPP

#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/name_utils.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/topo_view.hpp>

#include <type_traits>

#if (PROGRESS_BARS)
#include <mockturtle/utils/progress_bar.hpp>
#endif

namespace fiction
{

namespace detail
{

template <typename Ntk>
class inverter_balancing_impl
{
  public:
    explicit inverter_balancing_impl(const Ntk& ntk_src) : ntk{ntk_src} {}

    Ntk run()
    {
        // compute maximum sizes for vectors
        std::uint64_t vec_size{0};
        fo_ntk.foreach_node(
            [this, &vec_size](const auto& n)
            {
                if (fo_ntk.is_fanout(n) && ntk.fanout_size(n) >= 2)
                {
                    ++vec_size;
                }
            });
        x_inv.reserve(vec_size);
        m_inv.reserve(vec_size);
        blc_fos.reserve(vec_size);

        auto  init     = mockturtle::initialize_copy_network<Ntk>(ntk);
        auto& ntk_dest = init.first;
        auto& old2new  = init.second;

        const auto gather_fanin_signals = [this, &ntk_dest, &old2new](const auto& n)
        {
            std::vector<typename Ntk::signal> children{};

            if (fo_ntk.is_fanout(n))
            {
                auto fanout_inv = fanouts(fo_ntk, n);
                auto balance    = std::all_of(fanout_inv.begin(), fanout_inv.end(),
                                              [this](const auto& fo_node) { return fo_ntk.is_inv(fo_node); });
                if (balance && fanout_inv.size() > 1)
                {
                    x_inv.emplace_back(fanout_inv[1]);
                    blc_fos.emplace_back(n);
                    m_inv.emplace_back(fanout_inv[0]);
                }
            }

            ntk.foreach_fanin(n,
                              [this, &old2new, &children, &n](const auto& f)
                              {
                                  auto fn = ntk.get_node(f);

                                  // New children for nodes with deleted Inverters as fan-in
                                  for (int i = 0; i < blc_fos.size(); ++i)
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
        mockturtle::progress_bar bar{static_cast<uint32_t>(ntk.num_gates()), "[i] network conversion: |{0}|"};
#endif

        ntk.foreach_gate(
            [this, &gather_fanin_signals, &old2new, &ntk_dest](const auto& g, [[maybe_unused]] auto i)
            {
                auto children = gather_fanin_signals(g);

#if (PROGRESS_BARS)
                // update progress
                bar(i);
#endif

                if constexpr (has_is_inv_v<TopoNtkSrc>)
                {
                    if (ntk.is_inv(g) && std::find(m_inv.cbegin(), m_inv.cend(), g) != m_inv.cend())
                    {
                        old2new[g] = ntk_dest.create_buf(children[0]);
                        return true;
                    }
                    auto po_it = std::find(x_inv.cbegin(), x_inv.cend(), g);
                    if (ntk.is_inv(g) && po_it != x_inv.cend())
                    {
                        if (ntk.is_po(g))
                        {
                            // Preserve Outputs
                            int index = po_it - x_inv.begin();
                            preserved_po.push_back(m_inv[index]);
                        }
                        return true;
                    }
                }
                if constexpr (fiction::has_is_buf_v<TopoNtkSrc> && mockturtle::has_create_buf_v<Ntk>)
                {
                    if (ntk.is_buf(g) && std::find(blc_fos.cbegin(), blc_fos.cend(), g) != blc_fos.cend())
                    {
                        old2new[g] = ntk_dest.create_not(children[0]);
                        return true;
                    }
                }
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
                if constexpr (mockturtle::has_node_function_v<TopoNtkSrc> && mockturtle::has_create_node_v<Ntk>)
                {
                    old2new[g] = ntk_dest.create_node(children, ntk.node_function(g));
                    return true;
                }

                return true;
            });

        ntk.foreach_po(
            [this, &ntk_dest, &old2new](const auto& po)
            {
                auto tgt_signal = old2new[ntk.get_node(po)];
                auto tgt_po     = ntk.is_complemented(po) ? ntk_dest.create_not(tgt_signal) : tgt_signal;
                if (tgt_po == 0)
                {
                    tgt_signal = old2new[ntk.get_node(preserved_po[0])];
                    preserved_po.erase(preserved_po.begin());
                    tgt_po = ntk.is_complemented(po) ? ntk_dest.create_not(tgt_signal) : tgt_signal;
                }
                ntk_dest.create_po(tgt_po);
            });

        if constexpr (mockturtle::has_foreach_ri_v<TopoNtkSrc>)
        {
            ntk.foreach_ri(
                [this, &ntk_dest, &old2new](const auto& po)
                {
                    const auto tgt_signal = old2new[ntk.get_node(po)];
                    const auto tgt_po     = ntk.is_complemented(po) ? ntk_dest.create_not(tgt_signal) : tgt_signal;

                    ntk_dest.create_ri(tgt_po);
                });
        }

        // restore signal names if applicable
        fiction::restore_names(ntk, ntk_dest, old2new);

        return ntk_dest;
    }

  private:
    using TopoNtkSrc = mockturtle::topo_view<Ntk>;
    TopoNtkSrc                          ntk;
    mockturtle::fanout_view<TopoNtkSrc> fo_ntk{ntk};
    std::vector<mockturtle::node<Ntk>>  x_inv{};         // inverter nodes, which get deleted
    std::vector<mockturtle::node<Ntk>>  m_inv{};         // inverter nodes, which get moved to fanin position
    std::vector<mockturtle::node<Ntk>>  blc_fos{};       // fo nodes, where balancing is applied
    std::vector<mockturtle::node<Ntk>>  preserved_po{};  // nodes where pos need to be preserved
};

template <typename NtkSrc>
bool inverter_balancing_recursive(const NtkSrc& ntk)
{
    bool       return_val = false;
    const auto fo_ntk     = mockturtle::fanout_view<NtkSrc>(ntk);
    ntk.foreach_gate(
        [&](const auto& g)
        {
            if (ntk.fanout_size(g) >= 2)
            {
                auto fanout_inv = fanouts(fo_ntk, g);
                auto balance    = std::all_of(fanout_inv.begin(), fanout_inv.end(),
                                              [&](const auto& fo_node) { return fo_ntk.is_inv(fo_node); });
                if (balance && fanout_inv.size() > 1)
                {
                    return_val = true;
                }
            }
        });
    return return_val;
}

}  // namespace detail

/**
 * Balances inverters at fanout nodes. Thereby, this function is very similar to fiction::network_conversion.
 *
 * When both fanouts of a fanout-node are inverted, the inverters are replaced by one inverter as fanin of this node
 * This is part of the Distribution Newtork I: Input_Ordering
 *
 * @tparam Ntk Type of the input logic network.
 * @param ntk The input logic network.
 * @return A logic network of type Ntk
 * */

template <typename Ntk>
Ntk inverter_balancing(const Ntk& ntk)
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

    detail::inverter_balancing_impl<Ntk> p{ntk};

    auto result = p.run();
    // check if the new ntk is balanced
    bool rerun = detail::inverter_balancing_recursive(result);
    while (rerun)
    {
        detail::inverter_balancing_impl<Ntk> k{result};

        result = k.run();

        rerun = detail::inverter_balancing_recursive(result);
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_INVERTER_BALANCING_HPP
