//
// Created by Jan Drewniok on 23.12.22.
//

#ifndef FICTION_TIME_TO_SOLUTION_HPP
#define FICTION_TIME_TO_SOLUTION_HPP

#include "fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp"
#include "fiction/algorithms/simulation/sidb/is_ground_state.hpp"
#include "fiction/algorithms/simulation/sidb/minimum_energy.hpp"
#include "fiction/algorithms/simulation/sidb/quicksim.hpp"
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
     * Print the results to the given output stream.
     *
     * @param out Output stream.
     */
    void report(std::ostream& out = std::cout)
    {
        out << fmt::format("[i] time_to_solution: {} | acc: {} | t_(s): {} | t_exhaustive(s): {}\n", time_to_solution,
                           acc, mean_single_runtime, single_runtime_exhaustive);
    }
};
/**
 * This function determines the time-to-solution (TTS) and the accuracy (acc) of the *QuickSim* algorithm.
 *
 * @tparam Lyt Cell-level layout type.
 * @param lyt Layout that is used for the simulation.
 * @param sidb_params Physical SiDB parameters which are used for the simulation.
 * @param ps Pointer to a struct where the results (time_to_solution, acc, single runtime) are stored.
 * @param repetitions Number of repetitions to determine the simulation accuracy (`repetitions = 100` means that
 * accuracy is precise to 1%).
 * @param confidence_level The time-to-solution also depends on the given confidence level which can be set here.
 */
template <typename Lyt>
void sim_acc_tts(const Lyt& lyt, const quicksim_params& quicksim_params, time_to_solution_stats* ps = nullptr,
                 const uint64_t& repetitions = 100, const double confidence_level = 0.997) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");

    const auto simulation_results_exgs = exhaustive_ground_state_simulation(lyt, quicksim_params.phys_params);

    time_to_solution_stats st{};
    st.single_runtime_exhaustive = mockturtle::to_seconds(simulation_results_exgs.simulation_runtime);

    std::size_t         gs_count = 0;
    std::vector<double> time{};
    time.reserve(repetitions);

    for (auto i = 0u; i < repetitions; ++i)
    {
        sidb_simulation_result<Lyt> stats_quick{};

        const auto t_start = std::chrono::high_resolution_clock::now();

        const auto simulation_results_quicksim = quicksim<Lyt>(lyt, quicksim_params);

        const auto t_end      = std::chrono::high_resolution_clock::now();
        const auto elapsed    = t_end - t_start;
        const auto diff_first = std::chrono::duration<double>(elapsed).count();

        time.push_back(diff_first);

        if (is_ground_state(simulation_results_quicksim, simulation_results_exgs))
        {
            gs_count += 1;
        }
    }

    const auto single_runtime = std::accumulate(time.begin(), time.end(), 0.0) / static_cast<double>(repetitions);
    const auto acc            = static_cast<double>(gs_count) / static_cast<double>(repetitions);

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
        tts = (single_runtime * std::log(1.0 - confidence_level) / std::log(1.0 - acc));
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
