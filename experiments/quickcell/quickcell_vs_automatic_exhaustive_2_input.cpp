//
// Created by Jan Drewniok 10.06.24
//

#include "fiction_experiments.hpp"

#include <fiction/algorithms/iter/bdl_input_iterator.hpp>
#include <fiction/algorithms/physical_design/design_sidb_gates.hpp>
#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>
#include <fiction/io/read_sqd_layout.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/truth_table_utils.hpp>

#include <fmt/format.h>
#include <mockturtle/utils/stopwatch.hpp>

#include <array>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

// This script uses the *Automatic Exhaustive Gate Designer* and *QuickCell* to design gate implementations for 2-input
// Boolean functions. It records the number of designed gate implementations and the runtime required for each
// algorithm. The final column displays the runtime reduction factor achieved by *QuickCell*.

using namespace fiction;

int main()  // NOLINT
{
    experiments::experiment<std::string, uint64_t, double, uint64_t, uint64_t, double, double, uint64_t, double,
                            uint64_t, double, uint64_t, double, double>
        simulation_exp{"benchmark",
                       "gate",                                 // std::string
                       "#Total Layouts",                       // uint64_t
                       "runtime (Automatic Exhaustive) [s]",   // double
                       "#Gates (Automatic Exhaustive, sota)",  // uint64_t
                       "#Gates (QuickCell)",                   // uint64_t
                       "runtime (QuickCell, proposed) [s]",    // double
                       "t_sota / t_proposed",                  // double
                       "#Lp1",                                 // uint64_t
                       "#Lp1/N [%]",                           // double
                       "#Lp2",                                 // uint64_t
                       "#Lp2/N [%]",                           // double
                       "#Lp3",                                 // uint64_t
                       "#Lp3/N [%]",                           // double
                       "t_pruning [s]"};                       // double

    const auto truth_tables_and_names = std::array<std::pair<std::vector<tt>, std::string>, 15>{
        {{std::vector<tt>{create_not_tt()}, "inv"},
         {std::vector<tt>{create_id_tt()}, "wire"},
         {std::vector<tt>{create_and_tt()}, "and"},
         {std::vector<tt>{create_nand_tt()}, "nand"},
         {std::vector<tt>{create_or_tt()}, "or"},
         {std::vector<tt>{create_nor_tt()}, "nor"},
         {std::vector<tt>{create_xor_tt()}, "xor"},
         {std::vector<tt>{create_xnor_tt()}, "xnor"},
         {std::vector<tt>{create_lt_tt()}, "lt"},
         {std::vector<tt>{create_gt_tt()}, "gt"},
         {std::vector<tt>{create_le_tt()}, "le"},
         {std::vector<tt>{create_ge_tt()}, "ge"},
         {std::vector<tt>{create_crossing_wire_tt()}, "cx"},
         {std::vector<tt>{create_half_adder_tt()}, "ha"},
         {std::vector<tt>{create_double_wire_tt()}, "hourglass"}}};

    static const std::string folder = fmt::format("{}/gate_skeletons/skeleton_bestagons_with_tags", EXPERIMENTS_PATH);

    const auto skeleton_one_input_one_output_straight = read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(
        fmt::format("{}/{}", folder, "skeleton_hex_inputsdbp_1i1o_straight.sqd"));

    const auto skeleton_one_input_two_output =
        read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(fmt::format("{}/{}", folder, "skeleton_hex_inputsdbp_2i1o.sqd"));

    const auto skeleton_two_input_two_output =
        read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(fmt::format("{}/{}", folder, "skeleton_hex_inputsdbp_2i2o.sqd"));

    design_sidb_gates_params<fiction::cell<sidb_100_cell_clk_lyt_siqad>> params_1_in_1_out_straight{
        is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT,
                              bdl_input_iterator_params{}, is_operational_params::operational_condition::REJECT_KINKS},
        design_sidb_gates_params<
            fiction::cell<sidb_100_cell_clk_lyt_siqad>>::design_sidb_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
        {{9, 6, 0}, {21, 14, 0}},
        3};

    design_sidb_gates_params<fiction::cell<sidb_100_cell_clk_lyt_siqad>> params_2_in_1_out{
        is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT,
                              bdl_input_iterator_params{}, is_operational_params::operational_condition::REJECT_KINKS},
        design_sidb_gates_params<
            fiction::cell<sidb_100_cell_clk_lyt_siqad>>::design_sidb_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
        {{14, 6, 0}, {24, 10, 0}},
        3};

    design_sidb_gates_params<fiction::cell<sidb_100_cell_clk_lyt_siqad>> params_2_in_2_out{
        is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT,
                              bdl_input_iterator_params{}, is_operational_params::operational_condition::REJECT_KINKS},
        design_sidb_gates_params<
            fiction::cell<sidb_100_cell_clk_lyt_siqad>>::design_sidb_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
        {{14, 6, 0}, {24, 14, 0}},
        3};

    double sum_exhaustive_runtime = 0;
    double sum_quickcell_runtime  = 0;

    for (const auto& [truth_table, gate_name] : truth_tables_and_names)
    {
        design_sidb_gates_stats stats_automatic_exhaustive_design{};

        std::vector<sidb_100_cell_clk_lyt_siqad> automatic_exhaustive_design{};

        params_2_in_1_out.design_mode = design_sidb_gates_params<
            fiction::cell<sidb_100_cell_clk_lyt_siqad>>::design_sidb_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER;
        params_2_in_1_out.operational_params.op_condition = is_operational_params::operational_condition::REJECT_KINKS;
        params_2_in_2_out.design_mode                     = design_sidb_gates_params<
                                fiction::cell<sidb_100_cell_clk_lyt_siqad>>::design_sidb_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER;
        params_2_in_2_out.operational_params.op_condition = is_operational_params::operational_condition::REJECT_KINKS;

        params_1_in_1_out_straight.design_mode = design_sidb_gates_params<
            fiction::cell<sidb_100_cell_clk_lyt_siqad>>::design_sidb_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER;
        params_1_in_1_out_straight.operational_params.op_condition =
            is_operational_params::operational_condition::REJECT_KINKS;

        if (gate_name == "cx" || gate_name == "ha" || gate_name == "hourglass")
        {
            automatic_exhaustive_design = design_sidb_gates(skeleton_two_input_two_output, truth_table,
                                                            params_2_in_2_out, &stats_automatic_exhaustive_design);
        }
        else if (gate_name == "wire" || gate_name == "inv")
        {
            automatic_exhaustive_design =
                design_sidb_gates(skeleton_one_input_one_output_straight, truth_table, params_1_in_1_out_straight,
                                  &stats_automatic_exhaustive_design);
        }
        else
        {
            automatic_exhaustive_design = design_sidb_gates(skeleton_one_input_two_output, truth_table,
                                                            params_2_in_1_out, &stats_automatic_exhaustive_design);
        }

        std::vector<sidb_100_cell_clk_lyt_siqad> quickcell_design{};
        design_sidb_gates_stats                  stats_quickcell{};

        params_2_in_1_out.design_mode =
            design_sidb_gates_params<fiction::cell<sidb_100_cell_clk_lyt_siqad>>::design_sidb_gates_mode::QUICKCELL;

        params_2_in_2_out.design_mode =
            design_sidb_gates_params<fiction::cell<sidb_100_cell_clk_lyt_siqad>>::design_sidb_gates_mode::QUICKCELL;

        params_1_in_1_out_straight.design_mode =
            design_sidb_gates_params<fiction::cell<sidb_100_cell_clk_lyt_siqad>>::design_sidb_gates_mode::QUICKCELL;

        if (gate_name == "cx" || gate_name == "ha" || gate_name == "hourglass")
        {
            quickcell_design =
                design_sidb_gates(skeleton_two_input_two_output, truth_table, params_2_in_2_out, &stats_quickcell);
        }
        else if (gate_name == "wire" || gate_name == "inv")
        {
            quickcell_design = design_sidb_gates(skeleton_one_input_one_output_straight, truth_table,
                                                 params_1_in_1_out_straight, &stats_quickcell);
        }
        else
        {
            quickcell_design =
                design_sidb_gates(skeleton_one_input_two_output, truth_table, params_2_in_1_out, &stats_quickcell);
        }

        const auto runtime_automatic_exhaustive_design =
            mockturtle::to_seconds(stats_automatic_exhaustive_design.time_total);
        const auto runtime_quickcell = mockturtle::to_seconds(stats_quickcell.time_total);

        sum_exhaustive_runtime += runtime_automatic_exhaustive_design;
        sum_quickcell_runtime += runtime_quickcell;

        const auto time_reduction = runtime_automatic_exhaustive_design / runtime_quickcell;

        const auto total_number_of_layout = stats_quickcell.number_of_layouts;

        simulation_exp(gate_name, total_number_of_layout, runtime_automatic_exhaustive_design,
                       automatic_exhaustive_design.size(), quickcell_design.size(), runtime_quickcell, time_reduction,
                       stats_quickcell.number_of_layouts_after_first_pruning,
                       static_cast<double>(stats_quickcell.number_of_layouts_after_first_pruning) /
                           static_cast<double>(total_number_of_layout) * 100,
                       stats_quickcell.number_of_layouts_after_second_pruning,
                       static_cast<double>(stats_quickcell.number_of_layouts_after_second_pruning) /
                           static_cast<double>(total_number_of_layout) * 100,
                       stats_quickcell.number_of_layouts_after_third_pruning,
                       static_cast<double>(stats_quickcell.number_of_layouts_after_third_pruning) /
                           static_cast<double>(total_number_of_layout) * 100,
                       mockturtle::to_seconds(stats_quickcell.pruning_total));

        simulation_exp.save();
        simulation_exp.table();
    }

    const auto total_time_reduction = sum_exhaustive_runtime / sum_quickcell_runtime;

    simulation_exp("", 0, sum_exhaustive_runtime, 0, 0, sum_quickcell_runtime, total_time_reduction, 0, 0.0, 0, 0.0, 0,
                   0.0, 0.0);

    simulation_exp.save();
    simulation_exp.table();

    return EXIT_SUCCESS;
}
