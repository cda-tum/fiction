//
// Created by Jan Drewniok on 18.12.22.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/simulation/sidb/determine_groundstate_from_simulation_results.hpp>
#include <fiction/algorithms/simulation/sidb/energy_distribution.hpp>
#include <fiction/algorithms/simulation/sidb/quicksim.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/technology/physical_constants.hpp>
#include <fiction/technology/sidb_charge_state.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <cstdint>

using namespace fiction;

TEMPLATE_TEST_CASE("Zero iteration steps", "[quicksim]", (sidb_100_cell_clk_lyt_siqad),
                   (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};

    const quicksim_params quicksim_params{sidb_simulation_parameters{2, -0.30}, 0};

    const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

    CHECK(simulation_results.charge_distributions.empty());
    REQUIRE(simulation_results.additional_simulation_parameters.empty());
    CHECK(simulation_results.algorithm_name == "");
}

TEMPLATE_TEST_CASE("Empty layout QuickSim simulation", "[quicksim]", (sidb_100_cell_clk_lyt_siqad),
                   (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};

    const quicksim_params quicksim_params{sidb_simulation_parameters{2, -0.30}};

    REQUIRE(quicksim_params.simulation_parameters.mu_minus == -0.30);

    const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

    CHECK(simulation_results.charge_distributions.empty());
    REQUIRE(!simulation_results.additional_simulation_parameters.empty());
    CHECK(simulation_results.algorithm_name == "QuickSim");
    CHECK(std::any_cast<uint64_t>(simulation_results.additional_simulation_parameters.at("iteration_steps")) == 80);
    CHECK(std::any_cast<double>(simulation_results.additional_simulation_parameters.at("alpha")) == 0.7);
    CHECK(simulation_results.charge_distributions.empty());
}

TEMPLATE_TEST_CASE("Single SiDB QuickSim simulation", "[quicksim]", (sidb_100_cell_clk_lyt_siqad),
                   (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};

    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);

    const quicksim_params quicksim_params{sidb_simulation_parameters{2, -0.30}};

    REQUIRE(quicksim_params.simulation_parameters.mu_minus == -0.30);

    const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

    CHECK(simulation_results.charge_distributions.size() > 0);
}

template <typename Lyt>
void check_for_absence_of_positive_charges(const sidb_simulation_result<Lyt>& stats) noexcept
{
    REQUIRE(!stats.charge_distributions.empty());

    for (const auto& lyt : stats.charge_distributions)
    {
        CHECK(!lyt.charge_exists(sidb_charge_state::POSITIVE));
    }
}

template <typename Lyt>
void check_for_runtime_measurement(const sidb_simulation_result<Lyt>& stats) noexcept
{
    CHECK(stats.simulation_runtime.count() > 0);
}

TEMPLATE_TEST_CASE("QuickSim simulation of several SiDBs with varying thread counts", "[quicksim]",
                   (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};

    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({4, 3, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({6, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({7, 3, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({6, 10, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({7, 10, 0}, TestType::cell_type::NORMAL);

    const sidb_simulation_parameters params{2, -0.30};

    quicksim_params quicksim_params{params};

    REQUIRE(quicksim_params.simulation_parameters.mu_minus == -0.30);

    SECTION("Default settings")
    {
        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
    }
    SECTION("0 threads")
    {
        quicksim_params.number_threads = 0;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
    }

    SECTION("1 thread")
    {
        quicksim_params.number_threads = 1;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
    }
    SECTION("2 threads")
    {
        quicksim_params.number_threads = 2;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
    }
    SECTION("100 threads")
    {
        quicksim_params.number_threads = 100;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
    }
}

TEMPLATE_TEST_CASE("QuickSim simulation of an SiDB layout comprising of 10 SiDBs with varying thread counts",
                   "[quicksim]", (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};

    lyt.assign_cell_type({-13, -1, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({-9, -1, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({-7, -1, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({-3, -1, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({-1, -1, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({3, -1, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({5, -1, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({9, -1, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({11, -1, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({15, -1, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({17, -1, 1}, TestType::cell_type::NORMAL);
    ;
    const sidb_simulation_parameters params{2, -0.32};

    quicksim_params quicksim_params{params};

    REQUIRE(quicksim_params.simulation_parameters.mu_minus == -0.32);

    const auto check_charge_configuration = [](const sidb_simulation_result<TestType>& stats) noexcept
    {
        REQUIRE(!stats.charge_distributions.empty());

        const auto& charge_lyt_first = stats.charge_distributions.front();

        CHECK(charge_lyt_first.get_charge_state({-13, -1, 1}) == sidb_charge_state::NEGATIVE);

        CHECK(charge_lyt_first.get_charge_state({-9, -1, 1}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({-7, -1, 1}) == sidb_charge_state::NEGATIVE);

        CHECK(charge_lyt_first.get_charge_state({-3, -1, 1}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({-1, -1, 1}) == sidb_charge_state::NEGATIVE);

        CHECK(charge_lyt_first.get_charge_state({3, -1, 1}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({5, -1, 1}) == sidb_charge_state::NEGATIVE);

        CHECK(charge_lyt_first.get_charge_state({9, -1, 1}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({11, -1, 1}) == sidb_charge_state::NEGATIVE);

        CHECK(charge_lyt_first.get_charge_state({15, -1, 1}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({17, -1, 1}) == sidb_charge_state::NEGATIVE);

        CHECK_THAT(charge_lyt_first.get_system_energy(),
                   Catch::Matchers::WithinAbs(0.4798721334, physical_constants::POP_STABILITY_ERR));
    };

    SECTION("Default settings")
    {
        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("0 threads")
    {
        quicksim_params.number_threads = 0;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("1 thread")
    {
        quicksim_params.number_threads = 1;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("2 threads")
    {
        quicksim_params.number_threads = 2;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("100 threads")
    {
        quicksim_params.number_threads = 100;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
}

TEMPLATE_TEST_CASE("QuickSim simulation of a Y-shaped SiDB arrangement with varying thread counts", "[quicksim]",
                   (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};

    lyt.assign_cell_type({-11, -2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({-10, -1, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({-4, -1, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({-3, -2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({-7, 0, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({-7, 1, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({-7, 3, 0}, TestType::cell_type::NORMAL);

    const sidb_simulation_parameters params{2, -0.32};

    quicksim_params quicksim_params{params};

    REQUIRE(quicksim_params.simulation_parameters.mu_minus == -0.32);

    const auto check_charge_configuration = [](const sidb_simulation_result<TestType>& stats) noexcept
    {
        REQUIRE(!stats.charge_distributions.empty());

        const auto& charge_lyt_first = stats.charge_distributions.front();

        CHECK(charge_lyt_first.get_charge_state({-11, -2, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({-10, -1, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({-3, -2, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({-4, -1, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({-7, 0, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({-7, 1, 1}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({-7, 3, 0}) == sidb_charge_state::NEGATIVE);

        CHECK_THAT(charge_lyt_first.get_system_energy(),
                   Catch::Matchers::WithinAbs(0.3191788254, physical_constants::POP_STABILITY_ERR));
    };

    SECTION("Default settings")
    {
        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("0 threads")
    {
        quicksim_params.number_threads = 0;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("1 thread")
    {
        quicksim_params.number_threads = 1;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("2 threads")
    {
        quicksim_params.number_threads = 2;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("100 threads")
    {
        quicksim_params.number_threads = 100;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
}

TEMPLATE_TEST_CASE("QuickSim simulation of a Y-shaped SiDB OR gate with input 01 and varying thread counts",
                   "[quicksim]", (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};

    lyt.assign_cell_type({6, 2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({12, 3, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({14, 2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 5, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({10, 6, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 8, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({16, 1, 0}, TestType::cell_type::NORMAL);

    const sidb_simulation_result<TestType> quicksim_stats{};
    const sidb_simulation_parameters       params{2, -0.28};

    quicksim_params quicksim_params{params};

    REQUIRE(quicksim_params.simulation_parameters.mu_minus == -0.28);

    const auto check_charge_configuration = [](const sidb_simulation_result<TestType>& stats) noexcept
    {
        REQUIRE(!stats.charge_distributions.empty());

        const auto& charge_lyt_first = stats.charge_distributions.front();

        CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE);

        CHECK_THAT(charge_lyt_first.get_system_energy(),
                   Catch::Matchers::WithinAbs(0.4662582096, physical_constants::POP_STABILITY_ERR));
    };

    SECTION("Default settings")
    {
        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("0 threads")
    {
        quicksim_params.number_threads = 0;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("1 thread")
    {
        quicksim_params.number_threads = 1;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("2 threads")
    {
        quicksim_params.number_threads = 2;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("100 threads")
    {
        quicksim_params.number_threads = 100;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
}

TEMPLATE_TEST_CASE("QuickSim simulation of an SiDB BDL pair with varying thread counts", "[quicksim]",
                   (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};

    lyt.assign_cell_type({6, 2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({8, 2, 0}, TestType::cell_type::NORMAL);

    const sidb_simulation_parameters params{2, -0.25};

    quicksim_params quicksim_params{params};

    REQUIRE(quicksim_params.simulation_parameters.mu_minus == -0.25);

    const auto check_charge_configuration = [](const sidb_simulation_result<TestType>& result) noexcept
    {
        REQUIRE(!result.charge_distributions.empty());

        REQUIRE(!energy_distribution(result.charge_distributions).empty());

        const auto& charge_lyt_first = result.charge_distributions.front();

        CHECK((((charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE) &&
                (charge_lyt_first.get_charge_state({8, 2, 0}) == sidb_charge_state::NEUTRAL)) ||
               ((charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEUTRAL) &&
                (charge_lyt_first.get_charge_state({8, 2, 0}) == sidb_charge_state::NEGATIVE))));
    };

    SECTION("Default settings")
    {
        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("0 threads")
    {
        quicksim_params.number_threads = 0;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("1 thread")
    {
        quicksim_params.number_threads = 1;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("2 threads")
    {
        quicksim_params.number_threads = 2;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("100 threads")
    {
        quicksim_params.number_threads = 100;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
}

TEMPLATE_TEST_CASE("QuickSim simulation of an layout comprising of 13 SiDBs", "[quicksim]",
                   (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
{
    // initialize SiDB layout
    TestType lyt{};

    // design random SiDB layout
    lyt.assign_cell_type({11, 15, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({37, 8, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({17, 6, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({37, 19, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({4, 2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({0, 17, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({39, 10, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({22, 19, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({5, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({17, 2, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({27, 13, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({27, 16, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({8, 10, 1}, TestType::cell_type::NORMAL);

    const sidb_simulation_parameters params{2, -0.32};

    quicksim_params quicksim_params{params};

    // check QuickSim parameters
    REQUIRE(quicksim_params.simulation_parameters.mu_minus == -0.32);

    // lambda function to check charge configurations
    const auto check_charge_configuration = [](const sidb_simulation_result<TestType>& stats) noexcept
    {
        REQUIRE(!stats.charge_distributions.empty());

        REQUIRE(!energy_distribution(stats.charge_distributions).empty());

        const auto& charge_lyt_first = stats.charge_distributions.front();

        charge_lyt_first.foreach_cell(
            [&](const auto& cell)
            {
                if (cell != coordinate<TestType>{5, 3, 0})
                {
                    CHECK(charge_lyt_first.get_charge_state(cell) == sidb_charge_state::NEGATIVE);
                }
                else
                {
                    CHECK(charge_lyt_first.get_charge_state({5, 3, 0}) == sidb_charge_state::NEUTRAL);
                }
            });
    };

    SECTION("Default settings")
    {
        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }

    SECTION("0 threads")
    {
        quicksim_params.number_threads = 0;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("1 thread")
    {
        quicksim_params.number_threads = 1;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("2 threads")
    {
        quicksim_params.number_threads = 2;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("100 threads")
    {
        quicksim_params.number_threads = 100;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
}

TEMPLATE_TEST_CASE("QuickSim simulation of an layout comprising of 13 SiDBs, all negatively charged", "[quicksim]",
                   (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};

    lyt.assign_cell_type({26, 10, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({23, 19, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({0, 5, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({38, 10, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({11, 5, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({13, 2, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({40, 19, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({16, 9, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({19, 16, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({5, 8, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({8, 15, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({39, 9, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({30, 15, 0}, TestType::cell_type::NORMAL);

    const sidb_simulation_result<TestType> quicksim_stats{};
    const sidb_simulation_parameters       params{2, -0.32};

    quicksim_params quicksim_params{params};

    REQUIRE(quicksim_params.simulation_parameters.mu_minus == -0.32);

    const auto check_charge_configuration = [](const sidb_simulation_result<TestType>& stats) noexcept
    {
        REQUIRE(!stats.charge_distributions.empty());

        REQUIRE(!energy_distribution(stats.charge_distributions).empty());

        const auto& charge_lyt_first = stats.charge_distributions.front();

        charge_lyt_first.foreach_cell(
            [&](const auto& cell) { CHECK(charge_lyt_first.get_charge_state(cell) == sidb_charge_state::NEGATIVE); });
    };

    SECTION("Default settings")
    {
        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("0 threads")
    {
        quicksim_params.number_threads = 0;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("1 thread")
    {
        quicksim_params.number_threads = 1;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("2 threads")
    {
        quicksim_params.number_threads = 2;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("100 threads")
    {
        quicksim_params.number_threads = 100;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
}

TEMPLATE_TEST_CASE("QuickSim simulation of a Y-shaped SiDB OR gate with input 01", "[ExGS]",
                   (sidb_100_cell_clk_lyt_siqad), (charge_distribution_surface<sidb_100_cell_clk_lyt_siqad>))
{
    TestType lyt{};

    lyt.assign_cell_type({6, 2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({12, 3, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({14, 2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 5, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({10, 6, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 8, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({16, 1, 0}, TestType::cell_type::NORMAL);

    sidb_simulation_parameters params{2, -0.28};

    SECTION("Standard Physical Parameters")
    {
        const quicksim_params quicksim_params{params};

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        REQUIRE(!simulation_results.charge_distributions.empty());
        const auto& charge_lyt_first = simulation_results.charge_distributions.front();

        CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == sidb_charge_state::NEUTRAL);

        CHECK_THAT(charge_lyt_first.get_system_energy(),
                   Catch::Matchers::WithinAbs(0.4662582096, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("Increased mu_minus")
    {
        params.mu_minus = -0.1;

        const quicksim_params quicksim_params{params};

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        REQUIRE(!simulation_results.charge_distributions.empty());
        const auto& charge_lyt_first = simulation_results.charge_distributions.front();

        CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == sidb_charge_state::NEUTRAL);

        CHECK_THAT(charge_lyt_first.get_system_energy(),
                   Catch::Matchers::WithinAbs(0.061037632, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("Decreased mu_minus")
    {
        params.mu_minus = -0.7;

        const quicksim_params quicksim_params{params};

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        REQUIRE(!simulation_results.charge_distributions.empty());
        const auto& charge_lyt_first = simulation_results.charge_distributions.front();

        CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == sidb_charge_state::NEGATIVE);

        CHECK_THAT(charge_lyt_first.get_system_energy(),
                   Catch::Matchers::WithinAbs(2.069954113, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("Decreased lambda_tf")
    {
        params.lambda_tf = 1;

        const quicksim_params quicksim_params{params};

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        REQUIRE(!simulation_results.charge_distributions.empty());
        const auto& charge_lyt_first = simulation_results.charge_distributions.front();

        CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == sidb_charge_state::NEGATIVE);

        CHECK_THAT(charge_lyt_first.get_system_energy(),
                   Catch::Matchers::WithinAbs(0.5432404075, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("Increased lambda_tf")
    {
        params.lambda_tf = 10;

        const quicksim_params quicksim_params{params};

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        REQUIRE(!simulation_results.charge_distributions.empty());
        const auto& charge_lyt_first = simulation_results.charge_distributions.front();

        CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == sidb_charge_state::NEUTRAL);

        CHECK_THAT(charge_lyt_first.get_system_energy(),
                   Catch::Matchers::WithinAbs(0.2930574885, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("Increased epsilon_r")
    {
        params.epsilon_r = 10;

        const quicksim_params quicksim_params{params};

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        REQUIRE(!simulation_results.charge_distributions.empty());
        const auto& charge_lyt_first = simulation_results.charge_distributions.front();

        CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == sidb_charge_state::NEGATIVE);

        CHECK_THAT(charge_lyt_first.get_system_energy(),
                   Catch::Matchers::WithinAbs(0.505173434, physical_constants::POP_STABILITY_ERR));
    }
}

TEMPLATE_TEST_CASE("QuickSim simulation of a Y-shaped SiDB arrangement with varying thread counts, cube coordinates",
                   "[quicksim]", (sidb_100_cell_clk_lyt_cube), (cds_sidb_100_cell_clk_lyt_cube))
{
    TestType lyt{};

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{-11, -2, 0}),
                         TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{-10, -1, 0}),
                         TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{-4, -1, 0}),
                         TestType::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{-3, -2, 0}),
                         TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{-7, 0, 1}), TestType::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{-7, 1, 1}), TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{-7, 3, 0}), TestType::cell_type::NORMAL);

    const sidb_simulation_parameters params{2, -0.32};

    quicksim_params quicksim_params{params};

    REQUIRE(quicksim_params.simulation_parameters.mu_minus == -0.32);

    const auto check_charge_configuration = [](const sidb_simulation_result<TestType>& stats) noexcept
    {
        REQUIRE(!stats.charge_distributions.empty());

        const auto& charge_lyt_first = stats.charge_distributions.front();

        CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{-11, -2, 0})) ==
              sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{-10, -1, 0})) ==
              sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{-3, -2, 0})) ==
              sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{-4, -1, 0})) ==
              sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{-7, 0, 1})) ==
              sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{-7, 1, 1})) ==
              sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{-7, 3, 0})) ==
              sidb_charge_state::NEGATIVE);

        CHECK_THAT(charge_lyt_first.get_system_energy(),
                   Catch::Matchers::WithinAbs(0.3191788254, physical_constants::POP_STABILITY_ERR));
    };

    SECTION("Default settings")
    {
        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("0 threads")
    {
        quicksim_params.number_threads = 0;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("1 thread")
    {
        quicksim_params.number_threads = 1;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("2 threads")
    {
        quicksim_params.number_threads = 2;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
    SECTION("100 threads")
    {
        quicksim_params.number_threads = 100;

        const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

        check_for_absence_of_positive_charges(simulation_results);
        check_for_runtime_measurement(simulation_results);
        check_charge_configuration(simulation_results);
    }
}

TEMPLATE_TEST_CASE("QuickSim gate simulation on the Si-111 surface", "[quicksim]", (sidb_111_cell_clk_lyt_siqad),
                   (cds_sidb_111_cell_clk_lyt_siqad))
{
    TestType lyt{};
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({1, 1, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 2, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({8, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({6, 1, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({5, 2, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({4, 8, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({4, 10, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({4, 14, 0}, TestType::cell_type::NORMAL);

    const quicksim_params params{sidb_simulation_parameters{2, -0.32, 5.6, 5}};

    const auto simulation_results = quicksim<TestType>(lyt, params);

    const auto ground_state = determine_groundstate_from_simulation_results(simulation_results);
    REQUIRE(ground_state.size() == 1);

    CHECK(ground_state.front().get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(ground_state.front().get_charge_state({1, 1, 1}) == sidb_charge_state::NEUTRAL);
    CHECK(ground_state.front().get_charge_state({2, 2, 1}) == sidb_charge_state::NEGATIVE);
    CHECK(ground_state.front().get_charge_state({8, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(ground_state.front().get_charge_state({6, 1, 1}) == sidb_charge_state::NEUTRAL);
    CHECK(ground_state.front().get_charge_state({5, 2, 1}) == sidb_charge_state::NEGATIVE);
    CHECK(ground_state.front().get_charge_state({4, 8, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(ground_state.front().get_charge_state({4, 10, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(ground_state.front().get_charge_state({4, 14, 0}) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE("QuickSim AND gate simulation on the Si-111 surface", "[quicksim]", sidb_111_cell_clk_lyt_siqad,
                   cds_sidb_111_cell_clk_lyt_siqad)
{
    SECTION("no input applied")
    {
        const auto            lyt = blueprints::and_gate_111<TestType>();
        const quicksim_params params{sidb_simulation_parameters{2, -0.32, 5.6, 5}, 3000, 0.5};

        const auto simulation_results = quicksim<TestType>(lyt, params);

        const auto ground_state = determine_groundstate_from_simulation_results(simulation_results);
        REQUIRE(ground_state.size() == 1);

        CHECK(ground_state.front().get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({1, 1, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({25, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({23, 1, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({4, 4, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({21, 4, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({5, 5, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({19, 5, 1}) == sidb_charge_state::NEGATIVE);

        CHECK(ground_state.front().get_charge_state({17, 8, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({8, 8, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({9, 9, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({15, 9, 1}) == sidb_charge_state::NEGATIVE);

        CHECK(ground_state.front().get_charge_state({10, 18, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({13, 17, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({16, 18, 0}) == sidb_charge_state::NEGATIVE);

        CHECK(ground_state.front().get_charge_state({15, 21, 1}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({17, 23, 0}) == sidb_charge_state::NEGATIVE);

        CHECK(ground_state.front().get_charge_state({19, 25, 1}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({21, 27, 0}) == sidb_charge_state::NEGATIVE);

        CHECK(ground_state.front().get_charge_state({23, 29, 1}) == sidb_charge_state::NEGATIVE);
    }

    SECTION("10 input applied")
    {
        auto lyt = blueprints::and_gate_111<TestType>();
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::EMPTY);
        lyt.assign_cell_type({23, 1, 1}, TestType::cell_type::EMPTY);

        const quicksim_params params{sidb_simulation_parameters{2, -0.32, 5.6, 5}, 3000, 0.5};

        const auto simulation_results = quicksim<TestType>(lyt, params);

        const auto ground_state = determine_groundstate_from_simulation_results(simulation_results);

        REQUIRE(ground_state.size() == 1);

        CHECK(ground_state.front().get_charge_state({1, 1, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({25, 0, 0}) == sidb_charge_state::NEGATIVE);

        CHECK(ground_state.front().get_charge_state({4, 4, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({21, 4, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({5, 5, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({19, 5, 1}) == sidb_charge_state::NEUTRAL);

        CHECK(ground_state.front().get_charge_state({8, 8, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({17, 8, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({9, 9, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({15, 9, 1}) == sidb_charge_state::NEUTRAL);

        CHECK(ground_state.front().get_charge_state({10, 18, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({13, 17, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({16, 18, 0}) == sidb_charge_state::NEUTRAL);

        CHECK(ground_state.front().get_charge_state({15, 21, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({17, 23, 0}) == sidb_charge_state::NEUTRAL);

        CHECK(ground_state.front().get_charge_state({19, 25, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({21, 27, 0}) == sidb_charge_state::NEUTRAL);

        CHECK(ground_state.front().get_charge_state({23, 29, 1}) == sidb_charge_state::NEGATIVE);
    }
}
