//
// Created by Jan Drewniok 01.01.23
//

#include "fiction_experiments.hpp"

#include <fiction/algorithms/physical_design/design_sidb_gates.hpp>
#include <fiction/algorithms/simulation/sidb/band_bending_resilience.hpp>
#include <fiction/algorithms/simulation/sidb/critical_temperature.hpp>
#include <fiction/algorithms/simulation/sidb/defect_clearance.hpp>
#include <fiction/algorithms/simulation/sidb/defect_influence.hpp>
#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>
#include <fiction/algorithms/simulation/sidb/physical_population_stability.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/io/read_sqd_layout.hpp>
#include <fiction/technology/sidb_defects.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/math_utils.hpp>
#include <fiction/utils/truth_table_utils.hpp>

#include <fmt/core.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>

using namespace fiction;

// This script summarizes all experiments presented in \"Unifying Figures of Merit: A Versatile Cost Function for
// Silicon Dangling Bond Logic\" by J. Drewniok, M. Walter, S. S. H. Ng, K. Walus, and R. Wille in IEEE-NANO 2024
// (https://ieeexplore.ieee.org/abstract/document/10628671).

int main()  // NOLINT
{
    using Lyt = sidb_100_cell_clk_lyt_cube;

    // 2-input/1-output gate skeleton for the experiments. It is used to design gates with 2 inputs and 1 output.
    static const auto skeleton = read_sqd_layout<Lyt>(fmt::format(
        "{}/gate_skeletons/skeleton_bestagons_with_tags/skeleton_hex_inputsdbp_2i1o.sqd", EXPERIMENTS_PATH));

    // This table is used to explore the figures of merit for 2-input/1-output SiDB gates.
    experiments::experiment<std::string, std::size_t, double, double, double, double, double> fom_exploration{
        "Exploration", "gate", "#canvas SiDBs", "CT max", "OPD max", "MDC_ars_min", "MDC_vac_min", "BBR"};

    // This table is used to explore the minimal cost of 2-input/1-output SiDB gates.
    experiments::experiment<std::string, std::size_t, double, double, double, double, double, double> minimal_cost{
        "Minimal Cost", "gate", "#canvas SiDBs", "CT", "OPD", "MDC_arsenic", "MDC_vacancy", "BBR", "X_custom,min"};

    const auto op_params     = is_operational_params{sidb_simulation_parameters{2, -0.32}};
    auto       design_params = design_sidb_gates_params<cell<Lyt>>{};

    design_params.operational_params = op_params;
    design_params.design_mode =
        design_sidb_gates_params<cell<Lyt>>::design_sidb_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER;
    design_params.canvas                 = {{17, 14, 0}, {21, 22, 0}};
    design_params.number_of_canvas_sidbs = 2;
    design_params.termination_cond =
        design_sidb_gates_params<cell<Lyt>>::termination_condition::ALL_COMBINATIONS_ENUMERATED;
    // QuickExact was used for the paper. However, ClusterComplete is more efficient and faster but does not influence
    // the results.
    design_params.operational_params.sim_engine = sidb_simulation_engine::CLUSTERCOMPLETE;

    static const std::array<std::pair<std::string, std::vector<tt>>, 10> gates = {
        std::make_pair("and", std::vector<tt>{create_and_tt()}),
        std::make_pair("nand", std::vector<tt>{create_nand_tt()}),
        std::make_pair("nor", std::vector<tt>{create_nor_tt()}),
        std::make_pair("xnor", std::vector<tt>{create_xnor_tt()}),
        std::make_pair("xor", std::vector<tt>{create_xor_tt()}),
        std::make_pair("or", std::vector<tt>{create_or_tt()}),
        std::make_pair("wire", std::vector<tt>{create_id_tt()}),
        std::make_pair("wire_diag", std::vector<tt>{create_id_tt()}),
        std::make_pair("inv", std::vector<tt>{create_not_tt()}),
        std::make_pair("inv_diag", std::vector<tt>{create_not_tt()})};

    const critical_temperature_params ct_params{op_params};

    // defining the operational domain parameters
    operational_domain_params op_domain_params{op_params};

    op_domain_params.sweep_dimensions = {{sweep_parameter::EPSILON_R}, {sweep_parameter::LAMBDA_TF}};

    op_domain_params.sweep_dimensions[0].min  = 4.0;
    op_domain_params.sweep_dimensions[0].max  = 6.0;
    op_domain_params.sweep_dimensions[0].step = 0.2;
    op_domain_params.sweep_dimensions[1].min  = 4.0;
    op_domain_params.sweep_dimensions[1].max  = 6.0;
    op_domain_params.sweep_dimensions[1].step = 0.2;

    const band_bending_resilience_params bbr_params{
        physical_population_stability_params{op_params.simulation_parameters}};

    // for this experiment, we use two different defects: a vacancy in the Si lattice and an arsenic atom.
    const auto si_vacancy = fiction::sidb_defect{fiction::sidb_defect_type::SI_VACANCY, -1, 10.6, 5.9};
    const auto arsenic    = fiction::sidb_defect{fiction::sidb_defect_type::UNKNOWN, 1, 9.7, 2.1};

    const std::vector<sidb_defect> defects = {si_vacancy, arsenic};

    defect_influence_params<fiction::cell<sidb_100_cell_clk_lyt_cube>> params{};
    params.additional_scanning_area = {20, 20};
    params.defect                   = si_vacancy;
    params.operational_params       = op_params;

    for (const auto& [gate_name, truth_table] : gates)
    {
        std::size_t number_of_canvas_sidbs_to_minimize_chi   = 0;
        double      ct_to_minimize_chi                       = 0.0;
        double      op_domain_to_minimize_chi                = 0.0;
        double      defect_clearance_arsenic_to_minimize_chi = 0.0;
        double      defect_clearance_vacancy_to_minimize_chi = 0.0;
        double      bbr_to_minimize_chi                      = 0.0;
        double      minimized_chi                            = std::numeric_limits<double>::max();

        for (std::size_t num_sidbs = 2u; num_sidbs < 3; num_sidbs++)
        {
            design_params.number_of_canvas_sidbs = num_sidbs;

            std::vector<double> temps                    = {};
            std::vector<double> op_domains               = {};
            std::vector<double> defect_influence_arsenic = {};
            std::vector<double> defect_influence_vacancy = {};
            std::vector<double> bbr_all                  = {};

            std::vector<Lyt>        all_gates{};
            design_sidb_gates_stats efficent_stats{};

            all_gates = design_sidb_gates(skeleton, truth_table, design_params, &efficent_stats);

            if (all_gates.empty())
            {
                continue;
            }

            auto max_temp                     = 0.0;
            auto max_bbr                      = 0.0;
            auto min_defect_clearance_arsenic = std::numeric_limits<double>::max();
            auto max_defect_clearance_arsenic = 0.0;
            auto min_defect_clearance_vacancy = std::numeric_limits<double>::max();
            auto max_defect_clearance_vacancy = 0.0;
            auto max_relative_op_domain       = 0.0;

            for (const auto& gate : all_gates)
            {
                operational_domain_stats op_stats{};
                const auto op_domain = operational_domain_grid_search(gate, truth_table, op_domain_params, &op_stats);
                const auto percentual_op_area = static_cast<double>(op_stats.num_operational_parameter_combinations) /
                                                static_cast<double>(op_stats.num_total_parameter_points);
                op_domains.push_back(percentual_op_area);

                max_relative_op_domain = std::max(percentual_op_area, max_relative_op_domain);

                const auto ct = critical_temperature_gate_based(gate, truth_table, ct_params);
                temps.push_back(ct);
                max_temp                    = std::max(ct, max_temp);
                const auto bbr_in_volt      = band_bending_resilience(gate, truth_table, bbr_params);
                const auto bbr_in_millivolt = bbr_in_volt * 1000;  // convert to mV
                bbr_all.push_back(bbr_in_millivolt);
                max_bbr = std::max(bbr_in_millivolt, max_bbr);

                for (const auto& defect : defects)
                {
                    params.defect = defect;
                    defect_influence_stats quicktrace_stats{};
                    const auto             defect_inf_quicktrace =
                        defect_influence_grid_search(gate, truth_table, params, 4, &quicktrace_stats);
                    const auto clearance_quicktrace = calculate_defect_clearance(gate, defect_inf_quicktrace);

                    if (defect.type == sidb_defect_type::SI_VACANCY)
                    {
                        min_defect_clearance_vacancy =
                            std::min(clearance_quicktrace.defect_clearance_distance, min_defect_clearance_vacancy);
                        max_defect_clearance_vacancy =
                            std::max(clearance_quicktrace.defect_clearance_distance, max_defect_clearance_vacancy);
                        defect_influence_vacancy.push_back(clearance_quicktrace.defect_clearance_distance);
                    }
                    else if (defect.type == sidb_defect_type::UNKNOWN)
                    {
                        min_defect_clearance_arsenic =
                            std::min(clearance_quicktrace.defect_clearance_distance, min_defect_clearance_arsenic);
                        max_defect_clearance_arsenic =
                            std::max(clearance_quicktrace.defect_clearance_distance, max_defect_clearance_arsenic);
                        defect_influence_arsenic.push_back(clearance_quicktrace.defect_clearance_distance);
                    }
                }
            }

            std::cout << fmt::format("Gate design for {} with {} canvas SiDBs done.\n", gate_name, num_sidbs);

            fom_exploration(gate_name, static_cast<uint8_t>(num_sidbs), max_temp, max_relative_op_domain,
                            min_defect_clearance_arsenic, min_defect_clearance_vacancy, max_bbr);

            fom_exploration.save();

            auto     min_chi       = std::numeric_limits<double>::max();
            uint64_t index_min_chi = 0;

            // these values are used in the paper to weight all figures of merit equally
            const auto w_ct             = -1.0;
            const auto w_op_domain      = -1.0;
            const auto w_defect_arsenic = 1.0;
            const auto w_defect_vacancy = 1.0;
            const auto w_bbr            = -1.0;

            for (auto i = 0u; i < temps.size(); i++)
            {
                const auto chi = cost_function_chi({temps.at(i) / max_temp, op_domains[i] / max_relative_op_domain,
                                                    defect_influence_arsenic.at(i) / max_defect_clearance_arsenic,
                                                    defect_influence_vacancy.at(i) / max_defect_clearance_vacancy,
                                                    bbr_all.at(i) / max_bbr},
                                                   {w_ct, w_op_domain, w_defect_arsenic, w_defect_vacancy, w_bbr});

                if (chi < min_chi)
                {
                    min_chi       = chi;
                    index_min_chi = i;
                }
            }

            if (min_chi < minimized_chi)
            {
                number_of_canvas_sidbs_to_minimize_chi   = num_sidbs;
                ct_to_minimize_chi                       = temps.at(index_min_chi);
                op_domain_to_minimize_chi                = op_domains.at(index_min_chi);
                defect_clearance_arsenic_to_minimize_chi = defect_influence_arsenic.at(index_min_chi);
                defect_clearance_vacancy_to_minimize_chi = defect_influence_vacancy.at(index_min_chi);
                bbr_to_minimize_chi                      = bbr_all.at(index_min_chi);
                minimized_chi                            = min_chi;
            }
        }

        fom_exploration.table();

        minimal_cost(gate_name, number_of_canvas_sidbs_to_minimize_chi, ct_to_minimize_chi, op_domain_to_minimize_chi,
                     defect_clearance_arsenic_to_minimize_chi, defect_clearance_vacancy_to_minimize_chi,
                     bbr_to_minimize_chi, minimized_chi);

        minimal_cost.save();
        minimal_cost.table();
    }

    return EXIT_SUCCESS;
}
