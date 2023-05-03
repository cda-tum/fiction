//
// Created by Jan Drewniok on 18.12.22.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/simulation/sidb/quicksim.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/physical_constants.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE("Empty layout QuickSim simulation", "[quicksim]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

    const quicksim_params quicksim_params{sidb_simulation_parameters{2, -0.30}};

    REQUIRE(quicksim_params.phys_params.mu == -0.30);

    const auto simulation_results = quicksim<TestType>(lyt, quicksim_params);

    CHECK(simulation_results.charge_distributions.empty());
    REQUIRE(!simulation_results.additional_simulation_parameters.empty());
    CHECK(simulation_results.algorithm_name == "quicksim");
    CHECK(simulation_results.additional_simulation_parameters[0].first == "iteration_steps");
    CHECK(std::any_cast<uint64_t>(simulation_results.additional_simulation_parameters[0].second) == 80);
    CHECK(simulation_results.additional_simulation_parameters[1].first == "alpha");
    CHECK(std::any_cast<double>(simulation_results.additional_simulation_parameters[1].second) == 0.7);
    CHECK(simulation_results.charge_distributions.empty());
}

TEMPLATE_TEST_CASE("Single SiDB QuickSim simulation", "[quicksim]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);

    const quicksim_params quicksim_params{sidb_simulation_parameters{2, -0.30}};

    REQUIRE(quicksim_params.phys_params.mu == -0.30);

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
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({4, 3, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({6, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({7, 3, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({6, 10, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({7, 10, 0}, TestType::cell_type::NORMAL);

    const sidb_simulation_parameters params{2, -0.30};

    quicksim_params quicksim_params{params};

    REQUIRE(quicksim_params.phys_params.mu == -0.30);

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
                   "[quicksim]", (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

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

    REQUIRE(quicksim_params.phys_params.mu == -0.32);

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
                   Catch::Matchers::WithinAbs(0.47982940640, fiction::physical_constants::POP_STABILITY_ERR));
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

TEMPLATE_TEST_CASE("QuickSim simulation of a Y-shape SiDB arrangement with varying thread counts", "[quicksim]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

    lyt.assign_cell_type({-11, -2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({-10, -1, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({-4, -1, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({-3, -2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({-7, 0, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({-7, 1, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({-7, 3, 0}, TestType::cell_type::NORMAL);

    const sidb_simulation_parameters params{2, -0.32};

    quicksim_params quicksim_params{params};

    REQUIRE(quicksim_params.phys_params.mu == -0.32);

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
                   Catch::Matchers::WithinAbs(0.3191504062951, fiction::physical_constants::POP_STABILITY_ERR));
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

TEMPLATE_TEST_CASE("QuickSim simulation of a Y-shape SiDB OR gate with input 01 and varying thread counts",
                   "[quicksim]", (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

    lyt.assign_cell_type({6, 2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({12, 3, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({14, 2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 5, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({10, 6, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 8, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({16, 1, 0}, TestType::cell_type::NORMAL);

    sidb_simulation_result<TestType> quicksimstats{};
    const sidb_simulation_parameters params{2, -0.28};

    quicksim_params quicksim_params{params};

    REQUIRE(quicksim_params.phys_params.mu == -0.28);

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
                   Catch::Matchers::WithinAbs(0.46621669, fiction::physical_constants::POP_STABILITY_ERR));
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
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

    lyt.assign_cell_type({6, 2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({8, 2, 0}, TestType::cell_type::NORMAL);

    const sidb_simulation_result<TestType> quicksimstats{};
    const sidb_simulation_parameters       params{2, -0.25};

    quicksim_params quicksim_params{params};

    REQUIRE(quicksim_params.phys_params.mu == -0.25);

    const auto check_charge_configuration = [](const sidb_simulation_result<TestType>& stats) noexcept
    {
        REQUIRE(!stats.charge_distributions.empty());

        REQUIRE(!energy_distribution(stats.charge_distributions).empty());

        const auto& charge_lyt_first = stats.charge_distributions.front();

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
