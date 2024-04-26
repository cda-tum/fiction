//
// Created by Jan Drewniok 01.01.23
//

#include "fiction/types.hpp"  // pre-defined types suitable for the FCN domain

#include <fiction/algorithms/physical_design/design_sidb_gates.hpp>
#include <fiction/algorithms/simulation/sidb/assess_physical_population_stability.hpp>
#include <fiction/algorithms/simulation/sidb/assess_physical_population_stability_sidb_gate.hpp>
#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/algorithms/simulation/sidb/maximum_defect_influence_position_and_distance_of_sidb_gate.hpp>
#include <fiction/algorithms/simulation/sidb/quickexact.hpp>
#include <fiction/io/print_layout.hpp>
#include <fiction/io/read_sqd_layout.hpp>
#include <fiction/io/write_sqd_layout.hpp>
#include <fiction/utils/truth_table_utils.hpp>

#include <array>

using namespace fiction;

int main()  // NOLINT
{
    const auto skeleton = read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(
        "/Users/jandrewniok/CLionProjects/fiction_copy/fiction/experiments/"
        "skeleton_bestagons_with_tags/skeleton_hex_inputsdbp_2i1o.sqd");

    // toffoli
    //    const design_sidb_gates_params<sidb_cell_clk_lyt_cube> params{
    //        sidb_simulation_parameters{2, -0.32},
    //        design_sidb_gates_params<sidb_cell_clk_lyt_cube>::design_sidb_gates_mode::RANDOM,
    //        {{9, 0, 0}, {19, 14, 0}},
    //        6,
    //        sidb_simulation_engine::QUICKEXACT};

    design_sidb_gates_params<sidb_cell_clk_lyt_siqad> params{
        sidb_simulation_parameters{2, -0.32},
        {{17, 7, 0}, {21, 11, 0}},
        3,
        sidb_simulation_engine::QUICKEXACT,
        100};

    //    const auto truth_tables = std::vector<std::vector<tt>>{
    //        std::vector<tt>{create_and_tt()},  std::vector<tt>{create_xor_tt()}, std::vector<tt>{create_or_tt()},
    //        std::vector<tt>{create_xnor_tt()}, std::vector<tt>{create_nor_tt()}, std::vector<tt>{create_nand_tt()},
    //        std::vector<tt>{create_lt_tt()},   std::vector<tt>{create_gt_tt()},  std::vector<tt>{create_le_tt()},
    //        std::vector<tt>{create_ge_tt()}};

    const auto truth_tables = std::vector<std::vector<tt>>{std::vector<tt>{create_and_tt()}};

    //    const auto truth_tables
    //        std::vector<std::vector<tt>>{std::vector<tt>{create_id_tt()}, std::vector<tt>{create_not_tt()}};

    const critical_temperature_params ct_params{sidb_simulation_parameters{2, params.simulation_parameters.mu_minus,
                                                                           params.simulation_parameters.epsilon_r,
                                                                           params.simulation_parameters.lambda_tf}};

    operational_domain_params op_domain_params{sidb_simulation_parameters{2, params.simulation_parameters.mu_minus,
                                                                          params.simulation_parameters.epsilon_r,
                                                                          params.simulation_parameters.lambda_tf}};
    op_domain_params.x_min  = 4.0;
    op_domain_params.x_max  = 6;
    op_domain_params.x_step = 0.2;

    op_domain_params.y_min  = 4.0;
    op_domain_params.y_max  = 6;
    op_domain_params.y_step = 0.2;

    design_sidb_gates_sa_params sa_params{};

    sa_params.ct_params = ct_params;
    sa_params.op_params = op_domain_params;

    uint64_t truth_counter = 0;

    for (const auto& truth : truth_tables)
    {
        std::cout << fmt::format("{} truth table", truth_counter) << std::endl;
        for (auto i = 3u; i < 4; i++)
        {
            double total_time = 0;
            double net_cost   = 0;
            std::cout << fmt::format("{} placed SiDBs", i) << std::endl;
            sidb_cell_clk_lyt_siqad gate{};
            for (auto t = 0u; t < 1; t++)
            {
                params.number_of_sidbs = i;

                design_sidb_gates_stats stats{};

                gate = design_sidb_gates_metric_driven_simulated_annealing<sidb_cell_clk_lyt_siqad, tt>(
                    skeleton, truth, params, sa_params, &stats);
                net_cost += stats.cost_of_gate;
                total_time += mockturtle::to_seconds(stats.time_total);
            }
            const is_operational_params params_is_operational{params.simulation_parameters, params.sim_engine};
            auto result = is_operational(gate, truth, params_is_operational).first == operational_status::OPERATIONAL ?
                              "operational\n" :
                              "non_operational\n";
            std::cout << result;

            std::cout << fmt::format("average runtime: {}", total_time / 1) << std::endl;
            std::cout << fmt::format("net cost: {}", net_cost / 1) << std::endl;
            write_sqd_layout<sidb_cell_clk_lyt_siqad>(
                gate, "/Users/jandrewniok/CLionProjects/fiction_copy/fiction/experiments/gate_design_sa/" +
                          std::to_string(truth_counter) + ".sqd");
        }

        std::cout << "-----------" << std::endl;
        truth_counter++;
    }

    return EXIT_SUCCESS;
}
