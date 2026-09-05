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
 * @brief Tests for `fiction/technology/sidb/simulation/check_simulation_results_for_equivalence.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Willem Lambooy (wlambooy)
 * @author Marcel Walter (marcelwa)
 * @author OpenAI (Codex)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/charge_state.hpp>
#include <fiction/technology/sidb/simulation/check_simulation_results_for_equivalence.hpp>
#include <fiction/technology/sidb/simulation/potential_landscape.hpp>
#include <fiction/technology/sidb/simulation/result.hpp>
#include <fiction/technology/sidb/technology.hpp>

#include <cstddef>
#include <cstdint>
#include <string_view>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation;

TEST_CASE("Several tests", "[equivalence-check-for-simulation-results]")
{
    layout lyt1{};
    lyt1.assign_cell_type(site_at_row(0, 0), sidb_technology::cell_type::NORMAL);
    lyt1.assign_cell_type(site_at_row(0, 1), sidb_technology::cell_type::NORMAL);
    lyt1.assign_cell_type(site_at_row(0, 2), sidb_technology::cell_type::NORMAL);
    lyt1.assign_cell_type(site_at_row(0, 3), sidb_technology::cell_type::NORMAL);

    const potential_landscape land{lyt1};

    auto       cd1 = land.evaluate(charge_distribution{lyt1, charge_state::NEGATIVE});
    const auto cd2 = land.evaluate(charge_distribution{lyt1, charge_state::NEUTRAL});

    result results1{};
    result results2{};
    results1.lyt = lyt1;
    results2.lyt = lyt1;

    results1.charge_distributions = {cd1, cd2};
    results2.charge_distributions = {cd2, cd1};

    SECTION("equivalence")
    {
        CHECK(check_simulation_results_for_equivalence(results1, results2));
    }

    SECTION("non-equivalence with respect to distributions with different charge states")
    {
        cd1.assign_charge_state(site_at_row(0, 0), charge_state::POSITIVE);
        results1.charge_distributions = {cd1, cd2};
        CHECK(!check_simulation_results_for_equivalence(results1, results2));
    }

    SECTION("non-equivalence with respect to distributions with same charge state but different energy")
    {
        cd1.assign_energy(0.0);
        results1.charge_distributions = {cd1, cd2};
        CHECK(!check_simulation_results_for_equivalence(results1, results2));
    }

    SECTION("non-equivalence due to different number of solutions")
    {
        results1.charge_distributions = {cd1};
        CHECK(!check_simulation_results_for_equivalence(results1, results2));
    }

    SECTION("non-equivalence due to different number of SiDBs")
    {
        auto lyt2 = lyt1;
        lyt2.assign_cell_type(site_at_row(4, 2), sidb_technology::cell_type::NORMAL);
        lyt2.assign_cell_type(site_at_row(4, 3), sidb_technology::cell_type::NORMAL);
        results2.lyt                  = lyt2;
        results2.charge_distributions = {charge_distribution{lyt2, charge_state::NEUTRAL},
                                         charge_distribution{lyt2, charge_state::NEGATIVE}};
        CHECK(!check_simulation_results_for_equivalence(results1, results2));
    }

    SECTION("non-equivalence due to duplication in first solution")
    {
        results1.charge_distributions = {charge_distribution{lyt1}, charge_distribution{lyt1}};
        CHECK(!check_simulation_results_for_equivalence(results1, results2));
    }
    SECTION("non-equivalence due to duplication in second solution")
    {
        results2.charge_distributions = {charge_distribution{lyt1}, charge_distribution{lyt1}};
        CHECK(!check_simulation_results_for_equivalence(results1, results2));
    }
    SECTION("non-equivalence due to different charge states of the first and the second solution")
    {
        results1.charge_distributions = {charge_distribution{lyt1}};
        results2.charge_distributions = {charge_distribution{lyt1}};
        results2.charge_distributions.at(0).assign_charge_state(site_at_row(0, 0), charge_state::POSITIVE);
        CHECK(!check_simulation_results_for_equivalence(results1, results2));
    }
}

TEST_CASE("Simulation equivalence beyond the charge-index range", "[equivalence-check-for-simulation-results]")
{
    // Base-3 digits of 2^64 collide with zero in a 64-bit charge index.
    constexpr std::string_view digits{"11112220022122120101211020120210210211221"};
    layout                     lyt{};
    for (std::size_t i = 0; i < digits.size(); ++i)
    {
        lyt.assign_cell_type({i, 0}, sidb_technology::cell_type::NORMAL);
    }
    const charge_distribution zero{lyt};
    auto                      wrapped = zero;
    for (std::size_t i = 0; i < digits.size(); ++i)
    {
        wrapped.assign_charge_state_by_index(i, sign_to_charge_state(static_cast<int8_t>(digits[i] - '1')));
    }
    REQUIRE(zero.charge_index(3) == wrapped.charge_index(3));
    result res{};
    res.lyt                  = lyt;
    res.charge_distributions = {zero, wrapped};
    CHECK(check_simulation_results_for_equivalence(res, res));
    auto reordered                 = res;
    reordered.charge_distributions = {wrapped, zero};
    CHECK(check_simulation_results_for_equivalence(res, reordered));
    reordered.charge_distributions = {zero, zero};
    CHECK(!check_simulation_results_for_equivalence(res, reordered));
}
