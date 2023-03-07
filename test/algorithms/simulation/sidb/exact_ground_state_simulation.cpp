//
// Created by marcel on 07.02.23.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/exact_ground_state_simulation.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE("Exhaustive ground state search", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    const sidb_simulation_parameters       sidb_params{2, -0.32};
    const exact_ground_state_simulation_params exact_gs_params{sidb_params};

    SECTION("simulation with several SiDBs placed")
    {
        TestType lyt{{20, 10}};

        lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({4, 3, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({6, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 3, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({6, 10, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 10, 0}, TestType::cell_type::NORMAL);

        exact_ground_state_simulation_stats<TestType> exgs_stats{};

        exact_ground_state_simulation<TestType>(lyt, exact_gs_params, &exgs_stats);
        auto size_before = exgs_stats.valid_lyts.size();

        exact_ground_state_simulation<TestType>(lyt, exact_gs_params, &exgs_stats);
        auto size_after = exgs_stats.valid_lyts.size();

        CHECK(size_before == size_after);

        CHECK(!exgs_stats.valid_lyts.empty());

        for (const auto& charge_lyt : exgs_stats.valid_lyts)
        {
            CHECK(!charge_lyt.charge_exists(sidb_charge_state::POSITIVE));
        }
    }

    SECTION("simulation with one SiDBs placed")
    {
        TestType lyt{{20, 10}};
        lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);

        exact_ground_state_simulation_stats<TestType> exgs_stats{};

        exact_ground_state_simulation<TestType>(lyt, exact_gs_params, &exgs_stats);

        CHECK(exgs_stats.valid_lyts.size() == 1);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
    }

    SECTION("simulation with no SiDBs placed")
    {
        TestType lyt{{20, 10}};

        exact_ground_state_simulation_stats<TestType> exgs_stats{};

        exact_ground_state_simulation<TestType>(lyt, exact_gs_params, &exgs_stats);

        CHECK(exgs_stats.valid_lyts.size() == 0);  // empty layout is saved.
    }
}
