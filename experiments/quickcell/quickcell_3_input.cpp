//
// Created by Jan Drewniok 10.06.24
//

#include "fiction_experiments.hpp"

#include <fiction/algorithms/iter/bdl_input_iterator.hpp>
#include <fiction/algorithms/physical_design/design_sidb_gates.hpp>
#include <fiction/algorithms/simulation/sidb/detect_bdl_wires.hpp>
#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>
#include <fiction/io/read_sqd_layout.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/truth_table_utils.hpp>

#include <fmt/format.h>
#include <mockturtle/utils/stopwatch.hpp>

#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

using namespace fiction;

// This script designs standard cells for 3-input Boolean functions using *QuickCell*. The number of designed gate
// implementations and the required runtime are recorded.

int main()  // NOLINT
{
    experiments::experiment<std::string, uint64_t, uint64_t, double, uint64_t, double, uint64_t, double, uint64_t,
                            double>
        simulation_exp{"benchmark",
                       "gate",                     // std::string
                       "#Total Layouts",           // uint64_t
                       "#Gates (QuickCell)",       // uint64_t
                       "runtime (QuickCell) [s]",  // double
                       "#Lp1",                     // uint64_t
                       "#Lp1/N [%]",               // double
                       "#Lp2",                     // uint64_t
                       "#Lp2/N [%]",               // double
                       "#Lp3",                     // uint64_t
                       "#Lp3/N [%]"};              // double

    const auto truth_tables = std::vector<std::vector<tt>>{
        std::vector<tt>{create_and3_tt()},   std::vector<tt>{create_xor_and_tt()}, std::vector<tt>{create_or_and_tt()},
        std::vector<tt>{create_onehot_tt()}, std::vector<tt>{create_maj_tt()},     std::vector<tt>{create_gamble_tt()},
        std::vector<tt>{create_dot_tt()},    std::vector<tt>{create_ite_tt()},     std::vector<tt>{create_and_xor_tt()},
        std::vector<tt>{create_xor3_tt()}};

    static const std::vector<std::string> gate_names = {"and3",   "xor_and", "or_and", "onehot",  "maj",
                                                        "gamble", "dot",     "ite",    "and_xor", "xor3"};

    static const std::string folder = fmt::format("{}/gate_skeletons/skeleton_3_input_1_output/", EXPERIMENTS_PATH);

    // this skeleton is used for the design of AND3 and Gamble
    const auto skeleton_one =
        read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(fmt::format("{}/{}", folder, "3_in_1_out_skeleton_one.sqd"));

    // this skeleton is used for the design of all Boolean functions, except for AND3 and Gamble.
    const auto skeleton_two =
        read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(fmt::format("{}/{}", folder, "3_in_1_out_skeleton_two.sqd"));

    const design_sidb_gates_params<fiction::cell<sidb_100_cell_clk_lyt_siqad>> params{
        is_operational_params{sidb_simulation_parameters{2, -0.31}, sidb_simulation_engine::QUICKEXACT,
                              bdl_input_iterator_params{detect_bdl_wires_params{3.0}},
                              operational_condition::REJECT_KINKS},
        design_sidb_gates_params<fiction::cell<sidb_100_cell_clk_lyt_siqad>>::design_sidb_gates_mode::QUICKCELL,
        {{22, 6, 0}, {32, 12, 0}},
        4};

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

        const auto final_number_of_gates = quickcell_design.size();

        simulation_exp(gate_names[i], stats_quickcell.number_of_layouts, final_number_of_gates, runtime_quickcell,
                       stats_quickcell.number_of_layouts_after_first_pruning,
                       100.0 * static_cast<double>(stats_quickcell.number_of_layouts_after_first_pruning) /
                           static_cast<double>(stats_quickcell.number_of_layouts),
                       stats_quickcell.number_of_layouts_after_second_pruning,
                       100.0 * static_cast<double>(stats_quickcell.number_of_layouts_after_second_pruning) /
                           static_cast<double>(stats_quickcell.number_of_layouts),
                       stats_quickcell.number_of_layouts_after_third_pruning,
                       100.0 * static_cast<double>(stats_quickcell.number_of_layouts_after_third_pruning) /
                           static_cast<double>(stats_quickcell.number_of_layouts));

        simulation_exp.save();
        simulation_exp.table();
    }

    simulation_exp.save();
    simulation_exp.table();

    return EXIT_SUCCESS;
}
