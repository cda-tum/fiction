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
 * @brief Tests for `fiction/layouts/cartesian_layout.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Willem Lambooy (wlambooy)
 * @author Simon Hofmann (simon1hofmann)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/layout_base.hpp>

#include <utility>
#include <vector>

using namespace fiction;
using namespace fiction::layouts;

TEST_CASE("Cartesian neighbors report missing positions", "[cartesian-layout][size-contract]")
{
    using coordinate = layout_base::coordinate;
    const cartesian_layout layout{{3, 3, 2}};
    const coordinate       center{1, 1};
    CHECK(layout.north(center) == coordinate{1, 0});
    CHECK(layout.north_east(center) == coordinate{2, 0});
    CHECK(layout.east(center) == coordinate{2, 1});
    CHECK(layout.south_east(center) == coordinate{2, 2});
    CHECK(layout.south(center) == coordinate{1, 2});
    CHECK(layout.south_west(center) == coordinate{0, 2});
    CHECK(layout.west(center) == coordinate{0, 1});
    CHECK(layout.north_west(center) == coordinate{0, 0});
    CHECK(layout.above(center) == coordinate{1, 1, 1});
    CHECK(layout.below({1, 1, 1}) == center);
    CHECK_FALSE(layout.north({1, 0}));
    CHECK_FALSE(layout.north_east({1, 0}));
    CHECK_FALSE(layout.east({2, 1}));
    CHECK_FALSE(layout.south_east({2, 1}));
    CHECK_FALSE(layout.south({1, 2}));
    CHECK_FALSE(layout.south_west({1, 2}));
    CHECK_FALSE(layout.west({0, 1}));
    CHECK_FALSE(layout.north_west({0, 1}));
    CHECK_FALSE(layout.above({1, 1, 1}));
    CHECK_FALSE(layout.below(center));
    CHECK_FALSE(layout.east({-1, 0}));
    CHECK_FALSE(layout.north({0, 1, -1}));
    CHECK(layout.is_at_northern_border({1, 0}));
    CHECK(layout.is_at_eastern_border({2, 1}));
    CHECK(layout.is_at_southern_border({1, 2}));
    CHECK(layout.is_at_western_border({0, 1}));
    CHECK_FALSE(layout.is_at_northern_border({8, 0}));
    CHECK(layout.northern_border_of(center) == coordinate{1, 0});
    CHECK(layout.eastern_border_of(center) == coordinate{2, 1});
    CHECK(layout.southern_border_of(center) == coordinate{1, 2});
    CHECK(layout.western_border_of(center) == coordinate{0, 1});
    const auto adjacent = layout.adjacent_coordinates(center);
    CHECK_THAT(adjacent,
               Catch::Matchers::UnorderedRangeEquals(std::vector<coordinate>{{0, 1}, {1, 0}, {2, 1}, {1, 2}}));
    CHECK_THAT(layout.adjacent_coordinates({0, 0}),
               Catch::Matchers::UnorderedRangeEquals(std::vector<coordinate>{{1, 0}, {0, 1}}));
    CHECK_THAT(layout.adjacent_opposite_coordinates(center),
               Catch::Matchers::UnorderedRangeEquals(
                   std::vector<std::pair<coordinate, coordinate>>{{{1, 0}, {1, 2}}, {{2, 1}, {0, 1}}}));
    CHECK(layout.adjacent_opposite_coordinates({0, 0}).empty());
}

TEST_CASE("Cartesian predicates ignore frame bounds", "[cartesian-layout][size-contract]")
{
    const cartesian_layout layout{};
    CHECK(layout.is_north_of({-3, -2}, {-3, -3}));
    CHECK(layout.is_east_of({-3, -2}, {-2, -2}));
    CHECK(layout.is_south_of({-3, -2}, {-3, -1}));
    CHECK(layout.is_west_of({-3, -2}, {-4, -2}));
    CHECK(layout.is_above({-3, -2, 8}, {-3, -2, 9}));
    CHECK(layout.is_below({-3, -2, -8}, {-3, -2, -9}));
    CHECK(layout.is_adjacent_of({-3, -2}, {-2, -2}));
    CHECK(layout.is_adjacent_elevation_of({-3, -2, 8}, {-2, -2, 9}));
    CHECK_FALSE(layout.is_adjacent_elevation_of({-3, -2, 8}, {-2, -2, 10}));
    CHECK_FALSE(layout.is_east_of({2147483647, 0}, {-2147483648ll, 0}));
    /** @brief Geometry covering every nonnegative signed coordinate. */
    const cartesian_layout wide{{2147483648ull, 2147483648ull, 1}};
    CHECK(wide.eastern_border_of({0, 0}) == layout_base::coordinate{2147483647, 0});
    CHECK(wide.southern_border_of({0, 0}) == layout_base::coordinate{0, 2147483647});
    CHECK_FALSE(wide.east({2147483647, 0}));
    CHECK(wide.west({2147483647, 0}) == layout_base::coordinate{2147483646, 0});
}
