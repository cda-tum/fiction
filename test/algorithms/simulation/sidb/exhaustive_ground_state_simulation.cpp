//
// Created by Jan Drewniok on 18.12.22.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"

#include <fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/constants.hpp>
#include <fiction/technology/sidb_charge_state.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE("Empty layout ExGS simulation", "[exhaustive-ground-state-simulation]",
                   (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};

    const sidb_simulation_parameters params{2, -0.32};

    const auto simulation_results = exhaustive_ground_state_simulation<TestType>(lyt, params);

    CHECK(simulation_results.charge_distributions.empty());
    CHECK(simulation_results.additional_simulation_parameters.empty());
    CHECK(simulation_results.algorithm_name == "ExGS");
    CHECK(simulation_results.additional_simulation_parameters.empty());
}

TEMPLATE_TEST_CASE("Single SiDB ExGS simulation", "[exhaustive-ground-state-simulation]", (sidb_100_cell_clk_lyt_siqad),
                   (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);

    const sidb_simulation_parameters params{2, -0.32};

    const auto simulation_results = exhaustive_ground_state_simulation<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE("ExGS simulation of a one BDL pair with one perturber", "[exhaustive-ground-state-simulation]",
                   (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({4, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({6, 0, 0}, TestType::cell_type::NORMAL);

    const sidb_simulation_parameters params{2, -0.32};

    const auto simulation_results = exhaustive_ground_state_simulation<TestType>(lyt, params);
    CHECK(simulation_results.charge_distributions.size() == 1);
}

TEMPLATE_TEST_CASE("ExGS simulation of a two-pair BDL wire with one perturber, using siqad coordinates",
                   "[exhaustive-ground-state-simulation]", (sidb_100_cell_clk_lyt_siqad),
                   (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({11, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({13, 0, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({17, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({19, 0, 0}, TestType::cell_type::NORMAL);

    const sidb_simulation_parameters params{2, -0.32};

    const auto simulation_results = exhaustive_ground_state_simulation<TestType>(lyt, params);

    const auto size_before = simulation_results.charge_distributions.size();

    const auto simulation_results_after = exhaustive_ground_state_simulation<TestType>(lyt, params);
    auto       size_after               = simulation_results_after.charge_distributions.size();

    CHECK(size_before == 1);
    CHECK(size_after == 1);

    REQUIRE(!simulation_results_after.charge_distributions.empty());

    const auto& charge_lyt_first = simulation_results_after.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({5, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({7, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({11, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({13, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({17, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({19, 0, 0}) == sidb_charge_state::NEGATIVE);

    CHECK_THAT(charge_lyt_first.get_electrostatic_potential_energy(),
               Catch::Matchers::WithinAbs(0.2460493219, constants::ERROR_MARGIN));
}

TEMPLATE_TEST_CASE("ExGS simulation of a two-pair BDL wire with one perturber, using offset coordinates",
                   "[exhaustive-ground-state-simulation]", (sidb_100_cell_clk_lyt), (cds_sidb_100_cell_clk_lyt))
{
    TestType lyt{};

    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{0, 0, 0}),
                         TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{5, 0, 0}),
                         TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{7, 0, 0}),
                         TestType::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{11, 0, 0}),
                         TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{13, 0, 0}),
                         TestType::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{17, 0, 0}),
                         TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{19, 0, 0}),
                         TestType::cell_type::NORMAL);

    const sidb_simulation_parameters params{2, -0.32};

    const auto simulation_results = exhaustive_ground_state_simulation<TestType>(lyt, params);

    const auto size_before = simulation_results.charge_distributions.size();

    const auto simulation_results_after = exhaustive_ground_state_simulation<TestType>(lyt, params);
    auto       size_after               = simulation_results_after.charge_distributions.size();

    CHECK(size_before == 1);
    CHECK(size_after == 1);

    REQUIRE(!simulation_results_after.charge_distributions.empty());

    const auto& charge_lyt_first = simulation_results_after.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{0, 0, 0})) ==
          sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{5, 0, 0})) ==
          sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{7, 0, 0})) ==
          sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{11, 0, 0})) ==
          sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{13, 0, 0})) ==
          sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{17, 0, 0})) ==
          sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{19, 0, 0})) ==
          sidb_charge_state::NEGATIVE);

    CHECK_THAT(charge_lyt_first.get_electrostatic_potential_energy(),
               Catch::Matchers::WithinAbs(0.2460493219, constants::ERROR_MARGIN));
}

TEMPLATE_TEST_CASE("ExGS simulation of a two-pair BDL wire with one perturber, using cube coordinates",
                   "[exhaustive-ground-state-simulation]", (sidb_100_cell_clk_lyt_cube),
                   (cds_sidb_100_cell_clk_lyt_cube))
{
    TestType lyt{};

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{0, 0, 0}), TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{5, 0, 0}), TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{7, 0, 0}), TestType::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{11, 0, 0}), TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{13, 0, 0}), TestType::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{17, 0, 0}), TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{19, 0, 0}), TestType::cell_type::NORMAL);

    const sidb_simulation_parameters params{2, -0.32};

    const auto simulation_results = exhaustive_ground_state_simulation<TestType>(lyt, params);

    const auto size_before = simulation_results.charge_distributions.size();

    const auto simulation_results_after = exhaustive_ground_state_simulation<TestType>(lyt, params);
    auto       size_after               = simulation_results_after.charge_distributions.size();

    CHECK(size_before == 1);
    CHECK(size_after == 1);

    REQUIRE(!simulation_results_after.charge_distributions.empty());

    const auto& charge_lyt_first = simulation_results_after.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{0, 0, 0})) ==
          sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{5, 0, 0})) ==
          sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{7, 0, 0})) ==
          sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{11, 0, 0})) ==
          sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{13, 0, 0})) ==
          sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{17, 0, 0})) ==
          sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{19, 0, 0})) ==
          sidb_charge_state::NEGATIVE);

    CHECK_THAT(charge_lyt_first.get_electrostatic_potential_energy(),
               Catch::Matchers::WithinAbs(0.2460493219, constants::ERROR_MARGIN));
}

TEMPLATE_TEST_CASE("ExGS simulation of a Y-shaped SiDB arrangement", "[exhaustive-ground-state-simulation]",
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

    const auto simulation_results = exhaustive_ground_state_simulation<TestType>(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());

    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({-11, -2, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({-10, -1, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({-3, -2, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({-4, -1, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({-7, 0, 1}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({-7, 1, 1}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({-7, 3, 0}) == sidb_charge_state::NEGATIVE);

    CHECK_THAT(charge_lyt_first.get_electrostatic_potential_energy(),
               Catch::Matchers::WithinAbs(0.3191788254, constants::ERROR_MARGIN));
}

TEMPLATE_TEST_CASE("ExGS simulation of a Y-shaped SiDB OR gate with input 01", "[exhaustive-ground-state-simulation]",
                   (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
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
        const auto simulation_results = exhaustive_ground_state_simulation<TestType>(lyt, params);

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

        CHECK_THAT(charge_lyt_first.get_electrostatic_potential_energy(),
                   Catch::Matchers::WithinAbs(0.4662582096, constants::ERROR_MARGIN));
    }

    SECTION("Increased mu_minus")
    {
        params.mu_minus = -0.1;

        const auto simulation_results = exhaustive_ground_state_simulation<TestType>(lyt, params);

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

        CHECK_THAT(charge_lyt_first.get_electrostatic_potential_energy(),
                   Catch::Matchers::WithinAbs(0.061037632, constants::ERROR_MARGIN));
    }

    SECTION("Decreased mu_minus")
    {
        params.mu_minus = -0.7;

        const auto simulation_results = exhaustive_ground_state_simulation<TestType>(lyt, params);

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

        CHECK_THAT(charge_lyt_first.get_electrostatic_potential_energy(),
                   Catch::Matchers::WithinAbs(2.069954113, constants::ERROR_MARGIN));
    }

    SECTION("Decreased lambda_tf")
    {
        params.lambda_tf = 1;

        const auto simulation_results = exhaustive_ground_state_simulation<TestType>(lyt, params);

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

        CHECK_THAT(charge_lyt_first.get_electrostatic_potential_energy(),
                   Catch::Matchers::WithinAbs(0.5432404075, constants::ERROR_MARGIN));
    }

    SECTION("Increased lambda_tf")
    {
        params.lambda_tf = 10;

        const auto simulation_results = exhaustive_ground_state_simulation<TestType>(lyt, params);

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

        CHECK_THAT(charge_lyt_first.get_electrostatic_potential_energy(),
                   Catch::Matchers::WithinAbs(0.2930574885, constants::ERROR_MARGIN));
    }

    SECTION("Increased epsilon_r")
    {
        params.epsilon_r = 10;

        const auto simulation_results = exhaustive_ground_state_simulation<TestType>(lyt, params);

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

        CHECK_THAT(charge_lyt_first.get_electrostatic_potential_energy(),
                   Catch::Matchers::WithinAbs(0.505173434, constants::ERROR_MARGIN));
    }
}

TEMPLATE_TEST_CASE("ExGS simulation of positively charged SiDBs", "[exhaustive-ground-state-simulation]",
                   sidb_cell_clk_lyt_siqad, sidb_100_cell_clk_lyt_siqad, cds_sidb_100_cell_clk_lyt_siqad)
{
    TestType lyt{};
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({4, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({6, 0, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({11, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({12, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({11, 0, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({12, 0, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({18, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({20, 0, 0}, TestType::cell_type::NORMAL);

    const sidb_simulation_parameters params{3, -0.32};

    const auto simulation_results = exhaustive_ground_state_simulation<TestType>(lyt, params);

    CHECK(simulation_results.charge_distributions.size() == 4);
}

TEMPLATE_TEST_CASE("ExGS gate simulation of Si-111 surface", "[exhaustive-ground-state-simulation]",
                   sidb_111_cell_clk_lyt_siqad, cds_sidb_111_cell_clk_lyt_siqad)
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

    const sidb_simulation_parameters params{2, -0.32};

    const auto simulation_results = exhaustive_ground_state_simulation<TestType>(lyt, params);

    const auto ground_state = simulation_results.groundstates();
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

TEMPLATE_TEST_CASE("7 SiDB layout", "[exhaustive-ground-state-simulation]", (sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};

    lyt.assign_cell_type({-6, 1, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 4, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({4, 6, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({-8, -3, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({-1, -1, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({0, 2, 0}, TestType::cell_type::NORMAL);

    const sidb_simulation_parameters params{2, -0.25};

    const auto simulation_results = exhaustive_ground_state_simulation<TestType>(lyt, params);

    CHECK(simulation_results.charge_distributions.size() == 1);
}
