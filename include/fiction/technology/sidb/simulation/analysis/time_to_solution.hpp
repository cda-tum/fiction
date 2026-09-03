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
 * @brief Time-to-solution and accuracy of a heuristic engine against an exact one.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 * @author Willem Lambooy (wlambooy)
 * @author Simon Hofmann (simon1hofmann)
 */

#pragma once

#include "fiction/technology/sidb/cell_level_layout_conversion.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/simulation/engine.hpp"
#include "fiction/technology/sidb/simulation/engines/clustercomplete.hpp"
#include "fiction/technology/sidb/simulation/engines/exhaustive_ground_state_simulation.hpp"
#include "fiction/technology/sidb/simulation/engines/quickexact.hpp"
#include "fiction/technology/sidb/simulation/engines/quicksim.hpp"
#include "fiction/technology/sidb/simulation/is_ground_state.hpp"
#include "fiction/technology/sidb/simulation/result.hpp"
#include "fiction/traits.hpp"

#include <fmt/format.h>
#include <mockturtle/utils/stopwatch.hpp>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace fiction::sidb::simulation::analysis
{

struct time_to_solution_params
{
    /**
     * Exhaustive simulation algorithm used to simulate the ground state as reference.
     */
    sidb::simulation::exact_engine engine = exact_engine::QUICKEXACT;
    /**
     * Number of iterations of the heuristic algorithm used to determine the simulation accuracy (`repetitions = 100`
     * means that accuracy is precise to 1 %).
     */
    uint64_t repetitions = 100;
    /**
     * The confidence level represents the probability that the confidence interval calculated from the simulation
     * contains the true value. For example, a 99.7 % (0.997) confidence level means that if the simulation were
     * repeated many times, approximately 997 out of 1000 of the calculated confidence intervals would contain the true
     * value.
     */
    double confidence_level = 0.997;
};

/**
 * This struct stores the time-to-solution, the simulation accuracy and the average single simulation runtime of
 * *QuickSim*, the single runtime of the exact simulator used, and the number of valid charge
 * configurations found by the exact algorithm.
 */
struct time_to_solution_stats
{
    /**
     * Time-to-solution in seconds.
     */
    double time_to_solution{0};
    /**
     * Accuracy of the simulation in %.
     */
    double acc{};
    /**
     * Average single simulation runtime in seconds.
     */
    double mean_single_runtime{};
    /**
     * Single simulation runtime of the exact ground state simulation algorithm.
     */
    double single_runtime_exact{};
    /**
     * Exact simulation algorithm used to simulate the ground state as reference.
     */
    std::string algorithm;
    /**
     * Print the results to the given output stream.
     *
     * @param out Output stream.
     */
    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("time_to_solution: {} \n acc: {} \n t[s]: {} \n t_exact[s]: {} \n exact alg.: {}\n",
                           time_to_solution, acc, mean_single_runtime, single_runtime_exact, algorithm);
    }
};
/**
 * Computes the time-to-solution (TTS) of *QuickSim* from an exact result and a series of heuristic results: the
 * accuracy is the share of heuristic runs that found the ground state, and TTS is the mean heuristic runtime
 * scaled to reach the given confidence level, `t · log(1 - c) / log(1 - acc)`.
 *
 * @param results_exact The result of an exact engine, which provides the ground state.
 * @param results_heuristic The results of the heuristic runs.
 * @param confidence_level Confidence level for the TTS computation.
 * @param ps Statistics.
 */
inline void time_to_solution_for_given_simulation_results(const result&              results_exact,
                                                          const std::vector<result>& results_heuristic,
                                                          const double               confidence_level = 0.997,
                                                          time_to_solution_stats*    ps = nullptr) noexcept
{
    time_to_solution_stats st{};

    auto        total_runtime_heuristic = 0.0;
    std::size_t gs_count                = 0;

    for (const auto& heuristic : results_heuristic)
    {
        if (is_ground_state(heuristic, results_exact))
        {
            ++gs_count;
        }

        total_runtime_heuristic += mockturtle::to_seconds(heuristic.simulation_runtime);
    }

    const auto single_runtime_heuristic_average =
        total_runtime_heuristic / static_cast<double>(results_heuristic.size());

    const auto acc = static_cast<double>(gs_count) / static_cast<double>(results_heuristic.size());

    double tts = 0.0;

    if (acc == 1)
    {
        tts = single_runtime_heuristic_average;
    }
    else if (acc == 0)
    {
        tts = std::numeric_limits<double>::infinity();
    }
    else
    {
        tts = (single_runtime_heuristic_average * std::log(1.0 - confidence_level) / std::log(1.0 - acc));
    }

    st.single_runtime_exact = mockturtle::to_seconds(results_exact.simulation_runtime);
    st.time_to_solution     = tts;
    st.acc                  = acc * 100;
    st.mean_single_runtime  = single_runtime_heuristic_average;

    if (ps)
    {
        st.algorithm = (*ps).algorithm;
        *ps          = st;
    }
}
/**
 * Computes the time-to-solution (TTS) of *QuickSim* for a layout: the layout is simulated once with the exact
 * engine of the parameters and `tts_params.repetitions` times with *QuickSim*, and the runs are combined with
 * `time_to_solution_for_given_simulation_results`.
 *
 * @param lyt The layout to simulate.
 * @param qs_params *QuickSim* parameters.
 * @param tts_params TTS parameters.
 * @param ps Statistics.
 */
inline void time_to_solution(const layout& lyt, const engines::quicksim_params& qs_params,
                             const time_to_solution_params& tts_params = {},
                             time_to_solution_stats*        ps         = nullptr) noexcept
{
    time_to_solution_stats st{};

    if (lyt.num_cells() == 0)
    {
        st.single_runtime_exact = 0.0;
        st.time_to_solution     = std::numeric_limits<double>::infinity();
        st.acc                  = 0.0;
        st.mean_single_runtime  = 0.0;
        st.algorithm            = engine_name(tts_params.engine);

        if (ps)
        {
            *ps = st;
        }

        return;
    }

    result simulation_result{};

    if (tts_params.engine == exact_engine::QUICKEXACT)
    {
        const engines::quickexact_params params{
            .sim_params = qs_params.sim_params,
            .base_number_detection =
                engines::quickexact_params::automatic_base_number_detection::OFF};

        st.algorithm      = engine_name(exact_engine::QUICKEXACT);
        simulation_result = engines::quickexact(lyt, params);
    }
#if (FICTION_ALGLIB_ENABLED)
    else if (tts_params.engine == exact_engine::CLUSTERCOMPLETE)
    {
        const engines::clustercomplete_params params{.sim_params = qs_params.sim_params};

        st.algorithm      = engine_name(exact_engine::CLUSTERCOMPLETE);
        simulation_result = engines::clustercomplete(lyt, params);
    }
#endif  // FICTION_ALGLIB_ENABLED
    else
    {
        st.algorithm      = engine_name(exact_engine::EXGS);
        simulation_result = engines::exhaustive_ground_state_simulation(lyt, qs_params.sim_params);
    }

    std::vector<result> simulation_results_quicksim{};
    simulation_results_quicksim.reserve(tts_params.repetitions);

    for (auto i = 0u; i < tts_params.repetitions; ++i)
    {
        if (const auto quicksim_result = engines::quicksim(lyt, qs_params); quicksim_result.has_value())
        {
            simulation_results_quicksim.push_back(*quicksim_result);
        }
    }

    time_to_solution_for_given_simulation_results(simulation_result, simulation_results_quicksim,
                                                  tts_params.confidence_level, &st);

    if (ps)
    {
        *ps = st;
    }
}
/**
 * Transitional overload for SiDB cell-level layouts, converted with `to_sidb_layout`; see the `layout` overload.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt The layout to simulate.
 * @param qs_params *QuickSim* parameters.
 * @param tts_params TTS parameters.
 * @param ps Statistics.
 */
template <typename Lyt>
    requires(is_cell_level_layout_v<Lyt>)
void time_to_solution(const Lyt& lyt, const engines::quicksim_params& qs_params,
                      const time_to_solution_params& tts_params = {}, time_to_solution_stats* ps = nullptr) noexcept
{
    time_to_solution(to_sidb_layout(lyt), qs_params, tts_params, ps);
}

}  // namespace fiction::sidb::simulation::analysis
