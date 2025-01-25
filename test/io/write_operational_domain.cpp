//
// Created by marcel on 24.06.22.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>
#include <fiction/io/write_operational_domain.hpp>

#include <set>
#include <sstream>
#include <string>

using namespace fiction;

TEST_CASE("Write empty operational domain", "[write-operational-domain]")
{
    std::ostringstream os{};

    SECTION("default sweep dimensions")
    {
        operational_domain opdom{};

        opdom.dimensions.push_back(sweep_parameter::EPSILON_R);
        opdom.dimensions.push_back(sweep_parameter::LAMBDA_TF);

        static constexpr const char* expected = "epsilon_r,lambda_tf,operational status\n";

        write_operational_domain(opdom, os);

        CHECK(os.str() == expected);
    }
    SECTION("custom sweep dimensions")
    {
        operational_domain opdom{};

        opdom.dimensions.push_back(sweep_parameter::LAMBDA_TF);
        opdom.dimensions.push_back(sweep_parameter::MU_MINUS);

        static constexpr const char* expected = "lambda_tf,mu_minus,operational status\n";

        write_operational_domain(opdom, os);

        CHECK(os.str() == expected);
    }
}

TEST_CASE("Write simple operational domain", "[write-operational-domain]")
{
    operational_domain opdom{};

    opdom.dimensions.push_back(sweep_parameter::EPSILON_R);
    opdom.dimensions.push_back(sweep_parameter::LAMBDA_TF);

    opdom.operational_values = {{parameter_point{{0, 0}}, operational_status::OPERATIONAL},
                                {parameter_point{{0, 1}}, operational_status::NON_OPERATIONAL}};

    std::ostringstream os{};

    SECTION("default operational tags")
    {
        std::set<std::string> expected{"epsilon_r,lambda_tf,operational status", "0,0,1", "0,1,0"};

        write_operational_domain(opdom, os);
        const auto os_str = os.str();

        std::istringstream is{os_str};

        for (std::string line{}; std::getline(is, line);)
        {
            CHECK(expected.find(line) != expected.end());
        }
    }

    SECTION("custom operational tags")
    {
        const write_operational_domain_params params = {"True", "False"};

        std::set<std::string> expected{"epsilon_r,lambda_tf,operational status", "0,0,True", "0,1,False"};

        write_operational_domain(opdom, os, params);
        const auto os_str = os.str();

        std::istringstream is{os_str};

        for (std::string line{}; std::getline(is, line);)
        {
            CHECK(expected.find(line) != expected.end());
        }
    }

    SECTION("skip non-operational samples")
    {
        write_operational_domain_params params{};
        params.writing_mode = write_operational_domain_params::sample_writing_mode::OPERATIONAL_ONLY;

        std::set<std::string> expected{"epsilon_r,lambda_tf,operational status", "0,0,1"};

        write_operational_domain(opdom, os, params);
        const auto os_str = os.str();

        std::istringstream is{os_str};

        for (std::string line{}; std::getline(is, line);)
        {
            CHECK(expected.find(line) != expected.end());
        }
    }
}

TEST_CASE("Write operational domain with floating-point parameter values", "[write-operational-domain]")
{
    operational_domain opdom{};

    opdom.dimensions.push_back(sweep_parameter::EPSILON_R);
    opdom.dimensions.push_back(sweep_parameter::LAMBDA_TF);

    opdom.operational_values = {{parameter_point{{0.1, 0.2}}, operational_status::OPERATIONAL},
                                {parameter_point{{0.3, 0.4}}, operational_status::NON_OPERATIONAL},
                                {parameter_point{{1.2, 1.4}}, operational_status::OPERATIONAL},
                                {parameter_point{{2.4, 5.75}}, operational_status::NON_OPERATIONAL}};

    std::ostringstream os{};

    SECTION("default operational tags")
    {
        std::set<std::string> expected{"epsilon_r,lambda_tf,operational status", "0.1,0.2,1", "0.3,0.4,0", "1.2,1.4,1",
                                       "2.4,5.75,0"};

        write_operational_domain(opdom, os);
        const auto os_str = os.str();

        std::istringstream is{os_str};

        for (std::string line{}; std::getline(is, line);)
        {
            CHECK(expected.find(line) != expected.end());
        }
    }
    SECTION("custom operational tags")
    {
        const write_operational_domain_params params = {"operational", "non-operational"};

        std::set<std::string> expected{"epsilon_r,lambda_tf,operational status", "0.1,0.2,operational",
                                       "0.3,0.4,non-operational", "1.2,1.4,operational", "2.4,5.75,non-operational"};

        write_operational_domain(opdom, os, params);
        const auto os_str = os.str();

        std::istringstream is{os_str};

        for (std::string line{}; std::getline(is, line);)
        {
            CHECK(expected.find(line) != expected.end());
        }
    }

    SECTION("skip non-operational samples")
    {
        write_operational_domain_params params{};
        params.writing_mode = write_operational_domain_params::sample_writing_mode::OPERATIONAL_ONLY;

        std::set<std::string> expected{"epsilon_r,lambda_tf,operational status", "0.1,0.2,1", "1.2,1.4,1"};

        write_operational_domain(opdom, os, params);
        const auto os_str = os.str();

        std::istringstream is{os_str};

        for (std::string line{}; std::getline(is, line);)
        {
            CHECK(expected.find(line) != expected.end());
        }
    }
}

TEST_CASE("Write operational domain with floating-point parameter and temperature values", "[write-operational-domain]")
{
    operational_domain opdom{};

    opdom.dimensions.push_back(sweep_parameter::EPSILON_R);
    opdom.dimensions.push_back(sweep_parameter::LAMBDA_TF);

    opdom.operational_values = {{parameter_point{{0.1, 0.2}}, operational_status::OPERATIONAL},
                                {parameter_point{{0.3, 0.4}}, operational_status::NON_OPERATIONAL},
                                {parameter_point{{1.2, 1.4}}, operational_status::OPERATIONAL},
                                {parameter_point{{2.4, 5.75}}, operational_status::NON_OPERATIONAL}};

    opdom.metric_values = {{parameter_point{{0.1, 0.2}}, 50.3},
                           {parameter_point{{0.3, 0.4}}, 0.0},
                           {parameter_point{{1.2, 1.4}}, 400.00},
                           {parameter_point{{2.4, 5.75}}, 0.0}};
    ;

    std::ostringstream os{};

    SECTION("default operational tags")
    {
        std::set<std::string> expected{"epsilon_r,lambda_tf,operational status,ct", "0.1,0.2,1,50.3", "0.3,0.4,0,0",
                                       "1.2,1.4,1,400", "2.4,5.75,0,0"};

        write_operational_domain(opdom, os);
        const auto os_str = os.str();

        std::istringstream is{os_str};

        for (std::string line{}; std::getline(is, line);)
        {
            CHECK(expected.find(line) != expected.end());
        }
    }
    SECTION("custom operational tags")
    {
        const write_operational_domain_params params = {"operational", "non-operational"};

        std::set<std::string> expected{"epsilon_r,lambda_tf,operational status,ct", "0.1,0.2,operational,50.3",
                                       "0.3,0.4,non-operational,0", "1.2,1.4,operational,400",
                                       "2.4,5.75,non-operational,0"};

        write_operational_domain(opdom, os, params);
        const auto os_str = os.str();

        std::istringstream is{os_str};

        for (std::string line{}; std::getline(is, line);)
        {
            CHECK(expected.find(line) != expected.end());
        }
    }

    SECTION("skip non-operational samples")
    {
        write_operational_domain_params params{};
        params.writing_mode = write_operational_domain_params::sample_writing_mode::OPERATIONAL_ONLY;

        std::set<std::string> expected{"epsilon_r,lambda_tf,operational status,ct", "0.1,0.2,1,50.3", "1.2,1.4,1,400"};

        write_operational_domain(opdom, os, params);
        const auto os_str = os.str();

        std::istringstream is{os_str};

        for (std::string line{}; std::getline(is, line);)
        {
            CHECK(expected.find(line) != expected.end());
        }
    }
}
