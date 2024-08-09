//
// Created by Jan Drewniok on 07.02.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/simulation/sidb/critical_temperature.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/truth_table_utils.hpp>

#include <cmath>
#include <limits>
#include <vector>

using namespace fiction;

TEMPLATE_TEST_CASE("Test critical_temperature function", "[critical-temperature]", sidb_100_cell_clk_lyt_siqad,
                   cds_sidb_100_cell_clk_lyt_siqad)
{
    TestType lyt{};

    critical_temperature_params params{};
    sidb_simulation_parameters  sim_params{2, -0.32, 5.6, 5.0};

    critical_temperature_stats critical_stats{};

    SECTION("No physically valid charge distribution could be found")
    {
        lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type({6, 1, 0}, sidb_technology::cell_type::OUTPUT);
        lyt.assign_cell_type({8, 1, 0}, sidb_technology::cell_type::OUTPUT);

        params.simulation_parameters = sim_params;
        params.engine                = critical_temperature_params::simulation_engine::APPROXIMATE;
        params.confidence_level      = 0.99;
        params.max_temperature       = 350;
        params.iteration_steps       = 0;
        params.alpha                 = 0.0;

        critical_temperature_gate_based<TestType>(lyt, std::vector<tt>{create_id_tt()}, params, &critical_stats);

        CHECK(critical_stats.num_valid_lyt == 0);
        CHECK(critical_stats.critical_temperature == 0.0);
    }

    SECTION("No SiDB")
    {
        params.simulation_parameters = sim_params;
        params.engine                = critical_temperature_params::simulation_engine::EXACT;
        params.confidence_level      = 0.99;
        params.max_temperature       = 350;
        params.iteration_steps       = 80;
        params.alpha                 = 0.7;

        critical_temperature_gate_based<TestType>(lyt, std::vector<tt>{tt{}}, params, &critical_stats);

        CHECK(critical_stats.num_valid_lyt == 0);
        CHECK(critical_stats.critical_temperature == 0.0);
    }

    SECTION("Not working diagonal wire where positively charged SiDBs can occur")
    {
        lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::INPUT);

        lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({8, 3, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({12, 4, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({14, 5, 0}, sidb_technology::cell_type::NORMAL);

        // canvas SiDB
        lyt.assign_cell_type({14, 6, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({14, 6, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({15, 6, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({24, 15, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({26, 16, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({30, 17, 0}, sidb_technology::cell_type::OUTPUT);
        lyt.assign_cell_type({32, 18, 0}, sidb_technology::cell_type::OUTPUT);

        lyt.assign_cell_type({36, 19, 0}, sidb_technology::cell_type::NORMAL);

        params.simulation_parameters = sim_params;
        params.engine                = critical_temperature_params::simulation_engine::EXACT;
        params.confidence_level      = 0.99;
        params.max_temperature       = 350;
        params.iteration_steps       = 80;
        params.alpha                 = 0.7;

        critical_temperature_gate_based(lyt, std::vector<tt>{create_id_tt()}, params, &critical_stats);

        CHECK(critical_stats.critical_temperature == 0.0);
    }

    SECTION("four SiDBs with two valid charge distributions, QuickExact")
    {
        lyt.assign_cell_type({0, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({2, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({4, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({2, 2}, TestType::cell_type::NORMAL);

        params.simulation_parameters = sim_params;
        params.engine                = critical_temperature_params::simulation_engine::EXACT;
        params.confidence_level      = 0.99;
        params.max_temperature       = 350;
        params.iteration_steps       = 80;
        params.alpha                 = 0.7;

        critical_temperature_non_gate_based(lyt, params, &critical_stats);

        CHECK(critical_stats.num_valid_lyt == 2);
        CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous),
                   Catch::Matchers::WithinAbs(std::numeric_limits<double>::infinity(), 0.01));
        CHECK(critical_stats.critical_temperature == 350);
    }

    SECTION("Y-shaped SiDB AND gate")
    {
        lyt.assign_cell_type({0, 0, 1}, sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type({2, 1, 1}, sidb_technology::cell_type::INPUT);

        lyt.assign_cell_type({20, 0, 1}, sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type({18, 1, 1}, sidb_technology::cell_type::INPUT);

        lyt.assign_cell_type({4, 2, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({6, 3, 1}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({14, 3, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({16, 2, 1}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({10, 6, 0}, sidb_technology::cell_type::OUTPUT);
        lyt.assign_cell_type({10, 7, 0}, sidb_technology::cell_type::OUTPUT);

        lyt.assign_cell_type({10, 9, 1}, sidb_technology::cell_type::NORMAL);

        sim_params.mu_minus = -0.28;

        params.simulation_parameters = sim_params;
        params.engine                = critical_temperature_params::simulation_engine::EXACT;
        params.confidence_level      = 0.99;
        params.max_temperature       = 350;
        params.iteration_steps       = 80;
        params.alpha                 = 0.7;

        critical_temperature_gate_based(lyt, std::vector<tt>{create_and_tt()}, params, &critical_stats);

        CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous),
                   Catch::Matchers::WithinAbs(std::numeric_limits<double>::infinity(), 0.01));
        CHECK(critical_stats.critical_temperature == 350);
    }

    SECTION("Bestagon AND gate, QuickExact")
    {
        lyt.assign_cell_type({36, 1, 0}, sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::INPUT);

        lyt.assign_cell_type({38, 0, 0}, sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);

        lyt.assign_cell_type({23, 9, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({18, 11, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({18, 9, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({19, 8, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({20, 14, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({19, 13, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({26, 16, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({24, 15, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({32, 2, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({30, 3, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({26, 4, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({24, 5, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({12, 4, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({14, 5, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({8, 3, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({32, 18, 0}, sidb_technology::cell_type::OUTPUT);
        lyt.assign_cell_type({30, 17, 0}, sidb_technology::cell_type::OUTPUT);

        lyt.assign_cell_type({36, 19, 0}, sidb_technology::cell_type::NORMAL);

        params.simulation_parameters = sim_params;
        params.engine                = critical_temperature_params::simulation_engine::EXACT;
        params.confidence_level      = 0.99;
        params.max_temperature       = 350;
        params.iteration_steps       = 80;
        params.alpha                 = 0.7;

        critical_temperature_gate_based(lyt, std::vector<tt>{create_and_tt()}, params, &critical_stats);

        CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous),
                   Catch::Matchers::WithinAbs(26.02, 0.01));
        CHECK_THAT(std::abs(critical_stats.critical_temperature - 59.19), Catch::Matchers::WithinAbs(0.00, 0.01));
    }

    SECTION("Bestagon AND gate, QuickSim")
    {
        lyt.assign_cell_type({36, 1, 0}, sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::INPUT);

        lyt.assign_cell_type({38, 0, 0}, sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);

        lyt.assign_cell_type({23, 9, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({18, 11, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({18, 9, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({19, 8, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({20, 14, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({19, 13, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({26, 16, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({24, 15, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({32, 2, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({30, 3, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({26, 4, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({24, 5, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({12, 4, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({14, 5, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({8, 3, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({32, 18, 0}, sidb_technology::cell_type::OUTPUT);
        lyt.assign_cell_type({30, 17, 0}, sidb_technology::cell_type::OUTPUT);

        lyt.assign_cell_type({36, 19, 0}, sidb_technology::cell_type::NORMAL);

        params.simulation_parameters = sim_params;
        params.engine                = critical_temperature_params::simulation_engine::APPROXIMATE;
        params.confidence_level      = 0.99;
        params.max_temperature       = 350;
        params.iteration_steps       = 500;
        params.alpha                 = 0.6;

        critical_temperature_gate_based(lyt, std::vector<tt>{create_and_tt()}, params, &critical_stats);

        CHECK(critical_stats.critical_temperature > 0);
    }

    SECTION("Bestagon FO2 gate")
    {
        lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::INPUT);

        lyt.assign_cell_type({21, 11, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({17, 11, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({12, 4, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({18, 13, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({8, 3, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({19, 7, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({14, 5, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({18, 6, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({24, 15, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({26, 16, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({12, 16, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({14, 15, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({8, 17, 0}, sidb_technology::cell_type::OUTPUT);
        lyt.assign_cell_type({6, 18, 0}, sidb_technology::cell_type::OUTPUT);

        lyt.assign_cell_type({30, 17, 0}, sidb_technology::cell_type::OUTPUT);
        lyt.assign_cell_type({32, 18, 0}, sidb_technology::cell_type::OUTPUT);

        lyt.assign_cell_type({36, 19, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({2, 19, 0}, sidb_technology::cell_type::NORMAL);

        params.simulation_parameters = sim_params;
        params.engine                = critical_temperature_params::simulation_engine::EXACT;
        params.confidence_level      = 0.99;
        params.max_temperature       = 350;
        params.iteration_steps       = 80;
        params.alpha                 = 0.7;

        critical_temperature_gate_based(lyt, std::vector<tt>{create_fan_out_tt()}, params, &critical_stats);

        CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.56),
                   Catch::Matchers::WithinAbs(0.00, 0.01));
        CHECK_THAT(std::abs(critical_stats.critical_temperature - 1.46), Catch::Matchers::WithinAbs(0.00, 0.01));
    }

    SECTION("OR gate")
    {
        lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type({26, 0, 0}, sidb_technology::cell_type::INPUT);

        lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type({24, 1, 0}, sidb_technology::cell_type::INPUT);

        lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({20, 2, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({8, 3, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({18, 3, 0}, sidb_technology::cell_type::NORMAL);

        // three canvas SiDBs
        lyt.assign_cell_type({12, 6, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({12, 7, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({15, 11, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({18, 13, 0}, sidb_technology::cell_type::OUTPUT);
        lyt.assign_cell_type({20, 14, 0}, sidb_technology::cell_type::OUTPUT);

        lyt.assign_cell_type({24, 15, 0}, sidb_technology::cell_type::NORMAL);

        sim_params.mu_minus = -0.25;

        params.simulation_parameters = sim_params;
        params.engine                = critical_temperature_params::simulation_engine::EXACT;
        params.confidence_level      = 0.99;
        params.max_temperature       = 350;
        params.iteration_steps       = 80;
        params.alpha                 = 0.7;

        critical_temperature_gate_based(lyt, std::vector<tt>{create_or_tt()}, params, &critical_stats);

        CHECK(critical_stats.critical_temperature < 350);
    }

    SECTION("Not working diagonal Wire")
    {
        lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::INPUT);

        lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({8, 3, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({12, 4, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({14, 5, 0}, sidb_technology::cell_type::NORMAL);

        // canvas SiDB
        lyt.assign_cell_type({14, 6, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({24, 15, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({26, 16, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({30, 17, 0}, sidb_technology::cell_type::OUTPUT);
        lyt.assign_cell_type({32, 18, 0}, sidb_technology::cell_type::OUTPUT);

        lyt.assign_cell_type({36, 19, 0}, sidb_technology::cell_type::NORMAL);

        params.simulation_parameters = sim_params;
        params.engine                = critical_temperature_params::simulation_engine::EXACT;
        params.confidence_level      = 0.99;
        params.max_temperature       = 350;
        params.iteration_steps       = 80;
        params.alpha                 = 0.7;

        critical_temperature_gate_based(lyt, std::vector<tt>{create_id_tt()}, params, &critical_stats);

        CHECK(critical_stats.algorithm_name == "QuickExact");

        CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous),
                   Catch::Matchers::WithinAbs(305.95, 0.01));
        CHECK_THAT(std::abs(critical_stats.critical_temperature), Catch::Matchers::WithinAbs(0.00, 0.01));
    }

    SECTION("nine SiDBs, QuickSim, non-gate-based")
    {
        lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({3, 0, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({6, 0, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({9, 0, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({12, 0, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({3, 1, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({6, 1, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({9, 1, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({12, 1, 1}, sidb_technology::cell_type::NORMAL);

        params.simulation_parameters = sim_params;
        params.engine                = critical_temperature_params::simulation_engine::APPROXIMATE;
        params.confidence_level      = 0.99;
        params.max_temperature       = 750;
        params.iteration_steps       = 500;
        params.alpha                 = 0.6;

        critical_temperature_non_gate_based(lyt, params, &critical_stats);

        CHECK(critical_stats.algorithm_name == "QuickSim");

        CHECK_THAT(std::abs(critical_stats.critical_temperature), Catch::Matchers::WithinAbs(11.55, 0.01));
    }
}

TEMPLATE_TEST_CASE("Test critical_temperature function, using offset coordinates", "[critical-temperature]",
                   sidb_100_cell_clk_lyt, cds_sidb_100_cell_clk_lyt)
{
    TestType lyt{};

    critical_temperature_params params{};
    sidb_simulation_parameters  sim_params{2, -0.32, 5.6, 5.0};

    critical_temperature_stats critical_stats{};

    SECTION("No physically valid charge distribution could be found")
    {
        lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type({2, 2}, sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type({6, 2}, sidb_technology::cell_type::OUTPUT);
        lyt.assign_cell_type({8, 2}, sidb_technology::cell_type::OUTPUT);

        params.simulation_parameters = sim_params;
        params.engine                = critical_temperature_params::simulation_engine::APPROXIMATE;
        params.confidence_level      = 0.99;
        params.max_temperature       = 350;
        params.iteration_steps       = 0;
        params.alpha                 = 0.0;

        critical_temperature_gate_based<TestType>(lyt, std::vector<tt>{create_id_tt()}, params, &critical_stats);

        CHECK(critical_stats.algorithm_name == "QuickSim");

        CHECK(critical_stats.num_valid_lyt == 0);
        CHECK(critical_stats.critical_temperature == 0.0);
    }

    SECTION("One SiDB")
    {
        params.simulation_parameters = sim_params;
        params.engine                = critical_temperature_params::simulation_engine::EXACT;
        params.confidence_level      = 0.99;
        params.max_temperature       = 350;
        params.iteration_steps       = 80;
        params.alpha                 = 0.7;

        critical_temperature_gate_based<TestType>(lyt, std::vector<tt>{tt{}}, params, &critical_stats);

        CHECK(critical_stats.algorithm_name == "QuickExact");

        CHECK(critical_stats.num_valid_lyt == 0);
        CHECK(critical_stats.critical_temperature == 0.0);
    }

    SECTION("Not working diagonal Wire where positively charged SiDBs can occur")
    {
        lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type({2, 2, 0}, sidb_technology::cell_type::INPUT);

        lyt.assign_cell_type({6, 4, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({8, 6, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({12, 8, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({14, 10, 0}, sidb_technology::cell_type::NORMAL);

        // canvas SiDB
        lyt.assign_cell_type({14, 12, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({14, 13, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({15, 12, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({24, 30, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({26, 32, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({30, 34, 0}, sidb_technology::cell_type::OUTPUT);
        lyt.assign_cell_type({32, 36, 0}, sidb_technology::cell_type::OUTPUT);

        lyt.assign_cell_type({36, 38, 0}, sidb_technology::cell_type::NORMAL);

        params.simulation_parameters = sim_params;
        params.engine                = critical_temperature_params::simulation_engine::EXACT;
        params.confidence_level      = 0.99;
        params.max_temperature       = 350;
        params.iteration_steps       = 80;
        params.alpha                 = 0.7;

        critical_temperature_gate_based(lyt, std::vector<tt>{create_id_tt()}, params, &critical_stats);
        CHECK(critical_stats.critical_temperature == 0.0);
    }

    SECTION("four SiDBs with two valid charge distributions, QuickExact")
    {
        lyt.assign_cell_type({0, 2}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({2, 2}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({4, 2}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({2, 4}, TestType::cell_type::NORMAL);

        params.simulation_parameters = sim_params;
        params.engine                = critical_temperature_params::simulation_engine::EXACT;
        params.confidence_level      = 0.99;
        params.max_temperature       = 350;
        params.iteration_steps       = 80;
        params.alpha                 = 0.7;

        critical_temperature_non_gate_based(lyt, params, &critical_stats);

        CHECK(critical_stats.algorithm_name == "QuickExact");

        CHECK(critical_stats.num_valid_lyt == 2);
        CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous),
                   Catch::Matchers::WithinAbs(std::numeric_limits<double>::infinity(), 0.01));
        CHECK(critical_stats.critical_temperature == 350);
    }

    SECTION("Y-shape SiDB AND gate")
    {
        lyt.assign_cell_type({0, 1}, sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type({2, 3}, sidb_technology::cell_type::INPUT);

        lyt.assign_cell_type({20, 1}, sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type({18, 3}, sidb_technology::cell_type::INPUT);

        lyt.assign_cell_type({4, 5}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({6, 7}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({14, 7}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({16, 5}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({10, 12}, sidb_technology::cell_type::OUTPUT);
        lyt.assign_cell_type({10, 14}, sidb_technology::cell_type::OUTPUT);

        lyt.assign_cell_type({10, 19}, sidb_technology::cell_type::NORMAL);

        sim_params.mu_minus          = -0.28;
        params.simulation_parameters = sim_params;
        params.engine                = critical_temperature_params::simulation_engine::EXACT;
        params.confidence_level      = 0.99;
        params.max_temperature       = 350;
        params.iteration_steps       = 80;
        params.alpha                 = 0.7;

        critical_temperature_gate_based(lyt, std::vector<tt>{create_and_tt()}, params, &critical_stats);

        CHECK(critical_stats.algorithm_name == "QuickExact");

        CHECK_THAT(std::abs(critical_stats.energy_between_ground_state_and_first_erroneous),
                   Catch::Matchers::WithinAbs(std::numeric_limits<double>::infinity(), 0.01));
        CHECK(critical_stats.critical_temperature == 350);
    }
}

TEMPLATE_TEST_CASE("Critical temperature of Bestagon CX, QuickSim", "[critical-temperature], [quality]",
                   sidb_100_cell_clk_lyt_siqad, cds_sidb_100_cell_clk_lyt_siqad)
{
    TestType lyt{};

    lyt.assign_cell_type({36, 1, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({38, 0, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({20, 12, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 11, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({12, 4, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 15, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({26, 4, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({14, 9, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({24, 15, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 16, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({18, 9, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({26, 16, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({24, 13, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({24, 5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({30, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({16, 13, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({32, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({20, 8, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({30, 17, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({6, 18, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({32, 18, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({8, 17, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({2, 19, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({36, 19, 0}, sidb_technology::cell_type::NORMAL);

    critical_temperature_params params{};
    sidb_simulation_parameters  sim_params{2, -0.32, 5.6, 5.0};

    critical_temperature_stats critical_stats{};

    params.simulation_parameters = sim_params;
    params.engine                = critical_temperature_params::simulation_engine::EXACT;
    params.confidence_level      = 0.99;
    params.max_temperature       = 350;
    params.iteration_steps       = 80;
    params.alpha                 = 0.7;

    critical_temperature_gate_based(lyt, std::vector<tt>{create_crossing_wire_tt()}, params, &critical_stats);

    CHECK_THAT(std::fabs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.32),
               Catch::Matchers::WithinAbs(0.00, 0.01));
    CHECK_THAT(std::abs(critical_stats.critical_temperature - 0.85), Catch::Matchers::WithinAbs(0.00, 0.01));
}
