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
 * @brief Tests for `fiction/technology/sidb/model/potential_to_distance_conversion.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/technology/sidb/model/physical_constants.hpp>
#include <fiction/technology/sidb/model/potential_to_distance_conversion.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>

#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>

using namespace fiction;
using namespace fiction::sidb::model;

TEST_CASE("Conversion of potential to distance", "[potential_to_distance_conversion]")
{
    auto params = simulation_parameters{};

    SECTION("Valid conversion with default parameters")
    {
        constexpr uint64_t precision         = 1;
        constexpr double   potential_value   = 5.0;
        constexpr double   expected_distance = 0.1;
        REQUIRE_THAT(potential_to_distance_conversion(potential_value, params, precision),
                     Catch::Matchers::WithinAbs(expected_distance, 1e-5));
    }

    SECTION("Valid conversion with custom parameters, precision is 1")
    {
        params.epsilon_r                     = 2.0;
        params.lambda_tf                     = 1.0;
        constexpr uint64_t precision         = 1;
        constexpr double   potential_value   = 0.01;
        constexpr double   expected_distance = 3.2;
        REQUIRE_THAT(potential_to_distance_conversion(potential_value, params, precision),
                     Catch::Matchers::WithinAbs(expected_distance, 1e-5));
    }

    SECTION("Valid conversion with custom parameters, precision is 2")
    {
        params.epsilon_r                     = 2.0;
        params.lambda_tf                     = 1.0;
        constexpr uint64_t precision         = 2;
        constexpr double   potential_value   = 0.01;
        constexpr double   expected_distance = 3.14;
        REQUIRE_THAT(potential_to_distance_conversion(potential_value, params, precision),
                     Catch::Matchers::WithinAbs(expected_distance, 1e-5));
    }

    SECTION("Valid conversion with custom parameters, precision is 3")
    {
        params.epsilon_r                     = 2.0;
        params.lambda_tf                     = 1.0;
        constexpr uint64_t precision         = 3;
        constexpr double   potential_value   = 0.01;
        constexpr double   expected_distance = 3.135;
        REQUIRE_THAT(potential_to_distance_conversion(potential_value, params, precision),
                     Catch::Matchers::WithinAbs(expected_distance, 1e-5));
    }

    SECTION("Valid conversion with custom parameters, precision is 0")
    {
        constexpr uint64_t precision         = 0;
        constexpr double   potential_value   = 0.03;
        constexpr double   expected_distance = 4;
        REQUIRE_THAT(potential_to_distance_conversion(potential_value, params, precision),
                     Catch::Matchers::WithinAbs(expected_distance, 1e-5));
    }

    SECTION("Conversion with infinite potential")
    {
        constexpr uint64_t precision         = 3;
        constexpr double   potential_value   = std::numeric_limits<double>::infinity();
        constexpr double   expected_distance = 0.001;
        REQUIRE_THAT(potential_to_distance_conversion(potential_value, params, precision),
                     Catch::Matchers::WithinAbs(expected_distance, 1e-5));
    }
}

TEST_CASE("Potential-to-distance conversion validates inputs", "[potential_to_distance_conversion]")
{
    const auto infinity = std::numeric_limits<double>::infinity();
    REQUIRE_THROWS_AS(potential_to_distance_conversion(-1.0), std::invalid_argument);
    REQUIRE_THROWS_AS(potential_to_distance_conversion(-infinity), std::invalid_argument);
    REQUIRE_THROWS_AS(potential_to_distance_conversion(std::numeric_limits<double>::quiet_NaN()),
                      std::invalid_argument);
    REQUIRE_THROWS_AS(
        potential_to_distance_conversion(0.1, simulation_parameters{}, std::numeric_limits<double>::digits10 + 1),
        std::invalid_argument);
    for (const auto value : {0.0, -1.0, infinity, std::numeric_limits<double>::quiet_NaN()})
    {
        auto params      = simulation_parameters{};
        params.epsilon_r = value;
        REQUIRE_THROWS_AS(potential_to_distance_conversion(0.1, params), std::invalid_argument);
        params           = simulation_parameters{};
        params.lambda_tf = value;
        REQUIRE_THROWS_AS(potential_to_distance_conversion(0.1, params), std::invalid_argument);
    }
}

TEST_CASE("Potential-to-distance conversion bounds precision and extremes", "[potential_to_distance_conversion]")
{
    const auto params = simulation_parameters{};
    REQUIRE(std::isinf(potential_to_distance_conversion(0.0, params)));

    const auto distance = potential_to_distance_conversion(0.03, params, std::numeric_limits<double>::digits10);
    const auto recovered_potential =
        params.k() * ELEMENTARY_CHARGE / (distance * 1e-9) * std::exp(-distance / params.lambda_tf);
    REQUIRE_THAT(recovered_potential, Catch::Matchers::WithinRel(0.03, 1e-13));

    REQUIRE(std::isfinite(potential_to_distance_conversion(std::numeric_limits<double>::denorm_min(), params)));
    REQUIRE_THAT(potential_to_distance_conversion(std::numeric_limits<double>::max(), params),
                 Catch::Matchers::WithinAbs(0.01, 1e-15));

    auto extreme_params      = simulation_parameters{};
    extreme_params.epsilon_r = 1e300;
    extreme_params.lambda_tf = 1e300;
    REQUIRE_THAT(potential_to_distance_conversion(1e-300, extreme_params), Catch::Matchers::WithinAbs(1.44, 1e-14));

    extreme_params.epsilon_r = std::numeric_limits<double>::denorm_min();
    extreme_params.lambda_tf = std::numeric_limits<double>::max();
    REQUIRE(std::isinf(potential_to_distance_conversion(std::numeric_limits<double>::denorm_min(), extreme_params)));
}

TEST_CASE("Potential-to-distance conversion rounds decimal boundaries", "[potential_to_distance_conversion]")
{
    const auto params = simulation_parameters{};
    for (const auto distance : {0.01, 0.07, 0.1, 1.0, 2.0, 3.14, 10.0})
    {
        const auto potential =
            params.k() * ELEMENTARY_CHARGE / (distance * 1e-9) * std::exp(-distance / params.lambda_tf);
        REQUIRE_THAT(potential_to_distance_conversion(potential, params), Catch::Matchers::WithinAbs(distance, 1e-14));
        REQUIRE_THAT(potential_to_distance_conversion(potential / 1.001, params),
                     Catch::Matchers::WithinAbs(distance + 0.01, 1e-14));
    }
}

TEST_CASE("Potential-to-distance conversion rounds high precision upward", "[potential_to_distance_conversion]")
{
    const auto       params    = simulation_parameters{};
    constexpr double potential = 1.3772094688939445e-08;
    const auto       distance  = potential_to_distance_conversion(potential, params, 13);
    const auto       recovered_potential =
        params.k() * ELEMENTARY_CHARGE / (distance * 1e-9) * std::exp(-distance / params.lambda_tf);
    REQUIRE(recovered_potential <= potential);
}
