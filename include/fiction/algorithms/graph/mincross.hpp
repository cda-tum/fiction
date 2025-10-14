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
#include <limits>
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
            for (auto r = 1; r <= max_rank; ++r)
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
                std::vector<uint32_t> positions{};
                if (r1 > r0)
                {
                    // Go forward: fanouts
                    positions.reserve(fanout_ntk.fanout_size(n));
                    fanout_ntk.foreach_fanout(n,
                                              [this, &r1, &positions](auto const& fo)
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
                    positions.reserve(fanout_ntk.fanin_size(n));
                    fanout_ntk.foreach_fanin(n,
                                             [this, &r1, &positions](auto const& fi)
                                             {
                                                 const auto src = fanout_ntk.get_node(fi);
                                                 if (fanout_ntk.level(src) == r1 && !fanout_ntk.is_constant(src))
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
                    const std::size_t j = positions.size();
                    if (j % 2 == 1)
                    {
                        median_map[n] = positions[j / 2];
                    }
                    else
                    {
                        // Weighted median
                        const std::size_t rm    = j / 2;
                        const std::size_t lm    = rm - 1;
                        const uint32_t    lspan = positions[lm] - positions.front();
                        const uint32_t    rspan = positions.back() - positions[rm];

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
     * @param reverse If `true`, sorts in descending order of medians.
     */
    void reorder(const uint32_t r, median_sorting order)
    {
        // Get the nodes at rank r
        auto rank = fanout_ntk.get_ranks(r);

        // Sort by median value
        std::sort(rank.begin(), rank.end(),
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
     * Performs pairwise transpositions within ranks to further reduce crossings.
     *
     * @param reverse If `true`, applies reversed heuristic for tie-breaking.
     */
    void transpose(median_sorting order)
    {
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
    }

    /**
     * Performs a single transposition pass for rank `r`.
     *
     * @param r Rank index.
     * @param reverse If `true`, applies reversed heuristic for tie-breaking.
     * @return The number of crossings reduced.
     */
    uint32_t transpose_step(const uint32_t r, median_sorting order)
    {
        auto rank = fanout_ntk.get_ranks(r);

        if (rank.size() <= 1)
        {
            return 0;
        }

        uint32_t rv = 0;

        for (uint32_t i = 0; i < rank.size() - 1; ++i)
        {
            auto const& v = rank[i];
            auto const& w = rank[i + 1];

            // Ensure v comes before w
            assert(fanout_ntk.rank_position(v) < fanout_ntk.rank_position(w) &&
                   "invalid ranks: nodes must be ordered in ascending rank_position");

            uint32_t c0 = 0, c1 = 0;

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

            if (c1 < c0 || (c0 > 0 && order == median_sorting::DESCENDING && c1 == c0))
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
    uint32_t in_cross(const node& left, const node& right)
    {
        std::vector<uint32_t> left_sources{};
        left_sources.reserve(fanout_ntk.fanin_size(left));
        std::vector<uint32_t> right_sources{};
        right_sources.reserve(fanout_ntk.fanin_size(right));

        fanout_ntk.foreach_fanin(left,
                                 [this, &left_sources](auto const& f)
                                 {
                                     const auto src = fanout_ntk.get_node(f);
                                     if (fanout_ntk.is_constant(src))
                                     {
                                         return;
                                     }
                                     left_sources.push_back(fanout_ntk.rank_position(src));
                                 });

        fanout_ntk.foreach_fanin(right,
                                 [this, &right_sources](auto const& f)
                                 {
                                     const auto src = fanout_ntk.get_node(f);
                                     if (fanout_ntk.is_constant(src))
                                     {
                                         return;
                                     }
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
    uint32_t out_cross(const node& left, const node& right)
    {
        std::vector<uint32_t> left_targets{};
        left_targets.reserve(fanout_ntk.fanout_size(left));
        fanout_ntk.foreach_fanout(left, [this, &left_targets](auto const& tgt)
                                  { left_targets.push_back(fanout_ntk.rank_position(tgt)); });

        std::vector<uint32_t> right_targets{};
        right_targets.reserve(fanout_ntk.fanout_size(right));
        fanout_ntk.foreach_fanout(right, [this, &right_targets](auto const& tgt)
                                  { right_targets.push_back(fanout_ntk.rank_position(tgt)); });

        return count_crossings(left_targets, right_targets);
    }

    /**
     * Computes the number of crossings between two sets of ranked positions.
     *
     * @param a Positions from first set of connections.
     * @param b Positions from second set of connections.
     * @return Total number of crossings between the sets.
     */
    uint32_t count_crossings(const std::vector<uint32_t>& a, const std::vector<uint32_t>& b)
    {
        uint32_t count = 0;
        for (const auto x : a)
        {
            for (const auto y : b)
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

    assert(is_balanced(ntk) && "Networks have to be balanced for level-by-level crossing minimization");

    mincross_stats st{};
    mincross_impl  p{ntk, ps, st};

    auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_MINCROSS_HPP
