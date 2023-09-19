//
// Created by Jan Drewniok on 07.02.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/simulation/sidb/critical_temperature.hpp>
#include <fiction/algorithms/simulation/sidb/energy_distribution.hpp>
#include <fiction/algorithms/simulation/sidb/quicksim.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/types.hpp>

#include <cmath>
#include <limits>

using namespace fiction;

TEMPLATE_TEST_CASE(
    "Test critical_temperature function", "[critical-temperature]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_column_hex>>>))
{

    SECTION("empty layout")
    {
        TestType lyt{{10, 10}};

        critical_temperature_stats<TestType> criticalstats{};
        const critical_temperature_params    params{critical_temperature_params::simulation_engine::EXACT,
                                                 quicksim_params{sidb_simulation_parameters{2, -0.32}}, 0.99, 350};
        critical_temperature_non_gate_based<TestType>(lyt, params, &criticalstats);
        CHECK(criticalstats.num_valid_lyt == 0);
        CHECK(criticalstats.critical_temperature == 350);
    }

    SECTION("Not working diagonal Wire where positively charged SiDBs can occur")
    {
        TestType lyt{};

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

        critical_temperature_stats<TestType> criticalstats{};
        const critical_temperature_params    params{critical_temperature_params::simulation_engine::EXACT,
                                                 quicksim_params{sidb_simulation_parameters{2, -0.32}}, 0.99, 350};
        critical_temperature_gate_based(lyt, std::vector<tt>{create_id_tt()}, params, &criticalstats);
        CHECK(criticalstats.critical_temperature == 0.0);
    }

    SECTION("four SiDBs with two valid charge distributions, QuickExact")
    {
        TestType lyt{{10, 10}};
        lyt.assign_cell_type({0, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({2, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({4, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({2, 2}, TestType::cell_type::NORMAL);

        critical_temperature_stats<TestType> criticalstats_non_gate_based{};
        const critical_temperature_params params_non_gate_based{critical_temperature_params::simulation_engine::EXACT,
                                                                quicksim_params{sidb_simulation_parameters{2, -0.32}},
                                                                0.99, 350};
        critical_temperature_non_gate_based(lyt, params_non_gate_based, &criticalstats_non_gate_based);
        CHECK(criticalstats_non_gate_based.num_valid_lyt == 2);
        CHECK_THAT(std::abs(criticalstats_non_gate_based.energy_between_ground_state_and_first_erroneous),
                   Catch::Matchers::WithinAbs(std::numeric_limits<double>::infinity(), 0.01));
        CHECK(criticalstats_non_gate_based.critical_temperature == 350);
    }

    SECTION("Y-shape SiDB AND gate")
    {
        TestType lyt{{20, 10}, "AND gate"};

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

        critical_temperature_stats<TestType> criticalstats{};
        const critical_temperature_params    params{critical_temperature_params::simulation_engine::EXACT,
                                                 quicksim_params{sidb_simulation_parameters{2, -0.28}}, 0.99, 350};
        critical_temperature_gate_based(lyt, std::vector<tt>{create_and_tt()}, params, &criticalstats);

        CHECK_THAT(std::abs(criticalstats.energy_between_ground_state_and_first_erroneous),
                   Catch::Matchers::WithinAbs(std::numeric_limits<double>::infinity(), 0.01));
        CHECK(criticalstats.critical_temperature == 350);
    }

    SECTION("Bestagon AND gate, QuickExact")
    {
        TestType lyt{};

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

        critical_temperature_stats<TestType> criticalstats{};
        const critical_temperature_params    params{critical_temperature_params::simulation_engine::EXACT,
                                                 quicksim_params{sidb_simulation_parameters{2, -0.32}}, 0.99, 350};
        critical_temperature_gate_based(lyt, std::vector<tt>{create_and_tt()}, params, &criticalstats);
        CHECK_THAT(std::abs(criticalstats.energy_between_ground_state_and_first_erroneous),
                   Catch::Matchers::WithinAbs(26.02, 0.01));
        CHECK_THAT(std::abs(criticalstats.critical_temperature - 59.19), Catch::Matchers::WithinAbs(0.00, 0.01));
    }

    SECTION("Bestagon AND gate, QuickSim")
    {
        TestType lyt{};

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

        critical_temperature_stats<TestType> criticalstats{};
        const critical_temperature_params    params{critical_temperature_params::simulation_engine::APPROXIMATE,
                                                 quicksim_params{sidb_simulation_parameters{2, -0.32}, 500, 0.6}, 0.99,
                                                 350};
        critical_temperature_gate_based(lyt, std::vector<tt>{create_and_tt()}, params, &criticalstats);
        CHECK(criticalstats.critical_temperature > 0);
    }

    SECTION("Bestagon fo2 gate")
    {
        TestType lyt{};

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

        critical_temperature_stats<TestType> criticalstats{};
        const critical_temperature_params    params{critical_temperature_params::simulation_engine::EXACT,
                                                 quicksim_params{sidb_simulation_parameters{2, -0.32}}, 0.99, 350};
        critical_temperature_gate_based(lyt, std::vector<tt>{create_fan_out_tt()}, params, &criticalstats);

        CHECK_THAT(std::abs(criticalstats.energy_between_ground_state_and_first_erroneous - 0.56),
                   Catch::Matchers::WithinAbs(0.00, 0.01));
        CHECK_THAT(std::abs(criticalstats.critical_temperature - 1.46), Catch::Matchers::WithinAbs(0.00, 0.01));
    }

    SECTION("Bestagon cx gate")
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

        critical_temperature_stats<TestType> criticalstats{};
        const critical_temperature_params    params{critical_temperature_params::simulation_engine::EXACT,
                                                 quicksim_params{sidb_simulation_parameters{2, -0.32}}, 0.99, 350};
        critical_temperature_gate_based(lyt, std::vector<tt>{create_crossing_wire_tt()}, params, &criticalstats);

        CHECK_THAT(std::fabs(criticalstats.energy_between_ground_state_and_first_erroneous - 0.32),
                   Catch::Matchers::WithinAbs(0.00, 0.01));
        CHECK_THAT(std::abs(criticalstats.critical_temperature - 0.85), Catch::Matchers::WithinAbs(0.00, 0.01));
    }

    SECTION("Not working diagonal Wire")
    {
        TestType lyt{};

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

        critical_temperature_stats<TestType> criticalstats{};
        const critical_temperature_params    params{critical_temperature_params::simulation_engine::EXACT,
                                                 quicksim_params{sidb_simulation_parameters{2, -0.32}}, 0.99, 350};
        critical_temperature_gate_based(lyt, std::vector<tt>{create_id_tt()}, params, &criticalstats);

        CHECK_THAT(std::abs(criticalstats.energy_between_ground_state_and_first_erroneous),
                   Catch::Matchers::WithinAbs(305.95, 0.01));
        CHECK_THAT(std::abs(criticalstats.critical_temperature), Catch::Matchers::WithinAbs(0.00, 0.01));
    }

    SECTION("nine SiDBs, QuickSim, non-gate-based")
    {
        TestType lyt{};

        lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({3, 0, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({6, 0, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({9, 0, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({12, 0, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({3, 1, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({6, 1, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({9, 1, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({12, 1, 1}, sidb_technology::cell_type::NORMAL);

        critical_temperature_stats<TestType> criticalstats{};
        const critical_temperature_params    params{critical_temperature_params::simulation_engine::APPROXIMATE,
                                                 quicksim_params{sidb_simulation_parameters{2, -0.32}, 500, 0.6}, 0.99,
                                                 750};
        critical_temperature_non_gate_based(lyt, params, &criticalstats);

        CHECK_THAT(std::abs(criticalstats.critical_temperature), Catch::Matchers::WithinAbs(11.55, 0.01));
    }
}
