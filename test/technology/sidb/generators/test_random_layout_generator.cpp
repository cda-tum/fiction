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
// Created by Jan Drewniok on 04.05.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/layout_utils.hpp>
#include <fiction/technology/sidb/generators/random_layout_generator.hpp>
#include <fiction/technology/sidb/model/defect.hpp>
#include <fiction/technology/sidb/simulation/analysis/can_positive_charges_occur.hpp>
#include <fiction/technology/sidb/surfaces/defect_surface.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

using namespace fiction;

// every std::optional below is dereferenced only after a Catch2 REQUIRE on its has_value(), but the
// macro expands to a throwing assertion whose control flow bugprone-unchecked-optional-access does not
// model, so the check reports each of those dereferences
// NOLINTBEGIN(bugprone-unchecked-optional-access)

TEST_CASE("Random coords::cube layout generation", "[random-sidb-layout-generator]")
{
    SECTION("empty parameters")
    {
        const sidb::generators::generate_random_layout_params<layouts::coords::cube> params{};

        const auto lyt = sidb::generators::generate_random_layout<sidb_cell_clk_lyt_cube>(params);

        REQUIRE(lyt.has_value());
        CHECK(lyt.value().num_cells() == 0);
        CHECK(lyt.value().x() == 0);
        CHECK(lyt.value().y() == 0);
    }

    SECTION("given corner coordinates, wrong order")
    {
        const sidb::generators::generate_random_layout_params<layouts::coords::cube> params{
            .coordinate_pair = {{5, 7, 2}, {-10, -10, 0}}};

        const auto result_lyt = sidb::generators::generate_random_layout<sidb_cell_clk_lyt_cube>(params);

        REQUIRE(result_lyt.has_value());

        CHECK(result_lyt.value().num_cells() == 0);
        result_lyt.value().foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x == 0);
                CHECK(cell.y == 0);
                CHECK(cell.z == 0);
            });
    }

    SECTION("given corner coordinates")
    {
        const sidb::generators::generate_random_layout_params<layouts::coords::cube> params{
            .coordinate_pair = {{-10, -10, 0}, {5, 7, 2}}};

        const auto result_lyt = sidb::generators::generate_random_layout<sidb_cell_clk_lyt_cube>(params);
        REQUIRE(result_lyt.has_value());

        CHECK(result_lyt.value().num_cells() == 0);
        result_lyt.value().foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x == 0);
                CHECK(cell.y == 0);
                CHECK(cell.z == 0);
            });
    }

    SECTION("given two identical coordinates")
    {
        const sidb::generators::generate_random_layout_params<layouts::coords::cube> params{
            .coordinate_pair = {{-10, -10, 1}, {-10, -10, 1}},
            .number_of_sidbs = 1};

        const auto result_lyt = sidb::generators::generate_random_layout<sidb_cell_clk_lyt_cube>(params);
        REQUIRE(result_lyt.has_value());

        CHECK(result_lyt.value().num_cells() == 1);
        result_lyt.value().foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x == -10);
                CHECK(cell.y == -10);
                CHECK(cell.z == 1);
            });
    }

    SECTION("given corner coordinates and number of placed SiDBs")
    {
        const sidb::generators::generate_random_layout_params<layouts::coords::cube> params{
            .coordinate_pair = {{-10, -10, 0}, {5, 7, 1}},
            .number_of_sidbs = 10};

        const auto result_lyt = sidb::generators::generate_random_layout<sidb_cell_clk_lyt_cube>(params);
        REQUIRE(result_lyt.has_value());

        CHECK(result_lyt.value().num_cells() == 10);
        result_lyt.value().foreach_cell(
            [&](const auto& cell)
            {
                CHECK(cell.x < 6);
                CHECK(cell.x > -11);
                CHECK(cell.y < 8);
                CHECK(cell.y > -11);
                CHECK(cell.z < 21);
                CHECK(cell.z > -1);
            });
    }

    SECTION("given corner coordinates and number of placed SiDBs, and forbid positive charges")
    {
        const sidb::generators::generate_random_layout_params<layouts::coords::cube> params{
            .coordinate_pair = {{0, 0, 0}, {90, 90, 0}},
            .number_of_sidbs = 100,
            .positive_sidbs =
                sidb::generators::generate_random_layout_params<layouts::coords::cube>::positive_charges::ALLOWED};

        const auto result_lyt = sidb::generators::generate_random_layout<sidb_cell_clk_lyt_cube>(params);
        REQUIRE(result_lyt.has_value());

        CHECK(result_lyt.value().num_cells() == 100);
        result_lyt.value().foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x < 91);
                CHECK(cell.y < 91);
            });
    }

    SECTION("given corner coordinates and number of placed SiDBs, and allow positive charges")
    {
        const sidb::generators::generate_random_layout_params<layouts::coords::cube> params{
            .coordinate_pair = {{0, 0, 0}, {200, 200, 0}},
            .number_of_sidbs = 50,
            .positive_sidbs =
                sidb::generators::generate_random_layout_params<layouts::coords::cube>::positive_charges::FORBIDDEN};

        const auto result_lyt = sidb::generators::generate_random_layout<sidb_cell_clk_lyt_cube>(params);
        REQUIRE(result_lyt.has_value());

        CHECK(result_lyt.value().num_cells() == 50);
        result_lyt.value().foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x <= 200);
                CHECK(cell.y <= 200);
            });

        CHECK(!sidb::simulation::analysis::can_positive_charges_occur(result_lyt.value(),
                                                                      sidb::model::simulation_parameters{}));
    }

    SECTION("given previous layouts")
    {
        const sidb::generators::generate_random_layout_params<layouts::coords::cube> params{
            .coordinate_pair = {{-5, -2}, {20, 20}},
            .number_of_sidbs = 6,
            .positive_sidbs =
                sidb::generators::generate_random_layout_params<layouts::coords::cube>::positive_charges::FORBIDDEN,
            .sim_params                         = sidb::model::simulation_parameters{},
            .maximal_attempts                   = static_cast<uint64_t>(10E6),
            .number_of_unique_generated_layouts = 3};

        const auto result_lyts = sidb::generators::generate_multiple_random_layouts<sidb_cell_clk_lyt_cube>(params);
        REQUIRE(result_lyts.has_value());

        CHECK(result_lyts.value().size() == 3);

        for (const auto& lyt : result_lyts.value())
        {
            lyt.foreach_cell(
                [](const auto& cell)
                {
                    CHECK(cell.x <= 20);
                    CHECK(cell.x > -6);
                    CHECK(cell.y <= 20);
                    CHECK(cell.y > -3);
                });
        }
    }

    SECTION("Check uniqueness of two layouts")
    {
        // a 21x21 region this densely packed lets greedy placement corner itself, after which the generator
        // spends its whole attempt budget rejecting every remaining position. These budgets bound that worst
        // case at tens of milliseconds; the defaults let it run for tens of seconds
        const sidb::generators::generate_random_layout_params<layouts::coords::cube> params{
            .coordinate_pair = {{0, 0}, {20, 20}},
            .number_of_sidbs = 8,
            .positive_sidbs =
                sidb::generators::generate_random_layout_params<layouts::coords::cube>::positive_charges::FORBIDDEN,
            .sim_params                            = sidb::model::simulation_parameters{},
            .maximal_attempts                      = 10'000,
            .number_of_unique_generated_layouts    = 2,
            .maximal_attempts_for_multiple_layouts = 1'000};

        const auto result_lyts = sidb::generators::generate_multiple_random_layouts<sidb_cell_clk_lyt_cube>(params);
        REQUIRE(result_lyts.has_value());

        REQUIRE(result_lyts.value().size() == 2);

        const auto& first_lyt  = result_lyts.value().front();
        const auto& second_lyt = result_lyts.value().back();

        CHECK(!layouts::are_cell_layouts_identical(first_lyt, second_lyt));
    }

    SECTION("Check uniqueness of many layouts")
    {
        const sidb::generators::generate_random_layout_params<layouts::coords::cube> params{
            .coordinate_pair = {{0, 0}, {20, 20}},
            .number_of_sidbs = 8,
            .positive_sidbs =
                sidb::generators::generate_random_layout_params<layouts::coords::cube>::positive_charges::ALLOWED,
            .sim_params                         = sidb::model::simulation_parameters{},
            .maximal_attempts                   = 10'000,
            .number_of_unique_generated_layouts = 50};

        const auto result_lyts = sidb::generators::generate_multiple_random_layouts<sidb_cell_clk_lyt_cube>(params);
        const auto lyts        = result_lyts.value_or(std::vector<sidb_cell_clk_lyt_cube>{});

        REQUIRE(lyts.size() == 50);

        for (std::size_t i = 0; i < lyts.size(); ++i)
        {
            for (std::size_t j = i + 1; j < lyts.size(); ++j)
            {
                CHECK(!layouts::are_cell_layouts_identical(lyts.at(i), lyts.at(j)));
            }
        }
    }

    SECTION("Check all pairwise distances")
    {
        const sidb::generators::generate_random_layout_params<layouts::coords::cube> params{
            .coordinate_pair = {{0, 0}, {40, 40}},
            .number_of_sidbs = 10,
            .positive_sidbs =
                sidb::generators::generate_random_layout_params<layouts::coords::cube>::positive_charges::FORBIDDEN,
            .sim_params                         = sidb::model::simulation_parameters{},
            .maximal_attempts                   = static_cast<uint64_t>(10E6),
            .number_of_unique_generated_layouts = 10};

        const auto result_lyts = sidb::generators::generate_multiple_random_layouts<sidb_cell_clk_lyt_cube>(params);
        REQUIRE(result_lyts.has_value());
        REQUIRE(result_lyts.value().size() == 10);

        for (const auto& lyt : result_lyts.value())
        {
            CHECK(!sidb::simulation::analysis::can_positive_charges_occur(lyt, sidb::model::simulation_parameters{}));
        }
    }
}
TEST_CASE("Random coords::offset layout generation", "[random-sidb-layout-generator]")
{
    SECTION("empty parameters")
    {
        const sidb::generators::generate_random_layout_params<layouts::coords::offset> params{};

        const auto lyt = sidb::generators::generate_random_layout<sidb_100_cell_clk_lyt>(params);
        REQUIRE(lyt.has_value());

        CHECK(lyt.value().num_cells() == 0);
        CHECK(lyt.value().x() == 0);
        CHECK(lyt.value().y() == 0);
    }

    SECTION("design is impossible")
    {
        // the default region spans a single cell, so no attempt budget can place two SiDBs; a small budget keeps
        // the section from burning the default 10^7 attempts
        sidb::generators::generate_random_layout_params<layouts::coords::offset> params{};
        params.number_of_sidbs  = 2;
        params.maximal_attempts = 100;

        const auto lyt = sidb::generators::generate_random_layout<sidb_100_cell_clk_lyt>(params);
        CHECK(!lyt.has_value());
    }

    SECTION("given corner coordinates")
    {
        const sidb::generators::generate_random_layout_params<layouts::coords::offset> params{
            .coordinate_pair = {{1, 1, 0}, {5, 7, 2}}};

        const auto result_lyt = sidb::generators::generate_random_layout<sidb_100_cell_clk_lyt>(params);
        REQUIRE(result_lyt.has_value());

        CHECK(result_lyt.value().num_cells() == 0);
        result_lyt.value().foreach_cell(
            [&](const auto& cell)
            {
                CHECK(cell.x == 0);
                CHECK(cell.y == 0);
                CHECK(cell.z == 0);
            });
    }

    SECTION("given two identical coordinates")
    {
        const sidb::generators::generate_random_layout_params<layouts::coords::offset> params{
            .coordinate_pair = {{5, 5, 1}, {5, 5, 1}},
            .number_of_sidbs = 1};

        const auto result_lyt = sidb::generators::generate_random_layout<sidb_100_cell_clk_lyt>(params);
        REQUIRE(result_lyt.has_value());

        CHECK(result_lyt.value().num_cells() == 1);
        result_lyt.value().foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x == 5);
                CHECK(cell.y == 5);
                CHECK(cell.z == 1);
            });
    }

    SECTION("given corner coordinates and number of placed SiDBs")
    {
        const sidb::generators::generate_random_layout_params<layouts::coords::offset> params{
            .coordinate_pair = {{1, 1, 0}, {50, 7, 1}},
            .number_of_sidbs = 10};

        const auto result_lyt = sidb::generators::generate_random_layout<sidb_100_cell_clk_lyt>(params);
        REQUIRE(result_lyt.has_value());

        CHECK(result_lyt.value().num_cells() == 10);
        result_lyt.value().foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x < 51);
                CHECK(cell.x > 0);
                CHECK(cell.y < 8);
                CHECK(cell.y > 0);
                CHECK(cell.z < 21);
                CHECK(cell.z >= 0);
            });
    }

    SECTION("given corner coordinates and number of placed SiDBs, and forbid positive charges")
    {
        const sidb::generators::generate_random_layout_params<layouts::coords::offset> params{
            .coordinate_pair = {{0, 0, 0}, {90, 90, 0}},
            .number_of_sidbs = 100,
            .positive_sidbs =
                sidb::generators::generate_random_layout_params<layouts::coords::offset>::positive_charges::ALLOWED};

        const auto result_lyt = sidb::generators::generate_random_layout<sidb_100_cell_clk_lyt>(params);
        REQUIRE(result_lyt.has_value());

        CHECK(result_lyt.value().num_cells() == 100);
        result_lyt.value().foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x < 91);
                CHECK(cell.y < 91);
            });
    }

    SECTION("given corner coordinates and number of placed SiDBs, and allow positive charges")
    {
        const sidb::generators::generate_random_layout_params<layouts::coords::offset> params{
            .coordinate_pair = {{0, 0, 0}, {200, 200, 0}},
            .number_of_sidbs = 100,
            .positive_sidbs =
                sidb::generators::generate_random_layout_params<layouts::coords::offset>::positive_charges::FORBIDDEN};

        const auto result_lyt = sidb::generators::generate_random_layout<sidb_100_cell_clk_lyt>(params);
        REQUIRE(result_lyt.has_value());

        CHECK(result_lyt.value().num_cells() == 100);
        result_lyt.value().foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x <= 200);
                CHECK(cell.y <= 200);
            });

        CHECK(!sidb::simulation::analysis::can_positive_charges_occur(result_lyt.value(),
                                                                      sidb::model::simulation_parameters{}));
    }

    SECTION("given previous layouts")
    {
        const sidb::generators::generate_random_layout_params<layouts::coords::offset> params{
            .coordinate_pair = {{0, 0}, {40, 40}},
            .number_of_sidbs = 10,
            .positive_sidbs =
                sidb::generators::generate_random_layout_params<layouts::coords::offset>::positive_charges::FORBIDDEN,
            .sim_params                         = sidb::model::simulation_parameters{},
            .maximal_attempts                   = static_cast<uint64_t>(10E6),
            .number_of_unique_generated_layouts = 3};

        const auto result_lyts = sidb::generators::generate_multiple_random_layouts<sidb_100_cell_clk_lyt>(params);
        REQUIRE(result_lyts.has_value());
        CHECK(result_lyts.value().size() == 3);

        for (const auto& lyt : result_lyts.value())
        {
            lyt.foreach_cell(
                [](const auto& cell)
                {
                    CHECK(cell.x <= 40);
                    CHECK(cell.y <= 40);
                });
        }
    }

    SECTION("Check uniqueness of two layouts")
    {
        // a 21x21 region this densely packed lets greedy placement corner itself, after which the generator
        // spends its whole attempt budget rejecting every remaining position. These budgets bound that worst
        // case at tens of milliseconds; the defaults let it run for tens of seconds
        const sidb::generators::generate_random_layout_params<layouts::coords::offset> params{
            .coordinate_pair = {{0, 0}, {20, 20}},
            .number_of_sidbs = 10,
            .positive_sidbs =
                sidb::generators::generate_random_layout_params<layouts::coords::offset>::positive_charges::FORBIDDEN,
            .sim_params                            = sidb::model::simulation_parameters{},
            .maximal_attempts                      = 10'000,
            .number_of_unique_generated_layouts    = 2,
            .maximal_attempts_for_multiple_layouts = 1'000};

        const auto result_lyts = sidb::generators::generate_multiple_random_layouts<sidb_100_cell_clk_lyt>(params);
        REQUIRE(result_lyts.has_value());
        REQUIRE(result_lyts.value().size() == 2);

        const auto& first_lyt  = result_lyts.value().front();
        const auto& second_lyt = result_lyts.value().back();

        CHECK(!layouts::are_cell_layouts_identical(first_lyt, second_lyt));
    }

    SECTION("Check that duplicates are rejected when the region admits only two layouts")
    {
        // the region holds two cells and each layout carries a single SiDB, so only two layouts exist; every
        // further candidate repeats one of them and has to be rejected by the exact comparison behind the digest
        const sidb::generators::generate_random_layout_params<layouts::coords::offset> params{
            .coordinate_pair = {{0, 0}, {1, 0}},
            .number_of_sidbs = 1,
            .positive_sidbs =
                sidb::generators::generate_random_layout_params<layouts::coords::offset>::positive_charges::ALLOWED,
            .sim_params                            = sidb::model::simulation_parameters{},
            .maximal_attempts                      = 10'000,
            .number_of_unique_generated_layouts    = 5,
            .maximal_attempts_for_multiple_layouts = 500};

        const auto result_lyts = sidb::generators::generate_multiple_random_layouts<sidb_cell_clk_lyt>(params);
        const auto lyts        = result_lyts.value_or(std::vector<sidb_cell_clk_lyt>{});

        REQUIRE(lyts.size() == 2);

        CHECK(!layouts::are_cell_layouts_identical(lyts.front(), lyts.back()));
    }

    SECTION("Check correct use of skeleton layout when generating only one random layout")
    {
        const sidb::generators::generate_random_layout_params<layouts::coords::offset> params{
            .coordinate_pair = {{0, 0}, {9, 9}},
            .number_of_sidbs = 10};

        sidb_100_cell_clk_lyt skeleton_layout{};
        skeleton_layout.assign_cell_type({0, 0}, sidb_100_cell_clk_lyt::technology::cell_type::NORMAL);
        skeleton_layout.assign_cell_type({9, 1}, sidb_100_cell_clk_lyt::technology::cell_type::NORMAL);
        skeleton_layout.assign_cell_type({5, 0}, sidb_100_cell_clk_lyt::technology::cell_type::NORMAL);

        const auto result_lyt = sidb::generators::generate_random_layout(params, std::optional{skeleton_layout});

        REQUIRE(result_lyt.has_value());
        CHECK(result_lyt.value().num_cells() == 13);
    }

    SECTION("Check correct use of skeleton layout when generating multiple random layouts")
    {
        const sidb::generators::generate_random_layout_params<layouts::coords::offset> params{
            .coordinate_pair = {{0, 0}, {40, 40}},
            .number_of_sidbs = 10,
            .positive_sidbs =
                sidb::generators::generate_random_layout_params<layouts::coords::offset>::positive_charges::FORBIDDEN,
            .sim_params                         = sidb::model::simulation_parameters{},
            .maximal_attempts                   = static_cast<uint64_t>(10E6),
            .number_of_unique_generated_layouts = 2};

        sidb_100_cell_clk_lyt skeleton_layout{};

        skeleton_layout.assign_cell_type({0, 0}, sidb_100_cell_clk_lyt::technology::cell_type::NORMAL);
        skeleton_layout.assign_cell_type({3, 0}, sidb_100_cell_clk_lyt::technology::cell_type::NORMAL);
        skeleton_layout.assign_cell_type({9, 1}, sidb_100_cell_clk_lyt::technology::cell_type::NORMAL);

        const auto result_lyts =
            sidb::generators::generate_multiple_random_layouts(params, std::optional{skeleton_layout});
        REQUIRE(result_lyts.has_value());
        REQUIRE(result_lyts.value().size() == 2);

        CHECK(result_lyts.value().front().num_cells() == 13);
        CHECK(result_lyts.value().back().num_cells() == 13);
    }

    SECTION("Check if std::nullptr_t is returned when no layout can be designed")
    {
        // the default region spans a single cell, so no attempt budget can place two SiDBs; a small budget keeps
        // the section from burning the default 10^7 attempts five times over
        sidb::generators::generate_random_layout_params<layouts::coords::offset> params{};
        params.maximal_attempts                      = 100;
        params.maximal_attempts_for_multiple_layouts = 5;
        params.number_of_sidbs                       = 2;

        const auto result_lyts = sidb::generators::generate_multiple_random_layouts<sidb_cell_clk_lyt>(params);
        CHECK(!result_lyts.has_value());
    }
}
TEST_CASE("Random coords::siqad layout generation", "[random-sidb-layout-generator]")
{
    SECTION("empty parameters")
    {
        const sidb::generators::generate_random_layout_params<layouts::coords::siqad> params{};

        const auto lyt = sidb::generators::generate_random_layout<sidb_cell_clk_lyt_siqad>(params);
        REQUIRE(lyt.has_value());

        CHECK(lyt.value().num_cells() == 0);
        CHECK(lyt.value().x() == 0);
        CHECK(lyt.value().y() == 0);
    }

    SECTION("given two identical coordinates")
    {
        const sidb::generators::generate_random_layout_params<layouts::coords::siqad> params{
            .coordinate_pair = {{5, 5, 1}, {5, 5, 1}},
            .number_of_sidbs = 1};

        const auto result_lyt = sidb::generators::generate_random_layout<sidb_cell_clk_lyt_siqad>(params);
        REQUIRE(result_lyt.has_value());

        CHECK(result_lyt.value().num_cells() == 1);
        result_lyt.value().foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x == 5);
                CHECK(cell.y == 5);
                CHECK(cell.z == 1);
            });
    }

    SECTION("given corner coordinates")
    {
        const sidb::generators::generate_random_layout_params<layouts::coords::siqad> params{
            .coordinate_pair = {{1, 1, 0}, {5, 7, 1}}};

        const auto result_lyt = sidb::generators::generate_random_layout<sidb_cell_clk_lyt_siqad>(params);
        REQUIRE(result_lyt.has_value());

        CHECK(result_lyt.value().num_cells() == 0);
        result_lyt.value().foreach_cell(
            [&](const auto& cell)
            {
                CHECK(cell.x == 0);
                CHECK(cell.y == 0);
                CHECK(cell.z == 0);
            });
    }

    SECTION("given corner coordinates and number of placed SiDBs")
    {
        const sidb::generators::generate_random_layout_params<layouts::coords::siqad> params{
            .coordinate_pair = {{1, 1, 0}, {50, 7, 1}},
            .number_of_sidbs = 10};

        const auto result_lyt = sidb::generators::generate_random_layout<sidb_cell_clk_lyt_siqad>(params);
        REQUIRE(result_lyt.has_value());

        CHECK(result_lyt.value().num_cells() == 10);
        result_lyt.value().foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x < 51);
                CHECK(cell.x > 0);
                CHECK(cell.y < 8);
                CHECK(cell.y > 0);
                CHECK(cell.z <= 1);
            });
    }

    SECTION("given corner coordinates and number of placed SiDBs, and allow positive charges")
    {
        const sidb::generators::generate_random_layout_params<layouts::coords::siqad> params{
            .coordinate_pair = {{0, 0, 0}, {90, 90, 0}},
            .number_of_sidbs = 100,
            .positive_sidbs =
                sidb::generators::generate_random_layout_params<layouts::coords::siqad>::positive_charges::ALLOWED};

        const auto result_lyt = sidb::generators::generate_random_layout<sidb_cell_clk_lyt_siqad>(params);
        REQUIRE(result_lyt.has_value());

        CHECK(result_lyt.value().num_cells() == 100);
        result_lyt.value().foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x < 91);
                CHECK(cell.y < 91);
            });
    }

    SECTION("given corner coordinates and number of placed SiDBs, and forbid positive charges")
    {
        const sidb::generators::generate_random_layout_params<layouts::coords::siqad> params{
            .coordinate_pair = {{0, 0, 0}, {90, 90, 0}},
            .number_of_sidbs = 10,
            .positive_sidbs =
                sidb::generators::generate_random_layout_params<layouts::coords::siqad>::positive_charges::FORBIDDEN};

        const auto result_lyt = sidb::generators::generate_random_layout<sidb_cell_clk_lyt_siqad>(params);
        REQUIRE(result_lyt.has_value());

        CHECK(result_lyt.value().num_cells() == 10);
        result_lyt.value().foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x < 91);
                CHECK(cell.y < 91);
                CHECK(cell.z <= 1);
            });
    }

    SECTION("given previous layouts")
    {
        const sidb::generators::generate_random_layout_params<layouts::coords::siqad> params{
            .coordinate_pair = {{0, 0, 1}, {20, 20, 1}},
            .number_of_sidbs = 10,
            .positive_sidbs =
                sidb::generators::generate_random_layout_params<layouts::coords::siqad>::positive_charges::FORBIDDEN,
            .sim_params                         = sidb::model::simulation_parameters{},
            .maximal_attempts                   = static_cast<uint64_t>(10E6),
            .number_of_unique_generated_layouts = 3};

        const auto result_lyts = sidb::generators::generate_multiple_random_layouts<sidb_cell_clk_lyt_siqad>(params);
        REQUIRE(result_lyts.has_value());
        CHECK(result_lyts.value().size() == 3);

        for (const auto& lyt : result_lyts.value())
        {
            lyt.foreach_cell(
                [](const auto& cell)
                {
                    CHECK(cell.x <= 20);
                    CHECK(cell.x >= 0);
                    CHECK(cell.y <= 20);
                    CHECK(cell.y >= 0);
                    CHECK(cell.z <= 1);
                });
        }
    }
}

TEMPLATE_TEST_CASE("Random coords::siqad layout generation with defects", "[random-sidb-layout-generator]",
                   sidb_defect_cell_clk_lyt_siqad, sidb::surfaces::defect_surface<sidb_111_cell_clk_lyt_siqad>)
{
    SECTION("given two identical coordinates")
    {
        const sidb::generators::generate_random_layout_params<cell<TestType>> params{{{5, 5, 1}, {5, 5, 1}}, 1};

        const auto result_lyt = sidb::generators::generate_random_layout<TestType>(params);
        REQUIRE(result_lyt.has_value());

        CHECK(result_lyt.value().num_cells() == 1);
        result_lyt.value().foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x == 5);
                CHECK(cell.y == 5);
                CHECK(cell.z == 1);
            });
    }

    SECTION("region including only one cell and there is a defect")
    {
        // it is not possible to generate a random layout since the position where a SiDB could be placed is occupied by
        // a defect.
        const sidb::generators::generate_random_layout_params<cell<TestType>> params{
            {{2, 1, 1}, {2, 1, 1}},
            1,
            sidb::generators::generate_random_layout_params<cell<TestType>>::positive_charges::FORBIDDEN,
            sidb::model::simulation_parameters{},
            5u};

        auto defect_layout = TestType{};
        defect_layout.assign_defect({2, 1, 1}, sidb::model::defect{sidb::model::defect_type::DB, -1, 5.6, 5});

        const auto result_lyt = sidb::generators::generate_random_layout(params, std::optional{defect_layout});
        CHECK(!result_lyt.has_value());
    }

    SECTION("region including only one cell and there is no defect")
    {
        // it is not possible to generate a random layout since the position where a SiDB could be placed is occupied by
        // a defect.
        const sidb::generators::generate_random_layout_params<cell<TestType>> params{
            {{2, 1, 1}, {2, 1, 1}},
            1,
            sidb::generators::generate_random_layout_params<cell<TestType>>::positive_charges::FORBIDDEN,
            sidb::model::simulation_parameters{},
            5u};

        auto defect_layout = TestType{};
        defect_layout.assign_defect({3, 1, 1}, sidb::model::defect{sidb::model::defect_type::DB, -1, 5.6, 5});
        defect_layout.assign_defect({4, 1, 1},
                                    sidb::model::defect{sidb::model::defect_type::SINGLE_DIHYDRIDE, 1, 7.6, 7});

        const auto result_lyt = sidb::generators::generate_random_layout(params, std::optional{defect_layout});
        REQUIRE(result_lyt.has_value());

        CHECK(result_lyt.value().num_cells() == 1);
        CHECK(result_lyt.value().num_defects() == 2);

        CHECK(result_lyt.value().get_cell_type({2, 1, 1}) == TestType::technology::cell_type::LOGIC);
        CHECK(result_lyt.value().get_defect({3, 1, 1}) ==
              sidb::model::defect{sidb::model::defect_type::DB, -1, 5.6, 5});
        CHECK(result_lyt.value().get_defect({4, 1, 1}) ==
              sidb::model::defect{sidb::model::defect_type::SINGLE_DIHYDRIDE, 1, 7.6, 7});
    }

    SECTION("Check uniqueness of many layouts sharing a defective skeleton")
    {
        const sidb::generators::generate_random_layout_params<cell<TestType>> params{
            .coordinate_pair = {{0, 0, 0}, {10, 2, 0}},
            .number_of_sidbs = 5,
            .positive_sidbs =
                sidb::generators::generate_random_layout_params<cell<TestType>>::positive_charges::ALLOWED,
            .sim_params                         = sidb::model::simulation_parameters{},
            .maximal_attempts                   = 10'000,
            .number_of_unique_generated_layouts = 20};

        auto defect_layout = TestType{};
        defect_layout.assign_defect({2, 2, 0}, sidb::model::defect{sidb::model::defect_type::DB, -1, 5.6, 5});
        defect_layout.assign_defect({4, 1, 0},
                                    sidb::model::defect{sidb::model::defect_type::SINGLE_DIHYDRIDE, 1, 7.6, 7});

        const auto result_lyts =
            sidb::generators::generate_multiple_random_layouts(params, std::optional{defect_layout});
        const auto lyts = result_lyts.value_or(std::vector<TestType>{});

        REQUIRE(lyts.size() == 20);

        for (std::size_t i = 0; i < lyts.size(); ++i)
        {
            CHECK(lyts.at(i).num_cells() == 5);
            CHECK(lyts.at(i).num_defects() == 2);

            for (std::size_t j = i + 1; j < lyts.size(); ++j)
            {
                CHECK(!layouts::are_cell_layouts_identical(lyts.at(i), lyts.at(j)));
            }
        }
    }

    SECTION("given corner coordinates and number of placed SiDBs, and allow positive charges")
    {
        const sidb::generators::generate_random_layout_params<cell<TestType>> params{
            {{0, 0, 0}, {10, 2, 0}},
            10,
            sidb::generators::generate_random_layout_params<cell<TestType>>::positive_charges::ALLOWED,
            sidb::model::simulation_parameters{}};

        auto defect_layout = TestType{};
        defect_layout.assign_defect({2, 2, 0}, sidb::model::defect{sidb::model::defect_type::DB, -1, 5.6, 5});
        defect_layout.assign_defect({4, 1, 0},
                                    sidb::model::defect{sidb::model::defect_type::SINGLE_DIHYDRIDE, 1, 7.6, 7});
        defect_layout.assign_defect({5, 1, 0},
                                    sidb::model::defect{sidb::model::defect_type::SINGLE_DIHYDRIDE, 1, 7.6, 9});
        defect_layout.assign_defect({7, 1, 0},
                                    sidb::model::defect{sidb::model::defect_type::SINGLE_DIHYDRIDE, 1, 2.6, 7});
        defect_layout.assign_defect({2, 1, 0},
                                    sidb::model::defect{sidb::model::defect_type::SINGLE_DIHYDRIDE, 1, 7.6, 4});

        const auto result_lyt = sidb::generators::generate_random_layout(params, std::optional{defect_layout});
        REQUIRE(result_lyt.has_value());

        CHECK(result_lyt.value().num_cells() == 10);
        CHECK(result_lyt.value().num_defects() == 5);

        // check if all cells are not closer than two cells (Euclidean distance).
        result_lyt.value().foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell != layouts::coords::siqad{2, 2, 0});
                CHECK(cell != layouts::coords::siqad{4, 1, 0});
                CHECK(cell != layouts::coords::siqad{5, 1, 0});
                CHECK(cell != layouts::coords::siqad{7, 1, 0});
                CHECK(cell != layouts::coords::siqad{2, 1, 0});
            });
    }
}
TEST_CASE("Random coords::cube layout generation with defects", "[random-sidb-layout-generator]")
{
    using lyt = sidb_defect_cell_clk_lyt_cube;

    const sidb::generators::generate_random_layout_params<layouts::coords::cube> params{
        .coordinate_pair = {layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{0, 0, 0}),
                            layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{10, 2, 0})},
        .number_of_sidbs = 10,
        .positive_sidbs =
            sidb::generators::generate_random_layout_params<layouts::coords::cube>::positive_charges::ALLOWED,
        .sim_params = sidb::model::simulation_parameters{}};

    lyt layout{};

    layout.assign_defect(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{2, 2, 0}),
                         sidb::model::defect{sidb::model::defect_type::DB, -1, 5.6, 5});
    layout.assign_defect(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{4, 1, 0}),
                         sidb::model::defect{sidb::model::defect_type::SINGLE_DIHYDRIDE, 1, 7.6, 7});
    layout.assign_defect(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{5, 1, 0}),
                         sidb::model::defect{sidb::model::defect_type::SINGLE_DIHYDRIDE, 1, 7.6, 9});
    layout.assign_defect(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{7, 1, 0}),
                         sidb::model::defect{sidb::model::defect_type::SINGLE_DIHYDRIDE, 1, 2.6, 7});
    layout.assign_defect(layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{2, 1, 0}),
                         sidb::model::defect{sidb::model::defect_type::SINGLE_DIHYDRIDE, 1, 7.6, 4});

    const auto result_lyt = sidb::generators::generate_random_layout(params, std::optional{layout});

    REQUIRE(result_lyt.has_value());
    CHECK(result_lyt.value().num_cells() == 10);
    CHECK(result_lyt.value().num_defects() == 5);

    // check if all cells are not closer than two cells (Euclidean distance).
    result_lyt.value().foreach_cell(
        [](const auto& cell)
        {
            CHECK(cell != layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{2, 2, 0}));
            CHECK(cell != layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{4, 1, 0}));
            CHECK(cell != layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{5, 1, 0}));
            CHECK(cell != layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{7, 1, 0}));
            CHECK(cell != layouts::coords::from_siqad<layouts::coords::cube>(layouts::coords::siqad{2, 1, 0}));
        });
}

// NOLINTEND(bugprone-unchecked-optional-access)
