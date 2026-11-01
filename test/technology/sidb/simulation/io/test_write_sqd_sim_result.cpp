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
 * @brief Tests for `fiction/technology/sidb/simulation/io/write_sqd_sim_result.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/engines/exhaustive_ground_state_simulation.hpp>
#include <fiction/technology/sidb/simulation/io/write_sqd_sim_result.hpp>
#include <fiction/technology/sidb/simulation/result.hpp>
#include <fiction/utils/stl/stl_utils.hpp>
#include <fiction/utils/version_info.hpp>

#include <fmt/format.h>
#include <tinyxml2.h>

#include <any>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <ctime>
#include <initializer_list>
#include <iomanip>
#include <limits>
#include <numbers>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation;
using namespace fiction::sidb::simulation::engines;
using namespace fiction::sidb::simulation::io;
using namespace fiction::utils::stl;

namespace
{

/**
 * @brief Check that a canonical local timestamp denotes a second within an inclusive epoch interval.
 * @param timestamp Local timestamp to validate.
 * @param before First permitted epoch second.
 * @param after Last permitted epoch second.
 * @return Whether the timestamp denotes a second in the interval.
 */
bool timestamp_in_window(const std::string& timestamp, const std::time_t before, const std::time_t after)
{
    std::tm            parsed{};
    std::istringstream input{timestamp};
    input >> std::get_time(&parsed, "%Y-%m-%d %H:%M:%S");
    if (input.fail() || input.peek() != std::char_traits<char>::eof())
    {
        return false;
    }
    // Both DST interpretations can denote a valid local time during a fall-back transition.
    for (const auto daylight_saving : {-1, 0, 1})
    {
        auto candidate     = parsed;
        candidate.tm_isdst = daylight_saving;
        const auto epoch   = std::mktime(&candidate);
        if (epoch >= before && epoch <= after &&
            fmt::format("{:%Y-%m-%d %H:%M:%S}", safe_localtime(epoch)) == timestamp)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Write a simulation result and validate the timestamp independently of the document oracle.
 * @param res Simulation result to write.
 * @param os Destination stream.
 * @return Validated timestamp text.
 */
std::string write_timestamped_result(const result& res, std::stringstream& os)
{
    const auto before = std::time(nullptr);
    write_sqd_sim_result(res, os);
    const auto            after = std::time(nullptr);
    tinyxml2::XMLDocument document{};
    REQUIRE(document.Parse(os.str().c_str()) == tinyxml2::XML_SUCCESS);
    const auto* root = document.FirstChildElement("sim_out");
    REQUIRE(root != nullptr);
    const auto* engine_info = root->FirstChildElement("eng_info");
    REQUIRE(engine_info != nullptr);
    const auto* timestamp = engine_info->FirstChildElement("timestamp");
    REQUIRE(timestamp != nullptr);
    REQUIRE(timestamp->GetText() != nullptr);
    const std::string text{timestamp->GetText()};
    CHECK(timestamp_in_window(text, before, after));
    return text;
}

}  // namespace

TEST_CASE("SQD timestamp oracle accepts calendar boundaries and rejects invalid times", "[sqd-sim-result]")
{
    std::tm boundary{};
    boundary.tm_year  = 126;
    boundary.tm_mon   = 0;
    boundary.tm_mday  = 1;
    boundary.tm_hour  = 23;
    boundary.tm_min   = 59;
    boundary.tm_sec   = 59;
    boundary.tm_isdst = -1;
    const auto before = std::mktime(&boundary);
    REQUIRE(before != std::time_t{-1});
    CHECK(timestamp_in_window("2026-01-01 23:59:59", before, before));
    CHECK(timestamp_in_window("2026-01-02 00:00:00", before, before + 2));
    CHECK_FALSE(timestamp_in_window("2026-01-02 00:00:02", before, before + 2));
    CHECK_FALSE(timestamp_in_window("2026-02-30 00:00:00", before, before + 90 * 86400));
    CHECK_FALSE(timestamp_in_window("2026-01-02", before, before + 2));
    CHECK_FALSE(timestamp_in_window("2026-1-2 00:00:00", before, before + 2));
}

TEST_CASE("Utility function: any_to_string", "[sqd-sim-result]")
{
    SECTION("Empty std::any")
    {
        const std::any value{};
        const auto     result = sidb::simulation::io::detail::any_to_string(value);

        CHECK(result.empty());
    }
    SECTION("int8_t")
    {
        constexpr int8_t value  = -42;
        const auto       result = sidb::simulation::io::detail::any_to_string(value);

        CHECK(result == "-42");
    }
    SECTION("uint8_t")
    {
        constexpr uint8_t value  = 42;
        const auto        result = sidb::simulation::io::detail::any_to_string(value);

        CHECK(result == "42");
    }
    SECTION("int16_t")
    {
        constexpr int16_t value  = -32768;
        const auto        result = sidb::simulation::io::detail::any_to_string(value);

        CHECK(result == "-32768");
    }
    SECTION("uint16_t")
    {
        constexpr uint16_t value  = 65535;
        const auto         result = sidb::simulation::io::detail::any_to_string(value);

        CHECK(result == "65535");
    }
    SECTION("int32_t")
    {
        constexpr int32_t value  = -2'147'483'648;
        const auto        result = sidb::simulation::io::detail::any_to_string(value);

        CHECK(result == "-2147483648");
    }
    SECTION("uint32_t")
    {
        constexpr uint32_t value  = 4'294'967'295ul;
        const auto         result = sidb::simulation::io::detail::any_to_string(value);

        CHECK(result == "4294967295");
    }
    SECTION("int64_t")
    {
        constexpr int64_t value  = -9'223'372'036'854'775'807;
        const auto        result = sidb::simulation::io::detail::any_to_string(value);

        CHECK(result == "-9223372036854775807");
    }
    SECTION("uint64_t")
    {
        constexpr uint64_t value  = 18'446'744'073'709'551'615ull;
        const auto         result = sidb::simulation::io::detail::any_to_string(value);

        CHECK(result == "18446744073709551615");
    }
    SECTION("float")
    {
        constexpr float value  = -std::numbers::pi_v<float>;
        const auto      result = sidb::simulation::io::detail::any_to_string(value);

        CHECK(result == "-3.141593");
    }
    SECTION("double")
    {
        constexpr double value  = std::numbers::pi;
        const auto       result = sidb::simulation::io::detail::any_to_string(value);

        CHECK(result == "3.141593");  // will be rounded by std::to_string
    }
    SECTION("long double")
    {
        constexpr long double value  = std::numbers::e_v<long double>;
        const auto            result = sidb::simulation::io::detail::any_to_string(value);

        CHECK(result == "2.718282");  // will be rounded by std::to_string
    }
    SECTION("std::string")
    {
        const std::string value  = "hello, world!";
        const auto        result = sidb::simulation::io::detail::any_to_string(value);

        CHECK(result == "hello, world!");
    }
    SECTION("const char*")
    {
        const char* value  = "hello, world!";
        const auto  result = sidb::simulation::io::detail::any_to_string(value);

        CHECK(result == "hello, world!");
    }
    SECTION("char")
    {
        constexpr char value  = 'X';
        const auto     result = sidb::simulation::io::detail::any_to_string(value);

        CHECK(result == "X");
    }
    SECTION("unsupported type")
    {
        const std::vector value{1, 2, 3};
        const auto        result = sidb::simulation::io::detail::any_to_string(value);

        CHECK(result.empty());
    }
    SECTION("std::string in std::any")
    {
        const std::any value  = std::string{"hello, world!"};
        const auto     result = sidb::simulation::io::detail::any_to_string(value);

        CHECK(result == "hello, world!");
    }
    SECTION("const char* in std::any")
    {
        const std::any value  = "hello, world!";
        const auto     result = sidb::simulation::io::detail::any_to_string(value);

        CHECK(result == "hello, world!");
    }
}

TEST_CASE("Write empty simulation result", "[sqd-sim-result]")
{
    using namespace std::chrono_literals;

    result sim_result{};

    sim_result.algorithm_name     = "TestSim";
    sim_result.simulation_runtime = 42s;  // NOLINT(misc-include-cleaner)

    std::stringstream simulation_stream{};

    SECTION("without additional parameters")
    {
        const auto        timestamp = write_timestamped_result(sim_result, simulation_stream);
        const std::string sim_result_str =
            fmt::format("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                        "<sim_out>\n"
                        "    <eng_info>\n"
                        "        <engine>TestSim</engine>\n"
                        "        <version>{}</version>\n"
                        "        <repo>{}</repo>\n"
                        "        <return_code>0</return_code>\n"
                        "        <timestamp>{}</timestamp>\n"
                        "        <time_elapsed_s>42</time_elapsed_s>\n"
                        "    </eng_info>\n"
                        "    <sim_params>\n"
                        "        <debye_length>{}</debye_length>\n"
                        "        <eps_r>{}</eps_r>\n"
                        "        <muzm>{}</muzm>\n"
                        "    </sim_params>\n"
                        "    <physloc>\n"
                        "    </physloc>\n"
                        "    <elec_dist>\n"
                        "    </elec_dist>\n"
                        "</sim_out>\n",
                        FICTION_VERSION, FICTION_REPO, timestamp, sim_result.sim_params.lambda_tf,
                        sim_result.sim_params.epsilon_r, sim_result.sim_params.mu_minus);

        CHECK(simulation_stream.str() == sim_result_str);
    }
    SECTION("with additional parameter (string)")
    {
        sim_result.additional_simulation_parameters.emplace("param1", "value1");
        const auto        timestamp = write_timestamped_result(sim_result, simulation_stream);
        const std::string sim_result_str =
            fmt::format("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                        "<sim_out>\n"
                        "    <eng_info>\n"
                        "        <engine>TestSim</engine>\n"
                        "        <version>{}</version>\n"
                        "        <repo>{}</repo>\n"
                        "        <return_code>0</return_code>\n"
                        "        <timestamp>{}</timestamp>\n"
                        "        <time_elapsed_s>42</time_elapsed_s>\n"
                        "    </eng_info>\n"
                        "    <sim_params>\n"
                        "        <debye_length>{}</debye_length>\n"
                        "        <eps_r>{}</eps_r>\n"
                        "        <muzm>{}</muzm>\n"
                        "        <param1>value1</param1>\n"
                        "    </sim_params>\n"
                        "    <physloc>\n"
                        "    </physloc>\n"
                        "    <elec_dist>\n"
                        "    </elec_dist>\n"
                        "</sim_out>\n",
                        FICTION_VERSION, FICTION_REPO, timestamp, sim_result.sim_params.lambda_tf,
                        sim_result.sim_params.epsilon_r, sim_result.sim_params.mu_minus);

        CHECK(simulation_stream.str() == sim_result_str);
    }

    SECTION("with additional parameters (double)")
    {
        sim_result.additional_simulation_parameters.emplace("param3", 3.14);
        const auto        timestamp = write_timestamped_result(sim_result, simulation_stream);
        const std::string sim_result_str =
            fmt::format("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                        "<sim_out>\n"
                        "    <eng_info>\n"
                        "        <engine>TestSim</engine>\n"
                        "        <version>{}</version>\n"
                        "        <repo>{}</repo>\n"
                        "        <return_code>0</return_code>\n"
                        "        <timestamp>{}</timestamp>\n"
                        "        <time_elapsed_s>42</time_elapsed_s>\n"
                        "    </eng_info>\n"
                        "    <sim_params>\n"
                        "        <debye_length>{}</debye_length>\n"
                        "        <eps_r>{}</eps_r>\n"
                        "        <muzm>{}</muzm>\n"
                        "        <param3>3.140000</param3>\n"
                        "    </sim_params>\n"
                        "    <physloc>\n"
                        "    </physloc>\n"
                        "    <elec_dist>\n"
                        "    </elec_dist>\n"
                        "</sim_out>\n",
                        FICTION_VERSION, FICTION_REPO, timestamp, sim_result.sim_params.lambda_tf,
                        sim_result.sim_params.epsilon_r, sim_result.sim_params.mu_minus);

        CHECK(simulation_stream.str() == sim_result_str);
    }
}

TEST_CASE("Write simulation result with ExGS simulation", "[sqd-sim-result]")
{
    using namespace std::chrono_literals;

    layout lyt{};

    lyt.assign_sidb({0, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({5, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({7, 0, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({11, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({13, 0, 0}, dot_tag::NORMAL);

    lyt.assign_sidb({17, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({19, 0, 0}, dot_tag::NORMAL);

    const simulation_parameters params{2, -0.32};

    auto sim_result = exhaustive_ground_state_simulation(lyt, params);

    sim_result.algorithm_name = "ExGS";

    std::stringstream simulation_stream{};

    const auto        timestamp = write_timestamped_result(sim_result, simulation_stream);
    const std::string sim_result_str =
        fmt::format("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                    "<sim_out>\n"
                    "    <eng_info>\n"
                    "        <engine>ExGS</engine>\n"
                    "        <version>{}</version>\n"
                    "        <repo>{}</repo>\n"
                    "        <return_code>0</return_code>\n"
                    "        <timestamp>{}</timestamp>\n"
                    "        <time_elapsed_s>{}</time_elapsed_s>\n"
                    "    </eng_info>\n"
                    "    <sim_params>\n"
                    "        <debye_length>{}</debye_length>\n"
                    "        <eps_r>{}</eps_r>\n"
                    "        <muzm>{}</muzm>\n"
                    "    </sim_params>\n"
                    "    <physloc>\n"
                    "        <dbdot x=\"0.000000\" y=\"0.000000\"/>\n"
                    "        <dbdot x=\"19.200000\" y=\"0.000000\"/>\n"
                    "        <dbdot x=\"26.880000\" y=\"0.000000\"/>\n"
                    "        <dbdot x=\"42.240000\" y=\"0.000000\"/>\n"
                    "        <dbdot x=\"49.920000\" y=\"0.000000\"/>\n"
                    "        <dbdot x=\"65.280000\" y=\"0.000000\"/>\n"
                    "        <dbdot x=\"72.960000\" y=\"0.000000\"/>\n"
                    "    </physloc>\n"
                    "    <elec_dist>\n"
                    "        <dist energy=\"0.246049\" count=\"1\" physically_valid=\"1\" "
                    "state_count=\"3\">-0-0-0-</dist>\n"
                    "    </elec_dist>\n"
                    "</sim_out>\n",
                    FICTION_VERSION, FICTION_REPO, timestamp, sim_result.simulation_runtime.count(),
                    sim_result.sim_params.lambda_tf, sim_result.sim_params.epsilon_r, sim_result.sim_params.mu_minus);

    CHECK(simulation_stream.str() == sim_result_str);
}

TEST_CASE("Write simulation result with ExGS simulation and positive DBs", "[sqd-sim-result]")
{
    using namespace std::chrono_literals;

    layout lyt{};

    lyt.assign_sidb({5, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({6, 0, 0}, dot_tag::NORMAL);
    lyt.assign_sidb({7, 0, 0}, dot_tag::NORMAL);

    const simulation_parameters params{3, -0.32};

    auto sim_result = exhaustive_ground_state_simulation(lyt, params);

    sim_result.algorithm_name = "ExGS";
    std::stringstream simulation_stream{};

    const auto        timestamp      = write_timestamped_result(sim_result, simulation_stream);
    const std::string sim_result_str = fmt::format(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<sim_out>\n"
        "    <eng_info>\n"
        "        <engine>ExGS</engine>\n"
        "        <version>{}</version>\n"
        "        <repo>{}</repo>\n"
        "        <return_code>0</return_code>\n"
        "        <timestamp>{}</timestamp>\n"
        "        <time_elapsed_s>{}</time_elapsed_s>\n"
        "    </eng_info>\n"
        "    <sim_params>\n"
        "        <debye_length>{}</debye_length>\n"
        "        <eps_r>{}</eps_r>\n"
        "        <muzm>{}</muzm>\n"
        "    </sim_params>\n"
        "    <physloc>\n"
        "        <dbdot x=\"19.200000\" y=\"0.000000\"/>\n"
        "        <dbdot x=\"23.040000\" y=\"0.000000\"/>\n"
        "        <dbdot x=\"26.880000\" y=\"0.000000\"/>\n"
        "    </physloc>\n"
        "    <elec_dist>\n"
        "        <dist energy=\"-0.953108\" count=\"1\" physically_valid=\"1\" state_count=\"3\">-+-</dist>\n"
        "        <dist energy=\"0.000000\" count=\"1\" physically_valid=\"1\" state_count=\"3\">0-0</dist>\n"
        "    </elec_dist>\n"
        "</sim_out>\n",
        FICTION_VERSION, FICTION_REPO, timestamp, sim_result.simulation_runtime.count(),
        sim_result.sim_params.lambda_tf, sim_result.sim_params.epsilon_r, sim_result.sim_params.mu_minus);

    CHECK(simulation_stream.str() == sim_result_str);
}

TEST_CASE("SQD simulation orders mixed finite and non-finite energies", "[sqd-sim-result]")
{
    /** @brief Result with two sites that identify each distribution by its charge states. */
    result res{};
    res.lyt.assign_sidb({0, 0, 0}, dot_tag::NORMAL);
    res.lyt.assign_sidb({2, 0, 0}, dot_tag::NORMAL);
    /** @brief Shared sites for the five distinct configurations. */
    const charge_distribution prototype{res.lyt};
    /** @brief Positive NaN, ordered after positive infinity. */
    const auto nan = std::copysign(std::numeric_limits<double>::quiet_NaN(), 1.0);
    /** @brief Infinite energy bound. */
    const auto infinity      = std::numeric_limits<double>::infinity();
    res.charge_distributions = {{prototype.shared_sites(), {charge_state::NEGATIVE, charge_state::NEGATIVE}, nan},
                                {prototype.shared_sites(), {charge_state::NEGATIVE, charge_state::NEUTRAL}, 2.0},
                                {prototype.shared_sites(), {charge_state::NEUTRAL, charge_state::NEUTRAL}, -infinity},
                                {prototype.shared_sites(), {charge_state::NEUTRAL, charge_state::POSITIVE}, infinity},
                                {prototype.shared_sites(), {charge_state::POSITIVE, charge_state::POSITIVE}, -3.0}};
    /** @brief Serialized result. */
    std::stringstream stream{};
    write_sqd_sim_result(res, stream);
    /** @brief Parsed output, independent of its formatting. */
    tinyxml2::XMLDocument document{};
    REQUIRE(document.Parse(stream.str().c_str()) == tinyxml2::XML_SUCCESS);
    /** @brief Simulation output root. */
    const auto* root = document.FirstChildElement("sim_out");
    REQUIRE(root != nullptr);
    /** @brief Complete distribution block. */
    const auto* distributions = root->FirstChildElement("elec_dist");
    REQUIRE(distributions != nullptr);
    /** @brief Hand-derived configuration order and retained energies. */
    const std::vector<std::pair<std::string, double>> expected{{"00", -infinity},
                                                               {"++", -3.0},
                                                               {"-0", 2.0},
                                                               {"0+", infinity},
                                                               {"--", nan}};
    /** @brief Next emitted distribution. */
    auto* distribution = distributions->FirstChildElement("dist");
    for (const auto& [configuration, energy] : expected)
    {
        CAPTURE(configuration);
        REQUIRE(distribution != nullptr);
        REQUIRE(distribution->GetText() != nullptr);
        CHECK(std::string{distribution->GetText()} == configuration);
        /** @brief Parsed numeric energy without depending on a NaN or infinity spelling. */
        double actual{};
        REQUIRE(distribution->QueryDoubleAttribute("energy", &actual) == tinyxml2::XML_SUCCESS);
        if (std::isnan(energy))
        {
            CHECK(std::isnan(actual));
        }
        else
        {
            CHECK(actual == energy);
        }
        CHECK(distribution->IntAttribute("count") == 1);
        CHECK(distribution->IntAttribute("physically_valid") == 1);
        CHECK(distribution->IntAttribute("state_count") == 3);
        distribution = distribution->NextSiblingElement("dist");
    }
    CHECK(distribution == nullptr);
}

TEST_CASE("test_write_sqd_sim_result rejects mismatched distribution sites", "[sqd-sim-result]")
{
    layout lyt{};
    lyt.assign_sidb({0, 0}, dot_tag::NORMAL);
    layout shifted{};
    shifted.assign_sidb({1, 0}, dot_tag::NORMAL);
    const charge_distribution cd{shifted};
    result                    res{};
    res.lyt                  = lyt;
    res.charge_distributions = {cd};
    std::stringstream os{};
    CHECK_THROWS_AS(write_sqd_sim_result(res, os), std::invalid_argument);
    CHECK(os.str().empty());
}

TEST_CASE("SQD simulation metadata preserves XML text", "[sqd-sim-result]")
{
    result res{};
    res.algorithm_name = "a & <simulation> \"quoted\" 'name'\t\n\rend \xc3\xa9 \xe4\xb8\xad \xf0\x90\x80\x80";
    const std::string text{
        "a & <value> \"quoted\" 'text'\t\n\rend \xed\x9f\xbf \xee\x80\x80 \xef\xbf\xbd \xf4\x8f\xbf\xbf"};
    res.additional_simulation_parameters.emplace("string_parameter", text);
    res.additional_simulation_parameters.emplace("cstring_parameter", text.c_str());
    res.additional_simulation_parameters.emplace("char_parameter", '<');
    res.additional_simulation_parameters.emplace("unused invalid name", std::any{});
    std::stringstream os{};
    write_sqd_sim_result(res, os);

    tinyxml2::XMLDocument document{};
    REQUIRE(document.Parse(os.str().c_str()) == tinyxml2::XML_SUCCESS);
    const auto* root = document.FirstChildElement("sim_out");
    REQUIRE(root != nullptr);
    const auto* engine_info = root->FirstChildElement("eng_info");
    REQUIRE(engine_info != nullptr);
    const auto* engine = engine_info->FirstChildElement("engine");
    REQUIRE(engine != nullptr);
    REQUIRE(engine->GetText() != nullptr);
    CHECK(std::string{engine->GetText()} == res.algorithm_name);
    const auto* parameters = root->FirstChildElement("sim_params");
    REQUIRE(parameters != nullptr);
    for (const auto* name : {"string_parameter", "cstring_parameter", "char_parameter"})
    {
        CAPTURE(name);
        const auto* parameter = parameters->FirstChildElement(name);
        REQUIRE(parameter != nullptr);
        REQUIRE(parameter->GetText() != nullptr);
        CHECK(std::string{parameter->GetText()} == (std::string{name} == "char_parameter" ? "<" : text));
    }
}

TEST_CASE("SQD simulation metadata rejects invalid XML names before output", "[sqd-sim-result]")
{
    for (const std::string name : {"", "1bad", "bad name", "bad<name", "bad&name", "\xc2\xb7name", "\xcc\x80name",
                                   "\xcd\xbe", "\xe3\x80\x80", "\xef\xb7\x90", "\xf3\xb0\x80\x80"})
    {
        CAPTURE(name);
        result res{};
        res.additional_simulation_parameters.emplace(name, "value");
        std::stringstream os{};
        os << "original";
        CHECK_THROWS_AS(write_sqd_sim_result(res, os), std::invalid_argument);
        CHECK(os.str() == "original");
    }
}

TEST_CASE("SQD simulation metadata accepts XML Unicode names", "[sqd-sim-result]")
{
    for (const std::string name : {"\xc3\xa9", "\xe4\xb8\xad", "\xf0\x90\x80\x80", "\xf3\xaf\xbf\xbf",
                                   "a\xc2\xb7\xcc\x80\xe2\x80\xbf", "_name-1.value"})
    {
        CAPTURE(name);
        result res{};
        res.additional_simulation_parameters.emplace(name, "value");
        std::stringstream os{};
        write_sqd_sim_result(res, os);
        tinyxml2::XMLDocument document{};
        REQUIRE(document.Parse(os.str().c_str()) == tinyxml2::XML_SUCCESS);
        const auto* root = document.FirstChildElement("sim_out");
        REQUIRE(root != nullptr);
        const auto* parameters = root->FirstChildElement("sim_params");
        REQUIRE(parameters != nullptr);
        const auto* parameter = parameters->FirstChildElement(name.c_str());
        REQUIRE(parameter != nullptr);
        REQUIRE(parameter->GetText() != nullptr);
        CHECK(std::string{parameter->GetText()} == "value");
    }
}

TEST_CASE("SQD simulation metadata rejects invalid Unicode before output", "[sqd-sim-result]")
{
    const auto [label, invalid] = GENERATE(std::pair{"lone continuation", std::string{"\x80"}},
                                           std::pair{"invalid leading byte", std::string{"\xff"}},
                                           std::pair{"truncated two-byte sequence", std::string{"\xc2"}},
                                           std::pair{"invalid continuation", std::string{"\xc2 "}},
                                           std::pair{"overlong two-byte sequence", std::string{"\xc0\xaf"}},
                                           std::pair{"truncated three-byte sequence", std::string{"\xe0\xa0"}},
                                           std::pair{"overlong three-byte sequence", std::string{"\xe0\x80\xaf"}},
                                           std::pair{"surrogate", std::string{"\xed\xa0\x80"}},
                                           std::pair{"XML noncharacter FFFE", std::string{"\xef\xbf\xbe"}},
                                           std::pair{"XML noncharacter FFFF", std::string{"\xef\xbf\xbf"}},
                                           std::pair{"truncated four-byte sequence", std::string{"\xf0\x90\x80"}},
                                           std::pair{"overlong four-byte sequence", std::string{"\xf0\x80\x80\xaf"}},
                                           std::pair{"above Unicode range", std::string{"\xf4\x90\x80\x80"}});
    CAPTURE(label);
    result res{};
    /** @brief Check rejection without modifying the destination stream. */
    const auto check_rejected = [&res]
    {
        std::stringstream os{};
        os << "original";
        CHECK_THROWS_AS(write_sqd_sim_result(res, os), std::invalid_argument);
        CHECK(os.str() == "original");
    };
    SECTION("engine name")
    {
        res.algorithm_name = invalid;
        check_rejected();
    }
    SECTION("parameter value")
    {
        res.additional_simulation_parameters.emplace("parameter", invalid);
        check_rejected();
    }
    SECTION("parameter name")
    {
        res.additional_simulation_parameters.emplace("parameter" + invalid, "value");
        check_rejected();
    }
}

TEST_CASE("SQD simulation metadata rejects null strings before output", "[sqd-sim-result]")
{
    result res{};
    res.additional_simulation_parameters.emplace("parameter", static_cast<const char*>(nullptr));
    std::stringstream os{};
    os << "original";
    CHECK_THROWS_AS(write_sqd_sim_result(res, os), std::invalid_argument);
    CHECK(os.str() == "original");
}

TEST_CASE("SQD simulation metadata rejects illegal XML controls before output", "[sqd-sim-result]")
{
    const auto control = GENERATE('\0', '\x01', '\x1f');
    CAPTURE(static_cast<unsigned int>(control));
    result            res{};
    const std::string text(1, control);
    /** @brief Check rejection without modifying the destination stream. */
    const auto check_rejected = [&res]
    {
        std::stringstream os{};
        os << "original";
        CHECK_THROWS_AS(write_sqd_sim_result(res, os), std::invalid_argument);
        CHECK(os.str() == "original");
    };
    SECTION("engine name")
    {
        res.algorithm_name = text;
        check_rejected();
    }
    SECTION("string parameter")
    {
        res.additional_simulation_parameters.emplace("parameter", text);
        check_rejected();
    }
    SECTION("character parameter")
    {
        res.additional_simulation_parameters.emplace("parameter", control);
        check_rejected();
    }
    SECTION("parameter name")
    {
        res.additional_simulation_parameters.emplace("parameter" + text, "value");
        check_rejected();
    }
}
