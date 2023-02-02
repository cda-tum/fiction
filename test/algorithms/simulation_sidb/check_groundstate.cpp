//
// Created by Jan Drewniok on 02.02.23.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/algorithms/simulation_sidb/quicksim.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/algorithms/simulation_sidb/exhaustive_ground_state_simulation.hpp>
#include <fiction/algorithms/simulation_sidb/check_groundstate.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE(
    "check if ground state is found", "[check_groundstate]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_column_hex>>>))
{
    SECTION("layout with no SiDB placed")
    {
        TestType lyt{{20, 10}};
        charge_distribution_surface      charge_layout{lyt};
        exgs_stats<TestType>             exgs_stats{};
        const sidb_simulation_parameters params{2, -0.32};
        exhaustive_ground_state_simulation<TestType>(charge_layout, params, &exgs_stats);
        quicksim_stats<TestType> quicksimstats{};
        const quicksim_params    quicksim_params{params};
        quicksim<TestType>(charge_layout, quicksim_params, &quicksimstats);

        CHECK(!check_groundstate(quicksimstats, exgs_stats));
    }

    SECTION("layout with seven SiDBs placed")
    {
        TestType lyt{{20, 10}};

        lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({4, 3, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({6, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 3, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({6, 10, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 10, 0}, TestType::cell_type::NORMAL);

        charge_distribution_surface      charge_layout{lyt};
        exgs_stats<TestType>             exgs_stats{};
        const sidb_simulation_parameters params{2, -0.32};
        exhaustive_ground_state_simulation<TestType>(charge_layout, params, &exgs_stats);
        quicksim_stats<TestType>    quicksimstats{};
        const quicksim_params       quicksim_params{params};
        quicksim<TestType>(charge_layout, quicksim_params, &quicksimstats);

        CHECK(check_groundstate(quicksimstats, exgs_stats));
    }
}
