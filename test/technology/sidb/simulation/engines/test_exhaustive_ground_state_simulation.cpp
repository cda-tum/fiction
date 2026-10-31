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
 * @brief Tests for `fiction/technology/sidb/simulation/engines/exhaustive_ground_state_simulation.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Willem Lambooy (wlambooy)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "utils/progress_recorder.hpp"

#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/charge_state.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/engines/exhaustive_ground_state_simulation.hpp>
#include <fiction/technology/sidb/simulation/result.hpp>
#include <fiction/utils/execution_timeout.hpp>
#include <fiction/utils/math/math_utils.hpp>

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation::engines;
using namespace fiction::utils::math;

TEST_CASE("ExGS rejects incomplete simulations after the caller deadline", "[exhaustive-ground-state-simulation]")
{
    layout lyt{};
    auto   deadline = std::chrono::steady_clock::time_point::max();

    SECTION("Already expired")
    {
        deadline = std::chrono::steady_clock::now();
        CHECK_THROWS_AS(exhaustive_ground_state_simulation(lyt, simulation_parameters{2, -0.32}, {}, deadline),
                        utils::timeout_error);
    }
    SECTION("Expires while enumerating")
    {
        for (int32_t i = 0; i < 20; ++i)
        {
            lyt.assign_sidb({i, 0, 0}, dot_tag::NORMAL);
        }
        deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds{1};
        CHECK_THROWS_AS(exhaustive_ground_state_simulation(lyt, simulation_parameters{2, -0.32}, {}, deadline),
                        utils::timeout_error);
    }
}

TEST_CASE("Empty layout ExGS simulation", "[exhaustive-ground-state-simulation]")
{
    const layout lyt{};

    const simulation_parameters params{2, -0.32};

    const auto simulation_results = exhaustive_ground_state_simulation(lyt, params);

    CHECK(simulation_results.charge_distributions.empty());
    CHECK(simulation_results.additional_simulation_parameters.empty());
    CHECK(simulation_results.algorithm_name == "ExGS");
}

TEST_CASE("Single SiDB ExGS simulation", "[exhaustive-ground-state-simulation]")
{
    layout lyt{};
    lyt.assign_sidb({1, 3, 0}, dot_tag::NORMAL);

    const simulation_parameters params{2, -0.32};

    const auto simulation_results = exhaustive_ground_state_simulation(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == charge_state::NEGATIVE);
}

TEST_CASE("ExGS simulation of a one BDL pair with one perturber", "[exhaustive-ground-state-simulation]")
{
    layout lyt{};

    lyt.assign_sidb({0, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({4, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({6, 0, 0}, dot_tag::NORMAL);

    const simulation_parameters params{2, -0.32};

    const auto simulation_results = exhaustive_ground_state_simulation(lyt, params);
    CHECK(simulation_results.charge_distributions.size() == 1);
}

TEST_CASE("ExGS simulation of a two-pair BDL wire with one perturber", "[exhaustive-ground-state-simulation]")
{
    layout lyt{};

    lyt.assign_sidb({0, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({5, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({7, 0, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({11, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({13, 0, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({17, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({19, 0, 0}, dot_tag::NORMAL);

    const simulation_parameters params{2, -0.32};

    const auto simulation_results = exhaustive_ground_state_simulation(lyt, params);

    const auto size_before = simulation_results.charge_distributions.size();

    const auto simulation_results_after = exhaustive_ground_state_simulation(lyt, params);
    auto       size_after               = simulation_results_after.charge_distributions.size();

    CHECK(size_before == 1);
    CHECK(size_after == 1);

    REQUIRE(!simulation_results_after.charge_distributions.empty());

    const auto& charge_lyt_first = simulation_results_after.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({0, 0, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({5, 0, 0}) == charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({7, 0, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({11, 0, 0}) == charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({13, 0, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({17, 0, 0}) == charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({19, 0, 0}) == charge_state::NEGATIVE);

    CHECK_THAT(charge_lyt_first.energy(), Catch::Matchers::WithinAbs(0.2460493219, ERROR_MARGIN));
}

TEST_CASE("ExGS simulation of a Y-shaped SiDB arrangement", "[exhaustive-ground-state-simulation]")
{
    layout lyt{};

    lyt.assign_sidb({-11, -2, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({-10, -1, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({-4, -1, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({-3, -2, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({-7, 0, 1}, dot_tag::NORMAL);

    lyt.assign_sidb({-7, 1, 1}, dot_tag::NORMAL);
    lyt.assign_sidb({-7, 3, 0}, dot_tag::NORMAL);

    const simulation_parameters params{2, -0.32};

    const auto simulation_results = exhaustive_ground_state_simulation(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());

    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({-11, -2, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({-10, -1, 0}) == charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({-3, -2, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({-4, -1, 0}) == charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({-7, 0, 1}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({-7, 1, 1}) == charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({-7, 3, 0}) == charge_state::NEGATIVE);

    CHECK_THAT(charge_lyt_first.energy(), Catch::Matchers::WithinAbs(0.3191788254, ERROR_MARGIN));
}

TEST_CASE("ExGS simulation of a Y-shaped SiDB OR gate with input 01", "[exhaustive-ground-state-simulation]")
{
    layout lyt{};

    lyt.assign_sidb({6, 2, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({8, 3, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({12, 3, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({14, 2, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({10, 5, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({10, 6, 1}, dot_tag::NORMAL);
    lyt.assign_sidb({10, 8, 1}, dot_tag::NORMAL);
    lyt.assign_sidb({16, 1, 0}, dot_tag::NORMAL);

    const auto& [name, params, expected_charges,
                 expected_energy] = GENERATE(Catch::Generators::table<const char*, simulation_parameters,
                                                                      std::array<int, 8>, double>(
        {{"Standard Physical Parameters",
          simulation_parameters{2, -0.28, 5.6, 5.0},
          {-1, -1, -1, -1, -1, 0, 0, 0},
          0.4662582096},
         {"Increased mu_minus", simulation_parameters{2, -0.1, 5.6, 5.0}, {-1, 0, -1, 0, -1, 0, 0, 0}, 0.061037632},
         {"Decreased mu_minus",
          simulation_parameters{2, -0.7, 5.6, 5.0},
          {-1, -1, -1, -1, -1, -1, -1, -1},
          2.069954113},
         {"Decreased lambda_tf",
          simulation_parameters{2, -0.28, 5.6, 1},
          {-1, -1, -1, -1, -1, -1, -1, -1},
          0.5432404075},
         {"Increased lambda_tf", simulation_parameters{2, -0.28, 5.6, 10}, {-1, 0, -1, 0, -1, -1, 0, 0}, 0.2930574885},
         {"Increased epsilon_r",
          simulation_parameters{2, -0.28, 10, 5.0},
          {-1, -1, -1, -1, -1, 0, 0, -1},
          0.505173434}}));
    CAPTURE(name);

    const auto simulation_results = exhaustive_ground_state_simulation(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    const auto&                       charges = simulation_results.charge_distributions.front();
    const std::array<lattice_site, 8> sites{
        {{6, 2, 0}, {12, 3, 0}, {10, 8, 1}, {10, 6, 1}, {16, 1, 0}, {10, 5, 0}, {14, 2, 0}, {8, 3, 0}}};
    for (std::size_t i = 0; i < sites.size(); ++i)
    {
        CAPTURE(i);
        CHECK(static_cast<int>(charge_state_to_sign(charges.get_charge_state(sites[i]))) == expected_charges[i]);
    }
    CHECK_THAT(charges.energy(), Catch::Matchers::WithinAbs(expected_energy, ERROR_MARGIN));
}

TEST_CASE("ExGS simulation of positively charged SiDBs", "[exhaustive-ground-state-simulation]")
{
    layout lyt{};
    lyt.assign_sidb({0, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({4, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({6, 0, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({11, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({12, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({11, 0, 1}, dot_tag::NORMAL);
    lyt.assign_sidb({12, 0, 1}, dot_tag::NORMAL);

    lyt.assign_sidb({18, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({20, 0, 0}, dot_tag::NORMAL);

    const simulation_parameters params{3, -0.32};

    const auto simulation_results = exhaustive_ground_state_simulation(lyt, params);

    CHECK(simulation_results.charge_distributions.size() == 4);
}

TEST_CASE("ExGS gate simulation of Si-111 surface", "[exhaustive-ground-state-simulation]")
{
    layout lyt{lattice::si_111_1x1()};
    lyt.assign_sidb({0, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({1, 1, 1}, dot_tag::NORMAL);
    lyt.assign_sidb({2, 2, 1}, dot_tag::NORMAL);

    lyt.assign_sidb({8, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({6, 1, 1}, dot_tag::NORMAL);
    lyt.assign_sidb({5, 2, 1}, dot_tag::NORMAL);

    lyt.assign_sidb({4, 8, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({4, 10, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({4, 14, 0}, dot_tag::NORMAL);

    const simulation_parameters params{2, -0.32};

    const auto simulation_results = exhaustive_ground_state_simulation(lyt, params);

    const auto ground_state = simulation_results.groundstates();
    REQUIRE(ground_state.size() == 1);

    CHECK(ground_state.front().get_charge_state({0, 0, 0}) == charge_state::NEGATIVE);
    CHECK(ground_state.front().get_charge_state({1, 1, 1}) == charge_state::NEUTRAL);
    CHECK(ground_state.front().get_charge_state({2, 2, 1}) == charge_state::NEGATIVE);
    CHECK(ground_state.front().get_charge_state({8, 0, 0}) == charge_state::NEGATIVE);
    CHECK(ground_state.front().get_charge_state({6, 1, 1}) == charge_state::NEUTRAL);
    CHECK(ground_state.front().get_charge_state({5, 2, 1}) == charge_state::NEGATIVE);
    CHECK(ground_state.front().get_charge_state({4, 8, 0}) == charge_state::NEUTRAL);
    CHECK(ground_state.front().get_charge_state({4, 10, 0}) == charge_state::NEGATIVE);
    CHECK(ground_state.front().get_charge_state({4, 14, 0}) == charge_state::NEGATIVE);
}

TEST_CASE("7 SiDB layout", "[exhaustive-ground-state-simulation]")
{
    layout lyt{};

    lyt.assign_sidb({-6, 1, 1}, dot_tag::NORMAL);
    lyt.assign_sidb({2, 4, 1}, dot_tag::NORMAL);

    lyt.assign_sidb({4, 6, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({8, 3, 1}, dot_tag::NORMAL);

    lyt.assign_sidb({-8, -3, 1}, dot_tag::NORMAL);
    lyt.assign_sidb({-1, -1, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({0, 2, 0}, dot_tag::NORMAL);

    const simulation_parameters params{2, -0.25};

    const auto simulation_results = exhaustive_ground_state_simulation(lyt, params);

    CHECK(simulation_results.charge_distributions.size() == 1);
}

TEST_CASE("ExGS reports progress", "[exhaustive-ground-state-simulation]")
{
    layout lyt{};

    lyt.assign_sidb({0, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({4, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({6, 0, 0}, dot_tag::NORMAL);

    progress_recorder rec{};

    const auto simulation_results =
        exhaustive_ground_state_simulation(lyt, simulation_parameters{2, -0.32}, rec.callback());

    CHECK(simulation_results.charge_distributions.size() == 1);

    // three SiDBs in base 2 have eight charge configurations
    CHECK(rec.is_consistent("charge configurations"));
    CHECK(rec.final_count("charge configurations") == 8);
}
