//
// Created by marcel on 07.02.23.
//

#include <catch2/catch_template_test_macros.hpp>

#if (FICTION_Z3_SOLVER)

#include <fiction/algorithms/simulation/sidb/exact_ground_state_simulation.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE("Empty SiDB layout", "[exact-sidb-sim]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{};

    exact_ground_state_simulation_stats<TestType> exgs_stats{};

    exact_ground_state_simulation<TestType>(lyt, {}, &exgs_stats);

    CHECK(exgs_stats.valid_lyts.empty());
}

TEMPLATE_TEST_CASE("Single dot SiDB layout", "[exact-sidb-sim]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{2, 2}};

    lyt.assign_cell_type({1, 1, 0}, TestType::cell_type::NORMAL);

    exact_ground_state_simulation_stats<TestType> exgs_stats{};

    exact_ground_state_simulation<TestType>(lyt, {}, &exgs_stats);

    REQUIRE(exgs_stats.valid_lyts.size() == 1);
    CHECK(exgs_stats.valid_lyts[0].get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE("BDL pair plus perturber", "[exact-sidb-sim]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{5, 0}};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({3, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);

    exact_ground_state_simulation_params          exgs_params{};
    exact_ground_state_simulation_stats<TestType> exgs_stats{};

    SECTION("2-state simulation")
    {
        exgs_params.simulation_states = exact_ground_state_simulation_params::simulation_states::TWO;

        exact_ground_state_simulation<TestType>(lyt, exgs_params, &exgs_stats);

        REQUIRE(exgs_stats.valid_lyts.size() == 1);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
    }
    SECTION("3-state simulation")
    {
        exgs_params.simulation_states = exact_ground_state_simulation_params::simulation_states::THREE;

        exact_ground_state_simulation<TestType>(lyt, exgs_params, &exgs_stats);

        REQUIRE(exgs_stats.valid_lyts.size() == 1);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
    }
}

TEMPLATE_TEST_CASE("Simulation with several SiDBs placed", "[exact-sidb-sim]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

    const sidb_simulation_parameters           sidb_params{2, -0.32};
    const exact_ground_state_simulation_params exact_gs_params{sidb_params};

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

    REQUIRE(!exgs_stats.valid_lyts.empty());

    for (const auto& charge_lyt : exgs_stats.valid_lyts)
    {
        CHECK(!charge_lyt.charge_exists(sidb_charge_state::POSITIVE));
    }
}

#else  // FICTION_Z3_SOLVER

TEST_CASE("Exact SiDB ground state simulation", "[exact-sidb-sim]")
{
    CHECK(true);  // workaround for empty test case
}

#endif  // FICTION_Z3_SOLVER
