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
 * @brief *QuickSim*, a heuristic SiDB ground state simulation based on max-min diversity.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 * @author Willem Lambooy (wlambooy)
 */

#pragma once

#include "fiction/technology/sidb/cell_level_layout_conversion.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/model/charge_state.hpp"
#include "fiction/technology/sidb/model/simulation_parameters.hpp"
#include "fiction/technology/sidb/simulation/detail/simulation_state.hpp"
#include "fiction/technology/sidb/simulation/potential_landscape.hpp"
#include "fiction/technology/sidb/simulation/result.hpp"
#include "fiction/traits.hpp"

#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <mutex>
#include <optional>
#include <thread>
#include <utility>
#include <vector>

namespace fiction::sidb::simulation::engines
{

/**
 * This struct stores the parameters for the *QuickSim* algorithm.
 */
struct quicksim_params
{
    /**
     * Simulation parameters for the simulation of the physical SiDB system.
     */
    sidb::model::simulation_parameters sim_params{};
    /**
     * Number of iterations to run the simulation for.
     */
    uint64_t iteration_steps{80};
    /**
     * `alpha` parameter for the *QuickSim* algorithm.
     */
    double alpha{0.7};
    /**
     * Number of threads to spawn. By default the number of threads is set to the number of available hardware threads.
     */
    uint64_t number_threads{std::thread::hardware_concurrency()};
    /**
     * Timeout limit (in ms).
     */
    uint64_t timeout = std::numeric_limits<uint64_t>::max();
};

/**
 * *QuickSim* is a heuristic ground-state simulation of SiDB layouts proposed in \"QuickSim: Efficient and Accurate
 * Physical Simulation of Silicon Dangling Bond Logic\" by J. Drewniok, M. Walter, S. S. H. Ng, K. Walus, and R. Wille
 * in IEEE NANO 2023 (https://ieeexplore.ieee.org/document/10231266). It grows candidate charge distributions from the
 * SiDBs that have to be negative by repeatedly charging the neutral SiDB farthest from every negative one, keeping
 * the physically valid intermediate states. Every worker thread owns one simulation state and shares the layout's
 * potential landscape. Charged surface defects are not supported: a layout that holds any yields `std::nullopt`.
 *
 * @param lyt Layout to simulate.
 * @param ps *QuickSim* parameters.
 * @return The physically valid charge distributions found, or `std::nullopt` if the layout is empty, holds charged
 * defects, the iteration count is zero, the timeout was hit, or no valid distribution was found.
 */
[[nodiscard]] inline std::optional<result> quicksim(const layout&          lyt,
                                                    const quicksim_params& ps = quicksim_params{}) noexcept
{
    if (ps.iteration_steps == 0 || lyt.num_cells() == 0 || lyt.num_charged_defects() > 0)
    {
        return std::nullopt;
    }

    result st{};
    st.algorithm_name = "QuickSim";
    st.additional_simulation_parameters.emplace("iteration_steps", ps.iteration_steps);
    st.additional_simulation_parameters.emplace("alpha", ps.alpha);
    st.sim_params = ps.sim_params;
    st.lyt        = lyt;
    st.charge_distributions.reserve(ps.iteration_steps);

    // written by every worker thread, read after they have all joined
    std::atomic_bool timeout_limit_reached{false};

    mockturtle::stopwatch<>::duration time_counter{};

    // Track the start time for timeout
    const auto start_time = std::chrono::high_resolution_clock::now();

    // measure run time (artificial scope)
    {
        const mockturtle::stopwatch stop{time_counter};

        auto params = ps.sim_params;
        params.base = 2;

        const potential_landscape            land{lyt, params};
        simulation::detail::simulation_state state{land, model::charge_state::NEGATIVE,
                                                   simulation::detail::simulation_state::energy_model::INTERNAL_ONLY};

        const auto predefined_negative_sidb_indices = state.negative_sidb_detection();

        // Check that the layout with all SiDBs negatively charged is physically valid.
        if (state.is_physically_valid())
        {
            st.charge_distributions.push_back(state.snapshot());
        }

        // Check that the layout with all SiDBs neutrally charged is physically valid.
        state.assign_all_charge_states(model::charge_state::NEUTRAL);
        state.update_after_charge_change();

        if (!predefined_negative_sidb_indices.empty() && state.is_physically_valid())
        {
            st.charge_distributions.push_back(state.snapshot());
        }

        // Check if the layout where all SiDBs that need to be negatively charged are negatively charged and the rest
        // are neutrally charged is physically valid.
        std::vector<std::size_t> all_sidb_indices_with_unknown_charge_state{};
        all_sidb_indices_with_unknown_charge_state.reserve(state.num_sidbs());

        for (std::size_t i = 0; i < state.num_sidbs(); ++i)
        {
            // no execution policy: predefined_negative_sidb_indices holds a handful of entries, where the dispatch
            // costs an order of magnitude more than the search itself
            if (std::ranges::find(predefined_negative_sidb_indices, i) == predefined_negative_sidb_indices.cend())
            {
                all_sidb_indices_with_unknown_charge_state.push_back(i);
            }
        }

        for (const auto negative_sidb_index : predefined_negative_sidb_indices)
        {
            state.assign_charge_state_by_index(negative_sidb_index, model::charge_state::NEGATIVE);
        }

        state.update_after_charge_change();
        if (state.is_physically_valid())
        {
            st.charge_distributions.push_back(state.snapshot());
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
                    if (predefined_negative_sidb_indices.size() == state.num_sidbs())
                    {
                        return;
                    }

                    auto worker_state = state;
                    worker_state.reseed();

                    for (uint64_t l = 0ul; l < iter_per_thread; ++l)
                    {
                        for (const auto sidb_index_with_unknown_charge_state :
                             all_sidb_indices_with_unknown_charge_state)
                        {
                            // Check if the timeout has been reached before starting the iterations
                            const auto current_time = std::chrono::high_resolution_clock::now();
                            const auto elapsed_time =
                                std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time)
                                    .count();

                            if (std::cmp_greater_equal(elapsed_time, ps.timeout))
                            {
                                // relaxed: join establishes the happens-before edge to the read below
                                timeout_limit_reached.store(true, std::memory_order_relaxed);
                                return;  // Exit the thread if the timeout has been reached
                            }

                            worker_state.assign_all_charge_states(
                                model::charge_state::NEUTRAL, simulation::detail::charge_index_mode::KEEP_CHARGE_INDEX);

                            auto negative_sidbs_indices = predefined_negative_sidb_indices;
                            negative_sidbs_indices.push_back(sidb_index_with_unknown_charge_state);

                            for (const auto negative_sidb_index : negative_sidbs_indices)
                            {
                                worker_state.assign_charge_state_by_index(
                                    negative_sidb_index, model::charge_state::NEGATIVE,
                                    simulation::detail::charge_index_mode::KEEP_CHARGE_INDEX);
                            }

                            worker_state.update_after_charge_change();

                            if (worker_state.is_physically_valid())
                            {
                                const std::scoped_lock lock{mutex};
                                st.charge_distributions.push_back(worker_state.snapshot());
                            }

                            const auto upper_limit = all_sidb_indices_with_unknown_charge_state.size() - 1;

                            for (uint64_t num = 0ul; num < upper_limit; num++)
                            {
                                worker_state.adjacent_search(ps.alpha, negative_sidbs_indices);
                                worker_state.validity_check();

                                if (worker_state.is_physically_valid())
                                {
                                    const std::scoped_lock lock{mutex};
                                    st.charge_distributions.push_back(worker_state.snapshot());
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

    if (timeout_limit_reached.load(std::memory_order_relaxed) || st.charge_distributions.empty())
    {
        return std::nullopt;
    }

    return st;
}

/**
 * *QuickSim* on a Cartesian SiDB cell-level layout: the layout is converted with `to_sidb_layout`, simulated, and
 * the result converted back with `to_legacy_result`. This overload serves the algorithms that still consume
 * `legacy_result`.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt Layout to simulate.
 * @param ps *QuickSim* parameters.
 * @return Simulation result over surfaces of `lyt`, or `std::nullopt` as the `layout` overload returns it.
 */
template <typename Lyt>
    requires(is_cell_level_layout_v<Lyt>)
[[nodiscard]] std::optional<legacy_result<Lyt>> quicksim(const Lyt&             lyt,
                                                         const quicksim_params& ps = quicksim_params{}) noexcept
{
    if (const auto res = quicksim(to_sidb_layout(lyt), ps); res.has_value())
    {
        return to_legacy_result(*res, lyt);
    }

    return std::nullopt;
}

}  // namespace fiction::sidb::simulation::engines
