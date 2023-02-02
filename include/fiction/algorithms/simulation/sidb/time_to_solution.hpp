//
// Created by Jan Drewniok on 23.12.22.
//

#ifndef FICTION_TIME_TO_SOLUTION_HPP
#define FICTION_TIME_TO_SOLUTION_HPP

#include "fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp"
#include "fiction/algorithms/simulation/sidb/is_groundstate.hpp"
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
 * quicksim (see quicksim.hpp).
 *
 */
struct time_to_solution_stats
{
    double time_to_solution{};
    double acc{};
    double mean_single_runtime{};

    void report(std::ostream& out = std::cout)
    {
        out << fmt::format("time_to_solution: {} | acc: {} | t_(s): {} \n", time_to_solution, acc, mean_single_runtime);
    }
};
/**
 * This function determines the time-to-solution (TTS) and the accuracy (acc) of the *quicksim* algorithm.
 *
 * @tparam Lyt Cell-level layout type.
 * @param lyt Layout that is used for the simulation.
 * @param result_exact Statistics of the exhaustive simulation.
 * @param ps Pointer to a struct where the results (time_to_solution, acc, single runtime) are stored.
 * @param repetitions Number of repetitions to determine the simulation accuracy (repetitions = 100 ==> accuracy is
 * precise to 1%).
 * @param confidence_level The time-to-solution also depends one the given confidence level which can be set here.
 */
template <typename Lyt>
void sim_acc_tts(charge_distribution_surface<Lyt>& lyt, exgs_stats<Lyt>& result_exact,
                 time_to_solution_stats* ps = nullptr, const uint64_t& repetitions = 100,
                 const double confidence_level = 0.997) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");

    time_to_solution_stats st{};

    std::size_t         gs_count = 0;
    std::vector<double> time{};
    time.reserve(repetitions);

    for (uint64_t i = 0; i < repetitions; i++)
    {
        quicksim_stats<Lyt> stats_quick{};
        quicksim_params     quicksim_params{};

        const auto t_start = std::chrono::high_resolution_clock::now();

        quicksim<Lyt>(lyt, quicksim_params, &stats_quick);

        const auto t_end      = std::chrono::high_resolution_clock::now();
        const auto elapsed    = t_end - t_start;
        const auto diff_first = std::chrono::duration<double>(elapsed).count();

        time.push_back(diff_first);

        if (is_groundstate(stats_quick, result_exact))
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
