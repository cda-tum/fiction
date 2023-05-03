//
// Created by Jan Drewniok on 02.02.23.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp>
#include <fiction/algorithms/simulation/sidb/is_ground_state.hpp>
#include <fiction/algorithms/simulation/sidb/quicksim.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE(
    "check if ground state is found", "[is_ground_state]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_column_hex>>>))
{
    SECTION("layout with no SiDB placed")
    {
        TestType                         lyt{{20, 10}};
        const sidb_simulation_parameters params{2, -0.32};
        const auto            simulation_results_exgs = exhaustive_ground_state_simulation<TestType>(lyt, params);
        const quicksim_params quicksim_params{params};
        const auto            simulation_results_quicksim = quicksim<TestType>(lyt, quicksim_params);

        CHECK(!is_ground_state(simulation_results_exgs, simulation_results_quicksim));
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

        const sidb_simulation_parameters params{2, -0.32};

        const auto simulation_results_exgs = exhaustive_ground_state_simulation<TestType>(lyt, params);

        const quicksim_params quicksim_params{params};
        const auto            simulation_results_quicksim = quicksim<TestType>(lyt, quicksim_params);

        CHECK(is_ground_state(simulation_results_exgs, simulation_results_quicksim));
    }
}
