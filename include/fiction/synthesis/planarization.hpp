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
#include <stdexcept>

namespace fiction::synthesis
{

/**
 * @brief Parameters for the planarization pipeline.
 */
struct planarization_params
{
    /**
     * @brief Receives completed work and the phase total of every stage in turn, unless `duplication.on_progress` is
     * set.
     */
    utils::progress_callback on_progress{};
    /**
     * @brief Parameters of the node duplication stage, including the strategy and whether the crossing gates are built
     * from XOR gates.
     *
     * The pipeline defaults to the hybrid strategy, which decides per level between duplicating nodes and keeping the
     * crossings for the crossing gates.
     */
    node_duplication_planarization_params duplication{
        .strategy = node_duplication_planarization_params::planarization_strategy::HYBRID};
    /**
     * @brief Maximum output degree of the fanout nodes in the result.
     *
     * Must be at least 2.
     */
    uint32_t fanout_degree = 2u;
};

/**
 * @brief Statistics of the planarization pipeline.
 */
struct planarization_stats
{
    /**
     * @brief Runtime of the whole pipeline.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * @brief Statistics of the node duplication stage.
     */
    node_duplication_planarization_stats duplication{};
    /**
     * @brief Statistics of the crossing gate stage; empty if no level kept its crossings.
     */
    crossing_gate_planarization_stats crossing_gates{};
    /**
     * @brief Number of nodes of the result.
     */
    uint64_t num_nodes{0};
    /**
     * @brief Writes the statistics to a stream.
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
 * @brief Planarizes a balanced, ranked logic network and returns a planar, balanced, fanout-substituted network that
 * computes the same functions.
 *
 * The pipeline runs `node_duplication_planarization` with the chosen strategy, resolves the crossings that the hybrid
 * strategy kept with `crossing_gate_planarization`, restores fanout nodes with `planar_fanout_substitution`, and
 * removes the buffers that this leaves behind with `planar_rebalancing`.
 *
 * Duplicated primary inputs become virtual primary inputs of the result. The function checks the result for crossings
 * and for balance with unified outputs. The fanout degree is not checked; `is_fanout_substituted` does it on request.
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

    networks::virtual_pi_network<Ntk> result{};

    // the stopwatch must stop before the statistics are copied out
    {
        const mockturtle::stopwatch stop{st.time_total};

        auto duplication_ps        = ps.duplication;
        duplication_ps.on_progress = progress;

        auto planar = node_duplication_planarization(ntk, duplication_ps, &st.duplication);

        if (st.duplication.num_crossing_levels > 0)
        {
            auto cg_ps        = ps.duplication.crossing_gates;
            cg_ps.on_progress = progress;

            planar = crossing_gate_planarization(planar, cg_ps, &st.crossing_gates);
        }

        const planar_fanout_substitution_params fs_ps{.on_progress = progress, .degree = ps.fanout_degree};

        const planar_rebalancing_params rb_ps{.on_progress = progress};

        result = planar_rebalancing(planar_fanout_substitution(planar, fs_ps), rb_ps);

        // the stages keep their contracts by construction; a cheap check turns any slip into an error rather than
        // a silently wrong result
        if (const auto crossings = utils::graph::count_crossings(result); crossings != 0)
        {
            throw std::runtime_error(fmt::format("Planarization failed: the result contains {} crossings", crossings));
        }

        if (!is_balanced(result, {.unify_outputs = true, .buffer_constant_outputs = false}))
        {
            throw std::runtime_error("Planarization failed: the result is not balanced with unified outputs");
        }

        st.num_nodes = result.size();
    }

    if (pst != nullptr)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction::synthesis
