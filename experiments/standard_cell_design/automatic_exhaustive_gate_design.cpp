//
// Created by Jan Drewniok 10.06.25
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

#include <array>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

// This script uses the *Automatic Exhaustive Gate Designer* and designs all gate implementations with up to 3 SiDBs for
// the Bestagon template. The results are presented in * \"Minimal Design of SiDB Gates: An Optimal Basis for Circuits
// Based on Silicon Dangling Bonds\" by J. Drewniok, M. Walter, and R. Wille in NANOARCH 2023
// (https://dl.acm.org/doi/10.1145/3611315.3633241).

using namespace fiction;

int main()  // NOLINT
{
    // "d" describes the number of canvas SiDBs used to design the gates.
    experiments::experiment<std::string, uint64_t, uint64_t, uint64_t> simulation_exp{
        "benchmark",
        "gate",   // std::string
        "d = 1",  // uint64_t
        "d = 2",  // uint64_t
        "d = 3"   // uint64_t
    };

    const auto truth_tables_and_names = std::array<std::pair<std::vector<tt>, std::string>, 18>{
        {{std::vector<tt>{create_not_tt()}, "inv"},
         {std::vector<tt>{create_id_tt()}, "wire"},
         {std::vector<tt>{create_not_tt()}, "inv_diag"},
         {std::vector<tt>{create_id_tt()}, "wire_diag"},
         {std::vector<tt>{create_fan_out_tt()}, "fo2"},
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

    // 1-input, 1-output gates with straight I/O pins
    const auto skeleton_1i1o_straight = read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(
        fmt::format("{}/{}", folder, "skeleton_hex_inputsdbp_1i1o_straight.sqd"));

    design_sidb_gates_params<fiction::cell<sidb_100_cell_clk_lyt_siqad>> params_1i1o_straight{
        is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT,
                              bdl_input_iterator_params{}, is_operational_params::operational_condition::REJECT_KINKS},
        design_sidb_gates_params<
            fiction::cell<sidb_100_cell_clk_lyt_siqad>>::design_sidb_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
        {{13, 6, 0}, {23, 14, 0}},
        3,
        design_sidb_gates_params<
            fiction::cell<sidb_100_cell_clk_lyt_siqad>>::termination_condition::ALL_COMBINATIONS_ENUMERATED};
    // ------------------------------------------------

    // 1-input, 1-output gates with diagonal I/O pins
    const auto skeleton_1i1o_diagonal = read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(
        fmt::format("{}/{}", folder, "skeleton_hex_inputsdbp_1i1o_diagonal.sqd"));

    design_sidb_gates_params<fiction::cell<sidb_100_cell_clk_lyt_siqad>> params_1i1o_diagonal{
        is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT,
                              bdl_input_iterator_params{}, is_operational_params::operational_condition::REJECT_KINKS},
        design_sidb_gates_params<
            fiction::cell<sidb_100_cell_clk_lyt_siqad>>::design_sidb_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
        {{14, 6, 0}, {24, 14, 0}},
        3,
        design_sidb_gates_params<
            fiction::cell<sidb_100_cell_clk_lyt_siqad>>::termination_condition::ALL_COMBINATIONS_ENUMERATED};
    // ------------------------------------------------

    // 2-input, 1-output gates
    const auto skeleton_2i1o =
        read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(fmt::format("{}/{}", folder, "skeleton_hex_inputsdbp_2i1o.sqd"));

    design_sidb_gates_params<fiction::cell<sidb_100_cell_clk_lyt_siqad>> params_2i1o{
        is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT,
                              bdl_input_iterator_params{}, is_operational_params::operational_condition::REJECT_KINKS},
        design_sidb_gates_params<
            fiction::cell<sidb_100_cell_clk_lyt_siqad>>::design_sidb_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
        {{14, 6, 0}, {24, 12, 0}},
        3,
        design_sidb_gates_params<
            fiction::cell<sidb_100_cell_clk_lyt_siqad>>::termination_condition::ALL_COMBINATIONS_ENUMERATED};

    // 1-input, 2-output gates
    const auto skeleton_1i2o =
        read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(fmt::format("{}/{}", folder, "skeleton_hex_inputsdbp_1i2o.sqd"));

    design_sidb_gates_params<fiction::cell<sidb_100_cell_clk_lyt_siqad>> params_1i2o{
        is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT,
                              bdl_input_iterator_params{}, is_operational_params::operational_condition::REJECT_KINKS},
        design_sidb_gates_params<
            fiction::cell<sidb_100_cell_clk_lyt_siqad>>::design_sidb_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
        {{15, 8, 0}, {23, 14, 0}},
        3,
        design_sidb_gates_params<
            fiction::cell<sidb_100_cell_clk_lyt_siqad>>::termination_condition::ALL_COMBINATIONS_ENUMERATED};
    // ------------------------------------------------

    // 2-input, 2-output gates
    const auto skeleton_2i2o =
        read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(fmt::format("{}/{}", folder, "skeleton_hex_inputsdbp_2i2o.sqd"));

    design_sidb_gates_params<fiction::cell<sidb_100_cell_clk_lyt_siqad>> params_2i2o{
        is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT,
                              bdl_input_iterator_params{}, is_operational_params::operational_condition::REJECT_KINKS},
        design_sidb_gates_params<
            fiction::cell<sidb_100_cell_clk_lyt_siqad>>::design_sidb_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
        {{14, 6, 0}, {24, 13, 0}},
        3,
        design_sidb_gates_params<
            fiction::cell<sidb_100_cell_clk_lyt_siqad>>::termination_condition::ALL_COMBINATIONS_ENUMERATED};
    // ------------------------------------------------

    for (const auto& [truth_table, gate_name] : truth_tables_and_names)
    {
        std::vector<uint64_t> number_of_gate_implementations{};

        number_of_gate_implementations.reserve(3);

        for (uint64_t num_sidbs = 1; num_sidbs <= 3; num_sidbs++)
        {
            std::vector<sidb_100_cell_clk_lyt_siqad> automatic_exhaustive_design = {};

            if (gate_name == "cx" || gate_name == "ha" || gate_name == "hourglass")
            {
                params_2i2o.number_of_canvas_sidbs = num_sidbs;
                automatic_exhaustive_design        = design_sidb_gates(skeleton_2i2o, truth_table, params_2i2o);
            }
            else if (gate_name == "wire" || gate_name == "inv")
            {
                params_1i1o_straight.number_of_canvas_sidbs = num_sidbs;
                automatic_exhaustive_design =
                    design_sidb_gates(skeleton_1i1o_straight, truth_table, params_1i1o_straight);
            }
            else if (gate_name == "inv_diag" || gate_name == "wire_diag")
            {
                params_1i1o_diagonal.number_of_canvas_sidbs = num_sidbs;
                automatic_exhaustive_design =
                    design_sidb_gates(skeleton_1i1o_diagonal, truth_table, params_1i1o_diagonal);
            }
            else if (gate_name == "f02")
            {
                params_1i2o.number_of_canvas_sidbs = num_sidbs;
                automatic_exhaustive_design        = design_sidb_gates(skeleton_1i2o, truth_table, params_1i2o);
            }
            else
            {
                params_2i1o.number_of_canvas_sidbs = num_sidbs;
                automatic_exhaustive_design        = design_sidb_gates(skeleton_2i1o, truth_table, params_2i1o);
            }
            number_of_gate_implementations.push_back(automatic_exhaustive_design.size());
        }

        const auto number_of_gates_with_one_canvas_sidb    = number_of_gate_implementations.at(0);
        const auto number_of_gates_with_two_canvas_sidbs   = number_of_gate_implementations.at(1);
        const auto number_of_gates_with_three_canvas_sidbs = number_of_gate_implementations.at(2);

        simulation_exp(gate_name, number_of_gates_with_one_canvas_sidb, number_of_gates_with_two_canvas_sidbs,
                       number_of_gates_with_three_canvas_sidbs);

        simulation_exp.save();
        simulation_exp.table();
    }

    return EXIT_SUCCESS;
}
