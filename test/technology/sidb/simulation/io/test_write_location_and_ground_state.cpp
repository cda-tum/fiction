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
 * @brief Tests for `fiction/technology/sidb/simulation/io/write_location_and_ground_state.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/charge_state.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/engines/exhaustive_ground_state_simulation.hpp>
#include <fiction/technology/sidb/simulation/io/write_location_and_ground_state.hpp>
#include <fiction/technology/sidb/simulation/result.hpp>

#include <cctype>
#include <initializer_list>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation;
using namespace fiction::sidb::simulation::engines;
using namespace fiction::sidb::simulation::io;

// Helper function to compare string output with expected string
bool compare_output(const std::string& output, const std::string& expected)
{
    std::string clean_output = output;
    std::erase_if(clean_output, ::isspace);

    std::string clean_expected = expected;
    std::erase_if(clean_expected, ::isspace);

    return clean_output == clean_expected;
}

TEST_CASE("writes expected output", "[write_txt_sim_result]")
{
    layout lyt{};

    SECTION("Output is written to ostream correctly, degenerated GS")
    {
        lyt.assign_sidb({0, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({3, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({5, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({8, 0}, dot_tag::NORMAL);

        const simulation_parameters params{2, -0.32};
        const auto                  simulation_results = exhaustive_ground_state_simulation(lyt, params);

        std::stringstream ss;
        write_location_and_ground_state(simulation_results, ss);

        const std::string expected_output = R"(x [nm];y [nm];GS_0;GS_1;
                                                0.000;0.000;-1;-1;
                                                1.152;0.000;0;-1;
                                                1.920;0.000;-1;0;
                                                3.072;0.000;-1;-1;)";

        const std::string expected_output_second = R"(x [nm];y [nm];GS_0;GS_1;
                                                0.000;0.000;-1;-1;
                                                1.152;0.000;-1;0;
                                                1.920;0.000;0;-1;
                                                3.072;0.000;-1;-1;)";

        REQUIRE((compare_output(ss.str(), expected_output) || compare_output(ss.str(), expected_output_second)));
    }

    SECTION("Output is written to ostream correctly, unique GS")
    {
        lyt.assign_sidb({0, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({3, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({5, 0}, dot_tag::NORMAL);

        const simulation_parameters params{2, -0.32};
        const auto                  simulation_results = exhaustive_ground_state_simulation(lyt, params);

        std::stringstream ss;
        write_location_and_ground_state(simulation_results, ss);

        const std::string expected_output = R"(x [nm];y [nm];GS_0;
                                                0.000;0.000;-1;
                                                1.152;0.000;0;
                                                1.920;0.000;-1;)";

        REQUIRE(compare_output(ss.str(), expected_output));
    }
}

TEST_CASE("Ground-state CSV rejects mismatched distribution sites before output", "[write_txt_sim_result]")
{
    layout lyt{};
    lyt.assign_sidb({0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({2, 0}, dot_tag::NORMAL);
    layout other{};
    /** @brief Check rejection of a distribution without modifying the destination stream. */
    const auto check_rejected = [&lyt, &other](const double energy)
    {
        result res{};
        res.lyt = lyt;
        charge_distribution mismatch{other};
        mismatch.assign_energy(energy);
        res.charge_distributions = {charge_distribution{lyt}, mismatch};
        std::stringstream os{};
        os << "original";
        CHECK_THROWS_AS(write_location_and_ground_state(res, os), std::invalid_argument);
        CHECK_THAT(os.str(), Catch::Matchers::Equals("original"));
    };

    SECTION("fewer distribution sites")
    {
        other.assign_sidb({0, 0}, dot_tag::NORMAL);
        check_rejected(0.0);
    }
    SECTION("shifted distribution sites")
    {
        other.assign_sidb({1, 0}, dot_tag::NORMAL);
        other.assign_sidb({3, 0}, dot_tag::NORMAL);
        check_rejected(0.0);
    }
    SECTION("mismatched excited state")
    {
        other.assign_sidb({1, 0}, dot_tag::NORMAL);
        other.assign_sidb({3, 0}, dot_tag::NORMAL);
        check_rejected(1.0);
    }
}

TEST_CASE("Ground-state CSV writes nothing for an empty result", "[write_txt_sim_result]")
{
    std::stringstream os{};
    os << "original";
    write_location_and_ground_state(result{}, os);
    CHECK_THAT(os.str(), Catch::Matchers::Equals("original"));
}

TEST_CASE("Ground-state CSV preserves distributions across site-list sharing", "[write_txt_sim_result]")
{
    layout lyt{};
    lyt.assign_sidb({0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({2, 0}, dot_tag::NORMAL);
    const charge_distribution first{lyt};
    result                    res{};
    res.lyt = lyt;
    /** @brief Check complete CSV output, including repeated columns, charge order, and coordinate precision. */
    const auto check_output = [&res]
    {
        std::stringstream os{};
        write_location_and_ground_state(res, os);
        CHECK_THAT(os.str(), Catch::Matchers::Equals("x [nm]; y [nm];GS_0;GS_1;GS_2;\n"
                                                     "0.000;0.000;-1;0;-1;\n"
                                                     "0.768;0.000;-1;0;-1;\n"));
    };

    SECTION("shared site lists")
    {
        res.charge_distributions = {first, charge_distribution{first.shared_sites(), charge_state::NEUTRAL}, first};
        check_output();
    }
    SECTION("distinct equal-valued site lists")
    {
        res.charge_distributions = {charge_distribution{lyt}, charge_distribution{lyt, charge_state::NEUTRAL},
                                    charge_distribution{lyt}};
        check_output();
    }
    SECTION("nonconsecutive shared site list")
    {
        res.charge_distributions = {first, charge_distribution{lyt, charge_state::NEUTRAL}, first};
        check_output();
    }
    SECTION("largest-magnitude finite energies")
    {
        for (const auto energy : {std::numeric_limits<double>::max(), std::numeric_limits<double>::lowest()})
        {
            res.charge_distributions = {first, charge_distribution{first.shared_sites(), charge_state::NEUTRAL}, first};
            for (auto& cd : res.charge_distributions)
            {
                cd.assign_energy(energy);
            }
            check_output();
        }
    }
    SECTION("excited mismatch after shared distributions")
    {
        layout shifted{};
        shifted.assign_sidb({0, 0}, dot_tag::NORMAL);
        shifted.assign_sidb({3, 0}, dot_tag::NORMAL);
        charge_distribution mismatch{shifted};
        mismatch.assign_energy(1.0);
        res.charge_distributions = {first, charge_distribution{first.shared_sites(), charge_state::NEUTRAL}, mismatch};
        std::stringstream os{};
        os << "original";
        CHECK_THROWS_AS(write_location_and_ground_state(res, os), std::invalid_argument);
        CHECK_THAT(os.str(), Catch::Matchers::Equals("original"));
    }
}

TEST_CASE("Ground-state CSV rejects nonfinite energies before output", "[write_txt_sim_result]")
{
    layout lyt{};
    lyt.assign_sidb({0, 0}, dot_tag::NORMAL);
    result res{};
    res.lyt = lyt;
    for (const auto energy : {std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::infinity(),
                              -std::numeric_limits<double>::infinity()})
    {
        for (const auto index : {0u, 1u})
        {
            res.charge_distributions = {charge_distribution{lyt}, charge_distribution{lyt}};
            res.charge_distributions[index].assign_energy(energy);
            std::stringstream os{};
            os << "original";
            CAPTURE(energy, index);
            CHECK_THROWS_AS(write_location_and_ground_state(res, os), std::invalid_argument);
            CHECK_THAT(os.str(), Catch::Matchers::Equals("original"));
        }
    }
}
