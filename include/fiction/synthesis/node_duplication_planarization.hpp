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
#include "fiction/synthesis/crossing_gate_planarization.hpp"
#include "fiction/synthesis/network_balancing.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/graph/mincross.hpp"
#include "fiction/utils/progress.hpp"

#include <fmt/format.h>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <limits>
#include <numeric>
#include <optional>
#include <random>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
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
     * How a level is made crossing-free.
     */
    enum class planarization_strategy : uint8_t
    {
        /**
         * Duplicate nodes on every level. The result is planar.
         */
        DUPLICATION,
        /**
         * Decide per level whether duplicating nodes or keeping the crossings for `crossing_gate_planarization` is
         * cheaper. The result contains crossings on the levels where gadgets are cheaper.
         */
        HYBRID
    };
    /**
     * How the hybrid strategy estimates the cost of duplicating a level.
     */
    enum class decision_criterion : uint8_t
    {
        /**
         * The weighted size of the duplicated cones, see `duplication_cost_model`.
         */
        WEIGHTED_CONE,
        /**
         * The number of nodes that duplicating the rest of the network actually creates, measured by running the
         * duplication strategy on the levels below for both options and stopping once one exceeds the other. On the
         * benchmark sets this is never worse and up to 16 % better than the weighted cone at the same runtime.
         */
        LOOKAHEAD
    };
    /**
     * Weights of the duplication cost model of the hybrid strategy's `WEIGHTED_CONE` criterion. The cost of
     * duplicating a node is the weighted size of its transitive fanin, since every duplicate drags its whole cone
     * along. A gate weighs `node_weight` and a chain buffer or inverter weighs `buffer_weight`. The sum is scaled by
     * \f$\text{depth\_growth}^{d}\f$ for a duplication on level \f$d\f$, because duplicates on deep levels are
     * duplicated again by the decisions below. The weights are in units of one crossing gadget node. The defaults
     * were determined empirically on the benchmark sets; see `experiments/planarization/cost_model_sweep.cpp`.
     */
    struct duplication_cost_model
    {
        /**
         * Weight of a gate.
         */
        double node_weight = 2.0;
        /**
         * Weight of a buffer or inverter chain node.
         */
        double buffer_weight = 0.5;
        /**
         * Growth of the duplication cost per level on which the duplication happens.
         */
        double depth_growth = 1.02;
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
    /**
     * Planarization strategy.
     */
    planarization_strategy strategy = planarization_strategy::DUPLICATION;
    /**
     * Maximum number of adjacent swaps per level that the hybrid strategy tries after the barycenter ordering to
     * reduce the crossings it costs. `0` keeps the barycenter order.
     */
    uint32_t max_swaps = 32u;
    /**
     * Whether the subsequent `crossing_gate_planarization` builds its gadgets from XOR gates. Sets the gadget cost
     * of the hybrid strategy.
     */
    bool xor_gates = false;
    /**
     * Levels with more crossings are always duplicated in the hybrid strategy, matching the limit of
     * `crossing_gate_planarization`.
     */
    uint32_t max_crossings_per_rank = 1000u;
    /**
     * Decision criterion of the hybrid strategy.
     */
    decision_criterion criterion = decision_criterion::LOOKAHEAD;
    /**
     * Duplication cost model of the hybrid strategy.
     */
    duplication_cost_model duplication_cost{};
    /**
     * Nodes a lookahead may create before it is cut off; levels whose both options exceed it fall back to the
     * weighted cone model.
     */
    uint64_t lookahead_budget = 1000000u;
    /**
     * Abort with `std::runtime_error` once more nodes than this have been duplicated. Node duplication can grow
     * exponentially with the depth of the network; 0 disables the limit.
     */
    uint64_t max_duplications = 0u;
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
     * Number of levels on which the hybrid strategy kept the crossings.
     */
    uint64_t num_crossing_levels{0};
    /**
     * Number of crossings the hybrid strategy left for `crossing_gate_planarization`.
     */
    uint64_t num_crossings{0};
    /**
     * Writes the statistics to a stream.
     *
     * @param out Stream to write to.
     */
    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("[i] total time           = {:.2f} secs\n", mockturtle::to_seconds(time_total));
        out << fmt::format("[i] num. duplications    = {}\n", num_duplications);
        out << fmt::format("[i] num. crossing levels = {}\n", num_crossing_levels);
        out << fmt::format("[i] num. crossings       = {}\n", num_crossings);
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
        crossing_level.push_back(false);
        progress.advance();

        std::size_t copies_before = copy_origin.size();

        auto next_level    = compute_node_order();
        bool f_final_level = is_final_level(next_level);

        while (!next_level.empty() && !f_final_level)
        {
            // the hybrid strategy considers keeping the crossings of a level instead of its duplicates
            const bool duplicated = copy_origin.size() > copies_before;
            bool       crossings  = false;

            if (ps.strategy == node_duplication_planarization_params::planarization_strategy::HYBRID && duplicated)
            {
                // level of `next_level` in the source network
                const auto lvl = ntk.depth() - static_cast<uint32_t>(ntk_lvls.size());

                auto       original_rank = ntk.get_ranks(lvl);
                const auto cross_cost    = crossing_cost(ntk_lvls.back(), original_rank);

                if (crossings_are_cheaper(cross_cost.cost, next_level, original_rank, lvl))
                {
                    next_level = std::move(original_rank);
                    crossings  = true;
                    ++pst.num_crossing_levels;
                    pst.num_crossings += cross_cost.num_crossings;
                }
            }

            ntk_lvls.push_back(next_level);
            crossing_level.push_back(crossings);
            lvl_pairs.clear();

            // one slice of the H-graph per node of the level
            for (const auto& n : next_level)
            {
                fis.clear();
                compute_slice_delays(n);
            }

            copies_before = copy_origin.size();
            next_level    = compute_node_order();
            f_final_level = is_final_level(next_level);
            progress.advance();

            if (ps.max_duplications > 0 && copy_origin.size() > ps.max_duplications)
            {
                throw std::runtime_error(
                    fmt::format("Planarization aborted: more than {} duplications", ps.max_duplications));
            }
        }

        // the final level holds the primary inputs
        if (f_final_level)
        {
            ntk_lvls.push_back(next_level);
            crossing_level.push_back(false);
        }

        auto planar_ntk = build_network();

        networks::restore_network_name(ntk, planar_ntk);
        networks::restore_output_names(ntk, planar_ntk);

        pst.num_duplications = planar_ntk.size() - ntk.size();

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

        if (fis.empty())
        {
            throw std::invalid_argument("A gate has only constant fanins; propagate constants before planarization");
        }

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
                    // on a crossing level, the recorded fanins are the duplicates that were discarded
                    const bool below_is_crossing_level = crossing_level[i + 1];

                    old2new[n] = dest.create_node(collect_children(n, old2new, dest, below_is_crossing_level),
                                                  ntk.node_function(o));
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
     * @param by_origin Whether to connect to the source nodes themselves instead of the recorded copies, which is the
     * case when the level below kept its crossings.
     * @return Fanin signals of the new node.
     */
    template <typename NtkDest>
    [[nodiscard]] std::vector<mockturtle::signal<NtkDest>>
    collect_children(const mockturtle::node<Ntk> n, const std::vector<mockturtle::signal<NtkDest>>& old2new,
                     NtkDest& dest, const bool by_origin) const
    {
        const auto o = origin(n);

        std::vector<mockturtle::signal<NtkDest>> children{};
        children.reserve(ntk.fanin_size(o));

        // ids of the level below that this node connects to; a fanin that occurs twice connects to one copy
        const auto& candidates = fanins_of[n];

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
                                  const auto it = std::find_if(candidates.cbegin(), candidates.cend(),
                                                               [this, &fn](const auto& c) { return origin(c) == fn; });

                                  assert(it != candidates.cend() && "A fanin has no copy in the level below");

                                  sig = old2new[by_origin ? origin(*it) : *it];
                              }

                              children.push_back(ntk.is_complemented(f) ? dest.create_not(sig) : sig);
                          });

        return children;
    }
    /**
     * Decides whether keeping the crossings of a level beats duplicating it, according to the decision criterion.
     *
     * @param gadget_cost Nodes the crossing gadgets and their padding add.
     * @param duplicated The level as the duplication strategy would build it, duplicates as copy ids.
     * @param crossed The level in its crossing-minimized source order.
     * @param lvl Level in the source network.
     * @return `true` iff the crossings are cheaper.
     */
    [[nodiscard]] bool crossings_are_cheaper(const uint64_t                            gadget_cost,
                                             const std::vector<mockturtle::node<Ntk>>& duplicated,
                                             const std::vector<mockturtle::node<Ntk>>& crossed,
                                             const uint32_t                            lvl) const
    {
        if (gadget_cost == std::numeric_limits<uint64_t>::max())
        {
            return false;
        }

        if (ps.criterion == node_duplication_planarization_params::decision_criterion::LOOKAHEAD)
        {
            // what the rest of the network costs after each option; the second run stops once it loses
            const auto below_crossed = copies_below(crossed, ps.lookahead_budget);

            if (below_crossed <= ps.lookahead_budget)
            {
                const auto crossing_total = gadget_cost + below_crossed;

                return copies_below(duplicated, crossing_total) > crossing_total;
            }

            if (copies_below(duplicated, ps.lookahead_budget) <= ps.lookahead_budget)
            {
                return false;
            }
        }

        return gadget_cost < duplication_cost(duplicated, lvl);
    }
    /**
     * Counts the nodes the duplication strategy creates below a level, given its order, by running it on a copy of
     * the state. Stops early once the count exceeds `budget`.
     *
     * @param level Nodes of the level, duplicates as copy ids of this instance or as repeated source nodes.
     * @param budget Count at which the run is cut off.
     * @return Nodes created below the level, or a value above `budget` if cut off.
     */
    [[nodiscard]] uint64_t copies_below(const std::vector<mockturtle::node<Ntk>>& level, const uint64_t budget) const
    {
        node_duplication_planarization_params sub_ps{};
        sub_ps.strategy = node_duplication_planarization_params::planarization_strategy::DUPLICATION;

        node_duplication_planarization_stats sub_st{};

        node_duplication_planarization_impl sub{ntk, sub_ps, sub_st};

        std::vector<mockturtle::node<Ntk>> start{};
        start.reserve(level.size());

        std::vector<bool> seen(ntk.size(), false);
        for (const auto& n : level)
        {
            const auto o = origin(n);

            if (seen[o])
            {
                start.push_back(sub.make_copy(o));
            }
            else
            {
                seen[o] = true;
                start.push_back(o);
            }
        }

        return sub.run_from(start, budget);
    }
    /**
     * Runs the duplication strategy from a given level down to the primary inputs and counts the nodes it creates.
     *
     * @param start Nodes of the level to start from, duplicates as copy ids of this instance.
     * @param budget Count at which the run is cut off.
     * @return Nodes created below `start`, or a value above `budget` if cut off.
     */
    [[nodiscard]] uint64_t run_from(const std::vector<mockturtle::node<Ntk>>& start, const uint64_t budget)
    {
        const auto initial = copy_origin.size();

        ntk_lvls.push_back(start);
        lvl_pairs.clear();

        for (const auto& n : start)
        {
            fis.clear();
            compute_slice_delays(n);
        }

        auto next_level    = compute_node_order();
        bool f_final_level = is_final_level(next_level);

        while (!next_level.empty() && !f_final_level)
        {
            if (copy_origin.size() - initial > budget)
            {
                return budget + 1;
            }

            ntk_lvls.push_back(next_level);
            lvl_pairs.clear();

            for (const auto& n : next_level)
            {
                fis.clear();
                compute_slice_delays(n);
            }

            next_level    = compute_node_order();
            f_final_level = is_final_level(next_level);
        }

        return copy_origin.size() - initial;
    }
    /**
     * Result of costing the crossings of a level.
     */
    struct crossing_cost_result
    {
        /**
         * Estimated number of nodes the crossing gadgets and their padding add.
         */
        uint64_t cost;
        /**
         * Number of crossings between the level and the one above it.
         */
        uint64_t num_crossings;
    };
    /**
     * Number of nodes one crossing gadget adds.
     *
     * @return Gadget size.
     */
    [[nodiscard]] uint64_t gadget_nodes() const noexcept
    {
        return ps.xor_gates ? XOR_GADGET_NODES : AND_OR_GADGET_NODES;
    }
    /**
     * Number of levels one crossing gadget spans.
     *
     * @return Gadget depth.
     */
    [[nodiscard]] uint64_t gadget_depth() const noexcept
    {
        return ps.xor_gates ? XOR_GADGET_DEPTH : AND_OR_GADGET_DEPTH;
    }
    /**
     * Weighted size of the transitive fanin of a source node, see `duplication_cost_model`.
     *
     * @param root Source node.
     * @param current_level Level on which the node is duplicated.
     * @return Weighted cone size.
     */
    [[nodiscard]] uint64_t weighted_tfi_cost(const mockturtle::node<Ntk> root, const uint32_t current_level) const
    {
        const auto& m = ps.duplication_cost;

        const double scale = std::pow(m.depth_growth, static_cast<double>(current_level));

        // the cone weight of a source node does not depend on the level of the duplication
        if (const auto it = cone_weights.find(root); it != cone_weights.end())
        {
            return to_cost(it->second * scale);
        }

        std::vector<mockturtle::node<Ntk>>        stack{root};
        std::unordered_set<mockturtle::node<Ntk>> visited{};

        double total = 0.0;

        while (!stack.empty())
        {
            const auto n = stack.back();
            stack.pop_back();

            if (!visited.insert(n).second)
            {
                continue;
            }

            total += (ntk.fanin_size(n) == 1 && ntk.fanout_size(n) == 1) ? m.buffer_weight : m.node_weight;

            ntk.foreach_fanin(n, [&stack, this](const auto& f) { stack.push_back(ntk.get_node(f)); });
        }

        cone_weights.emplace(root, total);

        return to_cost(total * scale);
    }
    /**
     * Rounds a cost to an integer, saturating at the maximum.
     *
     * @param cost Cost.
     * @return Rounded cost.
     */
    [[nodiscard]] static uint64_t to_cost(const double cost) noexcept
    {
        if (cost >= static_cast<double>(std::numeric_limits<uint64_t>::max()))
        {
            return std::numeric_limits<uint64_t>::max();
        }

        return static_cast<uint64_t>(std::llround(std::max(cost, 0.0)));
    }
    /**
     * Cost of the duplicates in a level: every copy beyond the first occurrence of a source node costs the weighted
     * size of that node's cone.
     *
     * @param level Nodes of the level, duplicates as copy ids.
     * @param lvl Level in the source network.
     * @return Duplication cost.
     */
    [[nodiscard]] uint64_t duplication_cost(const std::vector<mockturtle::node<Ntk>>& level, const uint32_t lvl) const
    {
        std::unordered_map<mockturtle::node<Ntk>, uint32_t> occurrences{};
        for (const auto& n : level)
        {
            ++occurrences[origin(n)];
        }

        uint64_t cost = 0;
        for (const auto& [n, count] : occurrences)
        {
            if (count > 1)
            {
                cost += (count - 1) * weighted_tfi_cost(n, lvl);
            }
        }

        return cost;
    }
    /**
     * Positions of the nodes of a level, indexed by source node.
     *
     * @param level Nodes of the level, source ids only.
     * @return Position per source node, `npos` for nodes not in the level.
     */
    [[nodiscard]] std::vector<std::size_t> positions(const std::vector<mockturtle::node<Ntk>>& level) const
    {
        constexpr auto npos = std::numeric_limits<std::size_t>::max();

        std::vector<std::size_t> pos(ntk.size(), npos);
        for (std::size_t i = 0; i < level.size(); ++i)
        {
            pos[level[i]] = i;
        }

        return pos;
    }
    /**
     * Counts the crossings between an upper level and the level below it, both in their current order.
     *
     * @param upper Nodes of the upper level, duplicates as copy ids.
     * @param lower Nodes of the lower level, source ids in order.
     * @return Number of crossings.
     */
    [[nodiscard]] uint64_t count_crossings(const std::vector<mockturtle::node<Ntk>>& upper,
                                           const std::vector<mockturtle::node<Ntk>>& lower) const
    {
        const auto pos = positions(lower);

        std::vector<uint64_t> swept(lower.size() + 1, 0);
        std::size_t           max_pos   = 0;
        uint64_t              crossings = 0;

        for (const auto& n : upper)
        {
            std::vector<std::size_t> targets{};
            ntk.foreach_fanin(origin(n),
                              [&](const auto& f)
                              {
                                  if (const auto p = pos[ntk.get_node(f)]; p != std::numeric_limits<std::size_t>::max())
                                  {
                                      targets.push_back(p);
                                  }
                              });

            for (const auto p : targets)
            {
                for (auto k = p + 1; k <= max_pos; ++k)
                {
                    crossings += swept[k];
                }
            }

            for (const auto p : targets)
            {
                max_pos = std::max(max_pos, p);
                ++swept[p];
            }
        }

        return crossings;
    }
    /**
     * Orders a level by the barycenters of the positions its nodes are used from in the upper level, then applies up
     * to `max_swaps` adjacent swaps that reduce the crossing count.
     *
     * @param upper Nodes of the upper level, duplicates as copy ids.
     * @param lower Nodes of the lower level, source ids; reordered.
     */
    void minimize_crossings(const std::vector<mockturtle::node<Ntk>>& upper,
                            std::vector<mockturtle::node<Ntk>>&       lower) const
    {
        if (lower.size() < 2)
        {
            return;
        }

        const auto pos = positions(lower);

        std::vector<double>   sum(lower.size(), 0.0);
        std::vector<uint32_t> count(lower.size(), 0);

        for (std::size_t t = 0; t < upper.size(); ++t)
        {
            ntk.foreach_fanin(origin(upper[t]),
                              [&](const auto& f)
                              {
                                  if (const auto p = pos[ntk.get_node(f)]; p != std::numeric_limits<std::size_t>::max())
                                  {
                                      sum[p] += static_cast<double>(t);
                                      ++count[p];
                                  }
                              });
        }

        std::vector<std::size_t> order(lower.size());
        std::iota(order.begin(), order.end(), 0u);

        const auto key = [&](const std::size_t i)
        { return count[i] > 0 ? sum[i] / static_cast<double>(count[i]) : static_cast<double>(i); };

        std::stable_sort(order.begin(), order.end(),
                         [&key](const std::size_t a, const std::size_t b) { return key(a) < key(b); });

        std::vector<mockturtle::node<Ntk>> sorted{};
        sorted.reserve(lower.size());
        for (const auto i : order)
        {
            sorted.push_back(lower[i]);
        }
        lower = std::move(sorted);

        auto     current = count_crossings(upper, lower);
        uint32_t swaps   = 0;

        for (std::size_t i = 0; i + 1 < lower.size() && swaps < ps.max_swaps; ++i)
        {
            std::swap(lower[i], lower[i + 1]);

            if (const auto candidate = count_crossings(upper, lower); candidate < current)
            {
                current = candidate;
                ++swaps;
            }
            else
            {
                std::swap(lower[i], lower[i + 1]);
            }
        }
    }
    /**
     * Costs keeping the crossings between the upper level and a level in its original order: the level is reordered
     * to minimize crossings, then every crossing is charged one gadget and every edge the buffers that pad it to the
     * number of gadgets on the most crossed edge.
     *
     * @param upper Nodes of the upper level, duplicates as copy ids.
     * @param lower Nodes of the lower level in source order; reordered.
     * @return Cost and crossing count; the cost is infinite above `max_crossings_per_rank`.
     */
    [[nodiscard]] crossing_cost_result crossing_cost(const std::vector<mockturtle::node<Ntk>>& upper,
                                                     std::vector<mockturtle::node<Ntk>>&       lower) const
    {
        minimize_crossings(upper, lower);

        const auto pos = positions(lower);

        // crossings per edge, edges keyed by (upper node, position of its fanin)
        std::unordered_map<uint64_t, uint64_t> per_edge{};
        const auto                             edge_key = [](const mockturtle::node<Ntk> n, const std::size_t p)
        { return (static_cast<uint64_t>(n) << 32u) | static_cast<uint64_t>(p); };

        std::vector<std::vector<std::pair<uint64_t, uint64_t>>> swept(lower.size() + 1);
        std::size_t                                             max_pos       = 0;
        uint64_t                                                num_crossings = 0;
        uint64_t                                                max_per_edge  = 0;

        for (const auto& n : upper)
        {
            std::vector<std::size_t> targets{};
            ntk.foreach_fanin(origin(n),
                              [&](const auto& f)
                              {
                                  if (const auto p = pos[ntk.get_node(f)]; p != std::numeric_limits<std::size_t>::max())
                                  {
                                      targets.push_back(p);
                                  }
                              });

            for (const auto p : targets)
            {
                const auto key = edge_key(n, p);
                per_edge.try_emplace(key, 0);

                for (auto k = max_pos; k > p; --k)
                {
                    for (auto& [prev_key, prev_count] : swept[k])
                    {
                        ++num_crossings;
                        ++prev_count;
                        ++per_edge[prev_key];
                        ++per_edge[key];
                        max_per_edge = std::max({max_per_edge, per_edge[prev_key], per_edge[key]});
                    }
                }
            }

            for (const auto p : targets)
            {
                max_pos = std::max(max_pos, p);
                swept[p].emplace_back(edge_key(n, p), 0);
            }
        }

        if (num_crossings > ps.max_crossings_per_rank)
        {
            return {std::numeric_limits<uint64_t>::max(), num_crossings};
        }

        uint64_t cost = num_crossings * gadget_nodes();

        if (num_crossings > 0)
        {
            for (const auto& [key, count] : per_edge)
            {
                cost += (max_per_edge - count) * gadget_depth();
            }
        }

        return {cost, num_crossings};
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
    /**
     * Whether a level of `ntk_lvls` kept its crossings instead of its duplicates.
     */
    std::vector<bool> crossing_level{};
    /**
     * Unscaled cone weight per source node, filled on demand.
     */
    mutable std::unordered_map<mockturtle::node<Ntk>, double> cone_weights{};
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
 * With the hybrid strategy, the algorithm decides per level whether duplicating is cheaper than keeping the crossings
 * and resolving them later with `crossing_gate_planarization`: the duplication cost is the weighted size of the
 * duplicated cones, the crossing cost the size of the gadgets plus their padding after a crossing minimization of the
 * level. Levels that keep their crossings are not duplicated, and the result is then not planar.
 *
 * The input must be balanced with unified outputs (see `network_balancing`), carry ranks (see `mutable_rank_view`),
 * and contain no virtual primary inputs (see `delete_virtual_pis`). The result is ranked; with the duplication strategy
 * it is crossing-free, which `mincross` verifies before the function returns.
 *
 * @tparam Ntk Ranked, balanced source network type.
 * @param ntk Source network.
 * @param ps Parameters.
 * @param pst Statistics.
 * @return `virtual_pi_network` that computes the same functions as `ntk`; planar with the duplication strategy.
 * @throws std::invalid_argument If `ntk` is not balanced with unified outputs, contains virtual primary inputs, or
 * has a gate whose fanins are all constants.
 * @throws std::runtime_error If more than `max_duplications` nodes were duplicated, or if the result of the
 * duplication strategy still contains crossings.
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

    if (!is_balanced(ntk, {.unify_outputs = true}))
    {
        throw std::invalid_argument("The network must be balanced with unified outputs before planarization");
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

    if (ps.strategy == node_duplication_planarization_params::planarization_strategy::DUPLICATION)
    {
        utils::graph::mincross_params mc_ps{};
        mc_ps.optimize = false;
        utils::graph::mincross_stats mc_st{};

        utils::graph::mincross(result, mc_ps, &mc_st);

        if (mc_st.num_crossings != 0)
        {
            throw std::runtime_error(
                fmt::format("Planarization failed: the result contains {} crossings", mc_st.num_crossings));
        }
    }

    if (pst != nullptr)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction::synthesis
