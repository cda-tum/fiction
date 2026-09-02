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
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/technology/sidb/model/charge_state.hpp>
#include <fiction/technology/sidb/simulation/analysis/energy_distribution.hpp>
#include <fiction/technology/sidb/surfaces/charge_distribution_surface.hpp>
#include <fiction/types.hpp>

#include <vector>

using namespace fiction;

TEST_CASE("Test calculate_energy_distribution function", "[energy-distribution]")
{
    SECTION("one empty layout")
    {
        const sidb_100_cell_clk_lyt_siqad                                                     lyt{};
        std::vector<sidb::surfaces::charge_distribution_surface<sidb_100_cell_clk_lyt_siqad>> all_lyts{};
        const sidb::surfaces::charge_distribution_surface                                     charge_layout{lyt};
        all_lyts.push_back(charge_layout);
        auto result = sidb::simulation::analysis::calculate_energy_distribution(all_lyts);
        CHECK(result.size() == 1);
        REQUIRE(result.get_nth_state(0).has_value());
        CHECK_THAT(result.get_nth_state(0).value().electrostatic_potential_energy,
                   Catch::Matchers::WithinAbs(0.0, 0.00001));
    }

    SECTION("one layout with one SiDB placed")
    {
        sidb_100_cell_clk_lyt_siqad lyt{};
        lyt.assign_cell_type({0, 0}, sidb_100_cell_clk_lyt_siqad::cell_type::NORMAL);
        std::vector<sidb::surfaces::charge_distribution_surface<sidb_100_cell_clk_lyt_siqad>> all_lyts{};
        sidb::surfaces::charge_distribution_surface                                           charge_layout{lyt};
        charge_layout.assign_charge_state({0, 0}, sidb::model::charge_state::NEUTRAL);
        all_lyts.push_back(charge_layout);

        auto result = sidb::simulation::analysis::calculate_energy_distribution(all_lyts);
        CHECK(result.size() == 1);
        result.for_each([&](const auto& energy [[maybe_unused]], const auto& degeneracy) { CHECK(degeneracy == 1); });
    }

    SECTION("several layouts")
    {
        sidb_100_cell_clk_lyt_siqad lyt{};
        lyt.assign_cell_type({10, 10}, sidb_100_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type({11, 10}, sidb_100_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type({12, 10}, sidb_100_cell_clk_lyt_siqad::cell_type::NORMAL);

        const auto sim_param = sidb::model::simulation_parameters{};
        std::vector<sidb::surfaces::charge_distribution_surface<sidb_100_cell_clk_lyt_siqad>> all_lyts{};
        sidb::surfaces::charge_distribution_surface charge_layout_first{lyt, sim_param};

        charge_layout_first.assign_charge_state({10, 10}, sidb::model::charge_state::NEUTRAL,
                                                sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
        charge_layout_first.assign_charge_state({11, 10}, sidb::model::charge_state::NEUTRAL,
                                                sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
        charge_layout_first.assign_charge_state({12, 10}, sidb::model::charge_state::NEUTRAL,
                                                sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
        charge_layout_first.update_local_internal_potential();
        charge_layout_first.recompute_electrostatic_potential_energy();
        all_lyts.push_back(charge_layout_first);
        all_lyts.push_back(charge_layout_first);
        all_lyts.push_back(charge_layout_first);

        sidb::surfaces::charge_distribution_surface charge_layout_second{lyt};
        charge_layout_second.assign_charge_state({10, 10}, sidb::model::charge_state::NEUTRAL,
                                                 sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
        charge_layout_second.assign_charge_state({11, 10}, sidb::model::charge_state::NEGATIVE,
                                                 sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
        charge_layout_second.assign_charge_state({12, 10}, sidb::model::charge_state::NEUTRAL,
                                                 sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
        charge_layout_second.update_local_internal_potential();
        charge_layout_second.recompute_electrostatic_potential_energy();
        all_lyts.push_back(charge_layout_second);
        all_lyts.push_back(charge_layout_second);
        all_lyts.push_back(charge_layout_second);
        all_lyts.push_back(charge_layout_second);

        sidb::surfaces::charge_distribution_surface charge_layout_third{lyt};
        charge_layout_third.assign_charge_state({10, 10}, sidb::model::charge_state::NEGATIVE,
                                                sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
        charge_layout_third.assign_charge_state({11, 10}, sidb::model::charge_state::NEGATIVE,
                                                sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
        charge_layout_third.assign_charge_state({12, 10}, sidb::model::charge_state::NEGATIVE,
                                                sidb::surfaces::charge_index_mode::KEEP_CHARGE_INDEX);
        charge_layout_third.update_local_internal_potential();
        charge_layout_third.recompute_electrostatic_potential_energy();
        all_lyts.push_back(charge_layout_third);
        all_lyts.push_back(charge_layout_third);
        all_lyts.push_back(charge_layout_third);

        const auto result = sidb::simulation::analysis::calculate_energy_distribution(all_lyts);

        // "all_lyts" collects all three layouts (charge_layout_first, charge_layout_second, charge_layout_third). The
        // last two have an identical potential energy (it.second == 2) which is smaller than the one from the first
        // layout.
        REQUIRE(result.get_nth_state(0).has_value());
        CHECK(result.get_nth_state(0).value().degeneracy == 2);

        REQUIRE(result.get_nth_state(1).has_value());
        CHECK(result.get_nth_state(1).value().degeneracy == 1);
    }
}
