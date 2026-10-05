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
 * @brief Planarizes a ranked logic network by duplicating nodes level by level.
 * @author Benjamin Hien (hibenj)
 */

#pragma once

#include "fiction/networks/name_utils.hpp"
#include "fiction/networks/network_utils.hpp"
#include "fiction/networks/virtual_pi_network.hpp"
#include "fiction/synthesis/network_balancing.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/graph/mincross.hpp"
#include "fiction/utils/progress.hpp"

#include <fmt/format.h>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <limits>
#include <optional>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

namespace fiction::synthesis
{

/**
 * Parameters for the node duplication planarization algorithm.
 */
struct node_duplication_planarization_params
{
    /**
     * Order in which the primary outputs are placed in the first level before the algorithm starts.
     */
    enum class output_order : uint8_t
    {
        /**
         * Keep the primary output order of the input network.
         */
        KEEP_PO_ORDER,
        /**
         * Shuffle the primary outputs randomly. Different orders can yield different numbers of duplications.
         */
        RANDOM_PO_ORDER
    };
    /**
     * Receives completed work and the phase total.
     */
    utils::progress_callback on_progress{};
    /**
     * Primary output order used to seed the first level.
     */
    output_order po_order = output_order::KEEP_PO_ORDER;
    /**
     * Seed for the random primary output order. A random seed is drawn when none is given.
     */
    std::optional<uint32_t> seed = std::nullopt;
};

/**
 * Statistics of the node duplication planarization algorithm.
 */
struct node_duplication_planarization_stats
{
    /**
     * Runtime of the duplication core. Excludes the final planarity check.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * Number of nodes in the planarized network minus the number of nodes in the input network.
     */
    uint64_t num_duplications{0};
    /**
     * Writes the statistics to a stream.
     *
     * @param out Stream to write to.
     */
    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("[i] total time        = {:.2f} secs\n", mockturtle::to_seconds(time_total));
        out << fmt::format("[i] num. duplications = {}\n", num_duplications);
    }
};

namespace detail
{

/**
 * Nodes grouped by level, outermost vector indexed by level.
 *
 * @tparam Ntk Network type.
 */
template <typename Ntk>
using levelized_node_order = std::vector<std::vector<mockturtle::node<Ntk>>>;

/**
 * One node of the H-graph used to compute the duplication order of a level.
 *
 * For a node in level \f$l\f$ of the input network, all orderings of its fanins in level \f$l-1\f$ are enumerated. One
 * H-graph node represents one such ordering by its first and last fanin, which decide the delay of the ordering. The
 * remaining fanins sit in between; their mutual order does not matter for the algorithm.
 *
 * @tparam Ntk Network type.
 */
template <typename Ntk>
struct hgraph_node
{
    /**
     * Node of the upper level whose fanins are ordered. A copy id when that node is a duplicate.
     */
    mockturtle::node<Ntk> root;
    /**
     * First and last fanin of the ordering.
     */
    std::pair<mockturtle::node<Ntk>, mockturtle::node<Ntk>> outer_fanins;
    /**
     * All remaining fanins.
     */
    std::vector<mockturtle::node<Ntk>> middle_fanins{};
    /**
     * Delay of the shortest path through the H-graph that ends in this node.
     */
    uint64_t delay;
    /**
     * Index of the predecessor H-graph node on that shortest path, within the slice of the previous node.
     */
    std::size_t fanin_it{};
    /**
     * Creates an H-graph node.
     *
     * @param r Node whose fanins are ordered.
     * @param first Leftmost fanin of the ordering.
     * @param last Rightmost fanin of the ordering.
     * @param delay_value Initial delay.
     */
    hgraph_node(const mockturtle::node<Ntk> r, const mockturtle::node<Ntk> first, const mockturtle::node<Ntk> last,
                const uint64_t delay_value) :
            root{r},
            outer_fanins(first, last),
            delay(delay_value)
    {}
};

/**
 * Enumerates the H-graph nodes of one slice, i.e., all orderings of the given fanins by their first and last element.
 * A single fanin yields one H-graph node. Delays start at infinity.
 *
 * @tparam Ntk Network type.
 * @param root Node whose fanins are ordered.
 * @param nodes Fanins in rank order.
 * @return H-graph nodes of the slice.
 */
template <typename Ntk>
[[nodiscard]] std::vector<hgraph_node<Ntk>> calculate_pairs(const mockturtle::node<Ntk>               root,
                                                            const std::vector<mockturtle::node<Ntk>>& nodes)
{
    constexpr auto inf = std::numeric_limits<uint64_t>::max();

    std::vector<hgraph_node<Ntk>> combinations{};

    if (nodes.empty())
    {
        return combinations;
    }

    if (nodes.size() == 1)
    {
        combinations.emplace_back(root, nodes.front(), nodes.front(), inf);
        return combinations;
    }

    combinations.reserve(nodes.size() * (nodes.size() - 1));

    for (auto it1 = nodes.cbegin(); it1 != nodes.cend(); ++it1)
    {
        for (auto it2 = std::next(it1); it2 != nodes.cend(); ++it2)
        {
            std::vector<mockturtle::node<Ntk>> middle_fanins{};
            middle_fanins.reserve(nodes.size() - 2);

            for (auto it = nodes.cbegin(); it != nodes.cend(); ++it)
            {
                if (it != it1 && it != it2)
                {
                    middle_fanins.push_back(*it);
                }
            }

            hgraph_node<Ntk> forward{root, *it1, *it2, inf};
            hgraph_node<Ntk> backward{root, *it2, *it1, inf};

            forward.middle_fanins  = middle_fanins;
            backward.middle_fanins = std::move(middle_fanins);

            combinations.push_back(std::move(forward));
            combinations.push_back(std::move(backward));
        }
    }

    return combinations;
}

/**
 * Implementation of the node duplication planarization algorithm.
 *
 * Nodes of the source network keep their ids. Every duplicate receives a fresh id above `ntk.size()`; `origin()` maps
 * it back to the source node. For every node of a level, `fanins_of` records which nodes (originals or duplicates) of
 * the level below it connects to, so the planar network can be rebuilt without searching.
 *
 * @tparam Ntk Ranked, balanced source network type.
 */
template <typename Ntk>
class node_duplication_planarization_impl
{
  public:
    /**
     * Creates the implementation.
     *
     * @param src Source network.
     * @param p Parameters.
     * @param st Statistics.
     */
    node_duplication_planarization_impl(const Ntk& src, const node_duplication_planarization_params& p,
                                        node_duplication_planarization_stats& st) :
            ntk{src},
            ps{p},
            pst{st},
            fanins_of(src.size())
    {}

    /**
     * Runs the algorithm.
     *
     * @return Planar network with duplicated nodes and virtual primary inputs.
     */
    [[nodiscard]] networks::virtual_pi_network<Ntk> run()
    {
        const mockturtle::stopwatch stop{pst.time_total};

        // one step per gate level of the input network plus one for building the result
        utils::progress_reporter progress{ps.on_progress, "planarizing levels", ntk.depth() + 1};

        // first level: the primary outputs in rank order
        std::vector<mockturtle::node<Ntk>> pos{};
        pos.reserve(ntk.num_pos());
        ntk.foreach_node(
            [this, &pos](const auto& n)
            {
                if (ntk.is_po(n) && !ntk.is_constant(n) && std::find(pos.cbegin(), pos.cend(), n) == pos.cend())
                {
                    pos.push_back(n);
                }
            });

        if (ps.po_order == node_duplication_planarization_params::output_order::RANDOM_PO_ORDER)
        {
            std::mt19937_64 generator{ps.seed.has_value() ? *ps.seed : std::random_device{}()};
            std::shuffle(pos.begin(), pos.end(), generator);
        }

        for (const auto& po : pos)
        {
            fis.clear();
            compute_slice_delays(po);
        }

        ntk_lvls.push_back(pos);
        progress.advance();

        auto next_level    = compute_node_order();
        bool f_final_level = is_final_level(next_level);

        while (!next_level.empty() && !f_final_level)
        {
            ntk_lvls.push_back(next_level);
            lvl_pairs.clear();

            // one slice of the H-graph per node of the level
            for (const auto& n : next_level)
            {
                fis.clear();
                compute_slice_delays(n);
            }

            next_level    = compute_node_order();
            f_final_level = is_final_level(next_level);
            progress.advance();
        }

        // the final level holds the primary inputs
        if (f_final_level)
        {
            ntk_lvls.push_back(next_level);
        }

        auto planar_ntk = build_network();

        networks::restore_network_name(ntk, planar_ntk);
        networks::restore_output_names(ntk, planar_ntk);

        pst.num_duplications = copy_origin.size();

        progress.advance();

        return planar_ntk;
    }

  private:
    /**
     * Maps a node id to the source node it stands for.
     *
     * @param n Source node id or copy id.
     * @return The source node.
     */
    [[nodiscard]] mockturtle::node<Ntk> origin(const mockturtle::node<Ntk> n) const
    {
        return n < ntk.size() ? n : copy_origin[n - ntk.size()];
    }
    /**
     * Allocates a new copy id for a source node.
     *
     * @param n Source node.
     * @return The copy id.
     */
    [[nodiscard]] mockturtle::node<Ntk> make_copy(const mockturtle::node<Ntk> n)
    {
        copy_origin.push_back(n);
        fanins_of.emplace_back();

        return static_cast<mockturtle::node<Ntk>>(ntk.size() + copy_origin.size() - 1);
    }
    /**
     * Adds one slice to the H-graph of the current level.
     *
     * A slice holds every ordering of the fanins of `n` as an H-graph node (`calculate_pairs`). The delay of each
     * ordering is the shortest path from the first slice of the level: moving to an ordering whose first fanin equals
     * the last fanin of the previous ordering costs 1, any other move costs 2, and the first slice starts at 1. Ties
     * between equal delays are broken in favour of orderings that share a fanin in the level below, which avoids a
     * duplication there. The slice is appended to `lvl_pairs`.
     *
     * @param n Node (source id or copy id) whose fanins form the slice.
     */
    void compute_slice_delays(const mockturtle::node<Ntk> n)
    {
        const auto o = origin(n);

        // primary inputs propagate to the next level, since they must reach the input level without crossings
        if (ntk.is_pi(o))
        {
            fis.push_back(o);
        }

        // keep rank order among equal delays: a later insertion never overwrites an earlier one
        ntk.foreach_fanin(o,
                          [this](const auto& f)
                          {
                              const auto fn = ntk.get_node(f);

                              if (ntk.is_constant(fn))
                              {
                                  return;
                              }

                              const auto it =
                                  std::lower_bound(fis.cbegin(), fis.cend(), fn, [this](const auto& a, const auto& b)
                                                   { return ntk.rank_position(a) < ntk.rank_position(b); });

                              fis.insert(it, fn);
                          });

        assert(!fis.empty() && "A node without non-constant fanins is dangling");

        auto combinations = calculate_pairs<Ntk>(n, fis);

        if (lvl_pairs.empty())
        {
            for (auto& ordering : combinations)
            {
                ordering.delay = 1;
            }
        }
        else
        {
            const auto& previous = lvl_pairs.back();

            for (auto& cur : combinations)
            {
                for (std::size_t last_idx = 0; last_idx < previous.size(); ++last_idx)
                {
                    const auto& last = previous[last_idx];

                    if (cur.outer_fanins.first == last.outer_fanins.second && last.delay + 1 < cur.delay)
                    {
                        cur.fanin_it = last_idx;
                        cur.delay    = last.delay + 1;
                    }
                    else if (last.delay + 2 < cur.delay)
                    {
                        cur.fanin_it = last_idx;
                        cur.delay    = last.delay + 2;
                    }
                    else if (last.delay + 2 == cur.delay && last.fanin_it < previous.size() &&
                             share_fanin(cur.outer_fanins.first, last.outer_fanins.second))
                    {
                        cur.fanin_it = last_idx;
                        break;
                    }
                }
            }
        }

        lvl_pairs.push_back(std::move(combinations));
    }
    /**
     * Whether two source nodes have a fanin in common.
     *
     * @param a First node.
     * @param b Second node.
     * @return `true` iff `a` and `b` share at least one fanin.
     */
    [[nodiscard]] bool share_fanin(const mockturtle::node<Ntk> a, const mockturtle::node<Ntk> b) const
    {
        const auto fa = networks::fanins(ntk, a);
        const auto fb = networks::fanins(ntk, b);

        return std::any_of(
            fa.fanin_nodes.cbegin(), fa.fanin_nodes.cend(), [&fb](const auto& f)
            { return std::find(fb.fanin_nodes.cbegin(), fb.fanin_nodes.cend(), f) != fb.fanin_nodes.cend(); });
    }
    /**
     * Places one fanin into the next level, which is built from right to left.
     *
     * If the rightmost node already placed stands for `n`, that copy is shared. Otherwise `n` is appended: as the
     * source node itself when it has not been placed in this level yet, as a fresh copy when it has.
     *
     * @param n Source node to place.
     * @param level_rtl Next level under construction, rightmost node first.
     * @param placed Whether the source node itself is already in the level.
     * @param fanins Receives the id (source or copy) that the consumer connects to.
     */
    void place_fanin(const mockturtle::node<Ntk> n, std::vector<mockturtle::node<Ntk>>& level_rtl,
                     std::vector<bool>& placed, std::vector<mockturtle::node<Ntk>>& fanins)
    {
        if (!level_rtl.empty() && origin(level_rtl.back()) == n)
        {
            fanins.push_back(level_rtl.back());
            return;
        }

        if (!placed[n])
        {
            placed[n] = true;
            level_rtl.push_back(n);
            fanins.push_back(n);
        }
        else
        {
            const auto copy = make_copy(n);
            level_rtl.push_back(copy);
            fanins.push_back(copy);
        }
    }
    /**
     * Places the fanins of one H-graph ordering into the next level and records them for the ordering's root.
     *
     * @param ordering H-graph node to place.
     * @param level_rtl Next level under construction, rightmost node first.
     * @param placed Whether a source node itself is already in the level.
     */
    void place_ordering(const hgraph_node<Ntk>& ordering, std::vector<mockturtle::node<Ntk>>& level_rtl,
                        std::vector<bool>& placed)
    {
        std::vector<mockturtle::node<Ntk>> fanins{};
        fanins.reserve(ordering.middle_fanins.size() + 2);

        place_fanin(ordering.outer_fanins.second, level_rtl, placed, fanins);

        for (const auto& n : ordering.middle_fanins)
        {
            place_fanin(n, level_rtl, placed, fanins);
        }

        if (ordering.outer_fanins.first != ordering.outer_fanins.second)
        {
            place_fanin(ordering.outer_fanins.first, level_rtl, placed, fanins);
        }

        fanins_of[ordering.root] = std::move(fanins);
    }
    /**
     * Computes the order of the next level from the H-graph of the current level: the ordering with the least delay
     * in the last slice is chosen and its predecessors are followed back to the first slice. The fanins of each
     * ordering are placed from right to left as they are encountered.
     *
     * @return Nodes of the next level in planar order, duplicates as copy ids.
     */
    [[nodiscard]] std::vector<mockturtle::node<Ntk>> compute_node_order()
    {
        std::vector<mockturtle::node<Ntk>> level_rtl{};
        std::vector<bool>                  placed(ntk.size(), false);

        if (lvl_pairs.empty())
        {
            return level_rtl;
        }

        const auto& last_slice = lvl_pairs.back();

        const auto minimum_it = std::min_element(last_slice.cbegin(), last_slice.cend(),
                                                 [](const auto& a, const auto& b) { return a.delay < b.delay; });

        if (minimum_it == last_slice.cend())
        {
            return level_rtl;
        }

        place_ordering(*minimum_it, level_rtl, placed);

        std::size_t level    = lvl_pairs.size() - 1;
        std::size_t fanin_it = minimum_it->fanin_it;

        while (level > 0 && fanin_it < lvl_pairs[level - 1].size())
        {
            const auto& ordering = lvl_pairs[level - 1][fanin_it];

            place_ordering(ordering, level_rtl, placed);

            --level;
            fanin_it = ordering.fanin_it;
        }

        std::reverse(level_rtl.begin(), level_rtl.end());

        return level_rtl;
    }
    /**
     * Whether a level consists of primary inputs only.
     *
     * @param level Nodes of the level.
     * @return `true` iff every node stands for a primary input.
     */
    [[nodiscard]] bool is_final_level(const std::vector<mockturtle::node<Ntk>>& level) const
    {
        return std::all_of(level.cbegin(), level.cend(), [this](const auto& n) { return ntk.is_pi(origin(n)); });
    }
    /**
     * Builds the planar `virtual_pi_network` from the levelized duplication order.
     *
     * Levels are processed from the primary inputs upwards. A repeated primary input becomes a virtual primary input;
     * every other repeated node becomes a fresh gate with the same function. Fanins are connected in the fanin order
     * of the source node, matched by origin against the ids recorded in `fanins_of`, and keep their complementation.
     *
     * @return The planar destination network.
     */
    [[nodiscard]] networks::virtual_pi_network<Ntk> build_network()
    {
        using ntk_dest_t = networks::virtual_pi_network<Ntk>;
        using signal_t   = mockturtle::signal<ntk_dest_t>;

        static_assert(mockturtle::has_create_node_v<ntk_dest_t>, "virtual_pi_network<Ntk> lacks create_node");
        static_assert(mockturtle::has_create_po_v<ntk_dest_t>, "virtual_pi_network<Ntk> lacks create_po");
        static_assert(mockturtle::has_create_not_v<ntk_dest_t>, "virtual_pi_network<Ntk> lacks create_not");

        ntk_dest_t dest{};

        // signal in `dest` for every source id and copy id
        std::vector<signal_t> old2new(ntk.size() + copy_origin.size());

        old2new[ntk.get_constant(false)] = dest.get_constant(false);
        if (ntk.get_node(ntk.get_constant(true)) != ntk.get_node(ntk.get_constant(false)))
        {
            old2new[ntk.get_constant(true)] = dest.get_constant(true);
        }

        ntk.foreach_pi_unranked([&](const auto& n) { old2new[n] = dest.create_pi(); });

        levelized_node_order<ntk_dest_t> lvls_new(ntk_lvls.size());

        for (auto i = ntk_lvls.size(); i-- > 0;)
        {
            const auto& lvl     = ntk_lvls[i];
            auto&       lvl_new = lvls_new[i];
            lvl_new.reserve(lvl.size());

            for (const auto& n : lvl)
            {
                const auto o = origin(n);

                if (ntk.is_pi(o))
                {
                    // the first placement keeps the real primary input; every further one is a virtual copy
                    if (n != o)
                    {
                        old2new[n] = dest.create_virtual_pi(old2new[o]);
                    }

                    lvl_new.push_back(dest.get_node(old2new[n]));
                }
                else
                {
                    old2new[n] = dest.create_node(collect_children(n, old2new, dest), ntk.node_function(o));
                    lvl_new.push_back(dest.get_node(old2new[n]));
                }
            }
        }

        ntk.foreach_po(
            [&](const auto& po)
            {
                const auto sig = old2new[ntk.get_node(po)];

                dest.create_po(ntk.is_complemented(po) ? dest.create_not(sig) : sig);
            });

        // `ntk_lvls` starts at the primary outputs; ranks start at the primary inputs
        std::reverse(lvls_new.begin(), lvls_new.end());

        dest.update_ranks();
        dest.set_all_ranks(lvls_new);

        return dest;
    }
    /**
     * Collects the fanin signals of a node in the destination network, in the fanin order of its source node.
     *
     * @tparam NtkDest Destination network type.
     * @param n Source id or copy id of the node.
     * @param old2new Signals in the destination network per source id and copy id.
     * @param dest Destination network.
     * @return Fanin signals of the new node.
     */
    template <typename NtkDest>
    [[nodiscard]] std::vector<mockturtle::signal<NtkDest>>
    collect_children(const mockturtle::node<Ntk> n, const std::vector<mockturtle::signal<NtkDest>>& old2new,
                     NtkDest& dest) const
    {
        const auto o = origin(n);

        std::vector<mockturtle::signal<NtkDest>> children{};
        children.reserve(ntk.fanin_size(o));

        // ids of the level below that this node connects to, one per non-constant fanin; consumed as matched
        auto candidates = fanins_of[n];

        ntk.foreach_fanin(o,
                          [&](const auto& f)
                          {
                              const auto fn = ntk.get_node(f);

                              mockturtle::signal<NtkDest> sig{};

                              if (ntk.is_constant(fn))
                              {
                                  sig = old2new[fn];
                              }
                              else
                              {
                                  const auto it = std::find_if(candidates.begin(), candidates.end(),
                                                               [this, &fn](const auto& c) { return origin(c) == fn; });

                                  assert(it != candidates.end() && "A fanin has no copy in the level below");

                                  sig = old2new[*it];
                                  candidates.erase(it);
                              }

                              children.push_back(ntk.is_complemented(f) ? dest.create_not(sig) : sig);
                          });

        return children;
    }
    /**
     * Source network.
     */
    const Ntk& ntk;
    /**
     * Parameters.
     */
    const node_duplication_planarization_params& ps;
    /**
     * Statistics.
     */
    node_duplication_planarization_stats& pst;
    /**
     * Source node of every copy id, indexed by `copy id - ntk.size()`.
     */
    std::vector<mockturtle::node<Ntk>> copy_origin{};
    /**
     * For every source id and copy id, the ids of the level below it connects to, in placement order.
     */
    std::vector<std::vector<mockturtle::node<Ntk>>> fanins_of;
    /**
     * H-graph of the current level, one slice per node of the level.
     */
    std::vector<std::vector<hgraph_node<Ntk>>> lvl_pairs{};
    /**
     * Fanins of the node whose slice is being computed, in rank order.
     */
    std::vector<mockturtle::node<Ntk>> fis{};
    /**
     * Levelized duplication order, level 0 being the primary outputs; duplicates as copy ids.
     */
    levelized_node_order<Ntk> ntk_lvls{};
};

}  // namespace detail

/**
 * Planarizes a ranked, balanced logic network by duplicating nodes.
 *
 * The algorithm follows "Fabricatable Interconnect and Molecular QCA Circuits" by A. Chaudhary, D. Z. Chen, X. S. Hu,
 * M. T. Niemier, R. Ravichandran, and K. Whitton in IEEE TCAD 26(11), 2007. It solves the node duplication crossing
 * elimination problem level by level from the primary outputs to the primary inputs. For each level, an H-graph
 * enumerates the fanin orderings of every node; the shortest path through it gives an ordering of the level below in
 * which every edge can be drawn without a crossing, at the cost of duplicating the nodes that would otherwise be
 * crossed. Duplicated primary inputs become virtual primary inputs of the result.
 *
 * Consecutive consumers in a level share one copy of their common fanin, so the result is not fanout-substituted;
 * `planar_fanout_substitution` restores that property while keeping ranks and planarity.
 *
 * The input must be balanced (see `network_balancing`), carry ranks (see `mutable_rank_view`), and contain no
 * virtual primary inputs (see `delete_virtual_pis`). The result is ranked and crossing-free; `mincross` verifies the
 * latter before the function returns.
 *
 * @tparam Ntk Ranked, balanced source network type.
 * @param ntk Source network.
 * @param ps Parameters.
 * @param pst Statistics.
 * @return Planar `virtual_pi_network` that computes the same functions as `ntk`.
 * @throws std::invalid_argument If `ntk` is not balanced or contains virtual primary inputs.
 * @throws std::runtime_error If the result still contains crossings.
 */
template <typename Ntk>
[[nodiscard]] networks::virtual_pi_network<Ntk>
node_duplication_planarization(const Ntk& ntk, const node_duplication_planarization_params& ps = {},
                               node_duplication_planarization_stats* pst = nullptr)
{
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
    static_assert(mockturtle::has_create_node_v<Ntk>, "Ntk does not implement the create_node method");
    static_assert(mockturtle::has_rank_position_v<Ntk>, "Ntk does not implement the rank_position method");
    static_assert(mockturtle::has_depth_v<Ntk>, "Ntk does not implement the depth method");

    if (!is_balanced(ntk))
    {
        throw std::invalid_argument("The network must be balanced before planarization");
    }

    if constexpr (has_num_virtual_pis_v<Ntk>)
    {
        if (ntk.num_virtual_pis() > 0)
        {
            throw std::invalid_argument("The network must not contain virtual primary inputs; delete them first");
        }
    }

    node_duplication_planarization_stats st{};

    detail::node_duplication_planarization_impl<Ntk> p{ntk, ps, st};

    auto result = p.run();

    utils::graph::mincross_params mc_ps{};
    mc_ps.optimize = false;
    utils::graph::mincross_stats mc_st{};

    utils::graph::mincross(result, mc_ps, &mc_st);

    if (mc_st.num_crossings != 0)
    {
        throw std::runtime_error(
            fmt::format("Planarization failed: the result contains {} crossings", mc_st.num_crossings));
    }

    if (pst != nullptr)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction::synthesis
