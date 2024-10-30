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

int main()
{
    experiments::experiment<std::string, std::size_t, std::size_t, std::size_t, std::size_t> simulation_exp{
        "Benchmark", "Gate Name", "instance count", "#samples_grid", "#sample_contour", "#unequal"};

    static const std::string folder        = fmt::format("{}defect_avoidance_distance/fom_data/sqd/", EXPERIMENTS_PATH);
    static const std::string output_folder = fmt::format("{}defect_avoidance_distance/plots/", EXPERIMENTS_PATH);

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
        // Create gate directory for plots
        std::string gate_folder = fmt::format("{}{}/", output_folder, gate);
        std::filesystem::create_directories(gate_folder);

        std::size_t gate_counter           = 0;
        std::size_t unequal_counter        = 0;
        std::size_t sample_grid_counter    = 0;
        std::size_t sample_contour_counter = 0;

        bool start_with_new_boolean_function = false;

        for (const auto& file : std::filesystem::directory_iterator(fmt::format("{}{}", folder, gate)))
        {
            if (gate_counter == 100)
            {
                start_with_new_boolean_function = true;
                break;
            }
            const auto layout = read_sqd_layout<sidb_100_cell_clk_lyt_cube>(file.path().string());

            if (layout.num_cells() != 22)
            {
                continue;
            }
            gate_counter++;
            std::cout << gate_counter << std::endl;

            defect_influence_operational_domain_stats grid_stats{};
            const auto                                op_defect_grid =
                defect_influence_operational_domain_grid_search(layout, truth_table, 1, defect_params, &grid_stats);

            // Define file paths for the CSV and SQD
            const auto csv_path = fmt::format("{}{}_grid.csv", gate_folder, gate);
            const auto sqd_path = fmt::format("{}/{}.sqd", gate_folder, gate);

            // Write the CSV file
            write_defect_influence_operational_domain(op_defect_grid, csv_path);

            // Write the SQD layout
            write_sqd_layout(layout, sqd_path);

            const auto avoidance_grid = defect_avoidance_distance(layout, op_defect_grid);

            defect_influence_operational_domain_stats contour_stats{};
            const auto op_defect_contour = defect_influence_operational_domain_contour_tracing(
                layout, truth_table, 100, defect_params, &contour_stats);
            const auto avoidance_contour = defect_avoidance_distance(layout, op_defect_contour);

            const auto csv_path_contour = fmt::format("{}{}_contour.csv", gate_folder, gate);
            write_defect_influence_operational_domain(op_defect_contour, csv_path_contour);

            sample_grid_counter += grid_stats.num_evaluated_defect_positions;
            sample_contour_counter += contour_stats.num_evaluated_defect_positions;

            if (std::abs(avoidance_grid.minimum_defect_clearance - avoidance_contour.minimum_defect_clearance) > 0.1)
            {  // Log the simulation results
                unequal_counter++;
                std::cout << file.path().string() << std::endl;
            }
        }
        simulation_exp(gate, gate_counter, sample_grid_counter, sample_contour_counter, unequal_counter);
        simulation_exp.save();
        simulation_exp.table();
        if (start_with_new_boolean_function)
        {
            continue;
        }
    }
    return EXIT_SUCCESS;
}
