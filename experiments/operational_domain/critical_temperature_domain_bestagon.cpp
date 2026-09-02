/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include "fiction_experiments.hpp"

#include <fiction/synthesis/truth_tables.hpp>
#include <fiction/technology/sidb/io/read_sqd_layout.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/analysis/critical_temperature.hpp>
#include <fiction/technology/sidb/simulation/engine.hpp>
#include <fiction/technology/sidb/simulation/logic/is_operational.hpp>
#include <fiction/technology/sidb/simulation/logic/operational_domain.hpp>
#include <fiction/types.hpp>

#include <fmt/format.h>

#include <array>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

using namespace fiction;

// This script analyzes the critical temperature within the operational domain of the Bestagon gates. It calculates
// the operational domain at various temperatures relative to the total operational domain at 0 K.

int main()  // NOLINT
{
    experiments::experiment<std::string, uint64_t, double, double, double, double> opdomain_exp{
        "Critical Temperature Domain Bestagon",
        "Name",
        "#SiDBs",  // Benchmark
        "CT min",
        "CT (default physical values)",
        "CT max",
        "ΔCT"};

    // simulation parameters
    sidb::model::simulation_parameters sim_params{};
    sim_params.base     = 2;
    sim_params.mu_minus = -0.32;

    // operational domain parameters
    sidb::simulation::logic::operational_domain_params op_domain_params{};
    op_domain_params.operational_params.sim_params = sim_params;
    op_domain_params.operational_params.sim_engine = sidb::simulation::engine::QUICKEXACT;

    op_domain_params.sweep_dimensions         = {{sidb::simulation::logic::sweep_parameter::EPSILON_R},
                                                 {sidb::simulation::logic::sweep_parameter::LAMBDA_TF}};
    op_domain_params.sweep_dimensions[0].min  = 1.0;
    op_domain_params.sweep_dimensions[0].max  = 10.0;
    op_domain_params.sweep_dimensions[0].step = 0.01;
    op_domain_params.sweep_dimensions[1].min  = 1.0;
    op_domain_params.sweep_dimensions[1].max  = 10.0;
    op_domain_params.sweep_dimensions[1].step = 0.01;

    static const std::string folder = fmt::format("{}sidb_gate_libraries/bestagon_gates/", EXPERIMENTS_PATH);

    static const std::array<std::pair<std::vector<tt>, std::string>, 14> gates = {
        {{std::vector<tt>{synthesis::create_id_tt()}, "wire"},
         {std::vector<tt>{synthesis::create_id_tt()}, "wire_diag"},
         {std::vector<tt>{synthesis::create_not_tt()}, "inv"},
         {std::vector<tt>{synthesis::create_not_tt()}, "inv_diag"},
         {std::vector<tt>{synthesis::create_and_tt()}, "and"},
         {std::vector<tt>{synthesis::create_nand_tt()}, "nand"},
         {std::vector<tt>{synthesis::create_or_tt()}, "or"},
         {std::vector<tt>{synthesis::create_nor_tt()}, "nor"},
         {std::vector<tt>{synthesis::create_xor_tt()}, "xor"},
         {std::vector<tt>{synthesis::create_xnor_tt()}, "xnor"},
         {synthesis::create_fan_out_tt(), "fo2"},
         {synthesis::create_crossing_wire_tt(), "cx"},
         {synthesis::create_half_adder_tt(), "ha"},
         {synthesis::create_double_wire_tt(), "hourglass"}}};

    for (const auto& [truth_table, gate] : gates)
    {
        auto lyt = sidb::io::read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(fmt::format("{}/{}.sqd", folder, gate), gate);

        // Loop over operational conditions
        for (const auto cond : {sidb::simulation::logic::is_operational_params::operational_condition::TOLERATE_KINKS,
                                sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS})
        {
            sidb::simulation::logic::operational_domain_stats op_domain_stats_gs{};
            std::string                                       gate_name  = gate;
            double                                            ct_default = 0;

            op_domain_params.operational_params.op_condition = cond;
            ct_default = sidb::simulation::analysis::critical_temperature_gate_based(
                lyt, truth_table,
                sidb::simulation::analysis::critical_temperature_params{op_domain_params.operational_params});

            op_domain_params.operational_params.op_condition =
                sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS;
            ct_default = sidb::simulation::analysis::critical_temperature_gate_based(
                lyt, truth_table,
                sidb::simulation::analysis::critical_temperature_params{op_domain_params.operational_params});

            if (cond == sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS)
            {
                gate_name +=
                    " (I/O integrity)";  // Add I/O integrity to the gate name to make clear that kinks are rejected
            }

            const auto ct_domain = sidb::simulation::logic::critical_temperature_domain_grid_search(
                lyt, truth_table, op_domain_params, &op_domain_stats_gs);

            const auto min_ct   = ct_domain.minimum_ct();
            const auto max_ct   = ct_domain.maximum_ct();
            const auto delta_ct = max_ct - min_ct;

            // Benchmark and save
            opdomain_exp(gate_name, lyt.num_cells(), min_ct, ct_default, max_ct, delta_ct);
            opdomain_exp.save();
            opdomain_exp.table();
        }
    }

    op_domain_params.operational_params.op_condition =
        sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS;

    return EXIT_SUCCESS;
}
