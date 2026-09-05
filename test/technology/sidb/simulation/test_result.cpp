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
 * @brief Tests for `fiction/technology/sidb/simulation/result.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/charge_state.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/engines/exhaustive_ground_state_simulation.hpp>
#include <fiction/technology/sidb/simulation/potential_landscape.hpp>
#include <fiction/technology/sidb/simulation/result.hpp>
#include <fiction/technology/sidb/technology.hpp>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation;
using namespace fiction::sidb::simulation::engines;

TEST_CASE("Determine the groundstate from simulation results", "[sidb-simulation-result]")
{
    SECTION("Three distinct charge distributions")
    {
        layout lyt{};

        lyt.assign_cell_type({5, 2, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({5, 2, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({5, 3, 0}, sidb_technology::cell_type::NORMAL);

        const potential_landscape land{lyt};

        const auto cd1 = land.evaluate(charge_distribution{lyt, charge_state::NEGATIVE});
        const auto cd2 = land.evaluate(charge_distribution{lyt, charge_state::NEUTRAL});
        const auto cd3 = land.evaluate(charge_distribution{lyt, charge_state::POSITIVE});

        CHECK_THAT(cd2.energy(), Catch::Matchers::WithinAbs(0.0, 0.00001));
        CHECK(cd2.energy() < cd3.energy());
        CHECK(cd2.energy() < cd1.energy());

        result results{};
        results.lyt                  = lyt;
        results.charge_distributions = {cd1, cd2, cd3};
        results.algorithm_name       = "test";

        const auto ground_state = results.groundstates();
        REQUIRE(ground_state.size() == 1);
        CHECK(ground_state.front().same_charge_states(cd2));
        CHECK(results.charge_state(1, {5, 2, 0}) == charge_state::NEUTRAL);
        CHECK(results.charge_state(0, {5, 2, 0}) == charge_state::NEGATIVE);
        CHECK(results.charge_state(0, {9, 9, 0}) == charge_state::NONE);
    }
    SECTION("Several charge distributions with degeneracy")
    {
        layout lyt{};

        lyt.assign_cell_type({5, 2, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);

        const potential_landscape land{lyt};

        charge_distribution cd1{lyt};
        cd1.assign_charge_state({5, 2, 0}, charge_state::NEUTRAL);
        cd1.assign_charge_state({6, 2, 0}, charge_state::NEGATIVE);
        cd1 = land.evaluate(cd1);

        charge_distribution cd2{lyt};
        cd2.assign_charge_state({5, 2, 0}, charge_state::NEGATIVE);
        cd2.assign_charge_state({6, 2, 0}, charge_state::NEUTRAL);
        cd2 = land.evaluate(cd2);

        const auto cd3 = land.evaluate(charge_distribution{lyt, charge_state::POSITIVE});
        const auto cd4 = cd2;

        CHECK_THAT(cd2.energy() - cd1.energy(), Catch::Matchers::WithinAbs(0.0, 0.00001));

        result results{};
        results.lyt                  = lyt;
        results.charge_distributions = {cd1, cd2, cd3, cd4};
        results.algorithm_name       = "test";

        const auto ground_states = results.groundstates();
        REQUIRE(ground_states.size() == 2);
        CHECK_THAT(ground_states[0].energy() - ground_states[1].energy(), Catch::Matchers::WithinAbs(0.0, 0.00001));
    }
    SECTION("Empty result")
    {
        const result results{};
        CHECK(results.groundstates().empty());
    }
}

TEST_CASE("Determine the groundstate from simulation results for Si-111 lattice orientation",
          "[sidb-simulation-result]")
{
    SECTION("Three charge distributions with a degenerated ground state")
    {
        layout lyt{lattice::si_111_1x1()};

        lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({2, 0, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({4, 0, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({0, 1, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({2, 1, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({4, 1, 1}, sidb_technology::cell_type::NORMAL);

        const simulation_parameters params{2, -0.30};
        const auto                  results = exhaustive_ground_state_simulation(lyt, params);

        const auto ground_state = results.groundstates();
        REQUIRE(ground_state.size() == 2);
        CHECK_THAT(ground_state.front().energy(), Catch::Matchers::WithinAbs(0.29683, 0.00001));
        CHECK_THAT(ground_state.front().energy() - ground_state.back().energy(),
                   Catch::Matchers::WithinAbs(0.0, 0.00001));
    }
}

TEST_CASE("Determine the groundstate of a two BDL pair wire with input 1 applied", "[sidb-simulation-result]")
{
    layout lyt{};

    lyt.assign_cell_type({2, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({6, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 0, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({14, 0, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({18, 0, 0}, sidb_technology::cell_type::NORMAL);

    const simulation_parameters params{2, -0.32};

    const auto results = exhaustive_ground_state_simulation(lyt, params);

    const auto ground_state = results.groundstates();
    REQUIRE(ground_state.size() == 2);
}
