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
 * @brief Tests for `fiction/technology/sidb/simulation/engines/quickexact.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 * @author Willem Lambooy (wlambooy)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/technology/sidb/cell_level_layout_conversion.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/charge_state.hpp>
#include <fiction/technology/sidb/model/defect.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/engines/exhaustive_ground_state_simulation.hpp>
#include <fiction/technology/sidb/simulation/engines/quickexact.hpp>
#include <fiction/technology/sidb/simulation/result.hpp>
#include <fiction/technology/sidb/technology.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/math/math_utils.hpp>

#include <algorithm>
#include <any>
#include <cstdint>
#include <set>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation;
using namespace fiction::sidb::simulation::engines;
using namespace fiction::utils::math;

TEST_CASE("Empty layout QuickExact simulation", "[quickexact]")
{
    const layout lyt{};

    const quickexact_params params{.sim_params = simulation_parameters{2, -0.32}};

    const auto simulation_results = quickexact(lyt, params);

    CHECK(simulation_results.charge_distributions.empty());
    CHECK(simulation_results.algorithm_name == "QuickExact");
}

TEST_CASE("Single SiDB QuickExact simulation", "[quickexact]")
{
    layout lyt{};
    lyt.assign_cell_type({1, 3, 0}, sidb_technology::cell_type::NORMAL);

    const quickexact_params params{.sim_params = simulation_parameters{2, -0.32}};

    const auto simulation_results = quickexact(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == charge_state::NEGATIVE);
}

TEST_CASE("Single SiDB QuickExact simulation with one negatively charge defect (default initialization) in proximity",
          "[quickexact]")
{
    layout lyt{};
    lyt.assign_cell_type({1, 3, 0}, sidb_technology::cell_type::NORMAL);
    const quickexact_params params{.sim_params = simulation_parameters{2, -0.25}};
    lyt.assign_defect({1, 2, 0},
                      defect{defect_type::UNKNOWN, -1, params.sim_params.epsilon_r, params.sim_params.lambda_tf});

    const auto simulation_results = quickexact(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == charge_state::NEUTRAL);
}

TEST_CASE("Single SiDB QuickExact simulation with one negatively charge defect (changed lambda_tf) in proximity",
          "[quickexact]")
{
    layout lyt{};
    lyt.assign_cell_type({1, 3, 0}, sidb_technology::cell_type::NORMAL);

    const quickexact_params params{.sim_params = simulation_parameters{2, -0.25}};

    lyt.assign_defect({1, 2, 0}, defect{defect_type::UNKNOWN, -1, params.sim_params.epsilon_r, 2});
    const auto simulation_results = quickexact(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == charge_state::NEGATIVE);
}

TEST_CASE("Single SiDB QuickExact simulation with one negatively charge defect (changed epsilon_r) in proximity",
          "[quickexact]")
{
    layout lyt{};
    lyt.assign_cell_type({1, 3, 0}, sidb_technology::cell_type::NORMAL);

    const quickexact_params params{.sim_params = simulation_parameters{2, -0.25}};

    lyt.assign_defect({1, 6, 0}, defect{defect_type::UNKNOWN, -1, 0.3, params.sim_params.lambda_tf});

    const auto simulation_results = quickexact(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.lyt.num_defects() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == charge_state::POSITIVE);
}

TEST_CASE("four SiDBs QuickExact simulation with one negatively charge defect (changed mu_minus) in proximity",
          "[quickexact]")
{
    layout lyt{};
    lyt.assign_cell_type({-2, 0, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, 0, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({0, 1, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::NORMAL);

    const quickexact_params params{.sim_params = simulation_parameters{2, -0.15}};

    lyt.assign_defect({0, 0, 1},
                      defect{defect_type::UNKNOWN, -1, params.sim_params.epsilon_r, params.sim_params.lambda_tf});

    const auto simulation_results = quickexact(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.lyt.num_defects() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == charge_state::NEUTRAL);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(1) == charge_state::NEUTRAL);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(2) == charge_state::NEUTRAL);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(3) == charge_state::NEUTRAL);
}

TEST_CASE("Single SiDB QuickExact simulation with one highly negatively charge defect in proximity", "[quickexact]")
{
    layout lyt{};
    lyt.assign_cell_type({1, 3, 0}, sidb_technology::cell_type::NORMAL);

    const quickexact_params params{.sim_params = simulation_parameters{3, -0.1}};

    lyt.assign_defect({1, 2, 0},
                      defect{defect_type::UNKNOWN, -10, params.sim_params.epsilon_r, params.sim_params.lambda_tf});
    const auto simulation_results = quickexact(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.lyt.num_defects() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == charge_state::POSITIVE);
}

TEST_CASE(
    "Single SiDB QuickExact simulation with one highly negatively charge defect in proximity but with high screening",
    "[quickexact]")
{
    layout lyt{};
    lyt.assign_cell_type({1, 3, 0}, sidb_technology::cell_type::NORMAL);

    const quickexact_params params{.sim_params = simulation_parameters{2, -0.1}};

    lyt.assign_defect(
        {1, 2, 0}, defect{defect_type::UNKNOWN, -10, params.sim_params.epsilon_r, params.sim_params.lambda_tf * 10E-5});

    const auto simulation_results = quickexact(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == charge_state::NEGATIVE);
}

TEST_CASE("Single SiDB QuickExact simulation with two highly negatively and oppositely charged defects in proximity",
          "[quickexact]")
{
    layout lyt{};
    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);

    const quickexact_params params{.sim_params = simulation_parameters{2, -0.1}};

    lyt.assign_defect({2, 0, 0},
                      defect{defect_type::UNKNOWN, -10, params.sim_params.epsilon_r, params.sim_params.lambda_tf});
    lyt.assign_defect({-2, 0, 0},
                      defect{defect_type::UNKNOWN, 10, params.sim_params.epsilon_r, params.sim_params.lambda_tf});

    const auto simulation_results = quickexact(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.lyt.num_defects() == 2);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == charge_state::NEGATIVE);
}

TEST_CASE("Single SiDB QuickExact simulation with local external potential", "[quickexact]")
{
    layout lyt{};
    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);

    quickexact_params params{.sim_params = simulation_parameters{2, -0.25}};

    params.local_external_potential.insert({{0, 0, 0}, -0.5});

    const auto simulation_results = quickexact(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == charge_state::NEUTRAL);
}

TEST_CASE("Single SiDB QuickExact simulation with local external potential (high)", "[quickexact]")
{
    layout lyt{};
    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);

    quickexact_params params{.sim_params = simulation_parameters{3, -0.25}};

    params.local_external_potential.insert({{{0, 0, 0}, -1}});
    const auto simulation_results = quickexact(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == charge_state::POSITIVE);
}

TEST_CASE("Single SiDB QuickExact simulation with global external potential", "[quickexact]")
{
    layout lyt{};
    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);

    quickexact_params params{.sim_params = simulation_parameters{2, -0.25}};
    params.global_potential = -0.26;

    const auto simulation_results = quickexact(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(std::any_cast<double>(simulation_results.additional_simulation_parameters.at("global_potential")) == -0.26);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == charge_state::NEUTRAL);
}

TEST_CASE("Single SiDB QuickExact simulation with global external potential (high)", "[quickexact]")
{
    layout lyt{};
    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);

    quickexact_params params{.sim_params = simulation_parameters{3, -0.25}};
    params.global_potential = -1;

    const auto simulation_results = quickexact(lyt, params);
    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == charge_state::POSITIVE);
}

TEST_CASE("Single SiDB QuickExact simulation with global external potential (high, positive)", "[quickexact]")
{
    layout lyt{};
    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);

    quickexact_params params{.sim_params = simulation_parameters{3, -0.25}};
    params.global_potential = 1;

    const auto simulation_results = quickexact(lyt, params);
    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == charge_state::NEGATIVE);
}

TEST_CASE("QuickExact simulation of a BDL pair", "[quickexact]")
{
    layout lyt{};
    lyt.assign_cell_type({1, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({3, 3, 0}, sidb_technology::cell_type::NORMAL);

    const quickexact_params params{.sim_params = simulation_parameters{3, -0.25}};

    const auto simulation_results = quickexact(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 2);
    for (const auto& layouts : simulation_results.charge_distributions)
    {
        uint64_t counter_negative = 0;
        uint64_t counter_neutral  = 0;
        for (uint64_t i = 0; i < 2; i++)
        {
            if (layouts.get_charge_state_by_index(i) == charge_state::NEGATIVE)
            {
                counter_negative += 1;
            }
            else
            {
                counter_neutral += 1;
            }
        }
        CHECK(counter_neutral == 1);
        CHECK(counter_negative == 1);
    }
}

TEST_CASE("QuickExact simulation of a two-pair BDL wire with one perturber", "[quickexact]")
{
    layout lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({5, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({11, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({13, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({17, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({19, 0, 0}, sidb_technology::cell_type::NORMAL);

    const quickexact_params params{.sim_params = simulation_parameters{3, -0.32}};

    const auto simulation_results = quickexact(lyt, params);
    auto       size_before        = simulation_results.charge_distributions.size();

    const auto simulation_results_new = quickexact(lyt, params);
    auto       size_after             = simulation_results_new.charge_distributions.size();

    CHECK(size_before == 1);
    CHECK(size_after == 1);

    REQUIRE(!simulation_results_new.charge_distributions.empty());

    const auto& charge_lyt_first = simulation_results_new.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({0, 0, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({5, 0, 0}) == charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({7, 0, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({11, 0, 0}) == charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({13, 0, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({17, 0, 0}) == charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({19, 0, 0}) == charge_state::NEGATIVE);

    CHECK_THAT(charge_lyt_first.energy(), Catch::Matchers::WithinAbs(0.2460493219, ERROR_MARGIN));
}

TEST_CASE("QuickExact simulation of a one-pair BDL wire with two perturbers", "[quickexact]")
{
    layout lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({5, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({15, 0, 0}, sidb_technology::cell_type::NORMAL);

    const simulation_parameters params{2, -0.32};

    const quickexact_params sim_params{.sim_params = simulation_parameters{3, -0.32}};

    const auto simulation_results = quickexact(lyt, sim_params);

    REQUIRE(!simulation_results.charge_distributions.empty());

    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({0, 0, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({5, 0, 0}) == charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({7, 0, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({15, 0, 0}) == charge_state::NEGATIVE);

    CHECK_THAT(charge_lyt_first.energy(), Catch::Matchers::WithinAbs(0.1152677452, ERROR_MARGIN));
}

TEST_CASE("QuickExact simulation of a Y-shaped SiDB arrangement", "[quickexact]")
{
    layout lyt{};

    lyt.assign_cell_type({-11, -2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({-10, -1, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({-4, -1, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({-3, -2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({-7, 0, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({-7, 1, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({-7, 3, 0}, sidb_technology::cell_type::NORMAL);

    const quickexact_params sim_params{.sim_params = simulation_parameters{3, -0.32}};

    const auto simulation_results = quickexact(lyt, sim_params);

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

TEST_CASE("QuickExact simulation of a Y-shaped SiDB OR gate with input 01, check energy and charge "
          "distribution, using siqad coordinates",
          "[quickexact]")
{
    layout lyt{};

    lyt.assign_cell_type({16, 1, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 2, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({8, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 3, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, 5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({10, 6, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, 8, 1}, sidb_technology::cell_type::NORMAL);

    const quickexact_params sim_params{.sim_params = simulation_parameters{2, -0.28}};

    const auto simulation_results = quickexact(lyt, sim_params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == charge_state::NEGATIVE);

    CHECK_THAT(charge_lyt_first.energy(), Catch::Matchers::WithinAbs(0.4662582096, ERROR_MARGIN));
}

TEST_CASE(
    "QuickExact simulation of a Y-shaped SiDB OR gate with input 01 and local external potential at perturber, using "
    "siqad coordinates",
    "[quickexact]")
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

    quickexact_params params{.sim_params = simulation_parameters{3, -0.28}};
    params.local_external_potential.insert({{{6, 2, 0}, -0.5}});

    const auto simulation_results = quickexact(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == charge_state::NEGATIVE);
}

TEST_CASE("QuickExact simulation  of a Y-shaped SiDB OR gate with input 01 and global external potential",
          "[quickexact]")
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

    quickexact_params params{.sim_params = simulation_parameters{3, -0.28}};
    params.global_potential = -0.5;

    const auto simulation_results = quickexact(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == charge_state::NEUTRAL);
}

TEST_CASE("QuickExact simulation of a Y-shaped SiDB OR gate with input 01 and global external potential (high)",
          "[quickexact]")
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

    quickexact_params params{.sim_params = simulation_parameters{3, -0.28}};
    params.global_potential = -2;

    const auto simulation_results = quickexact(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == charge_state::POSITIVE);
    CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == charge_state::POSITIVE);
    CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == charge_state::POSITIVE);
    CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == charge_state::POSITIVE);
    CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == charge_state::POSITIVE);
    CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == charge_state::POSITIVE);
    CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == charge_state::POSITIVE);
    CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == charge_state::POSITIVE);
}

TEST_CASE("QuickExact simulation of four SiDBs (far away)", "[quickexact]")
{
    layout lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({10, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({20, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({30, 0, 0}, sidb_technology::cell_type::NORMAL);

    const quickexact_params params{.sim_params = simulation_parameters{3, -0.28}};

    const auto simulation_results = quickexact(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({0, 0, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({10, 0, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({20, 0, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({30, 0, 0}) == charge_state::NEGATIVE);
}

TEST_CASE("QuickExact with one SiDB and one negatively charged defect in proximity", "[quickexact]")
{
    layout lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);

    const quickexact_params params{.sim_params = simulation_parameters{3, -0.32}};
    lyt.assign_defect({-1, -1, 1},
                      defect{defect_type::UNKNOWN, -1, params.sim_params.epsilon_r, params.sim_params.lambda_tf});
    const auto simulation_results = quickexact(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({0, 0, 0}) == charge_state::NEUTRAL);
}

TEST_CASE("QuickExact simulation  of four SiDBs (far away) with one negatively charged defects in proximity",
          "[quickexact]")
{
    layout lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({10, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({20, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({30, 0, 0}, sidb_technology::cell_type::NORMAL);

    const quickexact_params params{.sim_params = simulation_parameters{3, -0.28}};
    lyt.assign_defect({1, 0, 0},
                      defect{defect_type::UNKNOWN, -1, params.sim_params.epsilon_r, params.sim_params.lambda_tf});
    const auto simulation_results = quickexact(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({0, 0, 0}) == charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({10, 0, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({20, 0, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({30, 0, 0}) == charge_state::NEGATIVE);
}

TEST_CASE("QuickExact simulation of four SiDBs (far away) with two negatively charged defects in proximity",
          "[quickexact]")
{
    layout lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({10, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({20, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({30, 0, 0}, sidb_technology::cell_type::NORMAL);

    const quickexact_params params{.sim_params = simulation_parameters{3, -0.28}};

    lyt.assign_defect({1, 0, 0},
                      defect{defect_type::UNKNOWN, -1, params.sim_params.epsilon_r, params.sim_params.lambda_tf});
    lyt.assign_defect({31, 0, 0},
                      defect{defect_type::UNKNOWN, -1, params.sim_params.epsilon_r, params.sim_params.lambda_tf});

    const auto simulation_results = quickexact(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    CHECK(simulation_results.lyt.num_defects() == 2);
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({0, 0, 0}) == charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({10, 0, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({20, 0, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({30, 0, 0}) == charge_state::NEUTRAL);
}

TEST_CASE(
    "QuickExact simulation of four SiDBs (far away) with one negatively and positively charged defect in proximity",
    "[quickexact]")
{
    layout lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({10, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({20, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({30, 0, 0}, sidb_technology::cell_type::NORMAL);

    const quickexact_params params{.sim_params = simulation_parameters{3, -0.28}};

    lyt.assign_defect({1, 0, 0},
                      defect{defect_type::UNKNOWN, 1, params.sim_params.epsilon_r, params.sim_params.lambda_tf});
    lyt.assign_defect({31, 0, 0},
                      defect{defect_type::UNKNOWN, -1, params.sim_params.epsilon_r, params.sim_params.lambda_tf});
    const auto simulation_results = quickexact(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    CHECK(simulation_results.lyt.num_defects() == 2);
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({0, 0, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({10, 0, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({20, 0, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({30, 0, 0}) == charge_state::NEUTRAL);
}

TEST_CASE("Seven randomly distributed DBs, test if dependent cell calculation works correctly", "[quickexact]")
{
    layout lyt{};

    lyt.assign_cell_type({1, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({3, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({4, 3, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({6, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({7, 3, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({6, 10, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({7, 10, 0}, sidb_technology::cell_type::NORMAL);

    const quickexact_params params{.sim_params = simulation_parameters{3, -0.28}};

    const auto simulation_results = quickexact(lyt, params);

    const auto simulation_results_exgs = exhaustive_ground_state_simulation(lyt, params.sim_params);

    REQUIRE(simulation_results.charge_distributions.size() == simulation_results_exgs.charge_distributions.size());

    const auto highest_state =
        std::ranges::min_element(simulation_results.charge_distributions,
                                 [](const auto& lhs, const auto& rhs) { return lhs.energy() > rhs.energy(); });

    CHECK(highest_state->get_charge_state({1, 3, 0}) == charge_state::NEGATIVE);
    CHECK(highest_state->get_charge_state({3, 3, 0}) == charge_state::NEUTRAL);
    CHECK(highest_state->get_charge_state({4, 3, 0}) == charge_state::NEUTRAL);
    CHECK(highest_state->get_charge_state({6, 3, 0}) == charge_state::NEUTRAL);
    CHECK(highest_state->get_charge_state({7, 3, 0}) == charge_state::NEGATIVE);
    CHECK(highest_state->get_charge_state({6, 10, 0}) == charge_state::NEUTRAL);
    CHECK(highest_state->get_charge_state({7, 10, 0}) == charge_state::NEGATIVE);
}

TEST_CASE("three DBs next to each other", "[quickexact]")
{
    layout lyt{};

    lyt.assign_cell_type({-1, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({1, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({3, 3, 0}, sidb_technology::cell_type::NORMAL);

    const quickexact_params params{.sim_params = simulation_parameters{2, -0.25}};

    const auto simulation_results = quickexact(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 4);

    const auto ground_state =
        std::ranges::min_element(simulation_results.charge_distributions,
                                 [](const auto& lhs, const auto& rhs) { return lhs.energy() < rhs.energy(); });

    CHECK(ground_state->get_charge_state({-1, 3, 0}) == charge_state::NEGATIVE);
    CHECK(ground_state->get_charge_state({1, 3, 0}) == charge_state::POSITIVE);
    CHECK(ground_state->get_charge_state({2, 3, 0}) == charge_state::NEGATIVE);
    CHECK(ground_state->get_charge_state({3, 3, 0}) == charge_state::NEUTRAL);
}

TEST_CASE("three DBs next to each other, small mu-", "[quickexact]")
{
    layout lyt{};

    lyt.assign_cell_type({-1, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({1, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({3, 3, 0}, sidb_technology::cell_type::NORMAL);

    const quickexact_params params{.sim_params = simulation_parameters{2, -0.8}};

    const auto simulation_results = quickexact(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();
    CHECK(charge_lyt_first.get_charge_state({1, 3, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({1, 3, 0}) == charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({2, 3, 0}) == charge_state::POSITIVE);
    CHECK(charge_lyt_first.get_charge_state({3, 3, 0}) == charge_state::NEGATIVE);
}

TEST_CASE("four DBs next to each other, small mu-", "[quickexact]")
{
    layout lyt{};

    lyt.assign_cell_type({0, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({1, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({3, 3, 0}, sidb_technology::cell_type::NORMAL);

    const quickexact_params params{.sim_params = simulation_parameters{3, -0.25}};

    const auto simulation_results = quickexact(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 4);
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();
    CHECK_THAT(charge_lyt_first.energy(), Catch::Matchers::WithinAbs(0, ERROR_MARGIN));
}

TEST_CASE("seven DBs next to each other, small mu-", "[quickexact]")
{
    layout lyt{};

    lyt.assign_cell_type({0, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({1, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({3, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({4, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({5, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 3, 0}, sidb_technology::cell_type::NORMAL);

    const quickexact_params params{.sim_params = simulation_parameters{3, -0.25}};

    const auto simulation_results = quickexact(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 10);
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();
    CHECK(charge_lyt_first.energy() < 0.08);
    CHECK(charge_lyt_first.energy() > -2.74);
}

TEST_CASE("7 DBs next to each other (positively charged DBs occur)", "[quickexact]")
{
    layout lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({1, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({3, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({4, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({5, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, sidb_technology::cell_type::NORMAL);

    const quickexact_params params{.sim_params = simulation_parameters{3, -0.25}};

    const auto simulation_results = quickexact(lyt, params);

    CHECK(simulation_results.charge_distributions.size() == 17);
}

TEST_CASE("7 DBs next to each other | only one physically valid charge distribution with only one neutrally charged DB",
          "[quickexact]")
{
    layout lyt{};

    lyt.assign_cell_type({-6, 1, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, 4, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({4, 6, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({-8, -3, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({-1, -1, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({0, 2, 0}, sidb_technology::cell_type::NORMAL);

    const quickexact_params params{.sim_params = simulation_parameters{3, -0.25}};

    const auto simulation_results = quickexact(lyt, params);

    CHECK(simulation_results.charge_distributions.size() == 1);
}

TEST_CASE("4 DBs next to each other (positively charged DBs occur)", "[quickexact]")
{
    layout lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({1, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({10, 0, 0}, sidb_technology::cell_type::NORMAL);

    const quickexact_params params{.sim_params = simulation_parameters{3, -0.1}};

    const auto simulation_results = quickexact(lyt, params);

    CHECK(simulation_results.charge_distributions.size() == 2);
}

TEST_CASE("6 DBs next to each other (positively charged DBs occur)", "[quickexact]")
{
    layout lyt{};

    lyt.assign_cell_type({-1, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({3, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({10, 0, 0}, sidb_technology::cell_type::NORMAL);

    const quickexact_params params{.sim_params = simulation_parameters{3, -0.25}};

    const auto simulation_results = quickexact(lyt, params);

    CHECK(lyt.num_cells() == 6);

    CHECK(simulation_results.charge_distributions.size() == 3);
}

TEST_CASE("4 DBs close to each other", "[quickexact]")
{
    layout lyt{};

    lyt.assign_cell_type({0, 0, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({3, 0, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({5, 0, 1}, sidb_technology::cell_type::NORMAL);

    const quickexact_params params{.sim_params = simulation_parameters{3, -0.25}};

    const auto simulation_results = quickexact(lyt, params);

    CHECK(!simulation_results.charge_distributions.empty());
}

TEST_CASE("3 DBs next to each other (positively charged DBs occur)", "[quickexact]")
{

    layout lyt{};

    lyt.assign_cell_type({5, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, sidb_technology::cell_type::NORMAL);

    const quickexact_params params{.sim_params = simulation_parameters{3, -0.32}};

    const auto simulation_results = quickexact(lyt, params);

    for (const auto& layout : simulation_results.charge_distributions)
    {
        CHECK(round_to_n_decimal_places(layout.energy(), 1) <= 0);
    }
}

TEST_CASE("3 DBs next to each other with automatic base number detection", "[quickexact]")
{
    layout lyt{};

    lyt.assign_cell_type({5, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, sidb_technology::cell_type::NORMAL);

    const quickexact_params params{.sim_params = simulation_parameters{3, -0.32}};

    const auto simulation_results = quickexact(lyt, params);

    REQUIRE(!simulation_results.additional_simulation_parameters.empty());
    CHECK(std::any_cast<uint64_t>(simulation_results.additional_simulation_parameters.at("base_number")) == 3);

    const quickexact_params params_new{.sim_params = simulation_parameters{2, -0.32},
                                       .base_number_detection =
                                           quickexact_params::automatic_base_number_detection::OFF};

    const auto simulation_results_new = quickexact(lyt, params_new);

    REQUIRE(!simulation_results_new.additional_simulation_parameters.empty());
    CHECK(std::any_cast<uint64_t>(simulation_results_new.additional_simulation_parameters.at("base_number")) == 2);
}

TEST_CASE("13 DBs which are all negatively charged", "[quickexact]")
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

    const quickexact_params params{.sim_params = simulation_parameters{3, -0.32}};

    const auto simulation_results = quickexact(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());

    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    charge_lyt_first.foreach_cell([&](const auto& cell)
                                  { CHECK(charge_lyt_first.get_charge_state(cell) == charge_state::NEGATIVE); });

    CHECK(lyt.num_cells() == 13);
}

TEST_CASE("QuickExact simulation of a Y-shaped SiDB OR gate with input 01", "[quickexact]")
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

    quickexact_params params{.sim_params            = simulation_parameters{2, -0.28},
                             .base_number_detection = quickexact_params::automatic_base_number_detection::OFF};

    SECTION("Check if QuickExact is deterministic")
    {
        SECTION("epsilon_r = 8")
        {
            params.sim_params.epsilon_r = 8;
            std::set<double>   ground_state{};
            std::set<uint64_t> charge_index{};
            for (auto i = 0; i < 10000; i++)
            {
                const auto  simulation_results = quickexact(lyt, params);
                const auto& charge_lyt_first   = simulation_results.charge_distributions.front();
                ground_state.insert(charge_lyt_first.energy());
                charge_index.insert(charge_lyt_first.charge_index(3));
            }
            CHECK(ground_state.size() == 1);
            CHECK(charge_index.size() == 1);
        }
        SECTION("epsilon_r = 2")
        {
            params.sim_params.epsilon_r = 2;
            std::set<double>   ground_state{};
            std::set<uint64_t> charge_index{};
            for (auto i = 0; i < 10000; i++)
            {
                const auto  simulation_results = quickexact(lyt, params);
                const auto& charge_lyt_first   = simulation_results.charge_distributions.front();
                ground_state.insert(charge_lyt_first.energy());
                charge_index.insert(charge_lyt_first.charge_index(3));
            }
            CHECK(ground_state.size() == 1);
            CHECK(charge_index.size() == 1);
        }
    }

    SECTION("Add SiDBs which are positively charged in the ground state, layout does not fulfill the logic anymore.")
    {
        params.sim_params.base = 3;
        lyt.assign_cell_type({15, 2, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({15, 2, 0}, sidb_technology::cell_type::NORMAL);

        const auto simulation_results = quickexact(lyt, params);
        // find the ground state, which is the charge distribution with the lowest energy
        const auto ground_state =
            std::ranges::min_element(simulation_results.charge_distributions,
                                     [](const auto& lhs, const auto& rhs) { return lhs.energy() < rhs.energy(); });

        CHECK(ground_state->num_positive_sidbs() > 0);
    }

    SECTION("Standard Physical Parameters")
    {
        const auto simulation_results = quickexact(lyt, params);

        REQUIRE(!simulation_results.charge_distributions.empty());
        const auto& charge_lyt_first = simulation_results.charge_distributions.front();

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
        params.sim_params.mu_minus = -0.1;

        const auto simulation_results = quickexact(lyt, params);

        REQUIRE(!simulation_results.charge_distributions.empty());
        const auto& charge_lyt_first = simulation_results.charge_distributions.front();

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
        params.sim_params.mu_minus = -0.7;

        const auto simulation_results = quickexact(lyt, params);

        REQUIRE(!simulation_results.charge_distributions.empty());
        const auto& charge_lyt_first = simulation_results.charge_distributions.front();

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
        params.sim_params.lambda_tf = 1;

        const auto simulation_results = quickexact(lyt, params);

        REQUIRE(!simulation_results.charge_distributions.empty());
        const auto& charge_lyt_first = simulation_results.charge_distributions.front();

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
        params.sim_params.lambda_tf = 10;

        const auto simulation_results = quickexact(lyt, params);

        REQUIRE(!simulation_results.charge_distributions.empty());
        const auto& charge_lyt_first = simulation_results.charge_distributions.front();

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
        params.sim_params.epsilon_r = 10;

        const auto simulation_results = quickexact(lyt, params);

        REQUIRE(!simulation_results.charge_distributions.empty());
        const auto& charge_lyt_first = simulation_results.charge_distributions.front();

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

// to save runtime in the CI, this test is only run in RELEASE mode
#ifdef NDEBUG
TEST_CASE("QuickExact simulation of a Y-shaped SiDB OR gate with input 01", "[quickexact], [quality]")
{
    SECTION("Check if QuickExact is deterministic")
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

        const quickexact_params params{.sim_params = simulation_parameters{2, -0.28},
                                       .base_number_detection =
                                           quickexact_params::automatic_base_number_detection::OFF};

        std::set<double>   ground_state{};
        std::set<uint64_t> charge_index{};
        for (auto i = 0; i < 100000; i++)
        {
            const auto  simulation_results = quickexact(lyt, params);
            const auto& charge_lyt_first   = simulation_results.charge_distributions.front();
            ground_state.insert(charge_lyt_first.energy());
            charge_index.insert(charge_lyt_first.charge_index(3));
        }
        CHECK(ground_state.size() == 1);
        CHECK(charge_index.size() == 1);
    }

    SECTION("Special layout")
    {
        layout lyt{};

        lyt.assign_cell_type({22, 1, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({24, 2, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({23, 3, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({13, 4, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({10, 4, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({1, 5, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({0, 6, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({1, 6, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({24, 6, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({4, 6, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({3, 7, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({0, 8, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({1, 8, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({9, 9, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({24, 9, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({22, 9, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({13, 10, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({14, 10, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({1, 11, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({17, 11, 1}, sidb_technology::cell_type::NORMAL);

        const quickexact_params params{.sim_params            = simulation_parameters{2, -0.32},
                                       .base_number_detection = quickexact_params::automatic_base_number_detection::ON};

        const auto simulation_results = quickexact(lyt, params);
        CHECK(simulation_results.charge_distributions.size() == 21);
    }
}

TEST_CASE("QuickExact simulation of a 3 DB Wire", "[quickexact]")
{
    layout lyt{};

    // three BDL pairs with one perturber
    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({5, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({12, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({15, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({19, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({22, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({26, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({29, 0, 0}, sidb_technology::cell_type::NORMAL);

    // quickexact parameters are initialized
    quickexact_params params{.sim_params            = simulation_parameters{3, -0.28},
                             .base_number_detection = quickexact_params::automatic_base_number_detection::OFF};

    SECTION("Standard Physical Parameters")
    {
        const auto simulation_results = quickexact(lyt, params);

        // check that physically valid charge distributions were found
        REQUIRE(!simulation_results.charge_distributions.empty());

        // find the ground state, which is the charge distribution with the lowest energy
        const auto ground_states = simulation_results.groundstates();

        REQUIRE(ground_states.size() == 1);

        const auto& gs = ground_states.front();

        CHECK(gs.num_negative_sidbs() == 5);
        CHECK(gs.num_neutral_sidbs() == 4);
        CHECK(gs.num_positive_sidbs() == 0);

        // check that charge distribution is correct; binary 1 is propagated through the BDL wire
        CHECK(gs.get_charge_state({0, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({5, 0, 0}) == charge_state::NEUTRAL);
        CHECK(gs.get_charge_state({8, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({12, 0, 0}) == charge_state::NEUTRAL);
        CHECK(gs.get_charge_state({15, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({19, 0, 0}) == charge_state::NEUTRAL);
        CHECK(gs.get_charge_state({22, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({26, 0, 0}) == charge_state::NEUTRAL);
        CHECK(gs.get_charge_state({29, 0, 0}) == charge_state::NEGATIVE);

        CHECK_THAT(gs.energy(), Catch::Matchers::WithinAbs(0.274134844, ERROR_MARGIN));
    }

    SECTION("Increased mu_minus")
    {
        // set a small absolute value for µ
        params.sim_params.mu_minus = -0.1;

        const auto simulation_results = quickexact(lyt, params);

        REQUIRE(!simulation_results.charge_distributions.empty());

        // find the ground state, which is the charge distribution with the lowest energy
        const auto ground_states = simulation_results.groundstates();

        REQUIRE(ground_states.size() == 1);

        const auto& gs = ground_states.front();

        // check charge distribution of the ground state; BDL wire no longer works as intended
        CHECK(gs.get_charge_state({0, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({5, 0, 0}) == charge_state::NEUTRAL);
        CHECK(gs.get_charge_state({8, 0, 0}) == charge_state::NEUTRAL);
        CHECK(gs.get_charge_state({12, 0, 0}) == charge_state::NEUTRAL);
        CHECK(gs.get_charge_state({15, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({19, 0, 0}) == charge_state::NEUTRAL);
        CHECK(gs.get_charge_state({22, 0, 0}) == charge_state::NEUTRAL);
        CHECK(gs.get_charge_state({26, 0, 0}) == charge_state::NEUTRAL);
        CHECK(gs.get_charge_state({29, 0, 0}) == charge_state::NEGATIVE);

        CHECK_THAT(gs.energy(), Catch::Matchers::WithinAbs(0.0329179963, ERROR_MARGIN));
    }

    SECTION("Decreased mu_minus")
    {
        // set a large absolute value for µ
        params.sim_params.mu_minus = -0.7;

        const auto simulation_results = quickexact(lyt, params);

        REQUIRE(!simulation_results.charge_distributions.empty());

        // find the ground state, which is the charge distribution with the lowest energy
        const auto ground_states = simulation_results.groundstates();

        REQUIRE(ground_states.size() == 1);

        const auto& gs = ground_states.front();

        // Due to the set µ-value, all SiDBs are negatively charged (electrostatic interaction is not strong enough to
        // change the charge state of individual SiDBs).
        CHECK(gs.get_charge_state({0, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({5, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({8, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({12, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({15, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({19, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({22, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({26, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({29, 0, 0}) == charge_state::NEGATIVE);

        CHECK_THAT(gs.energy(), Catch::Matchers::WithinAbs(1.8649862557, ERROR_MARGIN));
    }

    SECTION("Decreased lambda_tf")
    {
        // set small lambda value, i.e., electrostatic screening is significant.
        params.sim_params.lambda_tf = 1;

        const auto simulation_results = quickexact(lyt, params);

        REQUIRE(!simulation_results.charge_distributions.empty());

        const auto ground_states = simulation_results.groundstates();

        REQUIRE(ground_states.size() == 1);

        const auto& gs = ground_states.front();

        // Due to the small lambda value, the electrostatic interaction is small. Hence, all SiDBs are negatively
        // charged.
        CHECK(gs.get_charge_state({0, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({5, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({8, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({12, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({15, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({19, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({22, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({26, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({29, 0, 0}) == charge_state::NEGATIVE);

        CHECK_THAT(gs.energy(), Catch::Matchers::WithinAbs(0.4606785472, ERROR_MARGIN));
    }

    SECTION("Increased lambda_tf")
    {
        // set large lambda value, i.e., electrostatic screening is small.
        params.sim_params.lambda_tf = 10;

        const auto simulation_results = quickexact(lyt, params);

        REQUIRE(!simulation_results.charge_distributions.empty());

        // find the ground state, which is the charge distribution with the lowest energy
        const auto ground_state = simulation_results.groundstates();

        REQUIRE(ground_state.size() == 1);

        const auto& gs = ground_state.front();

        // check charge distribution of the ground state; BDL wire works as intended
        CHECK(gs.get_charge_state({0, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({5, 0, 0}) == charge_state::NEUTRAL);
        CHECK(gs.get_charge_state({8, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({12, 0, 0}) == charge_state::NEUTRAL);
        CHECK(gs.get_charge_state({15, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({19, 0, 0}) == charge_state::NEUTRAL);
        CHECK(gs.get_charge_state({22, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({26, 0, 0}) == charge_state::NEUTRAL);
        CHECK(gs.get_charge_state({29, 0, 0}) == charge_state::NEGATIVE);

        CHECK_THAT(gs.energy(), Catch::Matchers::WithinAbs(0.3967750406, ERROR_MARGIN));
    }

    SECTION("Increased epsilon_r")
    {
        // set large relative permittivity
        params.sim_params.epsilon_r = 10;

        const auto simulation_results = quickexact(lyt, params);

        REQUIRE(!simulation_results.charge_distributions.empty());

        const auto ground_states = simulation_results.groundstates();

        REQUIRE(ground_states.size() == 1);

        const auto& gs = ground_states.front();

        // The electrostatic interaction is small, due to the large relative permittivity.
        // Therefore, all SiDBs are negatively charged.
        CHECK(gs.get_charge_state({0, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({5, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({8, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({12, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({15, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({19, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({22, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({26, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({29, 0, 0}) == charge_state::NEGATIVE);

        CHECK_THAT(gs.energy(), Catch::Matchers::WithinAbs(1.0443923032, ERROR_MARGIN));
    }

    SECTION("Decrease epsilon_r, positively charged SiDBs can occur")
    {
        // set small relative permittivity
        params.sim_params.epsilon_r = 1;

        const auto simulation_results = quickexact(lyt, params);

        REQUIRE(!simulation_results.charge_distributions.empty());

        const auto ground_states = simulation_results.groundstates();

        REQUIRE(ground_states.size() == 1);

        const auto& gs = ground_states.front();

        // The electrostatic interaction is strong, due to the small relative permittivity.
        // Therefore, SiDBs can even be positively charged.
        CHECK(gs.get_charge_state({0, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({5, 0, 0}) == charge_state::POSITIVE);
        CHECK(gs.get_charge_state({8, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({12, 0, 0}) == charge_state::POSITIVE);
        CHECK(gs.get_charge_state({15, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({19, 0, 0}) == charge_state::POSITIVE);
        CHECK(gs.get_charge_state({22, 0, 0}) == charge_state::NEGATIVE);
        CHECK(gs.get_charge_state({26, 0, 0}) == charge_state::POSITIVE);
        CHECK(gs.get_charge_state({29, 0, 0}) == charge_state::NEGATIVE);

        CHECK_THAT(gs.energy(), Catch::Matchers::WithinAbs(-5.0592576221, ERROR_MARGIN));
    }
}

TEST_CASE(
    "QuickExact simulation of two SiDBs placed directly next to each other with non-realistic relative permittivity",
    "[quickexact]")
{
    layout lyt{};
    lyt.assign_cell_type({1, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, 3, 0}, sidb_technology::cell_type::NORMAL);

    SECTION("automatic base number detection is off")
    {
        const quickexact_params params{.sim_params = simulation_parameters{2, -0.32, 1.0e-3},
                                       .base_number_detection =
                                           quickexact_params::automatic_base_number_detection::OFF};

        const auto simulation_results = quickexact(lyt, params);

        CHECK(simulation_results.charge_distributions.empty());
    }

    SECTION("automatic base number detection is on")
    {
        const quickexact_params params{.sim_params            = simulation_parameters{2, -0.32, 1.0e-3},
                                       .base_number_detection = quickexact_params::automatic_base_number_detection::ON};

        const auto simulation_results = quickexact(lyt, params);

        CHECK(simulation_results.charge_distributions.size() == 2);
    }
}

TEST_CASE("QuickExact simulation of positively charged SiDBs", "[quickexact]")
{
    layout lyt{};
    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({4, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({11, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({11, 0, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 0, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({18, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({20, 0, 0}, sidb_technology::cell_type::NORMAL);

    SECTION("automatic base number detection is off, base number is 3")
    {
        const quickexact_params params{.sim_params = simulation_parameters{3, -0.32},
                                       .base_number_detection =
                                           quickexact_params::automatic_base_number_detection::OFF};

        const auto simulation_results = quickexact(lyt, params);

        CHECK(simulation_results.charge_distributions.size() == 4);
    }

    SECTION("automatic base number detection is on, base number is 2")
    {
        const quickexact_params params{.sim_params            = simulation_parameters{2, -0.32},
                                       .base_number_detection = quickexact_params::automatic_base_number_detection::ON};

        const auto simulation_results = quickexact(lyt, params);

        CHECK(simulation_results.charge_distributions.size() == 4);
    }
}

TEST_CASE("Special test cases", "[quickexact]")
{
    SECTION("Test case 1")
    {
        layout lyt{};

        lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({1, 1, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({1, 2, 0}, sidb_technology::cell_type::NORMAL);

        // default physical parameters and automatic base number detection
        const quickexact_params params{.sim_params            = simulation_parameters{2, -0.32},
                                       .base_number_detection = quickexact_params::automatic_base_number_detection::ON};

        const auto simulation_results = quickexact(lyt, params);
        CHECK(simulation_results.charge_distributions.size() == 2);
    }

    SECTION("Test case 2")
    {
        layout lyt{};

        // adding 20 normal cells to the layout
        lyt.assign_cell_type({22, 1, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({24, 2, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({23, 3, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({13, 4, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({10, 4, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({1, 5, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({0, 6, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({1, 6, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({24, 6, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({4, 6, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({3, 7, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({0, 8, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({1, 8, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({9, 9, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({24, 9, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({22, 9, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({13, 10, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({14, 10, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({1, 11, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({17, 11, 1}, sidb_technology::cell_type::NORMAL);

        // default physical parameters with automatic base number detection
        const quickexact_params params{.sim_params            = simulation_parameters{2, -0.32},
                                       .base_number_detection = quickexact_params::automatic_base_number_detection::ON};

        // default physical parameters and automatic base number detection
        const auto simulation_results = quickexact(lyt, params);
        CHECK(simulation_results.charge_distributions.size() == 21);
    }

    SECTION("Test case 3")
    {
        layout lyt{};

        // adding four normal cells to the layout
        lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({1, 0, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({3, 0, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({0, 1, 0}, sidb_technology::cell_type::NORMAL);

        SECTION("automatic base number detection on")
        {
            const quickexact_params params{.sim_params = simulation_parameters{2, -0.32},
                                           .base_number_detection =
                                               quickexact_params::automatic_base_number_detection::ON};

            const auto simulation_results = quickexact(lyt, params);
            CHECK(simulation_results.charge_distributions.size() == 3);
        }

        SECTION("automatic base number detection off")
        {
            const quickexact_params params{.sim_params = simulation_parameters{3, -0.32},
                                           .base_number_detection =
                                               quickexact_params::automatic_base_number_detection::OFF};

            const auto simulation_results = quickexact(lyt, params);
            CHECK(simulation_results.charge_distributions.size() == 3);
        }
    }

    // Test case disabled for now until a small bug in QuickExact is fixed.
    // SECTION("Test case 3")
    // {
    //     layout lyt{};
    //
    //     lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
    //     lyt.assign_cell_type({0, 1, 1}, sidb_technology::cell_type::NORMAL);
    //     lyt.assign_cell_type({9, 3, 0}, sidb_technology::cell_type::NORMAL);
    //     lyt.assign_cell_type({2, 4, 0}, sidb_technology::cell_type::NORMAL);
    //
    //     const quickexact_params params{
    //         sidb_simulation_parameters{3, -0.32},
    //         quickexact_params::automatic_base_number_detection::ON};
    //
    //     const auto simulation_results = quickexact(lyt, params);
    //     CHECK(simulation_results.charge_distributions.size() == 2);
    // }

    SECTION("Test case 4")
    {
        layout lyt{};

        // adding five cells to the layout
        lyt.assign_cell_type({2, 2, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({2, 3, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({7, 3, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({7, 4, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({8, 4, 1}, sidb_technology::cell_type::NORMAL);

        const result& qe_res = quickexact(lyt);

        REQUIRE(qe_res.charge_distributions.size() == 2);
    }

    SECTION("Test case 5")
    {
        layout lyt{};

        // adding 20 cells to the layout
        lyt.assign_cell_type({3, 0, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({4, 0, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({13, 0, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({5, 1, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({22, 3, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({11, 5, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({2, 6, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({4, 6, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({23, 7, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({16, 8, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({8, 8, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({15, 9, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({1, 10, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({12, 10, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({14, 10, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({9, 11, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({24, 11, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({10, 11, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({13, 12, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({24, 12, 0}, sidb_technology::cell_type::NORMAL);

        // default physical parameters
        const simulation_parameters params{3, -0.32};

        result qe_res = quickexact(
            lyt, quickexact_params{.sim_params            = params,
                                   .base_number_detection = quickexact_params::automatic_base_number_detection::ON});

        std::ranges::sort(qe_res.charge_distributions,
                          [](const auto& lhs, const auto& rhs) { return lhs.energy() < rhs.energy(); });

        REQUIRE(qe_res.charge_distributions.size() == 2);

        const auto groundstate  = qe_res.charge_distributions[0];
        const auto excitedstate = qe_res.charge_distributions[1];

        CHECK(groundstate.get_charge_state({3, 0, 0}) == charge_state::NEGATIVE);
        CHECK(groundstate.get_charge_state({4, 0, 0}) == charge_state::POSITIVE);
        CHECK(groundstate.get_charge_state({13, 0, 1}) == charge_state::NEGATIVE);
        CHECK(groundstate.get_charge_state({5, 1, 1}) == charge_state::NEGATIVE);
        CHECK(groundstate.get_charge_state({22, 3, 0}) == charge_state::NEGATIVE);
        CHECK(groundstate.get_charge_state({11, 5, 1}) == charge_state::NEGATIVE);
        CHECK(groundstate.get_charge_state({2, 6, 1}) == charge_state::NEGATIVE);
        CHECK(groundstate.get_charge_state({4, 6, 1}) == charge_state::NEUTRAL);
        CHECK(groundstate.get_charge_state({23, 7, 1}) == charge_state::NEGATIVE);
        CHECK(groundstate.get_charge_state({16, 8, 0}) == charge_state::NEGATIVE);
        CHECK(groundstate.get_charge_state({8, 8, 1}) == charge_state::NEGATIVE);
        CHECK(groundstate.get_charge_state({15, 9, 0}) == charge_state::NEUTRAL);
        CHECK(groundstate.get_charge_state({1, 10, 1}) == charge_state::NEGATIVE);
        CHECK(groundstate.get_charge_state({12, 10, 1}) == charge_state::NEUTRAL);
        CHECK(groundstate.get_charge_state({14, 10, 1}) == charge_state::NEUTRAL);
        CHECK(groundstate.get_charge_state({9, 11, 0}) == charge_state::NEGATIVE);
        CHECK(groundstate.get_charge_state({24, 11, 0}) == charge_state::NEUTRAL);
        CHECK(groundstate.get_charge_state({10, 11, 1}) == charge_state::POSITIVE);
        CHECK(groundstate.get_charge_state({13, 12, 0}) == charge_state::NEGATIVE);
        CHECK(groundstate.get_charge_state({24, 12, 0}) == charge_state::NEGATIVE);

        CHECK(excitedstate.get_charge_state({3, 0, 0}) == charge_state::NEGATIVE);
        CHECK(excitedstate.get_charge_state({4, 0, 0}) == charge_state::POSITIVE);
        CHECK(excitedstate.get_charge_state({13, 0, 1}) == charge_state::NEGATIVE);
        CHECK(excitedstate.get_charge_state({5, 1, 1}) == charge_state::NEGATIVE);
        CHECK(excitedstate.get_charge_state({22, 3, 0}) == charge_state::NEGATIVE);
        CHECK(excitedstate.get_charge_state({11, 5, 1}) == charge_state::NEGATIVE);
        CHECK(excitedstate.get_charge_state({2, 6, 1}) == charge_state::NEGATIVE);
        CHECK(excitedstate.get_charge_state({4, 6, 1}) == charge_state::NEUTRAL);
        CHECK(excitedstate.get_charge_state({23, 7, 1}) == charge_state::NEGATIVE);
        CHECK(excitedstate.get_charge_state({16, 8, 0}) == charge_state::NEGATIVE);
        CHECK(excitedstate.get_charge_state({8, 8, 1}) == charge_state::NEUTRAL);
        CHECK(excitedstate.get_charge_state({15, 9, 0}) == charge_state::NEUTRAL);
        CHECK(excitedstate.get_charge_state({1, 10, 1}) == charge_state::NEGATIVE);
        CHECK(excitedstate.get_charge_state({12, 10, 1}) == charge_state::NEUTRAL);
        CHECK(excitedstate.get_charge_state({14, 10, 1}) == charge_state::NEUTRAL);
        CHECK(excitedstate.get_charge_state({9, 11, 0}) == charge_state::NEGATIVE);
        CHECK(excitedstate.get_charge_state({24, 11, 0}) == charge_state::NEUTRAL);
        CHECK(excitedstate.get_charge_state({10, 11, 1}) == charge_state::NEUTRAL);
        CHECK(excitedstate.get_charge_state({13, 12, 0}) == charge_state::NEGATIVE);
        CHECK(excitedstate.get_charge_state({24, 12, 0}) == charge_state::NEGATIVE);
    }

    // Test case disabled for now until a small bug in QuickExact is fixed.
    // SECTION("Test case 6")
    // {
    //     layout lyt{};
    //
    //     lyt.assign_cell_type({11, 1, 1}, sidb_technology::cell_type::NORMAL);
    //     lyt.assign_cell_type({15, 1, 1}, sidb_technology::cell_type::NORMAL);
    //     lyt.assign_cell_type({5, 2, 1}, sidb_technology::cell_type::NORMAL);
    //     lyt.assign_cell_type({17, 2, 1}, sidb_technology::cell_type::NORMAL);
    //     lyt.assign_cell_type({20, 3, 0}, sidb_technology::cell_type::NORMAL);
    //     lyt.assign_cell_type({13, 3, 1}, sidb_technology::cell_type::NORMAL);
    //     lyt.assign_cell_type({9, 4, 1}, sidb_technology::cell_type::NORMAL);
    //     lyt.assign_cell_type({2, 5, 1}, sidb_technology::cell_type::NORMAL);
    //     lyt.assign_cell_type({17, 5, 1}, sidb_technology::cell_type::NORMAL);
    //     lyt.assign_cell_type({2, 6, 1}, sidb_technology::cell_type::NORMAL);
    //     lyt.assign_cell_type({9, 6, 1}, sidb_technology::cell_type::NORMAL);
    //     lyt.assign_cell_type({10, 7, 0}, sidb_technology::cell_type::NORMAL);
    //     lyt.assign_cell_type({8, 7, 1}, sidb_technology::cell_type::NORMAL);
    //     lyt.assign_cell_type({17, 9, 0}, sidb_technology::cell_type::NORMAL);
    //     lyt.assign_cell_type({11, 9, 1}, sidb_technology::cell_type::NORMAL);
    //     lyt.assign_cell_type({12, 9, 1}, sidb_technology::cell_type::NORMAL);
    //     lyt.assign_cell_type({5, 10, 0}, sidb_technology::cell_type::NORMAL);
    //     lyt.assign_cell_type({5, 10, 1}, sidb_technology::cell_type::NORMAL);
    //     lyt.assign_cell_type({7, 11, 1}, sidb_technology::cell_type::NORMAL);
    //     lyt.assign_cell_type({13, 11, 1}, sidb_technology::cell_type::NORMAL);
    //
    //     const sidb_simulation_parameters params{2, -0.32};
    //
    //     sidb_simulation_result qe_res = quickexact(
    //         lyt,
    //         quickexact_params{params,
    //                                           quickexact_params::automatic_base_number_detection::OFF,
    //                                           {},
    //                                           0});
    //
    //     std::sort(qe_res.charge_distributions.begin(), qe_res.charge_distributions.end(),
    //               [](const auto& lhs, const auto& rhs) { return lhs.energy() <
    //               rhs.energy();
    //               });
    //
    //     CHECK(qe_res.charge_distributions.size() == 2);
    // }
}

TEST_CASE("QuickExact gate simulation of Si-111 surface", "[quickexact]")
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

    const quickexact_params params{.sim_params = simulation_parameters{2, -0.32, 5.6, 5}};

    const auto simulation_results = quickexact(lyt, params);

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

TEST_CASE("QuickExact AND gate simulation of Si-111 surface", "[quickexact]")
{
    SECTION("no input applied")
    {
        const auto              lyt = to_sidb_layout(blueprints::and_gate_111<sidb_111_cell_clk_lyt_siqad>());
        const quickexact_params params{.sim_params = simulation_parameters{2, -0.32, 5.6, 5}};

        const auto simulation_results = quickexact(lyt, params);

        const auto ground_state = simulation_results.groundstates();
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

        const quickexact_params params{.sim_params = simulation_parameters{2, -0.32, 5.6, 5}};

        const auto simulation_results = quickexact(lyt, params);
        CHECK(simulation_results.charge_distributions.size() == 7);

        const auto ground_state = simulation_results.groundstates();

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
}
#endif
