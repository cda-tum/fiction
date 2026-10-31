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
 * @brief Tests for `fiction/technology/sidb/simulation/analysis/energy_distribution.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Willem Lambooy (wlambooy)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/charge_state.hpp>
#include <fiction/technology/sidb/simulation/analysis/energy_distribution.hpp>
#include <fiction/technology/sidb/simulation/potential_landscape.hpp>

#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation;
using namespace fiction::sidb::simulation::analysis;

TEST_CASE("Empty energy distributions reject extrema queries", "[energy-distribution]")
{
    const energy_distribution distribution{};

    CHECK_THROWS_AS(distribution.min_energy(), std::invalid_argument);
    CHECK_THROWS_AS(distribution.max_energy(), std::invalid_argument);
}

TEST_CASE("Energy degeneracy returns the stored state count", "[energy-distribution]")
{
    energy_distribution distribution{};
    distribution.add_energy_state({-1.0, 7});
    distribution.add_energy_state({0.0, 3});

    CHECK(distribution.degeneracy(-1.0) == 7);
    CHECK(distribution.degeneracy(-1.0 + 1e-7) == 7);
    CHECK(distribution.degeneracy(0.0) == 3);
    CHECK_FALSE(distribution.degeneracy(1.0).has_value());
}

TEST_CASE("Test calculate_energy_distribution function", "[energy-distribution]")
{
    SECTION("one empty layout")
    {
        const layout                           lyt{};
        const potential_landscape              land{lyt};
        const std::vector<charge_distribution> all_cds{land.evaluate(charge_distribution{lyt})};

        auto result = calculate_energy_distribution(all_cds);
        CHECK(result.size() == 1);
        const auto state = result.get_nth_state(0);
        REQUIRE(state.has_value());
        CHECK_THAT(state->electrostatic_potential_energy, Catch::Matchers::WithinAbs(0.0, 0.00001));
    }

    SECTION("one layout with one SiDB placed")
    {
        layout lyt{};
        lyt.assign_sidb({0, 0, 0}, dot_tag::NORMAL);

        const potential_landscape              land{lyt};
        const std::vector<charge_distribution> all_cds{land.evaluate(charge_distribution{lyt, charge_state::NEUTRAL})};

        auto result = calculate_energy_distribution(all_cds);
        CHECK(result.size() == 1);
        /** @brief Complete energy and degeneracy observations, including repeated visits. */
        std::vector<std::pair<double, uint64_t>> states{};
        result.for_each([&](const auto energy, const auto degeneracy) { states.emplace_back(energy, degeneracy); });
        REQUIRE(states.size() == 1);
        CHECK(states.front().first == 0.0);
        CHECK(states.front().second == 1);
    }

    SECTION("several layouts")
    {
        layout lyt{};
        lyt.assign_sidb({10, 5, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({11, 5, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({12, 5, 0}, dot_tag::NORMAL);

        const potential_landscape land{lyt, simulation_parameters{}};

        const auto first = land.evaluate(charge_distribution{lyt, charge_state::NEUTRAL});

        charge_distribution second{lyt, charge_state::NEUTRAL};
        second.assign_charge_state({11, 5, 0}, charge_state::NEGATIVE);
        second = land.evaluate(second);

        const auto third = land.evaluate(charge_distribution{lyt, charge_state::NEGATIVE});

        const std::vector<charge_distribution> all_cds{first,  first,  first, second, second,
                                                       second, second, third, third,  third};

        const auto result = calculate_energy_distribution(all_cds);

        CHECK(first.energy() == 0.0);
        CHECK(second.energy() == 0.0);
        CHECK(third.energy() > 0.0);
        // The first two distributions form the degenerate ground state.
        const auto ground_state = result.get_nth_state(0);
        REQUIRE(ground_state.has_value());
        CHECK(ground_state->degeneracy == 2);
        CHECK(ground_state->electrostatic_potential_energy == 0.0);

        const auto excited_state = result.get_nth_state(1);
        REQUIRE(excited_state.has_value());
        CHECK(excited_state->degeneracy == 1);
        CHECK(excited_state->electrostatic_potential_energy > 0.0);
        CHECK(result.size() == 2);
        CHECK_FALSE(result.get_nth_state(2).has_value());
    }
}
