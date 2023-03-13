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

    quicksim_stats<TestType> quicksimstats{};
    const quicksim_params    quicksim_params{sidb_simulation_parameters{2, -0.30}};

    REQUIRE(quicksim_params.phys_params.mu == -0.30);

    quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

    CHECK(quicksimstats.valid_lyts.empty());
}

TEMPLATE_TEST_CASE("Single SiDB QuickSim simulation", "[quicksim]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);

    quicksim_stats<TestType> quicksimstats{};
    const quicksim_params    quicksim_params{sidb_simulation_parameters{2, -0.30}};

    REQUIRE(quicksim_params.phys_params.mu == -0.30);

    quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

    CHECK(quicksimstats.valid_lyts.size() == quicksim_params.interation_steps + 1);
}

template <typename Lyt>
void check_for_absence_of_positive_charges(const quicksim_stats<Lyt>& stats) noexcept
{
    REQUIRE(!stats.valid_lyts.empty());

    for (const auto& lyt : stats.valid_lyts)
    {
        CHECK(!lyt.charge_exists(sidb_charge_state::POSITIVE));
    }
}

template <typename Lyt>
void check_for_runtime_measurement(const quicksim_stats<Lyt>& stats) noexcept
{
    CHECK(stats.time_total.count() > 0);
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

    quicksim_stats<TestType>         quicksimstats{};
    const sidb_simulation_parameters params{2, -0.30};

    quicksim_params quicksim_params{params};

    REQUIRE(quicksim_params.phys_params.mu == -0.30);

    SECTION("Default settings")
    {
        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        check_for_absence_of_positive_charges(quicksimstats);
        check_for_runtime_measurement(quicksimstats);
    }
    SECTION("0 threads")
    {
        quicksim_params.number_threads = 0;

        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        check_for_absence_of_positive_charges(quicksimstats);
        check_for_runtime_measurement(quicksimstats);
    }
    SECTION("1 thread")
    {
        quicksim_params.number_threads = 1;

        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        check_for_absence_of_positive_charges(quicksimstats);
        check_for_runtime_measurement(quicksimstats);
    }
    SECTION("2 threads")
    {
        quicksim_params.number_threads = 2;

        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        check_for_absence_of_positive_charges(quicksimstats);
        check_for_runtime_measurement(quicksimstats);
    }
    SECTION("100 threads")
    {
        quicksim_params.number_threads = 100;

        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        check_for_absence_of_positive_charges(quicksimstats);
        check_for_runtime_measurement(quicksimstats);
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

    quicksim_stats<TestType>         quicksimstats{};
    const sidb_simulation_parameters params{2, -0.32};

    quicksim_params quicksim_params{params};

    REQUIRE(quicksim_params.phys_params.mu == -0.32);

    const auto check_charge_configuration = [](const quicksim_stats<TestType>& stats) noexcept
    {
        REQUIRE(!stats.valid_lyts.empty());

        const auto& charge_lyt_first = stats.valid_lyts.front();

        CHECK(charge_lyt_first.get_charge_state({-11, -2, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({-10, -1, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({-3, -2, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({-4, -1, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({-7, 0, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({-7, 1, 1}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({-7, 3, 0}) == sidb_charge_state::NEGATIVE);

        CHECK_THAT(charge_lyt_first.get_system_energy(),
                   Catch::Matchers::WithinAbs(0.31915040629512115, fiction::physical_constants::POP_STABILITY_ERR));
    };

    SECTION("Default settings")
    {
        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        check_for_absence_of_positive_charges(quicksimstats);
        check_for_runtime_measurement(quicksimstats);
        check_charge_configuration(quicksimstats);
    }
    SECTION("0 threads")
    {
        quicksim_params.number_threads = 0;

        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        check_for_absence_of_positive_charges(quicksimstats);
        check_for_runtime_measurement(quicksimstats);
        check_charge_configuration(quicksimstats);
    }
    SECTION("1 thread")
    {
        quicksim_params.number_threads = 1;

        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        check_for_absence_of_positive_charges(quicksimstats);
        check_for_runtime_measurement(quicksimstats);
        check_charge_configuration(quicksimstats);
    }
    SECTION("2 threads")
    {
        quicksim_params.number_threads = 2;

        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        check_for_absence_of_positive_charges(quicksimstats);
        check_for_runtime_measurement(quicksimstats);
        check_charge_configuration(quicksimstats);
    }
    SECTION("100 threads")
    {
        quicksim_params.number_threads = 100;

        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        check_for_absence_of_positive_charges(quicksimstats);
        check_for_runtime_measurement(quicksimstats);
        check_charge_configuration(quicksimstats);
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

    quicksim_stats<TestType>         quicksimstats{};
    const sidb_simulation_parameters params{2, -0.28};

    quicksim_params quicksim_params{params};

    REQUIRE(quicksim_params.phys_params.mu == -0.28);

    const auto check_charge_configuration = [](const quicksim_stats<TestType>& stats) noexcept
    {
        REQUIRE(!stats.valid_lyts.empty());

        const auto& charge_lyt_first = stats.valid_lyts.front();

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
        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        check_for_absence_of_positive_charges(quicksimstats);
        check_for_runtime_measurement(quicksimstats);
        check_charge_configuration(quicksimstats);
    }
    SECTION("0 threads")
    {
        quicksim_params.number_threads = 0;

        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        check_for_absence_of_positive_charges(quicksimstats);
        check_for_runtime_measurement(quicksimstats);
        check_charge_configuration(quicksimstats);
    }
    SECTION("1 thread")
    {
        quicksim_params.number_threads = 1;

        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        check_for_absence_of_positive_charges(quicksimstats);
        check_for_runtime_measurement(quicksimstats);
        check_charge_configuration(quicksimstats);
    }
    SECTION("2 threads")
    {
        quicksim_params.number_threads = 2;

        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        check_for_absence_of_positive_charges(quicksimstats);
        check_for_runtime_measurement(quicksimstats);
        check_charge_configuration(quicksimstats);
    }
    SECTION("100 threads")
    {
        quicksim_params.number_threads = 100;

        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        check_for_absence_of_positive_charges(quicksimstats);
        check_for_runtime_measurement(quicksimstats);
        check_charge_configuration(quicksimstats);
    }
}

TEMPLATE_TEST_CASE("QuickSim simulation of an SiDB BDL pair with varying thread counts", "[quicksim]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

    lyt.assign_cell_type({6, 2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({8, 2, 0}, TestType::cell_type::NORMAL);

    quicksim_stats<TestType>         quicksimstats{};
    const sidb_simulation_parameters params{2, -0.25};

    quicksim_params quicksim_params{params};

    REQUIRE(quicksim_params.phys_params.mu == -0.25);

    const auto check_charge_configuration = [](const quicksim_stats<TestType>& stats) noexcept
    {
        REQUIRE(!stats.valid_lyts.empty());

        REQUIRE(!energy_distribution(stats.valid_lyts).empty());

        const auto& charge_lyt_first = stats.valid_lyts.front();

        CHECK((((charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE) &&
                (charge_lyt_first.get_charge_state({8, 2, 0}) == sidb_charge_state::NEUTRAL)) ||
               ((charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEUTRAL) &&
                (charge_lyt_first.get_charge_state({8, 2, 0}) == sidb_charge_state::NEGATIVE))));
    };

    SECTION("Default settings")
    {
        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        check_for_absence_of_positive_charges(quicksimstats);
        check_for_runtime_measurement(quicksimstats);
        check_charge_configuration(quicksimstats);
    }
    SECTION("0 threads")
    {
        quicksim_params.number_threads = 0;

        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        check_for_absence_of_positive_charges(quicksimstats);
        check_for_runtime_measurement(quicksimstats);
        check_charge_configuration(quicksimstats);
    }
    SECTION("1 thread")
    {
        quicksim_params.number_threads = 1;

        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        check_for_absence_of_positive_charges(quicksimstats);
        check_for_runtime_measurement(quicksimstats);
        check_charge_configuration(quicksimstats);
    }
    SECTION("2 threads")
    {
        quicksim_params.number_threads = 2;

        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        check_for_absence_of_positive_charges(quicksimstats);
        check_for_runtime_measurement(quicksimstats);
        check_charge_configuration(quicksimstats);
    }
    SECTION("100 threads")
    {
        quicksim_params.number_threads = 100;

        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        check_for_absence_of_positive_charges(quicksimstats);
        check_for_runtime_measurement(quicksimstats);
        check_charge_configuration(quicksimstats);
    }
}
