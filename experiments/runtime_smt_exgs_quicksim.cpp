//
// Created by Jan Drewniok 01.01.23
//

#include "fiction/algorithms/simulation/sidb/exact_ground_state_simulation.hpp"
#include "fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp"
#include "fiction/io/read_sqd_layout.hpp"  // reader for SiDB layouts including surface scan data
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/types.hpp"  // pre-defined types suitable for the FCN domain
#include "fiction_experiments.hpp"

#include <fiction/algorithms/simulation/sidb/time_to_solution.hpp>

#include <fmt/format.h>  // output formatting
#include <mockturtle/utils/stopwatch.hpp>

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>

using namespace fiction;

int main()  // NOLINT
{
    experiments::experiment<std::string, double, double, double, bool, std::string> simulation_exp{
        "benchmark", "gates", "runtime ExGS (in sec.)", "runtime SMT (in sec.)", "TTS QuickSim", "correct", "#SiDB"};

    double                sum_sr_exgs  = 0u;
    double                sum_tts      = 0u;
    double                sum_sr_quick = 0u;
    std::vector<uint64_t> db_num{};

    const std::string folder = fmt::format("{}/siqad_gates/", EXPERIMENTS_PATH);

    static const std::array<std::string, 5> folders = {folder + "and", folder + "nand", folder + "xnor", folder + "xor",
                                                       folder + "or"};

    bool all_true = true;
    for (const auto& folder_gate : folders)
    {
        for (const auto& file : std::filesystem::directory_iterator(folder_gate))
        {
            bool        groundstate_found = true;
            const auto& benchmark         = file.path();

            std::cout << benchmark << std::endl;

            const auto lyt = read_sqd_layout<sidb_cell_clk_lyt_siqad>(benchmark.string());

            // using ExGS for simulation
            exgs_stats<sidb_cell_clk_lyt_siqad> exgs_stats{};
            const sidb_simulation_parameters    params{2, -0.28};
            exhaustive_ground_state_simulation<sidb_cell_clk_lyt_siqad>(lyt, params, &exgs_stats);

            // TTS of QuickSim
            const quicksim_params  quicksim_params{params, 80, 0.7};
            time_to_solution_stats tts_stat{};
            sim_acc_tts<sidb_cell_clk_lyt_siqad>(lyt, quicksim_params, &tts_stat);

            // SMT solver
            exact_ground_state_simulation_params                         exgs_params{params};
            exact_ground_state_simulation_stats<sidb_cell_clk_lyt_siqad> exgs_exact_stats{};
            exact_ground_state_simulation<sidb_cell_clk_lyt_siqad>(lyt, exgs_params, &exgs_exact_stats);
            quicksim_stats<sidb_cell_clk_lyt_siqad> quicksim_stats{exgs_exact_stats.time_total,
                                                                   exgs_exact_stats.valid_lyts};
            groundstate_found = is_ground_state(quicksim_stats, exgs_stats);  // checks if SMT found the ground state

            simulation_exp(benchmark.string(), mockturtle::to_seconds(exgs_stats.time_total),
                           mockturtle::to_seconds(quicksim_stats.time_total), tts_stat.time_to_solution,
                           groundstate_found, std::to_string(lyt.num_cells()));
            db_num.push_back(lyt.num_cells());
            sum_sr_exgs += mockturtle::to_seconds(exgs_stats.time_total);
            sum_sr_quick += mockturtle::to_seconds(exgs_exact_stats.time_total);
            sum_tts += tts_stat.time_to_solution;
            if (!groundstate_found)
            {
                all_true = false;
            }
        }
    }

    auto min_db_num = std::min_element(db_num.begin(), db_num.end());
    auto max_db_num = std::max_element(db_num.begin(), db_num.end());

    simulation_exp("sum", sum_sr_exgs, sum_sr_quick, sum_tts, all_true,
                   std::to_string(*min_db_num) + " -- " + std::to_string(*max_db_num));
    simulation_exp.save();
    simulation_exp.table();
    return EXIT_SUCCESS;
}
