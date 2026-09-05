/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Tests for `fiction/technology/sidb/simulation/defects/defect_influence.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Willem Lambooy (wlambooy)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/synthesis/truth_tables.hpp>
#include <fiction/technology/sidb/cell_level_layout_conversion.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/defect.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/defects/defect_clearance.hpp>
#include <fiction/technology/sidb/simulation/defects/defect_influence.hpp>
#include <fiction/technology/sidb/simulation/logic/is_operational.hpp>
#include <fiction/technology/sidb/technology.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/math/math_utils.hpp>

#include <cmath>
#include <cstdint>
#include <limits>
#include <optional>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation::defects;
using namespace fiction::sidb::simulation::logic;
using namespace fiction::synthesis;
using namespace fiction::utils::math;

TEST_CASE("Defect influence grid-search edge cases", "[defect-influence]")
{
    const auto params =
        defect_influence_params{.influence_def     = defect_influence_params::influence_definition::GROUND_STATE_CHANGE,
                                .number_of_threads = 1};

    SECTION("zero step size")
    {
        CHECK_THROWS_AS(defect_influence_grid_search(layout{}, params, 0), std::invalid_argument);
    }

    SECTION("empty specification")
    {
        CHECK_THROWS_AS(defect_influence_grid_search(layout{}, std::vector<tt>{}, params), std::invalid_argument);
    }

    SECTION("empty layout")
    {
        const auto domain = defect_influence_grid_search(layout{}, params);
        const auto value  = domain.contains({0, 0});

        CHECK(value == std::optional{std::tuple{defect_influence_status::NON_INFLUENTIAL}});
    }

    SECTION("existing defect")
    {
        auto lyt = layout{};
        lyt.assign_cell_type({0, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_defect({1, 0}, defect{defect_type::DB, -1, 5.6, 5.0});

        const auto domain = defect_influence_grid_search(
            lyt,
            defect_influence_params{.defect                   = defect{defect_type::DB, -1, 5.6, 5.0},
                                    .additional_scanning_area = {1, 0},
                                    .influence_def = defect_influence_params::influence_definition::GROUND_STATE_CHANGE,
                                    .number_of_threads = 1});

        CHECK(domain.contains({1, 0}) == std::optional{std::tuple{defect_influence_status::NON_INFLUENTIAL}});
    }

    SECTION("maximum lattice boundary")
    {
        auto lyt = layout{};
        lyt.assign_cell_type({std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max(), 1},
                             sidb_technology::cell_type::NORMAL);

        auto boundary_params                     = params;
        boundary_params.additional_scanning_area = {1, 1};
        const auto domain                        = defect_influence_grid_search(lyt, boundary_params);

        CHECK(domain.size() == 4);
    }

    SECTION("QuickTrace at the maximum lattice column")
    {
        auto lyt = layout{};
        lyt.assign_cell_type({std::numeric_limits<int32_t>::max(), 0}, sidb_technology::cell_type::NORMAL);

        auto boundary_params                     = params;
        boundary_params.additional_scanning_area = {0, 0};
        const auto domain                        = defect_influence_quicktrace(lyt, 1, boundary_params);

        CHECK(domain.size() == 1);
    }

    SECTION("QuickTrace tries every requested sample after an influential left edge")
    {
        auto lyt = layout{};
        lyt.assign_cell_type(site_at_row(0, 0), sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(site_at_row(2, 0), sidb_technology::cell_type::NORMAL);

        auto contour_params                     = params;
        contour_params.defect                   = defect{defect_type::DB, -1, 5.6, 5.0};
        contour_params.additional_scanning_area = {1, 0};

        defect_influence_stats stats{};
        const auto             domain = defect_influence_quicktrace(lyt, 3, contour_params, &stats);

        CHECK(domain.contains(site_at_row(-1, 0)) == std::optional{std::tuple{defect_influence_status::INFLUENTIAL}});
        CHECK(stats.num_evaluated_defect_positions == 3);
        CHECK(stats.num_influencing_defect_positions == 1);
    }

    SECTION("QuickTrace abandons a contour whose backtrack point is occupied")
    {
        auto lyt = layout{};
        lyt.assign_cell_type(site_at_row(0, 0), sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(site_at_row(2, 0), sidb_technology::cell_type::NORMAL);

        auto contour_params                     = params;
        contour_params.defect                   = defect{defect_type::DB, -1, 5.6, 5.0};
        contour_params.additional_scanning_area = {0, 0};

        const auto domain = defect_influence_quicktrace(lyt, 1, contour_params);

        CHECK(domain.contains(site_at_row(1, 0)) == std::optional{std::tuple{defect_influence_status::INFLUENTIAL}});
    }

    SECTION("negative additional scanning area")
    {
        auto lyt = layout{};
        lyt.assign_cell_type({0, 0}, sidb_technology::cell_type::NORMAL);

        auto invalid_params                     = params;
        invalid_params.additional_scanning_area = {-1, 0};

        CHECK_THROWS_AS(defect_influence_quicktrace(lyt, 1, invalid_params), std::invalid_argument);
    }
}

TEST_CASE("novel designed AND Gate influence distance function which fails again", "[defect-influence]")
{
    const auto lyt = to_sidb_layout(blueprints::bestagon_and<sidb_cell_clk_lyt_siqad>());

    const auto& cube_lyt = lyt;

    const defect defect{defect_type::SI_VACANCY, -1, 10.6, 5.9};

    auto params = defect_influence_params{.defect = defect,
                                          .operational_params =
                                              is_operational_params{.sim_params = simulation_parameters{2, -0.32}}};

    // to save runtime in the CI, this test is only run in RELEASE mode
#ifdef NDEBUG
    SECTION("Grid Search")
    {
        params.additional_scanning_area = {20, 20};
        defect_influence_stats stats{};
        const auto             defect_influence_domain =
            defect_influence_grid_search(cube_lyt, std::vector<tt>{create_and_tt()}, params, 3, &stats);
        CHECK_THAT(calculate_defect_clearance(cube_lyt, defect_influence_domain).defect_clearance_distance,
                   Catch::Matchers::WithinAbs(5.81097444496187787, ERROR_MARGIN));
        CHECK(stats.num_evaluated_defect_positions == 676);
        CHECK(stats.num_non_influencing_defect_positions == 527);
        CHECK(stats.num_influencing_defect_positions == 149);
    }
#endif

    SECTION("Random Sampling")
    {
        params.additional_scanning_area = {20, 20};
        defect_influence_stats stats{};
        const auto             defect_influence_domain =
            defect_influence_random_sampling(cube_lyt, std::vector<tt>{create_and_tt()}, 100, params, &stats);
        CHECK(!defect_influence_domain.empty());
        CHECK(calculate_defect_clearance(cube_lyt, defect_influence_domain).defect_clearance_distance <=
              6.21261176961831474);
    }
}

TEST_CASE("Tests for determining the defect influence distance for an AND gate when considering ground state changes",
          "[defect-influence]")
{
    const auto  lyt      = to_sidb_layout(blueprints::siqad_and_gate<sidb_cell_clk_lyt_siqad>());
    const auto& lyt_cube = lyt;

    SECTION("Si Vacancy")
    {
        auto params = defect_influence_params{
            .defect                   = defect{defect_type::SI_VACANCY, -1, 10.6, 5.9},
            .operational_params       = is_operational_params{.sim_params = simulation_parameters{2, -0.32}},
            .additional_scanning_area = {20, 0},
            .influence_def            = defect_influence_params::influence_definition::GROUND_STATE_CHANGE};

        const auto defect_influence_vacancy =
            defect_influence_grid_search(lyt_cube, std::vector<tt>{create_and_tt()}, params);
        const auto clearance_result_vacancy = calculate_defect_clearance(lyt_cube, defect_influence_vacancy);

        CHECK(clearance_result_vacancy.defect_position == site_at_row(18, 17));
        CHECK_THAT(std::abs(clearance_result_vacancy.defect_clearance_distance - 3.1665),
                   Catch::Matchers::WithinAbs(0.00, 0.01));

// to save runtime in the CI, this test is only run in RELEASE mode
#ifdef NDEBUG
        SECTION("Grid Search")
        {
            params.additional_scanning_area = {20, 20};
            defect_influence_stats stats{};
            const auto             defect_influence_domain =
                defect_influence_grid_search(lyt_cube, std::vector<tt>{create_and_tt()}, params, 1, &stats);
            const auto defect_clearance = calculate_defect_clearance(lyt_cube, defect_influence_domain);
            CHECK_THAT(calculate_defect_clearance(lyt_cube, defect_influence_domain).defect_clearance_distance,
                       Catch::Matchers::WithinAbs(3.16654512047436443, ERROR_MARGIN));
            CHECK(stats.num_evaluated_defect_positions == 3599);
            CHECK(stats.num_non_influencing_defect_positions == 3062);
            CHECK(stats.num_influencing_defect_positions == 537);
            CHECK(defect_clearance.defect_position == site_at_row(18, 17));
        }
#endif

        SECTION("Random Sampling")
        {
            params.additional_scanning_area = {20, 20};
            defect_influence_stats stats{};
            const auto             defect_influence_domain =
                defect_influence_random_sampling(lyt_cube, std::vector<tt>{create_and_tt()}, 100, params, &stats);
            CHECK(!defect_influence_domain.empty());
            CHECK(calculate_defect_clearance(lyt_cube, defect_influence_domain).defect_clearance_distance <=
                  3.16654512047436443);
        }

        SECTION("QuickTrace")
        {
            // 3.16654512047436443 nm is the exact value.
            params.additional_scanning_area = {20, 20};
            defect_influence_stats stats{};
            const auto             defect_influence_domain =
                defect_influence_quicktrace(lyt_cube, std::vector<tt>{create_and_tt()}, 20, params, &stats);
            CHECK_THAT(calculate_defect_clearance(lyt_cube, defect_influence_domain).defect_clearance_distance,
                       Catch::Matchers::WithinAbs(3.16654512047436443, ERROR_MARGIN));
        }

        params.influence_def = defect_influence_params::influence_definition::GROUND_STATE_CHANGE;

        SECTION("Grid search, considering a change in the ground state as influence")
        {
            const auto defect_operational_domain = defect_influence_grid_search(lyt_cube, params);

            const auto clearance_result = calculate_defect_clearance(lyt_cube, defect_operational_domain);

            CHECK_THAT(round_to_n_decimal_places(clearance_result.defect_clearance_distance, 6),
                       Catch::Matchers::WithinAbs(2.76906300000000005, ERROR_MARGIN));
        }

        SECTION("QuickTrace, considering a change in the ground state as influence")
        {
            const auto defect_operational_domain = defect_influence_quicktrace(lyt_cube, 20, params);

            const auto clearance_result = calculate_defect_clearance(lyt_cube, defect_operational_domain);

            CHECK_THAT(round_to_n_decimal_places(clearance_result.defect_clearance_distance, 6),
                       Catch::Matchers::WithinAbs(2.76906300000000005, ERROR_MARGIN));
        }

        SECTION("Random Sampling, considering a change in the ground state as influence")
        {
            const auto defect_operational_domain = defect_influence_random_sampling(lyt_cube, 10, params);

            const auto clearance_result = calculate_defect_clearance(lyt_cube, defect_operational_domain);

            CHECK(clearance_result.defect_clearance_distance <= 4.85725799999999985);
        }
    }

    SECTION("Arsenic Defect")
    {
        const auto defect_operational_arsenic_params = defect_influence_params{
            .defect                   = defect{defect_type::UNKNOWN, 1, 9.7, 2.1},
            .operational_params       = is_operational_params{.sim_params = simulation_parameters{2, -0.32}},
            .additional_scanning_area = {10, 0},
            .influence_def            = defect_influence_params::influence_definition::GROUND_STATE_CHANGE};

        const auto defect_influence_arsenic =
            defect_influence_grid_search(lyt_cube, std::vector<tt>{create_and_tt()}, defect_operational_arsenic_params);
        const auto clearance_result_arsenic = calculate_defect_clearance(lyt_cube, defect_influence_arsenic);

        CHECK((((clearance_result_arsenic.defect_position == site_at_row(17, 12))) ||
               (clearance_result_arsenic.defect_position == site_at_row(3, 12))));
        CHECK_THAT(std::abs(clearance_result_arsenic.defect_clearance_distance - 2.376),
                   Catch::Matchers::WithinAbs(0.00, 0.01));
    }
}

TEST_CASE("Defect influence when considering the change of the ground state", "[defect-influence]")
{
    SECTION("layout with one SiDB")
    {
        const auto defect_operational_params = defect_influence_params{
            .defect =
                defect{defect_type::UNKNOWN, -1, simulation_parameters{}.epsilon_r, simulation_parameters{}.lambda_tf},
            .operational_params       = is_operational_params{.sim_params = simulation_parameters{2, -0.32}},
            .additional_scanning_area = std::pair<int32_t, int32_t>{2, 2},
            .influence_def            = defect_influence_params::influence_definition::GROUND_STATE_CHANGE};

        layout lyt{};

        lyt.assign_cell_type(site_at_row(0, 0), sidb_technology::cell_type::NORMAL);

        SECTION("Grid Search")
        {
            const auto defect_operational_domain = defect_influence_grid_search(lyt, defect_operational_params);

            const auto clearance_result = calculate_defect_clearance(lyt, defect_operational_domain);

            CHECK_THAT(round_to_n_decimal_places(clearance_result.defect_clearance_distance, 6),
                       Catch::Matchers::WithinAbs(0.665060, ERROR_MARGIN));
            CHECK((((clearance_result.defect_position.x == -1) && (clearance_result.defect_position.y == -1)) ||
                   ((clearance_result.defect_position.x == 1) && (clearance_result.defect_position.y == -1))));
        }
    }

    SECTION("layout with one SiDB, negative defect, smaller lambda_tf")
    {
        const auto defect_operational_params = defect_influence_params{
            .defect =
                defect{defect_type::UNKNOWN, -1, simulation_parameters{}.epsilon_r, simulation_parameters{}.lambda_tf},
            .operational_params       = is_operational_params{.sim_params = simulation_parameters{2, -0.32}},
            .additional_scanning_area = {2, 2},
            .influence_def            = defect_influence_params::influence_definition::GROUND_STATE_CHANGE};

        layout lyt{};
        lyt.assign_cell_type(site_at_row(0, 0), sidb_cell_clk_lyt_cube::cell_type::NORMAL);

        const auto defect_operational_domain = defect_influence_grid_search(lyt, defect_operational_params);
        const auto defect_clearance          = calculate_defect_clearance(lyt, defect_operational_domain);

        CHECK_THAT(
            round_to_n_decimal_places(defect_clearance.defect_clearance_distance, 4) -
                round_to_n_decimal_places(lyt.get_lattice().nm_distance(site_at_row(0, 0), site_at_row(1, -1)), 4),
            Catch::Matchers::WithinAbs(0.0, ERROR_MARGIN));
    }

    SECTION("layout with one SiDB, negative defect, large lambda_tf")
    {
        const auto defect_operational_params = defect_influence_params{
            .defect                   = defect{defect_type::UNKNOWN, -1, simulation_parameters{}.epsilon_r, 20},
            .operational_params       = is_operational_params{.sim_params = simulation_parameters{2, -0.32}},
            .additional_scanning_area = {30, 30},
            .influence_def            = defect_influence_params::influence_definition::GROUND_STATE_CHANGE};

        layout lyt{};
        lyt.assign_cell_type(site_at_row(0, 0), sidb_technology::cell_type::NORMAL);
        const layout lat{lyt};

        const auto defect_operational_domain = defect_influence_grid_search(lat, defect_operational_params);

        const auto defect_clearance = calculate_defect_clearance(lat, defect_operational_domain);

        CHECK_THAT(
            round_to_n_decimal_places(defect_clearance.defect_clearance_distance, 4) -
                round_to_n_decimal_places(lat.get_lattice().nm_distance(site_at_row(0, 0), site_at_row(0, 2)), 4),
            Catch::Matchers::WithinAbs(0.0, ERROR_MARGIN));
    }

    SECTION("layout with one pertuber and one DB pair, negative defect")
    {
        const auto defect_operational_params = defect_influence_params{
            .defect =
                defect{defect_type::UNKNOWN, -1, simulation_parameters{}.epsilon_r, simulation_parameters{}.lambda_tf},
            .operational_params       = is_operational_params{.sim_params = simulation_parameters{2, -0.32}},
            .additional_scanning_area = {20, 0},
            .influence_def            = defect_influence_params::influence_definition::GROUND_STATE_CHANGE};

        layout lyt{};

        lyt.assign_cell_type(site_at_row(0, 0), sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(site_at_row(4, 0), sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type(site_at_row(6, 0), sidb_technology::cell_type::NORMAL);

        const layout lat{lyt};

        const auto defect_operational_domain = defect_influence_grid_search(lat, defect_operational_params);
        const auto defect_clearance          = calculate_defect_clearance(lat, defect_operational_domain);

        CHECK_THAT(
            round_to_n_decimal_places(defect_clearance.defect_clearance_distance, 4) -
                round_to_n_decimal_places(lat.get_lattice().nm_distance(site_at_row(6, 0), site_at_row(10, 0)), 4),
            Catch::Matchers::WithinAbs(0.0, ERROR_MARGIN));
    }

    SECTION("QuickExact simulation of a Y-shape SiDB OR gate with input 01")
    {
        auto defect_operational_params = defect_influence_params{
            .defect =
                defect{defect_type::UNKNOWN, -1, simulation_parameters{}.epsilon_r, simulation_parameters{}.lambda_tf},
            .operational_params       = is_operational_params{.sim_params = simulation_parameters{2, -0.32}},
            .additional_scanning_area = {20, 20},
            .influence_def            = defect_influence_params::influence_definition::GROUND_STATE_CHANGE};

        layout lyt{};

        lyt.assign_cell_type(site_at_row(10, 0), sidb_cell_clk_lyt_cube::cell_type::NORMAL);
        lyt.assign_cell_type(site_at_row(0, 2), sidb_cell_clk_lyt_cube::cell_type::NORMAL);
        lyt.assign_cell_type(site_at_row(8, 2), sidb_cell_clk_lyt_cube::cell_type::NORMAL);

        lyt.assign_cell_type(site_at_row(2, 4), sidb_cell_clk_lyt_cube::cell_type::NORMAL);
        lyt.assign_cell_type(site_at_row(6, 4), sidb_cell_clk_lyt_cube::cell_type::NORMAL);
        lyt.assign_cell_type(site_at_row(4, 8), sidb_cell_clk_lyt_cube::cell_type::NORMAL);
        lyt.assign_cell_type(site_at_row(4, 11), sidb_cell_clk_lyt_cube::cell_type::NORMAL);
        lyt.assign_cell_type(site_at_row(4, 15), sidb_cell_clk_lyt_cube::cell_type::NORMAL);

        const layout lat{lyt};

        const auto defect_operational_domain = defect_influence_grid_search(lat, defect_operational_params);
        const auto defect_clearance          = calculate_defect_clearance(lat, defect_operational_domain);

        CHECK(defect_clearance.defect_position == site_at_row(12, 9));

        CHECK_THAT(defect_clearance.defect_clearance_distance, Catch::Matchers::WithinAbs(2.8999201713, ERROR_MARGIN));

        // high screening
        defect_operational_params.defect = defect{defect_type::UNKNOWN, -1, simulation_parameters{}.epsilon_r, 1};

        const auto defect_operational_domain_high_screening =
            defect_influence_grid_search(lat, defect_operational_params);
        const auto defect_clearance_high_screening =
            calculate_defect_clearance(lat, defect_operational_domain_high_screening);

        CHECK(defect_clearance_high_screening.defect_clearance_distance < defect_clearance.defect_clearance_distance);
    }

    SECTION("QuickExact simulation of a Y-shape SiDB OR gate with input 01, using cube coordinate")
    {
        const auto defect_operational_params = defect_influence_params{
            .defect =
                defect{defect_type::UNKNOWN, -1, simulation_parameters{}.epsilon_r, simulation_parameters{}.lambda_tf},
            .operational_params       = is_operational_params{.sim_params = simulation_parameters{2, -0.32}},
            .additional_scanning_area = {30, 30},
            .influence_def            = defect_influence_params::influence_definition::GROUND_STATE_CHANGE};

        layout lyt{};

        lyt.assign_cell_type({10, 0, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({0, 1, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({8, 1, 0}, sidb_technology::cell_type::NORMAL);

        lyt.assign_cell_type({2, 2, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({4, 4, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({4, 5, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({4, 7, 1}, sidb_technology::cell_type::NORMAL);

        const layout lat{lyt};

        const auto defect_operational_domain = defect_influence_grid_search(lat, defect_operational_params);
        const auto clearance_result          = calculate_defect_clearance(lat, defect_operational_domain);

        CHECK(clearance_result.defect_position == site_at_row(12, 9));

        CHECK_THAT(clearance_result.defect_clearance_distance, Catch::Matchers::WithinAbs(2.8999201713, ERROR_MARGIN));
    }
}
