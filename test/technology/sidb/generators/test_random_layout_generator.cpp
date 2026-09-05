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
 * @brief Tests for `fiction/technology/sidb/generators/random_layout_generator.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 * @author Benjamin Hien (hibenj)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/technology/sidb/generators/random_layout_generator.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/defect.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/technology.hpp>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

using namespace fiction;
using namespace fiction::sidb::generators;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation::analysis;

// every std::optional below is dereferenced only after a Catch2 REQUIRE on its has_value(), but the
// macro expands to a throwing assertion whose control flow bugprone-unchecked-optional-access does not
// model, so the check reports each of those dereferences
// NOLINTBEGIN(bugprone-unchecked-optional-access)

TEST_CASE("Random SiDB layout generation", "[random-sidb-layout-generator]")
{
    SECTION("empty parameters")
    {
        const generate_random_layout_params params{};

        const auto lyt = generate_random_layout(params);
        REQUIRE(lyt.has_value());

        CHECK(lyt.value().num_cells() == 0);
    }

    SECTION("given two identical coordinates")
    {
        const generate_random_layout_params params{.coordinate_pair = {{5, 5, 1}, {5, 5, 1}}, .number_of_sidbs = 1};

        const auto result_lyt = generate_random_layout(params);
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
        const generate_random_layout_params params{.coordinate_pair = {{1, 1, 0}, {5, 7, 1}}};

        const auto result_lyt = generate_random_layout(params);
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
        const generate_random_layout_params params{.coordinate_pair = {{1, 1, 0}, {50, 7, 1}}, .number_of_sidbs = 10};

        const auto result_lyt = generate_random_layout(params);
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
        const generate_random_layout_params params{.coordinate_pair = {{0, 0, 0}, {90, 90, 0}},
                                                   .number_of_sidbs = 100,
                                                   .positive_sidbs =
                                                       generate_random_layout_params::positive_charges::ALLOWED};

        const auto result_lyt = generate_random_layout(params);
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
        const generate_random_layout_params params{.coordinate_pair = {{0, 0, 0}, {90, 90, 0}},
                                                   .number_of_sidbs = 10,
                                                   .positive_sidbs =
                                                       generate_random_layout_params::positive_charges::FORBIDDEN};

        const auto result_lyt = generate_random_layout(params);
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
        const generate_random_layout_params params{.coordinate_pair = {{0, 0, 1}, {20, 20, 1}},
                                                   .number_of_sidbs = 10,
                                                   .positive_sidbs =
                                                       generate_random_layout_params::positive_charges::FORBIDDEN,
                                                   .sim_params                         = simulation_parameters{},
                                                   .maximal_attempts                   = static_cast<uint64_t>(10E6),
                                                   .number_of_unique_generated_layouts = 3};

        const auto result_lyts = generate_multiple_random_layouts(params);
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

TEST_CASE("Random SiDB layout generation with defects", "[random-sidb-layout-generator]")
{
    SECTION("given two identical coordinates")
    {
        const generate_random_layout_params params{.coordinate_pair = {{5, 5, 1}, {5, 5, 1}}, .number_of_sidbs = 1};

        const auto result_lyt = generate_random_layout(params);
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
        const generate_random_layout_params params{.coordinate_pair = {{2, 1, 1}, {2, 1, 1}},
                                                   .number_of_sidbs = 1,
                                                   .positive_sidbs =
                                                       generate_random_layout_params::positive_charges::FORBIDDEN,
                                                   .sim_params       = simulation_parameters{},
                                                   .maximal_attempts = 5u};

        auto defect_layout = layout{};
        defect_layout.assign_defect({2, 1, 1}, defect{defect_type::DB, -1, 5.6, 5});

        const auto result_lyt = generate_random_layout(params, std::optional{defect_layout});
        CHECK(!result_lyt.has_value());
    }

    SECTION("region including only one cell and there is no defect")
    {
        // it is not possible to generate a random layout since the position where a SiDB could be placed is occupied by
        // a defect.
        const generate_random_layout_params params{.coordinate_pair = {{2, 1, 1}, {2, 1, 1}},
                                                   .number_of_sidbs = 1,
                                                   .positive_sidbs =
                                                       generate_random_layout_params::positive_charges::FORBIDDEN,
                                                   .sim_params       = simulation_parameters{},
                                                   .maximal_attempts = 5u};

        auto defect_layout = layout{};
        defect_layout.assign_defect({3, 1, 1}, defect{defect_type::DB, -1, 5.6, 5});
        defect_layout.assign_defect({4, 1, 1}, defect{defect_type::SINGLE_DIHYDRIDE, 1, 7.6, 7});

        const auto result_lyt = generate_random_layout(params, std::optional{defect_layout});
        REQUIRE(result_lyt.has_value());

        CHECK(result_lyt.value().num_cells() == 1);
        CHECK(result_lyt.value().num_defects() == 2);

        CHECK(result_lyt.value().get_cell_type({2, 1, 1}) == sidb_technology::cell_type::LOGIC);
        CHECK(result_lyt.value().get_defect({3, 1, 1}) == defect{defect_type::DB, -1, 5.6, 5});
        CHECK(result_lyt.value().get_defect({4, 1, 1}) == defect{defect_type::SINGLE_DIHYDRIDE, 1, 7.6, 7});
    }

    SECTION("Check uniqueness of many layouts sharing a defective skeleton")
    {
        const generate_random_layout_params params{.coordinate_pair = {{0, 0, 0}, {10, 2, 0}},
                                                   .number_of_sidbs = 5,
                                                   .positive_sidbs =
                                                       generate_random_layout_params::positive_charges::ALLOWED,
                                                   .sim_params                         = simulation_parameters{},
                                                   .maximal_attempts                   = 10'000,
                                                   .number_of_unique_generated_layouts = 20};

        auto defect_layout = layout{};
        defect_layout.assign_defect({2, 2, 0}, defect{defect_type::DB, -1, 5.6, 5});
        defect_layout.assign_defect({4, 1, 0}, defect{defect_type::SINGLE_DIHYDRIDE, 1, 7.6, 7});

        const auto result_lyts = generate_multiple_random_layouts(params, std::optional{defect_layout});
        const auto lyts        = result_lyts.value_or(std::vector<layout>{});

        REQUIRE(lyts.size() == 20);

        for (std::size_t i = 0; i < lyts.size(); ++i)
        {
            CHECK(lyts.at(i).num_cells() == 5);
            CHECK(lyts.at(i).num_defects() == 2);

            for (std::size_t j = i + 1; j < lyts.size(); ++j)
            {
                CHECK(!(lyts.at(i) == lyts.at(j)));
            }
        }
    }

    SECTION("given corner coordinates and number of placed SiDBs, and allow positive charges")
    {
        const generate_random_layout_params params{.coordinate_pair = {{0, 0, 0}, {10, 2, 0}},
                                                   .number_of_sidbs = 10,
                                                   .positive_sidbs =
                                                       generate_random_layout_params::positive_charges::ALLOWED,
                                                   .sim_params = simulation_parameters{}};

        auto defect_layout = layout{};
        defect_layout.assign_defect({2, 2, 0}, defect{defect_type::DB, -1, 5.6, 5});
        defect_layout.assign_defect({4, 1, 0}, defect{defect_type::SINGLE_DIHYDRIDE, 1, 7.6, 7});
        defect_layout.assign_defect({5, 1, 0}, defect{defect_type::SINGLE_DIHYDRIDE, 1, 7.6, 9});
        defect_layout.assign_defect({7, 1, 0}, defect{defect_type::SINGLE_DIHYDRIDE, 1, 2.6, 7});
        defect_layout.assign_defect({2, 1, 0}, defect{defect_type::SINGLE_DIHYDRIDE, 1, 7.6, 4});

        const auto result_lyt = generate_random_layout(params, std::optional{defect_layout});
        REQUIRE(result_lyt.has_value());

        CHECK(result_lyt.value().num_cells() == 10);
        CHECK(result_lyt.value().num_defects() == 5);

        // check if all cells are not closer than two cells (Euclidean distance).
        result_lyt.value().foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell != lattice_site{2, 2, 0});
                CHECK(cell != lattice_site{4, 1, 0});
                CHECK(cell != lattice_site{5, 1, 0});
                CHECK(cell != lattice_site{7, 1, 0});
                CHECK(cell != lattice_site{2, 1, 0});
            });
    }
}

// NOLINTEND(bugprone-unchecked-optional-access)
