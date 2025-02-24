//
// Created by Jan Drewniok on 02.09.24.
//

#include "fiction_experiments.hpp"

#include <fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp>
#include <fiction/algorithms/simulation/sidb/groundstate_from_simulation_result.hpp>
#include <fiction/algorithms/simulation/sidb/quickexact.hpp>
#include <fiction/algorithms/simulation/sidb/random_sidb_layout_generator.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>

#include <mockturtle/utils/stopwatch.hpp>

#include <cstdint>
#include <cstdlib>
#include <numeric>
#include <vector>

using namespace fiction;

// This script compares the simulation runtime of ExGS and QuickExact for randomly
// generated layouts that may contain positively charged SiDBs (`can_positive_charges_occur` returns `true`). The number
// of SiDBs in the layout varies from 5 to 20.

int main()  // NOLINT
{
    using Lyt = sidb_100_cell_clk_lyt;

    experiments::experiment<std::size_t, std::size_t, double, double, double> simulation_exp{
        "Benchmark",
        "Number SiDBs",
        "#Instances",
        "Exhaustive Runtime [s]",
        "QuickExact Runtime [s]",
        "Average #positive SiDBs of ground state"};

    const sidb_simulation_parameters sim_params{3, -0.32};

    const quickexact_params<cell<Lyt>> qe_params{sim_params,
                                                 quickexact_params<cell<Lyt>>::automatic_base_number_detection::OFF};

    auto random_layouts_params = generate_random_sidb_layout_params<cell<Lyt>>{
        {{0, 0}, {10, 10}},
        0,
        generate_random_sidb_layout_params<cell<Lyt>>::positive_charges::MAY_OCCUR,
        sim_params,
        static_cast<uint64_t>(10E6),
        10};

    for (auto num_sidbs = 5u; num_sidbs < 20; num_sidbs++)
    {
        random_layouts_params.number_of_sidbs = num_sidbs;

        const auto random_layouts = generate_multiple_random_sidb_layouts<Lyt>(random_layouts_params);

        double runtime_exhaustive = 0;
        double runtime_quickexact = 0;

        std::vector<std::size_t> number_of_positive_sidbs_of_gs_per_layout{};
        number_of_positive_sidbs_of_gs_per_layout.reserve(random_layouts_params.number_of_unique_generated_layouts);

        for (const auto& layout : random_layouts)
        {
            const auto exhaustive_results_layout = exhaustive_ground_state_simulation(layout, sim_params);

            const auto quickexact_results_layout = quickexact(layout, qe_params);

            const auto gs = groundstate_from_simulation_result(exhaustive_results_layout);

            // determine the number of positive SiDBs in the ground state
            number_of_positive_sidbs_of_gs_per_layout.push_back(gs.at(0).num_positive_sidbs());

            runtime_exhaustive += mockturtle::to_seconds(exhaustive_results_layout.simulation_runtime);
            runtime_quickexact += mockturtle::to_seconds(quickexact_results_layout.simulation_runtime);
        }

        const auto average_pos_sibs_of_gs =
            static_cast<double>(std::accumulate(number_of_positive_sidbs_of_gs_per_layout.cbegin(),
                                                number_of_positive_sidbs_of_gs_per_layout.cend(), 0u)) /
            static_cast<double>(number_of_positive_sidbs_of_gs_per_layout.size());

        simulation_exp(random_layouts_params.number_of_sidbs, random_layouts.size(), runtime_exhaustive,
                       runtime_quickexact, average_pos_sibs_of_gs);

        simulation_exp.save();
        simulation_exp.table();
    }

    return EXIT_SUCCESS;
}
