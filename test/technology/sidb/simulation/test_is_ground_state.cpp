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
 * @brief Tests for `fiction/technology/sidb/simulation/is_ground_state.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/charge_state.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/engines/exhaustive_ground_state_simulation.hpp>
#include <fiction/technology/sidb/simulation/engines/quicksim.hpp>
#include <fiction/technology/sidb/simulation/is_ground_state.hpp>
#include <fiction/technology/sidb/simulation/result.hpp>
#include <fiction/technology/sidb/technology.hpp>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation;
using namespace fiction::sidb::simulation::engines;

TEST_CASE("check if ground state is found", "[is-ground-state]")
{
    SECTION("degenerate ground state")
    {
        layout lyt{};
        lyt.assign_cell_type({1, 3, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({3, 3, 0}, sidb_technology::cell_type::NORMAL);

        charge_distribution first{lyt};
        first.assign_charge_state({1, 3, 0}, charge_state::NEGATIVE);
        first.assign_charge_state({3, 3, 0}, charge_state::NEUTRAL);

        charge_distribution second{lyt};
        second.assign_charge_state({1, 3, 0}, charge_state::NEUTRAL);
        second.assign_charge_state({3, 3, 0}, charge_state::NEGATIVE);

        result simulation_result_exhaustive{};
        simulation_result_exhaustive.lyt                  = lyt;
        simulation_result_exhaustive.charge_distributions = {first, second};

        SECTION("heuristic only finds one ground state")
        {
            result simulation_result_heuristic{};
            simulation_result_heuristic.lyt                  = lyt;
            simulation_result_heuristic.charge_distributions = {first};
            CHECK(!is_ground_state(simulation_result_heuristic, simulation_result_exhaustive));
        }

        SECTION("two identical ground states are stored in the simulation results")
        {
            simulation_result_exhaustive.charge_distributions = {first, first};
            result simulation_result_heuristic{};
            simulation_result_heuristic.lyt                  = lyt;
            simulation_result_heuristic.charge_distributions = {first};
            CHECK(is_ground_state(simulation_result_heuristic, simulation_result_exhaustive));
        }

        SECTION("heuristic finds both ground states")
        {
            result simulation_result_heuristic{};
            simulation_result_heuristic.lyt                  = lyt;
            simulation_result_heuristic.charge_distributions = {second, first};
            CHECK(is_ground_state(simulation_result_heuristic, simulation_result_exhaustive));
        }

        SECTION("no exact result")
        {
            CHECK(!is_ground_state(simulation_result_exhaustive, result{}));
        }
    }

    SECTION("layout with no SiDB placed")
    {
        const layout                    lyt{};
        constexpr simulation_parameters params{2, -0.32};
        const auto                      simulation_results_exgs = exhaustive_ground_state_simulation(lyt, params);
        const quicksim_params           qs_params{params};
        const auto                      simulation_results_quicksim = quicksim(lyt, qs_params);

        REQUIRE(!simulation_results_quicksim.has_value());
        CHECK(simulation_results_exgs.charge_distributions.empty());
    }

    SECTION("Layout with seven SiDBs placed")
    {
        layout lyt{};

        lyt.assign_cell_type({1, 3, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({3, 3, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({4, 3, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({6, 3, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({7, 3, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({6, 10, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({7, 10, 0}, sidb_technology::cell_type::NORMAL);

        constexpr simulation_parameters params{2, -0.32};

        const auto simulation_results_exgs = exhaustive_ground_state_simulation(lyt, params);

        const quicksim_params qs_params{params};
        const auto            simulation_results_quicksim = quicksim(lyt, qs_params);

        REQUIRE(simulation_results_quicksim.has_value());

        CHECK(is_ground_state(simulation_results_quicksim.value(), simulation_results_exgs));
    }
}
