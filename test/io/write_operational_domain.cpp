//
// Created by marcel on 24.06.22.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>
#include <fiction/io/write_operational_domain.hpp>

#include <sstream>
#include <string>

using namespace fiction;

TEST_CASE("Write empty operational domain", "[write-operational-domain]")
{
    operational_domain opdom{};

    std::ostringstream os{};

    SECTION("default sweep dimensions")
    {
        static constexpr const char* expected = "epsilon_r, lambda_tf, operational status\n";

        write_operational_domain(opdom, os);

        CHECK(os.str() == expected);
    }
    SECTION("custom sweep dimensions")
    {
        opdom.x_dimension = operational_domain::sweep_parameter::LAMBDA_TF;
        opdom.y_dimension = operational_domain::sweep_parameter::MU_MINUS;

        static constexpr const char* expected = "lambda_tf, mu_minus, operational status\n";

        write_operational_domain(opdom, os);

        CHECK(os.str() == expected);
    }
}

TEST_CASE("Write simple operational domain", "[write-operational-domain]")
{
    operational_domain opdom{};
    opdom.operational_values = {{{0, 0}, operational_domain::operational_status::OPERATIONAL},
                                {{0, 1}, operational_domain::operational_status::NON_OPERATIONAL}};

    std::ostringstream os{};

    SECTION("default operational tags")
    {
        static constexpr const char* expected =
            "epsilon_r, lambda_tf, operational status\n0, 0, operational\n0, 1, non-operational\n";

        write_operational_domain(opdom, os);

        CHECK(os.str() == expected);
    }
    SECTION("custom operational tags")
    {
        const write_operational_domain_params params = {"True", "False"};

        static constexpr const char* expected = "epsilon_r, lambda_tf, operational status\n0, 0, True\n0, 1, False\n";

        write_operational_domain(opdom, os, params);

        CHECK(os.str() == expected);
    }
}

TEST_CASE("Write operational domain with floating-point parameter values", "[write-operational-domain]")
{
    operational_domain opdom{};
    opdom.operational_values = {{{0.1, 0.2}, operational_domain::operational_status::OPERATIONAL},
                                {{0.3, 0.4}, operational_domain::operational_status::NON_OPERATIONAL},
                                {{1.2, 1.4}, operational_domain::operational_status::OPERATIONAL},
                                {{2.4, 5.75}, operational_domain::operational_status::NON_OPERATIONAL}};

    std::ostringstream os{};

    SECTION("default operational tags")
    {
        static constexpr const char* expected =
            "epsilon_r, lambda_tf, operational status\n0.1, 0.2, operational\n0.3, 0.4, non-operational\n1.2, 1.4, "
            "operational\n2.4, 5.75, non-operational\n";

        write_operational_domain(opdom, os);

        CHECK(os.str() == expected);
    }
    SECTION("custom operational tags")
    {
        const write_operational_domain_params params = {"1", "0"};

        static constexpr const char* expected = "epsilon_r, lambda_tf, operational status\n0.1, 0.2, 1\n0.3, 0.4, "
                                                "0\n1.2, 1.4, 1\n2.4, 5.75, 0\n";

        write_operational_domain(opdom, os, params);

        CHECK(os.str() == expected);
    }
}
