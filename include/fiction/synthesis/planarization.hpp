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
#include "fiction/synthesis/node_duplication_planarization.hpp"
#include "fiction/synthesis/planar_fanout_substitution.hpp"
#include "fiction/synthesis/planar_rebalancing.hpp"
#include "fiction/utils/progress.hpp"

#include <fmt/format.h>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <cstdint>
#include <iostream>

namespace fiction::synthesis
{

/**
 * Parameters for the planarization pipeline.
 */
struct planarization_params
{
    /**
     * Receives completed work and the phase total of every stage in turn.
     */
    utils::progress_callback on_progress{};
    /**
     * Parameters of the node duplication stage, including the strategy and the gadget type of the crossing gates.
     */
    node_duplication_planarization_params duplication{};
    /**
     * Maximum output degree of the fanout nodes in the result.
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
 * @throws std::invalid_argument If `ntk` is not balanced or contains virtual primary inputs.
 * @throws std::runtime_error If a stage cannot keep its contract, see the stages' documentation.
 */
template <typename Ntk>
[[nodiscard]] networks::virtual_pi_network<Ntk> planarization(const Ntk& ntk, const planarization_params& ps = {},
                                                              planarization_stats* pst = nullptr)
{
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
    static_assert(mockturtle::has_rank_position_v<Ntk>, "Ntk does not implement the rank_position method");

    planarization_stats st{};

    const mockturtle::stopwatch stop{st.time_total};

    auto duplication_ps        = ps.duplication;
    duplication_ps.on_progress = ps.on_progress;

    auto planar = node_duplication_planarization(ntk, duplication_ps, &st.duplication);

    if (st.duplication.num_crossing_levels > 0)
    {
        crossing_gate_planarization_params cg_ps{};
        cg_ps.on_progress            = ps.on_progress;
        cg_ps.xor_gates              = ps.duplication.xor_gates;
        cg_ps.max_crossings_per_rank = ps.duplication.max_crossings_per_rank;

        planar = crossing_gate_planarization(planar, cg_ps, &st.crossing_gates);
    }

    planar_fanout_substitution_params fs_ps{};
    fs_ps.on_progress = ps.on_progress;
    fs_ps.degree      = ps.fanout_degree;

    planar_rebalancing_params rb_ps{};
    rb_ps.on_progress = ps.on_progress;

    auto result = planar_rebalancing(planar_fanout_substitution(planar, fs_ps), rb_ps);

    st.num_nodes = result.size();

    if (pst != nullptr)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction::synthesis
