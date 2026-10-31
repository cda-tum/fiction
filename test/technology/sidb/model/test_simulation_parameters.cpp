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
 * @brief Tests for `fiction/technology/sidb/model/simulation_parameters.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/technology/sidb/model/physical_constants.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>

using namespace fiction;
using namespace fiction::sidb::model;

TEST_CASE("Simulation parameter construction and derived values", "[sidb-simulation-parameters]")
{
    SECTION("Test constructor, parameter values and overwrite values")
    {
        const simulation_parameters params{};

        // Default values
        CHECK(params.base == 3);
        CHECK(params.mu_minus == -0.32);
        CHECK(params.epsilon_r == 5.6);
        CHECK(params.lambda_tf == 5.0);

        // Constructor
        simulation_parameters custom_params{2, -0.25, 4.2, 6.0};
        CHECK(custom_params.base == 2);
        CHECK(custom_params.mu_minus == -0.25);
        CHECK(custom_params.epsilon_r == 4.2);
        CHECK(custom_params.lambda_tf == 6.0);

        CHECK_THAT(custom_params.k(), Catch::Matchers::WithinAbs(K_E / 4.2, 0.00001));
        CHECK_THAT(custom_params.mu_plus(), Catch::Matchers::WithinAbs(-0.84, 0.00001));

        // Change values
        custom_params.base      = 3;
        custom_params.mu_minus  = -0.1;
        custom_params.epsilon_r = 10.2;
        custom_params.lambda_tf = 1.0;

        CHECK(custom_params.base == 3);
        CHECK(custom_params.mu_minus == -0.1);
        CHECK(custom_params.epsilon_r == 10.2);
        CHECK(custom_params.lambda_tf == 1.0);
        CHECK_THAT(custom_params.k(), Catch::Matchers::WithinAbs(K_E / 10.2, 0.00001));
        CHECK_THAT(custom_params.mu_plus(), Catch::Matchers::WithinAbs(-0.69, 0.00001));
    }

    SECTION("Test calculated values")
    {
        const simulation_parameters params{};

        const double epsilon_r  = 5.6;
        const double k_expected = K_E / epsilon_r;
        CHECK_THAT(params.k(), Catch::Matchers::WithinAbs(k_expected, 0.00001));

        const double mu_minus         = -0.32;
        const double mu_plus_expected = mu_minus - 0.59;
        CHECK_THAT(params.mu_plus(), Catch::Matchers::WithinAbs(mu_plus_expected, 0.00001));
    }
}
