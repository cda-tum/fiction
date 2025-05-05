//
// Created by benjamin on 13.04.23.
//

#ifndef FICTION_INVERTER_SUBSTITUTION_HPP
#define FICTION_INVERTER_SUBSTITUTION_HPP

#include "fiction/algorithms/properties/count_gate_types.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/name_utils.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/views/fanout_view.hpp>
#include <mockturtle/views/topo_view.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

#if (PROGRESS_BARS)
#include <mockturtle/utils/progress_bar.hpp>
#endif

namespace fiction
{

namespace detail
{

/**
 * Sets the level and rank of a node in the destination network by invoking its `on_add` hook.
 *
 * @tparam Ntk Type of the input logic network.
 * @tparam NtkDest Type of the output logic network.
 * @param ntk_dest Destination network where the node has been created.
 * @param old2new Mapping from nodes in the source network to signals in the destination network.
 * @param g Node from the source network whose corresponding destination node should have its level and rank set.
 */
template <typename Ntk, typename NtkDest>
void set_level_and_rank(NtkDest& ntk_dest, mockturtle::node_map<mockturtle::signal<Ntk>, Ntk>& old2new,
                        const mockturtle::node<Ntk>& g)
{
    if constexpr (mockturtle::has_depth_v<NtkDest> || mockturtle::has_rank_position_v<NtkDest>)
    {
        ntk_dest.on_add(old2new[g]);
    }
}

/**
 * Connects gates in the destination network whose fanins and fanouts are not modified by inverter substitution.
 *
 * @tparam Ntk Type of the input logic network.
 * @tparam NtkDest Type of the output logic network.
 * @param ntk Source network.
 * @param ntk_dest Destination network.
 * @param old2new Mapping from nodes in the source network to signals in the destination network.
 * @param g Node in the source network currently being processed.
 * @param children Signals of the children nodes in the destination network.
 * @return True if the gate was successfully processed and added to the destination network.
 */
template <typename Ntk, typename NtkDest>
bool connect_children_to_gates_unaffected(const Ntk& ntk, NtkDest& ntk_dest,
                                          mockturtle::node_map<mockturtle::signal<Ntk>, Ntk>& old2new,
                                          const mockturtle::node<Ntk>&                        g,
                                          const std::vector<typename Ntk::signal>&            children)
{
    if constexpr (mockturtle::has_is_and_v<Ntk> && mockturtle::has_create_and_v<NtkDest>)
    {
        if (ntk.is_and(g))
        {
            old2new[g] = ntk_dest.create_and(children[0], children[1]);
            set_level_and_rank(ntk_dest, old2new, g);
            return true;  // keep looping
        }
    }
    if constexpr (mockturtle::has_is_or_v<Ntk> && mockturtle::has_create_or_v<NtkDest>)
    {
        if (ntk.is_or(g))
        {
            old2new[g] = ntk_dest.create_or(children[0], children[1]);
            set_level_and_rank(ntk_dest, old2new, g);
            return true;  // keep looping
        }
    }
    if constexpr (mockturtle::has_is_xor_v<Ntk> && mockturtle::has_create_xor_v<NtkDest>)
    {
        if (ntk.is_xor(g))
        {
            old2new[g] = ntk_dest.create_xor(children[0], children[1]);
            set_level_and_rank(ntk_dest, old2new, g);
            return true;  // keep looping
        }
    }
    if constexpr (mockturtle::has_is_maj_v<Ntk> && mockturtle::has_create_maj_v<NtkDest>)
    {
        if (ntk.is_maj(g))
        {
            old2new[g] = ntk_dest.create_maj(children[0], children[1], children[2]);
            set_level_and_rank(ntk_dest, old2new, g);
            return true;  // keep looping
        }
    }
    if constexpr (mockturtle::has_is_nary_and_v<Ntk> && mockturtle::has_create_nary_and_v<NtkDest>)
    {
        if (ntk.is_nary_and(g))
        {
            old2new[g] = ntk_dest.create_nary_and(children);
            set_level_and_rank(ntk_dest, old2new, g);
            return true;  // keep looping
        }
    }
    if constexpr (mockturtle::has_is_nary_or_v<Ntk> && mockturtle::has_create_nary_or_v<NtkDest>)
    {
        if (ntk.is_nary_or(g))
        {
            old2new[g] = ntk_dest.create_nary_or(children);
            set_level_and_rank(ntk_dest, old2new, g);
            return true;  // keep looping
        }
    }
    if constexpr (mockturtle::has_is_nary_xor_v<Ntk> && mockturtle::has_create_nary_xor_v<NtkDest>)
    {
        if (ntk.is_nary_xor(g))
        {
            old2new[g] = ntk_dest.create_nary_xor(children);
            set_level_and_rank(ntk_dest, old2new, g);
            return true;  // keep looping
        }
    }
    if constexpr (mockturtle::has_is_not_v<Ntk> && mockturtle::has_create_not_v<NtkDest>)
    {
        if (ntk.is_not(g))
        {
            old2new[g] = ntk_dest.create_not(children[0]);
            set_level_and_rank(ntk_dest, old2new, g);
            return true;
        }
    }
    if constexpr (fiction::has_is_buf_v<Ntk> && mockturtle::has_create_buf_v<NtkDest>)
    {
        if (ntk.is_buf(g))
        {
            old2new[g] = ntk_dest.create_buf(children[0]);
            set_level_and_rank(ntk_dest, old2new, g);
            return true;
        }
    }
    if constexpr (mockturtle::has_node_function_v<Ntk> && mockturtle::has_create_node_v<NtkDest>)
    {
        old2new[g] = ntk_dest.create_node(children, ntk.node_function(g));
        set_level_and_rank(ntk_dest, old2new, g);
        return true;  // keep looping
    }
    return false;  // gate type not supported
}

/**
 * Defines the operation modes for inverter substitution.
 *
 * Determines which types of nodes should be considered for optimization during the substitution process.
 */
enum class operation_mode : std::uint8_t
{
    /**
     * Optimize only inverters at fanout nodes.
     */
    FO_ONLY,
    /**
     * Optimize only inverters at inputs of AND/OR gates.
     */
    AND_OR_ONLY,
    /**
     * Apply all available optimizations (fanout and AND/OR structures).
     */
    ALL_NODES
};

/**
 * Initializes a logic network by copying a source network, including its constants and primary inputs,
 * and optionally handling virtual primary inputs.
 *
 * This is used to create a destination network that will serve as the base for further optimizations.
 * A mapping from old to new signals is returned to allow reconstruction of internal logic.
 *
 * @tparam NtkDest Type of the destination network.
 * @tparam NtkSrc Type of the source network.
 * @param src Source network to copy.
 * @return A pair containing the new network and a node-to-signal mapping from the old to the new network.
 */

template <class NtkDest, class NtkSrc>
std::pair<NtkDest, mockturtle::node_map<mockturtle::signal<NtkDest>, NtkSrc>>
initialize_copy_network_virtual(NtkSrc const& src)
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

    if constexpr (has_is_virtual_pi_v<NtkSrc> && has_is_virtual_pi_v<NtkDest>)
    {
        if constexpr (mockturtle::has_rank_position_v<NtkSrc>)
        {
            src.foreach_pi_unranked(
                [&](auto const& n)
                {
                    if (src.is_real_pi(n))
                    {
                        old2new[n] = dest.create_pi();
                    }
                    else
                    {
                        old2new[n] = dest.create_virtual_pi(old2new[src.get_real_pi(n)]);
                    }
                });
        }
        else
        {
            src.foreach_pi(
                [&](auto const& n)
                {
                    if (src.is_real_pi(n))
                    {
                        old2new[n] = dest.create_pi();
                    }
                    else
                    {
                        old2new[n] = dest.create_virtual_pi(old2new[src.get_real_pi(n)]);
                    }
                });
        }
    }
    else
    {
        if constexpr (mockturtle::has_rank_position_v<NtkSrc>)
        {
            src.foreach_pi_unranked([&](auto const& n) { old2new[n] = dest.create_pi(); });
        }
        else
        {
            src.foreach_pi([&](auto const& n) { old2new[n] = dest.create_pi(); });
        }
    }
    return {dest, old2new};
}

template <typename Ntk>
class inverter_substitution_impl
{
  public:
    explicit inverter_substitution_impl(const Ntk& ntk_src, operation_mode substitution_mode) :
            ntk{ntk_src},
            mode{substitution_mode}
    {
        // compute maximum sizes for vectors
        count_gate_types_stats st{};
        count_gate_types(fo_ntk, &st);
        x_inv_fo.reserve(st.num_fanout);
        m_inv_fo.reserve(st.num_fanout);
        fo_nodes.reserve(st.num_fanout);

        // enable premature termination, when no optimizations are possible
        if (substitution_mode == operation_mode::FO_ONLY)
        {
            if (st.num_fanout == 0 || st.num_inv < 2)
            {
                rerun = false;
            }
        }
        if (substitution_mode == operation_mode::AND_OR_ONLY)
        {
            if ((st.num_and2 == 0 && st.num_or2 == 0) || st.num_inv < 2)
            {
                rerun = false;
            }
        }
        if (substitution_mode == operation_mode::ALL_NODES)
        {
            if ((st.num_fanout == 0 || st.num_inv < 2) && ((st.num_and2 == 0 && st.num_or2 == 0) || st.num_inv < 2))
            {
                rerun = false;
            }
        }
    }

    // check if optimizations were made
    [[nodiscard]] bool is_rerun() const
    {
        return rerun;
    }

    Ntk run()
    {
        auto  init     = initialize_copy_network_virtual<Ntk>(ntk);
        auto& ntk_dest = init.first;
        auto& old2new  = init.second;

#if (PROGRESS_BARS)
        // initialize a progress bar
        mockturtle::progress_bar bar{static_cast<uint32_t>(ntk.num_gates()), "[i] inverter substitution: |{0}|"};
#endif

        // Restore the rank order of the PIs
        if constexpr (mockturtle::has_rank_position_v<Ntk>)
        {
            ntk.foreach_pi([&ntk_dest](const auto& pi) { ntk_dest.on_add(pi); });
        }
        ntk.foreach_gate(
            [&, this](const auto& g, [[maybe_unused]] auto i)
            {
                const auto children = gather_fanin_signals<TopoNtkSrc>(g, old2new);

                for (const auto& pair : unplaced_nodes)
                {
                    const auto& arr = pair.second;
                    if (std::find(arr.begin(), arr.end(), g) != arr.end())
                    {
                        return true;
                    }
                }

#if (PROGRESS_BARS)
                // update progress
                bar(i);
#endif

                connect_children_to_gates<TopoNtkSrc>(ntk_dest, old2new, g, children);
                if (unplaced_nodes.find(g) != unplaced_nodes.end())
                {
                    for (int j = 0; j < 3; ++j)
                    {
                        const auto child = gather_fanin_signals<TopoNtkSrc>(unplaced_nodes[g][j], old2new);
                        connect_children_to_gates<TopoNtkSrc>(ntk_dest, old2new, unplaced_nodes[g][j], child);
                    }
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
        restore_names(ntk, ntk_dest, old2new);

        // check if any optimizations were made
        if (mode == operation_mode::FO_ONLY)
        {
            if (fo_nodes.empty())
            {
                rerun = false;
            }
        }
        else if (mode == operation_mode::AND_OR_ONLY)
        {
            if (and_or_nodes.empty())
            {
                rerun = false;
            }
        }
        else if (mode == operation_mode::ALL_NODES)
        {
            if (and_or_nodes.empty() && fo_nodes.empty())
            {
                rerun = false;
            }
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
    std::vector<mockturtle::node<Ntk>> x_inv_fo{};
    /**
     * A collection of inverter nodes that are set to be moved to fan-in position.
     */
    std::vector<mockturtle::node<Ntk>> m_inv_fo{};
    /**
     * These are fan-out nodes where balancing is applied.
     */
    std::vector<mockturtle::node<Ntk>> fo_nodes{};
    /**
     * A collection of inverter nodes that are set to be deleted.
     */
    std::vector<mockturtle::node<Ntk>> x_inv_ao{};
    /**
     * A collection of inverter nodes that are set to be moved to fan-in position.
     */
    std::vector<mockturtle::node<Ntk>> m_inv_ao{};
    /**
     * These are and/or nodes where balancing is applied.
     */
    std::vector<mockturtle::node<Ntk>> and_or_nodes{};
    /**
     * Collection of nodes where the children have not been placed yet.
     */
    std::unordered_map<mockturtle::node<Ntk>, std::array<mockturtle::node<Ntk>, 3>> unplaced_nodes{};
    /**
     * Collection of nodes where primary outputs need to be preserved.
     */
    std::vector<mockturtle::node<Ntk>> preserved_po{};
    /**
     * An indicator to check if optimizations can be applied or not.
     */
    bool rerun{true};
    /**
     * The operation mode of inverter substitution.
     * */
    detail::operation_mode mode;

    template <typename NtkSrc>
    auto gather_fanin_signals(const typename Ntk::node&                              n,
                              mockturtle::node_map<mockturtle::signal<Ntk>, NtkSrc>& old2new)
    {
        std::vector<typename Ntk::signal> children{};

        // store affected nodes fo
        if (fo_ntk.is_fanout(n) && (mode == operation_mode::FO_ONLY || mode == operation_mode::ALL_NODES))
        {
            const auto fanout_inv    = fanouts(fo_ntk, n);
            const auto do_substitute = std::all_of(fanout_inv.cbegin(), fanout_inv.cend(),
                                                   [this](const auto& fo_node) { return fo_ntk.is_inv(fo_node); });

            if (do_substitute && fanout_inv.size() > 1)
            {
                x_inv_fo.emplace_back(fanout_inv[1]);
                fo_nodes.emplace_back(n);
                m_inv_fo.emplace_back(fanout_inv[0]);
            }
        }
        // store affected nodes and/or
        if (fo_ntk.is_inv(n) && !fo_ntk.is_po(n) && (mode == operation_mode::AND_OR_ONLY || mode == operation_mode::ALL_NODES))
        {
            const auto inv_fanout = fanouts(fo_ntk, n);
            assert(inv_fanout.size() == 1);
            if ((fo_ntk.is_and(inv_fanout[0]) || fo_ntk.is_or(inv_fanout[0])) &&
                std::find(and_or_nodes.cbegin(), and_or_nodes.cend(), inv_fanout[0]) == and_or_nodes.cend())
            {
                const auto fc            = fanins(fo_ntk, inv_fanout[0]);
                const auto fanin_inv     = fc.fanin_nodes;
                const auto do_substitute = std::all_of(fanin_inv.cbegin(), fanin_inv.cend(),
                                                       [this](const auto& fi_node) { return fo_ntk.is_inv(fi_node); });

                if (do_substitute && fanin_inv.size() > 1)
                {
                    x_inv_ao.emplace_back(fanin_inv[1]);
                    and_or_nodes.emplace_back(inv_fanout[0]);
                    m_inv_ao.emplace_back(fanin_inv[0]);
                }
            }
        }

        if (mode == operation_mode::AND_OR_ONLY || mode == operation_mode::ALL_NODES)
        {
            /*
             * If n = m_inv_ao: use children of and_or_nodes
             * If n = and_or_nodes: use m_inv_ao as child
             * If n = x_inv_ao: no children, because this node will not be in the network
             * */
            mockturtle::node<Ntk> fn{};
            // If n = m_inv_ao: use children of and_or_nodes
            for (std::size_t i = 0; i < m_inv_ao.size(); ++i)
            {
                if (m_inv_ao[i] == n)
                {
                    const auto and_node = and_or_nodes[i];
                    ntk.foreach_fanin(and_node,
                                      [this, &old2new, &fn, &children](const auto& f)
                                      {
                                          const auto fis = fanins(fo_ntk, f);
                                          assert(fis.fanin_nodes.size() == 1);
                                          fn                    = fis.fanin_nodes[0];
                                          const auto tgt_signal = old2new[fn];
                                          // in this case one child is not placed yet
                                          if (tgt_signal == 0)
                                          {
                                              std::array<mockturtle::node<Ntk>, 3> nodes = {
                                                  m_inv_ao.back(), and_or_nodes.back(), x_inv_ao.back()};
                                              unplaced_nodes[fn] = nodes;
                                          }
                                          else
                                          {
                                              children.emplace_back(tgt_signal);
                                          }
                                      });
                    return children;
                }
            }
            // If n = and_or_nodes: use m_inv_ao as child
            for (std::size_t i = 0; i < and_or_nodes.size(); ++i)
            {
                if (and_or_nodes[i] == n)
                {
                    fn                    = m_inv_ao[i];
                    const auto tgt_signal = old2new[fn];
                    children.emplace_back(tgt_signal);
                    return children;
                }
            }
        }

        // compute children of affected nodes
        ntk.foreach_fanin(n,
                          [this, &old2new, &children](const auto& f)
                          {
                              auto fn = ntk.get_node(f);

                              if (mode == operation_mode::FO_ONLY || mode == operation_mode::ALL_NODES)
                              {
                                  for (std::size_t i = 0; i < fo_nodes.size(); ++i)
                                  {
                                      if (x_inv_fo[i] == fn)
                                      {
                                          // const auto fis = fanins(fo_ntk, fn);
                                          fn = m_inv_fo[i];
                                          break;
                                      }
                                  }
                              }

                              const auto tgt_signal = old2new[fn];
                              children.emplace_back(tgt_signal);
                          });

        return children;
    }

    template <typename NtkSrc>
    void connect_children_to_gates(Ntk& ntk_dest, mockturtle::node_map<mockturtle::signal<Ntk>, NtkSrc>& old2new,
                                   const mockturtle::node<Ntk>& g, const std::vector<typename Ntk::signal>& children)
    {
        // map all affected nodes for fo
        if constexpr (fiction::has_is_inv_v<TopoNtkSrc> && mockturtle::has_create_buf_v<Ntk>)
        {
            if (ntk.is_inv(g) && std::find(m_inv_fo.cbegin(), m_inv_fo.cend(), g) != m_inv_fo.cend())
            {
                old2new[g] = ntk_dest.create_buf(children[0]);
                set_level_and_rank(ntk_dest, old2new, g);
                return;  // keep looping
            }
            if (const auto po_it = std::find(x_inv_fo.cbegin(), x_inv_fo.cend(), g);
                ntk.is_inv(g) && po_it != x_inv_fo.cend())
            {
                if (ntk.is_po(g))
                {
                    // Preserve Outputs
                    const auto index = po_it - x_inv_fo.cbegin();
                    preserved_po.push_back(m_inv_fo[static_cast<std::vector<uint64_t>::size_type>(index)]);
                }
                return;  // keep looping
            }
        }
        if constexpr (fiction::has_is_buf_v<TopoNtkSrc> && mockturtle::has_create_not_v<Ntk>)
        {
            if (ntk.is_buf(g) && std::find(fo_nodes.cbegin(), fo_nodes.cend(), g) != fo_nodes.cend())
            {
                old2new[g] = ntk_dest.create_not(children[0]);
                set_level_and_rank(ntk_dest, old2new, g);
                return;  // keep looping
            }
        }

        // map all affected nodes for and/or
        if constexpr (fiction::has_is_inv_v<TopoNtkSrc> && mockturtle::has_is_and_v<TopoNtkSrc> &&
                      mockturtle::has_is_or_v<TopoNtkSrc> && mockturtle::has_create_and_v<Ntk> &&
                      mockturtle::has_create_or_v<Ntk>)
        {
            if (const auto po_it = std::find(m_inv_ao.cbegin(), m_inv_ao.cend(), g);
                ntk.is_inv(g) && po_it != m_inv_ao.cend())
            {
                const auto index = po_it - m_inv_ao.cbegin();
                // differentiate between and/or nodes
                if (ntk.is_and(and_or_nodes[static_cast<std::vector<uint64_t>::size_type>(index)]))
                {
                    old2new[g] = ntk_dest.create_or(children[0], children[1]);
                    set_level_and_rank(ntk_dest, old2new, g);
                }
                else if (ntk.is_or(and_or_nodes[static_cast<std::vector<uint64_t>::size_type>(index)]))
                {
                    old2new[g] = ntk_dest.create_and(children[0], children[1]);
                    set_level_and_rank(ntk_dest, old2new, g);
                }
                return;  // keep looping
            }
            if (ntk.is_inv(g) && std::find(x_inv_ao.cbegin(), x_inv_ao.cend(), g) != x_inv_ao.cend())
            {
                // inverter can be safely deleted, since it has the and/ornode as fanout
                return;  // keep looping
            }
        }
        if constexpr (mockturtle::has_is_and_v<TopoNtkSrc> && mockturtle::has_is_or_v<TopoNtkSrc> &&
                      mockturtle::has_create_not_v<Ntk>)
        {
            if ((ntk.is_and(g) || ntk.is_or(g)) &&
                std::find(and_or_nodes.cbegin(), and_or_nodes.cend(), g) != and_or_nodes.cend())
            {
                old2new[g] = ntk_dest.create_not(children[0]);
                set_level_and_rank(ntk_dest, old2new, g);
                return;  // keep looping
            }
        }

        // map all unaffected nodes
        if (connect_children_to_gates_unaffected(ntk, ntk_dest, old2new, g, children))
        {
            return;  // keep looping
        }
    }
};

}  // namespace detail

/**
 * Applies an inverter substitution optimization on a logic network.
 *
 * Substitutes redundant inverters at fanout points with a single inverter at the driving node’s input.
 * Optionally, replaces AND or OR gates with inverters on their inputs by equivalent dual gates with
 * a single inverter at their output. The goal is to reduce the overall number of inverters in the network.
 *
 * @tparam Ntk Logic network type.
 * @param ntk Input logic network.
 * @param mode Substitution mode specifying the types of structures to target (fanout only, AND/OR only, or all).
 * @return A logically equivalent network with a reduced number of inverters.
 */

template <typename Ntk>
Ntk inverter_substitution(const Ntk& ntk, detail::operation_mode mode = detail::operation_mode::FO_ONLY)
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
    static_assert(fiction::has_is_inv_v<Ntk>, "Ntk does not implement the is_inv function");

    static_assert(has_is_fanout_v<Ntk>, "Ntk does not implement the has_is_fanout function");

    assert(ntk.is_combinational() && "Network has to be combinational");

    bool run    = true;
    auto result = ntk;
    while (run)
    {
        detail::inverter_substitution_impl<Ntk> p{result, mode};
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
