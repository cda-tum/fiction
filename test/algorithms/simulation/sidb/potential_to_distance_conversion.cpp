//
// Created by Jan Drewniok on 10.11.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/simulation/sidb/potential_to_distance_conversion.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>

#include <cstdint>
#include <limits>

using namespace fiction;

TEST_CASE("Conversion of potential to distance", "[potential_to_distance_conversion]")
{
    auto params = sidb_simulation_parameters{};

    SECTION("Valid conversion with default parameters")
    {
        uint64_t const precision         = 1;
        const double   potential_value   = 5.0;
        const double   expected_distance = 0.1;
        REQUIRE_THAT(convert_potential_to_distance(potential_value, params, precision),
                     Catch::Matchers::WithinAbs(expected_distance, 1e-5));
    }

    SECTION("Valid conversion with custom parameters, precision is 1")
    {
        params.epsilon_r                 = 2.0;
        params.lambda_tf                 = 1.0;
        const uint64_t precision         = 1;
        const double   potential_value   = 0.01;
        const double   expected_distance = 3.2;
        REQUIRE_THAT(convert_potential_to_distance(potential_value, params, precision),
                     Catch::Matchers::WithinAbs(expected_distance, 1e-5));
    }

    SECTION("Valid conversion with custom parameters, precision is 2")
    {
        params.epsilon_r                 = 2.0;
        params.lambda_tf                 = 1.0;
        const uint64_t precision         = 2;
        const double   potential_value   = 0.01;
        const double   expected_distance = 3.14;
        REQUIRE_THAT(convert_potential_to_distance(potential_value, params, precision),
                     Catch::Matchers::WithinAbs(expected_distance, 1e-5));
    }

    SECTION("Valid conversion with custom parameters, precision is 3")
    {
        params.epsilon_r                 = 2.0;
        params.lambda_tf                 = 1.0;
        const uint64_t precision         = 3;
        const double   potential_value   = 0.01;
        const double   expected_distance = 3.135;
        REQUIRE_THAT(convert_potential_to_distance(potential_value, params, precision),
                     Catch::Matchers::WithinAbs(expected_distance, 1e-5));
    }

    SECTION("Valid conversion with custom parameters, precision is 0")
    {
        const uint64_t precision         = 0;
        const double   potential_value   = 0.03;
        const double   expected_distance = 4;
        REQUIRE_THAT(convert_potential_to_distance(potential_value, params, precision),
                     Catch::Matchers::WithinAbs(expected_distance, 1e-5));
    }

    SECTION("Conversion with infinite potential")
    {
        const uint64_t precision         = 3;
        const double   potential_value   = std::numeric_limits<double>::infinity();
        const double   expected_distance = 0.001;
        REQUIRE_THAT(convert_potential_to_distance(potential_value, params, precision),
                     Catch::Matchers::WithinAbs(expected_distance, 1e-5));
    }
}
