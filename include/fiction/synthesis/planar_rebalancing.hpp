/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Removes buffer chains from a planar ranked network and re-inserts buffers on as-soon-as-possible levels.
 * @author Benjamin Hien (hibenj)
 */

#pragma once

#include "fiction/networks/name_utils.hpp"
#include "fiction/networks/network_utils.hpp"
#include "fiction/utils/progress.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/views/fanout_view.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fiction::synthesis
{

/**
 * @brief Parameters for the planar rebalancing algorithm.
 */
struct planar_rebalancing_params
{
    /**
     * @brief Receives completed work and the phase total.
     */
    utils::progress_callback on_progress{};
};

namespace detail
{

/**
 * @brief Implementation of the planar rebalancing algorithm.
 *
 * @tparam Ntk Ranked network type.
 */
template <typename Ntk>
class planar_rebalancing_impl
{
  public:
    /**
     * @brief Creates the implementation.
     *
     * @param src Source network.
     * @param p Parameters.
     */
    planar_rebalancing_impl(const Ntk& src, const planar_rebalancing_params& p) :
            ntk{src},
            fanout_ntk{src},
            ps{p},
            output_count(src.size(), 0u)
    {
        ntk.foreach_po([this](const auto& po) { ++output_count[ntk.get_node(po)]; });
    }

    /**
     * @brief Runs the algorithm.
     *
     * @return Balanced copy of the source network without redundant buffers, with the same rank order.
     * @throws std::invalid_argument If a gate has only constant fanins.
     */
    [[nodiscard]] Ntk run()
    {
        reject_constant_gates();

        utils::progress_reporter progress{ps.on_progress, "rebalancing", 3};

        auto  init     = networks::initialize_copy_network_with_virtual_pis(ntk);
        auto& stripped = init.first;
        auto& old2new  = init.second;

        remove_buffer_chains(stripped, old2new);
        progress.advance();

        keep_input_order(stripped, old2new);
        progress.advance();

        auto balanced = insert_buffers(stripped, old2new);
        progress.advance();

        return balanced;
    }

  private:
    /**
     * @brief An edge of the stripped network while buffers are re-inserted along it.
     */
    struct edge
    {
        /**
         * @brief Source node in the stripped network.
         */
        mockturtle::node<Ntk> source;
        /**
         * @brief Target node in the stripped network.
         *
         * Equal to `source` for an edge that leads to a primary output.
         */
        mockturtle::node<Ntk> target;
        /**
         * @brief Buffers still to insert before the target is reached.
         */
        uint32_t buffers;
    };
    /**
     * @brief Whether a node is a buffer that drives exactly one consumer and can be removed.
     *
     * @param n Node of the source network.
     * @return `true` iff `n` is a chain buffer.
     */
    [[nodiscard]] bool is_chain_buffer(const mockturtle::node<Ntk> n) const noexcept
    {
        return ntk.is_buf(n) && fanout_ntk.fanout_size(n) == 1;
    }
    /**
     * @brief Copies every node of the source network except chain buffers into `stripped`.
     *
     * A chain buffer maps to the signal of its fanin, so that its consumer connects to the node the buffer delayed.
     *
     * @param stripped Destination network; holds the copied primary inputs on entry.
     * @param old2new Map from source nodes to destination signals; completed here.
     */
    void remove_buffer_chains(Ntk& stripped, mockturtle::node_map<mockturtle::signal<Ntk>, Ntk>& old2new) const
    {
        ntk.foreach_gate(
            [this, &stripped, &old2new](const auto& n)
            {
                if (is_chain_buffer(n))
                {
                    old2new[n] = old2new[networks::fanins(ntk, n).fanin_nodes.front()];
                    return;
                }

                std::vector<mockturtle::signal<Ntk>> children{};
                children.reserve(ntk.fanin_size(n));
                ntk.foreach_fanin(n, [this, &old2new, &children](const auto& f)
                                  { children.push_back(old2new[ntk.get_node(f)]); });

                old2new[n] = stripped.clone_node(ntk, n, children);
            });

        ntk.foreach_po(
            [this, &stripped, &old2new](const auto& po)
            {
                const auto sig = old2new[ntk.get_node(po)];
                stripped.create_po(ntk.is_complemented(po) ? stripped.create_not(sig) : sig);
            });

        networks::restore_names(ntk, stripped, old2new);
    }
    /**
     * @brief Recomputes the ranks of the stripped network and puts its primary inputs in the order of the source
     * network.
     *
     * Only the input order matters for the sweep that re-inserts the buffers; the order within the other levels is
     * derived from the consumer order of the source network as the sweep goes.
     *
     * @param stripped Stripped network; its ranks are recomputed.
     * @param old2new Map from source nodes to destination signals.
     */
    void keep_input_order(Ntk& stripped, const mockturtle::node_map<mockturtle::signal<Ntk>, Ntk>& old2new) const
    {
        stripped.update_ranks();

        std::vector<mockturtle::node<Ntk>> input_order{};
        input_order.reserve(ntk.rank_width(0));
        for (const auto& pi : ntk.get_ranks(0))
        {
            input_order.push_back(stripped.get_node(old2new[pi]));
        }
        stripped.set_ranks(0, input_order);
    }
    /**
     * @brief Consumers of a node in the source network, ordered by the rank position of the node's direct fanouts, with
     * chain buffers skipped.
     *
     * There is one entry per edge: a gate that reads the node twice appears twice, and a consumer equal to the node
     * itself stands for one primary output the node drives; direct outputs come first.
     *
     * @param n Node of the source network.
     * @return Consumers of `n` in rank order of the source network.
     */
    [[nodiscard]] std::vector<mockturtle::node<Ntk>> ordered_consumers(const mockturtle::node<Ntk> n) const
    {
        // ordered by the rank position of the direct fanout; a direct output has no position and comes first
        std::vector<std::pair<int64_t, mockturtle::node<Ntk>>> consumers{};

        fanout_ntk.foreach_fanout(n,
                                  [this, &consumers, n](const auto& fo)
                                  {
                                      auto c = fo;

                                      while (is_chain_buffer(c) && output_count[c] == 0)
                                      {
                                          c = networks::fanouts(fanout_ntk, c).front();
                                      }

                                      // one edge per fanin of the direct fanout that reads `n`; the fanout view
                                      // lists a consumer once however often it reads the node
                                      std::size_t edges = 0;
                                      ntk.foreach_fanin(fo,
                                                        [this, n, &edges](const auto& f)
                                                        {
                                                            if (ntk.get_node(f) == n)
                                                            {
                                                                ++edges;
                                                            }
                                                        });

                                      // ordered by the direct fanout, which sits in the level above `n`; a buffer
                                      // chain that ends in a primary output stands for that output
                                      for (std::size_t i = 0; i < edges; ++i)
                                      {
                                          consumers.emplace_back(static_cast<int64_t>(ntk.rank_position(fo)),
                                                                 is_chain_buffer(c) ? n : c);
                                      }
                                  });

        // one edge per primary output the node drives
        for (uint32_t i = 0; i < output_count[n]; ++i)
        {
            consumers.emplace_back(-1, n);
        }

        std::stable_sort(consumers.begin(), consumers.end(),
                         [](const auto& a, const auto& b) { return a.first < b.first; });

        std::vector<mockturtle::node<Ntk>> result{};
        result.reserve(consumers.size());
        for (const auto& [pos, c] : consumers)
        {
            result.push_back(c);
        }

        return result;
    }
    /**
     * @brief Consumers of every stripped node in rank order, derived from the source network.
     */
    using consumer_map = std::unordered_map<mockturtle::node<Ntk>, std::vector<mockturtle::node<Ntk>>>;
    /**
     * @brief Map from stripped nodes to signals of the network under construction.
     */
    using signal_map = mockturtle::node_map<mockturtle::signal<Ntk>, Ntk>;
    /**
     * @brief Collects the consumers of every stripped node in the rank order of the source network.
     *
     * @param stripped Stripped network with final ranks.
     * @param old2new Map from source nodes to signals of the stripped network.
     * @return Consumers per stripped node.
     */
    [[nodiscard]] consumer_map stripped_consumers(const Ntk& stripped, const signal_map& old2new) const
    {
        consumer_map consumers{};

        ntk.foreach_node(
            [this, &stripped, &old2new, &consumers](const auto& n)
            {
                if (ntk.is_constant(n) || is_chain_buffer(n))
                {
                    return;
                }

                std::vector<mockturtle::node<Ntk>> cs{};
                for (const auto& c : ordered_consumers(n))
                {
                    cs.push_back(stripped.get_node(old2new[c]));
                }

                consumers.emplace(stripped.get_node(old2new[n]), std::move(cs));
            });

        return consumers;
    }
    /**
     * @brief Appends the outgoing edges of a placed node to the edge list of its level, in consumer order.
     *
     * A primary output of the node is an edge from the node to itself that reaches up to the output level.
     *
     * @param n Placed node of the stripped network.
     * @param stripped Stripped network with final ranks.
     * @param consumers Consumers per stripped node.
     * @param po_level Level of the primary outputs in the result.
     * @param level_edges Edge list to append to.
     */
    static void append_out_edges(const mockturtle::node<Ntk> n, const Ntk& stripped, const consumer_map& consumers,
                                 const uint32_t po_level, std::vector<edge>& level_edges)
    {
        const auto it = consumers.find(n);

        // a primary input that drives nothing has no edges
        if (it == consumers.end())
        {
            return;
        }

        for (const auto& c : it->second)
        {
            if (c == n)
            {
                if (po_level > stripped.level(n))
                {
                    level_edges.push_back({n, n, po_level - stripped.level(n) - 1});
                }
            }
            else
            {
                level_edges.push_back({n, c, stripped.level(c) - stripped.level(n) - 1});
            }
        }
    }
    /**
     * @brief Places one level of the result: edges that span further get a buffer, one per source and level, shared by
     * all consecutive edges of that source; a node whose fanin edges all end on this level is cloned.
     *
     * The map from stripped nodes to signals is updated once the level is complete, so that every clone reads the
     * signals of the level below.
     *
     * @param current Edges that enter this level, in rank order.
     * @param stripped Stripped network with final ranks.
     * @param consumers Consumers per stripped node.
     * @param po_level Level of the primary outputs in the result.
     * @param balanced Network under construction.
     * @param new2bal Map from stripped nodes to their signals in `balanced`; updated.
     * @return Edges that enter the next level.
     * @throws std::runtime_error If the fanin edges of a node are not consecutive, i.e., the input is not planar.
     */
    [[nodiscard]] static std::vector<edge> place_level(const std::vector<edge>& current, const Ntk& stripped,
                                                       const consumer_map& consumers, const uint32_t po_level,
                                                       Ntk& balanced, signal_map& new2bal)
    {
        std::vector<edge>     next{};
        mockturtle::node<Ntk> last_source{};
        bool                  have_last = false;

        std::vector<std::pair<mockturtle::node<Ntk>, mockturtle::signal<Ntk>>> moved{};

        std::size_t i = 0;
        while (i < current.size())
        {
            const auto& e = current[i];

            if (e.buffers > 0 || e.source == e.target)
            {
                // one buffer per source and level, shared by all its edges
                if (!have_last || e.source != last_source)
                {
                    moved.emplace_back(e.source, balanced.create_buf(new2bal[e.source]));
                    last_source = e.source;
                    have_last   = true;
                }

                if (e.buffers > 0)
                {
                    next.push_back({e.source, e.target, e.buffers - 1});
                }

                ++i;
                continue;
            }

            // all fanin edges of the target are consecutive in a planar order and are consumed together
            const auto target = e.target;
            const auto fanins = non_constant_fanins(stripped, target);

            if (!consecutive_fanin_edges(current, i, fanins, target))
            {
                throw std::runtime_error("The fanin edges of a node are not consecutive; the network is not planar");
            }

            i += fanins;

            std::vector<mockturtle::signal<Ntk>> children{};
            children.reserve(stripped.fanin_size(target));
            stripped.foreach_fanin(target, [&stripped, &new2bal, &children](const auto& f)
                                   { children.push_back(new2bal[stripped.get_node(f)]); });

            new2bal[target] = balanced.clone_node(stripped, target, children);

            append_out_edges(target, stripped, consumers, po_level, next);

            // a placed node separates the edges of a source: the next one gets its own buffer
            have_last = false;
        }

        for (const auto& [n, sig] : moved)
        {
            new2bal[n] = sig;
        }

        return next;
    }
    /**
     * @brief Re-inserts buffers into the stripped network so that every edge spans exactly one level and all primary
     * outputs sit on the top level.
     *
     * Edges are swept level by level; consecutive edges from the same source share one buffer per level, so a fanout
     * splits right above its targets. The sweep starts from the primary inputs in their source order and appends the
     * edges of every placed node in the consumer order of the source network, which reproduces the planar embedding of
     * the source and keeps the result planar.
     *
     * @param stripped Stripped network with final ranks.
     * @param old2new Map from source nodes to signals of the stripped network.
     * @return Balanced network.
     */
    [[nodiscard]] Ntk insert_buffers(const Ntk& stripped, const signal_map& old2new) const
    {
        const auto consumers = stripped_consumers(stripped, old2new);

        auto  init     = networks::initialize_copy_network_with_virtual_pis(stripped);
        auto& balanced = init.first;
        auto& new2bal  = init.second;

        // level 0 of the result keeps the primary input order; captured before the sweep moves the map to buffers
        std::vector<mockturtle::node<Ntk>> input_order{};
        input_order.reserve(stripped.rank_width(0));
        for (const auto& pi : stripped.get_ranks(0))
        {
            input_order.push_back(balanced.get_node(new2bal[pi]));
        }

        const auto po_level = max_po_level(stripped);

        std::vector<edge> current{};
        stripped.foreach_pi([&stripped, &consumers, po_level, &current](const auto& pi)
                            { append_out_edges(pi, stripped, consumers, po_level, current); });

        while (!current.empty())
        {
            current = place_level(current, stripped, consumers, po_level, balanced, new2bal);
        }

        stripped.foreach_po(
            [&stripped, &balanced, &new2bal](const auto& po)
            {
                const auto sig = new2bal[stripped.get_node(po)];
                balanced.create_po(stripped.is_complemented(po) ? balanced.create_not(sig) : sig);
            });

        networks::restore_names(stripped, balanced, new2bal);

        balanced.update_ranks();
        balanced.set_ranks(0, input_order);

        return std::move(balanced);
    }
    /**
     * @brief Level of the highest primary output driver, constants excluded.
     *
     * @param net Ranked network.
     * @return Maximum level among the primary output nodes, 0 if there are none.
     */
    [[nodiscard]] static uint32_t max_po_level(const Ntk& net)
    {
        uint32_t level = 0;
        net.foreach_po(
            [&net, &level](const auto& po)
            {
                // constants have no level
                if (const auto n = net.get_node(po); !net.is_constant(n))
                {
                    level = std::max(level, net.level(n));
                }
            });

        return level;
    }
    /**
     * @brief Checks that the `fanins` edges from position `first` on all lead to `target` on this level, i.e., that the
     * node's fanin edges are consecutive as in a planar order.
     *
     * @param current Edges of the level.
     * @param first Position of the first edge of `target`.
     * @param fanins Number of non-constant fanins of `target`; zero is never consecutive.
     * @param target Node whose edges are checked.
     * @return `true` iff the edges are consecutive.
     */
    [[nodiscard]] static bool consecutive_fanin_edges(const std::vector<edge>& current, const std::size_t first,
                                                      const std::size_t fanins, const mockturtle::node<Ntk> target)
    {
        if (fanins == 0 || first + fanins > current.size())
        {
            return false;
        }

        return std::all_of(current.cbegin() + static_cast<std::ptrdiff_t>(first),
                           current.cbegin() + static_cast<std::ptrdiff_t>(first + fanins), [target](const edge& e)
                           { return e.target == target && e.buffers == 0 && e.source != e.target; });
    }
    /**
     * @brief Number of non-constant fanins of a node.
     *
     * @param net Network.
     * @param n Node.
     * @return Number of fanins that are not constants.
     */
    [[nodiscard]] static std::size_t non_constant_fanins(const Ntk& net, const mockturtle::node<Ntk> n)
    {
        std::size_t count = 0;
        net.foreach_fanin(n,
                          [&net, &count](const auto& f)
                          {
                              if (!net.is_constant(net.get_node(f)))
                              {
                                  ++count;
                              }
                          });

        return count;
    }
    /**
     * @brief Rejects gates whose fanins are all constants.
     *
     * The sweep reaches a gate only along edges that start at the primary inputs, so such a gate has no level to be
     * placed on; constant propagation folds it before rebalancing.
     *
     * @throws std::invalid_argument If a gate has only constant fanins.
     */
    void reject_constant_gates() const
    {
        ntk.foreach_gate(
            [this](const auto& n)
            {
                if (non_constant_fanins(ntk, n) == 0)
                {
                    throw std::invalid_argument(
                        "A gate has only constant fanins; propagate constants before planar rebalancing");
                }
            });
    }
    /**
     * @brief Source network.
     */
    const Ntk& ntk;
    /**
     * @brief Fanout view of the source network.
     */
    const mockturtle::fanout_view<Ntk> fanout_ntk;
    /**
     * @brief Parameters.
     */
    const planar_rebalancing_params& ps;
    /**
     * @brief Number of primary outputs every source node drives directly.
     */
    std::vector<uint32_t> output_count;
};

}  // namespace detail

/**
 * @brief Removes every buffer that drives a single consumer from a ranked, planar network and re-inserts buffers on the
 * as-soon-as-possible levels of the remaining network: every gate and fanout node sits on the lowest level its fanins
 * allow, and all primary outputs sit on the level of the deepest one.
 *
 * Buffers that lead from one source to several consumers are shared level by level, so the fanout splits right above
 * its targets. The depth is the smallest the network allows, and the buffer count is the smallest for these levels. A
 * gate is never delayed to save buffers, so other balanced level assignments can need fewer buffers. The rank order of
 * the input is kept, so a planar input yields a planar output; fanout nodes with more than one consumer are kept, so a
 * fanout-substituted input yields a fanout-substituted output.
 *
 * This is the post-processing step of `planar_fanout_substitution`, which pads every level to a uniform depth.
 *
 * @tparam Ntk Ranked network type (see `mutable_rank_view`) that supports `create_buf` and `is_buf`.
 * @param ntk Ranked, planar input network.
 * @param ps Parameters.
 * @return A balanced, planar network with unified outputs.
 * @throws std::invalid_argument If a gate has only constant fanins; such a gate cannot be reached from the inputs.
 * @throws std::runtime_error If the input is not planar.
 */
template <typename Ntk>
[[nodiscard]] Ntk planar_rebalancing(const Ntk& ntk, const planar_rebalancing_params& ps = {})
{
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
    static_assert(mockturtle::has_is_buf_v<Ntk>, "Ntk does not implement the is_buf method");
    static_assert(mockturtle::has_create_buf_v<Ntk>, "Ntk does not implement the create_buf method");
    static_assert(mockturtle::has_clone_node_v<Ntk>, "Ntk does not implement the clone_node method");
    static_assert(mockturtle::has_fanout_size_v<Ntk>, "Ntk does not implement the fanout_size method");
    static_assert(mockturtle::has_foreach_fanin_v<Ntk>, "Ntk does not implement the foreach_fanin method");
    static_assert(mockturtle::has_foreach_po_v<Ntk>, "Ntk does not implement the foreach_po method");
    static_assert(mockturtle::has_rank_position_v<Ntk>, "Ntk does not implement the rank_position method");
    static_assert(mockturtle::has_level_v<Ntk>, "Ntk does not implement the level method");

    detail::planar_rebalancing_impl<Ntk> p{ntk, ps};

    return p.run();
}

}  // namespace fiction::synthesis
