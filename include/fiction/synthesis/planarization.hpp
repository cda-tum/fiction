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
 * @brief Planarizes a ranked logic network and restores fanouts and balance in one pass.
 * @author Benjamin Hien (hibenj)
 */

#pragma once

#include "fiction/networks/virtual_pi_network.hpp"
#include "fiction/synthesis/crossing_gate_planarization.hpp"
#include "fiction/synthesis/network_balancing.hpp"
#include "fiction/synthesis/node_duplication_planarization.hpp"
#include "fiction/synthesis/planar_fanout_substitution.hpp"
#include "fiction/synthesis/planar_rebalancing.hpp"
#include "fiction/utils/graph/mincross.hpp"
#include "fiction/utils/progress.hpp"

#include <fmt/format.h>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <cstdint>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <utility>

namespace fiction::synthesis
{

/**
 * Parameters for the planarization pipeline.
 */
struct planarization_params
{
    /**
     * Receives completed work and the phase total of every stage in turn, unless `duplication.on_progress` is set.
     */
    utils::progress_callback on_progress{};
    /**
     * Parameters of the node duplication stage, including the strategy and the gadget type of the crossing gates.
     * The pipeline defaults to the hybrid strategy, which decides per level between duplicating nodes and keeping the
     * crossings for the crossing gates.
     */
    node_duplication_planarization_params duplication{
        .strategy = node_duplication_planarization_params::planarization_strategy::HYBRID};
    /**
     * Maximum output degree of the fanout nodes in the result. Must be at least 2.
     */
    uint32_t fanout_degree = 2u;
};

/**
 * Statistics of the planarization pipeline.
 */
struct planarization_stats
{
    /**
     * Runtime of the whole pipeline.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * Statistics of the node duplication stage.
     */
    node_duplication_planarization_stats duplication{};
    /**
     * Statistics of the crossing gate stage; empty if no level kept its crossings.
     */
    crossing_gate_planarization_stats crossing_gates{};
    /**
     * Number of nodes of the result.
     */
    uint64_t num_nodes{0};
    /**
     * Writes the statistics to a stream.
     *
     * @param out Stream to write to.
     */
    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("[i] total time           = {:.2f} secs\n", mockturtle::to_seconds(time_total));
        out << fmt::format("[i] num. duplications    = {}\n", duplication.num_duplications);
        out << fmt::format("[i] num. crossing levels = {}\n", duplication.num_crossing_levels);
        out << fmt::format("[i] num. crossings       = {}\n", crossing_gates.num_crossings);
        out << fmt::format("[i] num. nodes           = {}\n", num_nodes);
    }
};

/**
 * Planarizes a balanced, ranked logic network and returns a planar, balanced, fanout-substituted network that
 * computes the same functions. The pipeline runs `node_duplication_planarization` with the chosen strategy, resolves
 * the crossings that the hybrid strategy kept with `crossing_gate_planarization`, restores fanout nodes with
 * `planar_fanout_substitution`, and removes the buffers that this leaves behind with `planar_rebalancing`.
 *
 * Duplicated primary inputs become virtual primary inputs of the result.
 *
 * @tparam Ntk Ranked, balanced network type (see `mutable_rank_view`) without virtual primary inputs.
 * @param ntk Source network.
 * @param ps Parameters.
 * @param pst Statistics.
 * @return Planar `virtual_pi_network` with unified outputs and fanout nodes of at most `fanout_degree` outputs.
 * @throws std::invalid_argument If `ps.fanout_degree` is below 2, or if `ntk` is not balanced with unified outputs,
 * contains virtual primary inputs, or violates another precondition of `node_duplication_planarization`.
 * @throws std::runtime_error If a stage cannot keep its contract (see the stages' documentation), or if the result is
 * not planar or not balanced with unified outputs after all.
 */
template <typename Ntk>
[[nodiscard]] networks::virtual_pi_network<Ntk> planarization(const Ntk& ntk, const planarization_params& ps = {},
                                                              planarization_stats* pst = nullptr)
{
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
    static_assert(mockturtle::has_rank_position_v<Ntk>, "Ntk does not implement the rank_position method");

    // checked up front, so that the stages before the fanout substitution do not run for nothing
    if (ps.fanout_degree < 2)
    {
        throw std::invalid_argument("The fanout degree must be at least 2");
    }

    planarization_stats st{};

    // a callback set on the duplication stage wins over the pipeline-wide one
    const auto progress = ps.duplication.on_progress ? ps.duplication.on_progress : ps.on_progress;

    std::optional<networks::virtual_pi_network<Ntk>> result{};

    // the stopwatch must stop before the statistics are copied out
    {
        const mockturtle::stopwatch stop{st.time_total};

        auto duplication_ps        = ps.duplication;
        duplication_ps.on_progress = progress;

        auto planar = node_duplication_planarization(ntk, duplication_ps, &st.duplication);

        if (st.duplication.num_crossing_levels > 0)
        {
            crossing_gate_planarization_params cg_ps{};
            cg_ps.on_progress            = progress;
            cg_ps.xor_gates              = ps.duplication.xor_gates;
            cg_ps.max_crossings_per_rank = ps.duplication.max_crossings_per_rank;

            planar = crossing_gate_planarization(planar, cg_ps, &st.crossing_gates);
        }

        planar_fanout_substitution_params fs_ps{};
        fs_ps.on_progress = progress;
        fs_ps.degree      = ps.fanout_degree;

        planar_rebalancing_params rb_ps{};
        rb_ps.on_progress = progress;

        result.emplace(planar_rebalancing(planar_fanout_substitution(planar, fs_ps), rb_ps));

        // the stages keep their contracts by construction; a cheap check turns any slip into an error rather than
        // a silently wrong result
        utils::graph::mincross_params mc_ps{};
        mc_ps.optimize = false;
        utils::graph::mincross_stats mc_st{};

        utils::graph::mincross(*result, mc_ps, &mc_st);

        if (mc_st.num_crossings != 0)
        {
            throw std::runtime_error(
                fmt::format("Planarization failed: the result contains {} crossings", mc_st.num_crossings));
        }

        if (!is_balanced(*result, {.unify_outputs = true, .buffer_constant_outputs = false}))
        {
            throw std::runtime_error("Planarization failed: the result is not balanced with unified outputs");
        }

        st.num_nodes = result->size();
    }

    if (pst != nullptr)
    {
        *pst = st;
    }

    return std::move(*result);
}

}  // namespace fiction::synthesis
