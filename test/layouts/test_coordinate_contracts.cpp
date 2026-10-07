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

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/arrangement.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/layouts/layout_base.hpp>
#include <fiction/layouts/layout_utils.hpp>
#include <fiction/layouts/shifted_cartesian_layout.hpp>
#include <fiction/layouts/tile_clocking.hpp>

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

using namespace fiction;
using namespace fiction::layouts;

namespace
{

/**
 * Checks that the neighbors of every coordinate lie inside the layout, differ from the coordinate, and are symmetric,
 * and that a neighbor function returns no coordinate where no neighbor exists.
 * @tparam Lyt Geometry type.
 * @param lyt Layout to check.
 */
template <typename Lyt>
void check_border_contract(const Lyt& lyt)
{
    lyt.foreach_coordinate(
        [&lyt](const auto& c)
        {
            CHECK(lyt.contains_coordinate(c));

            CHECK(lyt.north(c).has_value() == (c.y != 0));
            CHECK(lyt.west(c).has_value() == (c.x != 0));
            CHECK(lyt.below(c).has_value() == (c.z != 0));

            for (const auto& n : lyt.adjacent_coordinates(c))
            {
                CHECK(n != c);
                CHECK(lyt.contains_coordinate(n));

                const auto back = lyt.adjacent_coordinates(n);
                CHECK(std::find(back.cbegin(), back.cend(), c) != back.cend());
            }
        });
}

/**
 * Checks that every directional move is absent or reaches an adjacent coordinate.
 * @tparam Lyt Geometry type.
 * @param lyt Layout to check.
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
                if (moved)
                {
                    CHECK(*moved != c);
                    CHECK(lyt.contains_coordinate(*moved));
                    CHECK(lyt.is_adjacent_of(c, *moved));
                }
            }
        });
}

}  // namespace

TEST_CASE("Layouts check sizes against the signed coordinate domain", "[coordinate-contracts]")
{
    /** Largest axis size supported by the coordinate domain. */
    constexpr uint64_t max_size = uint64_t{1} << 31u;
    CHECK_THROWS_AS((cartesian_layout{layout_base::extent{-1, 0, 0}}), std::invalid_argument);
    CHECK_THROWS_AS((cartesian_layout{layout_base::extent{0, max_size + 1, 0}}), std::invalid_argument);
    CHECK_THROWS_AS((hexagonal_layout{arrangement::ODD_ROW, {0, -1, 0}}), std::invalid_argument);
    CHECK_THROWS_AS((hexagonal_layout{arrangement::ODD_ROW, {max_size + 1, 0, 0}}), std::invalid_argument);
    CHECK_THROWS_AS((shifted_cartesian_layout{arrangement::EVEN_COLUMN, {0, 0, -1}}), std::invalid_argument);

    cartesian_layout cart{{2, 2}};
    CHECK_THROWS_AS(cart.resize({-1, 0, 0}), std::invalid_argument);
    CHECK_THROWS_AS(cart.resize({0, 0, max_size + 1}), std::invalid_argument);

    hexagonal_layout hex{arrangement::ODD_ROW, {2, 2}};
    CHECK_THROWS_AS(hex.resize({0, -1, 0}), std::invalid_argument);
    CHECK_NOTHROW((cartesian_layout{layout_base::extent{max_size, max_size, max_size}}));
    CHECK_NOTHROW((hexagonal_layout{arrangement::ODD_ROW, {max_size, max_size, 1}}));
}

TEST_CASE("The origin belongs to the first clock zone", "[coordinate-contracts]")
{
    const tile_clocking clk{3, 3};
    CHECK(clk.get_clock_zone({}) == layout_base::coordinate{});
    CHECK(clk.get_clock_zone({4, 7, 0}) == layout_base::coordinate{1, 2, 0});
}

TEST_CASE("Negative cells use the clock zone below zero", "[coordinate-contracts]")
{
    tile_clocking clocks{3, 3};
    clocks.assign_clock_number({-1, -1}, 2);

    CHECK(clocks.get_clock_zone({-1, -2}) == layout_base::coordinate{-1, -1});
    CHECK(clocks.get_clock_zone({-3, -3}) == layout_base::coordinate{-1, -1});
    CHECK(clocks.get_clock_zone({-4, 0}) == layout_base::coordinate{-2, 0});
    CHECK(clocks.get_clock_number({-1, -2}) == 2);
}

TEST_CASE("Coordinates iterate row by row, layer by layer", "[coordinate-contracts]")
{
    using coord = fiction::layouts::layout_base::coordinate;

    const cartesian_layout lyt{{3, 2, 2}};

    std::vector<coord> visited{};
    lyt.foreach_coordinate([&visited](const auto& c) { visited.push_back(c); });

    const std::vector<coord> expected{{0, 0, 0}, {1, 0, 0}, {2, 0, 0}, {0, 1, 0}, {1, 1, 0}, {2, 1, 0},
                                      {0, 0, 1}, {1, 0, 1}, {2, 0, 1}, {0, 1, 1}, {1, 1, 1}, {2, 1, 1}};

    CHECK(visited == expected);
}

TEST_CASE("Layouts support only the ground and crossing layers", "[coordinate-contracts]")
{
    /** Checks layer limits for construction, resize, and copies. */
    const auto check_layers = [](const auto& make_layout)
    {
        for (const auto layers : {0u, 1u, 2u})
        {
            /** Layout with a supported layer count. */
            auto lyt = make_layout(layout_base::extent{2, 3, layers});
            CHECK(lyt.layers() == layers);
            CHECK(lyt.clone().get_extent() == lyt.get_extent());
            CHECK_FALSE(lyt.is_crossing_layer({0, 0, -1}));
            CHECK_FALSE(lyt.is_crossing_layer({0, 0, 0}));
            CHECK(lyt.is_crossing_layer({0, 0, 1}));
            CHECK_FALSE(lyt.is_crossing_layer({0, 0, 2}));
            for (const auto too_many : {3u, uint32_t{1} << 31u})
            {
                CHECK_THROWS_AS(make_layout(layout_base::extent{2, 3, too_many}), std::out_of_range);
                CHECK_THROWS_AS(lyt.resize({4, 5, too_many}), std::out_of_range);
                CHECK(lyt.get_extent() == layout_base::extent{2, 3, layers});
            }
            /** Independent layout copy. */
            auto duplicate = lyt;
            CHECK_THROWS_AS(duplicate.resize({4, 5, 3}), std::out_of_range);
            CHECK(duplicate.get_extent() == lyt.get_extent());
            CHECK_THROWS_AS(lyt.clone().resize({4, 5, 3}), std::out_of_range);
            lyt.resize({4, 5, 2});
            CHECK(lyt.get_extent() == layout_base::extent{4, 5, 2});
            lyt.resize({0, 0, 0});
            CHECK(lyt.coordinates().empty());
        }
    };
    check_layers([](const auto& size) { return cartesian_layout{size}; });
    check_layers([](const auto& size) { return gate_level_layout<cartesian_layout>{size}; });
    for (const auto a : {arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN})
    {
        check_layers([a](const auto& size) { return hexagonal_layout{a, size}; });
        check_layers([a](const auto& size) { return shifted_cartesian_layout{a, size}; });
        check_layers([a](const auto& size) { return gate_level_layout<hexagonal_layout>{a, size}; });
        check_layers([a](const auto& size) { return gate_level_layout<shifted_cartesian_layout>{a, size}; });
    }
}

TEST_CASE("Direction predicates compare coordinates outside the frame", "[coordinate-contracts]")
{
    const cartesian_layout cart{{4, 4}};
    const hexagonal_layout hex{arrangement::ODD_ROW, {4, 4}};

    CHECK(cart.is_east_of({-1, 0, 0}, {}));
    CHECK(cart.is_south_of({0, -1, 0}, {}));
    CHECK(cart.is_above({0, 0, -1}, {}));
    CHECK(hex.is_east_of({-1, 0, 0}, {}));
    CHECK(hex.is_south_of({0, -1, 0}, {}));
    CHECK(hex.is_above({0, 0, -1}, {}));
    CHECK_FALSE(cart.is_adjacent_of({7, 0, 0}, {}));
    CHECK_FALSE(hex.is_adjacent_of({7, 0, 0}, {}));
}

TEMPLATE_TEST_CASE("Geometry neighbors respect borders", "[coordinate-contracts]", cartesian_layout,
                   shifted_cartesian_layout, hexagonal_layout)
{
    const auto a =
        GENERATE(arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN);

    const auto layout = blueprints::make_layout<TestType>(a, {5, 4, 2});
    check_border_contract(layout);
    if constexpr (!std::same_as<TestType, cartesian_layout>)
    {
        check_moves_stay_adjacent(layout);
    }
}

TEST_CASE("Hexagonal diagonals at the borders reject coordinates outside the layout", "[coordinate-contracts]")
{
    const auto a =
        GENERATE(arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN);

    const hexagonal_layout lyt{a, {4, 4, 1}};

    // the origin has no north or west neighbor in any direction
    const fiction::layouts::layout_base::coordinate origin{0, 0, 0};

    CHECK_FALSE(lyt.north(origin));
    CHECK_FALSE(lyt.west(origin));
    CHECK_FALSE(lyt.north_west(origin));

    // the far corner has no south or east neighbor in any direction
    const fiction::layouts::layout_base::coordinate corner{3, 3, 0};

    CHECK_FALSE(lyt.south(corner));
    CHECK_FALSE(lyt.east(corner));
    CHECK_FALSE(lyt.south_east(corner));
}

TEST_CASE("Hexagonal neighbor queries stay inside the layout for any 32-bit coordinate", "[coordinate-contracts]")
{
    for (const auto a :
         {arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN})
    {
        const hexagonal_layout hex{a, {7, 7, 2}};

        for (const auto& c : {fiction::layouts::layout_base::coordinate{},
                              {2, std::numeric_limits<int32_t>::min(), 0},
                              {2147483647, 0, 0},
                              {0, 2147483647, 0},
                              {2147483647, 2147483647, 0},
                              {-2147483647, 0, 0},
                              {0, -2147483647, 0},
                              {-2147483647, -2147483647, 0},
                              {-1, 1, 0},
                              {7, 1, 0}})
        {
            for (const auto& n : hex.adjacent_coordinates(c))
            {
                CHECK(hex.contains_coordinate(n));
            }

            for (const auto& n : {hex.north_east(c), hex.south_east(c), hex.south_west(c), hex.north_west(c)})
            {
                if (n)
                {
                    CHECK(*n != c);
                    CHECK(hex.contains_coordinate(*n));
                }
            }
        }

        CHECK_FALSE(hex.adjacent_coordinates({}).empty());
        CHECK(hex.adjacent_coordinates({std::numeric_limits<int32_t>::min(), 0}).empty());
    }
}

TEMPLATE_TEST_CASE("Gate placement accepts coordinates beyond its frame", "[coordinate-contracts]", cartesian_layout,
                   shifted_cartesian_layout, hexagonal_layout)
{
    /** @brief Gate layout with stable object identities. */
    using lyt_t = gate_level_layout<TestType>;
    /** Coordinate value used for placement. */
    using tile = fiction::layouts::layout_base::coordinate;

    SECTION("The geometry rejects more than two layers")
    {
        CHECK_THROWS_AS((blueprints::make_layout<lyt_t>(arrangement::EVEN_ROW, {6, 6, 3})), std::out_of_range);
    }
    SECTION("Every layer has distinct placement")
    {
        auto lyt = blueprints::make_layout<lyt_t>(arrangement::EVEN_ROW, {4, 4, 2}, clocking::twoddwave());
        /** Ground-layer primary input. */
        const auto ground = lyt.create_pi("a", {2, 0, 0});
        /** Primary input beyond the declared layer count. */
        const auto elevated = lyt.create_pi("b", {2, 0, 2});
        CHECK(lyt.find_object({2, 0, 0}) == ground);
        CHECK(lyt.find_object({2, 0, 2}) == elevated);
        lyt.clear_tile({2, 0, 2});
        CHECK_FALSE(lyt.find_object({2, 0, 2}));
        CHECK(lyt.find_object({2, 0, 0}) == ground);
    }
    SECTION("Negative coordinates identify placed objects")
    {
        auto       lyt = blueprints::make_layout<lyt_t>(arrangement::EVEN_ROW, {4, 4, 2}, clocking::twoddwave());
        const auto a   = lyt.create_pi("a", {std::numeric_limits<int32_t>::min(), 0, 0});
        const auto b   = lyt.create_pi("b", {-1, -1, -1});
        CHECK(lyt.num_pis() == 2);
        CHECK(lyt.get_tile(a) == tile{std::numeric_limits<int32_t>::min(), 0, 0});
        CHECK(lyt.get_tile(b) == tile{-1, -1, -1});
    }
}

TEMPLATE_TEST_CASE("Gate geometry copies have independent sizes", "[coordinate-contracts]", cartesian_layout,
                   shifted_cartesian_layout, hexagonal_layout)
{
    auto coordinates = blueprints::make_layout<TestType>(arrangement::EVEN_COLUMN, {4, 4, 2});
    const gate_level_layout<TestType> gates{coordinates};
    const auto                        original = coordinates.get_extent();
    coordinates.resize({4, 4, 1});
    CHECK(gates.get_extent() == original);
    auto copy = static_cast<const TestType&>(gates);
    copy.resize({5, 5, 1});
    CHECK(gates.get_extent() == original);
    auto clone = gates.clone();
    clone.resize({6, 6, 2});
    CHECK(gates.get_extent() == original);
    CHECK(clone.get_extent() == layout_base::extent{6, 6, 2});
}

TEST_CASE("Cell scaling checks the final signed coordinate", "[coordinate-contracts]")
{
    using coord = layout_base::coordinate;
    const gate_level_layout<cartesian_layout> gates{{1073741824, 1}};

    CHECK((relative_to_absolute_cell_position<5, 5>(gates, coord{400000000, 0}, coord{4, 0}) == coord{2000000004, 0}));
    CHECK_THROWS_AS((relative_to_absolute_cell_position<5, 5>(gates, coord{1073741823, 0}, coord{4, 0})),
                    std::overflow_error);
    CHECK_THROWS_AS((relative_to_absolute_cell_position<5, 5>(gates, coord{0, 0}, coord{-1, 0})),
                    std::invalid_argument);
}
