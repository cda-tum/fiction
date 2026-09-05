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
 * @brief *QuickTrace* contour tracing against grid search on the Bestagon gates.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include "fiction/synthesis/truth_tables.hpp"
#include "fiction/technology/sidb/io/read_sqd_layout.hpp"
#include "fiction/technology/sidb/io/write_sqd_layout.hpp"
#include "fiction/technology/sidb/model/defect.hpp"
#include "fiction/technology/sidb/simulation/defects/defect_clearance.hpp"
#include "fiction/technology/sidb/simulation/defects/defect_influence.hpp"
#include "fiction/technology/sidb/simulation/io/write_defect_influence_domain.hpp"
#include "fiction/technology/sidb/simulation/logic/is_operational.hpp"
#include "fiction/types.hpp"
#include "fiction_experiments.hpp"

#include <fmt/format.h>

#include <array>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

using namespace fiction;
using namespace fiction::sidb::io;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation::defects;
using namespace fiction::sidb::simulation::io;
using namespace fiction::sidb::simulation::logic;
using namespace fiction::synthesis;

int main()  // NOLINT
{
    experiments::experiment<std::string, std::size_t, int64_t, int64_t, double, std::size_t, double, std::size_t,
                            double, std::size_t>
        simulation_exp{
            "Benchmark",        "Gate Name",        "Num SiDBs",
            "x_coord",          "y_coord",          "distance_grid_search",
            "#samples_grid",    "distance_random",  "#samples_random",
            "distance_contour", "#samples_contour",
    };

    static const std::string bestagon_folder = fmt::format("{}sidb_gate_libraries/bestagon_gates/", EXPERIMENTS_PATH);
    static const std::string plot_folder     = fmt::format("{}quicktrace/plots/", EXPERIMENTS_PATH);

    static const std::array<std::pair<std::string, std::vector<tt>>, 10> gates = {
        std::make_pair("and", std::vector<tt>{create_and_tt()}),
        std::make_pair("nand", std::vector<tt>{create_nand_tt()}),
        std::make_pair("or", std::vector<tt>{create_or_tt()}),
        std::make_pair("nor", std::vector<tt>{create_nor_tt()}),
        std::make_pair("xor", std::vector<tt>{create_xor_tt()}),
        std::make_pair("xnor", std::vector<tt>{create_xnor_tt()}),
        std::make_pair("wire", std::vector<tt>{create_id_tt()}),
        std::make_pair("wire_diag", std::vector<tt>{create_id_tt()}),
        std::make_pair("inv", std::vector<tt>{create_not_tt()}),
        std::make_pair("inv_diag", std::vector<tt>{create_not_tt()})};

    const is_operational_params is_op_params{.sim_params = simulation_parameters{2, -0.32, 5.6, 5.0}};

    // for this experiment we use a stray SiDB defect
    const auto stray_db = defect{defect_type::DB, -1, 4.1, 1.8};
    // const auto si_vacancy = sidb_defect{sidb_defect_type::SI_VACANCY, -1, 10.6, 5.9};

    defect_influence_params params{};
    params.additional_scanning_area = {100, 100};
    params.defect                   = stray_db;
    params.operational_params       = is_op_params;

    std::size_t total_number_of_samples_grid       = 0;
    std::size_t total_number_of_samples_quicktrace = 0;

    for (const auto& [gate, truth_table] : gates)
    {
        // Create a folder where the defect influence plots are stored
        std::filesystem::create_directories(plot_folder);

        // Create a folder where the defect influence CSV files of each gate are stored
        const auto plot_folder_for_given_gate = fmt::format("{}{}/", plot_folder, gate);
        std::filesystem::create_directories(plot_folder_for_given_gate);

        // read the Bestagon SiDB layout
        const auto lyt = read_sqd_layout(fmt::format("{}{}.sqd", bestagon_folder, gate));

        // Write the SQD layout
        write_sqd_layout(lyt, fmt::format("{}/{}.sqd", plot_folder_for_given_gate, gate));

        // grid search
        defect_influence_stats grid_stats{};
        const auto             defect_inf_grid = defect_influence_grid_search(lyt, truth_table, params, 1, &grid_stats);
        total_number_of_samples_grid += grid_stats.num_evaluated_defect_positions;

        // Write the defect influence domain to a CSV file
        write_defect_influence_domain(defect_inf_grid, fmt::format("{}{}_grid.csv", plot_folder_for_given_gate, gate));
        const auto clearance_grid_search = calculate_defect_clearance(lyt, defect_inf_grid);

        // random sampling
        defect_influence_stats random_stats{};
        const auto defect_inf_random = defect_influence_random_sampling(lyt, truth_table, 100, params, &random_stats);
        const auto clearance_random  = calculate_defect_clearance(lyt, defect_inf_random);
        write_defect_influence_domain(defect_inf_random,
                                      fmt::format("{}{}_random.csv", plot_folder_for_given_gate, gate));

        // quicktrace
        defect_influence_stats quicktrace_stats{};
        const auto defect_inf_quicktrace = defect_influence_quicktrace(lyt, truth_table, 20, params, &quicktrace_stats);
        total_number_of_samples_quicktrace += quicktrace_stats.num_evaluated_defect_positions;
        const auto clearance_quicktrace = calculate_defect_clearance(lyt, defect_inf_quicktrace);
        write_defect_influence_domain(defect_inf_quicktrace,
                                      fmt::format("{}{}_quicktrace.csv", plot_folder_for_given_gate, gate));

        // Log the simulation results
        simulation_exp(gate, lyt.num_cells(), clearance_grid_search.defect_position.x,
                       row_of(clearance_grid_search.defect_position), clearance_grid_search.defect_clearance_distance,
                       grid_stats.num_evaluated_defect_positions, clearance_random.defect_clearance_distance,
                       random_stats.num_evaluated_defect_positions, clearance_quicktrace.defect_clearance_distance,
                       quicktrace_stats.num_evaluated_defect_positions);
        simulation_exp.save();
        simulation_exp.table();
    }

    // Log the total number of samples
    simulation_exp("Total", 0, 0.0, 0.0, 0.0, total_number_of_samples_grid, 0.0, 0, 0.0,
                   total_number_of_samples_quicktrace);
    simulation_exp.save();
    simulation_exp.table();

    return EXIT_SUCCESS;
}
