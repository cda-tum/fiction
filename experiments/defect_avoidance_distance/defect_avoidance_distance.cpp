#include "fiction/algorithms/simulation/sidb/defect_avoidance_distance.hpp"
#include "fiction/algorithms/simulation/sidb/defect_influence_operational_domain.hpp"
#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/algorithms/simulation/sidb/maximum_defect_influence_position_and_distance.hpp"
#include "fiction/io/read_sqd_layout.hpp"
#include "fiction/io/write_defect_influence_operational_domain.hpp"
#include "fiction/io/write_sqd_layout.hpp"
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

int main() {
    experiments::experiment<std::string, double, std::size_t, double, std::size_t, double, std::size_t> simulation_exp{
        "Benchmark", "Gate Name", "distance_grid_search", "#samples",
        "distance_random", "#samples", "distance_contour", "#samples",
    };

    static const std::string folder = fmt::format("{}bestagon_gates_type_tags/", EXPERIMENTS_PATH);
    static const std::string output_folder = fmt::format("{}defect_avoidance_distance/plots/", EXPERIMENTS_PATH);

    static const std::array<std::pair<std::string, std::vector<tt>>, 9> gates = {{
        {"and", {create_and_tt()}},
        {"xor", {create_xor_tt()}},
        {"or", {create_or_tt()}},
        {"xnor", {create_xnor_tt()}},
        {"fo2", {create_fan_out_tt()}},
        {"nor", {create_nor_tt()}},
        {"nand", {create_nand_tt()}},
        {"inv", {create_not_tt()}},
        {"wire", {create_id_tt()}},
    }};

    const auto sidb_sim = sidb_simulation_parameters{2, -0.32, 5.6, 5.0};
    const is_operational_params is_op_params{sidb_sim};

    maximum_defect_influence_position_and_distance_params max_defect_params{};
    max_defect_params.defect = sidb_defect{
        fiction::sidb_defect_type::UNKNOWN, -1, is_op_params.simulation_parameters.epsilon_r,
        is_op_params.simulation_parameters.lambda_tf
    };
    max_defect_params.additional_scanning_area = {50, 50};

    defect_influence_operational_domain_params defect_params{};
    defect_params.defect_influence_params = max_defect_params;
    defect_params.operational_params = is_op_params;

    for (const auto& [gate, truth_table] : gates) {
        // Create gate directory for plots
        std::string gate_folder = fmt::format("{}{}/", output_folder, gate);
        std::filesystem::create_directories(gate_folder);

        for (const auto& file : std::filesystem::directory_iterator(fmt::format("{}{}", folder, gate))) {
            const auto layout = read_sqd_layout<sidb_100_cell_clk_lyt_cube>(file.path().string());

            defect_influence_operational_domain_stats grid_stats{};
            const auto op_defect_grid_search = defect_influence_operational_domain_grid_search(
                layout, truth_table, 1, defect_params, &grid_stats);

            // Define file paths for the CSV and SQD
            const auto csv_path = fmt::format("{}{}_grid_search.csv", gate_folder, gate);
            const auto sqd_path = fmt::format("{}/{}.sqd", gate_folder, gate);

            // Write the CSV file
            write_defect_influence_operational_domain(op_defect_grid_search, csv_path);

            // Write the SQD layout
            write_sqd_layout(layout, sqd_path);

            const auto avoidance_grid = defect_avoidance_distance(layout, op_defect_grid_search);

            defect_influence_operational_domain_stats random_stats{};
            const auto op_defect_random_search = defect_influence_operational_domain_random_sampling(
                layout, truth_table, 100, defect_params, &random_stats);
            const auto avoidance_random = defect_avoidance_distance(layout, op_defect_random_search);

            const auto csv_path_random = fmt::format("{}{}_random.csv", gate_folder, gate);
            write_defect_influence_operational_domain(op_defect_random_search, csv_path_random);

            defect_influence_operational_domain_stats contour_stats{};
            const auto op_defect_contour_search = defect_influence_operational_domain_contour_tracing(
                layout, truth_table, 1, defect_params, &contour_stats);
            const auto avoidance_contour = defect_avoidance_distance(layout, op_defect_contour_search);

            const auto csv_path_contour = fmt::format("{}{}_contour.csv", gate_folder, gate);
            write_defect_influence_operational_domain(op_defect_contour_search, csv_path_contour);

            // Log the simulation results
            simulation_exp(gate, avoidance_grid.max_min_distance, grid_stats.num_evaluated_defect_positions,
                           avoidance_random.max_min_distance, random_stats.num_evaluated_defect_positions,
                           avoidance_contour.max_min_distance, contour_stats.num_evaluated_defect_positions);
            simulation_exp.save();
            simulation_exp.table();
        }
    }
    return EXIT_SUCCESS;
}
