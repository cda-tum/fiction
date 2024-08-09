//
// Created by marcel on 22.07.24.
//

#include "fiction_experiments.hpp"  // experiment class

#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>      // operational domain computation algorithms
#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>  // SiDB simulation engines
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>  // SiDB simulation parameters
#include <fiction/io/read_sqd_layout.hpp>                                     // reader for SiDB layouts
#include <fiction/io/write_operational_domain.hpp>                            // writer for operational domains
#include <fiction/types.hpp>                    // pre-defined types suitable for the FCN domain
#include <fiction/utils/truth_table_utils.hpp>  // truth tables helper functions

#include <fmt/format.h>                    // string formatting
#include <mockturtle/utils/stopwatch.hpp>  // stopwatch for measuring time

#include <array>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace fiction;

int main()  // NOLINT
{
    experiments::experiment<std::string, uint64_t, uint64_t, double, uint64_t, double, uint64_t, double, uint64_t,
                            double, uint64_t, double, uint64_t, double>
        opdomain_exp{
            "Operational Domain SiQAD 3D",
            "Name",
            "#SiDBs",  // Benchmark
            "#Samples (GS)",
            "op. (GS)",
            "sim calls (GS)",
            "t in s (GS)",  // Grid Search
            "#Samples (RS)",
            "op. (RS)",
            "sim calls (RS)",
            "t in s (RS)",  // Random Sampling
            "#Samples (FF)",
            "op. (FF)",
            "sim calls (FF)",
            "t in s (FF)"  // Flood Fill
        };

    // simulation parameters
    sidb_simulation_parameters sim_params{};
    sim_params.base     = 2;
    sim_params.mu_minus = -0.28;

    // operational domain parameters
    operational_domain_params op_domain_params{};
    op_domain_params.simulation_parameters    = sim_params;
    op_domain_params.sim_engine               = sidb_simulation_engine::QUICKEXACT;
    op_domain_params.sweep_dimensions         = {{sweep_parameter::EPSILON_R},
                                                 {sweep_parameter::LAMBDA_TF},
                                                 {sweep_parameter::MU_MINUS}};
    op_domain_params.sweep_dimensions[0].min  = 1.0;
    op_domain_params.sweep_dimensions[0].max  = 10.0;
    op_domain_params.sweep_dimensions[0].step = 0.05;
    op_domain_params.sweep_dimensions[1].min  = 1.0;
    op_domain_params.sweep_dimensions[1].max  = 10.0;
    op_domain_params.sweep_dimensions[1].step = 0.05;
    op_domain_params.sweep_dimensions[2].min  = -0.50;
    op_domain_params.sweep_dimensions[2].max  = -0.10;
    op_domain_params.sweep_dimensions[2].step = 0.0025;

    // write operational domain parameters
    static const write_operational_domain_params write_op_domain_params{"1", "0", false};

    static const std::string folder = fmt::format("{}siqad_gates_type_tags/", EXPERIMENTS_PATH);

    static const std::array<std::pair<std::string, std::vector<tt>>, 5> gates = {
        std::make_pair("and", std::vector<tt>{create_and_tt()}),
        std::make_pair("nand", std::vector<tt>{create_nand_tt()}),
        std::make_pair("xnor", std::vector<tt>{create_xnor_tt()}),
        std::make_pair("xor", std::vector<tt>{create_xor_tt()}), std::make_pair("or", std::vector<tt>{create_or_tt()})};

    // total number of samples
    static std::size_t total_samples_gs = 0;
    static std::size_t total_samples_rs = 0;
    static std::size_t total_samples_ff = 0;

    // total number of simulator calls
    static std::size_t total_sim_calls_gs = 0;
    static std::size_t total_sim_calls_rs = 0;
    static std::size_t total_sim_calls_ff = 0;

    // total runtime
    static double total_runtime_gs = 0.0;
    static double total_runtime_rs = 0.0;
    static double total_runtime_ff = 0.0;

    for (const auto& [gate, truth_table] : gates)
    {
        for (const auto& file : std::filesystem::directory_iterator(fmt::format("{}{}", folder, gate)))
        {
            const auto& benchmark = file.path();

            std::cout << benchmark << std::endl;

            const auto lyt = read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(benchmark.string(), gate);

            // operational domain stats
            operational_domain_stats op_domain_stats_gs{};
            operational_domain_stats op_domain_stats_rs{};
            operational_domain_stats op_domain_stats_ff{};

            // compute the operational domains
            const auto op_domain_gs =
                operational_domain_grid_search(lyt, truth_table, op_domain_params, &op_domain_stats_gs);
            const auto op_domain_rs =
                operational_domain_random_sampling(lyt, truth_table, 20000, op_domain_params, &op_domain_stats_rs);
            const auto op_domain_ff =
                operational_domain_flood_fill(lyt, truth_table, 2000, op_domain_params, &op_domain_stats_ff);

            // write the operational domains to a CSV file
            write_operational_domain(op_domain_gs,
                                     fmt::format("{}operational_domain_grid_search_3d_siqad_{}.csv", folder, gate),
                                     write_op_domain_params);
            write_operational_domain(op_domain_rs,
                                     fmt::format("{}operational_domain_random_sampling_3d_siqad_{}.csv", folder, gate),
                                     write_op_domain_params);
            write_operational_domain(op_domain_ff,
                                     fmt::format("{}operational_domain_flood_fill_3d_siqad_{}.csv", folder, gate),
                                     write_op_domain_params);

            // update the total number of samples
            total_samples_gs += op_domain_stats_gs.num_evaluated_parameter_combinations;
            total_samples_rs += op_domain_stats_rs.num_evaluated_parameter_combinations;
            total_samples_ff += op_domain_stats_ff.num_evaluated_parameter_combinations;

            // update the total number of simulator calls
            total_sim_calls_gs += op_domain_stats_gs.num_simulator_invocations;
            total_sim_calls_rs += op_domain_stats_rs.num_simulator_invocations;
            total_sim_calls_ff += op_domain_stats_ff.num_simulator_invocations;

            // update the total runtime
            total_runtime_gs += mockturtle::to_seconds(op_domain_stats_gs.time_total);
            total_runtime_rs += mockturtle::to_seconds(op_domain_stats_rs.time_total);
            total_runtime_ff += mockturtle::to_seconds(op_domain_stats_ff.time_total);

            // compute the operational percentages
            const auto operational_percentage_gs =
                static_cast<double>(op_domain_stats_gs.num_operational_parameter_combinations) /
                static_cast<double>(op_domain_stats_gs.num_evaluated_parameter_combinations);
            const auto operational_percentage_rs =
                static_cast<double>(op_domain_stats_rs.num_operational_parameter_combinations) /
                static_cast<double>(op_domain_stats_rs.num_evaluated_parameter_combinations);
            const auto operational_percentage_ff =
                static_cast<double>(op_domain_stats_ff.num_operational_parameter_combinations) /
                static_cast<double>(op_domain_stats_ff.num_evaluated_parameter_combinations);

            opdomain_exp(
                // Benchmark
                benchmark.string(), lyt.num_cells(),

                // Grid Search
                op_domain_stats_gs.num_evaluated_parameter_combinations, operational_percentage_gs,
                op_domain_stats_gs.num_simulator_invocations, mockturtle::to_seconds(op_domain_stats_gs.time_total),

                // Random Sampling
                op_domain_stats_rs.num_evaluated_parameter_combinations, operational_percentage_rs,
                op_domain_stats_rs.num_simulator_invocations, mockturtle::to_seconds(op_domain_stats_rs.time_total),

                // Flood Fill
                op_domain_stats_ff.num_evaluated_parameter_combinations, operational_percentage_ff,
                op_domain_stats_ff.num_simulator_invocations, mockturtle::to_seconds(op_domain_stats_ff.time_total)

            );
        }

        opdomain_exp.save();
        opdomain_exp.table();
    }

    // log the total number of samples and simulator calls
    opdomain_exp(
        // Benchmark
        "Total", 0,

        // Grid Search
        total_samples_gs, 0.0, total_sim_calls_gs, total_runtime_gs,

        // Random Sampling
        total_samples_rs, 0.0, total_sim_calls_rs, total_runtime_rs,

        // Flood Fill
        total_samples_ff, 0.0, total_sim_calls_ff, total_runtime_ff

    );

    opdomain_exp.save();
    opdomain_exp.table();

    return EXIT_SUCCESS;
}
