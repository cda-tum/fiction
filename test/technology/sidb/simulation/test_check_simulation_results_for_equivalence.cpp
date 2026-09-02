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
 * @author Willem Lambooy (wlambooy)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/technology/sidb/model/charge_state.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/check_simulation_results_for_equivalence.hpp>
#include <fiction/technology/sidb/simulation/result.hpp>
#include <fiction/technology/sidb/surfaces/charge_distribution_surface.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEST_CASE("Several tests", "[equivalence-check-for-simulation-results]")
{
    sidb_100_cell_clk_lyt lyt1{};
    lyt1.assign_cell_type({0, 0}, sidb_100_cell_clk_lyt::cell_type::NORMAL);
    lyt1.assign_cell_type({0, 1}, sidb_100_cell_clk_lyt::cell_type::NORMAL);
    lyt1.assign_cell_type({0, 2}, sidb_100_cell_clk_lyt::cell_type::NORMAL);
    lyt1.assign_cell_type({0, 3}, sidb_100_cell_clk_lyt::cell_type::NORMAL);

    cds_sidb_100_cell_clk_lyt       cds1{lyt1};
    const cds_sidb_100_cell_clk_lyt cds2{lyt1, sidb::model::simulation_parameters{},
                                         sidb::model::charge_state::NEUTRAL};

    sidb::simulation::result<sidb_100_cell_clk_lyt> results1{};
    sidb::simulation::result<sidb_100_cell_clk_lyt> results2{};

    results1.charge_distributions = {cds1, cds2};
    results2.charge_distributions = {cds2, cds1};

    SECTION("equivalence")
    {
        CHECK(sidb::simulation::check_simulation_results_for_equivalence(results1, results2));
    }

    SECTION("non-equivalence with respect to cds with different charge states")
    {
        cds1.assign_charge_state({0, 0}, sidb::model::charge_state::POSITIVE);
        results1.charge_distributions = {cds1, cds2};
        CHECK(!sidb::simulation::check_simulation_results_for_equivalence(results1, results2));
    }

    SECTION("non-equivalence with respect to cds with same charge state but different energy")
    {
        cds1.assign_electrostatic_potential_energy_to_zero();
        results1.charge_distributions = {cds1, cds2};
        CHECK(!sidb::simulation::check_simulation_results_for_equivalence(results1, results2));
    }

    SECTION("non-equivalence due to different number of solutions")
    {
        results1.charge_distributions = {cds1};
        CHECK(!sidb::simulation::check_simulation_results_for_equivalence(results1, results2));
    }

    SECTION("non-equivalence due to different number of SiDBs")
    {
        auto lyt2{lyt1.clone()};
        lyt2.assign_cell_type({4, 2}, sidb_100_cell_clk_lyt::cell_type::NORMAL);
        lyt2.assign_cell_type({4, 3}, sidb_100_cell_clk_lyt::cell_type::NORMAL);
        results2.charge_distributions = {
            sidb::surfaces::charge_distribution_surface{lyt2, sidb::model::simulation_parameters{},
                                                        sidb::model::charge_state::NEUTRAL},
            sidb::surfaces::charge_distribution_surface{lyt2}};
        CHECK(!sidb::simulation::check_simulation_results_for_equivalence(results1, results2));
    }

    SECTION("non-equivalence due to duplication in first solution")
    {
        results1.charge_distributions = {sidb::surfaces::charge_distribution_surface{lyt1},
                                         sidb::surfaces::charge_distribution_surface{lyt1}};
        CHECK(!sidb::simulation::check_simulation_results_for_equivalence(results1, results2));
    }
    SECTION("non-equivalence due to duplication in second solution")
    {
        results2.charge_distributions = {sidb::surfaces::charge_distribution_surface{lyt1},
                                         sidb::surfaces::charge_distribution_surface{lyt1}};
        CHECK(!sidb::simulation::check_simulation_results_for_equivalence(results1, results2));
    }
    SECTION("non-equivalence due to different charge states of the first and the second solution")
    {
        results1.charge_distributions = {sidb::surfaces::charge_distribution_surface{lyt1}};
        results2.charge_distributions = {sidb::surfaces::charge_distribution_surface{lyt1}};
        results2.charge_distributions.at(0).assign_charge_state({0, 0}, sidb::model::charge_state::POSITIVE);
        results2.charge_distributions.at(0).assign_electrostatic_potential_energy_to_zero();
        results1.charge_distributions.at(0).assign_electrostatic_potential_energy_to_zero();
        CHECK(!sidb::simulation::check_simulation_results_for_equivalence(results1, results2));
    }
}
