//
// Created by Jan Drewniok on 17.05.24.
//

#include "fiction/algorithms/iter/bdl_input_iterator.hpp"
#include "fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp"
#include "fiction/algorithms/simulation/sidb/quickexact.hpp"
#include "fiction/algorithms/simulation/sidb/quicksim.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/algorithms/simulation/sidb/time_to_solution.hpp"
#include "fiction/io/read_sqd_layout.hpp"
#include "fiction/layouts/coordinates.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/truth_table_utils.hpp"
#include "fiction_experiments.hpp"
#include "mockturtle/utils/stopwatch.hpp"

#include <fmt/format.h>

#include <array>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

using namespace fiction;

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

    static const std::string folder = fmt::format("{}bestagon_gates_type_tags/", EXPERIMENTS_PATH);

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

    const sidb_simulation_parameters        sim_params{2, -0.32};
    const quicksim_params                   qs_params{sim_params};
    const quickexact_params<siqad::coord_t> qe_params{sim_params};
    const time_to_solution_params           tts_params{};

    for (const auto& [gate, truth_table] : gates)
    {
        for (const auto& file : std::filesystem::directory_iterator(fmt::format("{}{}", folder, gate)))
        {
            const auto layout = read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(file.path().string());

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
            quicksim_accuracy_mean += stats.acc;
            quicksim_single_runtime += stats.mean_single_runtime;

            // simulate layout with all input patterns
            auto       bii                = bdl_input_iterator<sidb_100_cell_clk_lyt_siqad>{layout};
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
                quicksim_accuracy_mean += stats.acc;
                quicksim_single_runtime += stats.mean_single_runtime;
            }
            quicksim_accuracy_mean = quicksim_accuracy_mean / static_cast<double>(instances);
            simulation_exp(gate, instances, runtime_exhaustive, runtime_quickexact, quicksim_accuracy_mean,
                           quicksim_single_runtime, tts_quicksim);
            simulation_exp.save();
            simulation_exp.table();
        }
    }
    return EXIT_SUCCESS;
}
