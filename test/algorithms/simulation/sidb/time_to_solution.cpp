//
// Created by Jan Drewniok on 02.02.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/simulation/sidb/quickexact.hpp>
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

    TestType lyt{{20, 10}};

    SECTION("layout with no SiDB placed, exgs")
    {
        const sidb_simulation_parameters params{2, -0.30};
        const quicksim_params            quicksim_params{params};
        time_to_solution_stats           tts_stat{};
        sim_acc_tts<TestType>(lyt, quicksim_params, &tts_stat);

        CHECK(tts_stat.algorithm == "exgs");
        CHECK_THAT(tts_stat.acc, Catch::Matchers::WithinAbs(0.0, 0.00001));
        CHECK_THAT(tts_stat.time_to_solution, Catch::Matchers::WithinAbs(std::numeric_limits<double>::max(), 0.00001));
        CHECK(tts_stat.mean_single_runtime > 0.0);
    }

    SECTION("layout with no SiDB placed, quickexact")
    {
        const sidb_simulation_parameters params{2, -0.30};
        const quicksim_params            quicksim_params{params};
        time_to_solution_stats           tts_stat{};
        const time_to_solution_params    tts_params{exact_algorithm_type::QUICKEXACT};
        sim_acc_tts<TestType>(lyt, quicksim_params, &tts_stat, tts_params);
        CHECK(tts_stat.algorithm == "quickexact");
        CHECK_THAT(tts_stat.acc, Catch::Matchers::WithinAbs(0.0, 0.00001));
        CHECK_THAT(tts_stat.time_to_solution, Catch::Matchers::WithinAbs(std::numeric_limits<double>::max(), 0.00001));
        CHECK(tts_stat.mean_single_runtime > 0.0);
    }

    SECTION("layout with seven SiDBs placed")
    {
        lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({4, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({2, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({6, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({8, 3, 0}, TestType::cell_type::NORMAL);

        const sidb_simulation_parameters params{3, -0.30};
        const quicksim_params            quicksim_params{params};

        time_to_solution_stats tts_stat_exgs{};
        sim_acc_tts<TestType>(lyt, quicksim_params, &tts_stat_exgs);

        CHECK(tts_stat_exgs.acc == 100);
        CHECK(tts_stat_exgs.time_to_solution > 0.0);
        CHECK(tts_stat_exgs.mean_single_runtime > 0.0);

        time_to_solution_stats        tts_stat_quickexact{};
        const time_to_solution_params tts_params{exact_algorithm_type::QUICKEXACT};
        sim_acc_tts<TestType>(lyt, quicksim_params, &tts_stat_quickexact, tts_params);

        CHECK(tts_stat_quickexact.acc == 100);
        CHECK(tts_stat_quickexact.time_to_solution > 0.0);
        CHECK(tts_stat_quickexact.mean_single_runtime > 0.0);
        CHECK(tts_stat_exgs.single_runtime_exhaustive > tts_stat_quickexact.single_runtime_exhaustive);
    }
}
