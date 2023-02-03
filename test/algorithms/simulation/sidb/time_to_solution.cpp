//
// Created by Jan Drewniok on 02.02.23.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp>
#include <fiction/algorithms/simulation/sidb/quicksim.hpp>
#include <fiction/algorithms/simulation/sidb/time_to_solution.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE(
    "time to solution test", "[sim_acc_tss]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_column_hex>>>))
{

    SECTION("layout with no SiDB placed")
    {
        TestType                    lyt{{20, 10}};
        charge_distribution_surface charge_layout{lyt};

        const sidb_simulation_parameters params{2, -0.30};
        const quicksim_params            quicksim_params{params};
        time_to_solution_stats           tts_stat{};
        sim_acc_tts<TestType>(charge_layout, quicksim_params, &tts_stat);
        CHECK(tts_stat.acc == 0.0);
        CHECK(tts_stat.time_to_solution == std::numeric_limits<double>::max());
        CHECK(tts_stat.mean_single_runtime > 0.0);
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

        time_to_solution_stats           tts_stat{};
        const sidb_simulation_parameters params{2, -0.30};
        const quicksim_params            quicksim_params{params};
        sim_acc_tts<TestType>(lyt, quicksim_params, &tts_stat);
        CHECK(tts_stat.acc == 100);
        CHECK(tts_stat.time_to_solution > 0.0);
        CHECK(tts_stat.mean_single_runtime > 0.0);
    }
}
