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
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/layouts/layout_utils.hpp>

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
