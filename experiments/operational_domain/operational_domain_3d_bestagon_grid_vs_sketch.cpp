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
 * @brief 3D operational domains of the Bestagon gates: grid search against sketching.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include "fiction_experiments.hpp"  // experiment class

#include <fiction/synthesis/truth_tables.hpp>                                  // truth tables helper functions
#include <fiction/technology/sidb/io/read_sqd_layout.hpp>                      // reader for SiDB layouts
#include <fiction/technology/sidb/model/simulation_parameters.hpp>             // SiDB simulation parameters
#include <fiction/technology/sidb/simulation/engine.hpp>                       // SiDB simulation engines
#include <fiction/technology/sidb/simulation/io/write_operational_domain.hpp>  // writer for operational domains
#include <fiction/technology/sidb/simulation/logic/operational_domain.hpp>  // operational domain computation algorithms
#include <fiction/types.hpp>  // pre-defined types suitable for the FCN domain

#include <fmt/format.h>                    // output formatting
#include <mockturtle/utils/stopwatch.hpp>  // stopwatch for time measurement

#include <array>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

using namespace fiction;

// This script is the three-dimensional counterpart of `operational_domain_bestagon_grid_vs_sketch`. It compares the
// operational domain computation of Bestagon gates using grid search with the "Operational Domain Sketch", which
// identifies non-operational parameter points through filtering strategies and assumes the remaining points to be
// operational, which can lead to false positives.
//
// The third sweep dimension is `MU_MINUS`, the only remaining sweep parameter. Since the number of parameter points
// grows with the product of the step counts, the two-dimensional experiment's step size of 0.05 would put the
// simulated baseline out of reach here; 0.2 keeps it tractable while covering the same ranges.
//
// The sketch is also run through flood fill. Flood fill follows the boundary of the sketch region rather than of the
// operational region, and its frontier only stops where a filter proves non-operationality. Where the sketch region
// fills the swept range, it therefore visits every point and costs more than the grid search it is meant to undercut.
// The last two columns measure whether that happens for these gates.

int main()  // NOLINT
{
    experiments::experiment<std::string, uint64_t, uint64_t, double, uint64_t, double, uint64_t, double, double, double>
        opdomain_exp{"3D Operational Domain Sketch",
                     "Name",
                     "#SiDBs",  // Benchmark
                     "num op (grid search)",
                     "t in s (grid search)",
                     "num op (sketch)",
                     "t in s (sketch)",
                     "num op (sketch, flood fill)",
                     "t in s (sketch, flood fill)",
                     "num op (sketch) / num op (grid search)",
                     "t in s (grid search) / t in s (sketch)"};

    // simulation parameters
    sidb::model::simulation_parameters sim_params{};
    sim_params.base     = 2;
    sim_params.mu_minus = -0.32;

    // operational domain parameters
    sidb::simulation::logic::operational_domain_params op_domain_params{};
    op_domain_params.operational_params.sim_params = sim_params;
    op_domain_params.operational_params.sim_engine = sidb::simulation::engine::QUICKEXACT;

    // the sketch's filtering steps are only defined when kinks are rejected
    op_domain_params.operational_params.op_condition =
        sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS;

    op_domain_params.sweep_dimensions = {{sidb::simulation::logic::sweep_parameter::EPSILON_R},
                                         {sidb::simulation::logic::sweep_parameter::LAMBDA_TF},
                                         {sidb::simulation::logic::sweep_parameter::MU_MINUS}};

    op_domain_params.sweep_dimensions[0].min  = 1.0;
    op_domain_params.sweep_dimensions[0].max  = 10.0;
    op_domain_params.sweep_dimensions[0].step = 0.2;
    op_domain_params.sweep_dimensions[1].min  = 1.0;
    op_domain_params.sweep_dimensions[1].max  = 10.0;
    op_domain_params.sweep_dimensions[1].step = 0.2;
    op_domain_params.sweep_dimensions[2].min  = -0.50;
    op_domain_params.sweep_dimensions[2].max  = -0.10;
    op_domain_params.sweep_dimensions[2].step = 0.05;

    static const std::string folder = fmt::format("{}sidb_gate_libraries/bestagon_gates/", EXPERIMENTS_PATH);

    const auto truth_tables_and_names = std::array<std::pair<std::vector<tt>, std::string>, 14>{
        {{std::vector<tt>{synthesis::create_id_tt()}, "wire"},
         {std::vector<tt>{synthesis::create_id_tt()}, "wire_diag"},
         {std::vector<tt>{synthesis::create_not_tt()}, "inv"},
         {std::vector<tt>{synthesis::create_not_tt()}, "inv_diag"},
         {std::vector<tt>{synthesis::create_and_tt()}, "and"},
         {std::vector<tt>{synthesis::create_nand_tt()}, "nand"},
         {std::vector<tt>{synthesis::create_or_tt()}, "or"},
         {std::vector<tt>{synthesis::create_nor_tt()}, "nor"},
         {std::vector<tt>{synthesis::create_xor_tt()}, "xor"},
         {std::vector<tt>{synthesis::create_xnor_tt()}, "xnor"},
         {synthesis::create_fan_out_tt(), "fo2"},
         {synthesis::create_crossing_wire_tt(), "cx"},
         {synthesis::create_half_adder_tt(), "ha"},
         {synthesis::create_double_wire_tt(), "hourglass"}}};

    double mean_ratio_num_op_sketch_to_num_op_grid_search = 0.0;
    double total_runtime_grid_search                      = 0.0;
    double total_runtime_sketch                           = 0.0;
    double total_runtime_sketch_flood_fill                = 0.0;

    for (const auto& [truth_table, gate] : truth_tables_and_names)
    {
        const auto lyt =
            sidb::io::read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(fmt::format("{}/{}.sqd", folder, gate), gate);

        // operational domain stats
        sidb::simulation::logic::operational_domain_stats op_domain_stats_grid_search{};
        sidb::simulation::logic::operational_domain_stats op_domain_stats_sketch{};
        sidb::simulation::logic::operational_domain_stats op_domain_stats_sketch_flood_fill{};

        op_domain_params.operational_params.strategy_to_analyze_operational_status =
            sidb::simulation::logic::is_operational_params::operational_analysis_strategy::SIMULATION_ONLY;

        const auto op_domain_grid_search = sidb::simulation::logic::operational_domain_grid_search(
            lyt, truth_table, op_domain_params, &op_domain_stats_grid_search);

        total_runtime_grid_search += mockturtle::to_seconds(op_domain_stats_grid_search.time_total);

        op_domain_params.operational_params.strategy_to_analyze_operational_status =
            sidb::simulation::logic::is_operational_params::operational_analysis_strategy::FILTER_ONLY;

        const auto op_domain_sketch = sidb::simulation::logic::operational_domain_grid_search(
            lyt, truth_table, op_domain_params, &op_domain_stats_sketch);

        total_runtime_sketch += mockturtle::to_seconds(op_domain_stats_sketch.time_total);

        const auto op_domain_sketch_flood_fill = sidb::simulation::logic::operational_domain_flood_fill(
            lyt, truth_table, 100, op_domain_params, &op_domain_stats_sketch_flood_fill);

        total_runtime_sketch_flood_fill += mockturtle::to_seconds(op_domain_stats_sketch_flood_fill.time_total);

        sidb::simulation::io::write_operational_domain(op_domain_grid_search,
                                                       fmt::format("{}/3d_grid_search_{}.csv", folder, gate));
        sidb::simulation::io::write_operational_domain(op_domain_sketch,
                                                       fmt::format("{}/3d_sketch_{}.csv", folder, gate));
        sidb::simulation::io::write_operational_domain(op_domain_sketch_flood_fill,
                                                       fmt::format("{}/3d_sketch_flood_fill_{}.csv", folder, gate));

        // a gate without a single operational point in the swept range would turn the ratio into a NaN, which would
        // then propagate into the mean and corrupt the "Total" row for every other gate
        const auto num_op_grid_search = op_domain_stats_grid_search.num_operational_parameter_combinations;

        const auto ratio_num_op_sketch_to_num_op_grid_search =
            num_op_grid_search == 0 ?
                0.0 :
                static_cast<double>(op_domain_stats_sketch.num_operational_parameter_combinations) /
                    static_cast<double>(num_op_grid_search);

        mean_ratio_num_op_sketch_to_num_op_grid_search += ratio_num_op_sketch_to_num_op_grid_search;

        opdomain_exp(
            // Benchmark
            gate, lyt.num_cells(),

            // Operational Domain (determine the operation status by simulation)
            op_domain_stats_grid_search.num_operational_parameter_combinations,
            mockturtle::to_seconds(op_domain_stats_grid_search.time_total),

            // Operational Domain Sketch (determine the operation status by pruning)
            op_domain_stats_sketch.num_operational_parameter_combinations,
            mockturtle::to_seconds(op_domain_stats_sketch.time_total),

            // Operational Domain Sketch, sampled by flood fill instead of exhaustively
            op_domain_stats_sketch_flood_fill.num_operational_parameter_combinations,
            mockturtle::to_seconds(op_domain_stats_sketch_flood_fill.time_total),

            ratio_num_op_sketch_to_num_op_grid_search,
            mockturtle::to_seconds(op_domain_stats_grid_search.time_total) /
                mockturtle::to_seconds(op_domain_stats_sketch.time_total));

        opdomain_exp.save();
        opdomain_exp.table();
    }

    opdomain_exp("Total", 0, 0, total_runtime_grid_search, 0, total_runtime_sketch, 0, total_runtime_sketch_flood_fill,
                 mean_ratio_num_op_sketch_to_num_op_grid_search / truth_tables_and_names.size(), 0.0);

    opdomain_exp.save();
    opdomain_exp.table();

    return EXIT_SUCCESS;
}
