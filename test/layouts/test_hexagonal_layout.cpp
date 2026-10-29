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

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include <fiction/layouts/arrangement.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/layouts/layout_base.hpp>

#include <cstdint>
#include <limits>
#include <tuple>
#include <vector>

using namespace fiction;
using namespace fiction::layouts;

/**
 * @brief Checks the geometry contract for a layout type.
 * @tparam Lyt Coordinate layout type.
 * @param a Arrangement of the shifted rows or columns.
 */
template <typename Lyt>
void check_identity_conversion(const arrangement a)
{
    Lyt layout{a, typename Lyt::extent{10, 10}};

    layout.foreach_coordinate([&layout](const auto& coord)
                              { CHECK(layout.to_offset_coordinate(layout.to_cube_coordinate(coord)) == coord); });
}

TEST_CASE("Coordinate creation", "[hexagonal-layout]")
{
    const auto arr =
        GENERATE(arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN);
    CAPTURE(static_cast<int>(arr));
    const hexagonal_layout lyt{arr, {3, 3}};
    CHECK(lyt.coord(0, 0, 0) == layout_base::coordinate{0, 0, 0});
    CHECK(lyt.coord(0, 0, 1) == layout_base::coordinate{0, 0, 1});
    CHECK(lyt.coord(1, 0) == layout_base::coordinate{1, 0});
    CHECK(lyt.coord(2, 0) == layout_base::coordinate{2, 0});
    CHECK(lyt.coord(0, 1) == layout_base::coordinate{0, 1});
    CHECK(lyt.coord(1, 1) == layout_base::coordinate{1, 1});
    CHECK(lyt.coord(2, 1) == layout_base::coordinate{2, 1});
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

TEST_CASE("Cardinal and ordinal operations", "[hexagonal-layout]")
{
    using layout = hexagonal_layout;
    /** @brief Arrangement and its four independently expected diagonal positions. */
    const auto [arr, nec, sec, swc, nwc] =
        GENERATE(std::tuple{arrangement::ODD_ROW, layout::coordinate{2, 1}, layout::coordinate{2, 3},
                            layout::coordinate{1, 3}, layout::coordinate{1, 1}},
                 std::tuple{arrangement::EVEN_ROW, layout::coordinate{3, 1}, layout::coordinate{3, 3},
                            layout::coordinate{2, 3}, layout::coordinate{2, 1}},
                 std::tuple{arrangement::ODD_COLUMN, layout::coordinate{3, 1}, layout::coordinate{3, 2},
                            layout::coordinate{1, 2}, layout::coordinate{1, 1}},
                 std::tuple{arrangement::EVEN_COLUMN, layout::coordinate{3, 2}, layout::coordinate{3, 3},
                            layout::coordinate{1, 3}, layout::coordinate{1, 2}});
    CAPTURE(static_cast<int>(arr));

    const layout lyt{arr, {4, 4, 2}};

    const layout::coordinate c{2, 2};
    const layout::coordinate ac{2, 2, 1};

    const layout::coordinate nc{2, 1};
    const layout::coordinate ec{3, 2};
    const layout::coordinate sc{2, 3};
    const layout::coordinate wc{1, 2};

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
    CHECK_FALSE(lyt.north(bnc));
    CHECK_FALSE(lyt.north_east(bnec));
    CHECK_FALSE(lyt.east(bec));
    CHECK_FALSE(lyt.south_east(bsec));
    CHECK_FALSE(lyt.south(bsc));
    CHECK_FALSE(lyt.south_west(bswc));
    CHECK_FALSE(lyt.west(bwc));
    CHECK_FALSE(lyt.north_west(bnwc));
}

TEST_CASE("Coordinate adjacencies", "[hexagonal-layout]")
{
    SECTION("odd row")
    {
        using layout       = hexagonal_layout;
        constexpr auto arr = arrangement::ODD_ROW;

        const layout lyt{arr, {3, 3}};

        const auto adj00_v = lyt.adjacent_coordinates({0, 0});

        CHECK_THAT(adj00_v, Catch::Matchers::UnorderedRangeEquals(std::vector<layout::coordinate>{{0, 1}, {1, 0}}));

        const auto adj01_v = lyt.adjacent_coordinates({0, 1});

        CHECK_THAT(adj01_v, Catch::Matchers::UnorderedRangeEquals(
                                std::vector<layout::coordinate>{{0, 0}, {1, 0}, {1, 1}, {0, 2}, {1, 2}}));

        const auto adj22_v = lyt.adjacent_coordinates({2, 2});

        CHECK_THAT(adj22_v,
                   Catch::Matchers::UnorderedRangeEquals(std::vector<layout::coordinate>{{1, 2}, {1, 1}, {2, 1}}));
    }
    SECTION("even row")
    {
        using layout       = hexagonal_layout;
        constexpr auto arr = arrangement::EVEN_ROW;

        const layout lyt{arr, {3, 3}};

        const auto adj00_v = lyt.adjacent_coordinates({0, 0});

        CHECK_THAT(adj00_v,
                   Catch::Matchers::UnorderedRangeEquals(std::vector<layout::coordinate>{{0, 1}, {1, 0}, {1, 1}}));

        const auto adj01_v = lyt.adjacent_coordinates({0, 1});

        CHECK_THAT(adj01_v,
                   Catch::Matchers::UnorderedRangeEquals(std::vector<layout::coordinate>{{0, 0}, {1, 1}, {0, 2}}));

        const auto adj22_v = lyt.adjacent_coordinates({2, 2});

        CHECK_THAT(adj22_v, Catch::Matchers::UnorderedRangeEquals(std::vector<layout::coordinate>{{1, 2}, {2, 1}}));
    }
    SECTION("odd column")
    {
        using layout       = hexagonal_layout;
        constexpr auto arr = arrangement::ODD_COLUMN;

        const layout lyt{arr, {3, 3}};

        const auto adj00_v = lyt.adjacent_coordinates({0, 0});

        CHECK_THAT(adj00_v, Catch::Matchers::UnorderedRangeEquals(std::vector<layout::coordinate>{{0, 1}, {1, 0}}));

        const auto adj01_v = lyt.adjacent_coordinates({0, 1});

        CHECK_THAT(adj01_v, Catch::Matchers::UnorderedRangeEquals(
                                std::vector<layout::coordinate>{{0, 0}, {1, 0}, {1, 1}, {0, 2}}));

        const auto adj22_v = lyt.adjacent_coordinates({2, 2});

        CHECK_THAT(adj22_v,
                   Catch::Matchers::UnorderedRangeEquals(std::vector<layout::coordinate>{{1, 2}, {1, 1}, {2, 1}}));
    }
    SECTION("even column")
    {
        using layout       = hexagonal_layout;
        constexpr auto arr = arrangement::EVEN_COLUMN;

        const layout lyt{arr, {3, 3}};

        const auto adj00_v = lyt.adjacent_coordinates({0, 0});

        CHECK_THAT(adj00_v,
                   Catch::Matchers::UnorderedRangeEquals(std::vector<layout::coordinate>{{0, 1}, {1, 0}, {1, 1}}));

        const auto adj01_v = lyt.adjacent_coordinates({0, 1});

        CHECK_THAT(adj01_v, Catch::Matchers::UnorderedRangeEquals(
                                std::vector<layout::coordinate>{{0, 0}, {1, 1}, {0, 2}, {1, 2}}));

        const auto adj22_v = lyt.adjacent_coordinates({2, 2});

        CHECK_THAT(adj22_v, Catch::Matchers::UnorderedRangeEquals(std::vector<layout::coordinate>{{1, 2}, {2, 1}}));
    }
}

TEST_CASE("Hexagonal empty geometry and signed edge arithmetic", "[hexagonal-layout][size-contract]")
{
    const auto a =
        GENERATE(arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN);
    const hexagonal_layout empty{a};
    CHECK(empty.coordinates().empty());
    CHECK(empty.ground_coordinates().empty());
    CHECK_FALSE(empty.last_coordinate());
    CHECK_FALSE(empty.north({0, 0}));
    CHECK_FALSE(empty.north_east({0, 0}));
    CHECK_FALSE(empty.east({0, 0}));
    CHECK_FALSE(empty.south_east({0, 0}));
    CHECK_FALSE(empty.south({0, 0}));
    CHECK_FALSE(empty.south_west({0, 0}));
    CHECK_FALSE(empty.west({0, 0}));
    CHECK_FALSE(empty.north_west({0, 0}));
    CHECK(empty.adjacent_coordinates({0, 0}).empty());
    CHECK(empty.is_above({-1, -2, 7}, {-1, -2, 8}));
    auto copy = empty;
    copy.resize({3, 4, 2});
    CHECK(empty.get_extent() == layout_base::extent{});
    CHECK(copy.get_extent() == layout_base::extent{3, 4, 2});
    for (const auto x : {-2147483648ll, -2147483647ll, -7ll, -6ll, -5ll, -4ll, -3ll, -2ll, -1ll, 0ll, 1ll, 2ll, 3ll,
                         4ll, 5ll, 6ll, 7ll, 2147483647ll})
    {
        for (const auto y : {-2147483648ll, -2147483647ll, -7ll, -6ll, -5ll, -4ll, -3ll, -2ll, -1ll, 0ll, 1ll, 2ll, 3ll,
                             4ll, 5ll, 6ll, 7ll, 2147483647ll})
        {
            CAPTURE(static_cast<int>(a), x, y);
            const layout_base::coordinate c{x, y};
            const auto                    cube = empty.to_cube_coordinate(c);
            CHECK(cube.x + cube.y + cube.z == 0);
            CHECK(empty.to_offset_coordinate(cube) == c);
            CHECK_FALSE(empty.is_adjacent_of(c, c));
        }
    }
    CHECK(empty.is_in_odd_row({0, -3}));
    CHECK(empty.is_in_even_row({0, -2}));
    CHECK(empty.is_in_odd_column({-3, 0}));
    CHECK(empty.is_in_even_column({-2, 0}));
    CHECK_FALSE(empty.to_offset_coordinate({std::numeric_limits<int64_t>::max(), 0, 0}));
    CHECK_FALSE(empty.to_offset_coordinate({0, 0, std::numeric_limits<int64_t>::min()}));
    /** @brief Geometry covering every nonnegative signed x- and y-coordinate. */
    const hexagonal_layout wide{a, {2147483648ull, 2147483648ull, 2}};
    CHECK(wide.eastern_border_of({0, 0}) == layout_base::coordinate{2147483647, 0});
    CHECK(wide.southern_border_of({0, 0}) == layout_base::coordinate{0, 2147483647});
    CHECK_FALSE(wide.east({2147483647, 0}));
    CHECK_FALSE(wide.south({0, 2147483647}));
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

TEST_CASE("Hexagonal cube conversion rejects unrepresentable axes", "[hexagonal-layout]")
{
    const auto max_axis = std::numeric_limits<int64_t>::max();
    const auto min_axis = std::numeric_limits<int64_t>::min();
    for (const auto a :
         {arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN})
    {
        const hexagonal_layout hex{a, {4, 4}};
        CHECK_FALSE(hex.to_offset_coordinate({max_axis, min_axis, 1}));
        CHECK_FALSE(hex.to_offset_coordinate({1, min_axis, max_axis}));
    }
    const hexagonal_layout rows{arrangement::ODD_ROW, {4, 4}};
    CHECK(rows.to_offset_coordinate({2147483648ll, -2147483646ll, -2}) == layout_base::coordinate{2147483647, -2});
}
