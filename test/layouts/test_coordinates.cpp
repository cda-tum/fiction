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
 * @brief Tests for signed layout coordinates.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 * @author Willem Lambooy (wlambooy)
 */

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/arrangement.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/layouts/layout_base.hpp>
#include <fiction/layouts/shifted_cartesian_layout.hpp>

#include <fmt/format.h>

#include <concepts>
#include <cstdint>
#include <limits>
#include <memory>
#include <optional>
#include <set>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace fiction;
using namespace fiction::layouts;

/**
 * @brief Checks the geometry contract for a layout type.
 * @tparam Lyt Coordinate layout type.
 * @param a Arrangement of the shifted rows or columns.
 */
template <typename Lyt>
void check_visited_coordinates(const arrangement a)
{
    /** @brief Extent whose coordinate iteration is checked. */
    const typename Lyt::extent ex{10, 10, 2};

    auto layout = blueprints::make_layout<Lyt>(a, ex);

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

TEST_CASE("Every signed coordinate is a value", "[coordinates][size-contract]")
{
    using coordinate = layout_base::coordinate;
    CHECK(coordinate{} == coordinate{0, 0, 0});
    CHECK(coordinate{std::numeric_limits<int32_t>::min(), 0, 0} != coordinate{});
    CHECK(coordinate{-3, 0} < coordinate{-2, 0});
    CHECK(coordinate{5, -1} < coordinate{-5, 0});
    CHECK(coordinate{5, 5, -1} < coordinate{-5, -5, 0});
    CHECK(coordinate{1, 2} == coordinate{1, 2, 0});
    CHECK(coordinate{1, 2} != coordinate{2, 1});
    CHECK(coordinate{1, 2} <= coordinate{1, 2});
    CHECK(coordinate{1, 2} >= coordinate{1, 2});
    CHECK(coordinate{1, 2} > coordinate{0, 2});
    CHECK_THROWS_AS((coordinate{uint64_t{2147483648}, 0}), std::overflow_error);
    CHECK_THROWS_AS((coordinate{0, int64_t{-2147483649}}), std::overflow_error);
    CHECK_THROWS_AS((coordinate{0, 0, std::numeric_limits<uint64_t>::max()}), std::overflow_error);
    CHECK(coordinate{std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max()} ==
          coordinate{-2147483648ll, 2147483647ll});

    using coord = layout_base::coordinate;

    CHECK(coord{5, 0, 0} < coord{0, 1, 0});
    CHECK(coord{5, 9, 0} < coord{0, 0, 1});
    CHECK(coord{1, 2, 0} < coord{2, 2, 0});
    CHECK(!(coord{2, 2, 0} < coord{2, 2, 0}));

    const std::set<coord> ordered{{1, 1, 0}, {0, 0, 1}, {2, 0, 0}, {0, 1, 0}};

    CHECK(*ordered.begin() == coord{2, 0, 0});
    CHECK(*ordered.rbegin() == coord{0, 0, 1});

    CHECK_THROWS_AS((coordinate{4294967296ull, 0}), std::overflow_error);
    CHECK_THROWS_AS((coordinate{0, 0, -4294967296ll}), std::overflow_error);
    CHECK_THROWS_AS((cartesian_layout{{4294967296ull, 0}}), std::invalid_argument);
    CHECK((coordinate{std::numeric_limits<int32_t>::max(), 0}.x == std::numeric_limits<int32_t>::max()));

    std::ostringstream stream{};
    stream << coordinate{-3, 2, 7};
    CHECK(stream.str() == "(-3,2,7)");
    CHECK(fmt::format("{}", coordinate{-3, 2, 7}) == "(-3,2,7)");
    /** @brief Distinct signed positions used to check coordinate hashing. */
    const std::unordered_set<coordinate> positions{{0, 0, 0}, {0, 0, 2}, {-2147483648ll, 0, 0}, {2147483647, 0, 0}};
    CHECK(positions.size() == 4);
}

TEST_CASE("Extent sizes validate the coordinate domain", "[coordinates][size-contract]")
{
    using extent = layout_base::extent;
    CHECK(extent{} == extent{0, 0, 0});
    CHECK(extent{2, 3}.layers == 1);
    CHECK_THROWS_AS((extent{-1, 2}), std::invalid_argument);
    CHECK_THROWS_AS((extent{1, -1}), std::invalid_argument);
    CHECK_THROWS_AS((extent{1, 1, -1}), std::invalid_argument);
    CHECK_THROWS_AS((extent{uint64_t{2147483649}, 1}), std::invalid_argument);
    CHECK_THROWS_AS((extent{1, std::numeric_limits<uint64_t>::max()}), std::invalid_argument);
    CHECK(area_of(extent{2, 3, 4}) == 6);
    CHECK(area_of(extent{}) == 0);
    CHECK(area_of(extent{2147483648ull, 2147483648ull}) == 4611686018427387904ull);
    auto edited  = extent{};
    edited.width = std::numeric_limits<uint32_t>::max();
    CHECK_THROWS_AS(cartesian_layout{edited}, std::invalid_argument);
}

TEMPLATE_TEST_CASE("Geometry copies retain arrangement", "[coordinates]", shifted_cartesian_layout, hexagonal_layout)
{
    const auto a =
        GENERATE(arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN);

    const TestType lyt{a, {3, 3}};

    CHECK(lyt.get_arrangement() == a);
    CHECK(lyt.clone().get_arrangement() == a);
    CHECK(TestType{lyt}.get_arrangement() == a);
}
TEMPLATE_TEST_CASE("Geometry sizes and value copies", "[coordinates][size-contract]", cartesian_layout,
                   shifted_cartesian_layout, hexagonal_layout)
{
    const auto empty = blueprints::make_layout<TestType>(arrangement::EVEN_ROW, {});
    CHECK(empty.area() == 0);
    CHECK(empty.coordinates().empty());
    CHECK(empty.ground_coordinates().empty());
    CHECK(!empty.contains_coordinate({0, 0, 0}));
    CHECK(!empty.last_coordinate());
    CHECK(!empty.northern_border_of({0, 0}));
    CHECK(!empty.eastern_border_of({0, 0}));
    CHECK(!empty.southern_border_of({0, 0}));
    CHECK(!empty.western_border_of({0, 0}));
    CHECK(!empty.is_at_any_border({0, 0}));
    const auto original = blueprints::make_layout<TestType>(arrangement::EVEN_ROW, {5, 4, 2});
    auto       copy     = original;
    copy.resize({10, 9, 1});
    CHECK(original.get_extent() == layout_base::extent{5, 4, 2});
    CHECK(original.clone().get_extent() == original.get_extent());
    CHECK(copy.get_extent() == layout_base::extent{10, 9, 1});
    auto zero_layers = blueprints::make_layout<TestType>(arrangement::EVEN_ROW, {5, 5, 0});
    auto layered     = zero_layers.clone();
    layered.resize({10, 10, 1});
    CHECK(zero_layers.get_extent() == layout_base::extent{5, 5, 0});
    CHECK(layered.get_extent() == layout_base::extent{10, 10, 1});
    CHECK(original.width() == 5);
    CHECK(original.height() == 4);
    CHECK(original.layers() == 2);
    CHECK(original.last_coordinate() == layout_base::coordinate{4, 3, 1});
    CHECK(original.coord(-1, 2, 7) == layout_base::coordinate{-1, 2, 7});
    CHECK(!original.contains_coordinate({5, 0}));
    CHECK(!original.contains_coordinate({0, 4}));
    CHECK(!original.contains_coordinate({0, 0, 3}));
    CHECK(!original.contains_coordinate({-1, 0}));
    for (const auto sizes : {layout_base::extent{0, 4, 2}, layout_base::extent{5, 0, 2}, layout_base::extent{5, 4, 0}})
    {
        const auto layout = blueprints::make_layout<TestType>(arrangement::EVEN_ROW, sizes);
        CHECK(!layout.last_coordinate());
        CHECK(layout.coordinates().empty());
        CHECK(layout.ground_coordinates().empty());
    }
}

TEST_CASE("Coordinate iteration uses an explicit end", "[coordinates][size-contract]")
{
    using coordinate = layout_base::coordinate;
    using iterator   = layout_base::coordinate_iterator;
    const cartesian_layout  layout{{2, 2, 2}};
    std::vector<coordinate> actual{};
    layout.foreach_coordinate([&actual](const auto c) { actual.push_back(c); }, coordinate{1, 0}, coordinate{1, 1, 1});
    CHECK(actual == std::vector<coordinate>{{1, 0, 0}, {0, 1, 0}, {1, 1, 0}, {0, 0, 1}, {1, 0, 1}, {0, 1, 1}});
    CHECK(layout.coordinates(coordinate{}, coordinate{}).empty());
    CHECK(layout.coordinates(coordinate{1, 1}, coordinate{0, 0}).empty());
    CHECK(layout.coordinates(coordinate{-1, -1, -1}).begin() == layout.coordinates().begin());
    CHECK(layout.coordinates(coordinate{0, 0, 9}).empty());
    CHECK(layout.coordinates(std::nullopt, coordinate{2, 0}).end() ==
          layout.coordinates(std::nullopt, coordinate{0, 1}).end());
    CHECK_THROWS_AS(layout.ground_coordinates(coordinate{0, 0, 1}), std::invalid_argument);
    auto last = iterator{layout.get_extent(), layout.last_coordinate()};
    REQUIRE(last != iterator{});
    CHECK(*last == coordinate{1, 1, 1});
    ++last;
    CHECK(last == iterator{});
    ++last;
    CHECK(last == iterator{});
    const cartesian_layout wide{{2147483648ull, 1, 1}};
    auto                   edge = wide.coordinates(coordinate{2147483647, 0}).begin();
    CHECK(*edge == coordinate{2147483647, 0});
    ++edge;
    CHECK(edge == wide.coordinates().end());
}

TEMPLATE_TEST_CASE("Geometry ranges enumerate half-open sizes", "[coordinates][size-contract]", cartesian_layout,
                   shifted_cartesian_layout, hexagonal_layout)
{
    const auto a =
        GENERATE(arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN);
    check_visited_coordinates<TestType>(a);
}

TEMPLATE_TEST_CASE("Geometry neighbor callbacks propagate exceptions", "[coordinates][neighbor-exceptions]",
                   cartesian_layout, shifted_cartesian_layout, hexagonal_layout)
{
    const auto layout = blueprints::make_layout<TestType>(arrangement::ODD_ROW, {3, 3});
    CHECK_THROWS_AS(
        layout.foreach_adjacent_coordinate({1, 1}, [](const auto&) { throw std::runtime_error("callback"); }),
        std::runtime_error);
    CHECK_THROWS_AS(
        layout.foreach_adjacent_opposite_coordinates({1, 1}, [](const auto&) { throw std::runtime_error("callback"); }),
        std::runtime_error);
}

/** @brief Adjacent traversals invoke move-only visitors as lvalues. */
TEMPLATE_TEST_CASE("Geometry invokes temporary and lvalue visitors as lvalues", "[coordinates][visitors]",
                   cartesian_layout, shifted_cartesian_layout, hexagonal_layout)
{
    /** @brief Geometry with an interior coordinate. */
    const auto layout = []
    {
        if constexpr (std::same_as<TestType, cartesian_layout>)
        {
            return TestType{{3, 3}};
        }
        else
        {
            return TestType{arrangement::ODD_ROW, {3, 3}};
        }
    }();
    /** @brief Move-only visitor callable only through an lvalue. */
    struct visitor
    {
        /** @brief Owned invocation count. */
        std::unique_ptr<uint32_t> count;
        /** @brief Observed invocation count. */
        uint32_t& observed;
        /** @brief Records one adjacent coordinate. */
        void operator()(const layout_base::coordinate&) &
        {
            observed = ++*count;
        }
        /** @brief Rejects consuming invocation for a coordinate. */
        void operator()(const layout_base::coordinate&) && = delete;
        /** @brief Records one opposite adjacent pair. */
        void operator()(const std::pair<layout_base::coordinate, layout_base::coordinate>&) &
        {
            observed = ++*count;
        }
        /** @brief Rejects consuming invocation for a pair. */
        void operator()(const std::pair<layout_base::coordinate, layout_base::coordinate>&) && = delete;
    };
    /** @brief Invocation count published by the visitor. */
    uint32_t observed{};
    SECTION("Temporary visitor")
    {
        layout.foreach_adjacent_coordinate({1, 1}, visitor{std::make_unique<uint32_t>(0), observed});
        CHECK(observed == layout.adjacent_coordinates({1, 1}).size());
        layout.foreach_adjacent_opposite_coordinates({1, 1}, visitor{std::make_unique<uint32_t>(0), observed});
        CHECK(observed == layout.adjacent_opposite_coordinates({1, 1}).size());
    }
    SECTION("Lvalue visitor")
    {
        /** @brief Visitor for individual coordinates. */
        visitor coordinates{std::make_unique<uint32_t>(0), observed};
        layout.foreach_adjacent_coordinate({1, 1}, coordinates);
        CHECK(observed == layout.adjacent_coordinates({1, 1}).size());
        /** @brief Visitor for coordinate pairs. */
        visitor pairs{std::make_unique<uint32_t>(0), observed};
        layout.foreach_adjacent_opposite_coordinates({1, 1}, pairs);
        CHECK(observed == layout.adjacent_opposite_coordinates({1, 1}).size());
    }
}
