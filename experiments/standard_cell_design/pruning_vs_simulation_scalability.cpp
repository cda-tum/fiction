//
// Created by Jan Drewniok on 08.06.25.
//

#include "fiction_experiments.hpp"

#include <fiction/algorithms/iter/bdl_input_iterator.hpp>
#include <fiction/algorithms/simulation/sidb/detect_bdl_wires.hpp>
#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>
#include <fiction/io/read_sqd_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/types.hpp>

#include <fmt/format.h>
#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <chrono>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

using namespace fiction;

// This script compares the runtime of applying the three pruning techniques presented used in **QuickCell** with
// physical simulation to determine the non-operationiolity of different standard cell layouts.

// Use an anonymous namespace to ensure these functions are only visible within this translation unit.
namespace
{
/**
 * Creates and returns a vector of truth tables for a 2-input, 1-output function.
 *
 * This function generates a vector of truth tables, each representing the output of a 2-input, 1-output Boolean
 * function. The function returns a vector containing one truth table.
 *
 * @return Vector of truth tables, each representing an output of the 2-input, 1-output function.
 */
[[nodiscard]] inline static std::vector<kitty::dynamic_truth_table> create_truth_tables_2i1o()
{
    static constexpr const char* truth_table_string = "1110";

    kitty::dynamic_truth_table table{2};

    kitty::create_from_binary_string(table, truth_table_string);

    return std::vector<kitty::dynamic_truth_table>{table};
}

/**
 * Creates and returns a vector of truth tables for a 2-input, 2-output function.
 *
 * This function generates a vector of truth tables, each representing one of the outputs
 * of a 2-input, 2-output Boolean function. The function returns a vector containing
 * two truth tables.
 *
 * @return Vector of truth tables, each representing an output of the 2-input, 2-output function.
 */
[[nodiscard]] inline static std::vector<kitty::dynamic_truth_table> create_truth_tables_2i2o() noexcept
{
    static constexpr const char* truth_table_string1 = "1100";
    static constexpr const char* truth_table_string2 = "1010";

    kitty::dynamic_truth_table table1{2};
    kitty::dynamic_truth_table table2{2};

    kitty::create_from_binary_string(table1, truth_table_string1);
    kitty::create_from_binary_string(table2, truth_table_string2);

    return std::vector<kitty::dynamic_truth_table>{table1, table2};
}

/**
 * Creates and returns a vector of truth tables for a 3-input, 1-output function.
 *
 * This function generates a vector of truth tables, each representing the output of a 3-input, 1-output Boolean
 * function. The function returns a vector containing one truth table.
 *
 * @return Vector of truth tables, each representing an output of the 3-input, 1-output function.
 */
[[nodiscard]] inline static std::vector<kitty::dynamic_truth_table> create_truth_tables_3i1o() noexcept
{
    static constexpr const char* truth_table_string = "11100100";

    kitty::dynamic_truth_table table{3};

    kitty::create_from_binary_string(table, truth_table_string);

    return {table};
}

/**
 * Creates and returns a vector of truth tables for a 3-input, 2-output function.
 *
 * This function generates a vector of truth tables, each representing one of the outputs
 * of a 3-input, 2-output Boolean function. The function returns a vector containing
 * two truth tables.
 *
 * @return Vector of truth tables, each representing an output of the 3-input, 2-output function.
 */
[[nodiscard]] inline static std::vector<kitty::dynamic_truth_table> create_truth_tables_3i2o() noexcept
{
    static constexpr const char* truth_table_string1 = "11100100";
    static constexpr const char* truth_table_string2 = "11100100";

    kitty::dynamic_truth_table table1{3};
    kitty::dynamic_truth_table table2{3};

    kitty::create_from_binary_string(table1, truth_table_string1);
    kitty::create_from_binary_string(table2, truth_table_string2);

    return std::vector<kitty::dynamic_truth_table>{table1, table2};
}

/**
 * Creates and returns a vector of truth tables for a 3-input, 3-output function.
 *
 * This function generates a vector of truth tables, each representing one of the outputs
 * of a 3-input, 3-output Boolean function. The function returns a vector containing
 * three truth tables.
 *
 * @return Vector of truth tables, each representing an output of the 3-input, 3-output function.
 */
[[nodiscard]] inline static std::vector<kitty::dynamic_truth_table> create_truth_tables_3i3o() noexcept
{
    static constexpr const char* truth_table_string1 = "11100100";
    static constexpr const char* truth_table_string2 = "11100100";
    static constexpr const char* truth_table_string3 = "11100100";

    kitty::dynamic_truth_table table1{3};
    kitty::dynamic_truth_table table2{3};
    kitty::dynamic_truth_table table3{3};

    kitty::create_from_binary_string(table1, truth_table_string1);
    kitty::create_from_binary_string(table2, truth_table_string2);
    kitty::create_from_binary_string(table3, truth_table_string3);

    return std::vector<kitty::dynamic_truth_table>{table1, table2, table3};
}
}  // end anonymous namespace

int main()  // NOLINT
{
    experiments::experiment<std::string, double, double> simulation_exp{
        "benchmark",
        "gate",                       // std::string
        "runtime (simulation) [µs]",  // double
        "runtime (filtering) [µs]"    // double
    };

    static const std::string folder_3i = fmt::format("{}/gate_skeletons/skeleton_3_input/", EXPERIMENTS_PATH);

    static const std::string folder_2i =
        fmt::format("{}/gate_skeletons/skeleton_bestagons_with_tags/", EXPERIMENTS_PATH);

    const auto skeleton_2i1o = read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(
        fmt::format("{}/{}", folder_2i, "skeleton_hex_inputsdbp_2i1o.sqd"));

    const auto skeleton_2i2o = read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(
        fmt::format("{}/{}", folder_2i, "skeleton_hex_inputsdbp_2i2o.sqd"));

    const auto skeleton_3i1o =
        read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(fmt::format("{}/{}", folder_3i, "3_in_1_out_skeleton_one.sqd"));

    const auto skeleton_3i2o =
        read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(fmt::format("{}/{}", folder_3i, "3_in_2_out_skeleton.sqd"));

    const auto skeleton_3i3o =
        read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(fmt::format("{}/{}", folder_3i, "3_in_3_out_skeleton.sqd"));

    auto two_one = skeleton_2i1o;
    two_one.assign_cell_type({22, 10}, sidb_technology::cell_type::LOGIC);
    two_one.assign_cell_type({17, 7}, sidb_technology::cell_type::LOGIC);

    auto two_two = skeleton_2i2o;
    two_two.assign_cell_type({15, 7}, sidb_technology::cell_type::LOGIC);
    two_two.assign_cell_type({22, 8}, sidb_technology::cell_type::LOGIC);
    two_two.assign_cell_type({22, 10}, sidb_technology::cell_type::LOGIC);
    two_two.assign_cell_type({17, 7}, sidb_technology::cell_type::LOGIC);

    auto three_one = skeleton_3i1o;
    three_one.assign_cell_type({25, 7}, sidb_technology::cell_type::LOGIC);
    three_one.assign_cell_type({32, 8}, sidb_technology::cell_type::LOGIC);
    three_one.assign_cell_type({36, 11}, sidb_technology::cell_type::LOGIC);
    three_one.assign_cell_type({40, 7}, sidb_technology::cell_type::LOGIC);

    auto three_two = skeleton_3i2o;
    three_two.assign_cell_type({25, 7}, sidb_technology::cell_type::LOGIC);
    three_two.assign_cell_type({32, 8}, sidb_technology::cell_type::LOGIC);
    three_two.assign_cell_type({36, 11}, sidb_technology::cell_type::LOGIC);
    three_two.assign_cell_type({40, 7}, sidb_technology::cell_type::LOGIC);

    auto three_three = skeleton_3i3o;
    three_three.assign_cell_type({25, 7}, sidb_technology::cell_type::LOGIC);
    three_three.assign_cell_type({32, 8}, sidb_technology::cell_type::LOGIC);
    three_three.assign_cell_type({36, 11}, sidb_technology::cell_type::LOGIC);
    three_three.assign_cell_type({40, 7}, sidb_technology::cell_type::LOGIC);

    const std::vector<std::pair<sidb_100_cell_clk_lyt_siqad, std::vector<kitty::dynamic_truth_table>>> layouts_and_tt{
        {two_one, create_truth_tables_2i1o()},
        {two_two, create_truth_tables_2i2o()},
        {three_one, create_truth_tables_3i1o()},
        {three_two, create_truth_tables_3i2o()},
        {three_three, create_truth_tables_3i3o()}};

    const std::vector<std::string> layout_names{"2i1o", "2i2o", "3i1o", "3i2o", "3i3o"};

    is_operational_params operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT,
                                             bdl_input_iterator_params{detect_bdl_wires_params{3.0}},
                                             is_operational_params::operational_condition::REJECT_KINKS,
                                             is_operational_params::operational_analysis_strategy::SIMULATION_ONLY};

    for (size_t i = 0; i < layouts_and_tt.size(); ++i)
    {
        const auto& [layout, truth_table] = layouts_and_tt[i];

        // Set to SIMULATION_ONLY and measure time
        operational_params.strategy_to_analyze_operational_status =
            is_operational_params::operational_analysis_strategy::SIMULATION_ONLY;

        mockturtle::stopwatch<>::duration simulation_only_runtime{};
        {
            // measure run time (artificial scope)
            const mockturtle::stopwatch stop{simulation_only_runtime};
            [[maybe_unused]] const auto op_sim = is_operational(layout, truth_table, operational_params);
        }
        // Set to FILTER_ONLY and measure time
        operational_params.strategy_to_analyze_operational_status =
            is_operational_params::operational_analysis_strategy::FILTER_ONLY;

        mockturtle::stopwatch<>::duration filter_only_runtime{};
        {
            // measure run time (artificial scope)
            const mockturtle::stopwatch stop{filter_only_runtime};
            [[maybe_unused]] const auto op_filt = is_operational(layout, truth_table, operational_params);
        }

        simulation_exp(
            layout_names[i],
            static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(simulation_only_runtime).count()),
            static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(filter_only_runtime).count()));

        simulation_exp.save();
        simulation_exp.table();
    }

    return EXIT_SUCCESS;
}
