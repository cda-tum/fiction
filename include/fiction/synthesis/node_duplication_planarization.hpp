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
#include "fiction/synthesis/crossing_cost.hpp"
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
 * @brief Parameters for the node duplication planarization algorithm.
 */
struct node_duplication_planarization_params
{
    /**
     * @brief Order in which the primary outputs are placed in the first level before the algorithm starts.
     */
    enum class output_order : uint8_t
    {
        /**
         * @brief Keep the primary output order of the input network.
         */
        KEEP_PO_ORDER,
        /**
         * @brief Shuffle the primary outputs randomly.
         *
         * Different orders can yield different numbers of duplications.
         */
        RANDOM_PO_ORDER
    };
    /**
     * @brief How a level is made crossing-free.
     */
    enum class planarization_strategy : uint8_t
    {
        /**
         * @brief Duplicate nodes on every level.
         *
         * The result is planar.
         */
        DUPLICATION,
        /**
         * @brief Decide per level whether duplicating nodes or keeping the crossings for `crossing_gate_planarization`
         * is cheaper.
         *
         * The result contains crossings on the levels where crossing gates are cheaper.
         */
        HYBRID
    };
    /**
     * @brief How the hybrid strategy estimates the cost of duplicating a level.
     */
    enum class decision_criterion : uint8_t
    {
        /**
         * @brief The weighted size of the duplicated cones, see `duplication_cost_model`.
         */
        WEIGHTED_CONE,
        /**
         * @brief The number of nodes that duplicating actually creates.
         *
         * The copies on the level plus those that running the duplication strategy on the levels below creates.
         *
         * Both options are run, and the second run stops once it exceeds the first. Levels whose runs exceed
         * `lookahead_budget` fall back to `WEIGHTED_CONE`.
         */
        LOOKAHEAD
    };
    /**
     * @brief Weights of the duplication cost model of the hybrid strategy's `WEIGHTED_CONE` criterion.
     *
     * The cost of duplicating a node is the weighted size of its transitive fanin, since every duplicate drags its
     * whole cone along. A gate weighs `node_weight` and a chain buffer or inverter weighs `buffer_weight`. The sum is
     * scaled by \f$\text{depth\_growth}^{d}\f$ for a duplication on level \f$d\f$, because duplicates on deep levels
     * are duplicated again by the decisions below. The weights are in units of one crossing gate node. The defaults
     * were determined empirically on the benchmark sets. Weights must be finite and non-negative, the growth finite and
     * positive.
     */
    struct duplication_cost_model
    {
        /**
         * @brief Weight of a gate.
         */
        double node_weight = 2.0;
        /**
         * @brief Weight of a buffer or inverter chain node.
         */
        double buffer_weight = 0.5;
        /**
         * @brief Growth of the duplication cost per level on which the duplication happens.
         */
        double depth_growth = 1.02;
    };
    /**
     * @brief Planarization strategy.
     *
     * This is the first member, so that `planarization_params` can select the hybrid strategy with a plain default
     * initializer.
     */
    planarization_strategy strategy = planarization_strategy::DUPLICATION;
    /**
     * @brief Receives completed work and the phase total.
     */
    utils::progress_callback on_progress{};
    /**
     * @brief Primary output order used to seed the first level.
     */
    output_order po_order = output_order::KEEP_PO_ORDER;
    /**
     * @brief Seed for the random primary output order.
     *
     * A random seed is drawn when none is given.
     */
    std::optional<uint32_t> seed = std::nullopt;
    /**
     * @brief Maximum number of adjacent swaps per level that the hybrid strategy tries after the barycenter ordering to
     * reduce the crossings it costs.
     *
     * `0` keeps the barycenter order.
     */
    uint32_t max_swaps = 32u;
    /**
     * @brief Parameters of the `crossing_gate_planarization` that resolves the crossings the hybrid strategy keeps.
     *
     * The hybrid strategy costs the crossing gates by `xor_gates`, and it always duplicates a level whose crossings
     * exceed `max_crossings_per_level`. `on_progress` has no effect here; `planarization` forwards its own callback.
     */
    crossing_gate_planarization_params crossing_gates{};
    /**
     * @brief Decision criterion of the hybrid strategy.
     */
    decision_criterion criterion = decision_criterion::LOOKAHEAD;
    /**
     * @brief Duplication cost model of the hybrid strategy.
     */
    duplication_cost_model duplication_cost{};
    /**
     * @brief Nodes a lookahead may create before it is cut off; levels whose both options exceed it fall back to the
     * weighted cone model.
     *
     * Must be positive.
     */
    uint64_t lookahead_budget = 1000000u;
    /**
     * @brief Abort with `std::runtime_error` as soon as more nodes than this are duplicated at once.
     *
     * The duplicates of the result plus those of the level under construction, which the hybrid strategy may still
     * discard.
     *
     * Node duplication can grow exponentially with the depth of the network, so the limit is checked at every
     * duplicate. `0` disables the limit, which may exhaust the memory on an adverse network.
     */
    uint64_t max_duplications = 2000000u;
};

/**
 * @brief Statistics of the node duplication planarization algorithm.
 */
struct node_duplication_planarization_stats
{
    /**
     * @brief Runtime of the duplication core.
     *
     * Excludes the final planarity check.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * @brief Number of nodes in the planarized network minus the number of nodes in the input network.
     */
    uint64_t num_duplications{0};
    /**
     * @brief Number of levels on which the hybrid strategy kept the crossings.
     */
    uint64_t num_crossing_levels{0};
    /**
     * @brief Number of crossings the hybrid strategy left for `crossing_gate_planarization`.
     */
    uint64_t num_crossings{0};
    /**
     * @brief Writes the statistics to a stream.
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
 * @brief Nodes grouped by level, outermost vector indexed by level.
 *
 * @tparam Ntk Network type.
 */
template <typename Ntk>
using levelized_node_order = std::vector<std::vector<mockturtle::node<Ntk>>>;

/**
 * @brief One node of the H-graph used to compute the duplication order of a level.
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
     * @brief Node of the upper level whose fanins are ordered.
     *
     * A copy id when that node is a duplicate.
     */
    mockturtle::node<Ntk> root;
    /**
     * @brief Leftmost fanin of the ordering; the one a predecessor ordering can share.
     */
    mockturtle::node<Ntk> first_fanin;
    /**
     * @brief Rightmost fanin of the ordering; the one a successor ordering can share.
     */
    mockturtle::node<Ntk> last_fanin;
    /**
     * @brief All remaining fanins.
     */
    std::vector<mockturtle::node<Ntk>> middle_fanins{};
    /**
     * @brief Delay of the shortest path through the H-graph that ends in this node.
     */
    uint64_t delay;
    /**
     * @brief Index of the predecessor H-graph node on that shortest path, within the slice of the previous node.
     */
    std::size_t predecessor{};
    /**
     * @brief Sum of the cone weights of the fanins that the shortest path ending here places once for two consecutive
     * consumers.
     *
     * The tie-break among equally short paths prefers the heaviest sum.
     */
    double shared_weight{0.0};
    /**
     * @brief Whether the shortest path places two different fanins that share a fanin in the level below side by side.
     *
     * These are `first_fanin` and the last fanin of the predecessor. The last tie-break prefers such paths, since the
     * two side by side avoid a duplication below.
     */
    bool shares_below{false};
    /**
     * @brief Creates an H-graph node.
     *
     * @param r Node whose fanins are ordered.
     * @param first Leftmost fanin of the ordering.
     * @param last Rightmost fanin of the ordering.
     * @param delay_value Initial delay.
     */
    hgraph_node(const mockturtle::node<Ntk> r, const mockturtle::node<Ntk> first, const mockturtle::node<Ntk> last,
                const uint64_t delay_value) :
            root{r},
            first_fanin{first},
            last_fanin{last},
            delay{delay_value}
    {}
};

/**
 * @brief Enumerates the H-graph nodes of one slice, i.e., all orderings of the given fanins by their first and last
 * element.
 *
 * A single fanin yields one H-graph node. Delays start at infinity.
 *
 * @tparam Ntk Network type.
 * @param root Node whose fanins are ordered.
 * @param nodes Fanins in rank order.
 * @param orderings Receives the H-graph nodes of the slice; replaces its content and reuses its storage.
 */
template <typename Ntk>
void enumerate_orderings(const mockturtle::node<Ntk> root, const std::vector<mockturtle::node<Ntk>>& nodes,
                         std::vector<hgraph_node<Ntk>>& orderings)
{
    constexpr auto inf = std::numeric_limits<uint64_t>::max();

    orderings.clear();

    if (nodes.empty())
    {
        return;
    }

    if (nodes.size() == 1)
    {
        orderings.emplace_back(root, nodes.front(), nodes.front(), inf);
        return;
    }

    orderings.reserve(nodes.size() * (nodes.size() - 1));

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

            orderings.push_back(std::move(forward));
            orderings.push_back(std::move(backward));
        }
    }
}

/**
 * @brief Implementation of the node duplication planarization algorithm.
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
     * @brief Nodes of one level or the fanins of one node, source ids or copy ids.
     */
    using node_list = std::vector<mockturtle::node<Ntk>>;
    /**
     * @brief One slice of the H-graph: every ordering of the fanins of one node.
     */
    using hgraph_slice = std::vector<hgraph_node<Ntk>>;
    /**
     * @brief Cone weights of the source nodes and the scratch space that weighs a cone.
     */
    struct cone_weight_map
    {
        /**
         * @brief Unscaled cone weight per source node, negative for a node not weighed yet.
         */
        std::vector<double> weight;
        /**
         * @brief Stamp of the cone in which each source node was last visited.
         */
        std::vector<uint32_t> visited;
        /**
         * @brief Stamp of the cone being weighed, never 0.
         */
        uint32_t stamp{0};
    };
    /**
     * @brief Creates the implementation.
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
            fanins_of(src.size()),
            own_cone_weights{std::vector<double>(src.size(), UNWEIGHED), std::vector<uint32_t>(src.size(), 0u)},
            cone_weights{&own_cone_weights}
    {}

    /**
     * @brief Collects the primary outputs that form the first level.
     *
     * The level holds every non-constant output node once, in rank order, or shuffled when the parameters ask for a
     * random output order.
     *
     * @return Nodes of the output level.
     */
    [[nodiscard]] std::vector<mockturtle::node<Ntk>> output_level() const
    {
        std::vector<mockturtle::node<Ntk>> pos{};
        pos.reserve(ntk.num_pos());
        ntk.foreach_po(
            [this, &pos](const auto& po)
            {
                if (const auto n = ntk.get_node(po); !ntk.is_constant(n))
                {
                    pos.push_back(n);
                }
            });

        // the balanced network holds all non-constant outputs on one level, where the rank order is a total order
        std::ranges::sort(pos,
                          [this](const auto& a, const auto& b) { return ntk.rank_position(a) < ntk.rank_position(b); });
        pos.erase(std::unique(pos.begin(), pos.end()), pos.end());

        if (ps.po_order == node_duplication_planarization_params::output_order::RANDOM_PO_ORDER)
        {
            std::mt19937_64 generator{ps.seed.has_value() ? *ps.seed : std::random_device{}()};
            std::shuffle(pos.begin(), pos.end(), generator);
        }

        return pos;
    }
    /**
     * @brief Decides for the hybrid strategy whether the level just computed keeps its crossings instead of its
     * duplicates.
     *
     * If so, the level uses the source nodes reordered to reduce crossings, its duplicates are discarded, and the
     * statistics count the kept crossings.
     *
     * @param next_level Level below the current one as the duplication computed it; replaced if crossings are kept.
     * @param copies_before Number of duplicates before the computation of `next_level`.
     * @return `true` iff the level keeps its crossings.
     */
    [[nodiscard]] bool keep_crossings(std::vector<mockturtle::node<Ntk>>& next_level, const std::size_t copies_before)
    {
        const uint64_t copies_on_level = copy_origin.size() - copies_before;

        if (ps.strategy != node_duplication_planarization_params::planarization_strategy::HYBRID ||
            copies_on_level == 0)
        {
            return false;
        }

        // level of `next_level` in the source network
        const auto lvl = ntk.depth() - static_cast<uint32_t>(levels.size());

        auto original_rank = ntk.get_ranks(lvl);

        std::vector<mockturtle::node<Ntk>> upper{};
        upper.reserve(levels.back().size());
        std::ranges::transform(levels.back(), std::back_inserter(upper), [this](const auto& n) { return origin(n); });

        const auto cross_cost = crossing_cost(ntk, upper, original_rank, ps.max_swaps, ps.crossing_gates);

        if (!crossings_are_cheaper(cross_cost.cost, next_level, original_rank, lvl, copies_on_level))
        {
            return false;
        }

        next_level = std::move(original_rank);
        ++pst.num_crossing_levels;
        pst.num_crossings += cross_cost.num_crossings;

        discard_copies(copies_before);

        return true;
    }
    /**
     * @brief Runs the algorithm.
     *
     * @return Planar network with duplicated nodes and virtual primary inputs.
     */
    [[nodiscard]] networks::virtual_pi_network<Ntk> run()
    {
        const mockturtle::stopwatch stop{pst.time_total};

        // one step per gate level of the input network plus one for building the result
        utils::progress_reporter progress{ps.on_progress, "planarizing levels", ntk.depth() + 1};

        // first level: the primary outputs
        const auto pos = output_level();

        levels.push_back(pos);
        progress.advance();

        // outputs that inputs drive directly form the only level; there is nothing below them to order
        if (!is_final_level(pos))
        {
            std::size_t copies_before = copy_origin.size();

            auto next_level = next_level_of(pos);
            bool at_inputs  = is_final_level(next_level);

            while (!next_level.empty() && !at_inputs)
            {
                // the hybrid strategy considers keeping the crossings of a level instead of its duplicates
                static_cast<void>(keep_crossings(next_level, copies_before));

                levels.push_back(next_level);

                copies_before = copy_origin.size();
                next_level    = next_level_of(next_level);
                at_inputs     = is_final_level(next_level);
                progress.advance();
            }

            // the final level holds the primary inputs
            if (at_inputs)
            {
                levels.push_back(next_level);
            }
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
     * @brief Creates an instance that only counts the duplicates the duplication strategy creates.
     *
     * It shares the cone weights of `parent`, so that every cone is weighed once per run, and records neither the
     * planar order of the levels nor the fanins that `build_network` needs.
     *
     * @param parent Instance whose network and cone weights are shared.
     * @param p Parameters.
     * @param st Statistics.
     */
    node_duplication_planarization_impl(const node_duplication_planarization_impl&   parent,
                                        const node_duplication_planarization_params& p,
                                        node_duplication_planarization_stats&        st) :
            ntk{parent.ntk},
            ps{p},
            pst{st},
            record{false},
            cone_weights{parent.cone_weights}
    {}
    /**
     * @brief Maps a node id to the source node it stands for.
     *
     * @param n Source node id or copy id.
     * @return The source node.
     */
    [[nodiscard]] mockturtle::node<Ntk> origin(const mockturtle::node<Ntk> n) const noexcept
    {
        return n < ntk.size() ? n : copy_origin[n - ntk.size()];
    }
    /**
     * @brief Allocates a new copy id for a source node.
     *
     * @param n Source node.
     * @return The copy id.
     */
    [[nodiscard]] mockturtle::node<Ntk> make_copy(const mockturtle::node<Ntk> n)
    {
        if (ps.max_duplications > 0 && copy_origin.size() >= ps.max_duplications)
        {
            throw std::runtime_error(
                fmt::format("Planarization aborted: more than {} duplications", ps.max_duplications));
        }

        copy_origin.push_back(n);

        if (record)
        {
            fanins_of.emplace_back();
        }

        return static_cast<mockturtle::node<Ntk>>(ntk.size() + copy_origin.size() - 1);
    }
    /**
     * @brief Removes the duplicates created since a given count, which belong to a level that keeps its crossings.
     *
     * The nodes of the level above connect to the source nodes of the removed duplicates.
     *
     * @param copies_before Number of duplicates to keep.
     */
    void discard_copies(const std::size_t copies_before)
    {
        for (const auto& n : levels.back())
        {
            std::ranges::for_each(fanins_of[n], [this](auto& c) { c = origin(c); });
        }

        copy_origin.resize(copies_before);
        fanins_of.resize(ntk.size() + copies_before);
    }
    /**
     * @brief Computes the planar order of the level below a given level.
     *
     * Builds the H-graph of the level, one slice per node, and follows its shortest path.
     *
     * Duplicates of the level below receive copy ids.
     *
     * @param level Nodes of the current level, source ids or copy ids, in planar order.
     * @return Nodes of the level below in planar order, duplicates as copy ids.
     */
    [[nodiscard]] node_list next_level_of(const node_list& level)
    {
        hgraph_size = 0;

        for (const auto& n : level)
        {
            slice_fanins.clear();
            compute_slice_delays(n);
        }

        return compute_node_order();
    }
    /**
     * @brief Adds one slice to the H-graph of the current level.
     *
     * A slice holds every ordering of the fanins of `n` as an H-graph node (`enumerate_orderings`). The delay of each
     * ordering is the shortest path from the first slice of the level: moving to an ordering whose first fanin equals
     * the last fanin of the previous ordering costs 1, any other move costs 2, and the first slice starts at 1. Every
     * shortest path duplicates the same number of nodes but not the same nodes: where consecutive consumers share
     * several fanins, only one is placed once between them and the others are copied. Ties between equal delays are
     * therefore broken in favour of the path whose shared fanins have the heaviest cones, so that the copies drag the
     * lightest cones along, and then in favour of paths that place two different fanins with a common fanin in the
     * level below side by side, which avoids a duplication there. The slice is appended to `hgraph`.
     *
     * @param n Node (source id or copy id) whose fanins form the slice.
     */
    void compute_slice_delays(const mockturtle::node<Ntk> n)
    {
        const auto o = origin(n);

        // keep rank order among equal delays: a later insertion never overwrites an earlier one
        ntk.foreach_fanin(o,
                          [this](const auto& f)
                          {
                              const auto fn = ntk.get_node(f);

                              if (ntk.is_constant(fn))
                              {
                                  return;
                              }

                              const auto it = std::lower_bound(slice_fanins.cbegin(), slice_fanins.cend(), fn,
                                                               [this](const auto& a, const auto& b)
                                                               { return ntk.rank_position(a) < ntk.rank_position(b); });

                              slice_fanins.insert(it, fn);
                          });

        if (slice_fanins.empty())
        {
            throw std::invalid_argument("A gate has only constant fanins; propagate constants before planarization");
        }

        if (hgraph_size == hgraph.size())
        {
            hgraph.emplace_back();
        }

        auto& orderings = hgraph[hgraph_size];

        enumerate_orderings<Ntk>(n, slice_fanins, orderings);

        if (hgraph_size == 0)
        {
            for (auto& ordering : orderings)
            {
                ordering.delay = 1;
            }
        }
        else
        {
            const auto& previous = hgraph[hgraph_size - 1];

            for (auto& cur : orderings)
            {
                // the cone weight of the fanin this ordering can share with the previous one
                const double share = cone_weight(cur.first_fanin);

                // relax every edge from the previous slice: a shared fanin costs one placement, otherwise two. Three
                // keys decide: the shorter path, among equally short ones the heavier shared cone, and among those
                // a path whose adjacent fanins share a fanin in the level below
                for (std::size_t last_idx = 0; last_idx < previous.size(); ++last_idx)
                {
                    const auto& last = previous[last_idx];

                    const bool   shares = cur.first_fanin == last.last_fanin;
                    const auto   delay  = last.delay + (shares ? 1 : 2);
                    const double weight = last.shared_weight + (shares ? share : 0.0);

                    // evaluated at most once, and only if the delay and the weight do not decide
                    std::optional<bool> below{};
                    const auto          shares_below = [&]
                    {
                        if (!below.has_value())
                        {
                            below = !shares && share_fanin(cur.first_fanin, last.last_fanin);
                        }

                        return *below;
                    };

                    bool better = false;

                    if (delay != cur.delay)
                    {
                        better = delay < cur.delay;
                    }
                    else if (std::abs(weight - cur.shared_weight) > WEIGHT_TOLERANCE)
                    {
                        better = weight > cur.shared_weight;
                    }
                    else
                    {
                        better = shares_below() && !cur.shares_below;
                    }

                    if (better)
                    {
                        cur.predecessor   = last_idx;
                        cur.delay         = delay;
                        cur.shared_weight = weight;
                        cur.shares_below  = shares_below();
                    }
                }
            }
        }

        ++hgraph_size;
    }
    /**
     * @brief Whether two source nodes have a fanin in common.
     *
     * @param a First node.
     * @param b Second node.
     * @return `true` iff `a` and `b` share at least one fanin.
     */
    [[nodiscard]] bool share_fanin(const mockturtle::node<Ntk> a, const mockturtle::node<Ntk> b) const
    {
        bool shared = false;

        ntk.foreach_fanin(a,
                          [this, &shared, b](const auto& fa)
                          {
                              const auto na = ntk.get_node(fa);

                              if (!ntk.is_constant(na))
                              {
                                  ntk.foreach_fanin(b,
                                                    [this, &shared, na](const auto& fb)
                                                    {
                                                        shared = shared || na == ntk.get_node(fb);
                                                        return !shared;
                                                    });
                              }

                              return !shared;
                          });

        return shared;
    }
    /**
     * @brief Places one fanin into the next level, which is built from right to left.
     *
     * If the rightmost node already placed stands for `n`, that copy is shared. Otherwise `n` is appended: as the
     * source node itself when it has not been placed in this level yet, as a fresh copy when it has.
     *
     * @param n Source node to place.
     * @param level_rtl Next level under construction, rightmost node first.
     * @param fanins Receives the id (source or copy) that the consumer connects to; ignored by instances that do not
     * record.
     */
    void place_fanin(const mockturtle::node<Ntk> n, std::vector<mockturtle::node<Ntk>>& level_rtl,
                     std::vector<mockturtle::node<Ntk>>& fanins)
    {
        if (!level_rtl.empty() && origin(level_rtl.back()) == n)
        {
            if (record)
            {
                fanins.push_back(level_rtl.back());
            }
            return;
        }

        if (placed_in[n] != level_stamp)
        {
            placed_in[n] = level_stamp;
            level_rtl.push_back(n);
            if (record)
            {
                fanins.push_back(n);
            }
        }
        else
        {
            const auto copy = make_copy(n);
            level_rtl.push_back(copy);
            if (record)
            {
                fanins.push_back(copy);
            }
        }
    }
    /**
     * @brief Places the fanins of one H-graph ordering into the next level and records them for the ordering's root.
     *
     * @param ordering H-graph node to place.
     * @param level_rtl Next level under construction, rightmost node first.
     */
    void place_ordering(const hgraph_node<Ntk>& ordering, std::vector<mockturtle::node<Ntk>>& level_rtl)
    {
        std::vector<mockturtle::node<Ntk>> fanins{};

        if (record)
        {
            fanins.reserve(ordering.middle_fanins.size() + 2);
        }

        place_fanin(ordering.last_fanin, level_rtl, fanins);

        for (const auto& n : ordering.middle_fanins)
        {
            place_fanin(n, level_rtl, fanins);
        }

        if (ordering.first_fanin != ordering.last_fanin)
        {
            place_fanin(ordering.first_fanin, level_rtl, fanins);
        }

        if (record)
        {
            fanins_of[ordering.root] = std::move(fanins);
        }
    }
    /**
     * @brief Computes the order of the next level from the H-graph of the current level.
     *
     * The ordering with the least delay in the last slice is chosen and its predecessors are followed back to the first
     * slice.
     *
     * The fanins of each ordering are placed from right to left as they are encountered.
     *
     * @return Nodes of the next level in planar order, duplicates as copy ids.
     */
    [[nodiscard]] std::vector<mockturtle::node<Ntk>> compute_node_order()
    {
        std::vector<mockturtle::node<Ntk>> level_rtl{};

        // a fresh stamp empties the set of source nodes placed in the level without touching every node
        if (placed_in.empty())
        {
            placed_in.assign(ntk.size(), 0);
        }
        ++level_stamp;

        if (hgraph_size == 0)
        {
            return level_rtl;
        }

        const auto& last_slice = hgraph[hgraph_size - 1];

        // the least delay; among equal delays the heaviest shared weight; among those the first in rank order
        const auto best = std::min_element(
            last_slice.cbegin(), last_slice.cend(), [](const auto& a, const auto& b)
            { return a.delay < b.delay || (a.delay == b.delay && a.shared_weight > b.shared_weight); });

        if (best == last_slice.cend())
        {
            return level_rtl;
        }

        place_ordering(*best, level_rtl);

        // walk the shortest path back to the first slice, placing every ordering on the way
        std::size_t slice       = hgraph_size - 1;
        std::size_t predecessor = best->predecessor;

        while (slice > 0 && predecessor < hgraph[slice - 1].size())
        {
            const auto& ordering = hgraph[slice - 1][predecessor];

            place_ordering(ordering, level_rtl);

            --slice;
            predecessor = ordering.predecessor;
        }

        std::reverse(level_rtl.begin(), level_rtl.end());

        return level_rtl;
    }
    /**
     * @brief Whether a level consists of primary inputs only.
     *
     * @param level Nodes of the level.
     * @return `true` iff every node stands for a primary input.
     */
    [[nodiscard]] bool is_final_level(const std::vector<mockturtle::node<Ntk>>& level) const
    {
        return std::all_of(level.cbegin(), level.cend(), [this](const auto& n) { return ntk.is_pi(origin(n)); });
    }
    /**
     * @brief Builds the planar `virtual_pi_network` from the levelized duplication order.
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

        old2new[ntk.get_node(ntk.get_constant(false))] = dest.get_constant(false);
        if (ntk.get_node(ntk.get_constant(true)) != ntk.get_node(ntk.get_constant(false)))
        {
            old2new[ntk.get_node(ntk.get_constant(true))] = dest.get_constant(true);
        }

        ntk.foreach_pi_unranked([&old2new, &dest](const auto& n) { old2new[n] = dest.create_pi(); });

        levelized_node_order<ntk_dest_t> lvls_new(levels.size());

        for (auto i = levels.size(); i-- > 0;)
        {
            const auto& lvl     = levels[i];
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
            [this, &old2new, &dest](const auto& po)
            {
                const auto sig = old2new[ntk.get_node(po)];

                dest.create_po(ntk.is_complemented(po) ? dest.create_not(sig) : sig);
            });

        // an input that drives nothing is reached from no output, so no level holds it; it belongs to level 0 of the
        // result all the same, after the inputs the duplication placed
        if (!levels.empty() && is_final_level(levels.back()))
        {
            std::vector<bool> placed(ntk.size(), false);
            for (const auto& n : levels.back())
            {
                placed[origin(n)] = true;
            }

            ntk.foreach_pi_unranked(
                [&placed, &old2new, &dest, &inputs = lvls_new.back()](const auto& n)
                {
                    if (!placed[n])
                    {
                        inputs.push_back(dest.get_node(old2new[n]));
                    }
                });
        }

        // `levels` starts at the primary outputs; the levels of the result start at the primary inputs
        std::reverse(lvls_new.begin(), lvls_new.end());

        dest.update_ranks();
        dest.set_all_ranks(lvls_new);

        return dest;
    }
    /**
     * @brief Collects the fanin signals of a node in the destination network, in the fanin order of its source node.
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

        // ids of the level below that this node connects to; a fanin that occurs twice connects to one copy
        const auto& candidates = fanins_of[n];

        ntk.foreach_fanin(o,
                          [this, &old2new, &dest, &candidates, &children](const auto& f)
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

                                  sig = old2new[*it];
                              }

                              children.push_back(ntk.is_complemented(f) ? dest.create_not(sig) : sig);
                          });

        return children;
    }
    /**
     * @brief Decides whether keeping the crossings of a level beats duplicating it, according to the decision
     * criterion.
     *
     * @param crossing_gate_cost Nodes the crossing gates and their padding add.
     * @param duplicated The level as the duplication strategy would build it, duplicates as copy ids.
     * @param crossed The level in its crossing-minimized source order.
     * @param lvl Level in the source network.
     * @param copies_on_level Number of duplicates in `duplicated`.
     * @return `true` iff the crossings are cheaper.
     */
    [[nodiscard]] bool crossings_are_cheaper(const uint64_t                            crossing_gate_cost,
                                             const std::vector<mockturtle::node<Ntk>>& duplicated,
                                             const std::vector<mockturtle::node<Ntk>>& crossed, const uint32_t lvl,
                                             const uint64_t copies_on_level) const
    {
        if (crossing_gate_cost == std::numeric_limits<uint64_t>::max())
        {
            return false;
        }

        if (ps.criterion == node_duplication_planarization_params::decision_criterion::LOOKAHEAD)
        {
            // what each option costs in nodes: the crossing gates or the copies on this level, plus what the
            // duplication strategy creates below it; the second run stops once it loses
            const auto below_crossed = copies_below(crossed, ps.lookahead_budget);

            if (below_crossed <= ps.lookahead_budget)
            {
                const auto crossing_total = crossing_gate_cost + below_crossed;

                if (copies_on_level > crossing_total)
                {
                    return true;
                }

                const auto remaining = crossing_total - copies_on_level;

                return copies_below(duplicated, remaining) > remaining;
            }

            if (copies_below(duplicated, ps.lookahead_budget) <= ps.lookahead_budget)
            {
                return false;
            }
        }

        return crossing_gate_cost < duplication_cost(duplicated, lvl);
    }
    /**
     * @brief Counts the nodes the duplication strategy creates below a level, given its order, by running it on a copy
     * of the state.
     *
     * Stops early once the count exceeds `budget`.
     *
     * @param level Nodes of the level, duplicates as copy ids of this instance or as repeated source nodes.
     * @param budget Count at which the run is cut off.
     * @return Nodes created below the level, or a value above `budget` if cut off.
     */
    [[nodiscard]] uint64_t copies_below(const std::vector<mockturtle::node<Ntk>>& level, const uint64_t budget) const
    {
        // the cone weights steer the tie-breaks, so the run must weigh like the main one
        node_duplication_planarization_params sub_ps{};
        sub_ps.strategy         = node_duplication_planarization_params::planarization_strategy::DUPLICATION;
        sub_ps.max_duplications = 0;  // the budget cuts the run off
        sub_ps.duplication_cost = ps.duplication_cost;

        node_duplication_planarization_stats sub_st{};

        node_duplication_planarization_impl sub{*this, sub_ps, sub_st};

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
     * @brief Runs the duplication strategy from a given level down to the primary inputs and counts the nodes it
     * creates.
     *
     * @param start Nodes of the level to start from, duplicates as copy ids of this instance.
     * @param budget Count at which the run is cut off.
     * @return Nodes created below `start`, or a value above `budget` if cut off.
     */
    [[nodiscard]] uint64_t run_from(const std::vector<mockturtle::node<Ntk>>& start, const uint64_t budget)
    {
        const auto initial = copy_origin.size();

        auto next_level = next_level_of(start);
        bool at_inputs  = is_final_level(next_level);

        while (!next_level.empty() && !at_inputs)
        {
            if (copy_origin.size() - initial > budget)
            {
                return budget + 1;
            }

            next_level = next_level_of(next_level);
            at_inputs  = is_final_level(next_level);
        }

        return copy_origin.size() - initial;
    }
    /**
     * @brief Tolerance for comparing cone weights, which are sums of the model's weights scaled by powers of
     * `depth_growth`.
     *
     * Ties up to rounding count as equal.
     */
    static constexpr double WEIGHT_TOLERANCE = 1e-9;
    /**
     * @brief Marks a node in `cone_weight_map` whose cone has not been weighed.
     */
    static constexpr double UNWEIGHED = -1.0;
    /**
     * @brief Weighted size of the transitive fanin of a source node, see `duplication_cost_model`.
     *
     * @param root Source node.
     * @param current_level Level on which the node is duplicated.
     * @return Weighted cone size.
     */
    [[nodiscard]] uint64_t cone_cost(const mockturtle::node<Ntk> root, const uint32_t current_level) const
    {
        const auto& m = ps.duplication_cost;

        const double scale = std::pow(m.depth_growth, static_cast<double>(current_level));

        return to_cost(cone_weight(root) * scale);
    }
    /**
     * @brief Weighted size of the transitive fanin cone of a source node under `duplication_cost_model`, without the
     * depth scaling.
     *
     * A gate weighs `node_weight`, a chain buffer or inverter `buffer_weight`.
     *
     * Memoized per source node.
     *
     * @param root Source node.
     * @return Weighted cone size.
     */
    [[nodiscard]] double cone_weight(const mockturtle::node<Ntk> root) const
    {
        const auto& m = ps.duplication_cost;

        auto& weight = cone_weights->weight[root];

        if (weight != UNWEIGHED)
        {
            return weight;
        }

        // a fresh stamp empties the set of visited nodes without touching every node
        const auto stamp = ++cone_weights->stamp;

        std::vector<mockturtle::node<Ntk>> stack{};

        const auto visit = [&stack, &visited = cone_weights->visited, stamp](const mockturtle::node<Ntk> n)
        {
            if (visited[n] != stamp)
            {
                visited[n] = stamp;
                stack.push_back(n);
            }
        };

        visit(root);

        double total = 0.0;

        while (!stack.empty())
        {
            const auto n = stack.back();
            stack.pop_back();

            total += (ntk.fanin_size(n) == 1 && ntk.fanout_size(n) == 1) ? m.buffer_weight : m.node_weight;

            ntk.foreach_fanin(n, [&visit, this](const auto& f) { visit(ntk.get_node(f)); });
        }

        weight = total;

        return total;
    }
    /**
     * @brief Rounds a cost to an integer, saturating at the maximum.
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
     * @brief Cost of the duplicates in a level: every copy beyond the first occurrence of a source node costs the
     * weighted size of that node's cone.
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
                cost += (count - 1) * cone_cost(n, lvl);
            }
        }

        return cost;
    }
    /**
     * @brief Source network.
     */
    const Ntk& ntk;
    /**
     * @brief Parameters.
     */
    const node_duplication_planarization_params& ps;
    /**
     * @brief Statistics.
     */
    node_duplication_planarization_stats& pst;
    /**
     * @brief Source node of every copy id, indexed by `copy id - ntk.size()`.
     */
    std::vector<mockturtle::node<Ntk>> copy_origin{};
    /**
     * @brief For every source id and copy id, the ids of the level below it connects to, in placement order.
     */
    std::vector<node_list> fanins_of;
    /**
     * @brief H-graph of the current level, one slice per node of the level.
     */
    std::vector<hgraph_slice> hgraph{};
    /**
     * @brief Number of slices of `hgraph` in use; the slices beyond it keep their storage for the next level.
     */
    std::size_t hgraph_size{0};
    /**
     * @brief Fanins of the node whose slice is being computed, in rank order.
     */
    std::vector<mockturtle::node<Ntk>> slice_fanins{};
    /**
     * @brief Levelized duplication order, level 0 being the primary outputs; duplicates as copy ids.
     */
    levelized_node_order<Ntk> levels{};
    /**
     * @brief Stamp of the level in which each source node was last placed itself, indexed by source node; allocated on
     * first use.
     */
    std::vector<uint32_t> placed_in{};
    /**
     * @brief Stamp of the level under construction, never 0.
     */
    uint32_t level_stamp{0};
    /**
     * @brief Whether the instance records the order and the fanins of the levels.
     *
     * Counting instances (see `copies_below`) do not.
     */
    bool record{true};
    /**
     * @brief Cone weights of this instance, filled on demand; counting instances use their parent's.
     */
    cone_weight_map own_cone_weights{};
    /**
     * @brief Unscaled cone weight per source node, filled on demand.
     */
    cone_weight_map* cone_weights;
};

}  // namespace detail

/**
 * @brief Planarizes a ranked, balanced logic network by duplicating nodes.
 *
 * The algorithm was originally proposed in \"Fabricatable Interconnect and Molecular QCA Circuits\" by A. Chaudhary,
 * D. Z. Chen, X. S. Hu, M. T. Niemier, R. Ravichandran, and K. Whitton in IEEE TCAD 2007. It solves the node
 * duplication crossing elimination problem level by level from the primary outputs to the primary inputs. For each
 * level, an H-graph enumerates the fanin orderings of every node; the shortest path through it gives an ordering of the
 * level below in which every edge can be drawn without a crossing, at the cost of duplicating the nodes that would
 * otherwise be crossed. Duplicated primary inputs become virtual primary inputs of the result.
 *
 * Consecutive consumers in a level share one copy of their common fanin, so the result is not fanout-substituted;
 * `planar_fanout_substitution` restores that property while keeping ranks and planarity.
 *
 * With the hybrid strategy, the algorithm decides per level whether duplicating is cheaper than keeping the crossings
 * and resolving them later with `crossing_gate_planarization`: the duplication cost is the weighted size of the
 * duplicated cones, the crossing cost the size of the crossing gates plus their padding after a crossing minimization
 * of the level. Levels that keep their crossings are not duplicated, and the result is then not planar.
 *
 * For a bounded fanin, the duplication strategy takes time linear in the number of nodes it creates, which can be
 * exponential in the depth of the network. The hybrid strategy adds a crossing minimization per level. Its `LOOKAHEAD`
 * criterion also runs the duplication strategy below every level that has duplicates, at most twice, each run cut off
 * at `lookahead_budget` created nodes; the time is quadratic in the depth of the network in the worst case.
 * `WEIGHTED_CONE` avoids these runs and weighs every distinct cone once, which takes time proportional to the sum of
 * the cone sizes. Every level also allocates scratch space linear in the number of nodes of the input network.
 *
 * The input must be balanced with unified outputs (see `network_balancing`), carry ranks (see `mutable_rank_view`),
 * and contain no virtual primary inputs (see `delete_virtual_pis`). The result is ranked; with the duplication strategy
 * it is crossing-free, which the function verifies before it returns.
 *
 * @tparam Ntk Ranked, balanced source network type.
 * @param ntk Source network.
 * @param ps Parameters.
 * @param pst Statistics.
 * @return `virtual_pi_network` that computes the same functions as `ntk`; planar with the duplication strategy.
 * @throws std::invalid_argument If `ntk` is not balanced with unified outputs, contains virtual primary inputs, or
 * has a gate whose fanins are all constants; or if `ps.lookahead_budget` is zero or the duplication cost model has a
 * weight that is negative or not finite or a depth growth that is not positive.
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
    static_assert(mockturtle::has_foreach_po_v<Ntk>, "Ntk does not implement the foreach_po method");
    static_assert(mockturtle::has_foreach_fanin_v<Ntk>, "Ntk does not implement the foreach_fanin method");
    static_assert(mockturtle::has_fanout_size_v<Ntk>, "Ntk does not implement the fanout_size method");
    static_assert(mockturtle::has_node_function_v<Ntk>, "Ntk does not implement the node_function method");
    static_assert(has_foreach_pi_unranked_v<Ntk>, "Ntk does not implement the foreach_pi_unranked method");

    if (!is_balanced(ntk, {.unify_outputs = true, .buffer_constant_outputs = false}))
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

    if (ps.lookahead_budget == 0)
    {
        throw std::invalid_argument("The lookahead budget must be positive");
    }

    if (const auto& m = ps.duplication_cost; !std::isfinite(m.node_weight) || m.node_weight < 0.0 ||
                                             !std::isfinite(m.buffer_weight) || m.buffer_weight < 0.0 ||
                                             !std::isfinite(m.depth_growth) || m.depth_growth <= 0.0)
    {
        throw std::invalid_argument(
            "The duplication cost model needs finite, non-negative weights and a finite, positive depth growth");
    }

    node_duplication_planarization_stats st{};

    detail::node_duplication_planarization_impl<Ntk> p{ntk, ps, st};

    auto result = p.run();

    if (ps.strategy == node_duplication_planarization_params::planarization_strategy::DUPLICATION)
    {
        if (const auto crossings = utils::graph::count_crossings(result); crossings != 0)
        {
            throw std::runtime_error(fmt::format("Planarization failed: the result contains {} crossings", crossings));
        }
    }

    if (pst != nullptr)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction::synthesis
