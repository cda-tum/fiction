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
#include <fiction/types.hpp>

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
    experiments::experiment<std::string, double, double> simulation_exp{
        "Benchmark",
        "Gate Name",
        "Critical Temperature [K]",
        "E_{g,err} [meV]",
    };

    static const std::string folder = fmt::format("{}sidb_gate_libraries/bestagon_gates/", EXPERIMENTS_PATH);

    static const std::array<std::pair<std::string, std::vector<tt>>, 12> gates = {
        std::make_pair("hourglass", synthesis::create_double_wire_tt()),
        std::make_pair("cx", synthesis::create_crossing_wire_tt()),
        std::make_pair("ha", synthesis::create_half_adder_tt()),
        std::make_pair("and", std::vector<tt>{synthesis::create_and_tt()}),
        std::make_pair("xor", std::vector<tt>{synthesis::create_xor_tt()}),
        std::make_pair("or", std::vector<tt>{synthesis::create_or_tt()}),
        std::make_pair("xnor", std::vector<tt>{synthesis::create_xnor_tt()}),
        std::make_pair("fo2", std::vector<tt>{synthesis::create_fan_out_tt()}),
        std::make_pair("nor", std::vector<tt>{synthesis::create_nor_tt()}),
        std::make_pair("nand", std::vector<tt>{synthesis::create_nand_tt()}),
        std::make_pair("inv", std::vector<tt>{synthesis::create_not_tt()}),
        std::make_pair("wire", std::vector<tt>{synthesis::create_id_tt()})};

    const sidb::model::simulation_parameters                      sim_params{2, -0.32};
    const sidb::simulation::analysis::critical_temperature_params ct_params{sim_params};

    for (const auto& [gate, truth_table] : gates)
    {
        const auto layout =
            sidb::io::read_sqd_layout<sidb_100_cell_clk_lyt_siqad>(fmt::format("{}/{}.sqd", folder, gate));

        sidb::simulation::analysis::critical_temperature_stats ct_stats{};

        const auto ct = sidb::simulation::analysis::critical_temperature_gate_based<sidb_100_cell_clk_lyt_siqad>(
            layout, truth_table, ct_params, &ct_stats);

        simulation_exp(gate, ct, ct_stats.energy_between_ground_state_and_first_erroneous);
        simulation_exp.save();
        simulation_exp.table();
    }
    return EXIT_SUCCESS;
}
