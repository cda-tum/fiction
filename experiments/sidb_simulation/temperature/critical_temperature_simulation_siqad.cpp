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
 * @brief Critical temperature of the SiQAD gates.
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

    static const std::string folder = fmt::format("{}sidb_gate_libraries/siqad_gates/", EXPERIMENTS_PATH);

    static const std::array<std::pair<std::string, std::vector<tt>>, 5> gates = {
        std::make_pair("and", std::vector<tt>{synthesis::create_and_tt()}),
        std::make_pair("nand", std::vector<tt>{synthesis::create_nand_tt()}),
        std::make_pair("xnor", std::vector<tt>{synthesis::create_xnor_tt()}),
        std::make_pair("xor", std::vector<tt>{synthesis::create_xor_tt()}),
        std::make_pair("or", std::vector<tt>{synthesis::create_or_tt()})};

    const sidb::model::simulation_parameters                sim_params{2, -0.28};
    sidb::simulation::analysis::critical_temperature_params ct_params{sim_params};

    // this is how the gates are presented and simulated in "SiQAD: A Design and Simulation Tool for Atomic Silicon
    // Quantum Dot Circuits\" by Samuel Sze Hang Ng, Jacob Retallick, Hsi Nien Chiu, Robert Lupoiu, Lucian Livadaru,
    // Taleana Huff, Mohammad Rashidi, Wyatt Vine, Thomas Dienel, Robert A. Wolkow, and Konrad Walus in IEEE
    // TRANSACTIONS ON NANOTECHNOLOGY, Volume 19, 2020. (https://ieeexplore.ieee.org/abstract/document/8963859)
    ct_params.operational_params.input_bdl_iterator_params.input_bdl_config =
        sidb::simulation::logic::bdl_input_iterator_params::input_bdl_configuration::PERTURBER_ABSENCE_ENCODED;

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
