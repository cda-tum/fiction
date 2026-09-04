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
 * @brief Critical temperature of the Bestagon gates.
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
using namespace fiction::sidb::io;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation::analysis;
using namespace fiction::synthesis;

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
        std::make_pair("hourglass", create_double_wire_tt()),
        std::make_pair("cx", create_crossing_wire_tt()),
        std::make_pair("ha", create_half_adder_tt()),
        std::make_pair("and", std::vector<tt>{create_and_tt()}),
        std::make_pair("xor", std::vector<tt>{create_xor_tt()}),
        std::make_pair("or", std::vector<tt>{create_or_tt()}),
        std::make_pair("xnor", std::vector<tt>{create_xnor_tt()}),
        std::make_pair("fo2", std::vector<tt>{create_fan_out_tt()}),
        std::make_pair("nor", std::vector<tt>{create_nor_tt()}),
        std::make_pair("nand", std::vector<tt>{create_nand_tt()}),
        std::make_pair("inv", std::vector<tt>{create_not_tt()}),
        std::make_pair("wire", std::vector<tt>{create_id_tt()})};

    const simulation_parameters       sim_params{2, -0.32};
    const critical_temperature_params ct_params{sim_params};

    for (const auto& [gate, truth_table] : gates)
    {
        const auto layout = read_sqd_layout(fmt::format("{}/{}.sqd", folder, gate));

        critical_temperature_stats ct_stats{};

        const auto ct = critical_temperature_gate_based(layout, truth_table, ct_params, &ct_stats);

        simulation_exp(gate, ct, ct_stats.energy_between_ground_state_and_first_erroneous);
        simulation_exp.save();
        simulation_exp.table();
    }
    return EXIT_SUCCESS;
}
