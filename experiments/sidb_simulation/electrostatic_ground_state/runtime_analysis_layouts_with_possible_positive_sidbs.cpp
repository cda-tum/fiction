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
 * @brief Runtime of the SiDB simulation engines where positive charges can occur.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include "fiction_experiments.hpp"

#include <fiction/technology/sidb/generators/random_layout_generator.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/engines/exhaustive_ground_state_simulation.hpp>
#include <fiction/technology/sidb/simulation/engines/quickexact.hpp>
#include <fiction/technology/sidb/simulation/result.hpp>
#include <fiction/types.hpp>

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

    const sidb::model::simulation_parameters sim_params{3, -0.32};

    const sidb::simulation::engines::quickexact_params<cell<Lyt>> qe_params{
        sim_params, sidb::simulation::engines::quickexact_params<cell<Lyt>>::automatic_base_number_detection::OFF};

    auto random_layouts_params = sidb::generators::generate_random_layout_params<cell<Lyt>>{
        {{0, 0}, {10, 10}},
        0,
        sidb::generators::generate_random_layout_params<cell<Lyt>>::positive_charges::MAY_OCCUR,
        sim_params,
        static_cast<uint64_t>(10E6),
        10};

    for (auto num_sidbs = 5u; num_sidbs < 20; num_sidbs++)
    {
        random_layouts_params.number_of_sidbs = num_sidbs;

        const auto random_layouts = sidb::generators::generate_multiple_random_layouts<Lyt>(random_layouts_params);

        double runtime_exhaustive = 0;
        double runtime_quickexact = 0;

        std::vector<std::size_t> number_of_positive_sidbs_of_gs_per_layout{};
        number_of_positive_sidbs_of_gs_per_layout.reserve(random_layouts_params.number_of_unique_generated_layouts);

        if (!random_layouts.has_value())
        {
            continue;
        }

        for (const auto& layout : random_layouts.value())
        {
            const auto exhaustive_results_layout =
                sidb::simulation::engines::exhaustive_ground_state_simulation(layout, sim_params);

            const auto quickexact_results_layout = sidb::simulation::engines::quickexact(layout, qe_params);

            const auto gs = exhaustive_results_layout.groundstates();

            // determine the number of positive SiDBs in the ground state
            number_of_positive_sidbs_of_gs_per_layout.push_back(gs.at(0).num_positive_sidbs());

            runtime_exhaustive += mockturtle::to_seconds(exhaustive_results_layout.simulation_runtime);
            runtime_quickexact += mockturtle::to_seconds(quickexact_results_layout.simulation_runtime);
        }

        const auto average_pos_sibs_of_gs =
            static_cast<double>(std::accumulate(number_of_positive_sidbs_of_gs_per_layout.cbegin(),
                                                number_of_positive_sidbs_of_gs_per_layout.cend(), 0u)) /
            static_cast<double>(number_of_positive_sidbs_of_gs_per_layout.size());

        simulation_exp(random_layouts_params.number_of_sidbs, random_layouts.value().size(), runtime_exhaustive,
                       runtime_quickexact, average_pos_sibs_of_gs);

        simulation_exp.save();
        simulation_exp.table();
    }

    return EXIT_SUCCESS;
}
