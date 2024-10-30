#include "fiction/algorithms/physical_design/design_sidb_gates.hpp"
#include "fiction/algorithms/simulation/sidb/defect_avoidance_distance.hpp"
#include "fiction/algorithms/simulation/sidb/defect_influence_operational_domain.hpp"
#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/algorithms/simulation/sidb/maximum_defect_influence_position_and_distance.hpp"
#include "fiction/io/read_sqd_layout.hpp"
#include "fiction/io/write_defect_influence_operational_domain.hpp"
#include "fiction/io/write_sqd_layout.hpp"
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

int main()
{
    experiments::experiment<std::string, double, std::size_t, double, std::size_t, double, std::size_t> simulation_exp{
        "Benchmark",       "Gate Name",       "distance_grid_search", "#samples_grid",
        "distance_random", "#samples_random", "distance_contour",     "#samples_contour",
    };

    static const std::string folder        = fmt::format("{}bestagon_gates_type_tags/", EXPERIMENTS_PATH);
    static const std::string output_folder = fmt::format("{}defect_avoidance_distance/plots/", EXPERIMENTS_PATH);

    static const std::array<std::pair<std::string, std::vector<tt>>, 1> gates = {{{"and", {create_and_tt()}}}};

    auto lyt =
        read_sqd_layout<sidb_100_cell_clk_lyt_cube>("/Users/jandrewniok/CLionProjects/fiction_fork/experiments/"
                                                    "skeleton_efficient_gate_designer/skeleton_hex_inputsdbp_2i1o.sqd");

    const design_sidb_gates_params<cell<sidb_100_cell_clk_lyt_cube>> params_2_in_1_out{
        is_operational_params{sidb_simulation_parameters{2, -0.32}},
        design_sidb_gates_params<cell<sidb_100_cell_clk_lyt_cube>>::design_sidb_gates_mode::EXHAUSTIVE,
        {{14, 12, 0}, {24, 23, 0}},
        3};

    const auto                  sidb_sim = sidb_simulation_parameters{2, -0.32, 5.6, 5.0};
    const is_operational_params is_op_params{sidb_sim};

    maximum_defect_influence_position_and_distance_params max_defect_params{};
    max_defect_params.defect =
        sidb_defect{fiction::sidb_defect_type::UNKNOWN, -1, is_op_params.simulation_parameters.epsilon_r,
                    is_op_params.simulation_parameters.lambda_tf};
    max_defect_params.additional_scanning_area = {100, 100};

    defect_influence_operational_domain_params defect_params{};
    defect_params.defect_influence_params = max_defect_params;
    defect_params.operational_params      = is_op_params;

    for (const auto& [gate, truth_table] : gates)
    {
        const auto exhaustive_design = design_sidb_gates(lyt, truth_table, params_2_in_1_out);
        std::cout << exhaustive_design.size() << std::endl;
        // Create gate directory for plots
        std::string gate_folder = fmt::format("{}{}/", output_folder, gate);
        std::filesystem::create_directories(gate_folder);

        uint64_t counter = 0;
        for (const auto& gate_lyt : exhaustive_design)
        {
            defect_influence_operational_domain_stats grid_stats{};
            const auto                                op_defect_grid =
                defect_influence_operational_domain_grid_search(gate_lyt, truth_table, 1, defect_params, &grid_stats);

            // Define file paths for the CSV and SQD
            const auto csv_path = fmt::format("{}{}_grid.csv", gate_folder, gate);
            const auto sqd_path = fmt::format("{}/{}.sqd", gate_folder, gate);

            // Write the CSV file
            write_defect_influence_operational_domain(op_defect_grid, csv_path);

            // Write the SQD layout
            // write_sqd_layout(layout, sqd_path);

            const auto avoidance_grid = defect_avoidance_distance(gate_lyt, op_defect_grid);

            defect_influence_operational_domain_stats random_stats{};
            const auto op_defect_random = defect_influence_operational_domain_random_sampling(
                gate_lyt, truth_table, 100, defect_params, &random_stats);
            const auto avoidance_random = defect_avoidance_distance(gate_lyt, op_defect_random);

            const auto csv_path_random = fmt::format("{}{}_random.csv", gate_folder, gate);
            write_defect_influence_operational_domain(op_defect_random, csv_path_random);

            defect_influence_operational_domain_stats contour_stats{};
            const auto op_defect_contour = defect_influence_operational_domain_contour_tracing(
                gate_lyt, truth_table, 100, defect_params, &contour_stats);
            const auto avoidance_contour = defect_avoidance_distance(gate_lyt, op_defect_contour);

            const auto csv_path_contour = fmt::format("{}{}_contour.csv", gate_folder, gate);
            write_defect_influence_operational_domain(op_defect_contour, csv_path_contour);
            //
            //            simulation_exp(gate, avoidance_grid.minimum_defect_clearance,
            //            grid_stats.num_evaluated_defect_positions,
            //                           avoidance_random.minimum_defect_clearance, random_stats.num_evaluated_defect_positions,
            //                           avoidance_contour.minimum_defect_clearance,
            //                           contour_stats.num_evaluated_defect_positions);
            //            simulation_exp.save();
            //            simulation_exp.table();

            if (std::abs(avoidance_grid.minimum_defect_clearance - avoidance_contour.minimum_defect_clearance) > 0.1)
            {  // Log the simulation results
                write_sqd_layout(gate_lyt, sqd_path);
                write_defect_influence_operational_domain(op_defect_contour, csv_path_contour);
                simulation_exp(gate, avoidance_grid.minimum_defect_clearance, grid_stats.num_evaluated_defect_positions,
                               avoidance_random.minimum_defect_clearance, random_stats.num_evaluated_defect_positions,
                               avoidance_contour.minimum_defect_clearance, contour_stats.num_evaluated_defect_positions);
                simulation_exp.save();
                simulation_exp.table();
            }
            counter++;
            std::cout << counter << std::endl;
        }
    }
    return EXIT_SUCCESS;
}
