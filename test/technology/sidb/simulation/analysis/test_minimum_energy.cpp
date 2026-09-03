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
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/technology/sidb/model/charge_state.hpp>
#include <fiction/technology/sidb/simulation/analysis/minimum_energy.hpp>
#include <fiction/technology/sidb/surfaces/charge_distribution_surface.hpp>
#include <fiction/types.hpp>

#include <cmath>
#include <vector>

using namespace fiction;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation::analysis;
using namespace fiction::sidb::surfaces;

using layout = sidb_100_cell_clk_lyt_siqad;

TEST_CASE("Test minimum energy function", "[minimum-energy]")
{
    layout lyt{};

    SECTION("layout with no SiDB placed")
    {
        const charge_distribution_surface                charge_layout{lyt};
        std::vector<charge_distribution_surface<layout>> all_lyts{};

        CHECK(std::isinf(minimum_energy(all_lyts.begin(), all_lyts.end())));

        all_lyts.push_back(charge_layout);

        CHECK(std::abs(minimum_energy(all_lyts.begin(), all_lyts.end()) - 0) < 0.00000001);
    }

    SECTION("layout with one SiDB placed")
    {
        lyt.assign_cell_type({0, 0}, layout::cell_type::NORMAL);

        const charge_distribution_surface                charge_layout{lyt};
        std::vector<charge_distribution_surface<layout>> all_lyts{};

        CHECK(std::isinf(minimum_energy(all_lyts.cbegin(), all_lyts.cend())));

        all_lyts.push_back(charge_layout);

        CHECK(std::abs(minimum_energy(all_lyts.cbegin(), all_lyts.cend()) - 0) < 0.00000001);
    }

    SECTION("layout with three SiDBs placed")
    {
        lyt.assign_cell_type({0, 0}, layout::cell_type::NORMAL);
        lyt.assign_cell_type({10, 10}, layout::cell_type::NORMAL);
        lyt.assign_cell_type({9, 9}, layout::cell_type::NORMAL);

        charge_distribution_surface                      charge_layout_first{lyt};
        std::vector<charge_distribution_surface<layout>> all_lyts{};

        CHECK(std::isinf(minimum_energy(all_lyts.cbegin(), all_lyts.cend())));

        charge_layout_first.assign_charge_state({0, 0}, charge_state::NEUTRAL);

        charge_layout_first.update_local_internal_potential();
        charge_layout_first.recompute_electrostatic_potential_energy();
        all_lyts.push_back(charge_layout_first);

        charge_distribution_surface charge_layout_second{lyt};

        charge_layout_second.assign_charge_state({10, 10}, charge_state::NEUTRAL);
        charge_layout_second.assign_charge_state({9, 9}, charge_state::NEUTRAL);

        charge_layout_second.update_local_internal_potential();
        charge_layout_second.recompute_electrostatic_potential_energy();
        all_lyts.push_back(charge_layout_second);

        CHECK_THAT(minimum_energy(all_lyts.cbegin(), all_lyts.cend()), Catch::Matchers::WithinAbs(0.0, 0.00001));
    }
}
