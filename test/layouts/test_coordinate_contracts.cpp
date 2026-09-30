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
 * @brief Tests for the coordinate behavior that every layout guarantees.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <fiction/layouts/arrangement.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/layouts/shifted_cartesian_layout.hpp>
#include <fiction/traits.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <set>
#include <vector>

using namespace fiction;
using namespace fiction::layouts;

namespace
{

/**
 * Checks that the neighbors of every coordinate lie inside the layout, differ from the coordinate, and are symmetric,
 * and that a neighbor function returns the coordinate itself exactly where no neighbor exists.
 */
template <typename Lyt>
void check_border_contract(const Lyt& lyt)
{
    lyt.foreach_coordinate(
        [&lyt](const auto& c)
        {
            CHECK(lyt.is_within_bounds(c));

            CHECK((lyt.north(c) == c) == (c.y == 0));
            CHECK((lyt.west(c) == c) == (c.x == 0));
            CHECK((lyt.below(c) == c) == (c.z == 0));

            for (const auto& n : lyt.adjacent_coordinates(c))
            {
                CHECK(n != c);
                CHECK(lyt.is_within_bounds(n));

                const auto back = lyt.adjacent_coordinates(n);
                CHECK(std::find(back.cbegin(), back.cend(), c) != back.cend());
            }
        });
}

/**
 * Checks that every directional move either stays put or reaches an adjacent coordinate.
 */
template <typename Lyt>
void check_moves_stay_adjacent(const Lyt& lyt)
{
    lyt.foreach_coordinate(
        [&lyt](const auto& c)
        {
            for (const auto& moved : {lyt.north(c), lyt.north_east(c), lyt.east(c), lyt.south_east(c), lyt.south(c),
                                      lyt.south_west(c), lyt.west(c), lyt.north_west(c)})
            {
                if (moved != c && lyt.is_within_bounds(moved))
                {
                    CHECK(lyt.is_adjacent_of(c, moved));
                }
            }
        });
}

}  // namespace

TEST_CASE("Border contract of Cartesian layouts", "[coordinate-contracts]")
{
    const cartesian_layout lyt{{4, 3, 1}};

    check_border_contract(lyt);
}

TEST_CASE("Border contract of shifted Cartesian and hexagonal layouts", "[coordinate-contracts]")
{
    const auto a =
        GENERATE(arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN);

    const hexagonal_layout         hex{a, {4, 3, 1}};
    const shifted_cartesian_layout shifted{a, {4, 3, 1}};

    check_border_contract(hex);
    check_border_contract(shifted);
    check_moves_stay_adjacent(hex);
    check_moves_stay_adjacent(shifted);
}

TEST_CASE("Hexagonal diagonals at the borders reject coordinates outside the layout", "[coordinate-contracts]")
{
    const auto a =
        GENERATE(arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN);

    const hexagonal_layout lyt{a, {3, 3, 0}};

    // the origin has no north or west neighbor in any direction
    const coordinate<hexagonal_layout> origin{0, 0, 0};

    CHECK(lyt.north(origin) == origin);
    CHECK(lyt.west(origin) == origin);
    CHECK(lyt.north_west(origin) == origin);

    // the far corner has no south or east neighbor in any direction
    const coordinate<hexagonal_layout> corner{3, 3, 0};

    CHECK(lyt.south(corner) == corner);
    CHECK(lyt.east(corner) == corner);
    CHECK(lyt.south_east(corner) == corner);
}

TEST_CASE("Coordinates iterate row by row, layer by layer", "[coordinate-contracts]")
{
    using coord = coordinate<cartesian_layout>;

    const cartesian_layout lyt{{2, 1, 1}};

    std::vector<coord> visited{};
    lyt.foreach_coordinate([&visited](const auto& c) { visited.push_back(c); });

    const std::vector<coord> expected{{0, 0, 0}, {1, 0, 0}, {2, 0, 0}, {0, 1, 0}, {1, 1, 0}, {2, 1, 0},
                                      {0, 0, 1}, {1, 0, 1}, {2, 0, 1}, {0, 1, 1}, {1, 1, 1}, {2, 1, 1}};

    CHECK(visited == expected);
}

TEST_CASE("Coordinates order by layer, then row, then column", "[coordinate-contracts]")
{
    using coord = coordinate<cartesian_layout>;

    CHECK(coord{5, 0, 0} < coord{0, 1, 0});
    CHECK(coord{5, 9, 0} < coord{0, 0, 1});
    CHECK(coord{1, 2, 0} < coord{2, 2, 0});
    CHECK(!(coord{2, 2, 0} < coord{2, 2, 0}));

    std::set<coord> ordered{{1, 1, 0}, {0, 0, 1}, {2, 0, 0}, {0, 1, 0}};

    CHECK(*ordered.begin() == coord{2, 0, 0});
    CHECK(*ordered.rbegin() == coord{0, 0, 1});
}

TEST_CASE("Coordinates hash as their packed gate-level encoding", "[coordinate-contracts]")
{
    using coord = coordinate<cartesian_layout>;

    const auto hash_of = [](const coord& c) { return std::hash<coord>{}(c); };
    const auto packed  = [](const uint64_t x, const uint64_t y, const uint64_t z)
    { return std::hash<uint64_t>{}((z << 62) | (y << 31) | x); };

    CHECK(hash_of({0, 0, 0}) == packed(0, 0, 0));
    CHECK(hash_of({5, 7, 0}) == packed(5, 7, 0));
    CHECK(hash_of({5, 7, 1}) == packed(5, 7, 1));
    CHECK(hash_of({1'000'000, 2'000'000, 1}) == packed(1'000'000, 2'000'000, 1));
}

TEST_CASE("Gate-level layouts map tiles to nodes and back", "[coordinate-contracts]")
{
    gate_level_layout<cartesian_layout> lyt{{3, 3, 1}, clocking::twoddwave()};

    const auto a = lyt.create_pi("a", {0, 1, 0});
    const auto b = lyt.create_buf(a, {1, 1, 1});

    CHECK(lyt.get_tile(lyt.get_node(a)) == coordinate<cartesian_layout>{0, 1, 0});
    CHECK(lyt.get_tile(lyt.get_node(b)) == coordinate<cartesian_layout>{1, 1, 1});
    CHECK(lyt.get_node(coordinate<cartesian_layout>{1, 1, 1}) == lyt.get_node(b));
    CHECK(lyt.is_empty_tile({2, 2, 0}));
    CHECK(!lyt.is_empty_tile({0, 1, 0}));

    const auto moved = lyt.move_node(lyt.get_node(b), {2, 2, 0});

    CHECK(lyt.get_tile(lyt.get_node(moved)) == coordinate<cartesian_layout>{2, 2, 0});
    CHECK(lyt.is_empty_tile({1, 1, 1}));
}
