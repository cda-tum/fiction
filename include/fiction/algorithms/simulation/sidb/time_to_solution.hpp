//
// Created by Jan Drewniok on 23.12.22.
//

#ifndef FICTION_TIME_TO_SOLUTION_HPP
#define FICTION_TIME_TO_SOLUTION_HPP

#include "fiction/algorithms/simulation/sidb/determine_groundstate_from_simulation_results.hpp"
#include "fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp"
#include "fiction/algorithms/simulation/sidb/is_ground_state.hpp"
#include "fiction/algorithms/simulation/sidb/quickexact.hpp"
#include "fiction/algorithms/simulation/sidb/quicksim.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
#include "fiction/traits.hpp"

#include <fmt/format.h>

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace fiction
{

struct time_to_solution_params
{
    /**
     * Exhaustive simulation algorithm used to simulate the ground state as reference.
     */
    exact_sidb_simulation_engine engine = exact_sidb_simulation_engine::QUICKEXACT;
    /**
     * Number of iterations of the heuristic algorithm used to determine the simulation accuracy (`repetitions = 100`
     * means that accuracy is precise to 1%).
     */
    uint64_t repetitions = 100;
    /**
     * Confidence level.
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
     * Accuracy of the simulation.
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
    void report(std::ostream& out = std::cout)
    {
        out << fmt::format("time_to_solution: {} \n acc: {} \n t_(s): {} \n t_exact(s): {} \n exact alg.: {}\n",
                           time_to_solution, acc, mean_single_runtime, single_runtime_exact, algorithm);
    }
};
/**
 * This function determines the time-to-solution (TTS) and the accuracy (acc) of the *QuickSim* algorithm.
 *
 * @tparam Lyt Cell-level layout type.
 * @param lyt Layout that is used for the simulation.
 * @param quicksim_params Parameters required for the *QuickSim* algorithm.
 * @param tts_params Parameters used for the time-to-solution calculation.
 * @param ps Pointer to a struct where the results (time_to_solution, acc, single runtime) are stored.
 */
template <typename Lyt>
void time_to_solution(const Lyt& lyt, const quicksim_params& quicksim_params,
                      const time_to_solution_params& tts_params = {}, time_to_solution_stats* ps = nullptr) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    time_to_solution_stats st{};

    sidb_simulation_result<Lyt> simulation_result{};
    if (tts_params.engine == exact_sidb_simulation_engine::QUICKEXACT)
    {
        const quickexact_params<cell<Lyt>> params{quicksim_params.simulation_parameters};
        st.algorithm      = "QuickExact";
        simulation_result = quickexact(lyt, params);
    }
    else
    {
        st.algorithm      = "ExGS";
        simulation_result = exhaustive_ground_state_simulation(lyt, quicksim_params.simulation_parameters);
    }

    std::vector<sidb_simulation_result<Lyt>> simulation_results_quicksim{};
    simulation_results_quicksim.reserve(tts_params.repetitions);

    for (auto i = 0u; i < tts_params.repetitions; ++i)
    {
        simulation_results_quicksim.push_back(quicksim<Lyt>(lyt, quicksim_params));
    }

    time_to_solution_for_given_simulation_results(simulation_result, simulation_results_quicksim,
                                                  tts_params.confidence_level, &st);

    if (ps)
    {
        *ps = st;
    }
}

/**
 * This function calculates the Time-to-Solution (TTS) by analyzing the simulation results of a heuristic algorithm
 * in comparison to those of an exact algorithm. It provides further statistical metrics, including the accuracy of the
 * heuristic algorithm, and individual runtimes.
 *
 * @tparam Lyt Cell-level layout type.
 * @param results_exact Simulation results of the exact algorithm.
 * @param results_heuristic Simulation of the heuristic for which the TTS is determined.
 * @param confidence_level Confidence level for the TTS computation
 * @param ps Pointer to a struct where the results (time_to_solution, acc, single runtime) are stored.
 */
template <typename Lyt>
void time_to_solution_for_given_simulation_results(const sidb_simulation_result<Lyt>&              results_exact,
                                                   const std::vector<sidb_simulation_result<Lyt>>& results_heuristic,
                                                   const double            confidence_level = 0.997,
                                                   time_to_solution_stats* ps               = nullptr) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    time_to_solution_stats st{};

    const auto ground_state = determine_groundstate_from_simulation_results(results_exact);

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

    const auto single_runtime_heuristic_average = total_runtime_heuristic / results_heuristic.size();

    const auto acc = static_cast<double>(gs_count) / static_cast<double>(results_heuristic.size());

    double tts = 0.0;

    if (acc == 1)
    {
        tts = single_runtime_heuristic_average;
    }
    else if (acc == 0)
    {
        tts = std::numeric_limits<double>::max();
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

}  // namespace fiction

#endif  // FICTION_TIME_TO_SOLUTION_HPP
