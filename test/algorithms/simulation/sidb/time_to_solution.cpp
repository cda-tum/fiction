//
// Created by Jan Drewniok on 02.02.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/simulation/sidb/quickexact.hpp>
#include <fiction/algorithms/simulation/sidb/quicksim.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp>
#include <fiction/algorithms/simulation/sidb/time_to_solution.hpp>
#include <fiction/technology/constants.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <cmath>
#include <cstdlib>
#include <limits>
#include <vector>

using namespace fiction;

// Test 1: Basic time-to-solution test with different layout types
TEMPLATE_TEST_CASE("Basic time-to-solution test with varying layouts", "[time-to-solution]",
                   sidb_100_cell_clk_lyt_siqad, cds_sidb_100_cell_clk_lyt_siqad)
{
    TestType lyt{};

    SECTION("layout with no SiDB placed")
    {
        const sidb_simulation_parameters params{2, -0.30};
        const quicksim_params            quicksim_params{params};
        time_to_solution_stats           tts_stat_quickexact{};
        const time_to_solution_params    tts_params_quickexact{exact_sidb_simulation_engine::QUICKEXACT};
        time_to_solution<TestType>(lyt, quicksim_params, tts_params_quickexact, &tts_stat_quickexact);

        CHECK(tts_stat_quickexact.algorithm == "QuickExact");
        CHECK_THAT(tts_stat_quickexact.acc, Catch::Matchers::WithinAbs(0.0, 0.00001));
        CHECK_THAT(tts_stat_quickexact.time_to_solution,
                   Catch::Matchers::WithinAbs(std::numeric_limits<double>::max(), 0.00001));
        CHECK_THAT(tts_stat_quickexact.mean_single_runtime, Catch::Matchers::WithinAbs(0.0, 0.00001));

#if (FICTION_ALGLIB_ENABLED)

        time_to_solution_stats        tts_stat_clustercomplete{};
        const time_to_solution_params tts_params_clustercomplete{exact_sidb_simulation_engine::CLUSTERCOMPLETE};
        time_to_solution<TestType>(lyt, quicksim_params, tts_params_clustercomplete, &tts_stat_clustercomplete);

        CHECK(tts_stat_clustercomplete.algorithm == "ClusterComplete");
        CHECK_THAT(tts_stat_clustercomplete.acc, Catch::Matchers::WithinAbs(0.0, 0.00001));
        CHECK_THAT(tts_stat_clustercomplete.time_to_solution,
                   Catch::Matchers::WithinAbs(std::numeric_limits<double>::max(), 0.00001));
        CHECK_THAT(tts_stat_clustercomplete.mean_single_runtime, Catch::Matchers::WithinAbs(0.0, 0.00001));

#endif  // FICTION_ALGLIB_ENABLED

        time_to_solution_stats        tts_stat_exgs{};
        const time_to_solution_params tts_params_exgs{exact_sidb_simulation_engine::EXGS};
        time_to_solution<TestType>(lyt, quicksim_params, tts_params_exgs, &tts_stat_exgs);

        CHECK(tts_stat_exgs.algorithm == "ExGS");
        CHECK_THAT(tts_stat_exgs.acc, Catch::Matchers::WithinAbs(0.0, 0.00001));
        CHECK_THAT(tts_stat_exgs.time_to_solution,
                   Catch::Matchers::WithinAbs(std::numeric_limits<double>::max(), 0.00001));
        CHECK_THAT(tts_stat_exgs.mean_single_runtime, Catch::Matchers::WithinAbs(0.0, 0.00001));
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

        const time_to_solution_params tts_params_exgs{exact_sidb_simulation_engine::EXGS};
        time_to_solution_stats        tts_stat_exgs{};
        time_to_solution<TestType>(lyt, quicksim_params, tts_params_exgs, &tts_stat_exgs);

        CHECK(tts_stat_exgs.acc == 100.0);
        CHECK(tts_stat_exgs.time_to_solution > 0.0);
        CHECK(tts_stat_exgs.mean_single_runtime > 0.0);

        time_to_solution_stats        tts_stat_quickexact{};
        const time_to_solution_params tts_params{exact_sidb_simulation_engine::QUICKEXACT};
        time_to_solution<TestType>(lyt, quicksim_params, tts_params, &tts_stat_quickexact);

        REQUIRE(tts_stat_quickexact.acc == 100.0);
        CHECK(tts_stat_quickexact.time_to_solution > 0.0);
        CHECK(tts_stat_quickexact.mean_single_runtime > 0.0);

        // calculate tts manually.
        double tts_calculated = (tts_stat_quickexact.acc == 100) ? tts_stat_quickexact.mean_single_runtime :
                                                                   (tts_stat_quickexact.mean_single_runtime *
                                                                    std::log(1.0 - tts_params.confidence_level) /
                                                                    std::log(1.0 - tts_stat_quickexact.acc));

        CHECK_THAT(tts_stat_quickexact.time_to_solution - tts_calculated,
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));

#if (FICTION_ALGLIB_ENABLED)

        time_to_solution_stats        tts_stat_clustercomplete{};
        const time_to_solution_params tts_params_clustercomplete{exact_sidb_simulation_engine::CLUSTERCOMPLETE};
        time_to_solution<TestType>(lyt, quicksim_params, tts_params_clustercomplete, &tts_stat_clustercomplete);

        REQUIRE(tts_stat_clustercomplete.acc == 100);
        CHECK(tts_stat_clustercomplete.time_to_solution > 0.0);
        CHECK(tts_stat_clustercomplete.mean_single_runtime > 0.0);

        if (tts_stat_clustercomplete.acc == 100)
        {
            tts_calculated = tts_stat_clustercomplete.mean_single_runtime;
        }
        else
        {
            tts_calculated =
                (tts_stat_clustercomplete.mean_single_runtime * std::log(1.0 - tts_params.confidence_level) /
                 std::log(1.0 - tts_stat_clustercomplete.acc));
        }
        CHECK_THAT(tts_stat_clustercomplete.time_to_solution - tts_calculated,
                   Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));

#endif  // FICTION_ALGLIB_ENABLED
    }
}

// Test 2: Offset coordinates time-to-solution test
TEMPLATE_TEST_CASE("time-to-solution test with offset coordinates", "[time-to-solution]", cds_sidb_100_cell_clk_lyt)
{
    TestType lyt{};

    SECTION("layout with seven SiDBs placed")
    {
        lyt.assign_cell_type({1, 6, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 6, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 6, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({10, 6, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({18, 9, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({20, 9, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);

        const sidb_simulation_parameters params{2, -0.32};

        quicksim_params quicksim_params{params};
        quicksim_params.iteration_steps = 10;

        const time_to_solution_params tts_params_exgs{exact_sidb_simulation_engine::EXGS};
        time_to_solution_stats        tts_stat_exgs{};
        time_to_solution<TestType>(lyt, quicksim_params, tts_params_exgs, &tts_stat_exgs);

        CHECK(tts_stat_exgs.time_to_solution > 0.0);
        CHECK(tts_stat_exgs.mean_single_runtime > 0.0);

        time_to_solution_stats        tts_stat_quickexact{};
        const time_to_solution_params tts_params{exact_sidb_simulation_engine::QUICKEXACT};
        time_to_solution<TestType>(lyt, quicksim_params, tts_params, &tts_stat_quickexact);

        CHECK(tts_stat_quickexact.time_to_solution > 0.0);
        CHECK(tts_stat_quickexact.mean_single_runtime > 0.0);

        auto tts_calculated = std::numeric_limits<double>::max();

        if (tts_stat_quickexact.acc == 100)
        {
            tts_calculated = tts_stat_quickexact.mean_single_runtime;
            CHECK_THAT(tts_stat_quickexact.time_to_solution - tts_calculated,
                       Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        }
        else if (tts_stat_quickexact.acc != 0)
        {
            tts_calculated = (tts_stat_quickexact.mean_single_runtime * std::log(1.0 - tts_params.confidence_level) /
                              std::log(1.0 - (tts_stat_quickexact.acc / 100)));
            CHECK_THAT(tts_stat_quickexact.time_to_solution - tts_calculated,
                       Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
        }
    }
}

TEMPLATE_TEST_CASE("time-to-solution test with simulation results", "[time-to-solution]", sidb_100_cell_clk_lyt,
                   cds_sidb_100_cell_clk_lyt)
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

        const std::size_t                             number_of_repetitions = 100;
        std::vector<sidb_simulation_result<TestType>> simulation_results_quicksim{};
        simulation_results_quicksim.reserve(number_of_repetitions);

        for (auto i = 0u; i < number_of_repetitions; i++)
        {
            const auto simulation_result = quicksim<TestType>(lyt, quicksim_params);
            if (simulation_result.has_value())
            {
                simulation_results_quicksim.push_back(simulation_result.value());
            }
        }

        const auto simulation_results_quickexact =
            quickexact(lyt, quickexact_params<cell<TestType>>{quicksim_params.simulation_parameters});

        time_to_solution_stats st{};
        time_to_solution_for_given_simulation_results(simulation_results_quickexact, simulation_results_quicksim, 0.997,
                                                      &st);

        REQUIRE(st.acc == 100);
        CHECK(st.time_to_solution > 0.0);
        CHECK(st.mean_single_runtime > 0.0);

        const auto tts_calculated = (st.acc == 100) ?
                                        st.mean_single_runtime :
                                        (st.mean_single_runtime * std::log(1.0 - 0.997) / std::log(1.0 - st.acc));

        CHECK_THAT(st.time_to_solution - tts_calculated, Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
    }
}
