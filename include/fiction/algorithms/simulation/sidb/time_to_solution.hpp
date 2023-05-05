//
// Created by Jan Drewniok on 23.12.22.
//

#ifndef FICTION_TIME_TO_SOLUTION_HPP
#define FICTION_TIME_TO_SOLUTION_HPP

#include "fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp"
#include "fiction/algorithms/simulation/sidb/is_ground_state.hpp"
#include "fiction/algorithms/simulation/sidb/minimum_energy.hpp"
#include "fiction/algorithms/simulation/sidb/quickexact.hpp"
#include "fiction/algorithms/simulation/sidb/quicksim.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/traits.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

namespace fiction
{
/**
 * An enumeration of exact algorithms for the tts-simulation.
 */
enum class exact_algorithm_type
{
    /**
     * ExGS
     */
    EXGS,
    /**
     * QuickExact
     */
    QUICKEXACT
};

struct time_to_solution_params
{
    /**
     * Exact simulation algorithm used to simulate the ground state as reference.
     */
    exact_algorithm_type engine = exact_algorithm_type::EXGS;
    /**
     * Number of repetitions to determine the simulation accuracy (`repetitions = 100` means that accuracy is precise to
     * 1%).
     */
    uint64_t repetitions = 100;
    /**
     * The time-to-solution also depends on the given confidence level which can be set here.
     */
    double confidence_level = 0.997;
};

/**
 * This struct stores the time-to-solution, the simulation accuracy and the average single simulation runtime of
 * *QuickSim* (see quicksim.hpp).
 *
 */
struct time_to_solution_stats
{
    /**
     * Time-to-solution in seconds.
     */
    double time_to_solution{};
    /**
     * Accuracy of the simulation.
     */
    double acc{};
    /**
     * Average single simulation runtime in seconds.
     */
    double mean_single_runtime{};
    /**
     * Single simulation runtime of the exhaustive ground state searcher.
     */
    double single_runtime_exhaustive{};
    /**
     * Number of physically valid charge configurations found by ExGS.
     */
    std::size_t number_valid_layouts_exgs{};
    /**
     * Name of the exact simulation algorithm used.
     */
    std::string algorithm{};
    /**
     * Print the results to the given output stream.
     *
     * @param out Output stream.
     */
    void report(std::ostream& out = std::cout)
    {
        out << fmt::format(
            "[i] time_to_solution: {} | acc: {} | t_(s): {} | t_exhaustive(s): {} | algortihm used: {}\n",
            time_to_solution, acc, mean_single_runtime, single_runtime_exhaustive, algorithm);
    }
};
/**
 * This function determines the time-to-solution (TTS) and the accuracy (acc) of the *QuickSim* algorithm.
 *
 * @tparam Lyt Cell-level layout type.
 * @param lyt Layout that is used for the simulation.
 * @param quicksim_params Parameters required for the quicksim.hpp algorithm.
 * @param ps Pointer to a struct where the results (time_to_solution, acc, single runtime) are stored.
 * @param tts_params Parameters used for the time-to-solution calculation.
 */
template <typename Lyt>
void sim_acc_tts(Lyt& lyt, const quicksim_params& quicksim_params, time_to_solution_stats* ps = nullptr,
                 const time_to_solution_params& tts_params = {}) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");

    time_to_solution_stats      st{};
    sidb_simulation_result<Lyt> simulation_result{};
    if (tts_params.engine == exact_algorithm_type::EXGS)
    {
        st.algorithm      = "exgs";
        simulation_result = exhaustive_ground_state_simulation(lyt, quicksim_params.phys_params);
    }
    else
    {
        const quickexact_params<Lyt> params{quicksim_params.phys_params};
        st.algorithm      = "quickexact";
        simulation_result = quickexact(lyt, params);
    }

    st.single_runtime_exhaustive = mockturtle::to_seconds(simulation_result.simulation_runtime);

    std::size_t         gs_count = 0;
    std::vector<double> time{};
    time.reserve(tts_params.repetitions);

    for (uint64_t i = 0; i < tts_params.repetitions; i++)
    {
        sidb_simulation_result<Lyt> stats_quick{};

        const auto t_start = std::chrono::high_resolution_clock::now();

        const auto simulation_results_quicksim = quicksim<Lyt>(lyt, quicksim_params);

        const auto t_end      = std::chrono::high_resolution_clock::now();
        const auto elapsed    = t_end - t_start;
        const auto diff_first = std::chrono::duration<double>(elapsed).count();

        time.push_back(diff_first);

        if (is_ground_state(simulation_results_quicksim, simulation_result))
        {
            gs_count += 1;
        }
    }

    const auto single_runtime =
        std::accumulate(time.begin(), time.end(), 0.0) / static_cast<double>(tts_params.repetitions);
    const auto acc = static_cast<double>(gs_count) / static_cast<double>(tts_params.repetitions);

    double tts = single_runtime;

    if (acc == 1)
    {
        tts = single_runtime;
    }
    else if (acc == 0)
    {
        tts = std::numeric_limits<double>::max();
    }
    else
    {
        tts = (single_runtime * std::log(1.0 - tts_params.confidence_level) / std::log(1.0 - acc));
    }

    st.time_to_solution    = tts;
    st.acc                 = acc * 100;
    st.mean_single_runtime = single_runtime;

    if (ps)
    {
        *ps = st;
    }
}

}  // namespace fiction

#endif  // FICTION_TIME_TO_SOLUTION_HPP
