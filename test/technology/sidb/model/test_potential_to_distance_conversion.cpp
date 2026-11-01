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

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
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

    SECTION("Conversions round upward at the requested precision")
    {
        /** @brief Default and custom physical parameters with each original decimal precision. */
        const auto [potential, permittivity, screening_distance, precision, expected] =
            GENERATE(table<double, double, double, uint64_t, double>(
                {{5.0, 5.6, 5.0, 1, 0.1},
                 {0.01, 2.0, 1.0, 1, 3.2},
                 {0.01, 2.0, 1.0, 2, 3.14},
                 {0.01, 2.0, 1.0, 3, 3.135},
                 {0.03, 5.6, 5.0, 0, 4.0},
                 {std::numeric_limits<double>::infinity(), 5.6, 5.0, 3, 0.001}}));
        CAPTURE(potential, permittivity, screening_distance, precision);
        params.epsilon_r = permittivity;
        params.lambda_tf = screening_distance;
        CHECK_THAT(potential_to_distance_conversion(potential, params, precision),
                   Catch::Matchers::WithinAbs(expected, 1e-5));
    }

    SECTION("High precision converges")
    {
        const auto distance = potential_to_distance_conversion(0.5, params, 15);
        REQUIRE(distance > 0.4);
        REQUIRE(distance < 0.5);
        REQUIRE_THAT(params.k() / (distance * 1e-9) * std::exp(-distance / params.lambda_tf) * ELEMENTARY_CHARGE,
                     Catch::Matchers::WithinAbs(0.5, 1e-12));
    }

    SECTION("Invalid conversion values throw")
    {
        REQUIRE(std::isinf(potential_to_distance_conversion(0, params)));
        REQUIRE_THROWS_AS(potential_to_distance_conversion(0.5, params, 20), std::invalid_argument);
        REQUIRE_THROWS_AS(potential_to_distance_conversion(0.5, params, 1000), std::invalid_argument);
        params.lambda_tf = -1;
        REQUIRE_THROWS_AS(potential_to_distance_conversion(0.5, params), std::invalid_argument);
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
        CAPTURE(value);
        auto params      = simulation_parameters{};
        params.epsilon_r = value;
        REQUIRE_THROWS_AS(potential_to_distance_conversion(0.1, params), std::invalid_argument);
        if (!std::isinf(value))
        {
            params           = simulation_parameters{};
            params.lambda_tf = value;
            REQUIRE_THROWS_AS(potential_to_distance_conversion(0.1, params), std::invalid_argument);
        }
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

    extreme_params.epsilon_r             = std::numeric_limits<double>::max();
    extreme_params.lambda_tf             = std::numeric_limits<double>::max();
    constexpr double tiny_potential      = 1e-307;
    const auto       unscreened_distance = K_E * ELEMENTARY_CHARGE * 1e9 / (extreme_params.epsilon_r * tiny_potential);
    REQUIRE_THAT(potential_to_distance_conversion(tiny_potential, extreme_params, 12),
                 Catch::Matchers::WithinAbs(unscreened_distance, 1e-12));

    extreme_params.epsilon_r = std::numeric_limits<double>::denorm_min();
    extreme_params.lambda_tf = std::numeric_limits<double>::max();
    REQUIRE(std::isinf(potential_to_distance_conversion(std::numeric_limits<double>::denorm_min(), extreme_params)));
}

TEST_CASE("Potential-to-distance conversion rounds decimal boundaries", "[potential_to_distance_conversion]")
{
    const auto params = simulation_parameters{};
    for (const auto distance : {0.01, 0.07, 0.1, 1.0, 2.0, 3.14, 10.0})
    {
        CAPTURE(distance);
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

TEST_CASE("Potential-to-distance conversion avoids intermediate underflow", "[potential_to_distance_conversion]")
{
    const simulation_parameters params{3, -0.32, 1e-290, 1.0};
    // The log form preserves a representable potential when exp(-distance / lambda_tf) is subnormal.
    constexpr double target_distance = 743.305;
    const auto       potential       = std::exp(std::log(K_E * ELEMENTARY_CHARGE * 1e9) - std::log(params.epsilon_r) -
                                                std::log(target_distance) - (target_distance / params.lambda_tf));
    REQUIRE_THAT(potential_to_distance_conversion(potential, params), Catch::Matchers::WithinAbs(743.31, 1e-10));
}

TEST_CASE("Potential-to-distance conversion supports the unscreened Coulomb limit",
          "[potential_to_distance_conversion]")
{
    const simulation_parameters params{3, -0.32, 5.6, std::numeric_limits<double>::infinity()};
    REQUIRE_THAT(potential_to_distance_conversion(0.1, params), Catch::Matchers::WithinAbs(2.58, 1e-12));
}
