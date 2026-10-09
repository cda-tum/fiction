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
 * @brief Fanout substitution that keeps the ranks and the planarity of a ranked network.
 * @author Benjamin Hien (hibenj)
 */

#pragma once

#include "fiction/networks/name_utils.hpp"
#include "fiction/networks/network_utils.hpp"
#include "fiction/synthesis/network_balancing.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/progress.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/views/fanout_view.hpp>

#include <algorithm>
#include <cstdint>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

namespace fiction::synthesis
{

/**
 * Parameters for the planar fanout substitution algorithm.
 */
struct planar_fanout_substitution_params
{
    /**
     * Receives completed work and the phase total.
     */
    utils::progress_callback on_progress{};
    /**
     * Maximum output degree of each fanout node, at least 2. Every other node drives exactly one consumer.
     */
    uint32_t degree = 2u;
};

namespace detail
{

/**
 * Implementation of the planar fanout substitution algorithm.
 *
 * @tparam Ntk Ranked network type.
 */
template <typename Ntk>
class planar_fanout_substitution_impl
{
  public:
    /**
     * Creates the implementation.
     *
     * @param src Source network.
     * @param p Parameters.
     */
    planar_fanout_substitution_impl(const Ntk& src, const planar_fanout_substitution_params& p) :
            ntk{src},
            fanout_ntk{src},
            available_fanouts{src},
            ps{p}
    {}

    /**
     * Runs the algorithm.
     *
     * @return Fanout-substituted copy of the source network with the same ranks and no new crossings.
     */
    [[nodiscard]] Ntk run()
    {
        auto  init    = networks::initialize_copy_network_with_virtual_pis(ntk);
        auto& dest    = init.first;
        auto& old2new = init.second;

        // rank 0 of the result keeps the input order of the primary inputs
        std::vector<mockturtle::node<Ntk>> rank0{};
        rank0.reserve(ntk.rank_width(0));
        for (const auto& pi : ntk.get_ranks(0))
        {
            rank0.push_back(dest.get_node(old2new[pi]));
        }

        utils::progress_reporter progress{ps.on_progress, "substituting fanouts", ntk.depth() + 1};

        for (uint32_t r = 0; r <= ntk.depth(); ++r)
        {
            // every node of the rank is pushed down by the depth of the deepest fanout tree in the rank
            const auto pad = max_tree_depth(r);

            ntk.foreach_node_in_rank(r,
                                     [this, &dest, &old2new, pad](const auto& n)
                                     {
                                         if (ntk.is_constant(n))
                                         {
                                             return;
                                         }

                                         if (!ntk.is_ci(n))
                                         {
                                             old2new[n] = dest.clone_node(ntk, n, gather_children(dest, n, old2new));
                                         }

                                         substitute(dest, n, old2new, pad);
                                     });

            progress.advance();
        }

        ntk.foreach_po(
            [this, &dest, &old2new](const auto& po)
            {
                const auto n   = ntk.get_node(po);
                auto       sig = take_fanout(dest, n, old2new[n]);

                dest.create_po(ntk.is_complemented(po) ? dest.create_not(sig) : sig);
            });

        networks::restore_names(ntk, dest, old2new);

        dest.update_ranks();
        dest.set_ranks(0, rank0);

        return dest;
    }

  private:
    /**
     * Number of fanout nodes a node with the given number of outputs needs.
     *
     * @param fanouts Number of outputs.
     * @return Number of fanout nodes in its tree.
     */
    [[nodiscard]] uint32_t num_fanout_nodes(const uint32_t fanouts) const noexcept
    {
        if (fanouts <= 1)
        {
            return 0;
        }

        // every fanout node adds `degree - 1` outputs beyond the one it consumes
        const auto per_node = ps.degree - 1;

        return (fanouts - 1 + per_node - 1) / per_node;
    }
    /**
     * Level of the fanout node with the given index in a breadth-first fanout tree, the root's first node being 0.
     *
     * @param index Creation index of the fanout node.
     * @return Its level in the tree.
     */
    [[nodiscard]] uint32_t fanout_node_level(const uint32_t index) const noexcept
    {
        uint32_t level         = 0;
        uint64_t first_of_next = 1;

        while (index >= first_of_next)
        {
            ++level;
            first_of_next = (first_of_next * ps.degree) + 1;
        }

        return level;
    }
    /**
     * Depth of the fanout tree of a node with the given number of outputs.
     *
     * @param fanouts Number of outputs.
     * @return Number of levels the tree adds below the node.
     */
    [[nodiscard]] uint32_t tree_depth(const uint32_t fanouts) const noexcept
    {
        const auto num_nodes = num_fanout_nodes(fanouts);

        return num_nodes == 0 ? 0 : fanout_node_level(num_nodes - 1) + 1;
    }
    /**
     * Whether a node already is a fanout node within the degree limit and needs no tree.
     *
     * @param n Node.
     * @return `true` iff `n` is a proper fanout node.
     */
    [[nodiscard]] bool is_proper_fanout(const mockturtle::node<Ntk> n) const noexcept
    {
        if constexpr (has_is_fanout_v<Ntk>)
        {
            return ntk.is_fanout(n) && fanout_ntk.fanout_size(n) <= ps.degree;
        }
        else
        {
            return false;
        }
    }
    /**
     * Depth of the deepest fanout tree any node of a rank needs.
     *
     * @param r Rank.
     * @return Maximum tree depth in the rank.
     */
    [[nodiscard]] uint32_t max_tree_depth(const uint32_t r) const
    {
        uint32_t depth = 0;

        ntk.foreach_node_in_rank(r,
                                 [this, &depth](const auto& n)
                                 {
                                     if (!ntk.is_constant(n) && !is_proper_fanout(n))
                                     {
                                         depth = std::max(depth, tree_depth(fanout_ntk.fanout_size(n)));
                                     }
                                 });

        return depth;
    }
    /**
     * Collects the fanin signals of a node in the destination network, taking free fanout slots where needed.
     *
     * @param dest Destination network.
     * @param n Source node.
     * @param old2new Map from source nodes to destination signals.
     * @return Fanin signals in the destination network.
     */
    [[nodiscard]] std::vector<mockturtle::signal<Ntk>>
    gather_children(Ntk& dest, const mockturtle::node<Ntk> n,
                    const mockturtle::node_map<mockturtle::signal<Ntk>, Ntk>& old2new)
    {
        std::vector<mockturtle::signal<Ntk>> children{};
        children.reserve(ntk.fanin_size(n));

        ntk.foreach_fanin(n,
                          [this, &dest, &old2new, &children](const auto& f)
                          {
                              const auto fn    = ntk.get_node(f);
                              auto       child = old2new[fn];

                              // constants need no fanout trees
                              if (!ntk.is_constant(fn))
                              {
                                  child = take_fanout(dest, fn, child);
                              }

                              children.push_back(ntk.is_complemented(f) ? dest.create_not(child) : child);
                          });

        return children;
    }
    /**
     * Pushes a node down by `pad` levels: with a buffer chain, or with a shorter chain plus its fanout tree so that
     * the tree's leaves end up `pad` levels below the node.
     *
     * @param dest Destination network.
     * @param n Source node.
     * @param old2new Map from source nodes to destination signals; updated for `n`.
     * @param pad Depth every node of the rank is pushed down by.
     */
    void substitute(Ntk& dest, const mockturtle::node<Ntk> n,
                    mockturtle::node_map<mockturtle::signal<Ntk>, Ntk>& old2new, const uint32_t pad)
    {
        const auto fanouts = fanout_ntk.fanout_size(n);

        if (fanouts > 1 && !is_proper_fanout(n))
        {
            old2new[n] = buffer_chain(dest, old2new[n], pad - tree_depth(fanouts));
            generate_fanout_tree(dest, n, old2new[n]);
        }
        else
        {
            old2new[n] = buffer_chain(dest, old2new[n], pad);
        }
    }
    /**
     * Appends a chain of buffers to a signal.
     *
     * @param dest Destination network.
     * @param sig Signal to buffer.
     * @param length Number of buffers.
     * @return The end of the chain.
     */
    [[nodiscard]] static mockturtle::signal<Ntk> buffer_chain(Ntk& dest, mockturtle::signal<Ntk> sig,
                                                              const uint32_t length)
    {
        for (uint32_t i = 0; i < length; ++i)
        {
            sig = dest.create_buf(sig);
        }

        return sig;
    }
    /**
     * Builds the breadth-first fanout tree of a node. Exactly as many outputs as the node has consumers are kept, in
     * breadth-first order; each of them is padded with buffers to the depth of the tree, so that every consumer
     * connects at the same depth and no buffer is left without a consumer. The outputs are stored in
     * `available_fanouts` in planar order.
     *
     * @param dest Destination network.
     * @param n Source node.
     * @param root Signal the tree hangs from.
     */
    void generate_fanout_tree(Ntk& dest, const mockturtle::node<Ntk> n, const mockturtle::signal<Ntk> root)
    {
        const auto fanouts   = fanout_ntk.fanout_size(n);
        const auto num_nodes = num_fanout_nodes(fanouts);
        const auto depth     = tree_depth(fanouts);

        // free outputs as (signal, level below n)
        std::queue<std::pair<mockturtle::signal<Ntk>, uint32_t>> slots{};
        slots.emplace(root, 0);

        for (uint32_t i = 0; i < num_nodes; ++i)
        {
            const auto [sig, level] = slots.front();
            slots.pop();

            const auto buf = dest.create_buf(sig);

            for (uint32_t d = 0; d < ps.degree; ++d)
            {
                slots.emplace(buf, level + 1);
            }
        }

        std::vector<mockturtle::signal<Ntk>> ends{};
        ends.reserve(fanouts);

        for (uint32_t i = 0; i < fanouts && !slots.empty(); ++i)
        {
            const auto [sig, level] = slots.front();
            slots.pop();

            ends.push_back(buffer_chain(dest, sig, depth - level));
        }

        // nodes of the deepest level were created from left to right, so their ids give the planar order
        std::stable_sort(ends.begin(), ends.end(),
                         [&dest](const auto& a, const auto& b) { return dest.get_node(a) < dest.get_node(b); });

        auto& outputs = available_fanouts[n];
        for (const auto& sig : ends)
        {
            outputs.push(sig);
        }
    }
    /**
     * Returns the signal a consumer of `n` connects to: `child` itself while it has no output yet, otherwise the
     * next free output of the fanout tree of `n`.
     *
     * @param dest Destination network.
     * @param n Source node.
     * @param child Current destination signal of `n`.
     * @return Signal to connect to.
     */
    [[nodiscard]] mockturtle::signal<Ntk> take_fanout(const Ntk& dest, const mockturtle::node<Ntk> n,
                                                      const mockturtle::signal<Ntk> child)
    {
        if (dest.fanout_size(dest.get_node(child)) == 0)
        {
            return child;
        }

        auto& outputs = available_fanouts[n];

        if (outputs.empty())
        {
            return child;
        }

        const auto sig = outputs.front();
        outputs.pop();

        return sig;
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
     * Free outputs of the fanout tree per source node, each padded to the depth of the tree.
     */
    mockturtle::node_map<std::queue<mockturtle::signal<Ntk>>, Ntk> available_fanouts;
    /**
     * Parameters.
     */
    const planar_fanout_substitution_params& ps;
};

}  // namespace detail

/**
 * Substitutes high-output degrees in a ranked logic network with trees of fanout nodes while keeping the ranks and
 * the planarity of the network. Every node of a rank is pushed down by the depth of the deepest fanout tree in that
 * rank, with buffers for nodes that need no tree or a shallower one, so that all edges keep running between adjacent
 * ranks and the rank order stays the one of the input. The result is balanced if the input is.
 *
 * Virtual primary inputs of the input are kept.
 *
 * @tparam Ntk Ranked network type (see `mutable_rank_view`) that supports `create_buf`.
 * @param ntk Ranked input network, balanced with unified outputs.
 * @param ps Parameters.
 * @return A fanout-substituted network of the same type with the same ranks and no new crossings.
 * @throws std::invalid_argument If `ps.degree` is below 2 or `ntk` is not balanced with unified outputs.
 */
template <typename Ntk>
[[nodiscard]] Ntk planar_fanout_substitution(const Ntk& ntk, const planar_fanout_substitution_params& ps = {})
{
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
    static_assert(mockturtle::has_is_constant_v<Ntk>, "Ntk does not implement the is_constant method");
    static_assert(mockturtle::has_create_buf_v<Ntk>, "Ntk does not implement the create_buf method");
    static_assert(mockturtle::has_clone_node_v<Ntk>, "Ntk does not implement the clone_node method");
    static_assert(mockturtle::has_fanout_size_v<Ntk>, "Ntk does not implement the fanout_size method");
    static_assert(mockturtle::has_foreach_fanin_v<Ntk>, "Ntk does not implement the foreach_fanin method");
    static_assert(mockturtle::has_foreach_po_v<Ntk>, "Ntk does not implement the foreach_po method");
    static_assert(mockturtle::has_rank_position_v<Ntk>, "Ntk does not implement the rank_position method");
    static_assert(mockturtle::has_depth_v<Ntk>, "Ntk does not implement the depth method");

    if (ps.degree < 2)
    {
        throw std::invalid_argument("The fanout degree must be at least 2");
    }

    if (!is_balanced(ntk, {.unify_outputs = true, .buffer_constant_outputs = false}))
    {
        throw std::invalid_argument("The network must be balanced with unified outputs before fanout substitution");
    }

    detail::planar_fanout_substitution_impl<Ntk> p{ntk, ps};

    return p.run();
}

}  // namespace fiction::synthesis
