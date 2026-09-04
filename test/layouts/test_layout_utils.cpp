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
 * @brief Tests for `fiction/layouts/layout_utils.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 * @author Willem Lambooy (wlambooy)
 */

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/layouts/layout_utils.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

using namespace fiction;
using namespace fiction::fcn;
using namespace fiction::layouts;

TEMPLATE_TEST_CASE("Port directions to coordinates", "[layout-utils]", (cartesian_layout<coords::offset>),
                   (hexagonal_layout<coords::offset, odd_row_hex>), (hexagonal_layout<coords::offset, even_row_hex>),
                   (hexagonal_layout<coords::offset, odd_column_hex>),
                   (hexagonal_layout<coords::offset, even_column_hex>))
{
    TestType lyt{{4, 4}};

    lyt.foreach_coordinate(
        [&lyt](const auto& c)
        {
            CHECK(port_direction_to_coordinate(lyt, c, port_direction{port_direction::cardinal::NORTH}) ==
                  lyt.north(c));
            CHECK(port_direction_to_coordinate(lyt, c, port_direction{port_direction::cardinal::NORTH_EAST}) ==
                  lyt.north_east(c));
            CHECK(port_direction_to_coordinate(lyt, c, port_direction{port_direction::cardinal::EAST}) == lyt.east(c));
            CHECK(port_direction_to_coordinate(lyt, c, port_direction{port_direction::cardinal::SOUTH_EAST}) ==
                  lyt.south_east(c));
            CHECK(port_direction_to_coordinate(lyt, c, port_direction{port_direction::cardinal::SOUTH}) ==
                  lyt.south(c));
            CHECK(port_direction_to_coordinate(lyt, c, port_direction{port_direction::cardinal::SOUTH_WEST}) ==
                  lyt.south_west(c));
            CHECK(port_direction_to_coordinate(lyt, c, port_direction{port_direction::cardinal::WEST}) == lyt.west(c));
            CHECK(port_direction_to_coordinate(lyt, c, port_direction{port_direction::cardinal::NORTH_WEST}) ==
                  lyt.north_west(c));
        });
}

TEST_CASE("Generate random coords::offset coordinate", "[layout-utils]")
{
    SECTION("two identical cells as input")
    {
        const auto randomly_generated_coordinate = random_coordinate<coords::offset>({0, 0, 0}, {0, 0, 0});
        CHECK(randomly_generated_coordinate.x == 0);
        CHECK(randomly_generated_coordinate.y == 0);
        CHECK(randomly_generated_coordinate.z == 0);

        const auto randomly_generated_coordinate_second = random_coordinate<coords::offset>({1, 0, 0}, {1, 0, 0});
        CHECK(randomly_generated_coordinate_second.x == 1);
        CHECK(randomly_generated_coordinate_second.y == 0);
        CHECK(randomly_generated_coordinate_second.z == 0);
    }

    SECTION("two unidentical cells as input, correct order")
    {
        const auto randomly_generated_coordinate_second = random_coordinate<coords::offset>({1, 1, 1}, {5, 2, 3});
        CHECK(randomly_generated_coordinate_second.x >= 1);
        CHECK(randomly_generated_coordinate_second.x <= 5);
        CHECK(randomly_generated_coordinate_second.y <= 2);
        CHECK(randomly_generated_coordinate_second.y >= 0);
        CHECK(randomly_generated_coordinate_second.z <= 3);
        CHECK(randomly_generated_coordinate_second.z >= 1);
    }

    SECTION("two unidentical cells as input, switched correct order")
    {
        const auto randomly_generated_coordinate = random_coordinate<coords::offset>({5, 2, 3}, {1, 1, 1});
        CHECK(randomly_generated_coordinate.x >= 1);
        CHECK(randomly_generated_coordinate.x <= 5);
        CHECK(randomly_generated_coordinate.y <= 2);
        CHECK(randomly_generated_coordinate.y >= 0);
        CHECK(randomly_generated_coordinate.z <= 3);
        CHECK(randomly_generated_coordinate.z >= 1);
    }
}

TEST_CASE("Generate random coords::cube coordinate", "[layout-utils]")
{
    SECTION("two identical cells as input")
    {
        const auto randomly_generated_coordinate = random_coordinate<coords::cube>({-10, -5, 0}, {-10, -5, 0});
        CHECK(randomly_generated_coordinate.x == -10);
        CHECK(randomly_generated_coordinate.y == -5);
        CHECK(randomly_generated_coordinate.z == 0);

        const auto randomly_generated_coordinate_second = random_coordinate<coords::cube>({1, 0, 0}, {1, 0, 0});
        CHECK(randomly_generated_coordinate_second.x == 1);
        CHECK(randomly_generated_coordinate_second.y == 0);
        CHECK(randomly_generated_coordinate_second.z == 0);
    }

    SECTION("two unidentical cells as input, correct order")
    {
        const auto randomly_generated_coordinate = random_coordinate<coords::cube>({-10, -1, 3}, {-10, -1, 6});
        CHECK(randomly_generated_coordinate.x == -10);
        CHECK(randomly_generated_coordinate.y == -1);
        CHECK(randomly_generated_coordinate.z >= 3);
        CHECK(randomly_generated_coordinate.z <= 6);
    }

    SECTION("two unidentical cells as input, switched correct order")
    {
        const auto randomly_generated_coordinate = random_coordinate<coords::cube>({-10, -1, 6}, {-10, -1, 3});
        CHECK(randomly_generated_coordinate.x == -10);
        CHECK(randomly_generated_coordinate.y == -1);
        CHECK(randomly_generated_coordinate.z >= 3);
        CHECK(randomly_generated_coordinate.z <= 6);
    }
}

TEST_CASE("Test identity of two layouts", "[layout-utils]")
{
    sidb_cell_clk_lyt lyt_first{{5, 3}};

    lyt_first.assign_cell_type({5, 3}, sidb_cell_clk_lyt::cell_type::NORMAL);
    lyt_first.assign_cell_type({0, 0}, sidb_cell_clk_lyt::cell_type::INPUT);
    lyt_first.assign_cell_type({1, 1}, sidb_cell_clk_lyt::cell_type::INPUT);
    lyt_first.assign_cell_type({2, 2}, sidb_cell_clk_lyt::cell_type::OUTPUT);

    auto lyt_second{lyt_first.clone()};

    SECTION("cell-level layout")
    {
        SECTION("identical layouts")
        {
            CHECK(are_cell_layouts_identical(lyt_first, lyt_second));
        }
        SECTION("different cell type")
        {
            lyt_second.assign_cell_type({5, 3}, sidb_cell_clk_lyt::cell_type::INPUT);
            CHECK(!are_cell_layouts_identical(lyt_first, lyt_second));
        }
        SECTION("different number of cells")
        {
            lyt_second.assign_cell_type({5, 3}, sidb_cell_clk_lyt::cell_type::EMPTY);
            CHECK(!are_cell_layouts_identical(lyt_first, lyt_second));
        }
    }
}

TEST_CASE("Digest of a cell-level layout", "[layout-utils]")
{
    sidb_cell_clk_lyt lyt_first{{5, 3}};

    lyt_first.assign_cell_type({5, 3}, sidb_cell_clk_lyt::cell_type::NORMAL);
    lyt_first.assign_cell_type({0, 0}, sidb_cell_clk_lyt::cell_type::INPUT);
    lyt_first.assign_cell_type({1, 1}, sidb_cell_clk_lyt::cell_type::INPUT);
    lyt_first.assign_cell_type({2, 2}, sidb_cell_clk_lyt::cell_type::OUTPUT);

    auto lyt_second{lyt_first.clone()};

    SECTION("cell-level layout")
    {
        SECTION("identical layouts share a digest")
        {
            CHECK(cell_layout_digest(lyt_first) == cell_layout_digest(lyt_second));
        }
        SECTION("the digest ignores the aspect ratio")
        {
            lyt_second.resize({42, 42});
            REQUIRE(are_cell_layouts_identical(lyt_first, lyt_second));
            CHECK(cell_layout_digest(lyt_first) == cell_layout_digest(lyt_second));
        }
        SECTION("the digest ignores the order in which the cells were assigned")
        {
            sidb_cell_clk_lyt lyt_reversed{{5, 3}};

            lyt_reversed.assign_cell_type({2, 2}, sidb_cell_clk_lyt::cell_type::OUTPUT);
            lyt_reversed.assign_cell_type({1, 1}, sidb_cell_clk_lyt::cell_type::INPUT);
            lyt_reversed.assign_cell_type({0, 0}, sidb_cell_clk_lyt::cell_type::INPUT);
            lyt_reversed.assign_cell_type({5, 3}, sidb_cell_clk_lyt::cell_type::NORMAL);

            REQUIRE(are_cell_layouts_identical(lyt_first, lyt_reversed));
            CHECK(cell_layout_digest(lyt_first) == cell_layout_digest(lyt_reversed));
        }
        // the contract allows different layouts to share a digest, so the inequality checks below measure how well
        // the digest separates a single changed attribute rather than a promise it makes. A failure means the
        // digest stopped covering that attribute, not that a caller broke
        SECTION("different cell type")
        {
            lyt_second.assign_cell_type({5, 3}, sidb_cell_clk_lyt::cell_type::INPUT);
            CHECK(cell_layout_digest(lyt_first) != cell_layout_digest(lyt_second));
        }
        SECTION("different cell position")
        {
            lyt_second.assign_cell_type({5, 3}, sidb_cell_clk_lyt::cell_type::EMPTY);
            lyt_second.assign_cell_type({4, 3}, sidb_cell_clk_lyt::cell_type::NORMAL);
            CHECK(cell_layout_digest(lyt_first) != cell_layout_digest(lyt_second));
        }
        SECTION("different number of cells")
        {
            lyt_second.assign_cell_type({5, 3}, sidb_cell_clk_lyt::cell_type::EMPTY);
            CHECK(cell_layout_digest(lyt_first) != cell_layout_digest(lyt_second));
        }
    }
}

TEST_CASE("Digest of a layout holding a cell and its dead twin", "[layout-utils]")
{
    // the coordinate types order by x, y, and z alone, while comparing and hashing the dead indicator as well. An
    // ordered fold over the cells therefore treats a cell and its dead twin as one entry and keeps whichever of
    // the two foreach_cell reaches first, which makes the digest depend on the order the cells were assigned in.
    // Both layouts below hold the same two cells and differ only in that order
    const coords::offset live_cell{0, 2};
    const auto           dead_twin = live_cell.get_dead();

    sidb_cell_clk_lyt lyt_live_first{};
    lyt_live_first.assign_cell_type(live_cell, sidb_cell_clk_lyt::cell_type::NORMAL);
    lyt_live_first.assign_cell_type(dead_twin, sidb_cell_clk_lyt::cell_type::NORMAL);

    sidb_cell_clk_lyt lyt_dead_first{};
    lyt_dead_first.assign_cell_type(dead_twin, sidb_cell_clk_lyt::cell_type::NORMAL);
    lyt_dead_first.assign_cell_type(live_cell, sidb_cell_clk_lyt::cell_type::NORMAL);

    REQUIRE(lyt_live_first.num_cells() == 2);
    REQUIRE(lyt_dead_first.num_cells() == 2);
    REQUIRE(are_cell_layouts_identical(lyt_live_first, lyt_dead_first));

    CHECK(cell_layout_digest(lyt_live_first) == cell_layout_digest(lyt_dead_first));
}
