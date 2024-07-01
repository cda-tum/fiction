//
// Created by Jan Drewniok 01.01.23
//

// pre-defined types suitable for the FCN domain
#include "fiction_experiments.hpp"
#include "mockturtle/utils/stopwatch.hpp"

#include <fiction/algorithms/physical_design/design_sidb_gates.hpp>
#include <fiction/algorithms/simulation/sidb/assess_physical_population_stability.hpp>
#include <fiction/algorithms/simulation/sidb/calculate_min_potential_for_charge_change_for_all_input_combinations.hpp>
#include <fiction/algorithms/simulation/sidb/critical_temperature.hpp>
#include <fiction/algorithms/simulation/sidb/defect_avoidance_distance.hpp>
#include <fiction/algorithms/simulation/sidb/defect_influence_operational_domain.hpp>
#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>
#include <fiction/algorithms/simulation/sidb/quickexact.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/io/print_layout.hpp>
#include <fiction/io/read_sqd_layout.hpp>
#include <fiction/io/write_defect_influence_operational_domain.hpp>
#include <fiction/io/write_operational_domain.hpp>
#include <fiction/io/write_sqd_layout.hpp>
#include <fiction/technology/sidb_defects.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/math_utils.hpp>
#include <fiction/utils/truth_table_utils.hpp>

#include <fmt/core.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <mutex>
#include <numeric>
#include <string>
#include <thread>
#include <vector>

using namespace fiction;

int main()  // NOLINT
{
    using Lyt = sidb_100_cell_clk_lyt_cube;

    static const std::string folder =
        fmt::format("{}skeleton_bestagons_with_tags/skeleton_hex_inputsdbp_2i1o.sqd", EXPERIMENTS_PATH);

    experiments::experiment<std::string, double, double, double, double, double> fom_exp{
        "FOM", "benchmark", "CT max", "OPD max", "MDC_ars_min", "MDC_vac_max", "BBR"};

    const auto skeleton = read_sqd_layout<Lyt>(folder);

    const auto sim_params = sidb_simulation_parameters{2, -0.32};

    design_sidb_gates_params<fiction::cell<Lyt>> params{sim_params, {{17, 14, 0}, {21, 22, 0}}, 2};

    const auto truth_tables = std::vector<std::vector<tt>>{
        std::vector<tt>{create_and_tt()}, std::vector<tt>{create_nand_tt()}, std::vector<tt>{create_or_tt()},
        std::vector<tt>{create_nor_tt()}, std::vector<tt>{create_xor_tt()},  std::vector<tt>{create_xnor_tt()},
        std::vector<tt>{create_lt_tt()},  std::vector<tt>{create_gt_tt()},   std::vector<tt>{create_le_tt()},
        std::vector<tt>{create_ge_tt()}};

    const auto gate_names = std::vector<std::string>{"and", "nand", "or", "nor", "xor", "xnor", "lt", "gt", "le", "ge"};

    const critical_temperature_params ct_params{sim_params};

    // defining the operational domain parameters
    operational_domain_params op_domain_params{sim_params};

    // setting the operational domain range
    op_domain_params.x_min  = 4.0;
    op_domain_params.x_max  = 6;
    op_domain_params.x_step = 0.2;

    op_domain_params.y_min  = 4.0;
    op_domain_params.y_max  = 6;
    op_domain_params.y_step = 0.2;

    const calculate_min_potential_for_charge_change_for_all_input_combinations_params assess_params{
        assess_physical_population_stability_params{sim_params}};

    const maximum_defect_influence_position_and_distance_params defect_avoidance_params_arsenic{
        sidb_defect{sidb_defect_type::UNKNOWN, 1, 9.7, 2.1},
        sim_params,
        {30, 20}};

    const maximum_defect_influence_position_and_distance_params defect_avoidance_params_vacancy{
        sidb_defect{sidb_defect_type::SI_VACANCY, -1, 10.6, 5.9},
        sim_params,
        {30, 20}};

    uint64_t counter = 0;
    for (const auto& truth_table : truth_tables)
    {
        for (auto num_sidbs = 2u; num_sidbs < 6; num_sidbs++)
        {
            std::cout << fmt::format("num sidbs: {}", num_sidbs) << '\n';
            params.number_of_sidbs = num_sidbs;

            mockturtle::stopwatch<>::duration time_total{};

            std::vector<double> temps                             = {};
            std::vector<double> op_domains                        = {};
            std::vector<double> defect_influence_arsenic          = {};
            std::vector<double> defect_influence_vacancy          = {};
            std::vector<double> pop_stability_neutral_to_negative = {};
            std::vector<double> pop_stability_negative_to_neutral = {};
            std::vector<double> bbr_all                           = {};
            std::vector<double> runtime                           = {};
            {
                mockturtle::stopwatch stop{time_total};

                const auto all_gate = design_sidb_gates<Lyt, tt>(skeleton, truth_table, params);

                std::cout << fmt::format("Number of gates: {}", all_gate.size()) << '\n';

                for (const auto& gate : all_gate)
                {
                    temps.push_back(critical_temperature_gate_based(gate, truth_table, ct_params));
                    operational_domain_stats op_stats{};
                    const auto               op_domain = operational_domain_flood_fill(
                        gate, truth_table, 0, op_domain_params, operational_domain::parameter_point{5.6, 5}, &op_stats);
                    op_domains.push_back(op_stats.percentual_operational_area);

                    defect_influence_operational_domain_stats arsenic_stats{};
                    const auto defect_influence_domain_arsenic = defect_influence_operational_domain_grid_search(
                        gate, truth_table, 2,
                        defect_influence_operational_domain_params{
                            defect_avoidance_params_arsenic,
                            is_operational_params{defect_avoidance_params_arsenic.simulation_parameters}},
                        &arsenic_stats);
                    std::cout << fmt::format("runtime: {}", mockturtle::to_seconds(arsenic_stats.time_total)) << '\n';
                    runtime.push_back(mockturtle::to_seconds(arsenic_stats.time_total));

                    defect_influence_arsenic.push_back(
                        defect_avoidance_distance(gate, defect_influence_domain_arsenic).max_min_distance);

                    defect_influence_operational_domain_stats vacancy_stats{};
                    const auto defect_influence_domain_vacancy = defect_influence_operational_domain_grid_search(
                        gate, truth_table, 2,
                        defect_influence_operational_domain_params{
                            defect_avoidance_params_vacancy,
                            is_operational_params{defect_avoidance_params_arsenic.simulation_parameters}},
                        &vacancy_stats);
                    std::cout << fmt::format("runtime: {}", mockturtle::to_seconds(arsenic_stats.time_total)) << '\n';

                    defect_influence_vacancy.push_back(
                        defect_avoidance_distance(gate, defect_influence_domain_vacancy).max_min_distance);

                    pop_stability_neutral_to_negative.push_back(
                        calculate_min_potential_for_charge_change_for_all_input_combinations(
                            gate, truth_table,
                            calculate_min_potential_for_charge_change_for_all_input_combinations_params{assess_params},
                            -1) *
                        1000);

                    const auto bbr_neu_to_neg =
                        calculate_min_potential_for_charge_change_for_all_input_combinations(
                            gate, truth_table,
                            calculate_min_potential_for_charge_change_for_all_input_combinations_params{assess_params},
                            1) *
                        1000;

                    pop_stability_negative_to_neutral.push_back(
                        calculate_min_potential_for_charge_change_for_all_input_combinations(
                            gate, truth_table,
                            calculate_min_potential_for_charge_change_for_all_input_combinations_params{assess_params},
                            1) *
                        1000);
                    const auto bbr_neg_to_neu =
                        calculate_min_potential_for_charge_change_for_all_input_combinations(
                            gate, truth_table,
                            calculate_min_potential_for_charge_change_for_all_input_combinations_params{assess_params},
                            1) *
                        1000;

                    bbr_all.push_back(std::min(bbr_neu_to_neg, bbr_neg_to_neu));
                }

                if (temps.size() != 0)
                {
                    const auto max_temp      = *std::max_element(temps.begin(), temps.end());
                    const auto max_op_domain = *std::max_element(op_domains.begin(), op_domains.end());
                    const auto max_defect_influence_arsenic =
                        *std::min_element(defect_influence_arsenic.begin(), defect_influence_arsenic.end());
                    const auto max_defect_influence_vacancy =
                        *std::min_element(defect_influence_vacancy.begin(), defect_influence_vacancy.end());
                    const auto max_pop_stability_neutral_to_negative = *std::max_element(
                        pop_stability_neutral_to_negative.begin(), pop_stability_neutral_to_negative.end());
                    const auto max_pop_stability_negative_to_neutral = *std::max_element(
                        pop_stability_negative_to_neutral.begin(), pop_stability_negative_to_neutral.end());

                    const auto bbr_max =
                        std::max(max_pop_stability_neutral_to_negative, max_pop_stability_negative_to_neutral);

                    // log results
                    fom_exp(gate_names[counter], max_temp, max_op_domain, 0, 0, bbr_max);

                    fom_exp.save();
                    fom_exp.table();
                    for (auto i = 0; i < temps.size(); i++)
                    {
                        cost_function_chi({temps[i] / max_temp, op_domains[i] / max_op_domain,
                                           defect_influence_arsenic[i] / max_defect_influence_arsenic,
                                           defect_influence_vacancy[i] / max_defect_influence_vacancy,
                                           bbr_all[i] / bbr_max},
                                          {-1, -1, 1, 1, -1});
                    }
                }
            }
        }
        counter++;
    }
    return EXIT_SUCCESS;
}
