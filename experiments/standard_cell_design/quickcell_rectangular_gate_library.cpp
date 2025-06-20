//
// Created by Jan Drewniok 01.12.24
//

#include "fiction_experiments.hpp"

#include <fiction/algorithms/iter/bdl_input_iterator.hpp>
#include <fiction/algorithms/physical_design/design_sidb_gates.hpp>
#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>
#include <fiction/io/read_sqd_layout.hpp>
#include <fiction/io/write_sqd_layout.hpp>
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

// This script designs all gate implementations for 2-input Boolean functions using *QuickCell* and a 16.896 x 16.896 nm
// rectangular skeleton.

using namespace fiction;

int main()  // NOLINT
{
    experiments::experiment<std::string, uint64_t, double, uint64_t, double, uint64_t, double, uint64_t, double>
        simulation_exp{"benchmark",
                       "gate",                     // std::string
                       "#Gates (QuickCell)",       // uint64_t
                       "runtime (QuickCell) [s]",  // double
                       "#Lp1",                     // uint64_t
                       "#Lp1/N [%]",               // double
                       "#Lp2",                     // uint64_t
                       "#Lp2/N [%]",               // double
                       "#Lp3",                     // uint64_t
                       "#Lp3/N [%]"};              // double

    const auto truth_tables_and_names = std::array<std::pair<std::vector<tt>, std::string>, 22>{
        {{std::vector<tt>{create_not_tt()}, "inv_1i_top_1o_right"},
         {std::vector<tt>{create_id_tt()}, "wire_1i_top_1o_right"},
         {std::vector<tt>{create_not_tt()}, "inv_1i_top_1o_down"},
         {std::vector<tt>{create_id_tt()}, "wire_1i_top_1o_down"},
         {std::vector<tt>{create_not_tt()}, "inv_1i_left_1o_right"},
         {std::vector<tt>{create_id_tt()}, "wire_1i_left_1o_right"},
         {create_fan_out_tt(), "fo2_1i_top_2o_left_right"},
         {create_fan_out_tt(), "fo2_1i_top_2o_right_down"},
         {std::vector<tt>{create_and_tt()}, "and_2i_top_left_1o_right"},
         {std::vector<tt>{create_nand_tt()}, "nand_2i_top_left_1o_right"},
         {std::vector<tt>{create_or_tt()}, "or_2i_top_left_1o_right"},
         {std::vector<tt>{create_nor_tt()}, "nor_2i_top_left_1o_right"},
         {std::vector<tt>{create_xor_tt()}, "xor_2i_top_left_1o_right"},
         {std::vector<tt>{create_xnor_tt()}, "xnor_2i_top_left_1o_right"},
         {std::vector<tt>{create_lt_tt()}, "lt_2i_top_left_1o_right"},
         {std::vector<tt>{create_gt_tt()}, "gt_2i_top_left_1o_right"},
         {std::vector<tt>{create_le_tt()}, "le_2i_top_left_1o_right"},
         {std::vector<tt>{create_ge_tt()}, "ge_2i_top_left_1o_right"},
         {std::vector<tt>{create_crossing_wire_tt()}, "cx_2i_top_left_2o_down_right"},
         {std::vector<tt>{create_half_adder_tt()}, "ha_2i_top_left_2o_down_right"},
         {std::vector<tt>{create_double_wire_tt()}, "hourglass_2i_top_left_2o_down_right"}}};

    static const std::string folder = fmt::format("{}/gate_skeletons/rectangular_skeletons/", EXPERIMENTS_PATH);

    const auto rectangular_2i_top_left_2o_down_right = read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(
        fmt::format("{}/{}", folder, "rectangular_2i_top_left_2o_down_right.sqd"));

    const auto rectangular_2i_top_left_1o_right = read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(
        fmt::format("{}/{}", folder, "rectangular_2i_top_left_1o_right.sqd"));

    const auto rectangular_1i_top_1o_right =
        read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(fmt::format("{}/{}", folder, "rectangular_1i_top_1o_right.sqd"));

    const auto rectangular_1i_top_1o_down =
        read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(fmt::format("{}/{}", folder, "rectangular_1i_top_1o_down.sqd"));

    const auto rectangular_1i_left_1o_right =
        read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(fmt::format("{}/{}", folder, "rectangular_1i_left_1o_right.sqd"));

    const auto rectangular_1i_top_2o_right_down = read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(
        fmt::format("{}/{}", folder, "rectangular_1i_top_2o_right_down.sqd"));

    const auto rectangular_1i_top_2o_left_right = read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(
        fmt::format("{}/{}", folder, "rectangular_1i_top_2o_left_right.sqd"));

    constexpr auto num_canvas_sidbs                  = 3u;
    constexpr auto num_canvas_sidbs_2_input_2_output = 4u;

    design_sidb_gates_params<fiction::cell<sidb_100_cell_clk_lyt_siqad>> params{
        is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT,
                              bdl_input_iterator_params{{3}},
                              is_operational_params::operational_condition::REJECT_KINKS},
        design_sidb_gates_params<fiction::cell<sidb_100_cell_clk_lyt_siqad>>::design_sidb_gates_mode::QUICKCELL,
        {{18, 9, 0}, {26, 13, 0}},
        num_canvas_sidbs};

    for (const auto& [truth_table, gate_name] : truth_tables_and_names)
    {
        std::vector<sidb_100_cell_clk_lyt_siqad> quickcell_design{};
        design_sidb_gates_stats                  stats_quickcell{};

        if (gate_name == "cx_2i_top_left_2o_down_right" || gate_name == "ha_2i_top_left_2o_down_right" ||
            gate_name == "hourglass_2i_top_left_2o_down_right")
        {
            params.number_of_canvas_sidbs = num_canvas_sidbs_2_input_2_output;
            params.canvas                 = {{17, 8, 0}, {27, 14, 0}};
            quickcell_design =
                design_sidb_gates(rectangular_2i_top_left_2o_down_right, truth_table, params, &stats_quickcell);
        }

        else
        {
            params.number_of_canvas_sidbs = num_canvas_sidbs;

            if (gate_name == "fo2_1i_top_2o_left_right")
            {
                quickcell_design =
                    design_sidb_gates(rectangular_1i_top_2o_left_right, truth_table, params, &stats_quickcell);
            }

            else if (gate_name == "fo2_1i_top_2o_right_down")
            {
                quickcell_design =
                    design_sidb_gates(rectangular_1i_top_2o_right_down, truth_table, params, &stats_quickcell);
            }

            else if (gate_name == "wire_1i_top_1o_right" || gate_name == "inv_1i_top_1o_right")
            {
                quickcell_design =
                    design_sidb_gates(rectangular_1i_top_1o_right, truth_table, params, &stats_quickcell);
            }

            else if (gate_name == "wire_1i_top_1o_down" || gate_name == "inv_1i_top_1o_down")
            {
                quickcell_design = design_sidb_gates(rectangular_1i_top_1o_down, truth_table, params, &stats_quickcell);
            }

            else if (gate_name == "wire_1i_left_1o_right" || gate_name == "inv_1i_left_1o_right")
            {
                quickcell_design =
                    design_sidb_gates(rectangular_1i_left_1o_right, truth_table, params, &stats_quickcell);
            }

            else
            {
                quickcell_design =
                    design_sidb_gates(rectangular_2i_top_left_1o_right, truth_table, params, &stats_quickcell);
            }
        }

        // Write the layout to a file
        write_sqd_layout(quickcell_design.front(), fmt::format("{}/{}", folder, gate_name + ".sqd"));

        const auto runtime_quickcell = mockturtle::to_seconds(stats_quickcell.time_total);

        const auto total_number_of_layout = stats_quickcell.number_of_layouts;

        simulation_exp(gate_name, quickcell_design.size(), runtime_quickcell,
                       stats_quickcell.number_of_layouts_after_first_pruning,
                       static_cast<double>(stats_quickcell.number_of_layouts_after_first_pruning) /
                           static_cast<double>(total_number_of_layout) * 100,
                       stats_quickcell.number_of_layouts_after_second_pruning,
                       static_cast<double>(stats_quickcell.number_of_layouts_after_second_pruning) /
                           static_cast<double>(total_number_of_layout) * 100,
                       stats_quickcell.number_of_layouts_after_third_pruning,
                       static_cast<double>(stats_quickcell.number_of_layouts_after_third_pruning) /
                           static_cast<double>(total_number_of_layout) * 100);

        simulation_exp.save();
        simulation_exp.table();
    }

    simulation_exp.save();
    simulation_exp.table();

    return EXIT_SUCCESS;
}
