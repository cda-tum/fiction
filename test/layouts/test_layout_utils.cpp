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
 * @brief Tests for `fiction/layouts/layout_utils.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 * @author Willem Lambooy (wlambooy)
 */

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <fiction/layouts/arrangement.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/layouts/layout_base.hpp>
#include <fiction/layouts/layout_utils.hpp>
#include <fiction/technology/qca/layout.hpp>
#include <fiction/types.hpp>

#include <array>
#include <concepts>
#include <cstdint>
#include <future>
#include <latch>
#include <limits>
#include <optional>
#include <stdexcept>
#include <tuple>

using namespace fiction;
using namespace fiction::fcn;
using namespace fiction::layouts;

TEST_CASE("Gate-level layouts are created with the required arrangement", "[layout-utils]")
{
    const auto a =
        GENERATE(arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN);

    SECTION("Cartesian layouts ignore the arrangement")
    {
        CHECK_NOTHROW(make_gate_level_layout<cartesian_gate_layout>(std::nullopt, {3, 3}, clocking::twoddwave()));
        CHECK_NOTHROW(make_gate_level_layout<cartesian_gate_layout>(a, {3, 3}, clocking::twoddwave()));
    }
    SECTION("shifted Cartesian and hexagonal layouts take the arrangement")
    {
        CHECK(
            make_gate_level_layout<shifted_cartesian_gate_layout>(a, {3, 3}, clocking::twoddwave()).get_arrangement() ==
            a);
        CHECK(make_gate_level_layout<hexagonal_gate_layout>(a, {3, 3}, clocking::row()).get_arrangement() == a);
    }
    SECTION("shifted Cartesian and hexagonal layouts reject a missing arrangement")
    {
        CHECK_THROWS_AS(
            make_gate_level_layout<shifted_cartesian_gate_layout>(std::nullopt, {3, 3}, clocking::twoddwave()),
            std::invalid_argument);
        CHECK_THROWS_AS(make_gate_level_layout<hexagonal_gate_layout>(std::nullopt, {3, 3}, clocking::row()),
                        std::invalid_argument);
    }
}

TEMPLATE_TEST_CASE("Port directions to coordinates", "[layout-utils]", (cartesian_layout), (hexagonal_layout))
{
    const auto a =
        GENERATE(arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN);

    const auto lyt = [](const arrangement layout_arrangement)
    {
        if constexpr (std::derived_from<TestType, fiction::layouts::cartesian_layout>)
        {
            return TestType{{5, 5}};
        }
        else
        {
            return TestType{layout_arrangement, {5, 5}};
        }
    }(a);

    lyt.foreach_coordinate(
        [&lyt](const auto& c)
        {
            CHECK(port_direction_to_coordinate(lyt, c, port_direction{port_direction::cardinal::NORTH}) ==
                  lyt.north(c));
            CHECK(port_direction_to_coordinate(lyt, c, port_direction{port_direction::cardinal::NORTH_EAST}) ==
                  lyt.north_east(c));
            CHECK(port_direction_to_coordinate(lyt, c, port_direction{port_direction::cardinal::EAST}) == lyt.east(c));
            CHECK(port_direction_to_coordinate(lyt, c, port_direction{port_direction::cardinal::SOUTH_EAST}) ==
                  lyt.south_east(c));
            CHECK(port_direction_to_coordinate(lyt, c, port_direction{port_direction::cardinal::SOUTH}) ==
                  lyt.south(c));
            CHECK(port_direction_to_coordinate(lyt, c, port_direction{port_direction::cardinal::SOUTH_WEST}) ==
                  lyt.south_west(c));
            CHECK(port_direction_to_coordinate(lyt, c, port_direction{port_direction::cardinal::WEST}) == lyt.west(c));
            CHECK(port_direction_to_coordinate(lyt, c, port_direction{port_direction::cardinal::NORTH_WEST}) ==
                  lyt.north_west(c));
        });
}

TEST_CASE("Random coordinates stay inside inclusive corner bounds", "[layout-utils]")
{
    /** Coordinate type sampled by the public helper. */
    using coord = layout_base::coordinate;
    /** Input corners and independently specified minimum and maximum axes. */
    const auto [first, second, minimum, maximum] = GENERATE(
        std::tuple{coord{0, 0, 0}, coord{0, 0, 0}, coord{0, 0, 0}, coord{0, 0, 0}},
        std::tuple{coord{1, 0, 0}, coord{1, 0, 0}, coord{1, 0, 0}, coord{1, 0, 0}},
        std::tuple{coord{1, 1, 1}, coord{5, 2, 3}, coord{1, 1, 1}, coord{5, 2, 3}},
        std::tuple{coord{-10, -5, 0}, coord{-10, -5, 0}, coord{-10, -5, 0}, coord{-10, -5, 0}},
        std::tuple{coord{-10, -1, 3}, coord{-10, -1, 6}, coord{-10, -1, 3}, coord{-10, -1, 6}},
        std::tuple{coord{5, 1, 0}, coord{1, 2, 0}, coord{1, 1, 0}, coord{5, 2, 0}},
        std::tuple{coord{5, -1, 3}, coord{1, 2, -4}, coord{1, -1, -4}, coord{5, 2, 3}},
        std::tuple{coord{std::numeric_limits<int32_t>::min(), 0, 0}, coord{std::numeric_limits<int32_t>::max(), 0, 0},
                   coord{std::numeric_limits<int32_t>::min(), 0, 0}, coord{std::numeric_limits<int32_t>::max(), 0, 0}});
    /** Both argument orders describe the same region. */
    const auto reverse = GENERATE(false, true);

    for (uint32_t sample = 0; sample < 32; ++sample)
    {
        /** Sample from the specified coordinate region. */
        const auto c = reverse ? random_coordinate<coord>(second, first) : random_coordinate<coord>(first, second);
        CHECK(c.x >= minimum.x);
        CHECK(c.x <= maximum.x);
        CHECK(c.y >= minimum.y);
        CHECK(c.y <= maximum.y);
        CHECK(c.z >= minimum.z);
        CHECK(c.z <= maximum.z);
    }
}

TEST_CASE("Concurrent random-coordinate calls stay inside their bounds", "[layout-utils]")
{
    /** @brief Concurrent samplers report whether every coordinate stayed in bounds. */
    std::array<std::future<bool>, 4> samplers{};
    /** @brief Starts every sampler after all worker threads are ready. */
    std::latch start{samplers.size()};
    for (auto& sampler : samplers)
    {
        sampler = std::async(std::launch::async,
                             [&start]
                             {
                                 start.arrive_and_wait();
                                 for (uint32_t sample = 0; sample < 1000; ++sample)
                                 {
                                     /** @brief Coordinate sampled concurrently with the other workers. */
                                     const auto c = random_coordinate(layout_base::coordinate{-3, 4, 1},
                                                                      layout_base::coordinate{2, -5, 0});
                                     if (c.x < -3 || c.x > 2 || c.y < -5 || c.y > 4 || c.z < 0 || c.z > 1)
                                     {
                                         return false;
                                     }
                                 }
                                 return true;
                             });
    }
    for (auto& sampler : samplers)
    {
        CHECK(sampler.get());
    }
}

TEST_CASE("Normalize QCA layout coordinates", "[layout-utils]")
{
    qca::layout lyt{{7, 6, 2}, clocking::use(), "crossing", 2, 2};

    lyt.assign_cell_type({3, 2}, qca::cell_type::INPUT);
    lyt.assign_cell_name({3, 2}, "a");
    lyt.assign_cell_type({4, 3}, qca::cell_type::NORMAL);
    lyt.assign_cell_type({4, 3, 1}, qca::cell_type::NORMAL);
    lyt.assign_cell_mode({4, 3, 1}, qca::cell_mode::VERTICAL);

    const auto normalized = normalize_layout_coordinates(lyt);

    CHECK(normalized.width() == 4);
    CHECK(normalized.height() == 4);
    CHECK(normalized.layers() == 2);
    CHECK(normalized.num_cells() == 3);
    CHECK(normalized.get_cell_type({0, 0}) == qca::cell_type::INPUT);
    CHECK(normalized.get_cell_name({0, 0}) == "a");
    CHECK(normalized.get_cell_type({1, 1}) == qca::cell_type::NORMAL);
    CHECK(normalized.get_cell_type({1, 1, 1}) == qca::cell_type::NORMAL);
    CHECK(normalized.get_cell_mode({1, 1, 1}) == qca::cell_mode::VERTICAL);
    CHECK(normalized.get_layout_name() == "crossing");
    CHECK(normalized.is_clocking_scheme(clocking::USE_NAME));
    CHECK(normalized.get_tile_size_x() == 2);

    CHECK(lyt.get_cell_type({3, 2}) == qca::cell_type::INPUT);
}
TEST_CASE("Normalize occupied cells outside the layout extent", "[layout-utils]")
{
    /** Frame may be empty or smaller than its occupied coordinates. */
    const auto size = GENERATE(0, 3);
    /** Sparse layout with occupied cells beyond its declared frame. */
    qca::layout lyt{{size, size, size == 0 ? 0 : 1}};
    lyt.assign_cell_type({5, 7}, qca::cell_type::INPUT);
    lyt.assign_cell_type({8, 9}, qca::cell_type::NORMAL);
    /** Normalized frame encloses every translated cell. */
    const auto normalized = normalize_layout_coordinates(lyt);
    CHECK(normalized.get_cell_type({0, 0}) == qca::cell_type::INPUT);
    CHECK(normalized.get_cell_type({3, 2}) == qca::cell_type::NORMAL);
    CHECK(normalized.width() == 4);
    CHECK(normalized.height() == 3);
    CHECK(normalized.layers() == 1);
}

TEST_CASE("Normalize negative cells preserves trailing padding", "[layout-utils]")
{
    /** Layout with negative occupied coordinates and trailing padding. */
    qca::layout lyt{{4, 5, 1}};
    lyt.assign_cell_type({-2, -3}, qca::cell_type::INPUT);
    /** Translated layout retains the original upper frame edges. */
    const auto normalized = normalize_layout_coordinates(lyt);
    CHECK(normalized.get_cell_type({0, 0}) == qca::cell_type::INPUT);
    CHECK(normalized.width() == 6);
    CHECK(normalized.height() == 8);
}

TEST_CASE("Normalize rejects an extent beyond the coordinate domain", "[layout-utils]")
{
    /** Occupied span wider than a nonnegative signed coordinate axis. */
    qca::layout lyt{};
    lyt.assign_cell_type({std::numeric_limits<int32_t>::min(), 0}, qca::cell_type::INPUT);
    lyt.assign_cell_type({std::numeric_limits<int32_t>::max(), 0}, qca::cell_type::NORMAL);
    CHECK_THROWS_AS(normalize_layout_coordinates(lyt), std::invalid_argument);
    CHECK(lyt.num_cells() == 2);
}
