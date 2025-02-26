//
// Created by Jan Drewniok on 15.12.24.
//

#include "fiction_experiments.hpp"  // experiment class

#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>      // operational domain computation algorithms
#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>  // SiDB simulation engines
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>  // SiDB simulation parameters
#include <fiction/io/read_sqd_layout.hpp>                                     // reader for SiDB layouts
#include <fiction/io/write_operational_domain.hpp>                            // writer for operational domains
#include <fiction/types.hpp>                    // pre-defined types suitable for the FCN domain
#include <fiction/utils/truth_table_utils.hpp>  // truth tables helper functions

#include <fmt/format.h>                    // output formatting
#include <mockturtle/utils/stopwatch.hpp>  // stopwatch for time measurement

#include <array>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

using namespace fiction;

// This script compares the operational domain computation of Bestagon gates using grid search with an approximate
// algorithm that computes the so-called "Operational Domain Sketch". The approximate algorithm identifies
// non-operational parameter points through filtering strategies. For the remaining parameter points, it assumes they
// are operational, which can lead to false positives.

int main()  // NOLINT
{
    experiments::experiment<std::string, uint64_t, uint64_t, double, uint64_t, double, double, double> opdomain_exp{
        "Operational Domain vs Sketch (Bestagon)",
        "Name",
        "#SiDBs",  // Benchmark
        "num op (grid)",
        "t in s (grid)",
        "num op sketch",
        "t in s (sketch)",
        "num op (sketch) / num op (grid)",
        "t in s (grid) / t in s (sketch)"};

    // simulation parameters
    sidb_simulation_parameters sim_params{};
    sim_params.base     = 2;
    sim_params.mu_minus = -0.32;

    // operational domain parameters
    operational_domain_params op_domain_params{};
    op_domain_params.operational_params.simulation_parameters = sim_params;
    op_domain_params.operational_params.sim_engine            = sidb_simulation_engine::QUICKEXACT;

    op_domain_params.operational_params.op_condition = is_operational_params::operational_condition::REJECT_KINKS;

    op_domain_params.sweep_dimensions         = {{sweep_parameter::EPSILON_R}, {sweep_parameter::LAMBDA_TF}};
    op_domain_params.sweep_dimensions[0].min  = 1.0;
    op_domain_params.sweep_dimensions[0].max  = 10.0;
    op_domain_params.sweep_dimensions[0].step = 0.05;
    op_domain_params.sweep_dimensions[1].min  = 1.0;
    op_domain_params.sweep_dimensions[1].max  = 10.0;
    op_domain_params.sweep_dimensions[1].step = 0.05;

    static const std::string folder = fmt::format("{}sidb_gate_libraries/bestagon_gates/", EXPERIMENTS_PATH);

    const auto truth_tables_and_names =
        std::array<std::pair<std::vector<tt>, std::string>, 14>{{{std::vector<tt>{create_id_tt()}, "wire"},
                                                                 {std::vector<tt>{create_id_tt()}, "wire_diag"},
                                                                 {std::vector<tt>{create_not_tt()}, "inv"},
                                                                 {std::vector<tt>{create_not_tt()}, "inv_diag"},
                                                                 {std::vector<tt>{create_and_tt()}, "and"},
                                                                 {std::vector<tt>{create_nand_tt()}, "nand"},
                                                                 {std::vector<tt>{create_or_tt()}, "or"},
                                                                 {std::vector<tt>{create_nor_tt()}, "nor"},
                                                                 {std::vector<tt>{create_xor_tt()}, "xor"},
                                                                 {std::vector<tt>{create_xnor_tt()}, "xnor"},
                                                                 {create_fan_out_tt(), "fo2"},
                                                                 {create_crossing_wire_tt(), "cx"},
                                                                 {create_half_adder_tt(), "ha"},
                                                                 {create_double_wire_tt(), "hourglass"}}};

    double mean_ratio_num_op_sketch_to_num_op_grid = 0.0;
    double total_runtime_grid                      = 0.0;
    double total_runtime_sketch                    = 0.0;

    for (const auto& [truth_table, gate] : truth_tables_and_names)
    {
        const auto lyt = read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(fmt::format("{}/{}.sqd", folder, gate), gate);

        // operational domain stats
        operational_domain_stats op_domain_stats_grid{};
        operational_domain_stats op_domain_stats_sketch{};

        op_domain_params.operational_params.strategy_to_analyze_operational_status =
            is_operational_params::operational_analysis_strategy::SIMULATION_ONLY;

        const auto op_domain_grid =
            operational_domain_grid_search(lyt, truth_table, op_domain_params, &op_domain_stats_grid);

        total_runtime_grid += mockturtle::to_seconds(op_domain_stats_grid.time_total);

        op_domain_params.operational_params.strategy_to_analyze_operational_status =
            is_operational_params::operational_analysis_strategy::FILTER_ONLY;

        const auto op_domain_gs_sketch =
            operational_domain_grid_search(lyt, truth_table, op_domain_params, &op_domain_stats_sketch);

        total_runtime_sketch += mockturtle::to_seconds(op_domain_stats_sketch.time_total);

        write_operational_domain(op_domain_grid, fmt::format("{}/grid_{}.csv", folder, gate));
        write_operational_domain(op_domain_gs_sketch, fmt::format("{}/sketch_{}.csv", folder, gate));

        mean_ratio_num_op_sketch_to_num_op_grid +=
            static_cast<double>(op_domain_stats_sketch.num_operational_parameter_combinations) /
            static_cast<double>(op_domain_stats_grid.num_operational_parameter_combinations);

        opdomain_exp(
            // Benchmark
            gate, lyt.num_cells(),

            // Operational Domain (determine the operation status by simulation)
            op_domain_stats_grid.num_operational_parameter_combinations,
            mockturtle::to_seconds(op_domain_stats_grid.time_total),

            // Operational Domain Sketch (determine the operation status by pruning)
            op_domain_stats_sketch.num_operational_parameter_combinations,
            mockturtle::to_seconds(op_domain_stats_sketch.time_total),
            static_cast<double>(op_domain_stats_sketch.num_operational_parameter_combinations) /
                static_cast<double>(op_domain_stats_grid.num_operational_parameter_combinations),
            mockturtle::to_seconds(op_domain_stats_grid.time_total) /
                mockturtle::to_seconds(op_domain_stats_sketch.time_total));

        opdomain_exp.save();
        opdomain_exp.table();
    }

    opdomain_exp("Total", 0, 0, total_runtime_grid, 0, total_runtime_sketch,
                 mean_ratio_num_op_sketch_to_num_op_grid / truth_tables_and_names.size(), 0.0);

    opdomain_exp.save();
    opdomain_exp.table();

    return EXIT_SUCCESS;
}
