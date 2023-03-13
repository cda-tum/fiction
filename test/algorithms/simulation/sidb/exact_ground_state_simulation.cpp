//
// Created by marcel on 07.02.23.
//

#include <catch2/catch_template_test_macros.hpp>

#if (FICTION_Z3_SOLVER)

#include <catch2/matchers/catch_matchers_floating_point.hpp>

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
        exgs_params.phys_params.base = 2;

        exact_ground_state_simulation<TestType>(lyt, exgs_params, &exgs_stats);

        REQUIRE(exgs_stats.valid_lyts.size() == 1);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
    }
    SECTION("3-state simulation")
    {
        exgs_params.phys_params.base = 3;

        exact_ground_state_simulation<TestType>(lyt, exgs_params, &exgs_stats);

        REQUIRE(exgs_stats.valid_lyts.size() == 1);
        CHECK(exgs_stats.valid_lyts[0].get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
    }
}

TEMPLATE_TEST_CASE("SiDB OR gate with input 01", "[exact-sidb-sim]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
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

    exact_ground_state_simulation_stats<TestType> exgs_stats{};
    const sidb_simulation_parameters              params{2, -0.28};

    exact_ground_state_simulation<TestType>(lyt, {params}, &exgs_stats);

    const auto& charge_lyt = exgs_stats.valid_lyts[0];

    CHECK(charge_lyt.get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt.get_charge_state({12, 3, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt.get_charge_state({10, 8, 1}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt.get_charge_state({10, 6, 1}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt.get_charge_state({16, 1, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt.get_charge_state({10, 5, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt.get_charge_state({14, 2, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt.get_charge_state({8, 3, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt.get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE);

    CHECK_THAT(charge_lyt.get_system_energy(),
               Catch::Matchers::WithinAbs(0.4662166947, physical_constants::POP_STABILITY_ERR));
}

TEMPLATE_TEST_CASE("SiDB BDL wire with one perturber", "[exact-sidb-sim]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({11, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({13, 0, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({17, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({19, 0, 0}, TestType::cell_type::NORMAL);

    exact_ground_state_simulation_stats<TestType> exgs_stats{};
    const sidb_simulation_parameters              params{2, -0.32};

    exact_ground_state_simulation<TestType>(lyt, {params}, &exgs_stats);

    const auto& charge_lyt = exgs_stats.valid_lyts.front();

    CHECK(charge_lyt.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt.get_charge_state({5, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt.get_charge_state({7, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt.get_charge_state({11, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt.get_charge_state({13, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt.get_charge_state({17, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt.get_charge_state({19, 0, 0}) == sidb_charge_state::NEGATIVE);

    CHECK_THAT(charge_lyt.get_system_energy(),
               Catch::Matchers::WithinAbs(0.24602741408, physical_constants::POP_STABILITY_ERR));
}

#else  // FICTION_Z3_SOLVER

TEST_CASE("Exact SiDB ground state simulation", "[exact-sidb-sim]")
{
    CHECK(true);  // workaround for empty test case
}

#endif  // FICTION_Z3_SOLVER
