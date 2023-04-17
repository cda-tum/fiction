//
// Created by marcel on 24.06.22.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp>
#include <fiction/io/write_sqd_sim_result.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>

#include <chrono>
#include <sstream>
#include <string>

using namespace fiction;

TEST_CASE("Utility function: any_to_string", "[sqd-sim-result]")
{
    SECTION("Empty std::any")
    {
        const std::any value{};
        const auto     result = detail::any_to_string(value);

        REQUIRE(result.empty());
    }
    SECTION("int8_t")
    {
        const int8_t value  = -42;
        const auto   result = detail::any_to_string(value);

        REQUIRE(result == "-42");
    }
    SECTION("uint16_t")
    {
        const uint16_t value  = 65535;
        const auto     result = detail::any_to_string(value);

        CHECK(result == "65535");
    }
    SECTION("double")
    {
        const double value  = 3.141593;
        const auto   result = detail::any_to_string(value);

        CHECK(result == "3.141593");
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

    using sidb_layout = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>;

    sidb_simulation_result<sidb_layout> sim_result{};

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
            "        <time_elapsed_s>42.0</time_elapsed_s>\n"
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
            sim_result.physical_parameters.lambda_tf, sim_result.physical_parameters.epsilon_r,
            sim_result.physical_parameters.mu);

        write_sqd_sim_result(sim_result, simulation_stream);

        CHECK(simulation_stream.str() == sim_result_str);
    }
    SECTION("with additional parameters")
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
            "        <time_elapsed_s>42.0</time_elapsed_s>\n"
            "    </eng_info>\n"
            "    <sim_params>\n"
            "        <debye_length>{}</debye_length>\n"
            "        <eps_r>{}</eps_r>\n"
            "        <muzm>{}</muzm>\n"
            "        <param1>value1</param1>\n"
            "        <param2>2</param2>\n"
            "        <param3>3.140000</param3>\n"
            "        <param4>c</param4>\n"
            "    </sim_params>\n"
            "    <physloc>\n"
            "    </physloc>\n"
            "    <elec_dist>\n"
            "    </elec_dist>\n"
            "</sim_out>\n",
            FICTION_VERSION, FICTION_REPO, fmt::format("{:%Y-%m-%d %H:%M:%S}", fmt::localtime(std::time(nullptr))),
            sim_result.physical_parameters.lambda_tf, sim_result.physical_parameters.epsilon_r,
            sim_result.physical_parameters.mu);

        sim_result.additional_simulation_parameters.emplace_back("param1", "value1");
        sim_result.additional_simulation_parameters.emplace_back("param2", 2);
        sim_result.additional_simulation_parameters.emplace_back("param3", 3.14);
        sim_result.additional_simulation_parameters.emplace_back("param4", 'c');

        write_sqd_sim_result(sim_result, simulation_stream);

        CHECK(simulation_stream.str() == sim_result_str);
    }
}

TEST_CASE("Write simulation result with ExGS simulation", "[sqd-sim-result]")
{
    using namespace std::chrono_literals;

    using sidb_layout = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>;

    sidb_layout lyt{{20, 10}};

    lyt.assign_cell_type({0, 0, 0}, sidb_layout::cell_type::NORMAL);
    lyt.assign_cell_type({5, 0, 0}, sidb_layout::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, sidb_layout::cell_type::NORMAL);

    lyt.assign_cell_type({11, 0, 0}, sidb_layout::cell_type::NORMAL);
    lyt.assign_cell_type({13, 0, 0}, sidb_layout::cell_type::NORMAL);

    lyt.assign_cell_type({17, 0, 0}, sidb_layout::cell_type::NORMAL);
    lyt.assign_cell_type({19, 0, 0}, sidb_layout::cell_type::NORMAL);

    exgs_stats<sidb_layout>          exgs_stats{};
    const sidb_simulation_parameters params{2, -0.32};

    exhaustive_ground_state_simulation<sidb_layout>(lyt, params, &exgs_stats);

    sidb_simulation_result<sidb_layout> sim_result{};

    sim_result.algorithm_name       = "ExGS";
    sim_result.simulation_runtime   = exgs_stats.time_total;
    sim_result.charge_distributions = exgs_stats.valid_lyts;
    sim_result.physical_parameters  = params;

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
        "        <dbdot x=\"0.0\" y=\"0.0\"/>\n"
        "        <dbdot x=\"4.224e-09\" y=\"0.0\"/>\n"
        "        <dbdot x=\"6.528000000000001e-09\" y=\"0.0\"/>\n"
        "        <dbdot x=\"1.92e-09\" y=\"0.0\"/>\n"
        "        <dbdot x=\"7.2960000000000004e-09\" y=\"0.0\"/>\n"
        "        <dbdot x=\"4.992e-09\" y=\"0.0\"/>\n"
        "        <dbdot x=\"2.6880000000000002e-09\" y=\"0.0\"/>\n"
        "    </physloc>\n"
        "    <elec_dist>\n"
        "        <dist energy=\"0.24602741408534134\" count=\"1\" physically_valid=\"1\" "
        "state_count=\"2\">-000---</dist>\n"
        "    </elec_dist>\n"
        "</sim_out>\n",
        FICTION_VERSION, FICTION_REPO, fmt::format("{:%Y-%m-%d %H:%M:%S}", fmt::localtime(std::time(nullptr))),
        sim_result.simulation_runtime.count(), sim_result.physical_parameters.lambda_tf,
        sim_result.physical_parameters.epsilon_r, sim_result.physical_parameters.mu);

    write_sqd_sim_result(sim_result, simulation_stream);

    CHECK(simulation_stream.str() == sim_result_str);
}

TEST_CASE("Write simulation result with ExGS simulation and positive DBs", "[sqd-sim-result]")
{
    using namespace std::chrono_literals;

    using sidb_layout = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>;

    sidb_layout lyt{{20, 10}};

    lyt.assign_cell_type({5, 0, 0}, sidb_layout::cell_type::NORMAL);
    lyt.assign_cell_type({6, 0, 0}, sidb_layout::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, sidb_layout::cell_type::NORMAL);

    exgs_stats<sidb_layout>          exgs_stats{};
    const sidb_simulation_parameters params{3, -0.32};

    exhaustive_ground_state_simulation<sidb_layout>(lyt, params, &exgs_stats);

    sidb_simulation_result<sidb_layout> sim_result{};

    sim_result.algorithm_name       = "ExGS";
    sim_result.simulation_runtime   = exgs_stats.time_total;
    sim_result.charge_distributions = exgs_stats.valid_lyts;
    sim_result.physical_parameters  = params;

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
        "        <dbdot x=\"2.3040000000000003e-09\" y=\"0.0\"/>\n"
        "        <dbdot x=\"1.92e-09\" y=\"0.0\"/>\n"
        "        <dbdot x=\"2.6880000000000002e-09\" y=\"0.0\"/>\n"
        "    </physloc>\n"
        "    <elec_dist>\n"
        "        <dist energy=\"0.0\" count=\"1\" physically_valid=\"1\" state_count=\"3\">-00</dist>\n"
        "        <dist energy=\"-0.9530228790595917\" count=\"1\" physically_valid=\"1\" state_count=\"3\">+--</dist>\n"
        "    </elec_dist>\n"
        "</sim_out>\n",
        FICTION_VERSION, FICTION_REPO, fmt::format("{:%Y-%m-%d %H:%M:%S}", fmt::localtime(std::time(nullptr))),
        sim_result.simulation_runtime.count(), sim_result.physical_parameters.lambda_tf,
        sim_result.physical_parameters.epsilon_r, sim_result.physical_parameters.mu);

    write_sqd_sim_result(sim_result, simulation_stream);

    CHECK(simulation_stream.str() == sim_result_str);
}
