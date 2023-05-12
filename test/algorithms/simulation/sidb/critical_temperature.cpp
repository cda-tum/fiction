//
// Created by Jan Drewniok on 07.02.23.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/critical_temperature.hpp>
#include <fiction/algorithms/simulation/sidb/energy_distribution.hpp>
#include <fiction/algorithms/simulation/sidb/quicksim.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE(
    "Test critical_temperature function", "[critical_temperature]",
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
        const critical_temperature_params    params{simulation_engine::EXACT,
                                                 critical_temperature_mode::GATE_BASED_SIMULATION,
                                                 quicksim_params{sidb_simulation_parameters{2, -0.32}}, 0.99, 350};
        critical_temperature<TestType>(lyt, params, &criticalstats);
        CHECK(criticalstats.num_valid_lyt == 0);
        CHECK(criticalstats.critical_temperature == 0);
    }

    SECTION("one SiDB placed")
    {
        TestType lyt{{10, 10}};
        lyt.assign_cell_type({0, 0}, TestType::cell_type::NORMAL);

        critical_temperature_stats<TestType> criticalstats{};
        const critical_temperature_params    params{simulation_engine::EXACT,
                                                 critical_temperature_mode::GATE_BASED_SIMULATION,
                                                 quicksim_params{sidb_simulation_parameters{2, -0.32}},
                                                 0.99,
                                                 350,
                                                 create_or_tt(),
                                                 2};
        critical_temperature(lyt, params, &criticalstats);
        CHECK(criticalstats.num_valid_lyt == 1);
        CHECK(criticalstats.critical_temperature == 350);

        critical_temperature_stats<TestType> criticalstats_new{};
        critical_temperature(lyt, params, &criticalstats_new);
        CHECK(criticalstats_new.num_valid_lyt == 1);
        CHECK(criticalstats_new.critical_temperature == 350);
    }

    SECTION("several SiDBs placed")
    {
        TestType lyt{{10, 10}};
        lyt.assign_cell_type({1, 0, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({6, 0, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({0, 1, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 1, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({8, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({20, 3, 0}, TestType::cell_type::NORMAL);

        critical_temperature_stats<TestType> criticalstats{};
        const critical_temperature_params    params{simulation_engine::EXACT,
                                                 critical_temperature_mode::GATE_BASED_SIMULATION,
                                                 quicksim_params{sidb_simulation_parameters{2, -0.23}},
                                                 0.99,
                                                 350,
                                                 create_or_tt(),
                                                 2};
        critical_temperature(lyt, params, &criticalstats);
        CHECK(criticalstats.critical_temperature == 350);

        critical_temperature_stats<TestType> criticalstats_one{};
        const critical_temperature_params    params_one{simulation_engine::EXACT,
                                                     critical_temperature_mode::GATE_BASED_SIMULATION,
                                                     quicksim_params{sidb_simulation_parameters{2, -0.23}},
                                                     0.99,
                                                     350,
                                                     create_and_tt(),
                                                     3};

        critical_temperature(lyt, params_one, &criticalstats_one);
        CHECK(criticalstats_one.critical_temperature == 350);

        critical_temperature_stats<TestType> criticalstats_second{};
        critical_temperature(lyt, params_one, &criticalstats_second);
        CHECK(criticalstats_second.critical_temperature == 350);

        const critical_temperature_params    params_two{simulation_engine::EXACT,
                                                     critical_temperature_mode::NON_GATE_BASED_SIMULATION,
                                                     quicksim_params{sidb_simulation_parameters{2, -0.23}},
                                                     0.999,
                                                     450,
                                                     create_and_tt(),
                                                     3};
        critical_temperature_stats<TestType> criticalstats_no_logic{};
        critical_temperature(lyt, params_two, &criticalstats_no_logic);
        CHECK(criticalstats_no_logic.critical_temperature < 40);
    }

    SECTION("Y-shape SiDB XNOR gate with input 11")
    {
        TestType lyt{{20, 10}};

        lyt.assign_cell_type({39, 2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({35, 4, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({31, 7, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({31, 10, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({31, 13, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({31, 8, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({25, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({31, 11, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({31, 5, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({23, 2, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({27, 4, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({37, 3, 0}, TestType::cell_type::NORMAL);

        critical_temperature_stats<TestType> criticalstats{};
        const critical_temperature_params    params{simulation_engine::EXACT,
                                                 critical_temperature_mode::GATE_BASED_SIMULATION,
                                                 quicksim_params{sidb_simulation_parameters{2, -0.28}},
                                                 0.99,
                                                 350,
                                                 create_xnor_tt(),
                                                 3};
        critical_temperature(lyt, params, &criticalstats);
        CHECK(criticalstats.critical_temperature < 13);
    }

    SECTION("Y-shape SiDB XNOR gate with input 11, small µ, gate-based")
    {
        TestType lyt{{20, 10}};

        lyt.assign_cell_type({39, 2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({35, 4, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({31, 7, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({31, 10, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({31, 13, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({31, 8, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({25, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({31, 11, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({31, 5, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({23, 2, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({27, 4, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({37, 3, 0}, TestType::cell_type::NORMAL);

        critical_temperature_stats<TestType> criticalstats{};
        const critical_temperature_params    params{simulation_engine::EXACT,
                                                 critical_temperature_mode::GATE_BASED_SIMULATION,
                                                 quicksim_params{sidb_simulation_parameters{2, -0.15}},
                                                 0.99,
                                                 350,
                                                 create_xnor_tt(),
                                                 3};
        critical_temperature(lyt, params, &criticalstats);
        CHECK(criticalstats.critical_temperature == 0);
    }

    SECTION("Y-shape SiDB XNOR gate with input 11, small µ, non-gate-based")
    {
        TestType lyt{{20, 10}};

        lyt.assign_cell_type({39, 2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({35, 4, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({31, 7, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({31, 10, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({31, 13, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({31, 8, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({25, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({31, 11, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({31, 5, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({23, 2, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({27, 4, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({37, 3, 0}, TestType::cell_type::NORMAL);

        critical_temperature_stats<TestType> criticalstats{};
        const critical_temperature_params    params{simulation_engine::EXACT,
                                                 critical_temperature_mode::NON_GATE_BASED_SIMULATION,
                                                 quicksim_params{sidb_simulation_parameters{2, -0.15}}, 0.99, 350};
        critical_temperature(lyt, params, &criticalstats);
        CHECK(criticalstats.algorithm_name == "exgs");
        CHECK(criticalstats.critical_temperature < 200);
        CHECK(criticalstats.critical_temperature > 0);
    }

    SECTION("Y-shape SiDB XNOR gate with input 11, small µ, non-gate-based, approximate")
    {
        TestType lyt{{20, 10}};

        lyt.assign_cell_type({39, 2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({35, 4, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({31, 7, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({31, 10, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({31, 13, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({31, 8, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({25, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({31, 11, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({31, 5, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({23, 2, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({27, 4, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({37, 3, 0}, TestType::cell_type::NORMAL);

        critical_temperature_stats<TestType> criticalstats{};
        const critical_temperature_params    params{simulation_engine::APPROXIMATE,
                                                 critical_temperature_mode::NON_GATE_BASED_SIMULATION,
                                                 quicksim_params{sidb_simulation_parameters{2, -0.15}}, 0.99, 350};
        critical_temperature(lyt, params, &criticalstats);
        CHECK(criticalstats.algorithm_name == "quicksim");
        CHECK(criticalstats.critical_temperature < 200);
        CHECK(criticalstats.critical_temperature > 0);
    }
}
