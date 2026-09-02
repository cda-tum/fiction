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
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/analysis/band_bending_resilience.hpp>
#include <fiction/technology/sidb/simulation/analysis/physical_population_stability.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/math/math_utils.hpp>

#include <vector>

using namespace fiction;

using test_layout = sidb_cell_clk_lyt_siqad;

TEST_CASE("Single SiDB", "[band-bending-resilience]")
{
    const auto lyt = blueprints::bestagon_and_gate<test_layout>();

    constexpr auto params = sidb::simulation::analysis::band_bending_resilience_params{
        sidb::simulation::analysis::physical_population_stability_params{sidb::model::simulation_parameters{2, -0.32},
                                                                         2}};

    SECTION("Minimal potential required to conduct a charge change from neutral to negative")
    {
        const auto min_potential = sidb::simulation::analysis::band_bending_resilience(
            lyt, std::vector{synthesis::create_and_tt()}, params,
            sidb::simulation::analysis::transition_type::NEUTRAL_TO_NEGATIVE);

        CHECK_THAT(min_potential, Catch::Matchers::WithinAbs(0.020652, utils::math::ERROR_MARGIN));
    }

    SECTION("Minimal potential required to conduct a charge change from negative to neutral")
    {
        const auto min_potential = sidb::simulation::analysis::band_bending_resilience(
            lyt, std::vector{synthesis::create_and_tt()}, params,
            sidb::simulation::analysis::transition_type::NEGATIVE_TO_NEUTRAL);

        CHECK_THAT(min_potential, Catch::Matchers::WithinAbs(0.087417, utils::math::ERROR_MARGIN));
    }

    SECTION("Minimal potential required to conduct a charge change from positive to neutral")
    {
        const auto min_potential = sidb::simulation::analysis::band_bending_resilience(
            lyt, std::vector{synthesis::create_and_tt()}, params,
            sidb::simulation::analysis::transition_type::NEUTRAL_TO_POSITIVE);

        CHECK_THAT(min_potential, Catch::Matchers::WithinAbs(0.413859, utils::math::ERROR_MARGIN));
    }

    SECTION("Minimal potential required to conduct a charge change")
    {
        const auto min_potential =
            sidb::simulation::analysis::band_bending_resilience(lyt, std::vector{synthesis::create_and_tt()}, params);

        // the minimal potential for any charge change is the same as for neutral to negative
        CHECK_THAT(min_potential, Catch::Matchers::WithinAbs(0.020652, utils::math::ERROR_MARGIN));
    }
}
