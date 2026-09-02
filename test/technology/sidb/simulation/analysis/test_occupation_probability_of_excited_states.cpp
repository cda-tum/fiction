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

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/technology/sidb/simulation/analysis/calculate_energy_and_state_type.hpp>
#include <fiction/technology/sidb/simulation/analysis/energy_distribution.hpp>
#include <fiction/technology/sidb/simulation/analysis/occupation_probability_of_excited_states.hpp>
#include <fiction/utils/math/math_utils.hpp>

using namespace fiction;

TEST_CASE("occupation probability of all erroneous charge distribution states", "[occupation-probability-erroneous]")
{
    SECTION("empty energy distribution")
    {
        const sidb::simulation::analysis::energy_and_state_type est{};

        CHECK(sidb::simulation::analysis::occupation_probability_gate_based(est, 10) == 0.0);
        CHECK(sidb::simulation::analysis::occupation_probability_gate_based(est, 0.01) == 0.0);

        sidb::simulation::analysis::energy_distribution distribution{};
        distribution.add_energy_state(sidb::simulation::analysis::energy_state(0.1, 2));

        CHECK(fiction::utils::math::round_to_n_decimal_places(
                  sidb::simulation::analysis::occupation_probability_non_gate_based(distribution, 10.0), 6) == 0.0);
        CHECK(fiction::utils::math::round_to_n_decimal_places(
                  sidb::simulation::analysis::occupation_probability_non_gate_based(distribution, 0.01), 6) == 0.0);
    }

    SECTION("one state with degeneracy")
    {
        sidb::simulation::analysis::energy_and_state_type est{};
        est.emplace_back(0.1, sidb::simulation::analysis::state_type::ACCEPTED);
        est.emplace_back(0.1, sidb::simulation::analysis::state_type::REJECTED);

        CHECK(sidb::simulation::analysis::occupation_probability_gate_based(est, 10) == 0.5);
        CHECK(sidb::simulation::analysis::occupation_probability_gate_based(est, 0.01) == 0.5);

        sidb::simulation::analysis::energy_distribution distribution{};
        distribution.add_energy_state(sidb::simulation::analysis::energy_state(0.1, 2));

        CHECK(fiction::utils::math::round_to_n_decimal_places(
                  sidb::simulation::analysis::occupation_probability_non_gate_based(distribution, 10), 6) == 0);
        CHECK(fiction::utils::math::round_to_n_decimal_places(
                  sidb::simulation::analysis::occupation_probability_non_gate_based(distribution, 0.01), 6) == 0);
    }

    SECTION("a few states with degeneracy")
    {
        sidb::simulation::analysis::energy_and_state_type est{};
        est.emplace_back(0.1, sidb::simulation::analysis::state_type::ACCEPTED);
        est.emplace_back(0.1, sidb::simulation::analysis::state_type::REJECTED);

        est.emplace_back(0.2, sidb::simulation::analysis::state_type::ACCEPTED);
        est.emplace_back(0.2, sidb::simulation::analysis::state_type::ACCEPTED);

        CHECK(sidb::simulation::analysis::occupation_probability_gate_based(est, 0.001) == 0.5);

        sidb::simulation::analysis::energy_distribution distribution{};
        distribution.add_energy_state(sidb::simulation::analysis::energy_state(0.1, 2));
        distribution.add_energy_state(sidb::simulation::analysis::energy_state(0.2, 2));

        CHECK(sidb::simulation::analysis::occupation_probability_non_gate_based(distribution, 0.001) == 0.0);

        CHECK(fiction::utils::math::round_to_n_decimal_places(
                  sidb::simulation::analysis::occupation_probability_gate_based(est, 10E10), 6) == 0.25);
        CHECK(fiction::utils::math::round_to_n_decimal_places(
                  sidb::simulation::analysis::occupation_probability_non_gate_based(distribution, 10E10), 6) == 0.5);
    }

    SECTION("a few states with degeneracy, different oder of the energy_distribution_transparent_erroneous entries")
    {
        sidb::simulation::analysis::energy_and_state_type est{};

        est.emplace_back(0.2, sidb::simulation::analysis::state_type::ACCEPTED);
        est.emplace_back(0.2, sidb::simulation::analysis::state_type::ACCEPTED);

        est.emplace_back(0.1, sidb::simulation::analysis::state_type::ACCEPTED);
        est.emplace_back(0.1, sidb::simulation::analysis::state_type::REJECTED);

        CHECK(sidb::simulation::analysis::occupation_probability_gate_based(est, 0.001) == 0.5);

        sidb::simulation::analysis::energy_distribution distribution{};
        distribution.add_energy_state(sidb::simulation::analysis::energy_state(0.1, 2));
        distribution.add_energy_state(sidb::simulation::analysis::energy_state(0.2, 2));

        CHECK(sidb::simulation::analysis::occupation_probability_non_gate_based(distribution, 0.001) == 0.0);

        CHECK(fiction::utils::math::round_to_n_decimal_places(
                  sidb::simulation::analysis::occupation_probability_gate_based(est, 10E10), 2) == 0.25);
        CHECK(fiction::utils::math::round_to_n_decimal_places(
                  sidb::simulation::analysis::occupation_probability_non_gate_based(distribution, 10E10), 6) == 0.5);
    }

    SECTION("one state / true")
    {
        sidb::simulation::analysis::energy_and_state_type est{};
        est.emplace_back(1, sidb::simulation::analysis::state_type::ACCEPTED);
        CHECK(sidb::simulation::analysis::occupation_probability_gate_based(est, 1000) == 0.0);
        CHECK(sidb::simulation::analysis::occupation_probability_gate_based(est, 10) == 0.0);
        CHECK(sidb::simulation::analysis::occupation_probability_gate_based(est, 0.01) == 0.0);

        sidb::simulation::analysis::energy_distribution distribution{};
        distribution.add_energy_state(sidb::simulation::analysis::energy_state(1, 1));

        CHECK(sidb::simulation::analysis::occupation_probability_non_gate_based(distribution, 1000) == 0.0);
        CHECK(sidb::simulation::analysis::occupation_probability_non_gate_based(distribution, 10) == 0.0);
        CHECK(sidb::simulation::analysis::occupation_probability_non_gate_based(distribution, 0.01) == 0.0);
    }

    SECTION("one state / false")
    {
        sidb::simulation::analysis::energy_and_state_type est{};
        est.emplace_back(1, sidb::simulation::analysis::state_type::REJECTED);

        CHECK(sidb::simulation::analysis::occupation_probability_gate_based(est, 1000) == 1.0);
        CHECK(sidb::simulation::analysis::occupation_probability_gate_based(est, 10) == 1.0);
        CHECK(sidb::simulation::analysis::occupation_probability_gate_based(est, 0.01) == 1.0);

        sidb::simulation::analysis::energy_distribution distribution{};
        distribution.add_energy_state(sidb::simulation::analysis::energy_state(1, 1));

        CHECK(sidb::simulation::analysis::occupation_probability_non_gate_based(distribution, 1000) == 0.0);
        CHECK(sidb::simulation::analysis::occupation_probability_non_gate_based(distribution, 10) == 0.0);
        CHECK(sidb::simulation::analysis::occupation_probability_non_gate_based(distribution, 0.01) == 0.0);
    }
}
