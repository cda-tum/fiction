//
// Created by Jan Drewniok 01.01.23
//

#include "fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp"
#include "fiction/io/read_sqd_layout.hpp"  // reader for SiDB layouts including surface scan data
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/types.hpp"  // pre-defined types suitable for the FCN domain
#include "fiction_experiments.hpp"

#include <fiction/algorithms/simulation/sidb/time_to_solution.hpp>

#include <fmt/format.h>  // output formatting

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>

using namespace fiction;

int main()  // NOLINT
{
    experiments::experiment<std::string, double, double, double, double, std::string, std::string> simulation_exp{
        "benchmark",
        "gates",
        "single runtime exact (in sec.)",
        "simulation accuracy (in %)",
        "TTS (in sec.)",
        "single runtime of quicksim (in sec.)",
        "number of valid layouts",
        "SiDB dots"};

    double                sum_sr       = 0u;
    double                sum_tts      = 0u;
    double                sum_acc      = 0u;
    double                sum_sr_quick = 0u;
    std::vector<uint64_t> db_num{};
    uint64_t              benchmark_counter = 0u;

    const std::string folder = fmt::format("{}/bestagon_gates/", EXPERIMENTS_PATH);

    static const std::array<std::string, 12> folders = {
        folder + "and/",  folder + "cx/",  folder + "fo2/", folder + "ha/",   folder + "hourglass/", folder + "inv/",
        folder + "nand/", folder + "nor/", folder + "or/",  folder + "wire/", folder + "xnor/",      folder + "xor/"};
    //        static const std::array<std::string, 1> folders = {
    //                  folder + "hourglass/"};
    //    static const std::array<std::string, 10> folders = {
    //        folder + "and/",  folder + "fo2/",  folder + "inv/", folder + "inv_diag/",
    //        folder + "nand/", folder + "nor/", folder + "or/",  folder + "wire/", folder + "wire_diag/",  folder +
    //        "xor/"};
    //    static const std::array<std::string, 1> folders = {
    //        folder + "and/"};

    for (const auto& folder_gate : folders)
    {
        for (const auto& file : std::filesystem::directory_iterator(folder_gate))
        {
            benchmark_counter += 1;
            const auto& benchmark = file.path();

            std::cout << benchmark << std::endl;

            auto lyt = read_sqd_layout<sidb_cell_clk_lyt_siqad>(benchmark.string());

            const sidb_simulation_parameters params{2, -0.32};
            const quicksim_params            quicksim_params{params, 80, 0.70};

            time_to_solution_stats tts_stat{};
            sim_acc_tts<sidb_cell_clk_lyt_siqad>(lyt, quicksim_params, &tts_stat);

            simulation_exp(benchmark.string(), tts_stat.single_runtime_exhaustive, tts_stat.acc,
                           tts_stat.time_to_solution, tts_stat.mean_single_runtime,
                           std::to_string(tts_stat.number_valid_layouts_exgs), std::to_string(lyt.num_cells()));
            db_num.push_back(lyt.num_cells());
            sum_sr += tts_stat.single_runtime_exhaustive;
            sum_sr_quick += tts_stat.mean_single_runtime;
            sum_acc += tts_stat.acc;
            sum_tts += tts_stat.time_to_solution;
        }
    }

    auto min_db_num    = std::min_element(db_num.begin(), db_num.end());
    auto max_db_num    = std::max_element(db_num.begin(), db_num.end());
    auto mean_acc      = sum_acc / static_cast<double>(benchmark_counter);
    auto mean_sr_quick = sum_sr_quick / static_cast<double>(benchmark_counter);

    simulation_exp("sum", sum_sr, mean_acc, sum_tts, mean_sr_quick, "---",
                   std::to_string(*min_db_num) + " -- " + std::to_string(*max_db_num));
    simulation_exp.save();
    simulation_exp.table();
    return EXIT_SUCCESS;
}
