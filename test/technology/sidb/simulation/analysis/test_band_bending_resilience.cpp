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
 * @brief Tests for `fiction/technology/sidb/simulation/analysis/band_bending_resilience.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/synthesis/truth_tables.hpp>
#include <fiction/technology/sidb/cell_level_layout_conversion.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/analysis/band_bending_resilience.hpp>
#include <fiction/technology/sidb/simulation/analysis/physical_population_stability.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/math/math_utils.hpp>

#include <vector>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation::analysis;
using namespace fiction::synthesis;
using namespace fiction::utils::math;

using test_layout = sidb_cell_clk_lyt_siqad;

TEST_CASE("Single SiDB", "[band-bending-resilience]")
{
    const auto lyt = to_sidb_layout(blueprints::bestagon_and_gate<test_layout>());

    constexpr auto params =
        band_bending_resilience_params{.assess_population_stability_params = physical_population_stability_params{
                                           .sim_params = simulation_parameters{2, -0.32},
                                           .precision_for_distance_corresponding_to_potential = 2}};

    SECTION("Minimal potential required to conduct a charge change from neutral to negative")
    {
        const auto min_potential =
            band_bending_resilience(lyt, std::vector{create_and_tt()}, params, transition_type::NEUTRAL_TO_NEGATIVE);

        CHECK_THAT(min_potential, Catch::Matchers::WithinAbs(0.019990, ERROR_MARGIN));
    }

    SECTION("Minimal potential required to conduct a charge change from negative to neutral")
    {
        const auto min_potential =
            band_bending_resilience(lyt, std::vector{create_and_tt()}, params, transition_type::NEGATIVE_TO_NEUTRAL);

        CHECK_THAT(min_potential, Catch::Matchers::WithinAbs(0.064148, ERROR_MARGIN));
    }

    SECTION("Minimal potential required to conduct a charge change from neutral to positive")
    {
        const auto min_potential =
            band_bending_resilience(lyt, std::vector{create_and_tt()}, params, transition_type::NEUTRAL_TO_POSITIVE);

        CHECK_THAT(min_potential, Catch::Matchers::WithinAbs(0.386046, ERROR_MARGIN));
    }

    SECTION("Minimal potential required to conduct a charge change")
    {
        const auto min_potential = band_bending_resilience(lyt, std::vector{create_and_tt()}, params);

        // the minimal potential for any charge change is the same as for neutral to negative
        CHECK_THAT(min_potential, Catch::Matchers::WithinAbs(0.019990, ERROR_MARGIN));
    }
}
