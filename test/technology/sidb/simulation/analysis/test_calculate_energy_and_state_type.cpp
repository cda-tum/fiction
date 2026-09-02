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
 * @brief Tests for `fiction/technology/sidb/simulation/analysis/calculate_energy_and_state_type.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/synthesis/truth_tables.hpp>
#include <fiction/technology/sidb/simulation/analysis/calculate_energy_and_state_type.hpp>
#include <fiction/technology/sidb/simulation/analysis/energy_distribution.hpp>
#include <fiction/technology/sidb/simulation/engines/quickexact.hpp>
#include <fiction/technology/sidb/simulation/logic/bdl_input_iterator.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <limits>
#include <set>
#include <vector>

using namespace fiction;

using layout = sidb_cell_clk_lyt_siqad;

TEST_CASE("Single SiDB", "[calculate-energy-and-state-type]")
{
    const auto bestagon_and = blueprints::bestagon_and_gate<layout>();

    auto bii = sidb::simulation::logic::bdl_input_iterator{bestagon_and};

    // set input index 1
    bii = 1;

    const auto output_bdls = sidb::simulation::logic::detect_bdl_pairs(*bii, technology<layout>::cell_type::OUTPUT);

    const auto simulation_results = sidb::simulation::engines::quickexact(*bii);

    const auto simulated_charge_distributions = simulation_results.charge_distributions;

    REQUIRE(!simulated_charge_distributions.empty());

    const auto energy_distribution =
        sidb::simulation::analysis::calculate_energy_distribution(simulated_charge_distributions);

    const auto energy_state_with_state_info =
        sidb::simulation::analysis::calculate_energy_and_state_type_with_kinks_accepted(
            energy_distribution, simulated_charge_distributions, output_bdls,
            std::vector<tt>{synthesis::create_and_tt()}, 1);

    REQUIRE(energy_state_with_state_info.size() == 5);

    std::set<double> energy_values{};

    double energy_value = std::numeric_limits<double>::min();

    for (const auto& [energy, state] : energy_state_with_state_info)
    {
        // test if energy values are sorted
        CHECK(energy > energy_value);
        energy_value = energy;
        energy_values.insert(energy);
    }

    CHECK(energy_values.size() == 5);

    const auto ground_state   = energy_state_with_state_info.front();
    const auto first_excited  = energy_state_with_state_info[1];
    const auto second_excited = energy_state_with_state_info[2];
    const auto third_excited  = energy_state_with_state_info[3];
    const auto fourth_excited = energy_state_with_state_info[4];

    CHECK_THAT(ground_state.first, Catch::Matchers::WithinAbs(0.9915, 0.0001));
    CHECK(ground_state.second == sidb::simulation::analysis::state_type::ACCEPTED);

    CHECK_THAT(first_excited.first, Catch::Matchers::WithinAbs(0.9966, 0.0001));
    CHECK(first_excited.second == sidb::simulation::analysis::state_type::ACCEPTED);

    CHECK_THAT(second_excited.first, Catch::Matchers::WithinAbs(1.2827, 0.0001));
    CHECK(second_excited.second == sidb::simulation::analysis::state_type::REJECTED);

    CHECK_THAT(third_excited.first, Catch::Matchers::WithinAbs(1.2917, 0.0001));
    CHECK(third_excited.second == sidb::simulation::analysis::state_type::REJECTED);

    CHECK_THAT(fourth_excited.first, Catch::Matchers::WithinAbs(1.2976, 0.0001));
    CHECK(fourth_excited.second == sidb::simulation::analysis::state_type::REJECTED);
}
