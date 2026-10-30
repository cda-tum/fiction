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
 * @brief Costs the crossing gates that keep the crossings between two adjacent levels instead of duplicating nodes.
 * @author Benjamin Hien (hibenj)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/synthesis/crossing_gate_planarization.hpp"

#include <mockturtle/traits.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <numeric>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fiction::synthesis::detail
{

/**
 * @brief Result of costing the crossings between two adjacent levels.
 */
struct crossing_cost_result
{
    /**
     * @brief Estimated number of nodes the crossing gates and their padding add.
     */
    uint64_t cost;
    /**
     * @brief Number of crossings between the two levels.
     */
    uint64_t num_crossings;
};

/**
 * @brief Number of nodes one crossing gate adds.
 *
 * @param xor_gates Whether the crossing gate consists of XOR gates.
 * @return Crossing gate size.
 */
[[nodiscard]] constexpr uint64_t crossing_gate_nodes(const bool xor_gates) noexcept
{
    return xor_gates ? XOR_CROSSING_GATE_NODES : AND_OR_CROSSING_GATE_NODES;
}

/**
 * @brief Number of levels one crossing gate spans.
 *
 * @param xor_gates Whether the crossing gate consists of XOR gates.
 * @return Crossing gate depth.
 */
[[nodiscard]] constexpr uint64_t crossing_gate_depth(const bool xor_gates) noexcept
{
    return xor_gates ? XOR_CROSSING_GATE_DEPTH : AND_OR_CROSSING_GATE_DEPTH;
}

/**
 * @brief Positions of the nodes of a level, indexed by node.
 *
 * @tparam Ntk Network type.
 * @param ntk Network the nodes belong to.
 * @param level Nodes of the level in order.
 * @return Position per node, the maximum `std::size_t` for nodes not in the level.
 */
template <typename Ntk>
[[nodiscard]] std::vector<std::size_t> level_positions(const Ntk& ntk, const std::vector<mockturtle::node<Ntk>>& level)
{
    std::vector<std::size_t> pos(ntk.size(), std::numeric_limits<std::size_t>::max());

    for (std::size_t i = 0; i < level.size(); ++i)
    {
        pos[level[i]] = i;
    }

    return pos;
}

/**
 * @brief Positions in the lower level of the fanins of a node of the upper level.
 *
 * @tparam Ntk Network type.
 * @param ntk Network the nodes belong to.
 * @param n Node of the upper level.
 * @param pos Position of every node in the lower level, see `level_positions`.
 * @return Positions of the fanins of `n` in fanin order; fanins outside the lower level, such as constants, are
 * skipped.
 */
template <typename Ntk>
[[nodiscard]] std::vector<std::size_t> fanin_positions(const Ntk& ntk, const mockturtle::node<Ntk> n,
                                                       const std::vector<std::size_t>& pos)
{
    std::vector<std::size_t> targets{};

    ntk.foreach_fanin(n,
                      [&ntk, &pos, &targets](const auto& f)
                      {
                          if (const auto p = pos[ntk.get_node(f)]; p != std::numeric_limits<std::size_t>::max())
                          {
                              targets.push_back(p);
                          }
                      });

    return targets;
}

/**
 * @brief Orders a level by the barycenters of the positions its nodes are used from in the upper level, then applies up
 * to `max_swaps` adjacent swaps that reduce the crossing count.
 *
 * A swap of two neighbours only changes the crossings between their own edges, so each candidate is evaluated on those
 * edges alone.
 *
 * @tparam Ntk Network type.
 * @param ntk Network the nodes belong to.
 * @param upper Nodes of the upper level in order; a node can appear several times.
 * @param lower Nodes of the lower level; reordered.
 * @param max_swaps Maximum number of adjacent swaps; `0` keeps the barycenter order.
 */
template <typename Ntk>
void minimize_crossings(const Ntk& ntk, const std::vector<mockturtle::node<Ntk>>& upper,
                        std::vector<mockturtle::node<Ntk>>& lower, const uint32_t max_swaps)
{
    if (lower.size() < 2)
    {
        return;
    }

    const auto pos = level_positions(ntk, lower);

    // positions in the upper level that use each node of the lower level, one entry per edge
    std::vector<std::vector<std::size_t>> users(lower.size());

    for (std::size_t t = 0; t < upper.size(); ++t)
    {
        for (const auto p : fanin_positions(ntk, upper[t], pos))
        {
            users[p].push_back(t);
        }
    }

    std::vector<std::size_t> order(lower.size());
    std::iota(order.begin(), order.end(), 0u);

    const auto key = [&users](const std::size_t i)
    {
        if (users[i].empty())
        {
            return static_cast<double>(i);
        }

        return static_cast<double>(std::accumulate(users[i].cbegin(), users[i].cend(), std::size_t{0})) /
               static_cast<double>(users[i].size());
    };

    std::stable_sort(order.begin(), order.end(),
                     [&key](const std::size_t a, const std::size_t b) { return key(a) < key(b); });

    // crossings between the edges of two nodes of the lower level, the first standing left of the second: an edge of
    // the left node crosses every edge of the right node that ends further left in the upper level
    const auto crossings_between = [&users](const std::size_t left, const std::size_t right)
    {
        uint64_t count = 0;

        for (const auto a : users[left])
        {
            for (const auto b : users[right])
            {
                if (a > b)
                {
                    ++count;
                }
            }
        }

        return count;
    };

    uint32_t swaps = 0;

    for (std::size_t i = 0; i + 1 < order.size() && swaps < max_swaps; ++i)
    {
        if (crossings_between(order[i + 1], order[i]) < crossings_between(order[i], order[i + 1]))
        {
            std::swap(order[i], order[i + 1]);
            ++swaps;
        }
    }

    std::vector<mockturtle::node<Ntk>> sorted{};
    sorted.reserve(lower.size());
    for (const auto i : order)
    {
        sorted.push_back(lower[i]);
    }
    lower = std::move(sorted);
}

/**
 * @brief Costs keeping the crossings between an upper level and a lower level.
 *
 * The lower level is reordered to minimize crossings, then every crossing is charged one crossing gate and every edge
 * the buffers that pad it to the number of crossing gates on the most crossed edge. Crossings are found by sweeping the
 * edges in the order of the upper level. The sweep takes time in
 * \f$O(E \cdot W + C)\f$ for \f$E\f$ edges, a lower level of width \f$W\f$, and \f$C\f$ counted crossings.
 *
 * @tparam Ntk Network type.
 * @param ntk Network the nodes belong to.
 * @param upper Nodes of the upper level in order; a node can appear several times.
 * @param lower Nodes of the lower level; reordered.
 * @param max_swaps Maximum number of adjacent swaps of the crossing minimization, see `minimize_crossings`.
 * @param crossing_gates Parameters of the crossing gates: their kind sets the cost, and levels with more crossings than
 * `max_crossings_per_level` are not costed.
 * @return Cost and crossing count; the cost is infinite above `max_crossings_per_level`, where counting stops.
 */
template <typename Ntk>
[[nodiscard]] crossing_cost_result crossing_cost(const Ntk& ntk, const std::vector<mockturtle::node<Ntk>>& upper,
                                                 std::vector<mockturtle::node<Ntk>>& lower, const uint32_t max_swaps,
                                                 const crossing_gate_planarization_params& crossing_gates)
{
    minimize_crossings(ntk, upper, lower, max_swaps);

    const auto pos = level_positions(ntk, lower);

    // crossings per edge, edges keyed by (position in the upper level, position of the fanin)
    std::unordered_map<uint64_t, uint64_t> per_edge{};
    const auto                             edge_key = [](const std::size_t t, const std::size_t p)
    { return (static_cast<uint64_t>(t) << 32u) | static_cast<uint64_t>(p); };

    /** @brief Edge keys swept at one position of the lower level. */
    using swept_edges = std::vector<uint64_t>;

    std::vector<swept_edges> swept(lower.size() + 1);
    std::size_t              max_pos       = 0;
    uint64_t                 num_crossings = 0;
    uint64_t                 max_per_edge  = 0;

    for (std::size_t t = 0; t < upper.size(); ++t)
    {
        const auto targets = fanin_positions(ntk, upper[t], pos);

        // every edge swept so far that ends right of this edge's target crosses it
        for (const auto p : targets)
        {
            const auto key = edge_key(t, p);
            per_edge.try_emplace(key, 0);

            for (auto k = max_pos; k > p; --k)
            {
                for (const auto prev_key : swept[k])
                {
                    ++num_crossings;

                    // a level this crossed is always duplicated; no need to cost it
                    if (num_crossings > crossing_gates.max_crossings_per_level)
                    {
                        return {.cost = std::numeric_limits<uint64_t>::max(), .num_crossings = num_crossings};
                    }

                    ++per_edge[prev_key];
                    ++per_edge[key];
                    max_per_edge = std::max({max_per_edge, per_edge[prev_key], per_edge[key]});
                }
            }
        }

        for (const auto p : targets)
        {
            max_pos = std::max(max_pos, p);
            swept[p].push_back(edge_key(t, p));
        }
    }

    uint64_t cost = num_crossings * crossing_gate_nodes(crossing_gates.xor_gates);

    if (num_crossings > 0)
    {
        for (const auto& [key, count] : per_edge)
        {
            cost += (max_per_edge - count) * crossing_gate_depth(crossing_gates.xor_gates);
        }
    }

    return {.cost = cost, .num_crossings = num_crossings};
}

}  // namespace fiction::synthesis::detail
