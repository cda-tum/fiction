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
 * @brief Runtime of the SiDB simulation engines on the Bestagon gates.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include "fiction_experiments.hpp"

#include <fiction/synthesis/truth_tables.hpp>
#include <fiction/technology/sidb/io/read_sqd_layout.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/analysis/time_to_solution.hpp>
#include <fiction/technology/sidb/simulation/engines/exhaustive_ground_state_simulation.hpp>
#include <fiction/technology/sidb/simulation/engines/quickexact.hpp>
#include <fiction/technology/sidb/simulation/engines/quicksim.hpp>
#include <fiction/technology/sidb/simulation/logic/bdl_input_iterator.hpp>
#include <fiction/types.hpp>

#include <fmt/format.h>
#include <mockturtle/utils/stopwatch.hpp>

#include <array>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::sidb::io;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation::analysis;
using namespace fiction::sidb::simulation::engines;
using namespace fiction::sidb::simulation::logic;
using namespace fiction::synthesis;

int main()  // NOLINT
{
    experiments::experiment<std::string, std::size_t, double, double, double, double, double> simulation_exp{
        "Benchmark",
        "Gate Name",
        "#Instances",
        "Exhaustive Runtime [s]",
        "QuickExact Runtime [s]",
        "Average QuickSim Accuracy",
        "QuickSim Single Runtime [s]",
        "Total QuickSim TTS [s]"};

    static const std::string folder = fmt::format("{}sidb_gate_libraries/bestagon_gates/", EXPERIMENTS_PATH);

    static const std::array<std::pair<std::string, std::vector<tt>>, 12> gates = {
        std::make_pair("hourglass", create_crossing_wire_tt()),
        std::make_pair("cx", create_crossing_wire_tt()),
        std::make_pair("ha", create_half_adder_tt()),
        std::make_pair("and", std::vector<tt>{create_and_tt()}),
        std::make_pair("xor", std::vector<tt>{create_xor_tt()}),
        std::make_pair("or", std::vector<tt>{create_or_tt()}),
        std::make_pair("xnor", std::vector<tt>{create_xnor_tt()}),
        std::make_pair("fo2", std::vector<tt>{create_fan_out_tt()}),
        std::make_pair("nor", std::vector<tt>{create_nor_tt()}),
        std::make_pair("nand", std::vector<tt>{create_nand_tt()}),
        std::make_pair("inv", std::vector<tt>{create_not_tt()}),
        std::make_pair("wire", std::vector<tt>{create_id_tt()})};

    const simulation_parameters   sim_params{2, -0.32};
    const quicksim_params         qs_params{.sim_params = sim_params};
    const quickexact_params       qe_params{.sim_params = sim_params};
    const time_to_solution_params tts_params{};

    double      total_runtime_exhaustive      = 0.0;
    double      total_runtime_quickexact      = 0.0;
    double      average_accuracy_quicksim     = 0.0;
    double      total_single_runtime_quicksim = 0.0;
    double      total_tts_quicksim            = 0.0;
    std::size_t total_number_of_instances     = 0;

    for (const auto& [gate, truth_table] : gates)
    {
        const auto layout = read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(fmt::format("{}{}.sqd", folder, gate));

        double      runtime_exhaustive      = 0;
        double      runtime_quickexact      = 0;
        double      tts_quicksim            = 0;
        std::size_t instances               = 0;
        double      quicksim_accuracy_mean  = 0.0;
        double      quicksim_single_runtime = 0.0;

        // simulate layout with no input pattern
        const auto             exhaustive_results_layout = exhaustive_ground_state_simulation(layout, sim_params);
        time_to_solution_stats stats{};
        time_to_solution(layout, qs_params, tts_params, &stats);
        const auto quickexact_results_layout = quickexact(layout, qe_params);

        runtime_exhaustive += mockturtle::to_seconds(exhaustive_results_layout.simulation_runtime);
        runtime_quickexact += mockturtle::to_seconds(quickexact_results_layout.simulation_runtime);
        tts_quicksim += stats.time_to_solution;
        instances += 1;
        total_number_of_instances += 1;
        quicksim_accuracy_mean += stats.acc;
        quicksim_single_runtime += stats.mean_single_runtime;

        // simulate layout with all input patterns
        auto       bii                = legacy_bdl_input_iterator<sidb_100_cell_clk_lyt_siqad>{layout};
        const auto num_input_patterns = truth_table.front().num_bits();

        for (auto i = 0u; i < num_input_patterns; ++i, ++bii)
        {
            const auto             exhaustive_results = exhaustive_ground_state_simulation(*bii, sim_params);
            time_to_solution_stats tts_stats{};
            time_to_solution(*bii, qs_params, tts_params, &tts_stats);
            const auto quickexact_results = quickexact(*bii, qe_params);

            runtime_exhaustive += mockturtle::to_seconds(exhaustive_results.simulation_runtime);
            runtime_quickexact += mockturtle::to_seconds(quickexact_results.simulation_runtime);
            tts_quicksim += stats.time_to_solution;

            instances += 1;
            total_number_of_instances += 1;
            quicksim_accuracy_mean += stats.acc;
            quicksim_single_runtime += stats.mean_single_runtime;
        }
        quicksim_accuracy_mean = quicksim_accuracy_mean / static_cast<double>(instances);

        total_runtime_exhaustive += runtime_exhaustive;
        total_runtime_quickexact += runtime_quickexact;
        average_accuracy_quicksim += quicksim_accuracy_mean;
        total_single_runtime_quicksim += quicksim_single_runtime;
        total_tts_quicksim += tts_quicksim;

        simulation_exp(gate, instances, runtime_exhaustive, runtime_quickexact, quicksim_accuracy_mean,
                       quicksim_single_runtime, tts_quicksim);
        simulation_exp.save();
        simulation_exp.table();
    }

    simulation_exp("Total", total_number_of_instances, total_runtime_exhaustive, total_runtime_quickexact,
                   average_accuracy_quicksim / gates.size(), total_single_runtime_quicksim, total_tts_quicksim);
    simulation_exp.save();
    simulation_exp.table();

    return EXIT_SUCCESS;
}
