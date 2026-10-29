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
 * @brief Tests for `fiction/physical_design/path_finding/distance.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/layouts/layout_base.hpp>
#include <fiction/physical_design/path_finding/a_star.hpp>
#include <fiction/physical_design/path_finding/distance.hpp>

#include <cmath>
#include <cstdint>
#include <limits>
#include <tuple>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::physical_design::path_finding;

TEST_CASE("Manhattan distance", "[distance]")
{
    const auto [source, target, expected] =
        GENERATE(std::tuple{layout_base::coordinate{0, 0}, layout_base::coordinate{0, 0}, uint64_t{0}},
                 std::tuple{layout_base::coordinate{1, 1}, layout_base::coordinate{1, 1}, uint64_t{0}},
                 std::tuple{layout_base::coordinate{0, 0}, layout_base::coordinate{0, 1}, uint64_t{1}},
                 std::tuple{layout_base::coordinate{0, 0}, layout_base::coordinate{1, 1}, uint64_t{2}},
                 std::tuple{layout_base::coordinate{1, 2}, layout_base::coordinate{3, 3}, uint64_t{3}},
                 std::tuple{layout_base::coordinate{0, 0}, layout_base::coordinate{4, 4}, uint64_t{8}},
                 std::tuple{layout_base::coordinate{4, 4}, layout_base::coordinate{0, 0}, uint64_t{8}},
                 std::tuple{layout_base::coordinate{0, 0, 1}, layout_base::coordinate{8, 9, 0}, uint64_t{17}},
                 std::tuple{layout_base::coordinate{0, 0, 1}, layout_base::coordinate{8, 9, 1}, uint64_t{17}},
                 std::tuple{layout_base::coordinate{0, 0}, layout_base::coordinate{-1, -1}, uint64_t{2}},
                 std::tuple{layout_base::coordinate{-4, -3}, layout_base::coordinate{1, -1}, uint64_t{7}},
                 std::tuple{layout_base::coordinate{-2, -8}, layout_base::coordinate{-6, -4}, uint64_t{8}});
    CAPTURE(source, target);
    SECTION("Direct function")
    {
        CHECK(manhattan_distance(source, target) == expected);
    }
    SECTION("Functor")
    {
        const manhattan_distance_functor distance{};
        CHECK(distance(source, target) == expected);
    }
}

TEST_CASE("Euclidean distance", "[distance]")
{
    const auto [source, target, expected] =
        GENERATE(std::tuple{layout_base::coordinate{0, 0}, layout_base::coordinate{0, 0}, double{0.0}},
                 std::tuple{layout_base::coordinate{1, 1}, layout_base::coordinate{1, 1}, double{0.0}},
                 std::tuple{layout_base::coordinate{0, 0}, layout_base::coordinate{0, 1}, double{1.0}},
                 std::tuple{layout_base::coordinate{0, 0}, layout_base::coordinate{1, 1}, double{std::sqrt(2)}},
                 std::tuple{layout_base::coordinate{9, 1}, layout_base::coordinate{6, 2}, double{std::sqrt(10)}},
                 std::tuple{layout_base::coordinate{6, 2}, layout_base::coordinate{0, 4}, double{std::sqrt(40)}},
                 std::tuple{layout_base::coordinate{0, 4}, layout_base::coordinate{9, 1}, double{std::sqrt(90)}},
                 std::tuple{layout_base::coordinate{6, 2, 1}, layout_base::coordinate{0, 4, 0}, double{std::sqrt(40)}},
                 std::tuple{layout_base::coordinate{6, 2, 0}, layout_base::coordinate{0, 4, 1}, double{std::sqrt(40)}},
                 std::tuple{layout_base::coordinate{0, 4, 1}, layout_base::coordinate{9, 1, 1}, double{std::sqrt(90)}},
                 std::tuple{layout_base::coordinate{0, 0}, layout_base::coordinate{-1, -1}, double{std::sqrt(2)}},
                 std::tuple{layout_base::coordinate{-4, -3}, layout_base::coordinate{1, -1}, double{std::sqrt(29)}},
                 std::tuple{layout_base::coordinate{-2, -8}, layout_base::coordinate{-6, -4}, double{std::sqrt(32)}});
    CAPTURE(source, target);
    SECTION("Direct function")
    {
        CHECK_THAT(euclidean_distance(source, target), Catch::Matchers::WithinAbs(expected, 0.00001));
    }
    SECTION("Functor")
    {
        const euclidean_distance_functor distance{};
        CHECK_THAT(distance(source, target), Catch::Matchers::WithinAbs(expected, 0.00001));
    }
}

TEST_CASE("Squared Euclidean distance", "[distance]")
{
    const auto [source, target, expected] =
        GENERATE(std::tuple{layout_base::coordinate{0, 0}, layout_base::coordinate{0, 0}, uint64_t{0}},
                 std::tuple{layout_base::coordinate{1, 1}, layout_base::coordinate{1, 1}, uint64_t{0}},
                 std::tuple{layout_base::coordinate{0, 0}, layout_base::coordinate{0, 1}, uint64_t{1}},
                 std::tuple{layout_base::coordinate{0, 0}, layout_base::coordinate{1, 1}, uint64_t{2}},
                 std::tuple{layout_base::coordinate{9, 1}, layout_base::coordinate{6, 2}, uint64_t{10}},
                 std::tuple{layout_base::coordinate{6, 2}, layout_base::coordinate{0, 4}, uint64_t{40}},
                 std::tuple{layout_base::coordinate{0, 4}, layout_base::coordinate{9, 1}, uint64_t{90}},
                 std::tuple{layout_base::coordinate{6, 2, 1}, layout_base::coordinate{0, 4, 0}, uint64_t{40}},
                 std::tuple{layout_base::coordinate{6, 2, 0}, layout_base::coordinate{0, 4, 1}, uint64_t{40}},
                 std::tuple{layout_base::coordinate{0, 4, 1}, layout_base::coordinate{9, 1, 1}, uint64_t{90}});
    CAPTURE(source, target);
    SECTION("Direct function")
    {
        CHECK(squared_euclidean_distance(source, target) == expected);
    }
    SECTION("Functor")
    {
        const squared_euclidean_distance_functor distance{};
        CHECK(distance(source, target) == expected);
    }
}

TEST_CASE("2DDWave distance", "[distance]")
{
    const auto [source, target, expected] =
        GENERATE(std::tuple{layout_base::coordinate{0, 0}, layout_base::coordinate{0, 0}, uint64_t{0}},
                 std::tuple{layout_base::coordinate{1, 1}, layout_base::coordinate{1, 1}, uint64_t{0}},
                 std::tuple{layout_base::coordinate{0, 0}, layout_base::coordinate{0, 1}, uint64_t{1}},
                 std::tuple{layout_base::coordinate{0, 0}, layout_base::coordinate{1, 1}, uint64_t{2}},
                 std::tuple{layout_base::coordinate{1, 2}, layout_base::coordinate{3, 3}, uint64_t{3}},
                 std::tuple{layout_base::coordinate{0, 0}, layout_base::coordinate{4, 4}, uint64_t{8}},
                 std::tuple{layout_base::coordinate{4, 4}, layout_base::coordinate{0, 0},
                            uint64_t{std::numeric_limits<uint32_t>::max()}},
                 std::tuple{layout_base::coordinate{2, 1}, layout_base::coordinate{0, 2},
                            uint64_t{std::numeric_limits<uint32_t>::max()}},
                 std::tuple{layout_base::coordinate{1, 0}, layout_base::coordinate{0, 1},
                            uint64_t{std::numeric_limits<uint32_t>::max()}},
                 std::tuple{layout_base::coordinate{0, 0, 1}, layout_base::coordinate{8, 9, 0}, uint64_t{17}},
                 std::tuple{layout_base::coordinate{0, 0, 1}, layout_base::coordinate{8, 9, 1}, uint64_t{17}},
                 std::tuple{layout_base::coordinate{0, 0}, layout_base::coordinate{-1, -1},
                            uint64_t{std::numeric_limits<uint32_t>::max()}},
                 std::tuple{layout_base::coordinate{-2, -8}, layout_base::coordinate{-6, -4},
                            uint64_t{std::numeric_limits<uint32_t>::max()}},
                 std::tuple{layout_base::coordinate{-6, -4}, layout_base::coordinate{-2, -8},
                            uint64_t{std::numeric_limits<uint32_t>::max()}},
                 std::tuple{layout_base::coordinate{-4, -3}, layout_base::coordinate{1, -1}, uint64_t{7}});
    CAPTURE(source, target);
    SECTION("Direct function")
    {
        CHECK(twoddwave_distance(source, target) == expected);
    }
    SECTION("Functor")
    {
        const twoddwave_distance_functor distance{};
        CHECK(distance(source, target) == expected);
    }
}

TEST_CASE("Chebyshev distance", "[distance]")
{
    const auto [source, target, expected] =
        GENERATE(std::tuple{layout_base::coordinate{0, 0}, layout_base::coordinate{0, 0}, uint64_t{0}},
                 std::tuple{layout_base::coordinate{1, 1}, layout_base::coordinate{1, 1}, uint64_t{0}},
                 std::tuple{layout_base::coordinate{0, 0}, layout_base::coordinate{0, 1}, uint64_t{1}},
                 std::tuple{layout_base::coordinate{0, 0}, layout_base::coordinate{1, 1}, uint64_t{1}},
                 std::tuple{layout_base::coordinate{1, 2}, layout_base::coordinate{3, 3}, uint64_t{2}},
                 std::tuple{layout_base::coordinate{0, 0}, layout_base::coordinate{4, 4}, uint64_t{4}},
                 std::tuple{layout_base::coordinate{4, 4}, layout_base::coordinate{0, 0}, uint64_t{4}},
                 std::tuple{layout_base::coordinate{2, 1}, layout_base::coordinate{0, 2}, uint64_t{2}},
                 std::tuple{layout_base::coordinate{1, 0}, layout_base::coordinate{0, 1}, uint64_t{1}},
                 std::tuple{layout_base::coordinate{0, 0, 1}, layout_base::coordinate{8, 9, 0}, uint64_t{9}},
                 std::tuple{layout_base::coordinate{0, 0, 1}, layout_base::coordinate{8, 9, 1}, uint64_t{9}});
    CAPTURE(source, target);
    SECTION("Direct function")
    {
        CHECK(chebyshev_distance(source, target) == expected);
    }
    SECTION("Functor")
    {
        const chebyshev_distance_functor distance{};
        CHECK(distance(source, target) == expected);
    }
}

TEST_CASE("A* distance and functor", "[distance]")
{
    /** @brief Endpoints and independent coordinate/clocked distance expectations. */
    const auto [source, target, coordinate_distance, clocked_distance] =
        GENERATE(std::tuple{layout_base::coordinate{0, 0}, layout_base::coordinate{0, 0}, uint64_t{0}, uint64_t{0}},
                 std::tuple{layout_base::coordinate{1, 1}, layout_base::coordinate{1, 1}, uint64_t{0}, uint64_t{0}},
                 std::tuple{layout_base::coordinate{0, 0}, layout_base::coordinate{0, 1}, uint64_t{1}, uint64_t{1}},
                 std::tuple{layout_base::coordinate{0, 0}, layout_base::coordinate{1, 1}, uint64_t{2}, uint64_t{2}},
                 std::tuple{layout_base::coordinate{9, 1}, layout_base::coordinate{6, 2}, uint64_t{4},
                            std::numeric_limits<uint64_t>::max()},
                 std::tuple{layout_base::coordinate{6, 2}, layout_base::coordinate{0, 4}, uint64_t{8},
                            std::numeric_limits<uint64_t>::max()},
                 std::tuple{layout_base::coordinate{0, 4}, layout_base::coordinate{9, 1}, uint64_t{12},
                            std::numeric_limits<uint64_t>::max()},
                 std::tuple{layout_base::coordinate{6, 2, 0}, layout_base::coordinate{0, 4, 1},
                            std::numeric_limits<uint64_t>::max(), std::numeric_limits<uint64_t>::max()},
                 std::tuple{layout_base::coordinate{0, 4, 1}, layout_base::coordinate{9, 1, 1},
                            std::numeric_limits<uint64_t>::max(), std::numeric_limits<uint64_t>::max()});
    CAPTURE(source, target);

    SECTION("coordinate path distance")
    {
        /** @brief Coordinate geometry with disconnected crossing layers. */
        const cartesian_layout layout{{10, 5, 2}};
        /** @brief Borrowed-layout wrapper for the same distance contract. */
        const a_star_distance_functor<cartesian_layout> distance{layout};
        CHECK(a_star_distance(layout, source, target) == coordinate_distance);
        CHECK(distance(source, target) == coordinate_distance);
    }
    SECTION("2DDWave path distance")
    {
        /** @brief Directed clocking forbids westbound and northbound routes. */
        const cartesian_gate_layout layout{{10, 5, 2}, clocking::twoddwave()};
        /** @brief Borrowed-layout wrapper for clocked distances. */
        const a_star_distance_functor<cartesian_gate_layout> distance{layout};
        CHECK(a_star_distance(layout, source, target) == clocked_distance);
        CHECK(distance(source, target) == clocked_distance);
    }
}

TEST_CASE("Squared Euclidean distance widens signed differences", "[distance]")
{
    STATIC_REQUIRE(noexcept(squared_euclidean_distance({}, {})));
    STATIC_REQUIRE(squared_euclidean_distance({-2147483647, 0}, {2147483647, 0}) == 18446744056529682436ULL);
    STATIC_REQUIRE(squared_euclidean_distance({std::numeric_limits<int32_t>::min(), 0},
                                              {std::numeric_limits<int32_t>::max(), 0}) == 18446744065119617025ULL);
}
