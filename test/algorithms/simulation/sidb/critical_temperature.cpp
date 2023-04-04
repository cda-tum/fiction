//
// Created by Jan Drewniok on 07.02.23.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/critical_temperature.hpp>
#include <fiction/algorithms/simulation/sidb/energy_distribution.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE(
    "Test energy_distribution function", "[energy_distribution]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_column_hex>>>))
{

    SECTION("empty layout")
    {
        TestType lyt{{10, 10}};

        const sidb_simulation_parameters params{2, -0.32};

        critical_temperature_stats<TestType> criticalstats{};
        critical_temperature(lyt, true, params, &criticalstats, 0.99, 350, "or", "10");
        CHECK(criticalstats.num_valid_lyt == 0);
        CHECK(criticalstats.critical_temperature == 0);
    }

    SECTION("one SiDB placed")
    {
        TestType lyt{{10, 10}};
        lyt.assign_cell_type({0, 0}, TestType::cell_type::NORMAL);

        const sidb_simulation_parameters params{2, -0.32};

        critical_temperature_stats<TestType> criticalstats{};
        critical_temperature(lyt, true, params, &criticalstats, 0.99, 350, "or", "10");
        CHECK(criticalstats.num_valid_lyt == 1);
        CHECK(criticalstats.critical_temperature == 350);

        critical_temperature_stats<TestType> criticalstats_new{};
        critical_temperature(lyt, false, params, &criticalstats_new, 0.99, 350);
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

        const sidb_simulation_parameters params{2, -0.23};

        critical_temperature_stats<TestType> criticalstats{};
        critical_temperature(lyt, true, params, &criticalstats, 0.99, 350, "or", "10");
        CHECK(criticalstats.critical_temperature == 350);

        critical_temperature_stats<TestType> criticalstats_one{};
        critical_temperature(lyt, true, params, &criticalstats_one, 0.99, 350, "and", "11");
        CHECK(criticalstats_one.critical_temperature == 350);

        critical_temperature_stats<TestType> criticalstats_second{};
        critical_temperature(lyt, true, params, &criticalstats_second, 0.99, 350, "and", "11");
        CHECK(criticalstats_second.critical_temperature == 350);

        critical_temperature_stats<TestType> criticalstats_no_logic{};
        critical_temperature(lyt, false, params, &criticalstats_no_logic, 0.999, 450);
        CHECK(criticalstats_no_logic.critical_temperature < 40);
    }
}
