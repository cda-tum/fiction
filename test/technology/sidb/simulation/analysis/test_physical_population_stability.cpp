/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

//
// Created by Jan Drewniok on 02.11.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/analysis/physical_population_stability.hpp>
#include <fiction/technology/sidb/surfaces/lattice.hpp>
#include <fiction/technology/sidb/surfaces/lattice_orientations.hpp>
#include <fiction/technology/sidb/technology.hpp>
#include <fiction/types.hpp>

#include <cmath>

using namespace fiction;

TEST_CASE("Single SiDB", "[assess-physical-population-stability]")
{
    sidb_100_cell_clk_lyt_siqad lyt{};
    lyt.assign_cell_type({1, 1, 0}, sidb::sidb_technology::cell_type::NORMAL);

    SECTION("Precision of distance_corresponding_to_potential is two")
    {
        constexpr auto params = sidb::simulation::analysis::physical_population_stability_params{
            sidb::model::simulation_parameters{2, -0.29}, 2};
        const auto result = sidb::simulation::analysis::physical_population_stability(lyt, params);
        REQUIRE(result.size() == 1);
        const auto& population_stability_detail = result[0];
        CHECK(population_stability_detail.critical_cell == layouts::coords::siqad{1, 1, 0});
        REQUIRE(population_stability_detail.transition_potentials.size() == 4);
        CHECK(population_stability_detail.transition_potentials
                  .at(sidb::simulation::analysis::transition_type::NEGATIVE_TO_NEUTRAL)
                  .first == layouts::coords::siqad{1, 1, 0});
        CHECK(population_stability_detail.transition_potentials
                  .at(sidb::simulation::analysis::transition_type::NEGATIVE_TO_NEUTRAL)
                  .second == 0.29);
        CHECK(std::isinf(population_stability_detail.transition_potentials
                             .at(sidb::simulation::analysis::transition_type::NEUTRAL_TO_POSITIVE)
                             .second));
        CHECK(std::isinf(population_stability_detail.transition_potentials
                             .at(sidb::simulation::analysis::transition_type::POSITIVE_TO_NEUTRAL)
                             .second));

        REQUIRE_THAT(population_stability_detail.distance_corresponding_to_potential.at(
                         sidb::simulation::analysis::transition_type::NEGATIVE_TO_NEUTRAL),
                     Catch::Matchers::WithinAbs(0.77, 1e-5));
    }

    SECTION("Precision of distance_corresponding_to_potential is three")
    {
        constexpr auto params = sidb::simulation::analysis::physical_population_stability_params{
            sidb::model::simulation_parameters{2, -0.29}, 3};
        const auto result = sidb::simulation::analysis::physical_population_stability(lyt, params);
        REQUIRE(result.size() == 1);
        const auto& population_stability_detail = result[0];
        REQUIRE_THAT(population_stability_detail.distance_corresponding_to_potential.at(
                         sidb::simulation::analysis::transition_type::NEGATIVE_TO_NEUTRAL),
                     Catch::Matchers::WithinAbs(0.762, 1e-5));
    }
}

TEMPLATE_TEST_CASE("Three SiDBs with positive charge states", "[assess-physical-population-stability]",
                   sidb_cell_clk_lyt_siqad, cds_sidb_cell_clk_lyt_siqad)
{
    TestType lyt{};

    constexpr auto params = sidb::simulation::analysis::physical_population_stability_params{};
    lyt.assign_cell_type({1, 1, 0}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({1, 1, 1}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, 1, 0}, sidb::sidb_technology::cell_type::NORMAL);

    const auto result = sidb::simulation::analysis::physical_population_stability(lyt, params);
    REQUIRE(result.size() == 3);

    SECTION("Check correct energy order")
    {
        CHECK(result[0].system_energy < result[1].system_energy);
        CHECK(result[1].system_energy < result[2].system_energy);
    }

    SECTION("Ground state")
    {
        const auto& population_stability_detail = result[0];
        CHECK(population_stability_detail.critical_cell == layouts::coords::siqad{2, 1, 0});
        CHECK(population_stability_detail.transition_potentials
                  .at(sidb::simulation::analysis::transition_type::POSITIVE_TO_NEUTRAL)
                  .first == layouts::coords::siqad{1, 1, 0});
        CHECK(population_stability_detail.transition_potentials
                  .at(sidb::simulation::analysis::transition_type::NEGATIVE_TO_NEUTRAL)
                  .first == layouts::coords::siqad{2, 1, 0});
        CHECK(population_stability_detail.transition_potentials
                  .at(sidb::simulation::analysis::transition_type::NEGATIVE_TO_NEUTRAL)
                  .second < 0.43);
        CHECK(population_stability_detail.transition_potentials
                  .at(sidb::simulation::analysis::transition_type::POSITIVE_TO_NEUTRAL)
                  .second < 0.81);
        REQUIRE_THAT(population_stability_detail.distance_corresponding_to_potential.at(
                         sidb::simulation::analysis::transition_type::NEGATIVE_TO_NEUTRAL),
                     Catch::Matchers::WithinAbs(0.56, 1e-5));
        REQUIRE_THAT(population_stability_detail.distance_corresponding_to_potential.at(
                         sidb::simulation::analysis::transition_type::POSITIVE_TO_NEUTRAL),
                     Catch::Matchers::WithinAbs(0.31, 1e-5));
    }
    SECTION("1st excited state")
    {
        const auto& population_stability_detail = result[1];
        CHECK(population_stability_detail.critical_cell == layouts::coords::siqad{2, 1, 0});
        REQUIRE_THAT(population_stability_detail.distance_corresponding_to_potential.at(
                         sidb::simulation::analysis::transition_type::NEGATIVE_TO_NEUTRAL),
                     Catch::Matchers::WithinAbs(0.94, 1e-5));
    }

    SECTION("2nd excited state")
    {
        const auto& population_stability_detail = result[2];
        CHECK(population_stability_detail.critical_cell == layouts::coords::siqad{1, 1, 1});
        REQUIRE_THAT(population_stability_detail.distance_corresponding_to_potential.at(
                         sidb::simulation::analysis::transition_type::NEUTRAL_TO_NEGATIVE),
                     Catch::Matchers::WithinAbs(1.01, 1e-5));
    }
}

TEMPLATE_TEST_CASE("Bestagon AND gate", "[assess-physical-population-stability]", sidb_cell_clk_lyt_siqad,
                   cds_sidb_cell_clk_lyt_siqad)
{
    TestType lyt{};

    constexpr auto params = sidb::simulation::analysis::physical_population_stability_params{};

    lyt.assign_cell_type({36, 1, 0}, sidb::sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 0}, sidb::sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({38, 0, 0}, sidb::sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({0, 0, 0}, sidb::sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({23, 9, 0}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({18, 11, 1}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({18, 9, 0}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({19, 8, 0}, sidb::sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({20, 14, 0}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({19, 13, 0}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({26, 16, 0}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({24, 15, 0}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({32, 2, 0}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({30, 3, 0}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({26, 4, 0}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({24, 5, 0}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 4, 0}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 5, 0}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 2, 0}, sidb::sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, sidb::sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({32, 18, 0}, sidb::sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({30, 17, 0}, sidb::sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({36, 19, 0}, sidb::sidb_technology::cell_type::NORMAL);

    TestType lat{lyt};

    SECTION("no input specified")
    {
        const auto result = sidb::simulation::analysis::physical_population_stability(lyt, params);
        REQUIRE(result.size() == 8);
        const auto& population_stability_detail = result.at(0);
        CHECK(population_stability_detail.critical_cell == layouts::coords::siqad{2, 1, 0});
        REQUIRE_THAT(population_stability_detail.distance_corresponding_to_potential.at(
                         sidb::simulation::analysis::transition_type::NEUTRAL_TO_NEGATIVE),
                     Catch::Matchers::WithinAbs(4.79, 1e-5));
    }

    SECTION("input 00")
    {
        lyt.assign_cell_type({36, 1, 0}, sidb::sidb_technology::cell_type::EMPTY);
        lyt.assign_cell_type({2, 1, 0}, sidb::sidb_technology::cell_type::EMPTY);
        const auto result = sidb::simulation::analysis::physical_population_stability(lyt, params);
        REQUIRE(result.size() == 2);
        const auto& population_stability_detail = result[0];
        CHECK(population_stability_detail.critical_cell == layouts::coords::siqad{14, 5, 0});
        CHECK(population_stability_detail.transition_potentials
                  .at(sidb::simulation::analysis::transition_type::NEUTRAL_TO_NEGATIVE)
                  .second < 0.026);
        REQUIRE_THAT(population_stability_detail.distance_corresponding_to_potential.at(
                         sidb::simulation::analysis::transition_type::NEUTRAL_TO_NEGATIVE),
                     Catch::Matchers::WithinAbs(4.32, 1e-5));
    }

    SECTION("input 01")
    {
        lyt.assign_cell_type({36, 1, 0}, sidb::sidb_technology::cell_type::EMPTY);
        lyt.assign_cell_type({0, 0, 0}, sidb::sidb_technology::cell_type::EMPTY);

        const auto result = sidb::simulation::analysis::physical_population_stability(lyt, params);
        REQUIRE(result.size() == 4);
        const auto& population_stability_detail = result[0];
        CHECK(population_stability_detail.critical_cell == layouts::coords::siqad{32, 18, 0});
        CHECK(population_stability_detail.transition_potentials
                  .at(sidb::simulation::analysis::transition_type::NEUTRAL_TO_NEGATIVE)
                  .second < 0.041);
        REQUIRE_THAT(population_stability_detail.distance_corresponding_to_potential.at(
                         sidb::simulation::analysis::transition_type::NEUTRAL_TO_NEGATIVE),
                     Catch::Matchers::WithinAbs(3.3, 1e-5));
    }

    SECTION("input 10")
    {
        lyt.assign_cell_type({38, 0, 0}, sidb::sidb_technology::cell_type::EMPTY);
        lyt.assign_cell_type({0, 0, 0}, sidb::sidb_technology::cell_type::EMPTY);

        const auto result = sidb::simulation::analysis::physical_population_stability(lyt, params);
        REQUIRE(result.size() == 8);
        const auto& population_stability_detail = result[0];
        CHECK(population_stability_detail.critical_cell == layouts::coords::siqad{19, 8, 0});
        CHECK(population_stability_detail.transition_potentials
                  .at(sidb::simulation::analysis::transition_type::NEUTRAL_TO_NEGATIVE)
                  .second < 0.02);
        REQUIRE_THAT(population_stability_detail.distance_corresponding_to_potential.at(
                         sidb::simulation::analysis::transition_type::NEUTRAL_TO_NEGATIVE),
                     Catch::Matchers::WithinAbs(4.87, 1e-5));
    }

    SECTION("input 11")
    {
        lyt.assign_cell_type({36, 1, 0}, sidb::sidb_technology::cell_type::EMPTY);
        lyt.assign_cell_type({2, 1, 0}, sidb::sidb_technology::cell_type::EMPTY);

        const auto result = sidb::simulation::analysis::physical_population_stability(lyt, params);
        REQUIRE(result.size() == 2);
        const auto& population_stability_detail = result[0];
        CHECK(population_stability_detail.critical_cell == layouts::coords::siqad{14, 5, 0});
        CHECK(population_stability_detail.transition_potentials
                  .at(sidb::simulation::analysis::transition_type::NEUTRAL_TO_NEGATIVE)
                  .second < 0.026);
        REQUIRE_THAT(population_stability_detail.distance_corresponding_to_potential.at(
                         sidb::simulation::analysis::transition_type::NEUTRAL_TO_NEGATIVE),
                     Catch::Matchers::WithinAbs(4.32, 1e-5));
    }
}

TEST_CASE("Bestagon CX gate input 11", "[assess-physical-population-stability], [quality]")
{
    constexpr auto population_stability_params = sidb::simulation::analysis::physical_population_stability_params{};

    SECTION("using cube coordinates")
    {
        sidb_cell_clk_lyt_cube lyt{};
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{36, 1, 0}),
                             sidb::sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{2, 1, 0}),
                             sidb::sidb_technology::cell_type::INPUT);

        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{6, 2, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{20, 12, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{8, 3, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{14, 5, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{14, 11, 1}),
                             sidb::sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{12, 4, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{14, 15, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{26, 4, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{14, 9, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{24, 15, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{12, 16, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{18, 9, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{26, 16, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{24, 13, 1}),
                             sidb::sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{24, 5, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{30, 3, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{16, 13, 1}),
                             sidb::sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{32, 2, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{20, 8, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{30, 17, 0}),
                             sidb::sidb_technology::cell_type::OUTPUT);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{6, 18, 0}),
                             sidb::sidb_technology::cell_type::OUTPUT);

        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{32, 18, 0}),
                             sidb::sidb_technology::cell_type::OUTPUT);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{8, 17, 0}),
                             sidb::sidb_technology::cell_type::OUTPUT);

        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{2, 19, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{36, 19, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);

        CHECK(lyt.num_cells() == 27);

        const sidb::surfaces::lattice<sidb::surfaces::lattice_100, sidb_cell_clk_lyt_cube> lat{lyt};

        const auto result = sidb::simulation::analysis::physical_population_stability(lat, population_stability_params);
        REQUIRE(result.size() == 20);
        const auto& population_stability_detail = result[0];
        CHECK(population_stability_detail.transition_potentials
                  .at(sidb::simulation::analysis::transition_type::NEUTRAL_TO_NEGATIVE)
                  .second < 0.01);
        CHECK(population_stability_detail.transition_potentials
                  .at(sidb::simulation::analysis::transition_type::NEUTRAL_TO_POSITIVE)
                  .second < 0.5);
        REQUIRE_THAT(population_stability_detail.distance_corresponding_to_potential.at(
                         sidb::simulation::analysis::transition_type::NEUTRAL_TO_NEGATIVE),
                     Catch::Matchers::WithinAbs(6.88, 1e-5));
    }

    SECTION("using siqad coordinates")
    {
        sidb_100_cell_clk_lyt lyt{};

        constexpr auto params = sidb::simulation::analysis::physical_population_stability_params{};
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{36, 1, 0}),
                             sidb::sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{2, 1, 0}),
                             sidb::sidb_technology::cell_type::INPUT);

        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{6, 2, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{20, 12, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{8, 3, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{14, 5, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{14, 11, 1}),
                             sidb::sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{12, 4, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{14, 15, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{26, 4, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{14, 9, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{24, 15, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{12, 16, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{18, 9, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{26, 16, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{24, 13, 1}),
                             sidb::sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{24, 5, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{30, 3, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{16, 13, 1}),
                             sidb::sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{32, 2, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{20, 8, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{30, 17, 0}),
                             sidb::sidb_technology::cell_type::OUTPUT);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{6, 18, 0}),
                             sidb::sidb_technology::cell_type::OUTPUT);

        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{32, 18, 0}),
                             sidb::sidb_technology::cell_type::OUTPUT);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{8, 17, 0}),
                             sidb::sidb_technology::cell_type::OUTPUT);

        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{2, 19, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::offset>(layouts::coords::siqad{36, 19, 0}),
                             sidb::sidb_technology::cell_type::NORMAL);

        CHECK(lyt.num_cells() == 27);

        const sidb::surfaces::lattice<sidb::surfaces::lattice_100, sidb_100_cell_clk_lyt> lat{lyt};

        const auto result = sidb::simulation::analysis::physical_population_stability(lat, params);
        REQUIRE(result.size() == 20);
        const auto& population_stability_detail = result[0];
        CHECK(population_stability_detail.critical_cell == layouts::coords::offset{14, 18, 0});
        CHECK(population_stability_detail.transition_potentials
                  .at(sidb::simulation::analysis::transition_type::NEUTRAL_TO_NEGATIVE)
                  .second < 0.01);
        REQUIRE_THAT(population_stability_detail.distance_corresponding_to_potential.at(
                         sidb::simulation::analysis::transition_type::NEUTRAL_TO_NEGATIVE),
                     Catch::Matchers::WithinAbs(6.88, 1e-5));
    }
}
