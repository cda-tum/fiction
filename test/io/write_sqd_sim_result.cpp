//
// Created by marcel on 24.06.22.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp>
#include <fiction/io/write_sqd_sim_result.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/sidb_lattice.hpp>
#include <fiction/types.hpp>

#include <any>
#include <chrono>
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

using namespace fiction;

TEST_CASE("Utility function: any_to_string", "[sqd-sim-result]")
{
    SECTION("Empty std::any")
    {
        const std::any value{};
        const auto     result = detail::any_to_string(value);

        CHECK(result.empty());
    }
    SECTION("int8_t")
    {
        const int8_t value  = -42;
        const auto   result = detail::any_to_string(value);

        CHECK(result == "-42");
    }
    SECTION("uint8_t")
    {
        const uint8_t value  = 42;
        const auto    result = detail::any_to_string(value);

        CHECK(result == "42");
    }
    SECTION("int16_t")
    {
        const int16_t value  = -32768;
        const auto    result = detail::any_to_string(value);

        CHECK(result == "-32768");
    }
    SECTION("uint16_t")
    {
        const uint16_t value  = 65535;
        const auto     result = detail::any_to_string(value);

        CHECK(result == "65535");
    }
    SECTION("int32_t")
    {
        const int32_t value  = -2'147'483'648;
        const auto    result = detail::any_to_string(value);

        CHECK(result == "-2147483648");
    }
    SECTION("uint32_t")
    {
        const uint32_t value  = 4'294'967'295ul;
        const auto     result = detail::any_to_string(value);

        CHECK(result == "4294967295");
    }
    SECTION("int64_t")
    {
        const int64_t value  = -9'223'372'036'854'775'807;
        const auto    result = detail::any_to_string(value);

        CHECK(result == "-9223372036854775807");
    }
    SECTION("uint64_t")
    {
        const uint64_t value  = 18'446'744'073'709'551'615ull;
        const auto     result = detail::any_to_string(value);

        CHECK(result == "18446744073709551615");
    }
    SECTION("float")
    {
        const float value  = -3.141593f;
        const auto  result = detail::any_to_string(value);

        CHECK(result == "-3.141593");
    }
    SECTION("double")
    {
        const double value  = 3.14159265359;
        const auto   result = detail::any_to_string(value);

        CHECK(result == "3.141593");  // will be rounded by std::to_string
    }
    SECTION("long double")
    {
        const long double value  = 2.7182818284590452353602874l;
        const auto        result = detail::any_to_string(value);

        CHECK(result == "2.718282");  // will be rounded by std::to_string
    }
    SECTION("std::string")
    {
        const std::string value  = "hello, world!";
        const auto        result = detail::any_to_string(value);

        CHECK(result == "hello, world!");
    }
    SECTION("const char*")
    {
        const char* value  = "hello, world!";
        const auto  result = detail::any_to_string(value);

        CHECK(result == "hello, world!");
    }
    SECTION("char")
    {
        const char value  = 'X';
        const auto result = detail::any_to_string(value);

        CHECK(result == "X");
    }
    SECTION("unsupported type")
    {
        const std::vector<int> value{1, 2, 3};
        const auto             result = detail::any_to_string(value);

        CHECK(result.empty());
    }
    SECTION("std::string in std::any")
    {
        const std::any value  = std::string{"hello, world!"};
        const auto     result = detail::any_to_string(value);

        CHECK(result == "hello, world!");
    }
    SECTION("const char* in std::any")
    {
        const std::any value  = "hello, world!";
        const auto     result = detail::any_to_string(value);

        CHECK(result == "hello, world!");
    }
}

TEST_CASE("Write empty simulation result", "[sqd-sim-result]")
{
    using namespace std::chrono_literals;

    using lattice = sidb_lattice<sidb_100_lattice, sidb_cell_clk_lyt_siqad>;

    sidb_simulation_result<lattice> sim_result{};

    sim_result.algorithm_name     = "TestSim";
    sim_result.simulation_runtime = 42s;

    std::stringstream simulation_stream{};

    SECTION("without additional parameters")
    {
        const std::string sim_result_str = fmt::format(
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
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
            FICTION_VERSION, FICTION_REPO, fmt::format("{:%Y-%m-%d %H:%M:%S}", fmt::localtime(std::time(nullptr))),
            sim_result.simulation_parameters.lambda_tf, sim_result.simulation_parameters.epsilon_r,
            sim_result.simulation_parameters.mu_minus);

        write_sqd_sim_result(sim_result, simulation_stream);

        CHECK(simulation_stream.str() == sim_result_str);
    }
    SECTION("with additional parameter (string)")
    {
        const std::string sim_result_str = fmt::format(
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
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
            FICTION_VERSION, FICTION_REPO, fmt::format("{:%Y-%m-%d %H:%M:%S}", fmt::localtime(std::time(nullptr))),
            sim_result.simulation_parameters.lambda_tf, sim_result.simulation_parameters.epsilon_r,
            sim_result.simulation_parameters.mu_minus);

        sim_result.additional_simulation_parameters.emplace("param1", "value1");

        write_sqd_sim_result(sim_result, simulation_stream);

        CHECK(simulation_stream.str() == sim_result_str);
    }

    SECTION("with additional parameters (double)")
    {
        const std::string sim_result_str = fmt::format(
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
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
            FICTION_VERSION, FICTION_REPO, fmt::format("{:%Y-%m-%d %H:%M:%S}", fmt::localtime(std::time(nullptr))),
            sim_result.simulation_parameters.lambda_tf, sim_result.simulation_parameters.epsilon_r,
            sim_result.simulation_parameters.mu_minus);

        sim_result.additional_simulation_parameters.emplace("param3", 3.14);

        write_sqd_sim_result(sim_result, simulation_stream);

        CHECK(simulation_stream.str() == sim_result_str);
    }
}

TEST_CASE("Write simulation result with ExGS simulation", "[sqd-sim-result]")
{
    using namespace std::chrono_literals;

    using sidb_layout = sidb_cell_clk_lyt;

    sidb_layout lyt{{20, 10}};

    lyt.assign_cell_type({0, 0, 0}, sidb_layout::cell_type::NORMAL);
    lyt.assign_cell_type({5, 0, 0}, sidb_layout::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, sidb_layout::cell_type::NORMAL);

    lyt.assign_cell_type({11, 0, 0}, sidb_layout::cell_type::NORMAL);
    lyt.assign_cell_type({13, 0, 0}, sidb_layout::cell_type::NORMAL);

    lyt.assign_cell_type({17, 0, 0}, sidb_layout::cell_type::NORMAL);
    lyt.assign_cell_type({19, 0, 0}, sidb_layout::cell_type::NORMAL);

    const sidb_simulation_parameters params{2, -0.32};

    const sidb_lattice<sidb_100_lattice, sidb_layout> lat{lyt};

    auto sim_result = exhaustive_ground_state_simulation(lat, params);

    sim_result.algorithm_name = "ExGS";

    std::stringstream simulation_stream{};

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
        FICTION_VERSION, FICTION_REPO, fmt::format("{:%Y-%m-%d %H:%M:%S}", fmt::localtime(std::time(nullptr))),
        sim_result.simulation_runtime.count(), sim_result.simulation_parameters.lambda_tf,
        sim_result.simulation_parameters.epsilon_r, sim_result.simulation_parameters.mu_minus);

    write_sqd_sim_result(sim_result, simulation_stream);

    CHECK(simulation_stream.str() == sim_result_str);
}

TEST_CASE("Write simulation result with ExGS simulation and positive DBs", "[sqd-sim-result]")
{
    using namespace std::chrono_literals;

    using sidb_layout = sidb_cell_clk_lyt_siqad;

    sidb_layout lyt{{20, 10}};

    lyt.assign_cell_type({5, 0, 0}, sidb_layout::cell_type::NORMAL);
    lyt.assign_cell_type({6, 0, 0}, sidb_layout::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, sidb_layout::cell_type::NORMAL);

    const sidb_simulation_parameters params{3, -0.32};

    const sidb_lattice<sidb_100_lattice, sidb_layout> lat{lyt};

    auto sim_result = exhaustive_ground_state_simulation(lat, params);

    sim_result.algorithm_name = "ExGS";
    std::stringstream simulation_stream{};

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
        FICTION_VERSION, FICTION_REPO, fmt::format("{:%Y-%m-%d %H:%M:%S}", fmt::localtime(std::time(nullptr))),
        sim_result.simulation_runtime.count(), sim_result.simulation_parameters.lambda_tf,
        sim_result.simulation_parameters.epsilon_r, sim_result.simulation_parameters.mu_minus);

    write_sqd_sim_result(sim_result, simulation_stream);

    CHECK(simulation_stream.str() == sim_result_str);
}
