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

#include <fiction/layouts/arrangement.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/layouts/layout_base.hpp>

#include <fmt/format.h>

#include <concepts>
#include <cstdint>
#include <limits>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace fiction;
using namespace fiction::layouts;

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
    CHECK(volume_of(extent{2, 3, 4}) == 24);
    CHECK(area_of(extent{}) == 0);
    CHECK(volume_of(extent{2, 3, 0}) == 0);
    CHECK(area_of(extent{2147483648ull, 2147483648ull}) == 4611686018427387904ull);
    CHECK_THROWS_AS(volume_of(extent{2147483648ull, 2147483648ull, 4}), std::overflow_error);
    CHECK(volume_of(extent{2147483648ull, 2147483648ull, 3}) == 13835058055282163712ull);
    auto edited  = extent{};
    edited.width = std::numeric_limits<uint32_t>::max();
    CHECK_THROWS_AS(cartesian_layout{edited}, std::invalid_argument);
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
    auto last = iterator{layout.dimensions(), layout.last_coordinate()};
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

/** @brief Adjacent traversals invoke move-only visitors as lvalues. */
TEMPLATE_TEST_CASE("Geometry invokes temporary and lvalue visitors as lvalues", "[coordinates][visitors]",
                   cartesian_layout, hexagonal_layout)
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
