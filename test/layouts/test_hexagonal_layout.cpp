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
 * @brief Tests for `fiction/layouts/hexagonal_layout.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <fiction/layouts/arrangement.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/layouts/layout_base.hpp>
#include <fiction/traits.hpp>

#include <set>
#include <stdexcept>

using namespace fiction;
using namespace fiction::layouts;

/** Checks the geometry contract for a layout type. */
template <typename Lyt>
void check_common_traits()
{
    CHECK(has_north_v<Lyt>);
    CHECK(has_east_v<Lyt>);
    CHECK(has_south_v<Lyt>);
    CHECK(has_west_v<Lyt>);
    CHECK(has_cardinal_operations_v<Lyt>);
    CHECK(has_north_east_v<Lyt>);
    CHECK(has_south_east_v<Lyt>);
    CHECK(has_south_west_v<Lyt>);
    CHECK(has_north_west_v<Lyt>);
    CHECK(has_ordinal_operations_v<Lyt>);
    CHECK(has_above_v<Lyt>);
    CHECK(has_below_v<Lyt>);
    CHECK(has_elevation_operations_v<Lyt>);
    CHECK(is_coordinate_layout_v<Lyt>);
    CHECK(!is_gate_level_layout_v<Lyt>);
    CHECK(!is_cartesian_layout_v<Lyt>);
    CHECK(is_hexagonal_layout_v<Lyt>);

    CHECK(has_foreach_coordinate_v<Lyt>);
    CHECK(has_foreach_adjacent_coordinate_v<Lyt>);
    CHECK(has_foreach_adjacent_opposite_coordinates_v<Lyt>);
}

TEST_CASE("Hexagonal layout traits", "[hexagonal-layout]")
{
    using layout = hexagonal_layout;

    CHECK(!is_shifted_cartesian_layout_v<layout>);

    check_common_traits<layout>();
}

TEST_CASE("Hexagonal layout arrangement", "[hexagonal-layout]")
{
    const auto a =
        GENERATE(arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN);

    const hexagonal_layout lyt{a, {3, 3}};

    CHECK(lyt.get_arrangement() == a);
    CHECK(lyt.clone().get_arrangement() == a);
    CHECK(hexagonal_layout{lyt}.get_arrangement() == a);
}

TEST_CASE("Deep copy hexagonal layout", "[hexagonal-layout]")
{
    const hexagonal_layout original{arrangement::EVEN_ROW, {5, 5, 0}};

    auto copy = original.clone();

    copy.resize({10, 10, 1});

    CHECK(original.width() == 5);
    CHECK(original.height() == 5);
    CHECK(original.layers() == 0);

    CHECK(copy.width() == 10);
    CHECK(copy.height() == 10);
    CHECK(copy.layers() == 1);
}

/** Checks the geometry contract for a layout type. */
template <typename Lyt>
void check_identity_conversion(const arrangement a)
{
    Lyt layout{a, typename Lyt::extent{10, 10}};

    layout.foreach_coordinate([&layout](const auto& coord)
                              { CHECK(layout.to_offset_coordinate(layout.to_cube_coordinate(coord)) == coord); });
}

TEST_CASE("Coordinate creation", "[hexagonal-layout]")
{
    SECTION("odd row")
    {
        using layout       = hexagonal_layout;
        constexpr auto arr = arrangement::ODD_ROW;

        const layout lyt{arr, {3, 3}};

        CHECK(lyt.coord(0, 0, 0) == layout_base::coordinate{0, 0, 0});
        CHECK(lyt.coord(0, 0, 1) == layout_base::coordinate{0, 0, 1});
        CHECK(lyt.coord(1, 0) == layout_base::coordinate{1, 0});
        CHECK(lyt.coord(2, 0) == layout_base::coordinate{2, 0});
        CHECK(lyt.coord(0, 1) == layout_base::coordinate{0, 1});
        CHECK(lyt.coord(1, 1) == layout_base::coordinate{1, 1});
        CHECK(lyt.coord(2, 1) == layout_base::coordinate{2, 1});
    }
    SECTION("even row")
    {
        using layout       = hexagonal_layout;
        constexpr auto arr = arrangement::EVEN_ROW;

        const layout lyt{arr, {3, 3}};

        CHECK(lyt.coord(0, 0, 0) == layout_base::coordinate{0, 0, 0});
        CHECK(lyt.coord(0, 0, 1) == layout_base::coordinate{0, 0, 1});
        CHECK(lyt.coord(1, 0) == layout_base::coordinate{1, 0});
        CHECK(lyt.coord(2, 0) == layout_base::coordinate{2, 0});
        CHECK(lyt.coord(0, 1) == layout_base::coordinate{0, 1});
        CHECK(lyt.coord(1, 1) == layout_base::coordinate{1, 1});
        CHECK(lyt.coord(2, 1) == layout_base::coordinate{2, 1});
    }
    SECTION("odd column")
    {
        using layout       = hexagonal_layout;
        constexpr auto arr = arrangement::ODD_COLUMN;

        const layout lyt{arr, {3, 3}};

        CHECK(lyt.coord(0, 0, 0) == layout_base::coordinate{0, 0, 0});
        CHECK(lyt.coord(0, 0, 1) == layout_base::coordinate{0, 0, 1});
        CHECK(lyt.coord(1, 0) == layout_base::coordinate{1, 0});
        CHECK(lyt.coord(2, 0) == layout_base::coordinate{2, 0});
        CHECK(lyt.coord(0, 1) == layout_base::coordinate{0, 1});
        CHECK(lyt.coord(1, 1) == layout_base::coordinate{1, 1});
        CHECK(lyt.coord(2, 1) == layout_base::coordinate{2, 1});
    }
    SECTION("even column")
    {
        using layout       = hexagonal_layout;
        constexpr auto arr = arrangement::EVEN_COLUMN;

        const layout lyt{arr, {3, 3}};

        CHECK(lyt.coord(0, 0, 0) == layout_base::coordinate{0, 0, 0});
        CHECK(lyt.coord(0, 0, 1) == layout_base::coordinate{0, 0, 1});
        CHECK(lyt.coord(1, 0) == layout_base::coordinate{1, 0});
        CHECK(lyt.coord(2, 0) == layout_base::coordinate{2, 0});
        CHECK(lyt.coord(0, 1) == layout_base::coordinate{0, 1});
        CHECK(lyt.coord(1, 1) == layout_base::coordinate{1, 1});
        CHECK(lyt.coord(2, 1) == layout_base::coordinate{2, 1});
    }
}

TEST_CASE("Coordinate conversions", "[hexagonal-layout]")
{
    SECTION("odd row")
    {
        using layout       = hexagonal_layout;
        constexpr auto arr = arrangement::ODD_ROW;
        check_identity_conversion<layout>(arr);

        const layout lyt{arr, {3, 3}};

        CHECK(lyt.to_cube_coordinate({0, 0}) == typename layout::cube_coordinate{0, 0, 0});
        CHECK(lyt.to_cube_coordinate({1, 0}) == typename layout::cube_coordinate{+1, -1, 0});
        CHECK(lyt.to_cube_coordinate({2, 0}) == typename layout::cube_coordinate{+2, -2, 0});
        CHECK(lyt.to_cube_coordinate({0, 1}) == typename layout::cube_coordinate{0, -1, +1});
        CHECK(lyt.to_cube_coordinate({1, 1}) == typename layout::cube_coordinate{+1, -2, +1});
        CHECK(lyt.to_cube_coordinate({2, 1}) == typename layout::cube_coordinate{+2, -3, +1});
    }
    SECTION("even row")
    {
        using layout       = hexagonal_layout;
        constexpr auto arr = arrangement::EVEN_ROW;
        check_identity_conversion<layout>(arr);

        const layout lyt{arr, {3, 3}};

        CHECK(lyt.to_cube_coordinate({0, 0}) == typename layout::cube_coordinate{0, 0, 0});
        CHECK(lyt.to_cube_coordinate({1, 0}) == typename layout::cube_coordinate{+1, -1, 0});
        CHECK(lyt.to_cube_coordinate({2, 0}) == typename layout::cube_coordinate{+2, -2, 0});
        CHECK(lyt.to_cube_coordinate({0, 1}) == typename layout::cube_coordinate{-1, 0, +1});
        CHECK(lyt.to_cube_coordinate({1, 1}) == typename layout::cube_coordinate{0, -1, +1});
        CHECK(lyt.to_cube_coordinate({2, 1}) == typename layout::cube_coordinate{+1, -2, +1});
    }
    SECTION("odd column")
    {
        using layout       = hexagonal_layout;
        constexpr auto arr = arrangement::ODD_COLUMN;
        check_identity_conversion<layout>(arr);

        const layout lyt{arr, {3, 3}};

        CHECK(lyt.to_cube_coordinate({0, 0}) == typename layout::cube_coordinate{0, 0, 0});
        CHECK(lyt.to_cube_coordinate({1, 0}) == typename layout::cube_coordinate{+1, -1, 0});
        CHECK(lyt.to_cube_coordinate({2, 0}) == typename layout::cube_coordinate{+2, -1, -1});
        CHECK(lyt.to_cube_coordinate({0, 1}) == typename layout::cube_coordinate{0, -1, +1});
        CHECK(lyt.to_cube_coordinate({1, 1}) == typename layout::cube_coordinate{+1, -2, +1});
        CHECK(lyt.to_cube_coordinate({2, 1}) == typename layout::cube_coordinate{+2, -2, 0});
    }
    SECTION("even column")
    {
        using layout       = hexagonal_layout;
        constexpr auto arr = arrangement::EVEN_COLUMN;
        check_identity_conversion<layout>(arr);

        const layout lyt{arr, {3, 3}};

        CHECK(lyt.to_cube_coordinate({0, 0}) == typename layout::cube_coordinate{0, 0, 0});
        CHECK(lyt.to_cube_coordinate({1, 0}) == typename layout::cube_coordinate{+1, 0, -1});
        CHECK(lyt.to_cube_coordinate({2, 0}) == typename layout::cube_coordinate{+2, -1, -1});
        CHECK(lyt.to_cube_coordinate({0, 1}) == typename layout::cube_coordinate{0, -1, +1});
        CHECK(lyt.to_cube_coordinate({1, 1}) == typename layout::cube_coordinate{+1, -1, 0});
        CHECK(lyt.to_cube_coordinate({2, 1}) == typename layout::cube_coordinate{+2, -2, 0});
    }
}

/** Checks the geometry contract for a layout type. */
template <typename Lyt>
void check_visited_coordinates(const arrangement a)
{
    typename Lyt::extent ar{10, 10, 2};

    Lyt layout{a, ar};

    std::set<typename Lyt::coordinate> visited{};

    const auto check1 = [&visited, &layout](const auto& c)
    {
        CHECK(c <= *layout.last_coordinate());

        // all coordinates are within the layout bounds
        CHECK(layout.is_within_bounds(c));

        // no coordinate is visited twice
        CHECK(visited.count(c) == 0);
        visited.insert(c);
    };

    for (auto&& t : layout.coordinates())
    {
        check1(t);
    }
    CHECK(visited.size() == 200);

    visited.clear();

    layout.foreach_coordinate(check1);
    CHECK(visited.size() == 200);

    visited.clear();

    typename Lyt::coordinate ar_ground{9, 9, 0};

    const auto check2 = [&visited, &ar_ground, &layout](const auto& c)
    {
        // iteration stays in ground layer
        CHECK(c.z == 0);
        CHECK(c <= ar_ground);
        CHECK(layout.is_ground_layer(c));

        // all coordinates are within the layout bounds
        CHECK(layout.is_within_bounds(c));

        // no coordinate is visited twice
        CHECK(visited.count(c) == 0);
        visited.insert(c);
    };

    for (auto&& t : layout.ground_coordinates())
    {
        check2(t);
    }
    CHECK(visited.size() == 100);

    visited.clear();

    layout.foreach_ground_coordinate(check2);
    CHECK(visited.size() == 100);

    visited.clear();

    typename Lyt::coordinate start{2, 2}, stop{5, 4};

    const auto check3 = [&visited, &start, &stop, &layout](const auto& c)
    {
        CHECK(c.z == 0);
        // iteration stays in between the bounds
        CHECK(c >= start);
        CHECK(c < stop);

        // all coordinates are within the layout bounds
        CHECK(layout.is_within_bounds(c));

        // no coordinate is visited twice
        CHECK(visited.count(c) == 0);
        visited.insert(c);
    };

    for (auto&& t : layout.coordinates(start, stop))
    {
        check3(t);
    }
    CHECK(visited.size() == 23);

    visited.clear();

    layout.foreach_coordinate(check3, start, stop);
    CHECK(visited.size() == 23);
}

TEST_CASE("Hexagonal coordinate iteration", "[hexagonal-layout]")
{
    const auto a =
        GENERATE(arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN);

    check_visited_coordinates<hexagonal_layout>(a);
}

TEST_CASE("Cardinal and ordinal operations: odd row", "[hexagonal-layout]")
{
    using layout       = hexagonal_layout;
    constexpr auto arr = arrangement::ODD_ROW;

    const layout lyt{arr, {4, 4, 2}};

    const layout::coordinate c{2, 2};
    const layout::coordinate ac{2, 2, 1};

    const layout::coordinate nc{2, 1};
    const layout::coordinate nec{2, 1};
    const layout::coordinate ec{3, 2};
    const layout::coordinate sec{2, 3};
    const layout::coordinate sc{2, 3};
    const layout::coordinate swc{1, 3};
    const layout::coordinate wc{1, 2};
    const layout::coordinate nwc{1, 1};

    const layout::coordinate bnc{2, 0};
    const layout::coordinate bnec{3, 0};
    const layout::coordinate bec{3, 2};
    const layout::coordinate bsec{3, 3};
    const layout::coordinate bsc{2, 3};
    const layout::coordinate bswc{0, 3};
    const layout::coordinate bwc{0, 2};
    const layout::coordinate bnwc{0, 0};

    CHECK(lyt.is_above(c, ac));
    CHECK(lyt.is_below(ac, c));

    CHECK(lyt.north(c) == nc);
    CHECK(lyt.is_north_of(c, nc));
    CHECK(lyt.is_northwards_of(c, bnc));
    CHECK(lyt.northern_border_of(c) == bnc);
    CHECK(lyt.north_east(c) == nec);
    CHECK(lyt.east(c) == ec);
    CHECK(lyt.is_east_of(c, ec));
    CHECK(lyt.is_eastwards_of(c, bec));
    CHECK(lyt.eastern_border_of(c) == bec);
    CHECK(lyt.south_east(c) == sec);
    CHECK(lyt.south(c) == sc);
    CHECK(lyt.is_south_of(c, sc));
    CHECK(lyt.is_southwards_of(c, bsc));
    CHECK(lyt.southern_border_of(c) == bsc);
    CHECK(lyt.south_west(c) == swc);
    CHECK(lyt.west(c) == wc);
    CHECK(lyt.is_west_of(c, wc));
    CHECK(lyt.is_westwards_of(c, bwc));
    CHECK(lyt.western_border_of(c) == bwc);
    CHECK(lyt.north_west(c) == nwc);

    CHECK(lyt.is_adjacent_of(c, nc));
    CHECK(lyt.is_adjacent_of(c, nec));
    CHECK(lyt.is_adjacent_of(c, ec));
    CHECK(lyt.is_adjacent_of(c, sec));
    CHECK(lyt.is_adjacent_of(c, sc));
    CHECK(lyt.is_adjacent_of(c, swc));
    CHECK(lyt.is_adjacent_of(c, wc));
    CHECK(lyt.is_adjacent_of(c, nwc));

    // edge cases
    CHECK(!lyt.north(bnc));
    CHECK(!lyt.north_east(bnec));
    CHECK(!lyt.east(bec));
    CHECK(!lyt.south_east(bsec));
    CHECK(!lyt.south(bsc));
    CHECK(!lyt.south_west(bswc));
    CHECK(!lyt.west(bwc));
    CHECK(!lyt.north_west(bnwc));
}

TEST_CASE("Cardinal and ordinal operations: even row", "[hexagonal-layout]")
{
    using layout       = hexagonal_layout;
    constexpr auto arr = arrangement::EVEN_ROW;

    const layout lyt{arr, {4, 4, 2}};

    const layout::coordinate c{2, 2};
    const layout::coordinate ac{2, 2, 1};

    const layout::coordinate nc{2, 1};
    const layout::coordinate nec{3, 1};
    const layout::coordinate ec{3, 2};
    const layout::coordinate sec{3, 3};
    const layout::coordinate sc{2, 3};
    const layout::coordinate swc{2, 3};
    const layout::coordinate wc{1, 2};
    const layout::coordinate nwc{2, 1};

    const layout::coordinate bnc{2, 0};
    const layout::coordinate bnec{3, 0};
    const layout::coordinate bec{3, 2};
    const layout::coordinate bsec{3, 3};
    const layout::coordinate bsc{2, 3};
    const layout::coordinate bswc{0, 3};
    const layout::coordinate bwc{0, 2};
    const layout::coordinate bnwc{0, 0};

    CHECK(lyt.is_above(c, ac));
    CHECK(lyt.is_below(ac, c));

    CHECK(lyt.north(c) == nc);
    CHECK(lyt.is_north_of(c, nc));
    CHECK(lyt.is_northwards_of(c, bnc));
    CHECK(lyt.northern_border_of(c) == bnc);
    CHECK(lyt.north_east(c) == nec);
    CHECK(lyt.east(c) == ec);
    CHECK(lyt.is_east_of(c, ec));
    CHECK(lyt.is_eastwards_of(c, bec));
    CHECK(lyt.eastern_border_of(c) == bec);
    CHECK(lyt.south_east(c) == sec);
    CHECK(lyt.south(c) == sc);
    CHECK(lyt.is_south_of(c, sc));
    CHECK(lyt.is_southwards_of(c, bsc));
    CHECK(lyt.southern_border_of(c) == bsc);
    CHECK(lyt.south_west(c) == swc);
    CHECK(lyt.west(c) == wc);
    CHECK(lyt.is_west_of(c, wc));
    CHECK(lyt.is_westwards_of(c, bwc));
    CHECK(lyt.western_border_of(c) == bwc);
    CHECK(lyt.north_west(c) == nwc);

    CHECK(lyt.is_adjacent_of(c, nc));
    CHECK(lyt.is_adjacent_of(c, nec));
    CHECK(lyt.is_adjacent_of(c, ec));
    CHECK(lyt.is_adjacent_of(c, sec));
    CHECK(lyt.is_adjacent_of(c, sc));
    CHECK(lyt.is_adjacent_of(c, swc));
    CHECK(lyt.is_adjacent_of(c, wc));
    CHECK(lyt.is_adjacent_of(c, nwc));

    // edge cases
    CHECK(!lyt.north(bnc));
    CHECK(!lyt.north_east(bnec));
    CHECK(!lyt.east(bec));
    CHECK(!lyt.south_east(bsec));
    CHECK(!lyt.south(bsc));
    CHECK(!lyt.south_west(bswc));
    CHECK(!lyt.west(bwc));
    CHECK(!lyt.north_west(bnwc));
}

TEST_CASE("Cardinal and ordinal operations: odd column", "[hexagonal-layout]")
{
    using layout       = hexagonal_layout;
    constexpr auto arr = arrangement::ODD_COLUMN;

    const layout lyt{arr, {4, 4, 2}};

    const layout::coordinate c{2, 2};
    const layout::coordinate ac{2, 2, 1};

    const layout::coordinate nc{2, 1};
    const layout::coordinate nec{3, 1};
    const layout::coordinate ec{3, 2};
    const layout::coordinate sec{3, 2};
    const layout::coordinate sc{2, 3};
    const layout::coordinate swc{1, 2};
    const layout::coordinate wc{1, 2};
    const layout::coordinate nwc{1, 1};

    const layout::coordinate bnc{2, 0};
    const layout::coordinate bnec{3, 0};
    const layout::coordinate bec{3, 2};
    const layout::coordinate bsec{3, 3};
    const layout::coordinate bsc{2, 3};
    const layout::coordinate bswc{0, 3};
    const layout::coordinate bwc{0, 2};
    const layout::coordinate bnwc{0, 0};

    CHECK(lyt.is_above(c, ac));
    CHECK(lyt.is_below(ac, c));

    CHECK(lyt.north(c) == nc);
    CHECK(lyt.is_north_of(c, nc));
    CHECK(lyt.is_northwards_of(c, bnc));
    CHECK(lyt.northern_border_of(c) == bnc);
    CHECK(lyt.north_east(c) == nec);
    CHECK(lyt.east(c) == ec);
    CHECK(lyt.is_east_of(c, ec));
    CHECK(lyt.is_eastwards_of(c, bec));
    CHECK(lyt.eastern_border_of(c) == bec);
    CHECK(lyt.south_east(c) == sec);
    CHECK(lyt.south(c) == sc);
    CHECK(lyt.is_south_of(c, sc));
    CHECK(lyt.is_southwards_of(c, bsc));
    CHECK(lyt.southern_border_of(c) == bsc);
    CHECK(lyt.south_west(c) == swc);
    CHECK(lyt.west(c) == wc);
    CHECK(lyt.is_west_of(c, wc));
    CHECK(lyt.is_westwards_of(c, bwc));
    CHECK(lyt.western_border_of(c) == bwc);
    CHECK(lyt.north_west(c) == nwc);

    CHECK(lyt.is_adjacent_of(c, nc));
    CHECK(lyt.is_adjacent_of(c, nec));
    CHECK(lyt.is_adjacent_of(c, ec));
    CHECK(lyt.is_adjacent_of(c, sec));
    CHECK(lyt.is_adjacent_of(c, sc));
    CHECK(lyt.is_adjacent_of(c, swc));
    CHECK(lyt.is_adjacent_of(c, wc));
    CHECK(lyt.is_adjacent_of(c, nwc));

    // edge cases
    CHECK(!lyt.north(bnc));
    CHECK(!lyt.north_east(bnec));
    CHECK(!lyt.east(bec));
    CHECK(!lyt.south_east(bsec));
    CHECK(!lyt.south(bsc));
    CHECK(!lyt.south_west(bswc));
    CHECK(!lyt.west(bwc));
    CHECK(!lyt.north_west(bnwc));
}

TEST_CASE("Cardinal and ordinal operations: even column", "[hexagonal-layout]")
{
    using layout       = hexagonal_layout;
    constexpr auto arr = arrangement::EVEN_COLUMN;

    const layout lyt{arr, {4, 4, 2}};

    const layout::coordinate c{2, 2};
    const layout::coordinate ac{2, 2, 1};

    const layout::coordinate nc{2, 1};
    const layout::coordinate nec{3, 2};
    const layout::coordinate ec{3, 2};
    const layout::coordinate sec{3, 3};
    const layout::coordinate sc{2, 3};
    const layout::coordinate swc{1, 3};
    const layout::coordinate wc{1, 2};
    const layout::coordinate nwc{1, 2};

    const layout::coordinate bnc{2, 0};
    const layout::coordinate bnec{3, 0};
    const layout::coordinate bec{3, 2};
    const layout::coordinate bsec{3, 3};
    const layout::coordinate bsc{2, 3};
    const layout::coordinate bswc{0, 3};
    const layout::coordinate bwc{0, 2};
    const layout::coordinate bnwc{0, 0};

    CHECK(lyt.is_above(c, ac));
    CHECK(lyt.is_below(ac, c));

    CHECK(lyt.north(c) == nc);
    CHECK(lyt.is_north_of(c, nc));
    CHECK(lyt.is_northwards_of(c, bnc));
    CHECK(lyt.northern_border_of(c) == bnc);
    CHECK(lyt.north_east(c) == nec);
    CHECK(lyt.east(c) == ec);
    CHECK(lyt.is_east_of(c, ec));
    CHECK(lyt.is_eastwards_of(c, bec));
    CHECK(lyt.eastern_border_of(c) == bec);
    CHECK(lyt.south_east(c) == sec);
    CHECK(lyt.south(c) == sc);
    CHECK(lyt.is_south_of(c, sc));
    CHECK(lyt.is_southwards_of(c, bsc));
    CHECK(lyt.southern_border_of(c) == bsc);
    CHECK(lyt.south_west(c) == swc);
    CHECK(lyt.west(c) == wc);
    CHECK(lyt.is_west_of(c, wc));
    CHECK(lyt.is_westwards_of(c, bwc));
    CHECK(lyt.western_border_of(c) == bwc);
    CHECK(lyt.north_west(c) == nwc);

    CHECK(lyt.is_adjacent_of(c, nc));
    CHECK(lyt.is_adjacent_of(c, nec));
    CHECK(lyt.is_adjacent_of(c, ec));
    CHECK(lyt.is_adjacent_of(c, sec));
    CHECK(lyt.is_adjacent_of(c, sc));
    CHECK(lyt.is_adjacent_of(c, swc));
    CHECK(lyt.is_adjacent_of(c, wc));
    CHECK(lyt.is_adjacent_of(c, nwc));

    // edge cases
    CHECK(!lyt.north(bnc));
    CHECK(!lyt.north_east(bnec));
    CHECK(!lyt.east(bec));
    CHECK(!lyt.south_east(bsec));
    CHECK(!lyt.south(bsc));
    CHECK(!lyt.south_west(bswc));
    CHECK(!lyt.west(bwc));
    CHECK(!lyt.north_west(bnwc));
}

TEST_CASE("Coordinate adjacencies", "[hexagonal-layout]")
{
    SECTION("odd row")
    {
        using layout       = hexagonal_layout;
        constexpr auto arr = arrangement::ODD_ROW;

        const layout lyt{arr, {3, 3}};

        const auto adj00_v = lyt.adjacent_coordinates({0, 0});
        const auto adj00_s = std::set<layout::coordinate>{adj00_v.cbegin(), adj00_v.cend()};

        CHECK(std::set<layout::coordinate>{{{0, 1}, {1, 0}}} == adj00_s);

        const auto adj01_v = lyt.adjacent_coordinates({0, 1});
        const auto adj01_s = std::set<layout::coordinate>{adj01_v.cbegin(), adj01_v.cend()};

        CHECK(std::set<layout::coordinate>{{{0, 0}, {1, 0}, {1, 1}, {0, 2}, {1, 2}}} == adj01_s);

        const auto adj22_v = lyt.adjacent_coordinates({2, 2});
        const auto adj22_s = std::set<layout::coordinate>{adj22_v.cbegin(), adj22_v.cend()};

        CHECK(std::set<layout::coordinate>{{{1, 2}, {1, 1}, {2, 1}}} == adj22_s);
    }
    SECTION("even row")
    {
        using layout       = hexagonal_layout;
        constexpr auto arr = arrangement::EVEN_ROW;

        const layout lyt{arr, {3, 3}};

        const auto adj00_v = lyt.adjacent_coordinates({0, 0});
        const auto adj00_s = std::set<layout::coordinate>{adj00_v.cbegin(), adj00_v.cend()};

        CHECK(std::set<layout::coordinate>{{{0, 1}, {1, 0}, {1, 1}}} == adj00_s);

        const auto adj01_v = lyt.adjacent_coordinates({0, 1});
        const auto adj01_s = std::set<layout::coordinate>{adj01_v.cbegin(), adj01_v.cend()};

        CHECK(std::set<layout::coordinate>{{{0, 0}, {1, 1}, {0, 2}}} == adj01_s);

        const auto adj22_v = lyt.adjacent_coordinates({2, 2});
        const auto adj22_s = std::set<layout::coordinate>{adj22_v.cbegin(), adj22_v.cend()};

        CHECK(std::set<layout::coordinate>{{{1, 2}, {2, 1}}} == adj22_s);
    }
    SECTION("odd column")
    {
        using layout       = hexagonal_layout;
        constexpr auto arr = arrangement::ODD_COLUMN;

        const layout lyt{arr, {3, 3}};

        const auto adj00_v = lyt.adjacent_coordinates({0, 0});
        const auto adj00_s = std::set<layout::coordinate>{adj00_v.cbegin(), adj00_v.cend()};

        CHECK(std::set<layout::coordinate>{{{0, 1}, {1, 0}}} == adj00_s);

        const auto adj01_v = lyt.adjacent_coordinates({0, 1});
        const auto adj01_s = std::set<layout::coordinate>{adj01_v.cbegin(), adj01_v.cend()};

        CHECK(std::set<layout::coordinate>{{{0, 0}, {1, 0}, {1, 1}, {0, 2}}} == adj01_s);

        const auto adj22_v = lyt.adjacent_coordinates({2, 2});
        const auto adj22_s = std::set<layout::coordinate>{adj22_v.cbegin(), adj22_v.cend()};

        CHECK(std::set<layout::coordinate>{{{1, 2}, {1, 1}, {2, 1}}} == adj22_s);
    }
    SECTION("even column")
    {
        using layout       = hexagonal_layout;
        constexpr auto arr = arrangement::EVEN_COLUMN;

        const layout lyt{arr, {3, 3}};

        const auto adj00_v = lyt.adjacent_coordinates({0, 0});
        const auto adj00_s = std::set<layout::coordinate>{adj00_v.cbegin(), adj00_v.cend()};

        CHECK(std::set<layout::coordinate>{{{0, 1}, {1, 0}, {1, 1}}} == adj00_s);

        const auto adj01_v = lyt.adjacent_coordinates({0, 1});
        const auto adj01_s = std::set<layout::coordinate>{adj01_v.cbegin(), adj01_v.cend()};

        CHECK(std::set<layout::coordinate>{{{0, 0}, {1, 1}, {0, 2}, {1, 2}}} == adj01_s);

        const auto adj22_v = lyt.adjacent_coordinates({2, 2});
        const auto adj22_s = std::set<layout::coordinate>{adj22_v.cbegin(), adj22_v.cend()};

        CHECK(std::set<layout::coordinate>{{{1, 2}, {2, 1}}} == adj22_s);
    }
}

TEST_CASE("Hexagonal empty geometry and signed edge arithmetic", "[hexagonal-layout][size-contract]")
{
    const auto a =
        GENERATE(arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN);
    const hexagonal_layout empty{a};
    CHECK(empty.coordinates().empty());
    CHECK(empty.ground_coordinates().empty());
    CHECK(!empty.last_coordinate());
    CHECK(!empty.north({0, 0}));
    CHECK(!empty.north_east({0, 0}));
    CHECK(!empty.east({0, 0}));
    CHECK(!empty.south_east({0, 0}));
    CHECK(!empty.south({0, 0}));
    CHECK(!empty.south_west({0, 0}));
    CHECK(!empty.west({0, 0}));
    CHECK(!empty.north_west({0, 0}));
    CHECK(empty.adjacent_coordinates({0, 0}).empty());
    CHECK(empty.is_above({-1, -2, 7}, {-1, -2, 8}));
    auto copy = empty;
    copy.resize({3, 4, 7});
    CHECK(empty.dimensions() == layout_base::extent{});
    CHECK(copy.dimensions() == layout_base::extent{3, 4, 7});
    for (const auto x : {-2147483648ll, -3ll, -2ll, -1ll, 0ll, 2147483647ll})
    {
        for (const auto y : {-2147483648ll, -3ll, -2ll, -1ll, 0ll, 2147483647ll})
        {
            const layout_base::coordinate c{x, y};
            CHECK(empty.to_offset_coordinate(empty.to_cube_coordinate(c)) == c);
            CHECK(!empty.is_adjacent_of(c, c));
        }
    }
    CHECK(empty.is_in_odd_row({0, -3}));
    CHECK(empty.is_in_even_row({0, -2}));
    CHECK(empty.is_in_odd_column({-3, 0}));
    CHECK(empty.is_in_even_column({-2, 0}));
    CHECK(!empty.to_offset_coordinate({std::numeric_limits<int64_t>::max(), 0, 0}));
    CHECK(!empty.to_offset_coordinate({0, 0, std::numeric_limits<int64_t>::min()}));
    const hexagonal_layout wide{a, {2147483648ull, 2147483648ull, 2}};
    CHECK(!wide.east({2147483647, 0}));
    CHECK(!wide.south({0, 2147483647}));
    for (const auto c : {layout_base::coordinate{2147483647, 2147483647}, layout_base::coordinate{0, 0}})
    {
        const auto adjacent = wide.adjacent_coordinates(c);
        for (const auto n : adjacent)
        {
            CHECK(wide.contains_coordinate(n));
            CHECK(empty.is_adjacent_of(c, n));
            CHECK(empty.is_adjacent_of(n, c));
            CHECK(empty.is_adjacent_elevation_of(c, {n.x, n.y, 1}));
        }
    }
}

TEST_CASE("Hexagonal neighbor callbacks propagate exceptions", "[hexagonal-layout][neighbor-exceptions]")
{
    const hexagonal_layout layout{arrangement::ODD_ROW, {3, 3}};
    CHECK_THROWS_AS(
        layout.foreach_adjacent_coordinate({1, 1}, [](const auto&) { throw std::runtime_error("callback"); }),
        std::runtime_error);
    CHECK_THROWS_AS(
        layout.foreach_adjacent_opposite_coordinates({1, 1}, [](const auto&) { throw std::runtime_error("callback"); }),
        std::runtime_error);
}
