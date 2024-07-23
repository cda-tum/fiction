//
// Created by Jan Drewniok 10.06.24
//

#include "fiction_experiments.hpp"

#include <fiction/algorithms/physical_design/design_sidb_gates.hpp>
#include <fiction/algorithms/simulation/sidb/detect_bdl_wires.hpp>
#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>
#include <fiction/io/read_sqd_layout.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/truth_table_utils.hpp>

#include <fmt/format.h>

#include <array>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

using namespace fiction;

// This script designs standard cells for 3-input Boolean functions using *QuickCell*. The number of designed gate
// implementations and the required runtime are recorded.

int main()  // NOLINT
{
    experiments::experiment<std::string, uint64_t, double> simulation_exp{"benchmark", "gate", "#Gates (QuickCell)",
                                                                          "runtime (QuickCell)"};

    const auto truth_tables = std::vector<std::vector<tt>>{
        std::vector<tt>{create_and3_tt()},   std::vector<tt>{create_xor_and_tt()}, std::vector<tt>{create_or_and_tt()},
        std::vector<tt>{create_onehot_tt()}, std::vector<tt>{create_maj_tt()},     std::vector<tt>{create_gamble_tt()},
        std::vector<tt>{create_dot_tt()},    std::vector<tt>{create_ite_tt()},     std::vector<tt>{create_and_xor_tt()},
        std::vector<tt>{create_xor3_tt()}};

    static const std::array<std::string, 13> gate_names = {"and3",   "xor_and", "or_and", "onehot",  "maj",
                                                           "gamble", "dot",     "ite",    "and_xor", "xor3"};

    static const std::string folder = fmt::format("{}/gate_skeletons/skeleton_3_input_1_output/", EXPERIMENTS_PATH);

    const auto skeleton_one =
        read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(fmt::format("{}/{}", folder, "3_in_1_out_skeleton_one.sqd"));

    const auto skeleton_two =
        read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(fmt::format("{}/{}", folder, "3_in_1_out_skeleton_two.sqd"));

    const design_sidb_gates_params<fiction::cell<sidb_100_cell_clk_lyt_siqad>> params{
        is_operational_params{sidb_simulation_parameters{2, -0.31}, sidb_simulation_engine::QUICKEXACT,
                              detect_bdl_wires_params{}, operational_condition::FORBIDDING_KINKS},
        design_sidb_gates_params<
            fiction::cell<sidb_100_cell_clk_lyt_siqad>>::design_sidb_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
        {{22, 6, 0}, {32, 12, 0}},
        4};

    const design_sidb_gates_params<fiction::cell<sidb_100_cell_clk_lyt_siqad>> params_2_in_2_out{
        is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT,
                              detect_bdl_wires_params{}, operational_condition::FORBIDDING_KINKS},
        design_sidb_gates_params<
            fiction::cell<sidb_100_cell_clk_lyt_siqad>>::design_sidb_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
        {{14, 6, 0}, {24, 14, 0}},
        3};

    double sum_quickcell_runtime = 0;

    for (auto i = 0u; i < truth_tables.size(); i++)
    {
        const auto& table = truth_tables[i];

        std::vector<sidb_100_cell_clk_lyt_siqad> quickcell_design{};
        design_sidb_gates_stats                  stats_quickcell{};

        if (gate_names[i] == "and3" || gate_names[i] == "gamble")
        {
            quickcell_design = design_sidb_gates(skeleton_one, table, params, &stats_quickcell);
        }
        else
        {
            quickcell_design = design_sidb_gates(skeleton_two, table, params, &stats_quickcell);
        }

        const auto runtime_quickcell = mockturtle::to_seconds(stats_quickcell.time_total);

        sum_quickcell_runtime += runtime_quickcell;

        const auto final_number_of_gates = quickcell_design.size();

        simulation_exp(gate_names[i], final_number_of_gates, runtime_quickcell);

        simulation_exp.save();
        simulation_exp.table();
    }

    simulation_exp("", 0, sum_quickcell_runtime);

    simulation_exp.save();
    simulation_exp.table();

    return EXIT_SUCCESS;
}
