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
 * @brief Removes buffer chains from a planar ranked network and re-inserts the minimum that keeps it balanced.
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
#include <numeric>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fiction::synthesis
{

/**
 * Parameters for the planar rebalancing algorithm.
 */
struct planar_rebalancing_params
{
    /**
     * Receives completed work and the phase total.
     */
    utils::progress_callback on_progress{};
};

namespace detail
{

/**
 * Implementation of the planar rebalancing algorithm.
 *
 * @tparam Ntk Ranked network type.
 */
template <typename Ntk>
class planar_rebalancing_impl
{
  public:
    /**
     * Creates the implementation.
     *
     * @param src Source network.
     * @param p Parameters.
     */
    planar_rebalancing_impl(const Ntk& src, const planar_rebalancing_params& p) : ntk{src}, fanout_ntk{src}, ps{p} {}

    /**
     * Runs the algorithm.
     *
     * @return Balanced copy of the source network without redundant buffers, with the same rank order.
     */
    [[nodiscard]] Ntk run()
    {
        utils::progress_reporter progress{ps.on_progress, "rebalancing", 3};

        auto  init     = networks::initialize_copy_network_with_virtual_pis(ntk);
        auto& stripped = init.first;
        auto& old2new  = init.second;

        remove_buffer_chains(stripped, old2new);
        progress.advance();

        reorder_ranks(stripped, old2new);
        progress.advance();

        auto balanced = insert_buffers(stripped, old2new);
        progress.advance();

        return balanced;
    }

  private:
    /**
     * An edge of the stripped network while buffers are re-inserted along it.
     */
    struct edge
    {
        /**
         * Source node in the stripped network.
         */
        mockturtle::node<Ntk> source;
        /**
         * Target node in the stripped network. Equal to `source` for an edge that leads to a primary output.
         */
        mockturtle::node<Ntk> target;
        /**
         * Buffers still to insert before the target is reached.
         */
        uint32_t buffers;
    };
    /**
     * Whether a node is a buffer that drives exactly one consumer and can be removed.
     *
     * @param n Node of the source network.
     * @return `true` iff `n` is a chain buffer.
     */
    [[nodiscard]] bool is_chain_buffer(const mockturtle::node<Ntk> n) const noexcept
    {
        return ntk.is_buf(n) && fanout_ntk.fanout_size(n) == 1;
    }
    /**
     * Copies every node of the source network except chain buffers into `stripped`. A chain buffer maps to the
     * signal of its fanin, so that its consumer connects to the node the buffer delayed.
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
     * Recomputes the ranks of the stripped network and orders every rank by the barycenter of its fanins, with the
     * source rank position as the tie-breaker. Removing buffer chains merges nodes of different source levels into one
     * rank; the position of a deeper node is taken from the node above its former buffer chain, so that nodes of a
     * rank keep their relative order from the source network.
     *
     * @param stripped Stripped network; its ranks are rewritten.
     * @param old2new Map from source nodes to destination signals.
     */
    void reorder_ranks(Ntk& stripped, const mockturtle::node_map<mockturtle::signal<Ntk>, Ntk>& old2new) const
    {
        stripped.update_ranks();

        // source node of every stripped node
        std::unordered_map<mockturtle::node<Ntk>, mockturtle::node<Ntk>> new2old{};
        new2old.reserve(ntk.size());
        ntk.foreach_node(
            [this, &stripped, &old2new, &new2old](const auto& n)
            {
                if (!is_chain_buffer(n))
                {
                    new2old.emplace(stripped.get_node(old2new[n]), n);
                }
            });

        // rank 0 keeps the primary input order of the source
        std::vector<mockturtle::node<Ntk>> rank0{};
        rank0.reserve(ntk.rank_width(0));
        for (const auto& pi : ntk.get_ranks(0))
        {
            rank0.push_back(stripped.get_node(old2new[pi]));
        }
        stripped.set_ranks(0, rank0);

        for (uint32_t level = 1; level <= stripped.depth(); ++level)
        {
            auto       nodes   = stripped.get_ranks(level);
            const auto centers = networks::barycenters(stripped, nodes);

            std::vector<std::size_t> order(nodes.size());
            std::iota(order.begin(), order.end(), 0u);

            std::stable_sort(order.begin(), order.end(),
                             [this, &centers, &nodes, &new2old](const std::size_t a, const std::size_t b)
                             {
                                 if (centers[a] != centers[b])
                                 {
                                     return centers[a] < centers[b];
                                 }

                                 return source_position(new2old.at(nodes[a]), new2old.at(nodes[b]));
                             });

            std::vector<mockturtle::node<Ntk>> sorted{};
            sorted.reserve(nodes.size());
            for (const auto i : order)
            {
                sorted.push_back(nodes[i]);
            }

            stripped.set_ranks(level, sorted);
        }
    }
    /**
     * Compares two source nodes by their rank position after lifting the deeper one to the level of the shallower one
     * along its chain of buffers.
     *
     * @param a First source node.
     * @param b Second source node.
     * @return `true` iff `a` comes before `b`.
     */
    [[nodiscard]] bool source_position(mockturtle::node<Ntk> a, mockturtle::node<Ntk> b) const
    {
        const auto lift = [this](mockturtle::node<Ntk> n, const uint32_t target_level)
        {
            while (ntk.level(n) > target_level)
            {
                const auto fanin_nodes = networks::fanins(ntk, n).fanin_nodes;

                if (fanin_nodes.empty())
                {
                    break;
                }

                n = fanin_nodes.front();
            }

            return n;
        };

        const auto target_level = std::min(ntk.level(a), ntk.level(b));

        a = lift(a, target_level);
        b = lift(b, target_level);

        if (ntk.rank_position(a) != ntk.rank_position(b))
        {
            return ntk.rank_position(a) < ntk.rank_position(b);
        }

        return a < b;
    }
    /**
     * Consumers of a node in the source network, ordered by the rank position of the node's direct fanouts, with
     * chain buffers skipped. A consumer equal to the node itself stands for a primary output; one that the node drives
     * directly comes first.
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

                                      while (is_chain_buffer(c) && !ntk.is_po(c))
                                      {
                                          c = networks::fanouts(fanout_ntk, c).front();
                                      }

                                      // ordered by the direct fanout, which sits in the rank above `n`; a buffer
                                      // chain that ends in a primary output stands for that output
                                      consumers.emplace_back(static_cast<int64_t>(ntk.rank_position(fo)),
                                                             is_chain_buffer(c) ? n : c);
                                  });

        if (ntk.is_po(n))
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
     * Consumers of every stripped node in rank order, derived from the source network.
     */
    using consumer_map = std::unordered_map<mockturtle::node<Ntk>, std::vector<mockturtle::node<Ntk>>>;
    /**
     * Map from stripped nodes to signals of the network under construction.
     */
    using signal_map = mockturtle::node_map<mockturtle::signal<Ntk>, Ntk>;
    /**
     * Collects the consumers of every stripped node in the rank order of the source network.
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
     * Appends the outgoing edges of a placed node to the edge list of its level, in consumer order. A primary output
     * of the node is an edge from the node to itself that reaches up to the output level.
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
     * Places one level of the result: edges that span further get a buffer, one per source and level, shared by all
     * consecutive edges of that source; a node whose fanin edges all end on this level is cloned. The map from
     * stripped nodes to signals is updated once the level is complete, so that every clone reads the signals of the
     * level below.
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

        for (std::size_t i = 0; i < current.size(); ++i)
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

                continue;
            }

            // all fanin edges of the target are consecutive in a planar order; skip its other ones
            const auto target = e.target;
            const auto fanins = non_constant_fanins(stripped, target);

            if (i + fanins > current.size())
            {
                throw std::runtime_error("The fanin edges of a node are not consecutive; the network is not planar");
            }

            i += fanins - 1;

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
     * Re-inserts buffers into the stripped network so that every edge spans exactly one level and all primary outputs
     * sit on the top level. Edges are swept level by level in rank order; consecutive edges from the same source share
     * one buffer per level, so a fanout splits right above its targets. The sweep follows the rank order of the
     * stripped network, which keeps the result planar.
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

        // rank 0 of the result keeps the primary input order; captured before the sweep moves the map to buffers
        std::vector<mockturtle::node<Ntk>> rank0{};
        rank0.reserve(stripped.rank_width(0));
        for (const auto& pi : stripped.get_ranks(0))
        {
            rank0.push_back(balanced.get_node(new2bal[pi]));
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
        balanced.set_ranks(0, rank0);

        return balanced;
    }
    /**
     * Level of the highest primary output driver, constants excluded.
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
     * Number of non-constant fanins of a node.
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
     * Source network.
     */
    const Ntk& ntk;
    /**
     * Fanout view of the source network.
     */
    const mockturtle::fanout_view<Ntk> fanout_ntk;
    /**
     * Parameters.
     */
    const planar_rebalancing_params& ps;
};

}  // namespace detail

/**
 * Removes every buffer that drives a single consumer from a ranked, planar network and re-inserts the minimum number
 * of buffers that makes all edges span exactly one level and puts all primary outputs on the top level. Buffers that
 * lead from one source to several consumers are shared level by level, so the fanout splits right above its targets.
 * The rank order of the input is kept, so a planar input yields a planar output; fanout nodes with more than one
 * consumer are kept, so a fanout-substituted input yields a fanout-substituted output.
 *
 * This is the post-processing step of `planar_fanout_substitution`, which pads every rank to a uniform depth.
 *
 * @tparam Ntk Ranked network type (see `mutable_rank_view`) that supports `create_buf` and `is_buf`.
 * @param ntk Ranked, planar input network.
 * @param ps Parameters.
 * @return A balanced, planar network with unified outputs and no redundant buffers.
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
