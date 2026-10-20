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
 * @brief Planarizes a ranked logic network by replacing every edge crossing with a crossing gate.
 * @author Benjamin Hien (hibenj)
 */

#pragma once

#include "fiction/networks/name_utils.hpp"
#include "fiction/networks/network_utils.hpp"
#include "fiction/synthesis/network_balancing.hpp"
#include "fiction/utils/graph/mincross.hpp"
#include "fiction/utils/progress.hpp"
#include "fiction/utils/stl/hash.hpp"

#include <fmt/format.h>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/views/fanout_view.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fiction::synthesis
{

/**
 * @brief Parameters for the crossing gate planarization algorithm.
 */
struct crossing_gate_planarization_params
{
    /**
     * @brief Receives completed work and the phase total.
     */
    utils::progress_callback on_progress{};
    /**
     * @brief Build crossing gates from XOR gates (three XORs, four levels) instead of AND, OR, and NOT gates (fourteen
     * levels).
     */
    bool xor_gates = true;
    /**
     * @brief Adjacent levels with more crossings between them than this are rejected, since the number of crossing
     * gates grows with the crossings.
     *
     * The limit bounds the crossing gates between two levels, not the buffers that every pass adds along the other
     * edges between them.
     */
    uint32_t max_crossings_per_level = 1000u;
};

/**
 * @brief Statistics of the crossing gate planarization algorithm.
 */
struct crossing_gate_planarization_stats
{
    /**
     * @brief Runtime of the planarization core.
     *
     * Excludes the final planarity check.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * @brief Number of crossings replaced by crossing gates.
     */
    uint64_t num_crossings{0};
    /**
     * @brief Writes the statistics to a stream.
     *
     * @param out Stream to write to.
     */
    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("[i] total time     = {:.2f} secs\n", mockturtle::to_seconds(time_total));
        out << fmt::format("[i] num. crossings = {}\n", num_crossings);
    }
};

namespace detail
{

/**
 * @brief Number of nodes of one XOR crossing gate, buffers included.
 */
inline constexpr uint64_t XOR_CROSSING_GATE_NODES = 10u;
/**
 * @brief Number of levels one XOR crossing gate spans.
 */
inline constexpr uint64_t XOR_CROSSING_GATE_DEPTH = 4u;
/**
 * @brief Number of nodes of one crossing gate built from AND, OR, and NOT gates, buffers included.
 */
inline constexpr uint64_t AND_OR_CROSSING_GATE_NODES = 59u;
/**
 * @brief Number of levels one crossing gate built from AND, OR, and NOT gates spans.
 */
inline constexpr uint64_t AND_OR_CROSSING_GATE_DEPTH = 14u;

/**
 * @brief Implementation of the crossing gate planarization algorithm.
 *
 * @tparam Ntk Ranked, balanced network type.
 */
template <typename Ntk>
class crossing_gate_planarization_impl
{
  public:
    /**
     * @brief Creates the implementation.
     *
     * @param src Source network.
     * @param p Parameters.
     * @param st Statistics.
     */
    crossing_gate_planarization_impl(const Ntk& src, const crossing_gate_planarization_params& p,
                                     crossing_gate_planarization_stats& st) :
            ntk{src},
            fanout_ntk{src},
            ps{p},
            pst{st}
    {}

    /**
     * @brief Runs the algorithm.
     *
     * @return Planar copy of the source network with crossing gates.
     * @throws std::runtime_error If two adjacent levels have more crossings than `max_crossings_per_level` allows.
     */
    [[nodiscard]] Ntk run()
    {
        const mockturtle::stopwatch stop{pst.time_total};

        utils::progress_reporter progress{ps.on_progress, "replacing crossings", ntk.depth()};

        detect_crossings();

        auto init    = networks::initialize_copy_network_with_virtual_pis(ntk);
        auto dest    = std::move(init.first);
        auto old2new = std::move(init.second);

        // level 0 of the result keeps the order of the primary inputs
        std::vector<mockturtle::node<Ntk>> input_order{};
        input_order.reserve(ntk.rank_width(0));
        for (const auto& pi : ntk.get_ranks(0))
        {
            input_order.push_back(dest.get_node(old2new[pi]));
        }

        for (uint32_t r = 1; r <= ntk.depth(); ++r)
        {
            resolve_crossings(dest, old2new, stages[r - 1]);

            ntk.foreach_node_in_rank(
                r,
                [this, &dest, &old2new](const auto& n)
                {
                    if (ntk.is_constant(n) || ntk.is_ci(n))
                    {
                        return;
                    }

                    std::vector<mockturtle::signal<Ntk>> children{};
                    children.reserve(ntk.fanin_size(n));

                    ntk.foreach_fanin(n,
                                      [this, &dest, &old2new, &children, &n](const auto& f)
                                      {
                                          const auto fn  = ntk.get_node(f);
                                          auto       sig = old2new[fn];

                                          if (const auto it = edge_end.find({fn, n}); it != edge_end.end())
                                          {
                                              sig = it->second;
                                          }

                                          children.push_back(ntk.is_complemented(f) ? dest.create_not(sig) : sig);
                                      });

                    old2new[n] = dest.create_node(children, ntk.node_function(n));
                });

            progress.advance();
        }

        ntk.foreach_po(
            [this, &dest, &old2new](const auto& po)
            {
                const auto sig = old2new[ntk.get_node(po)];
                dest.create_po(ntk.is_complemented(po) ? dest.create_not(sig) : sig);
            });

        networks::restore_names(ntk, dest, old2new);

        dest.update_ranks();
        dest.set_ranks(0, input_order);

        return dest;
    }

  private:
    /**
     * @brief An edge between two adjacent levels of the source network.
     */
    struct edge
    {
        /**
         * @brief Source node.
         */
        mockturtle::node<Ntk> source;
        /**
         * @brief Target node.
         */
        mockturtle::node<Ntk> target;
        /**
         * @brief Equality of two edges.
         */
        bool operator==(const edge&) const = default;
    };
    /**
     * @brief Hash of an edge.
     */
    struct edge_hash
    {
        /**
         * @brief Combines source and target.
         *
         * @param e Edge.
         * @return Hash value.
         */
        std::size_t operator()(const edge& e) const noexcept
        {
            std::size_t h = 0;
            utils::stl::hash_combine(h, e.source, e.target);
            return h;
        }
    };
    /**
     * @brief A crossing of two edges.
     *
     * `level` orders the crossings on the same edge: a crossing of level \f$l\f$ can only be resolved after all
     * crossings of lower levels on its edges.
     */
    struct crossing
    {
        /**
         * @brief The left edge of the crossing.
         */
        edge left;
        /**
         * @brief The right edge of the crossing.
         */
        edge right;
        /**
         * @brief Resolution level.
         */
        uint64_t level;
    };
    /**
     * @brief Crossings between two adjacent levels and the edges between them in rank order of their sources.
     */
    struct stage
    {
        /**
         * @brief Crossings of the stage.
         */
        std::vector<crossing> crossings{};
        /**
         * @brief All edges of the stage, sources in rank order, targets in rank order per source.
         */
        std::vector<edge> edges{};
    };
    /**
     * @brief Levels an input of the XOR crossing gate is delayed by so that it meets the buffered XOR of both inputs:
     * the XOR and its buffer.
     */
    static constexpr uint32_t XOR_CORE_DELAY = 2u;
    /**
     * @brief Levels an input of the AND-OR-NOT XOR is delayed by so that it meets the buffered NAND of both inputs: the
     * NAND, its inverter, and its buffer.
     */
    static constexpr uint32_t AND_OR_INNER_DELAY = 3u;
    /**
     * @brief Levels an input of the AND-OR-NOT crossing gate is delayed by so that it meets the buffered AND-OR-NOT XOR
     * of both inputs: the XOR of depth six and its buffer.
     */
    static constexpr uint32_t AND_OR_CORE_DELAY = 7u;
    /**
     * @brief Depth of one crossing gate, including the buffers that align the edges it is placed on.
     *
     * @return Number of levels a crossing gate occupies.
     */
    [[nodiscard]] uint32_t crossing_gate_depth() const noexcept
    {
        return static_cast<uint32_t>(ps.xor_gates ? XOR_CROSSING_GATE_DEPTH : AND_OR_CROSSING_GATE_DEPTH);
    }
    /**
     * @brief Finds the crossings between every pair of adjacent levels.
     *
     * Edges are swept in rank order of their sources; an edge that ends left of the end of an earlier edge crosses it.
     * The level of a crossing is one more than the highest level already assigned to either edge, so that crossings on
     * one edge are resolved in order.
     *
     * @throws std::runtime_error If two adjacent levels have more crossings than `max_crossings_per_level`.
     */
    void detect_crossings()
    {
        stages.clear();
        stages.reserve(ntk.depth());

        for (uint32_t r = 0; r < ntk.depth(); ++r)
        {
            stage level_stage{};

            // edges already swept, grouped by target position, with the level each one has reached
            std::vector<std::deque<std::pair<edge, uint64_t>>> swept(ntk.rank_width(r + 1) + 1);

            uint64_t max_pos         = 0;
            uint64_t level_crossings = 0;

            ntk.foreach_node_in_rank(
                r,
                [this, &level_stage, &swept, &max_pos, &level_crossings, r](const auto& n)
                {
                    std::vector<edge> targets{};
                    targets.reserve(fanout_ntk.fanout_size(n));
                    fanout_ntk.foreach_fanout(n, [&targets, &n](const auto& fo) { targets.push_back({n, fo}); });
                    std::sort(targets.begin(), targets.end(), [this](const edge& a, const edge& b)
                              { return ntk.rank_position(a.target) < ntk.rank_position(b.target); });

                    // every edge swept so far that ends right of this edge's target crosses it; the crossing's
                    // level is one more than the levels both edges have reached, so crossing gates stack bottom-up
                    for (const auto& e : targets)
                    {
                        const uint64_t pos       = ntk.rank_position(e.target);
                        uint64_t       local_lvl = 0;

                        for (auto k = max_pos; k > pos; --k)
                        {
                            for (auto& [prev_edge, prev_lvl] : swept[k])
                            {
                                const auto level = std::max(local_lvl, prev_lvl);

                                level_stage.crossings.push_back({prev_edge, e, level});
                                ++level_crossings;

                                // checked per crossing, so that a pair of levels far above the limit does not fill the
                                // memory the limit is meant to protect
                                if (level_crossings > ps.max_crossings_per_level)
                                {
                                    throw std::runtime_error(
                                        fmt::format("Levels {} and {} have more than the {} crossings allowed", r,
                                                    r + 1, ps.max_crossings_per_level));
                                }

                                local_lvl = std::max(local_lvl, prev_lvl) + 1;
                                ++prev_lvl;
                            }
                        }

                        level_stage.edges.push_back(e);
                    }

                    for (const auto& e : targets)
                    {
                        const uint64_t pos = ntk.rank_position(e.target);
                        max_pos            = std::max(max_pos, pos);
                        swept[pos].push_front({e, 0});
                    }
                });

            pst.num_crossings += level_crossings;

            stages.push_back(std::move(level_stage));
        }
    }
    /**
     * @brief Resolves the crossings of one stage in order of their levels.
     *
     * Each pass over the edges places one crossing gate per crossing whose left edge is next in line and swaps the two
     * edges; every other edge is extended by a buffer chain of the crossing gate depth, so that all edges of the stage
     * grow by the same number of levels per pass.
     *
     * @param dest Destination network.
     * @param old2new Map from source nodes to destination signals.
     * @param st Stage whose crossings are resolved; its edge order is rewritten.
     * @throws std::runtime_error If a pass cannot place any crossing.
     */
    void resolve_crossings(Ntk& dest, const mockturtle::node_map<mockturtle::signal<Ntk>, Ntk>& old2new, stage& st)
    {
        auto ordered = st.crossings;
        std::stable_sort(ordered.begin(), ordered.end(),
                         [](const crossing& a, const crossing& b) { return a.level < b.level; });

        std::size_t placed = 0;

        // one pass per crossing gate depth: adjacent crossing edges get a crossing gate and swap places, every other
        // edge a buffer chain of the same depth, until every crossing between the two levels is resolved
        while (placed < ordered.size())
        {
            const auto placed_before = placed;

            std::size_t i = 0;
            while (i < st.edges.size())
            {
                if (placed < ordered.size() && st.edges[i] == ordered[placed].left)
                {
                    if (i + 1 >= st.edges.size() || st.edges[i + 1] != ordered[placed].right)
                    {
                        throw std::runtime_error("A crossing's edges are not adjacent; the rank order is inconsistent");
                    }

                    place_crossing_gate(dest, old2new, ordered[placed]);
                    ++placed;

                    // the crossing gate consumes both edges of the crossing
                    std::swap(st.edges[i], st.edges[i + 1]);
                    i += 2;
                }
                else
                {
                    extend_edge(dest, old2new, st.edges[i]);
                    ++i;
                }
            }

            if (placed == placed_before)
            {
                throw std::runtime_error("No crossing could be placed in a pass; the rank order is inconsistent");
            }
        }
    }
    /**
     * @brief Signal at the current end of an edge: the end of its last crossing gate or buffer chain, or its source.
     *
     * @param old2new Map from source nodes to destination signals.
     * @param e Edge.
     * @return Signal in the destination network.
     */
    [[nodiscard]] mockturtle::signal<Ntk> edge_signal(const mockturtle::node_map<mockturtle::signal<Ntk>, Ntk>& old2new,
                                                      const edge&                                               e) const
    {
        if (const auto it = edge_end.find(e); it != edge_end.end())
        {
            return it->second;
        }

        return old2new[e.source];
    }
    /**
     * @brief Extends an edge by a buffer chain of the crossing gate depth.
     *
     * @param dest Destination network.
     * @param old2new Map from source nodes to destination signals.
     * @param e Edge.
     */
    void extend_edge(Ntk& dest, const mockturtle::node_map<mockturtle::signal<Ntk>, Ntk>& old2new, const edge& e)
    {
        edge_end[e] = networks::buffer_chain(dest, edge_signal(old2new, e), crossing_gate_depth());
    }
    /**
     * @brief An XOR built from AND, OR, and NOT gates, balanced to six levels: \f$(a \land \lnot(a \land b)) \lor (b
     * \land \lnot(a \land b))\f$ with the direct paths of `a` and `b` buffered to the depth of the shared term.
     *
     * Nodes are created from left to right per level, which keeps the XOR planar in creation order.
     *
     * @param dest Destination network.
     * @param a Left input.
     * @param b Right input.
     * @return Output signal of the XOR.
     */
    [[nodiscard]] static mockturtle::signal<Ntk> and_or_xor(Ntk& dest, const mockturtle::signal<Ntk> a,
                                                            const mockturtle::signal<Ntk> b)
    {
        const auto fo_a = dest.create_buf(a);
        const auto fo_b = dest.create_buf(b);

        const auto a_delayed = networks::buffer_chain(dest, fo_a, AND_OR_INNER_DELAY);
        const auto core      = dest.create_buf(dest.create_not(dest.create_and(fo_a, fo_b)));
        const auto p_a       = dest.create_and(a_delayed, core);
        const auto b_delayed = networks::buffer_chain(dest, fo_b, AND_OR_INNER_DELAY);
        const auto p_b       = dest.create_and(b_delayed, core);

        return dest.create_or(p_a, p_b);
    }
    /**
     * @brief Places the crossing gate that swaps the signals of two crossing edges: with \f$c_0 = a \oplus b\f$, the
     * left output \f$a \oplus c_0 = b\f$ continues the right edge and the right output \f$c_0 \oplus b = a\f$ continues
     * the left edge, so the left edge ends up right of the right edge.
     *
     * The direct paths are buffered to the depth of the crossing gate, and nodes are created from left to right per
     * level, which keeps the crossing gate planar in creation order.
     *
     * @param dest Destination network.
     * @param old2new Map from source nodes to destination signals.
     * @param c Crossing to resolve.
     */
    void place_crossing_gate(Ntk& dest, const mockturtle::node_map<mockturtle::signal<Ntk>, Ntk>& old2new,
                             const crossing& c)
    {
        const auto a = dest.create_buf(edge_signal(old2new, c.left));
        const auto b = dest.create_buf(edge_signal(old2new, c.right));

        if (ps.xor_gates)
        {
            const auto a_delayed = networks::buffer_chain(dest, a, XOR_CORE_DELAY);
            const auto c0        = dest.create_buf(dest.create_xor(a, b));
            const auto b_delayed = networks::buffer_chain(dest, b, XOR_CORE_DELAY);

            const auto left_out  = dest.create_xor(a_delayed, c0);  // = b
            const auto right_out = dest.create_xor(c0, b_delayed);  // = a

            edge_end[c.right] = left_out;
            edge_end[c.left]  = right_out;
        }
        else
        {
            const auto a_delayed = networks::buffer_chain(dest, a, AND_OR_CORE_DELAY);
            const auto c0        = dest.create_buf(and_or_xor(dest, a, b));
            const auto b_delayed = networks::buffer_chain(dest, b, AND_OR_CORE_DELAY);

            const auto left_out  = and_or_xor(dest, a_delayed, c0);  // = b
            const auto right_out = and_or_xor(dest, c0, b_delayed);  // = a

            edge_end[c.right] = left_out;
            edge_end[c.left]  = right_out;
        }
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
    const crossing_gate_planarization_params& ps;
    /**
     * @brief Statistics.
     */
    crossing_gate_planarization_stats& pst;
    /**
     * @brief Crossings and edges per pair of adjacent levels.
     */
    std::vector<stage> stages{};
    /**
     * @brief Current end of every edge in the destination network, once a crossing gate or buffer chain was placed on
     * it.
     */
    std::unordered_map<edge, mockturtle::signal<Ntk>, edge_hash> edge_end{};
};

}  // namespace detail

/**
 * @brief Planarizes a ranked, balanced logic network by replacing every crossing of two edges with a crossing gate that
 * swaps the two signals.
 *
 * Crossings between two levels are resolved pass by pass; in every pass, each remaining crossing whose edges are
 * adjacent receives a crossing gate and every other edge a buffer chain of the same depth, so the result stays
 * balanced. The rank order of the input is kept.
 *
 * Crossing gates consist of three XORs when `xor_gates` is set, otherwise of AND, OR, and NOT gates, which makes them
 * deeper. The result is verified crossing-free before the function returns.
 *
 * @tparam Ntk Ranked, balanced network type (see `mutable_rank_view`).
 * @param ntk Source network.
 * @param ps Parameters.
 * @param pst Statistics.
 * @return Planar network of the same type that computes the same functions as `ntk`.
 * @throws std::invalid_argument If `ntk` is not balanced.
 * @throws std::runtime_error If two adjacent levels exceed `max_crossings_per_level` or the result still contains
 * crossings.
 */
template <typename Ntk>
[[nodiscard]] Ntk crossing_gate_planarization(const Ntk& ntk, const crossing_gate_planarization_params& ps = {},
                                              crossing_gate_planarization_stats* pst = nullptr)
{
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
    static_assert(mockturtle::has_create_node_v<Ntk>, "Ntk does not implement the create_node method");
    static_assert(mockturtle::has_create_buf_v<Ntk>, "Ntk does not implement the create_buf method");
    static_assert(mockturtle::has_create_xor_v<Ntk>, "Ntk does not implement the create_xor method");
    static_assert(mockturtle::has_rank_position_v<Ntk>, "Ntk does not implement the rank_position method");
    static_assert(mockturtle::has_depth_v<Ntk>, "Ntk does not implement the depth method");

    if (!is_balanced(ntk))
    {
        throw std::invalid_argument("The network must be balanced before planarization");
    }

    crossing_gate_planarization_stats st{};

    detail::crossing_gate_planarization_impl<Ntk> p{ntk, ps, st};

    auto result = p.run();

    if (const auto crossings = utils::graph::count_crossings(result); crossings != 0)
    {
        throw std::runtime_error(fmt::format("Planarization failed: the result contains {} crossings", crossings));
    }

    if (pst != nullptr)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction::synthesis
