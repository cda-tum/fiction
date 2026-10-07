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
#include <fiction/layouts/layout_utils.hpp>
#include <fiction/layouts/shifted_cartesian_layout.hpp>
#include <fiction/layouts/tile_clocking.hpp>
#include <fiction/traits.hpp>

#include <algorithm>
#include <cstdint>
#include <functional>
#include <limits>
#include <set>
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

TEST_CASE("Border contract of Cartesian layouts", "[coordinate-contracts]")
{
    const cartesian_layout lyt{{5, 4, 2}};

    check_border_contract(lyt);
}

TEST_CASE("Border contract of shifted Cartesian and hexagonal layouts", "[coordinate-contracts]")
{
    const auto a =
        GENERATE(arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN);

    const hexagonal_layout         hex{a, {5, 4, 2}};
    const shifted_cartesian_layout shifted{a, {5, 4, 2}};

    check_border_contract(hex);
    check_border_contract(shifted);
    check_moves_stay_adjacent(hex);
    check_moves_stay_adjacent(shifted);
}

TEST_CASE("Hexagonal diagonals at the borders reject coordinates outside the layout", "[coordinate-contracts]")
{
    const auto a =
        GENERATE(arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN);

    const hexagonal_layout lyt{a, {4, 4, 1}};

    // the origin has no north or west neighbor in any direction
    const coordinate<hexagonal_layout> origin{0, 0, 0};

    CHECK_FALSE(lyt.north(origin));
    CHECK_FALSE(lyt.west(origin));
    CHECK_FALSE(lyt.north_west(origin));

    // the far corner has no south or east neighbor in any direction
    const coordinate<hexagonal_layout> corner{3, 3, 0};

    CHECK_FALSE(lyt.south(corner));
    CHECK_FALSE(lyt.east(corner));
    CHECK_FALSE(lyt.south_east(corner));
}

TEST_CASE("Coordinates iterate row by row, layer by layer", "[coordinate-contracts]")
{
    using coord = coordinate<cartesian_layout>;

    const cartesian_layout lyt{{3, 2, 2}};

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
    CHECK_NOTHROW((cartesian_layout{layout_base::extent{max_size, max_size, 2}}));
    CHECK_NOTHROW((hexagonal_layout{arrangement::ODD_ROW, {max_size, max_size, 1}}));
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
    for (const auto a :
         {arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN})
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

TEST_CASE("Hexagonal offset and cube coordinates convert back and forth for negative and extreme values",
          "[coordinate-contracts]")
{
    for (const auto a :
         {arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN})
    {
        const hexagonal_layout hex{a, {0, 0, 0}};

        for (int32_t x = -7; x <= 7; ++x)
        {
            for (int32_t y = -7; y <= 7; ++y)
            {
                const auto cube = hex.to_cube_coordinate({x, y});

                CHECK(cube.x + cube.y + cube.z == 0);
                CHECK(hex.to_offset_coordinate(cube) == layout_base::coordinate{x, y});
            }
        }

        for (const auto x : {-2147483647, 2147483647})
        {
            for (const auto y : {-2147483647, 2147483647})
            {
                CHECK(hex.to_offset_coordinate(hex.to_cube_coordinate({x, y})) == layout_base::coordinate{x, y});
            }
        }
    }
}

TEST_CASE("Hexagonal neighbor queries stay inside the layout for any 32-bit coordinate", "[coordinate-contracts]")
{
    for (const auto a :
         {arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN})
    {
        const hexagonal_layout hex{a, {7, 7, 2}};

        for (const auto& c : {coordinate<hexagonal_layout>{},
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

TEST_CASE("Gate-level layouts map tiles to object identities and back", "[coordinate-contracts]")
{
    gate_level_layout<cartesian_layout> lyt{{4, 4, 2}, clocking::twoddwave()};

    const auto a = lyt.create_pi("a", {0, 1, 0});
    const auto b = lyt.create_buf(a, {1, 1, 1});

    CHECK(lyt.get_tile(a.object) == coordinate<cartesian_layout>{0, 1, 0});
    CHECK(lyt.get_tile(b.object) == coordinate<cartesian_layout>{1, 1, 1});
    CHECK(lyt.find_object({1, 1, 1}) == b.object);
    CHECK(lyt.is_empty_tile({2, 2, 0}));
    CHECK_FALSE(lyt.is_empty_tile({0, 1, 0}));

    const auto moved = lyt.move_node(b.object, {2, 2, 0});

    CHECK(moved == b);
    CHECK(lyt.get_tile(b.object) == coordinate<cartesian_layout>{2, 2, 0});
    CHECK(lyt.source({b.object, 0}) == a);
    CHECK(lyt.is_empty_tile({1, 1, 1}));
}

TEST_CASE("Gate placement accepts coordinates beyond its frame", "[coordinate-contracts]")
{
    /** Cartesian layout with stable object identities. */
    using lyt_t = gate_level_layout<cartesian_layout>;
    /** Coordinate value used for placement. */
    using tile = coordinate<lyt_t>;

    SECTION("The geometry rejects more than two layers")
    {
        CHECK_THROWS_AS((lyt_t{{6, 6, 3}}), std::out_of_range);
    }
    SECTION("Occupied placement leaves objects and connections unchanged")
    {
        lyt_t      lyt{{4, 4, 2}, clocking::twoddwave()};
        const auto a   = lyt.create_pi("a", {0, 0, 0});
        const auto buf = lyt.create_buf(a, {1, 0, 0});
        const auto po  = lyt.create_po(buf, "po", {2, 0, 0});

        CHECK_THROWS_AS(lyt.create_pi("b", {0, 0, 0}), std::invalid_argument);
        CHECK_THROWS_AS(lyt.move_node(buf.object, {2, 0, 0}), std::invalid_argument);
        CHECK(lyt.get_tile(buf.object) == tile{1, 0, 0});
        CHECK(lyt.source({po.object, 0}) == buf);
        CHECK(lyt.size() == 3);
        CHECK(lyt.num_pis() == 1);
        CHECK(lyt.num_pos() == 1);

        CHECK(lyt.move_node(buf.object, {5, 0, 2}) == buf);
        CHECK_FALSE(lyt.contains_coordinate(lyt.get_tile(buf.object)));
        CHECK(lyt.source({po.object, 0}) == buf);
    }
    SECTION("Every layer has distinct placement")
    {
        lyt_t lyt{{4, 4, 2}, clocking::twoddwave()};
        /** Ground-layer primary input. */
        const auto ground = lyt.create_pi("a", {2, 0, 0});
        /** Primary input beyond the declared layer count. */
        const auto elevated = lyt.create_pi("b", {2, 0, 2});
        CHECK(lyt.find_object({2, 0, 0}) == ground.object);
        CHECK(lyt.find_object({2, 0, 2}) == elevated.object);
        lyt.clear_tile({2, 0, 2});
        CHECK_FALSE(lyt.find_object({2, 0, 2}));
        CHECK(lyt.find_object({2, 0, 0}) == ground.object);
    }
    SECTION("Negative coordinates identify placed objects")
    {
        lyt_t      lyt{{4, 4, 2}, clocking::twoddwave()};
        const auto a = lyt.create_pi("a", {std::numeric_limits<int32_t>::min(), 0, 0});
        const auto b = lyt.create_pi("b", {-1, -1, -1});
        CHECK(lyt.num_pis() == 2);
        CHECK(lyt.get_tile(a.object) == tile{std::numeric_limits<int32_t>::min(), 0, 0});
        CHECK(lyt.get_tile(b.object) == tile{-1, -1, -1});
    }
}

TEST_CASE("Bounding boxes include occupied coordinates outside the frame", "[coordinate-contracts]")
{
    /** Cartesian layout with stable object identities. */
    using lyt_t = gate_level_layout<cartesian_layout>;
    lyt_t           lyt{{4, 4, 2}, clocking::twoddwave()};
    bounding_box_2d bb{lyt};
    CHECK_FALSE(bb.get_min());
    CHECK_FALSE(bb.get_max());
    CHECK(bb.get_x_size() == 0);
    CHECK(bb.get_y_size() == 0);

    lyt.create_pi("a", {10, 10, 0});
    bb.update_bounding_box();
    CHECK(bb.get_x_size() == 1);
    CHECK(bb.get_y_size() == 1);
    CHECK(bb.get_min() == coordinate<lyt_t>{10, 10, 0});
    CHECK(bb.get_max() == coordinate<lyt_t>{10, 10, 0});
}

TEST_CASE("The origin belongs to the first clock zone", "[coordinate-contracts]")
{
    const tile_clocking clk{3, 3};
    CHECK(clk.get_clock_zone({}) == layout_base::coordinate{});
    CHECK(clk.get_clock_zone({4, 7, 0}) == layout_base::coordinate{1, 2, 0});
}

TEST_CASE("Coordinate construction rejects narrowing", "[coordinate-regressions]")
{
    using coord = layout_base::coordinate;

    CHECK_THROWS_AS((coord{4294967296ull, 0}), std::overflow_error);
    CHECK_THROWS_AS((coord{0, 0, -4294967296ll}), std::overflow_error);
    CHECK_THROWS_AS((cartesian_layout{{4294967296ull, 0}}), std::invalid_argument);
    CHECK((coord{std::numeric_limits<int32_t>::max(), 0}.x == std::numeric_limits<int32_t>::max()));
}

TEST_CASE("Gate geometry copies have independent sizes", "[coordinate-regressions]")
{
    /** Checks independent sizes for geometry copies and complete layout clones. */
    const auto check_sizes = [](auto coordinates)
    {
        /** Geometry copied into the gate layout. */
        using geometry = decltype(coordinates);
        /** @brief Gate layout whose size remains unchanged. */
        const gate_level_layout<geometry> gates{coordinates};
        /** Sizes retained by the source gate layout. */
        const auto original = coordinates.get_extent();

        coordinates.resize({4, 4, 1});
        CHECK(gates.get_extent() == original);
        /** Independent geometry value. */
        auto copy = static_cast<const geometry&>(gates);
        copy.resize({5, 5, 1});
        CHECK(gates.get_extent() == original);
        /** Independent complete layout clone. */
        auto clone = gates.clone();
        clone.resize({6, 6, 2});
        CHECK(gates.get_extent() == original);
        CHECK(clone.get_extent() == layout_base::extent{6, 6, 2});
    };

    check_sizes(cartesian_layout{{4, 4, 2}});
    check_sizes(hexagonal_layout{arrangement::ODD_ROW, {4, 4, 2}});
    check_sizes(shifted_cartesian_layout{arrangement::EVEN_COLUMN, {4, 4, 2}});
}

TEST_CASE("Cell scaling checks the final signed coordinate", "[coordinate-regressions]")
{
    using coord = layout_base::coordinate;
    const gate_level_layout<cartesian_layout> gates{{1073741824, 1}};

    CHECK((relative_to_absolute_cell_position<5, 5>(gates, coord{400000000, 0}, coord{4, 0}) == coord{2000000004, 0}));
    CHECK_THROWS_AS((relative_to_absolute_cell_position<5, 5>(gates, coord{1073741823, 0}, coord{4, 0})),
                    std::overflow_error);
    CHECK_THROWS_AS((relative_to_absolute_cell_position<5, 5>(gates, coord{0, 0}, coord{-1, 0})),
                    std::invalid_argument);
}

TEST_CASE("Negative cells use the clock zone below zero", "[coordinate-regressions]")
{
    tile_clocking clocks{3, 3};
    clocks.assign_clock_number({-1, -1}, 2);

    CHECK(clocks.get_clock_zone({-1, -2}) == layout_base::coordinate{-1, -1});
    CHECK(clocks.get_clock_zone({-3, -3}) == layout_base::coordinate{-1, -1});
    CHECK(clocks.get_clock_zone({-4, 0}) == layout_base::coordinate{-2, 0});
    CHECK(clocks.get_clock_number({-1, -2}) == 2);
}

TEST_CASE("Hexagonal cube conversion rejects unrepresentable axes", "[coordinate-regressions]")
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
