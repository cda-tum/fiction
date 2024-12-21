#include "fiction/algorithms/physical_design/design_sidb_gates.hpp"
#include "fiction/algorithms/simulation/sidb/defect_clearance.hpp"
#include "fiction/algorithms/simulation/sidb/defect_influence.hpp"
#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/io/read_sqd_layout.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/truth_table_utils.hpp"
#include "fiction_experiments.hpp"

#include <fmt/format.h>

#include <array>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

using namespace fiction;

int main()  // NOLINT
{
    experiments::experiment<std::string, std::size_t, std::size_t> simulation_exp{"Benchmark", "Gate Name", "samples",
                                                                                  "wrong output of quicktrace"};

    static const std::string folder        = fmt::format("{}bestagon_gates_type_tags/", EXPERIMENTS_PATH);
    static const std::string output_folder = fmt::format("{}quicktrace/plots/", EXPERIMENTS_PATH);

    static const std::array<std::pair<std::string, std::vector<tt>>, 9> gates = {
        std::make_pair("and", std::vector<tt>{create_and_tt()}),
        std::make_pair("xor", std::vector<tt>{create_xor_tt()}),
        std::make_pair("or", std::vector<tt>{create_or_tt()}),
        std::make_pair("xnor", std::vector<tt>{create_xnor_tt()}),
        std::make_pair("fo2", std::vector<tt>{create_fan_out_tt()}),
        std::make_pair("nor", std::vector<tt>{create_nor_tt()}),
        std::make_pair("nand", std::vector<tt>{create_nand_tt()}),
        std::make_pair("inv", std::vector<tt>{create_not_tt()}),
        std::make_pair("wire", std::vector<tt>{create_id_tt()})};

    auto lyt = read_sqd_layout<sidb_100_cell_clk_lyt_cube>(
        fmt::format("{}/gate_skeletons/skeleton_bestagons_with_tags/skeleton_hex_inputsdbp_2i1o.sqd", folder));

    const design_sidb_gates_params<cell<sidb_100_cell_clk_lyt_cube>> params_2_in_1_out{
        is_operational_params{sidb_simulation_parameters{2, -0.32}},
        design_sidb_gates_params<
            cell<sidb_100_cell_clk_lyt_cube>>::design_sidb_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
        {{14, 12, 0}, {24, 23, 0}},
        3};

    const auto                  sidb_sim = sidb_simulation_parameters{2, -0.32, 5.6, 5.0};
    const is_operational_params is_op_params{sidb_sim};

    // for this experiment, we use a stray SiDB defect
    const auto stray_db = fiction::sidb_defect{fiction::sidb_defect_type::DB, -1, 4.1, 1.8};
    // const auto si_vacancy = fiction::sidb_defect{fiction::sidb_defect_type::SI_VACANCY, -1, 10.6, 5.9};

    defect_influence_params<fiction::cell<sidb_100_cell_clk_lyt_cube>> params{};
    params.additional_scanning_area = {100, 1000};
    params.defect                   = stray_db;
    params.operational_params       = is_op_params;

    for (const auto& [gate, truth_table] : gates)
    {
        const auto exhaustive_design = design_sidb_gates(lyt, truth_table, params_2_in_1_out);

        // Create gate directory for plots
        const std::string gate_folder = fmt::format("{}{}/", output_folder, gate);
        std::filesystem::create_directories(gate_folder);

        std::size_t counter_for_wrong_output_of_quicktrace = 0;

        for (const auto& gate_lyt : exhaustive_design)
        {
            // using grid search to find the minimum defect clearance
            defect_influence_stats grid_stats{};
            const auto             op_defect_grid =
                defect_influence_grid_search(gate_lyt, truth_table, 1, defect_params, &grid_stats);
            const auto avoidance_grid = calculate_defect_clearance(gate_lyt, op_defect_grid);

            // using QuickTrace to find the minimum clearance
            defect_influence_stats contour_stats{};
            const auto             op_defect_contour =
                defect_influence_quicktrace(gate_lyt, truth_table, 100, defect_params, &contour_stats);
            const auto avoidance_contour = calculate_defect_clearance(gate_lyt, op_defect_contour);

            // check if QuickTrace and grid search give the same result
            if (std::abs(avoidance_grid.defect_clearance_distance - avoidance_contour.defect_clearance_distance) >
                    10E-6 &&
                avoidance_grid.defect_position != avoidance_contour.defect_position)
            {
                counter_for_wrong_output_of_quicktrace++;
            }
        }

        simulation_exp(gate, exhaustive_design.size(), counter_for_wrong_output_of_quicktrace);
    }
    return EXIT_SUCCESS;
}
