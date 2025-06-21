//
// Created by benjamin on 08.04.25.
//

#ifndef FICTION_MINCROSS_HPP
#define FICTION_MINCROSS_HPP

#include <mockturtle/traits.hpp>
#include <mockturtle/views/fanout_view.hpp>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace fiction
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
};

/**
 * Statistics collected during the execution of the `mincross` algorithm.
 */
struct mincross_stats
{
    /**
     * The total number of edge crossings after optimization.
     */
    uint64_t num_crossings = UINT64_MAX;
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
    using node = typename Ntk::node;

    /**
     * Constructs the crossing minimization implementation object.
     *
     * @param src The logic network on which to perform crossing minimization.
     * @param optimize Whether to run the optimization procedure or only count current crossings.
     * @param p Configuration parameters for the algorithm.
     * @param st Statistics object to store the resulting number of crossings.
     */
    mincross_impl(const Ntk& src, bool optimize, const mincross_params p, mincross_stats& st) :
            ntk(src),
            fanout_ntk(src),
            opt(optimize),
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
        if (opt)
        {
            minimize_crossings();
        }
        ncross();
        pst.num_crossings = total_crossings;
        return ntk;
    }

  private:
    /**
     * Performs the main iterative crossing minimization using median and transpose heuristics.
     */
    void minimize_crossings()
    {
        constexpr int    max_iter    = 24;
        constexpr int    min_quit    = 4;
        constexpr double convergence = 0.995;

        uint64_t best_cross = UINT64_MAX;
        uint64_t cur_cross  = 0;

        ncross();
        cur_cross  = total_crossings;
        best_cross = cur_cross;

        std::vector<std::vector<node>> best_ranks = fanout_ntk.get_all_ranks();

        // Passes: 0 = init, 1 = refinement, 2 = full optimization
        for (int pass = 0; pass <= 2; ++pass)
        {
            const int max_this_pass = (pass <= 1) ? std::min(4, max_iter) : max_iter;
            int       trying        = 0;

            for (int iter = 0; iter < max_this_pass; ++iter)
            {
                if (trying++ >= min_quit || cur_cross == 0)
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
                    best_cross = cur_cross;
                    if (static_cast<double>(cur_cross) < convergence * static_cast<double>(best_cross))
                    {
                        trying = 0;
                    }
                }
            }

            // After the pass, restore best found ordering
            fanout_ntk.set_all_ranks(best_ranks);
            if (cur_cross == 0)
            {
                break;
            }
        }
        ntk.set_all_ranks(best_ranks);
    }

    /**
     * Executes one full up/down pass of median ordering followed by transposition to reduce crossings.
     *
     * @param pass The current pass number, determines direction and ordering.
     */
    void mincross_step(int pass)
    {
        const bool reverse = pass % 4 < 2;

        int max_rank = static_cast<int>(fanout_ntk.depth());

        int first = 0, last = 0, dir = 0;

        if (pass % 2 == 0)
        {  // down pass
            first = 1;
            last  = max_rank;
            dir   = 1;
        }
        else
        {  // up pass
            first = max_rank - 1;
            last  = 0;
            dir   = -1;
        }

        for (int r = first; r != last + dir; r += dir)
        {
            if (r == 0 && ps.fixed_pis)
            {
                continue;
            }

            const int other = r - dir;

            medians(static_cast<uint32_t>(r), static_cast<uint32_t>(other));

            reorder(static_cast<uint32_t>(r), reverse);
        }

        transpose(!reverse);
    }

    /**
     * Computes median values for the nodes in rank `r0` based on their connections to `r1`.
     *
     * @param r0 Current rank.
     * @param r1 Adjacent rank to which connections are considered.
     */
    void medians(uint32_t r0, uint32_t r1)
    {
        median_map.clear();

        // Loop over all nodes in rank r0
        fanout_ntk.foreach_node_in_rank(
            r0,
            [&](auto const& n)
            {
                std::vector<uint32_t> positions;

                if (r1 > r0)
                {
                    // Go forward: fanouts
                    fanout_ntk.foreach_fanout(n,
                                              [&](auto const& fo)
                                              {
                                                  if (fanout_ntk.level(fo) == r1)
                                                  {
                                                      positions.push_back(fanout_ntk.rank_position(fo));
                                                  }
                                              });
                }
                else
                {
                    // Go backward: fanins
                    fanout_ntk.foreach_fanin(n,
                                             [&](auto const& fi)
                                             {
                                                 const auto src = fanout_ntk.get_node(fi);
                                                 if (fanout_ntk.level(src) == r1)
                                                 {
                                                     positions.push_back(fanout_ntk.rank_position(src));
                                                 }
                                             });
                }

                if (positions.empty())
                {
                    median_map[n] = -1;  // No connections
                    return;
                }

                std::sort(positions.begin(), positions.end());

                if (positions.size() == 1)
                {
                    median_map[n] = positions[0];
                }
                else if (positions.size() == 2)
                {
                    median_map[n] = (positions[0] + positions[1]) / 2.0;
                }
                else
                {
                    const size_t j = positions.size();
                    if (j % 2 == 1)
                    {
                        median_map[n] = positions[j / 2];
                    }
                    else
                    {
                        // Weighted median
                        const size_t   rm    = j / 2;
                        const size_t   lm    = rm - 1;
                        const uint32_t lspan = positions[lm] - positions.front();
                        const uint32_t rspan = positions.back() - positions[rm];

                        if (lspan == rspan)
                        {
                            median_map[n] = (positions[lm] + positions[rm]) / 2.0;
                        }
                        else
                        {
                            const double w = (positions[lm] * static_cast<double>(rspan)) +
                                             (positions[rm] * static_cast<double>(lspan));
                            median_map[n] = w / (lspan + rspan);
                        }
                    }
                }
            });
    }

    /**
     * Reorders the nodes in a given rank according to computed medians.
     *
     * @param r The rank index.
     * @param reverse If true, sorts in descending order of medians.
     */
    void reorder(uint32_t r, bool reverse)
    {
        // Get the nodes at rank r
        auto rank = fanout_ntk.get_ranks(r);

        // Sort by median value
        std::sort(rank.begin(), rank.end(),
                  [&](auto const& n1, auto const& n2)
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

                      return reverse ? (m1 > m2) : (m1 < m2);
                  });

        // Re-assign sorted rank and update rank positions
        fanout_ntk.set_ranks(r, rank);
    }

    /**
     * Performs pairwise transpositions within ranks to further reduce crossings.
     *
     * @param reverse If true, applies reversed heuristic for tie-breaking.
     */
    void transpose(bool reverse)
    {
        std::vector<bool> candidate(fanout_ntk.depth(), true);
        int               delta          = 0;
        int               max_iterations = 0;

        while (true)
        {
            delta = 0;

            for (uint32_t l = 0; l <= fanout_ntk.depth(); ++l)
            {
                if (l == 0 && ps.fixed_pis)
                {
                    continue;
                }

                if (candidate[l])
                {
                    int d = transpose_step(l, reverse);
                    delta += d;

                    candidate[l] = (d > 0);  // if changed, keep candidate
                }
            }
            ++max_iterations;

            if (delta < 1 || max_iterations == 1000)
            {
                break;
            }
        }
    }

    /**
     * Performs a single transposition pass for rank `r`.
     *
     * @param r Rank index.
     * @param reverse If true, applies reversed heuristic for tie-breaking.
     * @return The number of crossings reduced.
     */
    int transpose_step(uint32_t r, bool reverse)
    {
        auto rank = fanout_ntk.get_ranks(r);

        if (rank.size() <= 1)
        {
            return 0;
        }

        int rv = 0;

        for (uint32_t i = 0; i < rank.size() - 1; ++i)
        {
            auto const& v = rank[i];
            auto const& w = rank[i + 1];

            // Ensure v comes before w
            assert(fanout_ntk.rank_position(v) < fanout_ntk.rank_position(w));

            int c0 = 0, c1 = 0;

            if (r > 0)
            {
                c0 += in_cross(v, w);
                c1 += in_cross(w, v);
            }

            if (r + 1 < fanout_ntk.depth())
            {
                c0 += out_cross(v, w);
                c1 += out_cross(w, v);
            }

            if (c1 < c0 || (c0 > 0 && reverse && c1 == c0))
            {
                fanout_ntk.swap(v, w);  // Perform the swap in the rank
                rv += (c0 - c1);
            }
        }

        return rv;
    }

    /**
     * Counts the number of edge crossings between fanins of two nodes in the previous rank.
     *
     * @param left First node.
     * @param right Second node.
     * @return Number of crossings.
     */
    int in_cross(const node& left, const node& right)
    {
        std::vector<uint32_t> left_sources, right_sources;

        fanout_ntk.foreach_fanin(left,
                                 [&](auto const& f)
                                 {
                                     auto src = fanout_ntk.get_node(f);
                                     left_sources.push_back(fanout_ntk.rank_position(src));
                                 });

        fanout_ntk.foreach_fanin(right,
                                 [&](auto const& f)
                                 {
                                     auto src = fanout_ntk.get_node(f);
                                     right_sources.push_back(fanout_ntk.rank_position(src));
                                 });

        return count_crossings(left_sources, right_sources);
    }

    /**
     * Counts the number of edge crossings between fanouts of two nodes in the next rank.
     *
     * @param left First node.
     * @param right Second node.
     * @return Number of crossings.
     */
    int out_cross(const node& left, const node& right)
    {
        std::vector<uint32_t> left_targets, right_targets;

        fanout_ntk.foreach_fanout(left,
                                  [&](auto const& tgt) { left_targets.push_back(fanout_ntk.rank_position(tgt)); });

        fanout_ntk.foreach_fanout(right,
                                  [&](auto const& tgt) { right_targets.push_back(fanout_ntk.rank_position(tgt)); });

        return count_crossings(left_targets, right_targets);
    }

    /**
     * Computes the number of crossings between two sets of ranked positions.
     *
     * @param a Positions from first set of connections.
     * @param b Positions from second set of connections.
     * @return Total number of crossings between the sets.
     */
    int count_crossings(const std::vector<uint32_t>& a, const std::vector<uint32_t>& b)
    {
        int count = 0;
        for (auto x : a)
        {
            for (auto y : b)
            {
                if (x > y)
                {
                    ++count;
                }
            }
        }
        return count;
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
                                                std::vector<uint64_t> targets;

                                                fanout_ntk.foreach_fanout(n,
                                                                          [&](auto const& fanout)
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

    // private:
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
    uint64_t total_crossings = UINT64_MAX;
    /**
     * Stores median values used to sort nodes within ranks during optimization.
     */
    std::unordered_map<node, double> median_map;
    /**
     * Flag indicating whether to perform optimization or only count crossings.
     */
    bool opt = true;
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
 * Reimplementation of Graphviz's `crossing.c` algorithm for edge crossing minimization.
 * This function reorders nodes in a leveled logic network to minimize the number of edge crossings using
 * iterative median and transpose heuristics.
 *
 * @tparam Ntk A logic network type with level and fanout support.
 * @param ntk The input leveled network whose ranks are to be reordered.
 * @param ps Configuration parameters for the minimization algorithm.
 * @param pst Optional pointer to a statistics structure for storing the resulting number of crossings.
 * @param optimize If false, skips optimization and only reports the current number of crossings.
 * @return A copy of the input network with reordered ranks to reduce edge crossings.
 */

template <typename Ntk>
Ntk mincross(const Ntk& ntk, const mincross_params& ps = {}, mincross_stats* pst = nullptr, bool optimize = true)
{
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
    static_assert(mockturtle::has_rank_position_v<Ntk>, "ntk is const!");
    static_assert(!std::is_const_v<Ntk>, "Ntk is const!");

    assert(is_balanced(ntk) && "Networks have to be balanced for this duplication");

    mincross_stats st{};
    mincross_impl  p{ntk, optimize, ps, st};

    auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_MINCROSS_HPP
