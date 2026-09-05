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
 * @brief Tests for `fiction/technology/sidb/simulation/engines/quicksim.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/technology/sidb/cell_level_layout_conversion.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/charge_state.hpp>
#include <fiction/technology/sidb/simulation/analysis/energy_distribution.hpp>
#include <fiction/technology/sidb/simulation/engines/quicksim.hpp>
#include <fiction/technology/sidb/simulation/result.hpp>
#include <fiction/technology/sidb/technology.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/math/math_utils.hpp>

#include <optional>
#include <stdexcept>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation;
using namespace fiction::sidb::simulation::analysis;
using namespace fiction::sidb::simulation::engines;
using namespace fiction::utils::math;

/**
 * @brief Returns the result contained in a successful QuickSim invocation.
 * @param simulation_result Optional QuickSim result.
 * @return Contained simulation result.
 * @throws std::logic_error If QuickSim did not produce a result.
 */
static const result& checked_result(const std::optional<result>& simulation_result)
{
    if (!simulation_result.has_value())
    {
        throw std::logic_error{"QuickSim did not produce a result"};
    }
    return *simulation_result;
}

TEST_CASE("Zero iteration steps", "[quicksim]")
{
    const layout lyt{};

    const quicksim_params qs_params{.sim_params = simulation_parameters{2, -0.30}, .iteration_steps = 0};

    const auto simulation_results = quicksim(lyt, qs_params);

    CHECK(!simulation_results.has_value());
}

TEST_CASE("Empty layout QuickSim simulation", "[quicksim]")
{
    const layout lyt{};

    const quicksim_params qs_params{.sim_params = simulation_parameters{2, -0.30}};

    REQUIRE(qs_params.sim_params.mu_minus == -0.30);

    const auto simulation_results = quicksim(lyt, qs_params);

    REQUIRE(!simulation_results.has_value());
}

TEST_CASE("Single SiDB QuickSim simulation", "[quicksim]")
{
    layout lyt{};

    lyt.assign_cell_type({1, 3, 0}, sidb_technology::cell_type::NORMAL);

    const quicksim_params qs_params{.sim_params = simulation_parameters{2, -0.30}};

    REQUIRE(qs_params.sim_params.mu_minus == -0.30);

    const auto simulation_results = quicksim(lyt, qs_params);

    REQUIRE(simulation_results.has_value());
    CHECK(!checked_result(simulation_results).charge_distributions.empty());
}

static void check_for_absence_of_positive_charges(const result& simulation_results)
{
    REQUIRE(!simulation_results.charge_distributions.empty());

    for (const auto& lyt : simulation_results.charge_distributions)
    {
        CHECK(!lyt.charge_exists(charge_state::POSITIVE));
    }
}

static void check_for_runtime_measurement(const result& simulation_results) noexcept
{
    CHECK(simulation_results.simulation_runtime.count() > 0);
}

TEST_CASE("QuickSim simulation of several SiDBs with varying thread counts", "[quicksim]")
{
    layout lyt{};

    lyt.assign_cell_type({1, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({3, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({4, 3, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({6, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({7, 3, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({6, 10, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({7, 10, 0}, sidb_technology::cell_type::NORMAL);

    const simulation_parameters params{2, -0.30};

    quicksim_params qs_params{.sim_params = params};

    REQUIRE(qs_params.sim_params.mu_minus == -0.30);

    SECTION("Default settings")
    {
        const auto simulation_results = quicksim(lyt, qs_params);

        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
    }
    SECTION("0 threads")
    {
        qs_params.number_threads = 0;

        const auto simulation_results = quicksim(lyt, qs_params);

        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
    }

    SECTION("1 thread")
    {
        qs_params.number_threads = 1;

        const auto simulation_results = quicksim(lyt, qs_params);

        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
    }
    SECTION("2 threads")
    {
        qs_params.number_threads = 2;

        const auto simulation_results = quicksim(lyt, qs_params);

        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
    }
    SECTION("100 threads")
    {
        qs_params.number_threads = 100;

        const auto simulation_results = quicksim(lyt, qs_params);

        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
    }
}

TEST_CASE("QuickSim simulation of an SiDB layout comprising of 10 SiDBs with varying thread counts", "[quicksim]")
{
    layout lyt{};

    lyt.assign_cell_type({-13, -1, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({-9, -1, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({-7, -1, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({-3, -1, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({-1, -1, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({3, -1, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({5, -1, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({9, -1, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({11, -1, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({15, -1, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({17, -1, 1}, sidb_technology::cell_type::NORMAL);

    const simulation_parameters params{2, -0.32};

    quicksim_params qs_params{.sim_params = params};

    REQUIRE(qs_params.sim_params.mu_minus == -0.32);

    const auto check_charge_configuration = [](const result& simulation_results)
    {
        REQUIRE(!simulation_results.charge_distributions.empty());

        const auto& charge_lyt_first = simulation_results.charge_distributions.front();

        CHECK(charge_lyt_first.get_charge_state({-13, -1, 1}) == charge_state::NEGATIVE);

        CHECK(charge_lyt_first.get_charge_state({-9, -1, 1}) == charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({-7, -1, 1}) == charge_state::NEGATIVE);

        CHECK(charge_lyt_first.get_charge_state({-3, -1, 1}) == charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({-1, -1, 1}) == charge_state::NEGATIVE);

        CHECK(charge_lyt_first.get_charge_state({3, -1, 1}) == charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({5, -1, 1}) == charge_state::NEGATIVE);

        CHECK(charge_lyt_first.get_charge_state({9, -1, 1}) == charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({11, -1, 1}) == charge_state::NEGATIVE);

        CHECK(charge_lyt_first.get_charge_state({15, -1, 1}) == charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({17, -1, 1}) == charge_state::NEGATIVE);

        CHECK_THAT(charge_lyt_first.energy(), Catch::Matchers::WithinAbs(0.4798721334, ERROR_MARGIN));
    };

    SECTION("Default settings")
    {
        const auto simulation_results = quicksim(lyt, qs_params);
        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
    SECTION("0 threads")
    {
        qs_params.number_threads = 0;

        const auto simulation_results = quicksim(lyt, qs_params);
        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
    SECTION("1 thread")
    {
        qs_params.number_threads = 1;

        const auto simulation_results = quicksim(lyt, qs_params);
        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
    SECTION("2 threads")
    {
        qs_params.number_threads = 2;

        const auto simulation_results = quicksim(lyt, qs_params);
        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
    SECTION("100 threads")
    {
        qs_params.number_threads = 100;

        const auto simulation_results = quicksim(lyt, qs_params);
        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
}

TEST_CASE("QuickSim simulation of a Y-shaped SiDB arrangement with varying thread counts", "[quicksim]")
{
    layout lyt{};

    lyt.assign_cell_type({-11, -2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({-10, -1, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({-4, -1, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({-3, -2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({-7, 0, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({-7, 1, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({-7, 3, 0}, sidb_technology::cell_type::NORMAL);

    const simulation_parameters params{2, -0.32};

    quicksim_params qs_params{.sim_params = params};

    REQUIRE(qs_params.sim_params.mu_minus == -0.32);

    const auto check_charge_configuration = [](const result& simulation_results)
    {
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
    };

    SECTION("Default settings")
    {
        const auto simulation_results = quicksim(lyt, qs_params);
        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
    SECTION("0 threads")
    {
        qs_params.number_threads = 0;

        const auto simulation_results = quicksim(lyt, qs_params);

        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
    SECTION("1 thread")
    {
        qs_params.number_threads = 1;

        const auto simulation_results = quicksim(lyt, qs_params);

        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
    SECTION("2 threads")
    {
        qs_params.number_threads = 2;

        const auto simulation_results = quicksim(lyt, qs_params);
        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
    SECTION("100 threads")
    {
        qs_params.number_threads = 100;

        const auto simulation_results = quicksim(lyt, qs_params);
        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
}

TEST_CASE("QuickSim simulation of a Y-shaped SiDB OR gate with input 01 and varying thread counts", "[quicksim]")
{
    layout lyt{};

    lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 3, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({14, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({10, 5, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, 6, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({10, 8, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({16, 1, 0}, sidb_technology::cell_type::NORMAL);

    const simulation_parameters params{2, -0.28};

    quicksim_params qs_params{.sim_params = params};

    REQUIRE(qs_params.sim_params.mu_minus == -0.28);

    const auto check_charge_configuration = [](const result& sim_results)
    {
        REQUIRE(!sim_results.charge_distributions.empty());

        const auto& charge_lyt_first = sim_results.charge_distributions.front();

        CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == charge_state::NEGATIVE);

        CHECK_THAT(charge_lyt_first.energy(), Catch::Matchers::WithinAbs(0.4662582096, ERROR_MARGIN));
    };

    SECTION("Default settings")
    {
        const auto simulation_results = quicksim(lyt, qs_params);
        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
    SECTION("0 threads")
    {
        qs_params.number_threads = 0;

        const auto simulation_results = quicksim(lyt, qs_params);
        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
    SECTION("1 thread")
    {
        qs_params.number_threads = 1;

        const auto simulation_results = quicksim(lyt, qs_params);
        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
    SECTION("2 threads")
    {
        qs_params.number_threads = 2;

        const auto simulation_results = quicksim(lyt, qs_params);
        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
    SECTION("100 threads")
    {
        qs_params.number_threads = 100;

        const auto simulation_results = quicksim(lyt, qs_params);
        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));

        SECTION("timeout with 0 ms")
        {
            qs_params.timeout                       = 0;
            const auto simulation_results_timeout_0 = quicksim(lyt, qs_params);

            CHECK(!simulation_results_timeout_0.has_value());
        }
    }
}

TEST_CASE("QuickSim simulation of an SiDB BDL pair with varying thread counts", "[quicksim]")
{
    layout lyt{};

    lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 2, 0}, sidb_technology::cell_type::NORMAL);

    const simulation_parameters params{2, -0.25};

    quicksim_params qs_params{.sim_params = params};

    REQUIRE(qs_params.sim_params.mu_minus == -0.25);

    const auto check_charge_configuration = [](const result& sim_result)
    {
        REQUIRE(!sim_result.charge_distributions.empty());

        REQUIRE(!calculate_energy_distribution(sim_result.charge_distributions).empty());

        const auto& charge_lyt_first = sim_result.charge_distributions.front();

        CHECK((((charge_lyt_first.get_charge_state({6, 2, 0}) == charge_state::NEGATIVE) &&
                (charge_lyt_first.get_charge_state({8, 2, 0}) == charge_state::NEUTRAL)) ||
               ((charge_lyt_first.get_charge_state({6, 2, 0}) == charge_state::NEUTRAL) &&
                (charge_lyt_first.get_charge_state({8, 2, 0}) == charge_state::NEGATIVE))));
    };

    SECTION("Default settings")
    {
        const auto simulation_results = quicksim(lyt, qs_params);
        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
    SECTION("0 threads")
    {
        qs_params.number_threads = 0;

        const auto simulation_results = quicksim(lyt, qs_params);
        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
    SECTION("1 thread")
    {
        qs_params.number_threads = 1;

        const auto simulation_results = quicksim(lyt, qs_params);

        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
    SECTION("2 threads")
    {
        qs_params.number_threads = 2;

        const auto simulation_results = quicksim(lyt, qs_params);
        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
    SECTION("100 threads")
    {
        qs_params.number_threads = 100;

        const auto simulation_results = quicksim(lyt, qs_params);

        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
}

TEST_CASE("QuickSim simulation of an layout comprising of 13 SiDBs", "[quicksim]")
{
    // initialize SiDB layout
    layout lyt{};

    // design random SiDB layout
    lyt.assign_cell_type({11, 15, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({37, 8, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({17, 6, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({37, 19, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({4, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({0, 17, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({39, 10, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({22, 19, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({5, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({17, 2, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({27, 13, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({27, 16, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({8, 10, 1}, sidb_technology::cell_type::NORMAL);

    const simulation_parameters params{2, -0.32};

    quicksim_params qs_params{.sim_params = params};

    // check QuickSim parameters
    REQUIRE(qs_params.sim_params.mu_minus == -0.32);

    // lambda function to check charge configurations
    const auto check_charge_configuration = [](const result& sim_results)
    {
        REQUIRE(!sim_results.charge_distributions.empty());

        REQUIRE(!calculate_energy_distribution(sim_results.charge_distributions).empty());

        const auto& charge_lyt_first = sim_results.charge_distributions.front();

        charge_lyt_first.foreach_cell(
            [&](const auto& cell)
            {
                if (cell != lattice_site{5, 3, 0})
                {
                    CHECK(charge_lyt_first.get_charge_state(cell) == charge_state::NEGATIVE);
                }
                else
                {
                    CHECK(charge_lyt_first.get_charge_state({5, 3, 0}) == charge_state::NEUTRAL);
                }
            });
    };

    SECTION("Default settings")
    {
        const auto simulation_results = quicksim(lyt, qs_params);

        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }

    SECTION("0 threads")
    {
        qs_params.number_threads = 0;

        const auto simulation_results = quicksim(lyt, qs_params);

        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
    SECTION("1 thread")
    {
        qs_params.number_threads = 1;

        const auto simulation_results = quicksim(lyt, qs_params);

        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
    SECTION("2 threads")
    {
        qs_params.number_threads = 2;

        const auto simulation_results = quicksim(lyt, qs_params);

        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
    SECTION("100 threads")
    {
        qs_params.number_threads = 100;

        const auto simulation_results = quicksim(lyt, qs_params);

        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
}

TEST_CASE("QuickSim simulation of an layout comprising of 13 SiDBs, all negatively charged", "[quicksim]")
{
    layout lyt{};

    lyt.assign_cell_type({26, 10, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({23, 19, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({0, 5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({38, 10, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({11, 5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({13, 2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({40, 19, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({16, 9, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({19, 16, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({5, 8, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({8, 15, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({39, 9, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({30, 15, 0}, sidb_technology::cell_type::NORMAL);

    const simulation_parameters params{2, -0.32};

    quicksim_params qs_params{.sim_params = params};

    REQUIRE(qs_params.sim_params.mu_minus == -0.32);

    const auto check_charge_configuration = [](const result& simulation_results)
    {
        REQUIRE(!simulation_results.charge_distributions.empty());

        REQUIRE(!calculate_energy_distribution(simulation_results.charge_distributions).empty());

        const auto& charge_lyt_first = simulation_results.charge_distributions.front();

        for (const auto& cell : charge_lyt_first.sites())
        {
            CHECK(charge_lyt_first.get_charge_state(cell) == charge_state::NEGATIVE);
        }
    };

    SECTION("Default settings")
    {
        const auto simulation_results = quicksim(lyt, qs_params);

        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
    SECTION("0 threads")
    {
        qs_params.number_threads = 0;

        const auto simulation_results = quicksim(lyt, qs_params);

        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
    SECTION("1 thread")
    {
        qs_params.number_threads = 1;

        const auto simulation_results = quicksim(lyt, qs_params);

        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
    SECTION("2 threads")
    {
        qs_params.number_threads = 2;

        const auto simulation_results = quicksim(lyt, qs_params);

        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
    SECTION("100 threads")
    {
        qs_params.number_threads = 100;

        const auto simulation_results = quicksim(lyt, qs_params);

        REQUIRE(simulation_results.has_value());

        check_for_absence_of_positive_charges(checked_result(simulation_results));
        check_for_runtime_measurement(checked_result(simulation_results));
        check_charge_configuration(checked_result(simulation_results));
    }
}

TEST_CASE("Edge case with four SiDBs", "[quicksim]")
{
    layout lyt{};

    lyt.assign_cell_type({0, 1, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({3, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({3, 0, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, sidb_technology::cell_type::NORMAL);

    quicksim_params qs_params{};

    SECTION("alpha = 1.0 does not work")
    {
        qs_params.alpha               = 1.0;
        const auto simulation_results = quicksim(lyt, qs_params);
        CHECK(!simulation_results.has_value());
    }
    SECTION("alpha = 0.7 works")
    {
        qs_params.alpha               = 0.7;
        const auto simulation_results = quicksim(lyt, qs_params);
        REQUIRE(simulation_results.has_value());
        CHECK(!checked_result(simulation_results).charge_distributions.empty());
    }
}

TEST_CASE("QuickSim simulation of a Y-shaped SiDB OR gate with input 01", "[ExGS]")
{
    layout lyt{};

    lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 3, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({14, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({10, 5, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, 6, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({10, 8, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({16, 1, 0}, sidb_technology::cell_type::NORMAL);

    simulation_parameters params{2, -0.28};

    SECTION("Standard Physical Parameters")
    {
        const quicksim_params qs_params{.sim_params = params};

        const auto simulation_results = quicksim(lyt, qs_params);

        REQUIRE(simulation_results.has_value());

        REQUIRE(!checked_result(simulation_results).charge_distributions.empty());
        const auto& charge_lyt_first = checked_result(simulation_results).charge_distributions.front();

        CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == charge_state::NEUTRAL);

        CHECK_THAT(charge_lyt_first.energy(), Catch::Matchers::WithinAbs(0.4662582096, ERROR_MARGIN));
    }

    SECTION("Increased mu_minus")
    {
        params.mu_minus = -0.1;

        const quicksim_params qs_params{.sim_params = params};

        const auto simulation_results = quicksim(lyt, qs_params);

        REQUIRE(simulation_results.has_value());

        REQUIRE(!checked_result(simulation_results).charge_distributions.empty());
        const auto& charge_lyt_first = checked_result(simulation_results).charge_distributions.front();

        CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == charge_state::NEUTRAL);

        CHECK_THAT(charge_lyt_first.energy(), Catch::Matchers::WithinAbs(0.061037632, ERROR_MARGIN));
    }

    SECTION("Decreased mu_minus")
    {
        params.mu_minus = -0.7;

        const quicksim_params qs_params{.sim_params = params};

        const auto simulation_results = quicksim(lyt, qs_params);

        REQUIRE(simulation_results.has_value());

        REQUIRE(!checked_result(simulation_results).charge_distributions.empty());
        const auto& charge_lyt_first = checked_result(simulation_results).charge_distributions.front();

        CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == charge_state::NEGATIVE);

        CHECK_THAT(charge_lyt_first.energy(), Catch::Matchers::WithinAbs(2.069954113, ERROR_MARGIN));
    }

    SECTION("Decreased lambda_tf")
    {
        params.lambda_tf = 1;

        const quicksim_params qs_params{.sim_params = params};

        const auto simulation_results = quicksim(lyt, qs_params);

        REQUIRE(simulation_results.has_value());

        REQUIRE(!checked_result(simulation_results).charge_distributions.empty());
        const auto& charge_lyt_first = checked_result(simulation_results).charge_distributions.front();

        CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == charge_state::NEGATIVE);

        CHECK_THAT(charge_lyt_first.energy(), Catch::Matchers::WithinAbs(0.5432404075, ERROR_MARGIN));
    }

    SECTION("Increased lambda_tf")
    {
        params.lambda_tf = 10;

        const quicksim_params qs_params{.sim_params = params};

        const auto simulation_results = quicksim(lyt, qs_params);

        REQUIRE(simulation_results.has_value());

        REQUIRE(!checked_result(simulation_results).charge_distributions.empty());
        const auto& charge_lyt_first = checked_result(simulation_results).charge_distributions.front();

        CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == charge_state::NEUTRAL);

        CHECK_THAT(charge_lyt_first.energy(), Catch::Matchers::WithinAbs(0.2930574885, ERROR_MARGIN));
    }

    SECTION("Increased epsilon_r")
    {
        params.epsilon_r = 10;

        const quicksim_params qs_params{.sim_params = params};

        const auto simulation_results = quicksim(lyt, qs_params);

        REQUIRE(simulation_results.has_value());

        REQUIRE(!checked_result(simulation_results).charge_distributions.empty());
        const auto& charge_lyt_first = checked_result(simulation_results).charge_distributions.front();

        CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == charge_state::NEGATIVE);

        CHECK_THAT(charge_lyt_first.energy(), Catch::Matchers::WithinAbs(0.505173434, ERROR_MARGIN));
    }
}

TEST_CASE("QuickSim gate simulation on the Si-111 surface", "[quicksim]")
{
    layout lyt{lattice::si_111_1x1()};
    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({1, 1, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, 2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({8, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 1, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({5, 2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({4, 8, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({4, 10, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({4, 14, 0}, sidb_technology::cell_type::NORMAL);

    const quicksim_params params{.sim_params = simulation_parameters{2, -0.32, 5.6, 5}};

    const auto simulation_results = quicksim(lyt, params);

    REQUIRE(simulation_results.has_value());

    const auto ground_state = checked_result(simulation_results).groundstates();
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

TEST_CASE("QuickSim AND gate simulation on the Si-111 surface", "[quicksim]")
{
    SECTION("no input applied")
    {
        const auto            lyt = to_sidb_layout(blueprints::and_gate_111<sidb_111_cell_clk_lyt_siqad>());
        const quicksim_params params{.sim_params      = simulation_parameters{2, -0.32, 5.6, 5},
                                     .iteration_steps = 3000,
                                     .alpha           = 0.5};

        const auto simulation_results = quicksim(lyt, params);

        REQUIRE(simulation_results.has_value());

        const auto ground_state = checked_result(simulation_results).groundstates();
        REQUIRE(ground_state.size() == 1);

        CHECK(ground_state.front().get_charge_state({0, 0, 0}) == charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({1, 1, 1}) == charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({25, 0, 0}) == charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({23, 1, 1}) == charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({4, 4, 0}) == charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({21, 4, 0}) == charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({5, 5, 1}) == charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({19, 5, 1}) == charge_state::NEGATIVE);

        CHECK(ground_state.front().get_charge_state({17, 8, 0}) == charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({8, 8, 0}) == charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({9, 9, 1}) == charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({15, 9, 1}) == charge_state::NEGATIVE);

        CHECK(ground_state.front().get_charge_state({10, 18, 0}) == charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({13, 17, 0}) == charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({16, 18, 0}) == charge_state::NEGATIVE);

        CHECK(ground_state.front().get_charge_state({15, 21, 1}) == charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({17, 23, 0}) == charge_state::NEGATIVE);

        CHECK(ground_state.front().get_charge_state({19, 25, 1}) == charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({21, 27, 0}) == charge_state::NEGATIVE);

        CHECK(ground_state.front().get_charge_state({23, 29, 1}) == charge_state::NEGATIVE);
    }

    SECTION("10 input applied")
    {
        auto lyt = to_sidb_layout(blueprints::and_gate_111<sidb_111_cell_clk_lyt_siqad>());
        lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::EMPTY);
        lyt.assign_cell_type({23, 1, 1}, sidb_technology::cell_type::EMPTY);

        const quicksim_params params{.sim_params      = simulation_parameters{2, -0.32, 5.6, 5},
                                     .iteration_steps = 3000,
                                     .alpha           = 0.5};

        const auto simulation_results = quicksim(lyt, params);

        REQUIRE(simulation_results.has_value());

        const auto ground_state = checked_result(simulation_results).groundstates();

        REQUIRE(ground_state.size() == 1);

        CHECK(ground_state.front().get_charge_state({1, 1, 1}) == charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({25, 0, 0}) == charge_state::NEGATIVE);

        CHECK(ground_state.front().get_charge_state({4, 4, 0}) == charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({21, 4, 0}) == charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({5, 5, 1}) == charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({19, 5, 1}) == charge_state::NEUTRAL);

        CHECK(ground_state.front().get_charge_state({8, 8, 0}) == charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({17, 8, 0}) == charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({9, 9, 1}) == charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({15, 9, 1}) == charge_state::NEUTRAL);

        CHECK(ground_state.front().get_charge_state({10, 18, 0}) == charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({13, 17, 0}) == charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({16, 18, 0}) == charge_state::NEUTRAL);

        CHECK(ground_state.front().get_charge_state({15, 21, 1}) == charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({17, 23, 0}) == charge_state::NEUTRAL);

        CHECK(ground_state.front().get_charge_state({19, 25, 1}) == charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({21, 27, 0}) == charge_state::NEUTRAL);

        CHECK(ground_state.front().get_charge_state({23, 29, 1}) == charge_state::NEGATIVE);
    }

    SECTION("timeout with 100 ms")
    {
        auto                  lyt = to_sidb_layout(blueprints::and_gate_111<sidb_111_cell_clk_lyt_siqad>());
        const quicksim_params params{.sim_params      = simulation_parameters{2, -0.32, 5.6, 5},
                                     .iteration_steps = 300000,
                                     .alpha           = 0.5,
                                     .number_threads  = 1,
                                     .timeout         = 100};
        const auto            simulation_results_timeout_100 = quicksim(lyt, params);

        REQUIRE(!simulation_results_timeout_100.has_value());
    }
}
