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

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <fiction/layouts/arrangement.hpp>
#include <fiction/layouts/bounding_box.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/layouts/layout_base.hpp>
#include <fiction/layouts/shifted_cartesian_layout.hpp>
#include <fiction/layouts/tile_clocking.hpp>
#include <fiction/traits.hpp>

#include <algorithm>
#include <functional>
#include <set>
#include <stdexcept>
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

    const std::set<coord> ordered{{1, 1, 0}, {0, 0, 1}, {2, 0, 0}, {0, 1, 0}};

    CHECK(*ordered.begin() == coord{2, 0, 0});
    CHECK(*ordered.rbegin() == coord{0, 0, 1});
}

TEST_CASE("Equal coordinates hash equally", "[coordinate-contracts]")
{
    using coord = coordinate<cartesian_layout>;

    const auto hash_of = [](const coord& c) { return std::hash<coord>{}(c); };

    CHECK(hash_of({5, 7, 1}) == hash_of({5, 7, 1}));
    CHECK(hash_of({-5, 7, 0}) == hash_of({-5, 7, 0}));
    CHECK(hash_of({}) == hash_of({}));
}

TEST_CASE("Layouts reject extents outside of [0, 2^30 - 1]", "[coordinate-contracts]")
{
    CHECK_THROWS_AS((cartesian_layout{layout_base::aspect_ratio{-1, 0, 0}}), std::invalid_argument);
    CHECK_THROWS_AS((cartesian_layout{layout_base::aspect_ratio{0, 1073741824, 0}}), std::invalid_argument);
    CHECK_THROWS_AS((hexagonal_layout{arrangement::ODD_ROW, {0, -1, 0}}), std::invalid_argument);
    CHECK_THROWS_AS((hexagonal_layout{arrangement::ODD_ROW, {1073741824, 0, 0}}), std::invalid_argument);
    CHECK_THROWS_AS((shifted_cartesian_layout{arrangement::EVEN_COLUMN, {0, 0, -1}}), std::invalid_argument);

    cartesian_layout cart{{1, 1, 0}};
    CHECK_THROWS_AS(cart.resize({-1, 0, 0}), std::invalid_argument);
    CHECK_THROWS_AS(cart.resize({0, 0, 2147483647}), std::invalid_argument);

    hexagonal_layout hex{arrangement::ODD_ROW, {1, 1, 0}};
    CHECK_THROWS_AS(hex.resize({0, -1, 0}), std::invalid_argument);

    // the largest extent is accepted
    CHECK_NOTHROW((cartesian_layout{layout_base::aspect_ratio{1073741823, 1073741823, 1073741823}}));
    CHECK_NOTHROW((hexagonal_layout{arrangement::ODD_ROW, {1073741823, 1073741823, 0}}));
}

TEST_CASE("Predicates on directions reject the invalid coordinate", "[coordinate-contracts]")
{
    const cartesian_layout cart{{3, 3, 0}};
    const hexagonal_layout hex{arrangement::ODD_ROW, {3, 3, 0}};

    CHECK(!cart.is_east_of({7, 0, 0}, {}));
    CHECK(!cart.is_south_of({0, 9, 0}, {}));
    CHECK(!cart.is_above({0, 0, 5}, {}));
    CHECK(!hex.is_east_of({7, 0, 0}, {}));
    CHECK(!hex.is_south_of({0, 9, 0}, {}));
    CHECK(!hex.is_above({0, 0, 5}, {}));
}

TEST_CASE("Hexagonal layouts give coordinates outside of the layout no neighbors", "[coordinate-contracts]")
{
    for (const auto a :
         {arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN})
    {
        const hexagonal_layout hex{a, {6, 6, 1}};

        for (const auto& c : {coordinate<hexagonal_layout>{-1, 1, 0},
                              {0, -1, 0},
                              {1, 1, -1},
                              {7, 1, 0},
                              {1, 7, 0},
                              {1, 1, 2},
                              {},
                              {2, layout_base::coordinate::INVALID_AXIS, 0},
                              {2147483647, 0, 0},
                              {0, 2147483647, 0},
                              {2147483647, 2147483647, 0}})
        {
            CHECK(hex.adjacent_coordinates(c).empty());
            CHECK(!hex.is_adjacent_of(c, {0, 0, 0}));
            CHECK(hex.north_east(c) == c);
            CHECK(hex.south_east(c) == c);
            CHECK(hex.south_west(c) == c);
            CHECK(hex.north_west(c) == c);
        }
    }
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

TEST_CASE("Gate-level layouts check tiles before they change anything", "[coordinate-contracts]")
{
    using lyt_t = gate_level_layout<cartesian_layout>;
    using tile  = coordinate<lyt_t>;

    SECTION("The converting constructor checks the extent")
    {
        CHECK_THROWS_AS((lyt_t{cartesian_layout{{5, 5, 3}}}), std::out_of_range);
        CHECK_NOTHROW((lyt_t{cartesian_layout{{5, 5, 1}}}));
    }
    SECTION("A tile without a signal leaves the layout unchanged")
    {
        lyt_t lyt{{3, 3, 1}, clocking::twoddwave()};

        const auto a = lyt.create_pi("a", {0, 0, 0});

        CHECK_THROWS_AS(lyt.create_pi("b", {1073741824, 0, 0}), std::out_of_range);
        CHECK_THROWS_AS(lyt.create_po(a, "po", {0, 1073741824, 0}), std::out_of_range);
        CHECK_THROWS_AS(lyt.create_and(a, a, {0, 0, 2}), std::out_of_range);
        CHECK_THROWS_AS(lyt.create_buf(a, {0, 0, -1}), std::out_of_range);

        CHECK(lyt.num_pis() == 1);
        CHECK(lyt.num_pos() == 0);
        CHECK(lyt.size() == 3);
        CHECK(lyt.num_gates() == 0);

        const auto buf = lyt.create_buf(a, {1, 0, 0});
        const auto po  = lyt.create_po(buf, "po", {2, 0, 0});

        CHECK_THROWS_AS(lyt.move_node(lyt.get_node(buf), {5, 0, 2}, {a}), std::out_of_range);

        CHECK(!lyt.is_dead(lyt.get_node(buf)));
        CHECK(lyt.get_tile(lyt.get_node(buf)) == tile{1, 0, 0});
        CHECK(lyt.num_wires() == 3);
        CHECK(lyt.fanin_size(lyt.get_node(po)) == 1);

        CHECK_THROWS_AS(lyt.move_node(lyt.get_node(po), {0, 0, 5}, {a}), std::out_of_range);

        lyt.foreach_po([&po](const auto& s) { CHECK(s == po); });
    }
    SECTION("A tile without a signal does not alias another tile")
    {
        lyt_t lyt{{3, 3, 1}, clocking::twoddwave()};

        lyt.create_pi("a", {2, 0, 0});

        CHECK(lyt.is_empty_tile({2, 0, 2}));
        CHECK(lyt.get_node(tile{2, 0, 2}) == 0);

        lyt.clear_tile({2, 0, 2});

        CHECK(!lyt.is_empty_tile({2, 0, 0}));
    }
    SECTION("An axis equal to the invalid value leaves a node unplaced")
    {
        lyt_t lyt{{3, 3, 1}, clocking::twoddwave()};

        lyt.create_pi("a", {layout_base::coordinate::INVALID_AXIS, 0, 0});
        lyt.create_pi("b", {0, layout_base::coordinate::INVALID_AXIS, 0});

        CHECK(lyt.num_pis() == 2);
        lyt.foreach_pi([&lyt](const auto& n) { CHECK(!lyt.get_tile(n).is_valid()); });
    }
}

TEST_CASE("Bounding boxes of layouts without a tile in range", "[coordinate-contracts]")
{
    using lyt_t = gate_level_layout<cartesian_layout>;

    lyt_t lyt{{3, 3, 1}, clocking::twoddwave()};

    lyt.create_pi("a", {10, 10, 0});

    const bounding_box_2d bb{lyt};

    CHECK(bb.get_x_size() == 0);
    CHECK(bb.get_y_size() == 0);
    CHECK(bb.get_min() == coordinate<lyt_t>{0, 0, 0});
    CHECK(bb.get_max() == coordinate<lyt_t>{0, 0, 0});
}

TEST_CASE("The clock zone of an invalid cell is invalid", "[coordinate-contracts]")
{
    const tile_clocking clk{3, 3};

    CHECK(!clk.get_clock_zone({}).is_valid());
    CHECK(clk.get_clock_zone({4, 7, 0}) == layout_base::coordinate{1, 2, 0});
}
