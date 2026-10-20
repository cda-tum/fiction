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
 * @brief Edge crossing minimization modeled after Graphviz's `mincross`.
 * @author Benjamin Hien (hibenj)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/synthesis/network_balancing.hpp"  // NOLINT(misc-include-cleaner): is_balanced is used in debug assertions.

#include <mockturtle/traits.hpp>
#include <mockturtle/views/fanout_view.hpp>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fiction::utils::graph
{

/**
 * Parameters for the `mincross` crossing minimization algorithm.
 */
struct mincross_params
{
    /**
     * Whether the rank positions of primary inputs (PIs) should remain fixed during the minimization process.
     * If set to `true`, PIs will not be reordered.
     */
    bool fixed_pis = false;
    /**
     * If `false`, skips optimization and only reports the current number of crossings.
     */
    bool optimize = true;
    /**
     * Maximum number of iterations per optimization pass (heuristic from Graphviz).
     * Larger values allow more refinement but increase runtime.
     * Default (`24`) works well for small and medium-sized networks.
     */
    uint64_t max_iter = 24;
    /**
     * Minimum number of consecutive iterations without sufficient improvement
     * before quitting early (heuristic from Graphviz). Prevents wasting time when the number of crossings
     * no longer decreases.
     */
    uint64_t min_quit = 8;
    /**
     * Convergence threshold: relative improvement factor required to reset the early-quit counter (heuristic from
     * Graphviz). If the current crossing count drops below (`convergence` * `best_cross`), where `best_cross` is the
     * best/lowest crossing count found so far, the process continues. Default is `0.995` (i.e., at least 0.5%
     * improvement required).
     */
    double convergence = 0.995;
    /**
     * Maximum number of iterations in the initial (pass 0) and refinement (pass 1)
     * phases of the crossing minimization procedure (heuristic from Graphviz).
     *
     * - In these early passes, the algorithm explores simple reorderings to quickly
     *   reduce crossings without investing in the full optimization effort.
     * - By default, the number of iterations is capped at `4` to prevent excessive
     *   runtime during initialization and refinement. This cap can be lifted if a
     *   larger global maximum (`ps.max_iter`) is set.
     * - In the full optimization pass (pass 2), `ps.max_iter` is always used instead.
     */
    uint64_t init_refine_max_iters = 4;
};

/**
 * Statistics collected during the execution of the `mincross` algorithm.
 */
struct mincross_stats
{
    /**
     * The total number of edge crossings after optimization.
     */
    uint64_t num_crossings = std::numeric_limits<uint64_t>::max();
};

/**
 * Implements the crossing minimization algorithm inspired by Graphviz's `mincross`.
 * This algorithm reorders nodes in ranks to reduce edge crossings in a leveled graph representation of the logic
 * network.
 *
 * @tparam Ntk Logic network type that models a leveled circuit with rank information.
 */
template <typename Ntk>
class mincross_impl
{
  public:
    /** @brief Network node type. */
    using node = typename Ntk::node;

    /**
     * Constructs the crossing minimization implementation object.
     *
     * @param src The logic network on which to perform crossing minimization.
     * @param p Configuration parameters for the algorithm (e.g., `optimize`)
     * @param st Statistics object to store the resulting number of crossings.
     */
    mincross_impl(const Ntk& src, const mincross_params p, mincross_stats& st) :
            ntk(src),
            fanout_ntk(src),
            ps(p),
            pst{st}
    {}

    /**
     * Runs the crossing minimization algorithm and returns a reordered network.
     *
     * @return A network with reordered nodes in ranks to reduce edge crossings.
     */
    [[nodiscard]] Ntk run()
    {
        if (ps.optimize)
        {
            minimize_crossings();
        }
        ncross();
        pst.num_crossings = total_crossings;
        return ntk;
    }

  private:
    static constexpr uint32_t MAX_TRANSPOSE_ITERATIONS = 1000;

    enum class median_sorting : uint8_t
    {
        ASCENDING,
        DESCENDING
    };

    /**
     * Performs the main iterative crossing minimization using median and transpose heuristics.
     */
    void minimize_crossings()
    {
        ncross();
        auto cur_cross  = total_crossings;
        auto best_cross = cur_cross;

        auto best_ranks = fanout_ntk.get_all_ranks();

        // Passes: 0 = init, 1 = refinement, 2 = full optimization
        for (uint64_t pass = 0; pass <= 2; ++pass)
        {
            const auto max_this_pass = (pass <= 1) ? std::min(ps.init_refine_max_iters, ps.max_iter) : ps.max_iter;
            uint64_t   trying        = 0;

            for (uint64_t iter = 0; iter < max_this_pass; ++iter)
            {
                if (trying++ >= ps.min_quit || cur_cross == 0)
                {
                    break;
                }

                // Core step: reorder ranks to reduce crossings
                mincross_step(iter);

                // Recompute crossing count
                ncross();
                cur_cross = total_crossings;

                if (cur_cross <= best_cross)
                {
                    best_ranks = fanout_ntk.get_all_ranks();
                    // Note: Conversion to double is safe here since best_cross < 2^53 in all practical cases.
                    if (static_cast<double>(cur_cross) < ps.convergence * static_cast<double>(best_cross))
                    {
                        trying = 0;
                    }
                    best_cross = cur_cross;
                }
            }

            // After the pass, restore best found ordering
            fanout_ntk.set_all_ranks(best_ranks);
            if (cur_cross == 0)
            {
                break;
            }
        }

        if (best_cross > 0)
        {
            transpose(median_sorting::ASCENDING);
            ncross();
            cur_cross = total_crossings;
            if (cur_cross < best_cross)
            {
                best_ranks = fanout_ntk.get_all_ranks();
            }
        }
        fanout_ntk.set_all_ranks(best_ranks);
        ntk.set_all_ranks(best_ranks);
    }

    /**
     * Executes one full up/down pass of median ordering followed by transposition to reduce crossings.
     *
     * @param pass The current pass number, determines direction and ordering.
     */
    void mincross_step(const uint64_t pass)
    {
        // Graphviz toggles the sorting direction depending on the pass number
        const auto order    = (pass % 4 < 2) ? median_sorting::ASCENDING : median_sorting::DESCENDING;
        const auto max_rank = fanout_ntk.depth();

        if (pass % 2 == 0)
        {
            // Upward pass: from rank 1 to max_rank
            for (uint32_t r = 1; r <= max_rank; ++r)
            {
                if (r == 0 && ps.fixed_pis)
                {
                    continue;
                }

                const auto other = r - 1;
                medians(r, other);
                reorder(r, order);
            }
        }
        else
        {
            // Downward pass: from max_rank - 1 down to 0
            for (auto r = max_rank - 1; r != static_cast<uint32_t>(-1); --r)
            {
                if (r == 0 && ps.fixed_pis)
                {
                    continue;
                }

                const auto other = r + 1;
                medians(r, other);
                reorder(r, order);
            }
        }

        transpose(order);
    }

    /**
     * Computes median values for the nodes in rank `r0` based on their connections to `r1`.
     *
     * @param r0 Current rank.
     * @param r1 Adjacent rank to which connections are considered.
     */
    void medians(const uint32_t r0, const uint32_t r1)
    {
        median_map.clear();

        // Loop over all nodes in rank r0
        fanout_ntk.foreach_node_in_rank(
            r0,
            [this, &r0, &r1](auto const& n)
            {
                std::vector<uint32_t> neighbor_positions{};
                if (r1 > r0)
                {
                    // Go forward: fanouts
                    neighbor_positions.reserve(fanout_ntk.fanout_size(n));
                    fanout_ntk.foreach_fanout(n,
                                              [this, &r1, &neighbor_positions](auto const& fo)
                                              {
                                                  if (fanout_ntk.level(fo) == r1)
                                                  {
                                                      neighbor_positions.push_back(fanout_ntk.rank_position(fo));
                                                  }
                                              });
                }
                else
                {
                    // Go backward: fanins
                    neighbor_positions.reserve(fanout_ntk.fanin_size(n));
                    fanout_ntk.foreach_fanin(n,
                                             [this, &r1, &neighbor_positions](auto const& fi)
                                             {
                                                 const auto src = fanout_ntk.get_node(fi);
                                                 if (fanout_ntk.level(src) == r1 && !fanout_ntk.is_constant(src))
                                                 {
                                                     neighbor_positions.push_back(fanout_ntk.rank_position(src));
                                                 }
                                             });
                }

                if (neighbor_positions.empty())
                {
                    median_map[n] = -1;  // No connections
                    return;
                }

                std::ranges::sort(neighbor_positions);

                if (neighbor_positions.size() == 1)
                {
                    median_map[n] = neighbor_positions[0];
                }
                else if (neighbor_positions.size() == 2)
                {
                    median_map[n] = (neighbor_positions[0] + neighbor_positions[1]) / 2.0;
                }
                else
                {
                    const std::size_t j = neighbor_positions.size();
                    if (j % 2 == 1)
                    {
                        median_map[n] = neighbor_positions[j / 2];
                    }
                    else
                    {
                        // Weighted median
                        const std::size_t rm    = j / 2;
                        const std::size_t lm    = rm - 1;
                        const uint32_t    lspan = neighbor_positions[lm] - neighbor_positions.front();
                        const uint32_t    rspan = neighbor_positions.back() - neighbor_positions[rm];

                        if (lspan == rspan)
                        {
                            median_map[n] = (neighbor_positions[lm] + neighbor_positions[rm]) / 2.0;
                        }
                        else
                        {
                            const double w = (neighbor_positions[lm] * static_cast<double>(rspan)) +
                                             (neighbor_positions[rm] * static_cast<double>(lspan));
                            median_map[n]  = w / (lspan + rspan);
                        }
                    }
                }
            });
    }

    /**
     * Reorders the nodes in a given rank according to computed medians.
     *
     * @param r The rank index.
     * @param order Sorting order of medians.
     */
    void reorder(const uint32_t r, const median_sorting order)
    {
        // Get the nodes at rank r
        auto rank = fanout_ntk.get_ranks(r);

        // Sort by median value
        std::ranges::sort(rank,
                          [this, order](auto const& n1, auto const& n2)
                          {
                              const double m1 = median_map[n1];
                              const double m2 = median_map[n2];

                              // Handle -1 (no connections) as infinity to push to the back
                              if (m1 == -1)
                              {
                                  return false;
                              }
                              if (m2 == -1)
                              {
                                  return true;
                              }

                              return order == median_sorting::DESCENDING ? (m1 > m2) : (m1 < m2);
                          });

        // Re-assign sorted rank and update rank positions
        fanout_ntk.set_ranks(r, rank);
    }

    /**
     * @brief Performs pairwise transpositions within ranks to further reduce crossings.
     *
     * The sweeps run on flat copies of the ranks, the rank positions, and the neighbor lists; the ranks of the network
     * are written back at the end.
     *
     * @param order Sorting heuristic for tie-breaking.
     */
    void transpose(const median_sorting order)
    {
        build_neighbors();

        ranks = fanout_ntk.get_all_ranks();

        positions.assign(fanout_ntk.size(), 0);
        for (const auto& rank : ranks)
        {
            for (std::size_t i = 0; i < rank.size(); ++i)
            {
                positions[index_of(rank[i])] = static_cast<uint32_t>(i);
            }
        }

        std::vector<uint8_t> candidate(fanout_ntk.depth() + 1, 1);
        uint32_t             delta          = 0;
        uint32_t             max_iterations = 0;

        while (true)
        {
            delta = 0;

            for (uint32_t l = 0; l <= fanout_ntk.depth(); ++l)
            {
                if (l == 0 && ps.fixed_pis)
                {
                    continue;
                }

                if (candidate[l] != 0u)
                {
                    auto d = transpose_step(l, order);
                    delta += d;

                    candidate[l] = (d > 0) ? 1 : 0;  // if changed, keep candidate
                }
            }
            ++max_iterations;

            if (delta < 1 || max_iterations == MAX_TRANSPOSE_ITERATIONS)
            {
                break;
            }
        }

        fanout_ntk.set_all_ranks(ranks);
    }

    /**
     * Performs a single transposition pass for rank `r`.
     *
     * @param r Rank index.
     * @param order Sorting heuristic for tie-breaking.
     * @return The number of crossings reduced.
     */
    uint32_t transpose_step(const uint32_t r, const median_sorting order)
    {
        // the pairs of the pass are taken from the order at its start
        const auto rank = ranks[r];

        if (rank.size() <= 1)
        {
            return 0;
        }

        const bool has_fanins  = r > 0;
        const bool has_fanouts = r + 1 < fanout_ntk.depth();

        uint32_t rv = 0;

        for (uint32_t i = 0; i < rank.size() - 1; ++i)
        {
            const auto v = index_of(rank[i]);
            const auto w = index_of(rank[i + 1]);

            uint32_t c0 = 0, c1 = 0;

            if (has_fanins)
            {
                count_crossings(in_begin, in_nodes, v, w, c0, c1);
            }

            if (has_fanouts)
            {
                count_crossings(out_begin, out_nodes, v, w, c0, c1);
            }

            if (c1 < c0 || (c0 > 0 && order == median_sorting::DESCENDING && c1 == c0))
            {
                std::swap(ranks[r][positions[v]], ranks[r][positions[w]]);
                std::swap(positions[v], positions[w]);
                rv += (c0 - c1);
            }
        }

        return rv;
    }

    /**
     * @brief Index of a node in the flat arrays.
     *
     *
     * @param n Node.
     * @return Index of `n`.
     */
    [[nodiscard]] std::size_t index_of(const node& n) const
    {
        return static_cast<std::size_t>(fanout_ntk.node_to_index(n));
    }

    /**
     * @brief Collects the non-constant fanins and the fanouts of every node in flat arrays, once.
     */
    void build_neighbors()
    {
        if (!in_begin.empty())
        {
            return;
        }

        const auto size = static_cast<std::size_t>(fanout_ntk.size());

        in_begin.assign(size + 1, 0);
        out_begin.assign(size + 1, 0);

        for (std::size_t i = 0; i < size; ++i)
        {
            const node n = fanout_ntk.index_to_node(static_cast<uint32_t>(i));

            if (!fanout_ntk.is_constant(n))
            {
                fanout_ntk.foreach_fanin(n,
                                         [this](auto const& f)
                                         {
                                             const auto src = fanout_ntk.get_node(f);
                                             if (!fanout_ntk.is_constant(src))
                                             {
                                                 in_nodes.push_back(static_cast<uint32_t>(index_of(src)));
                                             }
                                         });
                fanout_ntk.foreach_fanout(n, [this](auto const& tgt)
                                          { out_nodes.push_back(static_cast<uint32_t>(index_of(tgt))); });
            }

            in_begin[i + 1]  = static_cast<uint32_t>(in_nodes.size());
            out_begin[i + 1] = static_cast<uint32_t>(out_nodes.size());
        }
    }

    /**
     * @brief Adds the crossings between the edges of two nodes to a pair of counters, for both orders of the nodes.
     *
     * The neighbors of `left` and `right` in an adjacent rank are compared by their rank positions.
     *
     * @param begin Offsets of the neighbor lists.
     * @param neighbors Neighbor lists.
     * @param left Index of the left node.
     * @param right Index of the right node.
     * @param kept Receives the crossings with `left` left of `right`.
     * @param swapped Receives the crossings with the nodes swapped.
     */
    void count_crossings(const std::vector<uint32_t>& begin, const std::vector<uint32_t>& neighbors,
                         const std::size_t left, const std::size_t right, uint32_t& kept, uint32_t& swapped) const
    {
        for (auto a = begin[left]; a < begin[left + 1]; ++a)
        {
            const auto x = positions[neighbors[a]];

            for (auto b = begin[right]; b < begin[right + 1]; ++b)
            {
                const auto y = positions[neighbors[b]];

                kept += static_cast<uint32_t>(x > y);
                swapped += static_cast<uint32_t>(x < y);
            }
        }
    }

    /**
     * Computes the total number of edge crossings in the current network state.
     */
    void ncross()
    {
        total_crossings = 0;

        for (uint32_t r = 0; r < fanout_ntk.depth(); ++r)
        {
            std::vector<uint64_t> penalty_array(fanout_ntk.rank_width(r + 1) + 1, 0);
            uint64_t              max_pos = 0;

            fanout_ntk.foreach_node_in_rank(r,
                                            [this, &penalty_array, &max_pos](auto const& n)
                                            {
                                                std::vector<uint64_t> targets{};
                                                targets.reserve(fanout_ntk.fanout_size(n));

                                                fanout_ntk.foreach_fanout(n,
                                                                          [this, &targets](auto const& fanout)
                                                                          {
                                                                              uint64_t pos =
                                                                                  fanout_ntk.rank_position(fanout);
                                                                              targets.push_back(pos);
                                                                          });

                                                for (const auto pos : targets)
                                                {
                                                    for (auto k = pos + 1; k <= max_pos; ++k)
                                                    {
                                                        total_crossings += penalty_array[k];
                                                    }
                                                }

                                                for (const auto pos : targets)
                                                {
                                                    max_pos = std::max(max_pos, pos);
                                                    penalty_array[pos]++;
                                                }
                                            });
        }
    }

    /**
     * Logic network being reordered.
     */
    Ntk ntk;
    /**
     * Fanout-augmented view of the network for efficient traversal and connectivity queries.
     */
    mockturtle::fanout_view<Ntk> fanout_ntk;
    /**
     * Current total number of edge crossings in the network.
     */
    uint64_t total_crossings = std::numeric_limits<uint64_t>::max();
    /**
     * @brief Nodes of every rank while the transposition sweeps run.
     */
    std::vector<std::vector<node>> ranks{};
    /**
     * @brief Position within its rank per node index while the transposition sweeps run.
     */
    std::vector<uint32_t> positions{};
    /**
     * @brief Offsets of the fanin lists in `in_nodes`: node `i` has the entries `in_begin[i]` to `in_begin[i + 1]`.
     */
    std::vector<uint32_t> in_begin{};
    /**
     * @brief Node indices of the non-constant fanins of all nodes.
     */
    std::vector<uint32_t> in_nodes{};
    /**
     * @brief Offsets of the fanout lists in `out_nodes`, like `in_begin`.
     */
    std::vector<uint32_t> out_begin{};
    /**
     * @brief Node indices of the fanouts of all nodes.
     */
    std::vector<uint32_t> out_nodes{};
    /**
     * Stores median values used to sort nodes within ranks during optimization.
     */
    std::unordered_map<node, double> median_map;
    /**
     * Parameters for crossing minimization.
     */
    mincross_params ps;
    /**
     * Statistics that store the final number of crossings after optimization.
     */
    mincross_stats& pst;
};

/**
 * Reimplementation of Graphviz's `mincross` algorithm for edge crossing minimization. This function reorders nodes in a
 * leveled logic network to minimize the number of edge crossings using iterative median and transpose heuristics.
 *
 * Reference implementation:
 * https://gitlab.com/graphviz/graphviz/-/blob/main/lib/dotgen/mincross.c
 *
 * For more on Graphviz's `dot` layout generation:
 * https://graphviz.org/docs/layouts/dot/
 *
 * @tparam Ntk A logic network type with level and fanout support.
 * @param ntk The input leveled network whose ranks are to be reordered.
 * @param ps Configuration parameters for the minimization algorithm.
 * @param pst Optional pointer to a statistics structure for storing the resulting number of crossings.
 * @return A copy of the input network with reordered ranks to reduce edge crossings.
 */
template <typename Ntk>
Ntk mincross(Ntk& ntk, const mincross_params& ps = {}, mincross_stats* pst = nullptr)
{
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
    static_assert(mockturtle::has_rank_position_v<Ntk>, "Ntk does not implement the rank_position function");

    assert(fiction::synthesis::is_balanced(ntk) &&
           "Networks have to be balanced for level-by-level crossing minimization");

    mincross_stats st{};
    mincross_impl  p{ntk, ps, st};

    auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

/**
 * @brief Counts the edge crossings between every pair of adjacent levels of a ranked, balanced network in its current
 * rank order.
 *
 * In contrast to `mincross` with `optimize = false`, the network is neither copied nor reordered.
 *
 * @tparam Ntk A ranked network type.
 * @param ntk The balanced network whose crossings are counted.
 * @return The number of crossings.
 * @throws std::invalid_argument If an edge does not connect adjacent levels, i.e., if `ntk` is not balanced.
 */
template <typename Ntk>
[[nodiscard]] uint64_t count_crossings(const Ntk& ntk)
{
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
    static_assert(mockturtle::has_rank_position_v<Ntk>, "Ntk does not implement the rank_position function");
    static_assert(mockturtle::has_level_v<Ntk>, "Ntk does not implement the level function");

    uint64_t crossings = 0;

    for (uint32_t r = 0; r < ntk.depth(); ++r)
    {
        // positions in level `r + 1` that every node of level `r` drives, one entry per edge
        std::vector<std::vector<uint32_t>> targets(ntk.rank_width(r));

        ntk.foreach_node_in_rank(r + 1,
                                 [&ntk, &targets, r](const auto& t)
                                 {
                                     ntk.foreach_fanin(t,
                                                       [&ntk, &targets, r, pos = ntk.rank_position(t)](const auto& f)
                                                       {
                                                           const auto s = ntk.get_node(f);

                                                           if (ntk.is_constant(s))
                                                           {
                                                               return;
                                                           }

                                                           if (ntk.level(s) != r)
                                                           {
                                                               throw std::invalid_argument(
                                                                   "An edge does not connect adjacent levels");
                                                           }

                                                           targets[ntk.rank_position(s)].push_back(pos);
                                                       });
                                 });

        // sweep the sources from left to right; an edge that ends left of an earlier edge's end crosses it. A Fenwick
        // tree over the target positions counts the earlier edges that end right of a position.
        const auto width = static_cast<std::size_t>(ntk.rank_width(r + 1));

        std::vector<uint64_t> fenwick(width + 1, 0);
        uint64_t              swept = 0;

        for (const auto& edges : targets)
        {
            for (const auto pos : edges)
            {
                uint64_t not_right = 0;

                for (auto i = static_cast<std::size_t>(pos) + 1; i > 0; i -= i & (~i + 1))
                {
                    not_right += fenwick[i];
                }

                crossings += swept - not_right;
            }

            // edges of one source do not cross each other, so they are counted once all of them are queried
            for (const auto pos : edges)
            {
                for (auto i = static_cast<std::size_t>(pos) + 1; i <= width; i += i & (~i + 1))
                {
                    ++fenwick[i];
                }

                ++swept;
            }
        }
    }

    return crossings;
}

}  // namespace fiction::utils::graph
