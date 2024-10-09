//
// Created by Jan Drewniok on 03.03.24.
//

#include <fiction/algorithms/simulation/sidb/check_simulation_results_for_equivalence.hpp>
#include <fiction/algorithms/simulation/sidb/clustercomplete.hpp>
#include <fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp>
#include <fiction/algorithms/simulation/sidb/quickexact.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/layout_utils.hpp>
#include <fiction/utils/math_utils.hpp>

#include <fmt/format.h>

#include <cstdint>
#include <iostream>
#include <mutex>
#include <thread>

using namespace fiction;

// This script verifies the equivalence between ExGS and QuickExact. It generates all layouts consisting of
// 4 SiDBs within an 11x11 spanned area. The simulation is then executed using both simulators, and the results
// are compared.

int main()  // NOLINT
{
    const auto all_cells_in_region = all_coordinates_in_spanned_area<offset::ucoord_t>({0, 0}, {10, 10});

    const auto all_distributions =
        determine_all_combinations_of_distributing_k_entities_on_n_positions(4, all_cells_in_region.size());

    const auto params = sidb_simulation_parameters{3, -0.32};

    uint64_t quickexact_non_equivalence_counter      = 0;
    uint64_t clustercomplete_non_equivalence_counter = 0;

    const uint64_t num_threads_to_use =
        std::min(std::max(uint64_t{std::thread::hardware_concurrency()}, uint64_t{1}), all_distributions.size());

    // define the top cluster charge space ranges per thread
    std::vector<std::pair<uint64_t, uint64_t>> ranges{};
    ranges.reserve(num_threads_to_use);

    const uint64_t chunk_size = std::max(all_distributions.size() / num_threads_to_use, uint64_t{1});
    uint64_t       start      = 0;
    uint64_t       end        = chunk_size - 1;

    for (uint64_t i = 0; i < num_threads_to_use; ++i)
    {
        ranges.emplace_back(start, end);
        start = end + 1;
        end   = i == num_threads_to_use - 2 ? all_distributions.size() - 1 : start + chunk_size - 1;
    }

    std::vector<std::thread> threads{};
    threads.reserve(num_threads_to_use);

    std::mutex mutex_qe{};
    std::mutex mutex_cc{};

    for (const auto& [range_start, range_end] : ranges)
    {
        threads.emplace_back(
            [&, start = range_start, end = range_end]
            {
                for (uint64_t ix = start; ix <= end; ++ix)
                {
                    sidb_100_cell_clk_lyt lyt{};

                    for (const auto idx : *std::next(all_distributions.cbegin(), static_cast<int64_t>(ix)))
                    {
                        lyt.assign_cell_type(all_cells_in_region[idx], sidb_technology::cell_type::NORMAL);
                    }

                    clustercomplete_params<fiction::cell<sidb_100_cell_clk_lyt>> cc_params{params};
                    cc_params.available_threads = 1;

                    auto result_exgs = exhaustive_ground_state_simulation(lyt, params);
                    auto result_quickexact =
                        quickexact(lyt, quickexact_params<fiction::cell<sidb_100_cell_clk_lyt>>{params});
                    auto result_clustercomplete = clustercomplete(lyt, cc_params);

                    if (!check_simulation_results_for_equivalence(result_exgs, result_quickexact))
                    {
                        std::lock_guard lock{mutex_qe};
                        quickexact_non_equivalence_counter++;
                    }

                    if (!check_simulation_results_for_equivalence(result_exgs, result_clustercomplete))
                    {
                        std::lock_guard lock{mutex_cc};
                        clustercomplete_non_equivalence_counter++;
                    }
                }
            });
    }

    // wait for all threads to complete
    for (auto& thread : threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }

    std::cout << fmt::format("QuickExact non equivalent layouts = {}\nClusterComplete non equivalent layouts = {}\n",
                             quickexact_non_equivalence_counter, clustercomplete_non_equivalence_counter);

    return EXIT_SUCCESS;
}
