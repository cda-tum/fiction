//
// Created by Jan Drewniok on 18.12.22.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/simulation/sidb/quicksim.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/physical_constants.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE(
    "quicksim test", "[quicksim]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_row_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, odd_column_hex>>>),
    (cell_level_layout<sidb_technology, clocked_layout<hexagonal_layout<siqad::coord_t, even_column_hex>>>))
{
    SECTION("layout with no SiDB placed")
    {
        TestType lyt{{20, 10}};

        quicksim_stats<TestType> quicksimstats{};
        const quicksim_params    quicksim_params{sidb_simulation_parameters{2, -0.30}};
        CHECK(quicksim_params.phys_params.mu == -0.30);
        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        CHECK(quicksimstats.valid_lyts.empty());
    }

    SECTION("layout with one SiDB placed")
    {
        TestType lyt{{20, 10}};

        lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);

        quicksim_stats<TestType> quicksimstats{};
        const quicksim_params    quicksim_params{sidb_simulation_parameters{2, -0.30}};
        CHECK(quicksim_params.phys_params.mu == -0.30);
        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        CHECK(quicksimstats.valid_lyts.size() == quicksim_params.interation_steps + 1);
    }

    SECTION("layout with several SiDBs placed")
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
        const quicksim_params            quicksim_params{params};
        CHECK(quicksim_params.phys_params.mu == -0.30);
        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        CHECK(!quicksimstats.valid_lyts.empty());

        for (const auto& it : quicksimstats.valid_lyts)
        {
            CHECK(!it.charge_exists(sidb_charge_state::POSITIVE));
        }
    }

    SECTION("zero threads")
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
        const quicksim_params            quicksim_params{params, 80, 0.7, 0};
        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);
        CHECK(!quicksimstats.valid_lyts.empty());
        CHECK(quicksimstats.time_total.count() > 0);
    }

    SECTION("one thread")
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
        const quicksim_params            quicksim_params{params, 80, 0.7, 1};
        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);
        CHECK(!quicksimstats.valid_lyts.empty());
        CHECK(quicksimstats.time_total.count() > 0);
    }

    SECTION("two threads")
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
        const quicksim_params            quicksim_params{params, 80, 0.7, 2};
        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);
        CHECK(!quicksimstats.valid_lyts.empty());
        CHECK(quicksimstats.time_total.count() > 0);
    }

    SECTION("100 threads")
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
        const quicksim_params            quicksim_params{params, 80, 0.7, 100};
        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);
        CHECK(!quicksimstats.valid_lyts.empty());
        CHECK(quicksimstats.time_total.count() > 0);
    }

    SECTION("simulation of SiQAD-OR gate with input 01 | one thread")
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
        const quicksim_params            quicksim_params{params, 80, 0.7, 1};
        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        REQUIRE(!quicksimstats.valid_lyts.empty());
        const auto& charge_lyt_first = quicksimstats.valid_lyts.front();

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
    }

    SECTION("simulation of SiQAD-OR gate with input 01 | zero threads")
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
        const quicksim_params            quicksim_params{params, 80, 0.7, 0};
        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        REQUIRE(!quicksimstats.valid_lyts.empty());
        const auto& charge_lyt_first = quicksimstats.valid_lyts.front();

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
    }

    SECTION("simulation of SiQAD-OR gate with input 01 | 100 threads")
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
        const quicksim_params            quicksim_params{params, 80, 0.7, 100};
        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        REQUIRE(!quicksimstats.valid_lyts.empty());
        const auto& charge_lyt_first = quicksimstats.valid_lyts.front();

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
    }

    SECTION("simulation of 2 SiDBs | 1 threads")
    {
        TestType lyt{{20, 10}};

        lyt.assign_cell_type({6, 2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({8, 2, 0}, TestType::cell_type::NORMAL);

        quicksim_stats<TestType>         quicksimstats{};
        const sidb_simulation_parameters params{2, -0.25};
        const quicksim_params            quicksim_params{params, 80, 0.7, 1};
        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        REQUIRE(!quicksimstats.valid_lyts.empty());
        const auto& charge_lyt_first = quicksimstats.valid_lyts.front();

        auto distribution = energy_distribution(quicksimstats.valid_lyts);
        REQUIRE(!distribution.empty());

        CHECK(((charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE) &&
               (charge_lyt_first.get_charge_state({8, 2, 0}) == sidb_charge_state::NEUTRAL)) ^
              ((charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEUTRAL) &&
               (charge_lyt_first.get_charge_state({8, 2, 0}) == sidb_charge_state::NEGATIVE)));
    }

    SECTION("simulation of 2 SiDBs | 100 threads")
    {
        TestType lyt{{20, 10}};

        lyt.assign_cell_type({6, 2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({8, 2, 0}, TestType::cell_type::NORMAL);

        quicksim_stats<TestType>         quicksimstats{};
        const sidb_simulation_parameters params{2, -0.25};
        const quicksim_params            quicksim_params{params, 80, 0.7, 100};
        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        REQUIRE(!quicksimstats.valid_lyts.empty());
        const auto& charge_lyt_first = quicksimstats.valid_lyts.front();

        auto distribution = energy_distribution(quicksimstats.valid_lyts);
        REQUIRE(!distribution.empty());

        CHECK(((charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE) &&
               (charge_lyt_first.get_charge_state({8, 2, 0}) == sidb_charge_state::NEUTRAL)) ^
              ((charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEUTRAL) &&
               (charge_lyt_first.get_charge_state({8, 2, 0}) == sidb_charge_state::NEGATIVE)));
    }

    SECTION("simulation of 2 SiDBs | 1 thread but 0 to initialize")
    {
        TestType lyt{{20, 10}};

        lyt.assign_cell_type({6, 2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({8, 2, 0}, TestType::cell_type::NORMAL);

        quicksim_stats<TestType>         quicksimstats{};
        const sidb_simulation_parameters params{2, -0.25};
        const quicksim_params            quicksim_params{params, 80, 0.7, 0};
        quicksim<TestType>(lyt, quicksim_params, &quicksimstats);

        REQUIRE(!quicksimstats.valid_lyts.empty());
        const auto& charge_lyt_first = quicksimstats.valid_lyts.front();

        auto distribution = energy_distribution(quicksimstats.valid_lyts);
        REQUIRE(!distribution.empty());

        CHECK(((charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE) &&
               (charge_lyt_first.get_charge_state({8, 2, 0}) == sidb_charge_state::NEUTRAL)) ^
              ((charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEUTRAL) &&
               (charge_lyt_first.get_charge_state({8, 2, 0}) == sidb_charge_state::NEGATIVE)));
    }
}
