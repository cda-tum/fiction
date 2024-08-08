//
// Created by Jan Drewniok on 02.02.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/simulation/sidb/quicksim.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/algorithms/simulation/sidb/time_to_solution.hpp>
#include <fiction/technology/physical_constants.hpp>
#include <fiction/types.hpp>

#include <cmath>
#include <limits>

using namespace fiction;

TEMPLATE_TEST_CASE("time to solution test", "[time-to-solution]", (sidb_100_cell_clk_lyt_siqad),
                   (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};

    SECTION("layout with no SiDB placed")
    {
        const sidb_simulation_parameters params{2, -0.30};
        const quicksim_params            quicksim_params{params};
        time_to_solution_stats           tts_stat_quickexact{};
        const time_to_solution_params    tts_params_quickexact{exhaustive_sidb_simulation_engine::QUICKEXACT};
        time_to_solution<TestType>(lyt, quicksim_params, tts_params_quickexact, &tts_stat_quickexact);

        CHECK(tts_stat_quickexact.algorithm == "QuickExact");
        CHECK_THAT(tts_stat_quickexact.acc, Catch::Matchers::WithinAbs(0.0, 0.00001));
        CHECK_THAT(tts_stat_quickexact.time_to_solution,
                   Catch::Matchers::WithinAbs(std::numeric_limits<double>::max(), 0.00001));
        CHECK(tts_stat_quickexact.mean_single_runtime > 0.0);

        time_to_solution_stats        tts_stat_exgs{};
        const time_to_solution_params tts_params_exgs{exhaustive_sidb_simulation_engine::EXGS};
        time_to_solution<TestType>(lyt, quicksim_params, tts_params_exgs, &tts_stat_exgs);

        CHECK(tts_stat_exgs.algorithm == "ExGS");
        CHECK_THAT(tts_stat_exgs.acc, Catch::Matchers::WithinAbs(0.0, 0.00001));
        CHECK_THAT(tts_stat_exgs.time_to_solution,
                   Catch::Matchers::WithinAbs(std::numeric_limits<double>::max(), 0.00001));
        CHECK(tts_stat_exgs.mean_single_runtime > 0.0);
    }

    SECTION("layout with seven SiDBs placed")
    {
        // layout should not have positively charged SiDBs since QuickSim is a two-state simulator at the moment.
        lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({10, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({12, 3, 0}, TestType::cell_type::NORMAL);

        const sidb_simulation_parameters params{2, -0.30};
        const quicksim_params            quicksim_params{params};

        const time_to_solution_params tts_params_exgs{exhaustive_sidb_simulation_engine::EXGS};
        time_to_solution_stats        tts_stat_exgs{};
        time_to_solution<TestType>(lyt, quicksim_params, tts_params_exgs, &tts_stat_exgs);

        CHECK(tts_stat_exgs.acc == 100.0);
        CHECK(tts_stat_exgs.time_to_solution > 0.0);
        CHECK(tts_stat_exgs.mean_single_runtime > 0.0);

        time_to_solution_stats        tts_stat_quickexact{};
        const time_to_solution_params tts_params{exhaustive_sidb_simulation_engine::QUICKEXACT};
        time_to_solution<TestType>(lyt, quicksim_params, tts_params, &tts_stat_quickexact);

        REQUIRE(tts_stat_quickexact.acc == 100.0);
        CHECK(tts_stat_quickexact.time_to_solution > 0.0);
        CHECK(tts_stat_quickexact.mean_single_runtime > 0.0);

        // calculate tts manually.
        double tts_calculated = 0.0;

        if (tts_stat_quickexact.acc == 100)
        {
            tts_calculated = tts_stat_quickexact.mean_single_runtime;
        }
        else
        {
            tts_calculated = (tts_stat_quickexact.mean_single_runtime * std::log(1.0 - tts_params.confidence_level) /
                              std::log(1.0 - tts_stat_quickexact.acc));
        }
        CHECK_THAT(tts_stat_quickexact.time_to_solution - tts_calculated,
                   Catch::Matchers::WithinAbs(0.0, physical_constants::POP_STABILITY_ERR));
    }
}

TEMPLATE_TEST_CASE("time to solution test, using offset coordinates", "[time-to-solution]", sidb_cell_clk_lyt,
                   cds_sidb_cell_clk_lyt)
{

    TestType lyt{};

    SECTION("layout with seven SiDBs placed")
    {
        lyt.assign_cell_type({1, 6, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 6, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 6, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 6, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({10, 6, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({12, 6, 0}, TestType::cell_type::NORMAL);

        const sidb_simulation_parameters params{3, -0.32};
        const quicksim_params            quicksim_params{params};

        const time_to_solution_params tts_params_exgs{exhaustive_sidb_simulation_engine::EXGS};
        time_to_solution_stats        tts_stat_exgs{};
        time_to_solution<TestType>(lyt, quicksim_params, tts_params_exgs, &tts_stat_exgs);

        CHECK(tts_stat_exgs.acc == 100);
        CHECK(tts_stat_exgs.time_to_solution > 0.0);
        CHECK(tts_stat_exgs.mean_single_runtime > 0.0);

        time_to_solution_stats        tts_stat_quickexact{};
        const time_to_solution_params tts_params{exhaustive_sidb_simulation_engine::QUICKEXACT};
        time_to_solution<TestType>(lyt, quicksim_params, tts_params, &tts_stat_quickexact);

        REQUIRE(tts_stat_quickexact.acc == 100);
        CHECK(tts_stat_quickexact.time_to_solution > 0.0);
        CHECK(tts_stat_quickexact.mean_single_runtime > 0.0);

        // calculate tts manually.
        double tts_calculated = 0.0;

        if (tts_stat_quickexact.acc == 100)
        {
            tts_calculated = tts_stat_quickexact.mean_single_runtime;
        }
        else
        {
            tts_calculated = (tts_stat_quickexact.mean_single_runtime * std::log(1.0 - tts_params.confidence_level) /
                              std::log(1.0 - tts_stat_quickexact.acc));
        }
        CHECK_THAT(tts_stat_quickexact.time_to_solution - tts_calculated,
                   Catch::Matchers::WithinAbs(0.0, physical_constants::POP_STABILITY_ERR));
    }
}
