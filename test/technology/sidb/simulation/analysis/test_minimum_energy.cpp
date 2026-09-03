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
 * @brief Tests for `fiction/technology/sidb/simulation/analysis/minimum_energy.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 * @author Willem Lambooy (wlambooy)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/charge_state.hpp>
#include <fiction/technology/sidb/simulation/analysis/minimum_energy.hpp>
#include <fiction/technology/sidb/simulation/potential_landscape.hpp>
#include <fiction/technology/sidb/technology.hpp>

#include <cmath>
#include <vector>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation;
using namespace fiction::sidb::simulation::analysis;

TEST_CASE("Test minimum energy function", "[minimum-energy]")
{
    layout lyt{};

    SECTION("layout with no SiDB placed")
    {
        const potential_landscape        land{lyt};
        std::vector<charge_distribution> all_cds{};

        CHECK(std::isinf(minimum_energy(all_cds.begin(), all_cds.end())));

        all_cds.push_back(land.evaluate(charge_distribution{lyt}));

        CHECK(std::abs(minimum_energy(all_cds.begin(), all_cds.end()) - 0) < 0.00000001);
    }

    SECTION("layout with one SiDB placed")
    {
        lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);

        const potential_landscape        land{lyt};
        std::vector<charge_distribution> all_cds{};

        CHECK(std::isinf(minimum_energy(all_cds.cbegin(), all_cds.cend())));

        all_cds.push_back(land.evaluate(charge_distribution{lyt}));

        CHECK(std::abs(minimum_energy(all_cds.cbegin(), all_cds.cend()) - 0) < 0.00000001);
    }

    SECTION("layout with three SiDBs placed")
    {
        lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({10, 5, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({9, 4, 1}, sidb_technology::cell_type::NORMAL);

        const potential_landscape        land{lyt};
        std::vector<charge_distribution> all_cds{};

        CHECK(std::isinf(minimum_energy(all_cds.cbegin(), all_cds.cend())));

        charge_distribution first{lyt};
        first.assign_charge_state({0, 0, 0}, charge_state::NEUTRAL);
        all_cds.push_back(land.evaluate(first));

        charge_distribution second{lyt};
        second.assign_charge_state({10, 5, 0}, charge_state::NEUTRAL);
        second.assign_charge_state({9, 4, 1}, charge_state::NEUTRAL);
        all_cds.push_back(land.evaluate(second));

        CHECK_THAT(minimum_energy(all_cds.cbegin(), all_cds.cend()), Catch::Matchers::WithinAbs(0.0, 0.00001));
        CHECK(minimum_energy_distribution(all_cds.cbegin(), all_cds.cend())->same_charge_states(second));
    }
}
