//
// Created by marcel on 24.06.22.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/io/write_operational_domain.hpp>
#include <fiction/technology/sidb/simulation/logic/is_operational.hpp>
#include <fiction/technology/sidb/simulation/logic/operational_domain.hpp>

#include <cstddef>
#include <set>
#include <sstream>
#include <string>

using namespace fiction;

TEST_CASE("Write empty operational domain", "[write-operational-domain]")
{
    std::ostringstream os{};

    SECTION("default sweep dimensions")
    {
        sidb::simulation::logic::operational_domain opdom{};

        opdom.add_dimension(sidb::simulation::logic::sweep_parameter::EPSILON_R);
        opdom.add_dimension(sidb::simulation::logic::sweep_parameter::LAMBDA_TF);

        static constexpr const char* expected = "epsilon_r,lambda_tf,operational status\n";

        write_operational_domain(opdom, os);

        CHECK(os.str() == expected);
    }
    SECTION("One sweep dimensions")
    {
        sidb::simulation::logic::operational_domain opdom{};

        opdom.add_dimension(sidb::simulation::logic::sweep_parameter::LAMBDA_TF);

        static constexpr const char* expected = "lambda_tf,operational status\n";

        write_operational_domain(opdom, os);

        CHECK(os.str() == expected);
    }
    SECTION("Two sweep dimensions")
    {
        sidb::simulation::logic::operational_domain opdom{};

        opdom.add_dimension(sidb::simulation::logic::sweep_parameter::LAMBDA_TF);
        opdom.add_dimension(sidb::simulation::logic::sweep_parameter::MU_MINUS);

        static constexpr const char* expected = "lambda_tf,mu_minus,operational status\n";

        write_operational_domain(opdom, os);

        CHECK(os.str() == expected);
    }
    SECTION("Three sweep dimensions")
    {
        sidb::simulation::logic::operational_domain opdom{};

        opdom.add_dimension(sidb::simulation::logic::sweep_parameter::LAMBDA_TF);
        opdom.add_dimension(sidb::simulation::logic::sweep_parameter::MU_MINUS);
        opdom.add_dimension(sidb::simulation::logic::sweep_parameter::EPSILON_R);

        static constexpr const char* expected = "lambda_tf,mu_minus,epsilon_r,operational status\n";

        write_operational_domain(opdom, os);

        CHECK(os.str() == expected);
    }
}

TEST_CASE("Write simple operational domain", "[write-operational-domain]")
{
    sidb::simulation::logic::operational_domain opdom{};

    opdom.add_dimension(sidb::simulation::logic::sweep_parameter::EPSILON_R);
    opdom.add_dimension(sidb::simulation::logic::sweep_parameter::LAMBDA_TF);

    opdom.add_value(sidb::simulation::logic::parameter_point{{0, 0}},
                    {sidb::simulation::logic::operational_status::OPERATIONAL});
    opdom.add_value(sidb::simulation::logic::parameter_point{{0, 1}},
                    {sidb::simulation::logic::operational_status::NON_OPERATIONAL});

    std::ostringstream os{};

    SECTION("default operational tags")
    {
        const std::set<std::string> expected{"epsilon_r,lambda_tf,operational status", "0,0,1", "0,1,0"};

        write_operational_domain(opdom, os);
        const auto os_str = os.str();

        std::istringstream is{os_str};

        for (std::string line{}; std::getline(is, line);)
        {
            CHECK(expected.contains(line));
        }
    }

    SECTION("custom operational tags")
    {
        const write_operational_domain_params params{.operational_tag = "True", .non_operational_tag = "False"};

        const std::set<std::string> expected{"epsilon_r,lambda_tf,operational status", "0,0,True", "0,1,False"};

        write_operational_domain(opdom, os, params);
        const auto os_str = os.str();

        std::istringstream is{os_str};

        for (std::string line{}; std::getline(is, line);)
        {
            CHECK(expected.contains(line));
        }
    }

    SECTION("skip non-operational samples")
    {
        write_operational_domain_params const params{
            .writing_mode = write_operational_domain_params::sample_writing_mode::OPERATIONAL_ONLY};

        const std::set<std::string> expected{"epsilon_r,lambda_tf,operational status", "0,0,1"};

        write_operational_domain(opdom, os, params);
        const auto os_str = os.str();

        std::istringstream is{os_str};

        for (std::string line{}; std::getline(is, line);)
        {
            CHECK(expected.contains(line));
        }
    }
}

TEST_CASE("Write operational domain with one and three sweep dimensions", "[write-operational-domain]")
{
    SECTION("one sweep dimension")
    {
        sidb::simulation::logic::operational_domain opdom{};

        opdom.add_dimension(sidb::simulation::logic::sweep_parameter::EPSILON_R);

        opdom.add_value(sidb::simulation::logic::parameter_point{{0.1}},
                        {sidb::simulation::logic::operational_status::OPERATIONAL});
        opdom.add_value(sidb::simulation::logic::parameter_point{{0.3}},
                        {sidb::simulation::logic::operational_status::NON_OPERATIONAL});

        const std::set<std::string> expected{"epsilon_r,operational status", "0.1,1", "0.3,0"};

        std::ostringstream os{};
        write_operational_domain(opdom, os);

        std::istringstream is{os.str()};

        std::set<std::string> emitted{};

        std::size_t num_lines = 0;

        for (std::string line{}; std::getline(is, line); ++num_lines)
        {
            emitted.insert(line);
        }

        // the emitted rows must be exactly the expected ones, and the line count must match so that a duplicated
        // row cannot mask a missing one
        CHECK(emitted == expected);
        CHECK(num_lines == expected.size());
    }

    SECTION("three sweep dimensions")
    {
        sidb::simulation::logic::operational_domain opdom{};

        opdom.add_dimension(sidb::simulation::logic::sweep_parameter::EPSILON_R);
        opdom.add_dimension(sidb::simulation::logic::sweep_parameter::LAMBDA_TF);
        opdom.add_dimension(sidb::simulation::logic::sweep_parameter::MU_MINUS);

        opdom.add_value(sidb::simulation::logic::parameter_point{{0.1, 0.2, -0.3}},
                        {sidb::simulation::logic::operational_status::OPERATIONAL});
        opdom.add_value(sidb::simulation::logic::parameter_point{{0.4, 0.5, -0.6}},
                        {sidb::simulation::logic::operational_status::NON_OPERATIONAL});

        const std::set<std::string> expected{"epsilon_r,lambda_tf,mu_minus,operational status", "0.1,0.2,-0.3,1",
                                             "0.4,0.5,-0.6,0"};

        std::ostringstream os{};
        write_operational_domain(opdom, os);

        std::istringstream is{os.str()};

        std::set<std::string> emitted{};

        std::size_t num_lines = 0;

        for (std::string line{}; std::getline(is, line); ++num_lines)
        {
            emitted.insert(line);
        }

        // the emitted rows must be exactly the expected ones, and the line count must match so that a duplicated
        // row cannot mask a missing one
        CHECK(emitted == expected);
        CHECK(num_lines == expected.size());
    }

    SECTION("three sweep dimensions with critical temperature")
    {
        sidb::simulation::logic::critical_temperature_domain opdom{};

        opdom.add_dimension(sidb::simulation::logic::sweep_parameter::EPSILON_R);
        opdom.add_dimension(sidb::simulation::logic::sweep_parameter::LAMBDA_TF);
        opdom.add_dimension(sidb::simulation::logic::sweep_parameter::MU_MINUS);

        opdom.add_value(sidb::simulation::logic::parameter_point{{0.1, 0.2, -0.3}},
                        {sidb::simulation::logic::operational_status::OPERATIONAL, 50.3});
        opdom.add_value(sidb::simulation::logic::parameter_point{{0.4, 0.5, -0.6}},
                        {sidb::simulation::logic::operational_status::NON_OPERATIONAL, 0.0});

        const std::set<std::string> expected{"epsilon_r,lambda_tf,mu_minus,operational status,critical temperature",
                                             "0.1,0.2,-0.3,1,50.3", "0.4,0.5,-0.6,0,0"};

        std::ostringstream os{};
        write_operational_domain(opdom, os);

        std::istringstream is{os.str()};

        std::set<std::string> emitted{};

        std::size_t num_lines = 0;

        for (std::string line{}; std::getline(is, line); ++num_lines)
        {
            emitted.insert(line);
        }

        // the emitted rows must be exactly the expected ones, and the line count must match so that a duplicated
        // row cannot mask a missing one
        CHECK(emitted == expected);
        CHECK(num_lines == expected.size());
    }
}

TEST_CASE("Write operational domain with floating-point parameter values", "[write-operational-domain]")
{
    sidb::simulation::logic::operational_domain opdom{};

    opdom.add_dimension(sidb::simulation::logic::sweep_parameter::EPSILON_R);
    opdom.add_dimension(sidb::simulation::logic::sweep_parameter::LAMBDA_TF);

    opdom.add_value(sidb::simulation::logic::parameter_point{{0.1, 0.2}},
                    {sidb::simulation::logic::operational_status::OPERATIONAL});
    opdom.add_value(sidb::simulation::logic::parameter_point{{0.3, 0.4}},
                    {sidb::simulation::logic::operational_status::NON_OPERATIONAL});
    opdom.add_value(sidb::simulation::logic::parameter_point{{1.2, 1.4}},
                    {sidb::simulation::logic::operational_status::OPERATIONAL});
    opdom.add_value(sidb::simulation::logic::parameter_point{{2.4, 5.75}},
                    {sidb::simulation::logic::operational_status::NON_OPERATIONAL});

    std::ostringstream os{};

    SECTION("default operational tags")
    {
        const std::set<std::string> expected{"epsilon_r,lambda_tf,operational status", "0.1,0.2,1", "0.3,0.4,0",
                                             "1.2,1.4,1", "2.4,5.75,0"};

        write_operational_domain(opdom, os);
        const auto os_str = os.str();

        std::istringstream is{os_str};

        for (std::string line{}; std::getline(is, line);)
        {
            CHECK(expected.contains(line));
        }
    }
    SECTION("custom operational tags")
    {
        const write_operational_domain_params params{.operational_tag     = "operational",
                                                     .non_operational_tag = "non-operational"};

        const std::set<std::string> expected{"epsilon_r,lambda_tf,operational status", "0.1,0.2,operational",
                                             "0.3,0.4,non-operational", "1.2,1.4,operational",
                                             "2.4,5.75,non-operational"};

        write_operational_domain(opdom, os, params);
        const auto os_str = os.str();

        std::istringstream is{os_str};

        for (std::string line{}; std::getline(is, line);)
        {
            CHECK(expected.contains(line));
        }
    }

    SECTION("skip non-operational samples")
    {
        write_operational_domain_params const params{
            .writing_mode = write_operational_domain_params::sample_writing_mode::OPERATIONAL_ONLY};

        const std::set<std::string> expected{"epsilon_r,lambda_tf,operational status", "0.1,0.2,1", "1.2,1.4,1"};

        write_operational_domain(opdom, os, params);
        const auto os_str = os.str();

        std::istringstream is{os_str};

        for (std::string line{}; std::getline(is, line);)
        {
            CHECK(expected.contains(line));
        }
    }
}

TEST_CASE("Write operational domain with floating-point parameter and temperature values", "[write-operational-domain]")
{
    SECTION("One sweep parameters")
    {
        sidb::simulation::logic::critical_temperature_domain opdom{};

        opdom.add_dimension(sidb::simulation::logic::sweep_parameter::EPSILON_R);

        opdom.add_value(sidb::simulation::logic::parameter_point{{0.1}},
                        {sidb::simulation::logic::operational_status::OPERATIONAL, 50.3});
        opdom.add_value(sidb::simulation::logic::parameter_point{{0.3}},
                        {sidb::simulation::logic::operational_status::NON_OPERATIONAL, 0.0});
        opdom.add_value(sidb::simulation::logic::parameter_point{{1.2}},
                        {sidb::simulation::logic::operational_status::OPERATIONAL, 400.0});
        opdom.add_value(sidb::simulation::logic::parameter_point{{2.4}},
                        {sidb::simulation::logic::operational_status::NON_OPERATIONAL, 0.0});

        std::ostringstream os{};

        SECTION("default operational tags")
        {
            const std::set<std::string> expected{"epsilon_r,operational status,critical temperature", "0.1,1,50.3",
                                                 "0.3,0,0", "1.2,1,400", "2.4,0,0"};

            write_operational_domain(opdom, os);
            const auto os_str = os.str();

            std::istringstream is{os_str};

            for (std::string line{}; std::getline(is, line);)
            {
                CHECK(expected.contains(line));
            }
        }
        SECTION("custom operational tags")
        {
            const write_operational_domain_params params{.operational_tag     = "operational",
                                                         .non_operational_tag = "non-operational"};

            const std::set<std::string> expected{"epsilon_r,operational status,critical temperature",
                                                 "0.1,operational,50.3", "0.3,non-operational,0", "1.2,operational,400",
                                                 "2.4,non-operational,0"};

            write_operational_domain(opdom, os, params);
            const auto os_str = os.str();

            std::istringstream is{os_str};

            for (std::string line{}; std::getline(is, line);)
            {
                CHECK(expected.contains(line));
            }
        }

        SECTION("skip non-operational samples")
        {
            write_operational_domain_params const params{
                .writing_mode = write_operational_domain_params::sample_writing_mode::OPERATIONAL_ONLY};

            const std::set<std::string> expected{"epsilon_r,operational status,critical temperature", "0.1,1,50.3",
                                                 "1.2,1,400"};

            write_operational_domain(opdom, os, params);
            const auto os_str = os.str();

            std::istringstream is{os_str};

            for (std::string line{}; std::getline(is, line);)
            {
                CHECK(expected.contains(line));
            }
        }
    }
    SECTION("Two sweep parameters")
    {
        sidb::simulation::logic::critical_temperature_domain opdom{};

        opdom.add_dimension(sidb::simulation::logic::sweep_parameter::EPSILON_R);
        opdom.add_dimension(sidb::simulation::logic::sweep_parameter::LAMBDA_TF);

        opdom.add_value(sidb::simulation::logic::parameter_point{{0.1, 0.2}},
                        {sidb::simulation::logic::operational_status::OPERATIONAL, 50.3});
        opdom.add_value(sidb::simulation::logic::parameter_point{{0.3, 0.4}},
                        {sidb::simulation::logic::operational_status::NON_OPERATIONAL, 0.0});
        opdom.add_value(sidb::simulation::logic::parameter_point{{1.2, 1.4}},
                        {sidb::simulation::logic::operational_status::OPERATIONAL, 400.0});
        opdom.add_value(sidb::simulation::logic::parameter_point{{2.4, 5.75}},
                        {sidb::simulation::logic::operational_status::NON_OPERATIONAL, 0.0});

        std::ostringstream os{};

        SECTION("default operational tags")
        {
            const std::set<std::string> expected{"epsilon_r,lambda_tf,operational status,critical temperature",
                                                 "0.1,0.2,1,50.3", "0.3,0.4,0,0", "1.2,1.4,1,400", "2.4,5.75,0,0"};

            write_operational_domain(opdom, os);
            const auto os_str = os.str();

            std::istringstream is{os_str};

            for (std::string line{}; std::getline(is, line);)
            {
                CHECK(expected.contains(line));
            }
        }
        SECTION("custom operational tags")
        {
            const write_operational_domain_params params{.operational_tag     = "operational",
                                                         .non_operational_tag = "non-operational"};

            const std::set<std::string> expected{"epsilon_r,lambda_tf,operational status,critical temperature",
                                                 "0.1,0.2,operational,50.3", "0.3,0.4,non-operational,0",
                                                 "1.2,1.4,operational,400", "2.4,5.75,non-operational,0"};

            write_operational_domain(opdom, os, params);
            const auto os_str = os.str();

            std::istringstream is{os_str};

            for (std::string line{}; std::getline(is, line);)
            {
                CHECK(expected.contains(line));
            }
        }

        SECTION("skip non-operational samples")
        {
            write_operational_domain_params const params{
                .writing_mode = write_operational_domain_params::sample_writing_mode::OPERATIONAL_ONLY};

            const std::set<std::string> expected{"epsilon_r,lambda_tf,operational status,critical temperature",
                                                 "0.1,0.2,1,50.3", "1.2,1.4,1,400"};

            write_operational_domain(opdom, os, params);
            const auto os_str = os.str();

            std::istringstream is{os_str};

            for (std::string line{}; std::getline(is, line);)
            {
                CHECK(expected.contains(line));
            }
        }
    }
}
