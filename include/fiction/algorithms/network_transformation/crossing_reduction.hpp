//
// Created by marcel on 18.01.23.
//

#ifndef FICTION_CROSSING_REDUCTION_HPP
#define FICTION_CROSSING_REDUCTION_HPP

#include "fiction/algorithms/optimization/simulated_annealing.hpp"
#include "fiction/utils/network_utils.hpp"

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/views/rank_view.hpp>

#include <cassert>
#include <cstdint>
#include <random>
#include <vector>

#include <combinations.h>

namespace fiction
{

/**
 * Parameters for the Simulated Annealing crossing reduction.
 */
struct crossing_reduction_params
{
    /**
     * Initial temperature.
     */
    double initial_temperature{100.0};
    /**
     * Final temperature.
     */
    double final_temperature{1.0};
    /**
     * Number of iterations per temperature.
     */
    std::size_t number_of_cycles{10};
    /**
     * Number of parallel runs to pick the overall best result from.
     */
    std::size_t number_of_instances{50};
};
/**
 * Statistics for the Simulated Annealing placer.
 */
struct crossing_reduction_stats
{
    /**
     * Runtime measurement.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * Number of crossings before the reduction.
     */
    std::size_t crossings_before{0};
    /**
     * Number of crossings after the reduction.
     */
    std::size_t crossings_after{0};
};

namespace detail
{

struct node_pos
{
    double x, y;
};

template <typename Ntk>
[[nodiscard]] node_pos get_node_pos(const mockturtle::rank_view<Ntk>& ntk, mockturtle::node<Ntk> const& n) noexcept
{
    return {static_cast<double>(ntk.rank_position(n)), static_cast<double>(ntk.level(n))};
}
/**
 * Computes whether two lines share a crossing point. The lines are defined by the endpoints of the given logic network
 * nodes.
 *
 * This function uses Cramer's Rule to solve a linear equation system. The code is based on:
 * https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
 *
 * @tparam Ntk Network type.
 * @param ntk The network.
 * @param src1 The source node of the first line.
 * @param tgt1 The target node of the first line.
 * @param src2 The source node of the second line.
 * @param tgt2 The target node of the second line.
 * @return True iff the lines share a crossing point.
 */
template <typename Ntk>
[[nodiscard]] bool is_straight_line_crossing(const mockturtle::rank_view<Ntk>& ntk, mockturtle::node<Ntk> const& src1,
                                             mockturtle::node<Ntk> const& tgt1, mockturtle::node<Ntk> const& src2,
                                             mockturtle::node<Ntk> const& tgt2) noexcept
{
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");

    const auto p_src1 = get_node_pos(ntk, src1);
    const auto p_tgt1 = get_node_pos(ntk, tgt1);
    const auto p_src2 = get_node_pos(ntk, src2);
    const auto p_tgt2 = get_node_pos(ntk, tgt2);

    const double s1_x = p_tgt1.x - p_src1.x;
    const double s1_y = p_tgt1.y - p_src1.y;
    const double s2_x = p_tgt2.x - p_src2.x;
    const double s2_y = p_tgt2.y - p_src2.y;

    const double s = (-s1_y * (p_src1.x - p_src2.x) + s1_x * (p_src1.y - p_src2.y)) / (-s2_x * s1_y + s1_x * s2_y);
    const double t = (s2_x * (p_src1.y - p_src2.y) - s2_y * (p_src1.x - p_src2.x)) / (-s2_x * s1_y + s1_x * s2_y);

    // do not use >= / <= here to not consider lines that share the same endpoints as a crossing
    return (s > 0 && s < 1 && t > 0 && t < 1);
}
/**
 * The same as above, but for two edges instead of four nodes.
 *
 * @tparam Ntk The network type.
 * @param ntk The network.
 * @param e1 First edge.
 * @param e2 Second edge
 * @return True iff the edges share a crossing point.
 */
template <typename Ntk>
[[nodiscard]] bool is_straight_line_crossing(const mockturtle::rank_view<Ntk>& ntk, const mockturtle::edge<Ntk>& e1,
                                             const mockturtle::edge<Ntk>& e2) noexcept
{
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");

    return is_straight_line_crossing(ntk, e1.source, e1.target, e2.source, e2.target);
}

template <typename Ntk>
[[nodiscard]] std::size_t number_of_crossings_between_ranks(const mockturtle::rank_view<Ntk>& ntk, uint32_t r1,
                                                            uint32_t r2) noexcept
{
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");

    // there are no crossings with itself
    if (r1 == r2)
    {
        return 0;
    }
    // make sure that r1 is the lower rank
    if (r1 > r2)
    {
        std::swap(r1, r2);
    }

    assert(r1 + 1 == r2 && "r1 and r2 must be consecutive ranks");

    std::size_t crossings{0};

    // store all edges between ranks r1 and r2
    std::vector<mockturtle::edge<Ntk>> edges{};
    // since we know the maximum fanin size of Ntk, we can reserve sufficient space
    edges.reserve(ntk.rank_width(r2) * Ntk::max_fanin_size);

    // gather all edges between ranks r1 and r2 by iterating over all nodes in rank r2 and checking their fanins
    ntk.foreach_node_in_rank(r2,
                             [&ntk, &edges, &r1](const auto& n2)
                             {
                                 ntk.foreach_fanin(n2,
                                                   [&ntk, &edges, &r1, &n2](const auto& s1)
                                                   {
                                                       if (const auto n1 = ntk.get_node(s1);
                                                           !ntk.is_constant(n1) && ntk.level(n1) == r1)
                                                       {
                                                           edges.push_back({n1, n2});
                                                       }
                                                   });
                             });

    if (edges.size() >= 2)
    {
        // check all pairwise edge combinations for crossings
        combinations::for_each_combination(edges.begin(), edges.begin() + 2, edges.end(),
                                           [&ntk, &crossings](const auto begin, [[maybe_unused]] const auto end)
                                           {
                                               if (is_straight_line_crossing(ntk, *begin, *(begin + 1)))
                                               {
                                                   ++crossings;
                                               }

                                               return false;  // keep looping
                                           });
    }

    return crossings;
}

template <typename Ntk>
[[nodiscard]] std::size_t number_of_crossings_with_adjacent_ranks(const mockturtle::rank_view<Ntk>& ntk,
                                                                  const uint32_t                    r) noexcept
{
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");

    std::size_t crossings{0};

    // if r is the last rank, there are no crossings with the next rank
    if (r != ntk.depth())
    {
        // add crossings with rank r + 1
        crossings += number_of_crossings_between_ranks(ntk, r, r + 1);
    }

    // if r is 0, there are no crossings with the previous rank
    if (r != 0)
    {
        // add crossings with rank r - 1
        crossings += number_of_crossings_between_ranks(ntk, r - 1, r);
    }

    return crossings;
}

template <typename Ntk>
std::size_t number_of_crossings(const mockturtle::rank_view<Ntk>& ntk) noexcept
{
    std::size_t crossings = 0;

    for (uint32_t r = 0; r < ntk.depth() + 1; r += 2)  // only check every second rank to avoid double counting
    {
        crossings += number_of_crossings_with_adjacent_ranks(ntk, r);
    }

    return crossings;
}

template <typename Ntk>
class crossing_reduction_impl
{
  public:
    crossing_reduction_impl(const Ntk& ntk, const crossing_reduction_params& p, crossing_reduction_stats& s) :
            rank_ntk{ntk},
            ps{p},
            pst{s}
    {}

    mockturtle::rank_view<Ntk> run()
    {
        mockturtle::stopwatch stop{pst.time_total};

        pst.crossings_before = number_of_crossings(rank_ntk);

        auto rank_ntk_opt = rank_ntk;

        // perform one upward sweep
        for (uint32_t r = 0; r < rank_ntk_opt.depth(); ++r)
        {
            // initialize rank r with a random permutation
            const auto random_rank_order = [&r, &rank_ntk_opt]() -> mockturtle::rank_view<Ntk>
            {
                auto rank_ntk_copy = rank_ntk_opt;
                // shuffle nodes in rank r
                rank_ntk_copy.shuffle_rank(r);

                return rank_ntk_copy;
            };

            // determine the crossings of rank r
            const auto crossing_cost = [&r](const mockturtle::rank_view<Ntk>& ntk) -> int64_t
            { return static_cast<int64_t>(number_of_crossings_with_adjacent_ranks(ntk, r)); };

            // swap two nodes to generate the next order
            const auto next_rank_order = [&r](const mockturtle::rank_view<Ntk>& ntk) -> mockturtle::rank_view<Ntk>
            {
                static std::mt19937                     generator(std::random_device{}());
                std::uniform_int_distribution<uint32_t> distribution(0, ntk.rank_width(r) - 1);

                auto rank_ntk_copy = ntk;

                // swap two random nodes in rank r
                rank_ntk_copy.swap(rank_ntk_copy.at_rank_position(r, distribution(generator)),
                                   rank_ntk_copy.at_rank_position(r, distribution(generator)));

                return rank_ntk_copy;
            };

            const auto [result, cost] = multi_simulated_annealing(
                ps.initial_temperature, ps.final_temperature, ps.number_of_cycles, ps.number_of_instances,
                random_rank_order, crossing_cost, linear_temperature_schedule, next_rank_order);

            //            const auto [result, cost] =
            //                simulated_annealing(rank_ntk, ps.initial_temperature, ps.final_temperature,
            //                ps.number_of_cycles,
            //                                    crossing_cost, linear_temperature_schedule, next_rank_order);

            rank_ntk_opt = result;
        }

        pst.crossings_after = number_of_crossings(rank_ntk_opt);

        if (pst.crossings_after < pst.crossings_before)
        {
            return rank_ntk_opt;
        }
        else
        {
            pst.crossings_after = pst.crossings_before;

            return rank_ntk;
        }
    }

  private:
    /**
     * The network whose nodes crossings are to be minimized.
     */
    mockturtle::rank_view<Ntk> rank_ntk;
    /**
     * Parameters.
     */
    const crossing_reduction_params& ps;
    /**
     * Statistics.
     */
    crossing_reduction_stats& pst;
};

}  // namespace detail

template <typename Ntk>
[[nodiscard]] mockturtle::rank_view<Ntk> crossing_reduction(const Ntk& ntk, const crossing_reduction_params& ps = {},
                                                            crossing_reduction_stats* pst = nullptr)
{
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");

    crossing_reduction_stats st{};

    detail::crossing_reduction_impl<Ntk> p{ntk, ps, st};

    const auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_CROSSING_REDUCTION_HPP
