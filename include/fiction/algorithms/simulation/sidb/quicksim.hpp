//
// Created by Jan Drewniok on 11.01.23.
//

#ifndef FICTION_QUICKSIM_HPP
#define FICTION_QUICKSIM_HPP

#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/traits.hpp"

#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <cstdint>
#include <mutex>
#include <thread>
#include <vector>

namespace fiction
{

/**
 * This struct stores the parameters for the *QuickSim* algorithm.
 */
struct quicksim_params
{
    /**
     * Simulation parameters for the simulation of the physical SiDB system.
     */
    sidb_simulation_parameters simulation_parameters{};
    /**
     * Number of iterations to run the simulation for.
     */
    uint64_t iteration_steps{80};
    /**
     * `alpha` parameter for the *QuickSim* algorithm (should be reduced if no result is found).
     */
    double alpha{0.7};
    /**
     * Number of threads to spawn. By default the number of threads is set to the number of available hardware threads.
     */
    uint64_t number_threads{std::thread::hardware_concurrency()};
};

/**
 * The *QuickSim* algorithm which was proposed in \"QuickSim: Efficient and Accurate Physical Simulation of Silicon
 * Dangling Bond Logic\" by J. Drewniok, M. Walter, S. S. H. Ng, K. Walus, and R. Wille in IEEE NANO 2023
 * (https://ieeexplore.ieee.org/document/10231266) is an electrostatic ground state simulation algorithm for SiDB
 * layouts. It determines physically valid charge configurations (with minimal energy) of a given (already initialized)
 * charge distribution layout. Depending on the simulation parameters, the ground state is found with a certain
 * probability after one run.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt The layout to simulate.
 * @param ps Physical parameters. They are material-specific and may vary from experiment to experiment.
 * @return sidb_simulation_result is returned with all results.
 */
template <typename Lyt>
sidb_simulation_result<Lyt> quicksim(const Lyt& lyt, const quicksim_params& ps = quicksim_params{})
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt must be an SiDB layout");

    if (ps.iteration_steps == 0)
    {
        return sidb_simulation_result<Lyt>{};
    }

    sidb_simulation_result<Lyt> st{};
    st.algorithm_name = "QuickSim";
    st.additional_simulation_parameters.emplace("iteration_steps", ps.iteration_steps);
    st.additional_simulation_parameters.emplace("alpha", ps.alpha);
    st.simulation_parameters = ps.simulation_parameters;
    st.charge_distributions.reserve(ps.iteration_steps);

    mockturtle::stopwatch<>::duration time_counter{};

    // measure run time (artificial scope)
    {
        const mockturtle::stopwatch stop{time_counter};

        charge_distribution_surface<Lyt> charge_lyt{lyt};

        // set the given physical parameters
        charge_lyt.assign_physical_parameters(ps.simulation_parameters);
        charge_lyt.assign_base_number(2);
        charge_lyt.assign_all_charge_states(sidb_charge_state::NEGATIVE);
        charge_lyt.update_after_charge_change(dependent_cell_mode::VARIABLE);
        const auto negative_sidb_indices = charge_lyt.negative_sidb_detection();

        // Check that the layout with all SiDBs negatively charged is physically valid.
        if (charge_lyt.is_physically_valid())
        {
            st.charge_distributions.push_back(charge_distribution_surface<Lyt>{charge_lyt});
        }

        // Check that the layout with all SiDBs neutrally charged is physically valid.
        charge_lyt.assign_all_charge_states(sidb_charge_state::NEUTRAL);
        charge_lyt.update_after_charge_change();

        if (!negative_sidb_indices.empty())
        {
            if (charge_lyt.is_physically_valid())
            {
                st.charge_distributions.push_back(charge_distribution_surface<Lyt>{charge_lyt});
            }
        }

        // Check if the layout where all SiDBs that need to be negatively charged are negatively charged and the rest
        // are neutrally charged is physically valid.
        for (const auto& index : negative_sidb_indices)
        {
            charge_lyt.assign_charge_state_by_cell_index(static_cast<uint64_t>(index), sidb_charge_state::NEGATIVE);
        }
        charge_lyt.update_after_charge_change();
        if (charge_lyt.is_physically_valid())
        {
            st.charge_distributions.push_back(charge_distribution_surface<Lyt>{charge_lyt});
        }

        charge_lyt.assign_all_charge_states(sidb_charge_state::NEUTRAL);
        charge_lyt.update_after_charge_change();

        // If the number of threads is initially set to zero, the simulation is run with one thread.
        const uint64_t num_threads = std::max(ps.number_threads, uint64_t{1});

        // split the iterations among threads
        const auto iter_per_thread =
            std::max(ps.iteration_steps / num_threads,
                     uint64_t{1});  // If the number of set threads is greater than the number of iterations, the
                                    // number of threads defines how many times QuickSim is repeated

        std::vector<std::thread> threads{};
        threads.reserve(num_threads);
        std::mutex mutex{};  // used to control access to shared resources

        for (uint64_t z = 0ul; z < num_threads; z++)
        {
            threads.emplace_back(
                [&]
                {
                    charge_distribution_surface<Lyt> charge_lyt_copy{charge_lyt};

                    for (uint64_t l = 0ul; l < iter_per_thread; ++l)
                    {
                        for (uint64_t i = 0ul; i < charge_lyt.num_cells(); ++i)
                        {
                            {
                                const std::lock_guard lock{mutex};
                                if (std::find(negative_sidb_indices.cbegin(), negative_sidb_indices.cend(), i) !=
                                    negative_sidb_indices.cend())
                                {
                                    continue;
                                }
                            }

                            std::vector<uint64_t> index_start{i};

                            charge_lyt_copy.assign_all_charge_states(sidb_charge_state::NEUTRAL);

                            for (const auto& index : negative_sidb_indices)
                            {
                                charge_lyt_copy.assign_charge_state_by_cell_index(static_cast<uint64_t>(index),
                                                                                  sidb_charge_state::NEGATIVE);
                                index_start.push_back(static_cast<uint64_t>(index));
                            }

                            charge_lyt_copy.assign_charge_state_by_cell_index(i, sidb_charge_state::NEGATIVE);
                            charge_lyt_copy.update_after_charge_change();

                            if (charge_lyt_copy.is_physically_valid())
                            {
                                const std::lock_guard lock{mutex};
                                st.charge_distributions.push_back(charge_distribution_surface<Lyt>{charge_lyt_copy});
                            }

                            const auto upper_limit =
                                std::min(static_cast<uint64_t>(static_cast<double>(charge_lyt_copy.num_cells()) / 1.5),
                                         charge_lyt.num_cells() - negative_sidb_indices.size());

                            for (uint64_t num = 0ul; num < upper_limit; num++)
                            {
                                charge_lyt_copy.adjacent_search(ps.alpha, index_start);
                                charge_lyt_copy.validity_check();

                                if (charge_lyt_copy.is_physically_valid())
                                {
                                    const std::lock_guard lock{mutex};
                                    st.charge_distributions.push_back(
                                        charge_distribution_surface<Lyt>{charge_lyt_copy});
                                }
                            }
                        }
                    }
                });
        }

        for (auto& thread : threads)
        {
            thread.join();
        }
    }

    st.simulation_runtime = time_counter;

    return st;
}

}  // namespace fiction

#endif  // FICTION_QUICKSIM_HPP
