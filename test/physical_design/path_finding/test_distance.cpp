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

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/physical_design/path_finding/a_star.hpp>
#include <fiction/physical_design/path_finding/distance.hpp>

#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <tuple>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::physical_design::path_finding;

TEST_CASE("Manhattan distance", "[distance]")
{
    const cartesian_layout layout{};
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
    SECTION("Direct function")
    {
        CHECK(manhattan_distance<cartesian_layout>(layout, source, target) == expected);
    }
    SECTION("Functor")
    {
        const manhattan_distance_functor<cartesian_layout> distance{};
        CHECK(distance(layout, source, target) == expected);
    }
}

TEST_CASE("Euclidean distance", "[distance]")
{
    const cartesian_layout layout{};
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
    SECTION("Direct function")
    {
        CHECK_THAT(euclidean_distance<cartesian_layout>(layout, source, target),
                   Catch::Matchers::WithinAbs(expected, 0.00001));
    }
    SECTION("Functor")
    {
        const euclidean_distance_functor<cartesian_layout> distance{};
        CHECK_THAT(distance(layout, source, target), Catch::Matchers::WithinAbs(expected, 0.00001));
    }
}

TEST_CASE("Squared Euclidean distance", "[distance]")
{
    const cartesian_layout layout{};
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
    SECTION("Direct function")
    {
        CHECK(squared_euclidean_distance<cartesian_layout>(layout, source, target) == expected);
    }
    SECTION("Functor")
    {
        const squared_euclidean_distance_functor<cartesian_layout> distance{};
        CHECK(distance(layout, source, target) == expected);
    }
}

TEST_CASE("2DDWave distance", "[distance]")
{
    const cartesian_layout layout{};
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
    SECTION("Direct function")
    {
        CHECK(twoddwave_distance<cartesian_layout>(layout, source, target) == expected);
    }
    SECTION("Functor")
    {
        const twoddwave_distance_functor<cartesian_layout> distance{};
        CHECK(distance(layout, source, target) == expected);
    }
}

TEST_CASE("Chebyshev distance", "[distance]")
{
    const cartesian_layout layout{};
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
    SECTION("Direct function")
    {
        CHECK(chebyshev_distance<cartesian_layout>(layout, source, target) == expected);
    }
    SECTION("Functor")
    {
        const chebyshev_distance_functor<cartesian_layout> distance{};
        CHECK(distance(layout, source, target) == expected);
    }
}

TEST_CASE("A* distance", "[distance]")
{
    SECTION("Cartesian layout")
    {
        using lyt = cartesian_layout;

        SECTION("coordinate path distance")
        {
            const lyt layout{lyt::extent{10, 5, 2}};

            SECTION("Default distance type (uint64_t)")
            {
                CHECK(a_star_distance<lyt>(layout, {0, 0}, {0, 0}) == 0);
                CHECK(a_star_distance<lyt>(layout, {1, 1}, {1, 1}) == 0);
                CHECK(a_star_distance<lyt>(layout, {0, 0}, {0, 1}) == 1);
                CHECK(a_star_distance<lyt>(layout, {0, 0}, {1, 1}) == 2);
                CHECK(a_star_distance<lyt>(layout, {9, 1}, {6, 2}) == 4);
                CHECK(a_star_distance<lyt>(layout, {6, 2}, {0, 4}) == 8);
                CHECK(a_star_distance<lyt>(layout, {0, 4}, {9, 1}) == 12);

                // A* is not meant for routing to the z-layer
                CHECK(a_star_distance<lyt>(layout, {6, 2, 0}, {0, 4, 1}) == std::numeric_limits<uint64_t>::max());
                CHECK(a_star_distance<lyt>(layout, {0, 4, 1}, {9, 1, 1}) == std::numeric_limits<uint64_t>::max());
            }
        }
        SECTION("clocking path distance")
        {
            using clk_lyt = gate_level_layout<lyt>;

            SECTION("2DDWave")
            {
                const clk_lyt layout{clk_lyt::extent{10, 5, 2}, clocking::twoddwave()};

                SECTION("Default distance type (uint64_t)")
                {
                    CHECK(a_star_distance<clk_lyt>(layout, {0, 0}, {0, 0}) == 0);
                    CHECK(a_star_distance<clk_lyt>(layout, {1, 1}, {1, 1}) == 0);
                    CHECK(a_star_distance<clk_lyt>(layout, {0, 0}, {0, 1}) == 1);
                    CHECK(a_star_distance<clk_lyt>(layout, {0, 0}, {1, 1}) == 2);
                    CHECK(a_star_distance<clk_lyt>(layout, {9, 1}, {6, 2}) == std::numeric_limits<uint64_t>::max());
                    CHECK(a_star_distance<clk_lyt>(layout, {6, 2}, {0, 4}) == std::numeric_limits<uint64_t>::max());
                    CHECK(a_star_distance<clk_lyt>(layout, {0, 4}, {9, 1}) == std::numeric_limits<uint64_t>::max());

                    // A* is not meant for routing to the z-layer
                    CHECK(a_star_distance<clk_lyt>(layout, {6, 2, 0}, {0, 4, 1}) ==
                          std::numeric_limits<uint64_t>::max());
                    CHECK(a_star_distance<clk_lyt>(layout, {0, 4, 1}, {9, 1, 1}) ==
                          std::numeric_limits<uint64_t>::max());
                }
                SECTION("Floating-point distance type (double)")
                {
                    using namespace Catch::Matchers;

                    CHECK_THAT((a_star_distance<clk_lyt, double>(layout, {0, 0}, {0, 0})), WithinAbs(0.0, 0.00001));
                    CHECK_THAT((a_star_distance<clk_lyt, double>(layout, {1, 1}, {1, 1})), WithinAbs(0.0, 0.00001));
                    CHECK_THAT((a_star_distance<clk_lyt, double>(layout, {0, 0}, {0, 1})), WithinAbs(1.0, 0.00001));
                    CHECK_THAT((a_star_distance<clk_lyt, double>(layout, {0, 0}, {1, 1})), WithinAbs(2.0, 0.00001));

                    CHECK(std::isinf(a_star_distance<clk_lyt, double>(layout, {9, 1}, {6, 2})));
                    CHECK(std::isinf(a_star_distance<clk_lyt, double>(layout, {6, 2}, {0, 4})));
                    CHECK(std::isinf(a_star_distance<clk_lyt, double>(layout, {0, 4}, {9, 1})));

                    // A* is not meant for routing in the z-layer
                    CHECK(std::isinf(a_star_distance<clk_lyt, double>(layout, {6, 2, 1}, {0, 4, 0})));
                    CHECK(std::isinf(a_star_distance<clk_lyt, double>(layout, {6, 2, 0}, {0, 4, 1})));
                    CHECK(std::isinf(a_star_distance<clk_lyt, double>(layout, {0, 4, 1}, {9, 1, 1})));
                }
            }
        }
    }
}

TEST_CASE("A* distance functor", "[distance]")
{
    SECTION("Cartesian layout")
    {
        using lyt = cartesian_layout;

        SECTION("coordinate path distance")
        {
            const lyt layout{lyt::extent{10, 5, 2}};

            const a_star_distance_functor<lyt> distance{};

            CHECK(distance(layout, {0, 0}, {0, 0}) == 0);
            CHECK(distance(layout, {1, 1}, {1, 1}) == 0);
            CHECK(distance(layout, {0, 0}, {0, 1}) == 1);
            CHECK(distance(layout, {0, 0}, {1, 1}) == 2);
            CHECK(distance(layout, {9, 1}, {6, 2}) == 4);
            CHECK(distance(layout, {6, 2}, {0, 4}) == 8);
            CHECK(distance(layout, {0, 4}, {9, 1}) == 12);

            // A* is not meant for routing to the z-layer
            CHECK(distance(layout, {6, 2, 0}, {0, 4, 1}) == std::numeric_limits<uint64_t>::max());
            CHECK(distance(layout, {0, 4, 1}, {9, 1, 1}) == std::numeric_limits<uint64_t>::max());
        }
        SECTION("clocking path distance")
        {
            using clk_lyt = gate_level_layout<lyt>;

            SECTION("2DDWave")
            {
                const clk_lyt layout{clk_lyt::extent{10, 5, 2}, clocking::twoddwave()};

                SECTION("Default distance type (uint64_t)")
                {
                    const a_star_distance_functor<clk_lyt> distance{};

                    CHECK(distance(layout, {0, 0}, {0, 0}) == 0);
                    CHECK(distance(layout, {1, 1}, {1, 1}) == 0);
                    CHECK(distance(layout, {0, 0}, {0, 1}) == 1);
                    CHECK(distance(layout, {0, 0}, {1, 1}) == 2);
                    CHECK(distance(layout, {9, 1}, {6, 2}) == std::numeric_limits<uint64_t>::max());
                    CHECK(distance(layout, {6, 2}, {0, 4}) == std::numeric_limits<uint64_t>::max());
                    CHECK(distance(layout, {0, 4}, {9, 1}) == std::numeric_limits<uint64_t>::max());

                    // A* is not meant for routing to the z-layer
                    CHECK(distance(layout, {6, 2, 0}, {0, 4, 1}) == std::numeric_limits<uint64_t>::max());
                    CHECK(distance(layout, {0, 4, 1}, {9, 1, 1}) == std::numeric_limits<uint64_t>::max());
                }
                SECTION("Floating-point distance type (double)")
                {
                    using namespace Catch::Matchers;

                    const a_star_distance_functor<clk_lyt, double> distance{};

                    CHECK_THAT(distance(layout, {0, 0}, {0, 0}), WithinAbs(0.0, 0.00001));
                    CHECK_THAT(distance(layout, {1, 1}, {1, 1}), WithinAbs(0.0, 0.00001));
                    CHECK_THAT(distance(layout, {0, 0}, {0, 1}), WithinAbs(1.0, 0.00001));
                    CHECK_THAT(distance(layout, {0, 0}, {1, 1}), WithinAbs(2.0, 0.00001));

                    CHECK(std::isinf(distance(layout, {9, 1}, {6, 2})));
                    CHECK(std::isinf(distance(layout, {6, 2}, {0, 4})));
                    CHECK(std::isinf(distance(layout, {0, 4}, {9, 1})));

                    // A* is not meant for routing to the z-layer
                    CHECK(std::isinf(distance(layout, {6, 2, 0}, {0, 4, 1})));
                    CHECK(std::isinf(distance(layout, {0, 4, 1}, {9, 1, 1})));
                }
            }
        }
    }
}

TEST_CASE("Squared Euclidean distance checks integral range", "[distance]")
{
    /** Layout used by the coordinate distance API. */
    const cartesian_layout layout{};
    STATIC_REQUIRE(squared_euclidean_distance(layout, {-2147483647, 0}, {2147483647, 0}) == 18446744056529682436ULL);
    STATIC_REQUIRE(squared_euclidean_distance(layout, {std::numeric_limits<int32_t>::min(), 0},
                                              {std::numeric_limits<int32_t>::max(), 0}) == 18446744065119617025ULL);
    CHECK_THROWS_AS(squared_euclidean_distance(layout, {-2147483647, -2147483647}, {2147483647, 2147483647}),
                    std::overflow_error);
    CHECK_THROWS_AS((squared_euclidean_distance<cartesian_layout, uint8_t>(layout, {0, 0}, {16, 0})),
                    std::overflow_error);
    CHECK_THROWS_AS((squared_euclidean_distance<cartesian_layout, int8_t>(layout, {0, 0}, {8, 8})),
                    std::overflow_error);
    CHECK((squared_euclidean_distance<cartesian_layout, uint8_t>(layout, {0, 0}, {15, 0})) == 225);
}
