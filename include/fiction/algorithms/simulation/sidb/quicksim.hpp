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
#include "fiction/utils/execution_utils.hpp"

#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <limits>
#include <mutex>
#include <optional>
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
    uint64_t number_threads{1};
    /**
     * Timeout limit (in ms).
     */
    uint64_t timeout = std::numeric_limits<uint64_t>::max();
};

/**
 * The *QuickSim* algorithm which was proposed in \"QuickSim: Efficient and Accurate Physical Simulation of Silicon
 * Dangling Bond Logic\" by J. Drewniok, M. Walter, S. S. H. Ng, K. Walus, and R. Wille in IEEE NANO 2023
 * (https://ieeexplore.ieee.org/document/10231266) is an electrostatic ground state simulation algorithm for SiDB
 * layouts. It determines physically valid charge configurations (with minimal energy) of a given (already initialized)
 * charge distribution layout. Depending on the simulation parameters, the ground state is found with a certain
 * probability after one run.
 *
 * @note *QuickSim* currently does not support atomic defect simulation.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt The layout to simulate.
 * @param ps QuickSim parameters.
 * @return `sidb_simulation_result` is returned if the simulation was successful, otherwise `std::nullopt`.
 */
template <typename Lyt>
[[nodiscard]] std::optional<sidb_simulation_result<Lyt>>
quicksim(const Lyt& lyt, const quicksim_params& ps = quicksim_params{}) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt must be an SiDB layout");
    static_assert(!is_sidb_defect_surface_v<Lyt>,
                  "Lyt cannot be an SiDB defect surface, defects are not supported by the QuickSim algorithm");

    if (ps.iteration_steps == 0)
    {
        return std::nullopt;
    }

    sidb_simulation_result<Lyt> st{};
    st.algorithm_name = "QuickSim";
    st.additional_simulation_parameters.emplace("iteration_steps", ps.iteration_steps);
    st.additional_simulation_parameters.emplace("alpha", ps.alpha);
    st.simulation_parameters = ps.simulation_parameters;
    st.charge_distributions.reserve(ps.iteration_steps);

    if (ps.iteration_steps == 0 || lyt.num_cells() == 0)
    {
        return std::nullopt;
    }

    bool timeout_limit_reached = false;

    mockturtle::stopwatch<>::duration time_counter{};

    // Track the start time for timeout
    const auto start_time = std::chrono::high_resolution_clock::now();

    // measure run time (artificial scope)
    {
        const mockturtle::stopwatch stop{time_counter};

        charge_distribution_surface<Lyt> charge_lyt{lyt};
        charge_lyt.set_sidb_simulation_engine(sidb_simulation_engine::QUICKSIM);

        // set the given physical parameters
        charge_lyt.assign_physical_parameters(ps.simulation_parameters);
        charge_lyt.assign_base_number(2);
        charge_lyt.assign_all_charge_states(sidb_charge_state::NEGATIVE);
        charge_lyt.update_after_charge_change();
        const auto predefined_negative_sidb_indices = charge_lyt.negative_sidb_detection();

        // Check that the layout with all SiDBs negatively charged is physically valid.
        if (charge_lyt.is_physically_valid())
        {
            st.charge_distributions.emplace_back(charge_lyt);
        }

        // Check that the layout with all SiDBs neutrally charged is physically valid.
        charge_lyt.assign_all_charge_states(sidb_charge_state::NEUTRAL);
        charge_lyt.update_after_charge_change();

        if (!predefined_negative_sidb_indices.empty())
        {
            if (charge_lyt.is_physically_valid())
            {
                st.charge_distributions.emplace_back(charge_lyt);
            }
        }

        // Check if the layout where all SiDBs that need to be negatively charged are negatively charged and the rest
        // are neutrally charged is physically valid.
        std::vector<uint64_t> all_sidb_indices_with_unknown_charge_state{};
        all_sidb_indices_with_unknown_charge_state.reserve(charge_lyt.num_cells());

        for (const auto& cell : charge_lyt.get_sidb_order())
        {
            if (std::find(FICTION_EXECUTION_POLICY_PAR_UNSEQ predefined_negative_sidb_indices.cbegin(),
                          predefined_negative_sidb_indices.cend(),
                          charge_lyt.cell_to_index(cell)) == predefined_negative_sidb_indices.cend())
            {
                all_sidb_indices_with_unknown_charge_state.push_back(
                    static_cast<uint64_t>(charge_lyt.cell_to_index(cell)));
            }
        }

        for (const auto& negative_sidb_index : predefined_negative_sidb_indices)
        {
            charge_lyt.assign_charge_state_by_index(negative_sidb_index, sidb_charge_state::NEGATIVE);
        }

        charge_lyt.update_after_charge_change();
        if (charge_lyt.is_physically_valid())
        {
            st.charge_distributions.emplace_back(charge_lyt);
        }

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
                    // if all SiDBs are negatively charged, abort
                    if (predefined_negative_sidb_indices.size() == charge_lyt.num_cells())
                    {
                        return;
                    }

                    auto charge_lyt_copy = charge_distribution_surface{charge_lyt};

                    for (uint64_t l = 0ul; l < iter_per_thread; ++l)
                    {
                        for (const auto& sidb_index_with_unknown_charge_state :
                             all_sidb_indices_with_unknown_charge_state)
                        {
                            // Check if the timeout has been reached before starting the iterations
                            const auto current_time = std::chrono::high_resolution_clock::now();
                            const auto elapsed_time =
                                std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time)
                                    .count();

                            if (static_cast<uint64_t>(elapsed_time) >= ps.timeout)
                            {
                                timeout_limit_reached = true;
                                return;  // Exit the thread if the timeout has been reached
                            }

                            charge_lyt_copy.assign_all_charge_states(sidb_charge_state::NEUTRAL,
                                                                     charge_index_mode::KEEP_CHARGE_INDEX);

                            auto negative_sidbs_indices = predefined_negative_sidb_indices;
                            negative_sidbs_indices.push_back(sidb_index_with_unknown_charge_state);

                            for (const auto& negative_sidb_index : negative_sidbs_indices)
                            {
                                charge_lyt_copy.assign_charge_state_by_index(negative_sidb_index,
                                                                             sidb_charge_state::NEGATIVE,
                                                                             charge_index_mode::KEEP_CHARGE_INDEX);
                            }

                            charge_lyt_copy.update_after_charge_change();

                            if (charge_lyt_copy.is_physically_valid())
                            {
                                charge_lyt_copy.charge_distribution_to_index();

                                const std::lock_guard lock{mutex};
                                st.charge_distributions.emplace_back(charge_lyt_copy);
                            }

                            const auto upper_limit = all_sidb_indices_with_unknown_charge_state.size() - 1;

                            for (uint64_t num = 0ul; num < upper_limit; num++)
                            {
                                charge_lyt_copy.adjacent_search(ps.alpha, negative_sidbs_indices);
                                charge_lyt_copy.validity_check();

                                if (charge_lyt_copy.is_physically_valid())
                                {
                                    charge_lyt_copy.charge_distribution_to_index();

                                    const std::lock_guard lock{mutex};
                                    st.charge_distributions.emplace_back(charge_lyt_copy);
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

    if (timeout_limit_reached || st.charge_distributions.empty())
    {
        return std::nullopt;
    }

    return st;
}

}  // namespace fiction

#endif  // FICTION_QUICKSIM_HPP
