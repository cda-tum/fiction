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

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/layout_base.hpp>
#include <fiction/traits.hpp>

#include <set>
#include <stdexcept>

using namespace fiction;
using namespace fiction::layouts;

TEST_CASE("Cartesian layout traits", "[cartesian-layout]")
{
    using layout = cartesian_layout;

    CHECK(has_north_v<layout>);
    CHECK(has_east_v<layout>);
    CHECK(has_south_v<layout>);
    CHECK(has_west_v<layout>);
    CHECK(has_cardinal_operations_v<layout>);
    CHECK(has_north_east_v<layout>);
    CHECK(has_south_east_v<layout>);
    CHECK(has_south_west_v<layout>);
    CHECK(has_north_west_v<layout>);
    CHECK(has_ordinal_operations_v<layout>);
    CHECK(has_above_v<layout>);
    CHECK(has_below_v<layout>);
    CHECK(has_elevation_operations_v<layout>);
    CHECK(is_coordinate_layout_v<layout>);
    CHECK(is_cartesian_layout_v<layout>);
    CHECK(!is_gate_level_layout_v<layout>);
    CHECK(!is_hexagonal_layout_v<layout>);

    CHECK(has_foreach_coordinate_v<layout>);
    CHECK(has_foreach_adjacent_coordinate_v<layout>);
    CHECK(has_foreach_adjacent_opposite_coordinates_v<layout>);
}

TEST_CASE("Cartesian sizes and value copies", "[cartesian-layout][size-contract]")
{
    const cartesian_layout empty{};
    CHECK(empty.area() == 0);
    CHECK(empty.volume() == 0);
    CHECK(empty.coordinates().empty());
    CHECK(empty.ground_coordinates().empty());
    CHECK(!empty.contains_coordinate({0, 0, 0}));
    CHECK(!empty.last_coordinate());
    CHECK(!empty.northern_border_of({0, 0}));
    CHECK(!empty.eastern_border_of({0, 0}));
    CHECK(!empty.southern_border_of({0, 0}));
    CHECK(!empty.western_border_of({0, 0}));
    CHECK(!empty.is_at_any_border({0, 0}));
    const cartesian_layout original{{5, 4, 3}};
    auto                   copy = original;
    copy.resize({10, 9, 8});
    CHECK(original.dimensions() == layout_base::extent{5, 4, 3});
    CHECK(original.clone().dimensions() == original.dimensions());
    CHECK(copy.dimensions() == layout_base::extent{10, 9, 8});
    CHECK(original.width() == 5);
    CHECK(original.height() == 4);
    CHECK(original.layers() == 3);
    CHECK(original.last_coordinate() == layout_base::coordinate{4, 3, 2});
    CHECK(original.coord(-1, 2, 7) == layout_base::coordinate{-1, 2, 7});
    CHECK(!original.contains_coordinate({5, 0}));
    CHECK(!original.contains_coordinate({0, 4}));
    CHECK(!original.contains_coordinate({0, 0, 3}));
    CHECK(!original.contains_coordinate({-1, 0}));
    for (const auto sizes : {layout_base::extent{0, 4, 3}, layout_base::extent{5, 0, 3}, layout_base::extent{5, 4, 0}})
    {
        const cartesian_layout layout{sizes};
        CHECK(!layout.last_coordinate());
        CHECK(layout.coordinates().empty());
        CHECK(layout.ground_coordinates().empty());
    }
}

TEST_CASE("Cartesian ranges enumerate half-open sizes", "[cartesian-layout][size-contract]")
{
    const cartesian_layout                 layout{{10, 10, 2}};
    std::set<cartesian_layout::coordinate> visited{};
    layout.foreach_coordinate(
        [&](const auto c)
        {
            CHECK(layout.contains_coordinate(c));
            CHECK(visited.insert(c).second);
        });
    CHECK(visited.size() == 200);
    visited.clear();
    layout.foreach_ground_coordinate(
        [&](const auto c)
        {
            CHECK(c.z == 0);
            CHECK(visited.insert(c).second);
        });
    CHECK(visited.size() == 100);
    visited.clear();
    for (const auto c : layout.coordinates(cartesian_layout::coordinate{2, 2}, cartesian_layout::coordinate{5, 4}))
    {
        CHECK(visited.insert(c).second);
    }
    CHECK(visited.size() == 23);
}

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
    CHECK(!layout.north({1, 0}));
    CHECK(!layout.north_east({1, 0}));
    CHECK(!layout.east({2, 1}));
    CHECK(!layout.south_east({2, 1}));
    CHECK(!layout.south({1, 2}));
    CHECK(!layout.south_west({1, 2}));
    CHECK(!layout.west({0, 1}));
    CHECK(!layout.north_west({0, 1}));
    CHECK(!layout.above({1, 1, 1}));
    CHECK(!layout.below(center));
    CHECK(!layout.east({-1, 0}));
    CHECK(!layout.north({0, 1, -1}));
    CHECK(layout.is_at_northern_border({1, 0}));
    CHECK(layout.is_at_eastern_border({2, 1}));
    CHECK(layout.is_at_southern_border({1, 2}));
    CHECK(layout.is_at_western_border({0, 1}));
    CHECK(!layout.is_at_northern_border({8, 0}));
    CHECK(layout.northern_border_of(center) == coordinate{1, 0});
    CHECK(layout.eastern_border_of(center) == coordinate{2, 1});
    CHECK(layout.southern_border_of(center) == coordinate{1, 2});
    CHECK(layout.western_border_of(center) == coordinate{0, 1});
    const auto adjacent = layout.adjacent_coordinates(center);
    CHECK(std::set<coordinate>{adjacent.begin(), adjacent.end()} ==
          std::set<coordinate>{{0, 1}, {1, 0}, {2, 1}, {1, 2}});
    CHECK(layout.adjacent_coordinates({0, 0}).size() == 2);
    CHECK(layout.adjacent_opposite_coordinates(center).size() == 2);
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
    CHECK(!layout.is_adjacent_elevation_of({-3, -2, 8}, {-2, -2, 10}));
    CHECK(!layout.is_east_of({2147483647, 0}, {-2147483648ll, 0}));
    /** @brief Geometry covering every nonnegative signed coordinate. */
    const cartesian_layout wide{{2147483648ull, 2147483648ull, 1}};
    CHECK(wide.eastern_border_of({0, 0}) == layout_base::coordinate{2147483647, 0});
    CHECK(wide.southern_border_of({0, 0}) == layout_base::coordinate{0, 2147483647});
    CHECK(!wide.east({2147483647, 0}));
    CHECK(wide.west({2147483647, 0}) == layout_base::coordinate{2147483646, 0});
}

TEST_CASE("Cartesian neighbor callbacks propagate exceptions", "[cartesian-layout][neighbor-exceptions]")
{
    const cartesian_layout layout{{3, 3}};
    CHECK_THROWS_AS(
        layout.foreach_adjacent_coordinate({1, 1}, [](const auto&) { throw std::runtime_error("callback"); }),
        std::runtime_error);
    CHECK_THROWS_AS(
        layout.foreach_adjacent_opposite_coordinates({1, 1}, [](const auto&) { throw std::runtime_error("callback"); }),
        std::runtime_error);
}
