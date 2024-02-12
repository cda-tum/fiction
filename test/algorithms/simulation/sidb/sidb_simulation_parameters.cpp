//
// Created by Jan Drewniok on 03.08.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/technology/physical_constants.hpp>
#include <fiction/utils/math_utils.hpp>

using namespace fiction;

TEST_CASE("Test sidb_simulation_parameters", "[sidb-simulation-parameters]")
{
    SECTION("Test constructor, parameter values and overwrite values, Si-100")
    {
        const sidb_simulation_parameters params{};

        CHECK_THAT(params.lat_a - 3.84, Catch::Matchers::WithinAbs(0.0, 0.00001));
        CHECK_THAT(params.lat_b - 7.68, Catch::Matchers::WithinAbs(0.0, 0.00001));
        CHECK_THAT(params.lat_c.first, Catch::Matchers::WithinAbs(0.0, 0.00001));
        CHECK_THAT(params.lat_c.second - 2.25, Catch::Matchers::WithinAbs(0.0, 0.00001));

        // Default values
        REQUIRE(params.base == 3);
        REQUIRE(params.mu_minus == -0.32);
        REQUIRE(params.epsilon_r == 5.6);
        REQUIRE(params.lambda_tf == 5.0);
        REQUIRE(params.base == 3);

        // Constructor
        sidb_simulation_parameters custom_params(2, -0.25, 4.2, 6.0);
        REQUIRE(custom_params.base == 2);
        REQUIRE(custom_params.mu_minus == -0.25);
        REQUIRE(custom_params.epsilon_r == 4.2);
        REQUIRE(custom_params.lambda_tf == 6.0);
        REQUIRE(custom_params.base == 2);

        // Change values
        custom_params.base      = 3;
        custom_params.mu_minus  = -0.1;
        custom_params.epsilon_r = 10.2;
        custom_params.lambda_tf = 1.0;

        REQUIRE(custom_params.base == 3);
        REQUIRE(custom_params.mu_minus == -0.1);
        REQUIRE(custom_params.epsilon_r == 10.2);
        REQUIRE(custom_params.lambda_tf == 1.0);
    }

    SECTION("Test constructor, parameter values and overwrite values, Si-111")
    {
        // Constructor
        const sidb_simulation_parameters params(2, -0.25, 4.2, 6.0, lattice_orientation::SI_111);
        REQUIRE(params.base == 2);
        REQUIRE(params.mu_minus == -0.25);
        REQUIRE(params.epsilon_r == 4.2);
        REQUIRE(params.lambda_tf == 6.0);
        REQUIRE(params.base == 2);

        CHECK_THAT(params.lat_a - 6.65, Catch::Matchers::WithinAbs(0.0, 0.00001));
        CHECK_THAT(params.lat_b - 3.84, Catch::Matchers::WithinAbs(0.0, 0.00001));
        CHECK_THAT(params.lat_c.first - 3.3255, Catch::Matchers::WithinAbs(0.0, 0.00001));
        CHECK_THAT(params.lat_c.second - 1.92, Catch::Matchers::WithinAbs(0.0, 0.00001));
    }

    SECTION("Test calculated values")
    {
        const sidb_simulation_parameters params{};

        const double epsilon_r  = 5.6;
        const double k_expected = physical_constants::K_E / epsilon_r;
        CHECK_THAT(params.k() - k_expected, Catch::Matchers::WithinAbs(0.0, 0.00001));

        const double mu_minus         = -0.32;
        const double mu_plus_expected = mu_minus - 0.59;
        CHECK_THAT(params.mu_plus() - mu_plus_expected, Catch::Matchers::WithinAbs(0.0, 0.00001));
    }
}
