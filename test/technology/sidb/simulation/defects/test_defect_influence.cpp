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
// Created by Jan Drewniok on 07.02.24.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/layout_utils.hpp>
#include <fiction/synthesis/truth_tables.hpp>
#include <fiction/technology/sidb/model/defect.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/defects/defect_clearance.hpp>
#include <fiction/technology/sidb/simulation/defects/defect_influence.hpp>
#include <fiction/technology/sidb/simulation/logic/is_operational.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/math/math_utils.hpp>

#include <vector>

using namespace fiction;

TEMPLATE_TEST_CASE("novel designed AND Gate influence distance function which fails again", "[defect-influence]",
                   sidb_cell_clk_lyt_cube)
{
    const auto lyt = blueprints::bestagon_and<sidb_cell_clk_lyt_siqad>();

    const auto cube_lyt = layouts::convert_layout_to_fiction_coordinates<TestType>(lyt);

    const sidb::model::defect defect{sidb::model::defect_type::SI_VACANCY, -1, 10.6, 5.9};

    auto params = sidb::simulation::defects::defect_influence_params<cell<TestType>>{
        defect, sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.32}}};

    // to save runtime in the CI, this test is only run in RELEASE mode
#ifdef NDEBUG
    SECTION("Grid Search")
    {
        params.additional_scanning_area = {20, 20};
        sidb::simulation::defects::defect_influence_stats stats{};
        const auto defect_influence_domain = sidb::simulation::defects::defect_influence_grid_search(
            cube_lyt, std::vector<tt>{synthesis::create_and_tt()}, params, 3, &stats);
        CHECK_THAT(sidb::simulation::defects::calculate_defect_clearance(cube_lyt, defect_influence_domain)
                       .defect_clearance_distance,
                   Catch::Matchers::WithinAbs(5.81097444496187787, utils::math::ERROR_MARGIN));
        CHECK(stats.num_evaluated_defect_positions == 676);
        CHECK(stats.num_non_influencing_defect_positions == 527);
        CHECK(stats.num_influencing_defect_positions == 149);
    }
#endif

    SECTION("Random Sampling")
    {
        params.additional_scanning_area = {20, 20};
        sidb::simulation::defects::defect_influence_stats stats{};
        const auto defect_influence_domain = sidb::simulation::defects::defect_influence_random_sampling(
            cube_lyt, std::vector<tt>{synthesis::create_and_tt()}, 100, params, &stats);
        CHECK(defect_influence_domain.size() > 0);
        CHECK(sidb::simulation::defects::calculate_defect_clearance(cube_lyt, defect_influence_domain)
                  .defect_clearance_distance <= 6.21261176961831474);
    }
}

TEMPLATE_TEST_CASE(
    "Tests for determining the defect influence distance for an AND gate when considering ground state changes",
    "[defect-influence]", sidb_cell_clk_lyt_cube)
{
    const auto lyt      = blueprints::siqad_and_gate<sidb_cell_clk_lyt_siqad>();
    const auto lyt_cube = layouts::convert_layout_to_fiction_coordinates<TestType>(lyt);

    SECTION("Si Vacancy")
    {
        auto params = sidb::simulation::defects::defect_influence_params<cell<TestType>>{
            sidb::model::defect{sidb::model::defect_type::SI_VACANCY, -1, 10.6, 5.9},
            sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.32}},
            {20, 0},
            sidb::simulation::defects::defect_influence_params<
                cell<TestType>>::influence_definition::GROUND_STATE_CHANGE};

        const auto defect_influence_vacancy = sidb::simulation::defects::defect_influence_grid_search(
            lyt_cube, std::vector<tt>{synthesis::create_and_tt()}, params);
        const auto clearance_result_vacancy =
            sidb::simulation::defects::calculate_defect_clearance(lyt_cube, defect_influence_vacancy);

        CHECK(clearance_result_vacancy.defect_position == layouts::coords::cube{18, 17});
        CHECK_THAT(std::abs(clearance_result_vacancy.defect_clearance_distance - 3.1665),
                   Catch::Matchers::WithinAbs(0.00, 0.01));

// to save runtime in the CI, this test is only run in RELEASE mode
#ifdef NDEBUG
        SECTION("Grid Search")
        {
            params.additional_scanning_area = {20, 20};
            sidb::simulation::defects::defect_influence_stats stats{};
            const auto defect_influence_domain = sidb::simulation::defects::defect_influence_grid_search(
                lyt_cube, std::vector<tt>{synthesis::create_and_tt()}, params, 1, &stats);
            const auto defect_clearance =
                sidb::simulation::defects::calculate_defect_clearance(lyt_cube, defect_influence_domain);
            CHECK_THAT(sidb::simulation::defects::calculate_defect_clearance(lyt_cube, defect_influence_domain)
                           .defect_clearance_distance,
                       Catch::Matchers::WithinAbs(3.16654512047436443, utils::math::ERROR_MARGIN));
            CHECK(stats.num_evaluated_defect_positions == 3599);
            CHECK(stats.num_non_influencing_defect_positions == 3062);
            CHECK(stats.num_influencing_defect_positions == 537);
            CHECK(defect_clearance.defect_position == layouts::coords::cube{18, 17});
        }
#endif

        SECTION("Random Sampling")
        {
            params.additional_scanning_area = {20, 20};
            sidb::simulation::defects::defect_influence_stats stats{};
            const auto defect_influence_domain = sidb::simulation::defects::defect_influence_random_sampling(
                lyt_cube, std::vector<tt>{synthesis::create_and_tt()}, 100, params, &stats);
            CHECK(defect_influence_domain.size() > 0);
            CHECK(sidb::simulation::defects::calculate_defect_clearance(lyt_cube, defect_influence_domain)
                      .defect_clearance_distance <= 3.16654512047436443);
        }

        SECTION("QuickTrace")
        {
            // 3.16654512047436443 nm is the exact value.
            params.additional_scanning_area = {20, 20};
            sidb::simulation::defects::defect_influence_stats stats{};
            const auto defect_influence_domain = sidb::simulation::defects::defect_influence_quicktrace(
                lyt_cube, std::vector<tt>{synthesis::create_and_tt()}, 20, params, &stats);
            CHECK_THAT(sidb::simulation::defects::calculate_defect_clearance(lyt_cube, defect_influence_domain)
                           .defect_clearance_distance,
                       Catch::Matchers::WithinAbs(3.16654512047436443, utils::math::ERROR_MARGIN));
        }

        params.influence_def = sidb::simulation::defects::defect_influence_params<
            cell<TestType>>::influence_definition::GROUND_STATE_CHANGE;

        SECTION("Grid search, considering a change in the ground state as influence")
        {
            const auto defect_operational_domain =
                sidb::simulation::defects::defect_influence_grid_search(lyt_cube, params);

            const auto clearance_result =
                sidb::simulation::defects::calculate_defect_clearance(lyt_cube, defect_operational_domain);

            CHECK_THAT(fiction::utils::math::round_to_n_decimal_places(clearance_result.defect_clearance_distance, 6),
                       Catch::Matchers::WithinAbs(2.76906300000000005, utils::math::ERROR_MARGIN));
        }

        SECTION("QuickTrace, considering a change in the ground state as influence")
        {
            const auto defect_operational_domain =
                sidb::simulation::defects::defect_influence_quicktrace(lyt_cube, 20, params);

            const auto clearance_result =
                sidb::simulation::defects::calculate_defect_clearance(lyt_cube, defect_operational_domain);

            CHECK_THAT(fiction::utils::math::round_to_n_decimal_places(clearance_result.defect_clearance_distance, 6),
                       Catch::Matchers::WithinAbs(2.76906300000000005, utils::math::ERROR_MARGIN));
        }

        SECTION("Random Sampling, considering a change in the ground state as influence")
        {
            const auto defect_operational_domain =
                sidb::simulation::defects::defect_influence_random_sampling(lyt_cube, 10, params);

            const auto clearance_result =
                sidb::simulation::defects::calculate_defect_clearance(lyt_cube, defect_operational_domain);

            CHECK(clearance_result.defect_clearance_distance <= 4.85725799999999985);
        }
    }

    SECTION("Arsenic Defect")
    {
        const auto defect_operational_arsenic_params =
            sidb::simulation::defects::defect_influence_params<cell<TestType>>{
                sidb::model::defect{sidb::model::defect_type::UNKNOWN, 1, 9.7, 2.1},
                sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.32}},
                {10, 0},
                sidb::simulation::defects::defect_influence_params<
                    cell<TestType>>::influence_definition::GROUND_STATE_CHANGE};

        const auto defect_influence_arsenic = sidb::simulation::defects::defect_influence_grid_search(
            lyt_cube, std::vector<tt>{synthesis::create_and_tt()}, defect_operational_arsenic_params);
        const auto clearance_result_arsenic =
            sidb::simulation::defects::calculate_defect_clearance(lyt_cube, defect_influence_arsenic);

        CHECK((((clearance_result_arsenic.defect_position == layouts::coords::cube{17, 12, 0})) ||
               (clearance_result_arsenic.defect_position == layouts::coords::cube{3, 12, 0})));
        CHECK_THAT(std::abs(clearance_result_arsenic.defect_clearance_distance - 2.376),
                   Catch::Matchers::WithinAbs(0.00, 0.01));
    }
}

TEMPLATE_TEST_CASE("Defect influence when considering the change of the ground state", "[defect-influence]",
                   sidb_cell_clk_lyt_cube, cds_sidb_100_cell_clk_lyt_cube)
{
    SECTION("layout with one SiDB")
    {
        const auto defect_operational_params = sidb::simulation::defects::defect_influence_params<cell<TestType>>{
            sidb::model::defect{sidb::model::defect_type::UNKNOWN, -1, sidb::model::simulation_parameters{}.epsilon_r,
                                sidb::model::simulation_parameters{}.lambda_tf},
            sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.32}},
            cell<TestType>{2, 2},
            sidb::simulation::defects::defect_influence_params<
                cell<TestType>>::influence_definition::GROUND_STATE_CHANGE};

        TestType lyt{};

        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);

        SECTION("Grid Search")
        {
            const auto defect_operational_domain =
                sidb::simulation::defects::defect_influence_grid_search(lyt, defect_operational_params);

            const auto clearance_result =
                sidb::simulation::defects::calculate_defect_clearance(lyt, defect_operational_domain);

            CHECK_THAT(fiction::utils::math::round_to_n_decimal_places(clearance_result.defect_clearance_distance, 6),
                       Catch::Matchers::WithinAbs(0.665060, utils::math::ERROR_MARGIN));
            CHECK((((clearance_result.defect_position.x == -1) && (clearance_result.defect_position.y == -1) ||
                    ((clearance_result.defect_position.x == 1) && (clearance_result.defect_position.y == -1)))));
        }
    }

    SECTION("layout with one SiDB, negative defect, smaller lambda_tf")
    {
        const auto defect_operational_params = sidb::simulation::defects::defect_influence_params<cell<TestType>>{
            sidb::model::defect{sidb::model::defect_type::UNKNOWN, -1, sidb::model::simulation_parameters{}.epsilon_r,
                                sidb::model::simulation_parameters{}.lambda_tf},
            sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.32}},
            {2, 2},
            sidb::simulation::defects::defect_influence_params<
                cell<TestType>>::influence_definition::GROUND_STATE_CHANGE};

        TestType lyt{};
        lyt.assign_cell_type({0, 0, 0}, sidb_cell_clk_lyt_cube::cell_type::NORMAL);

        const auto defect_operational_domain =
            sidb::simulation::defects::defect_influence_grid_search(lyt, defect_operational_params);
        const auto defect_clearance =
            sidb::simulation::defects::calculate_defect_clearance(lyt, defect_operational_domain);

        CHECK_THAT(
            fiction::utils::math::round_to_n_decimal_places(defect_clearance.defect_clearance_distance, 4) -
                fiction::utils::math::round_to_n_decimal_places(sidb::model::nm_distance(lyt, {0, 0}, {1, -1}), 4),
            Catch::Matchers::WithinAbs(0.0, utils::math::ERROR_MARGIN));
    }

    SECTION("layout with one SiDB, negative defect, large lambda_tf")
    {
        const auto defect_operational_params = sidb::simulation::defects::defect_influence_params<cell<TestType>>{
            sidb::model::defect{sidb::model::defect_type::UNKNOWN, -1, sidb::model::simulation_parameters{}.epsilon_r,
                                20},
            sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.32}},
            {30, 30},
            sidb::simulation::defects::defect_influence_params<
                cell<TestType>>::influence_definition::GROUND_STATE_CHANGE};

        TestType lyt{};
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        const TestType lat{lyt};

        const auto defect_operational_domain =
            sidb::simulation::defects::defect_influence_grid_search(lat, defect_operational_params);

        const auto defect_clearance =
            sidb::simulation::defects::calculate_defect_clearance(lat, defect_operational_domain);

        CHECK_THAT(
            fiction::utils::math::round_to_n_decimal_places(defect_clearance.defect_clearance_distance, 4) -
                fiction::utils::math::round_to_n_decimal_places(sidb::model::nm_distance(lat, {0, 0, 0}, {0, 2, 0}), 4),
            Catch::Matchers::WithinAbs(0.0, utils::math::ERROR_MARGIN));
    }

    SECTION("layout with one pertuber and one DB pair, negative defect")
    {
        const auto defect_operational_params = sidb::simulation::defects::defect_influence_params<cell<TestType>>{
            sidb::model::defect{sidb::model::defect_type::UNKNOWN, -1, sidb::model::simulation_parameters{}.epsilon_r,
                                sidb::model::simulation_parameters{}.lambda_tf},
            sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.32}},
            {20, 0},
            sidb::simulation::defects::defect_influence_params<
                cell<TestType>>::influence_definition::GROUND_STATE_CHANGE};

        TestType lyt{};

        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({4, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({6, 0, 0}, TestType::cell_type::NORMAL);

        const TestType lat{lyt};

        const auto defect_operational_domain =
            sidb::simulation::defects::defect_influence_grid_search(lat, defect_operational_params);
        const auto defect_clearance =
            sidb::simulation::defects::calculate_defect_clearance(lat, defect_operational_domain);

        CHECK_THAT(fiction::utils::math::round_to_n_decimal_places(defect_clearance.defect_clearance_distance, 4) -
                       fiction::utils::math::round_to_n_decimal_places(
                           sidb::model::nm_distance(lat, {6, 0, 0}, {10, 0, 0}), 4),
                   Catch::Matchers::WithinAbs(0.0, utils::math::ERROR_MARGIN));
    }

    SECTION("QuickExact simulation of a Y-shape SiDB OR gate with input 01")
    {
        auto defect_operational_params = sidb::simulation::defects::defect_influence_params<cell<TestType>>{
            sidb::model::defect{sidb::model::defect_type::UNKNOWN, -1, sidb::model::simulation_parameters{}.epsilon_r,
                                sidb::model::simulation_parameters{}.lambda_tf},
            sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.32}},
            {20, 20},
            sidb::simulation::defects::defect_influence_params<
                cell<TestType>>::influence_definition::GROUND_STATE_CHANGE};

        TestType lyt{};

        lyt.assign_cell_type({10, 0}, sidb_cell_clk_lyt_cube::cell_type::NORMAL);
        lyt.assign_cell_type({0, 2}, sidb_cell_clk_lyt_cube::cell_type::NORMAL);
        lyt.assign_cell_type({8, 2}, sidb_cell_clk_lyt_cube::cell_type::NORMAL);

        lyt.assign_cell_type({2, 4}, sidb_cell_clk_lyt_cube::cell_type::NORMAL);
        lyt.assign_cell_type({6, 4}, sidb_cell_clk_lyt_cube::cell_type::NORMAL);
        lyt.assign_cell_type({4, 8}, sidb_cell_clk_lyt_cube::cell_type::NORMAL);
        lyt.assign_cell_type({4, 11}, sidb_cell_clk_lyt_cube::cell_type::NORMAL);
        lyt.assign_cell_type({4, 15}, sidb_cell_clk_lyt_cube::cell_type::NORMAL);

        const sidb_100_cell_clk_lyt_cube lat{lyt};

        const auto defect_operational_domain =
            sidb::simulation::defects::defect_influence_grid_search(lat, defect_operational_params);
        const auto defect_clearance =
            sidb::simulation::defects::calculate_defect_clearance(lat, defect_operational_domain);

        CHECK(defect_clearance.defect_position.x == 12);
        CHECK(defect_clearance.defect_position.y == 9);

        CHECK_THAT(defect_clearance.defect_clearance_distance,
                   Catch::Matchers::WithinAbs(2.8999201713, utils::math::ERROR_MARGIN));

        // high screening
        defect_operational_params.defect = sidb::model::defect{sidb::model::defect_type::UNKNOWN, -1,
                                                               sidb::model::simulation_parameters{}.epsilon_r, 1};

        const auto defect_operational_domain_high_screening =
            sidb::simulation::defects::defect_influence_grid_search(lat, defect_operational_params);
        const auto defect_clearance_high_screening =
            sidb::simulation::defects::calculate_defect_clearance(lat, defect_operational_domain_high_screening);

        CHECK(defect_clearance_high_screening.defect_clearance_distance < defect_clearance.defect_clearance_distance);
    }

    SECTION("QuickExact simulation of a Y-shape SiDB OR gate with input 01, using cube coordinate")
    {
        const auto defect_operational_params = sidb::simulation::defects::defect_influence_params<cell<TestType>>{
            sidb::model::defect{sidb::model::defect_type::UNKNOWN, -1, sidb::model::simulation_parameters{}.epsilon_r,
                                sidb::model::simulation_parameters{}.lambda_tf},
            sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.32}},
            {30, 30},
            sidb::simulation::defects::defect_influence_params<
                cell<TestType>>::influence_definition::GROUND_STATE_CHANGE};

        sidb_cell_clk_lyt_cube lyt{{30, 30}};

        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{10, 0, 0}),
                             sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{0, 1, 0}),
                             sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{8, 1, 0}),
                             sidb_cell_clk_lyt_siqad::cell_type::NORMAL);

        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{2, 2, 0}),
                             sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{6, 2, 0}),
                             sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{4, 4, 0}),
                             sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{4, 5, 1}),
                             sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{4, 7, 1}),
                             sidb_cell_clk_lyt_siqad::cell_type::NORMAL);

        const sidb_100_cell_clk_lyt_cube lat{lyt};

        const auto defect_operational_domain =
            sidb::simulation::defects::defect_influence_grid_search(lat, defect_operational_params);
        const auto clearance_result =
            sidb::simulation::defects::calculate_defect_clearance(lat, defect_operational_domain);

        CHECK(clearance_result.defect_position.x == 12);
        CHECK(clearance_result.defect_position.y == 9);
        CHECK(clearance_result.defect_position.z == 0);

        CHECK_THAT(clearance_result.defect_clearance_distance,
                   Catch::Matchers::WithinAbs(2.8999201713, utils::math::ERROR_MARGIN));
    }
}
