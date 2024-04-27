//
// Created by marcel on 17.02.22.
//

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/path_finding/a_star.hpp>
#include <fiction/algorithms/path_finding/distance.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>

#include <cmath>
#include <cstdint>
#include <limits>

using namespace fiction;

TEST_CASE("Manhattan distance", "[distance]")
{
    SECTION("Unsigned Cartesian layout")
    {
        using cart_lyt = cartesian_layout<offset::ucoord_t>;

        const cart_lyt layout{};

        CHECK(manhattan_distance<cart_lyt>(layout, {0, 0}, {0, 0}) == 0);
        CHECK(manhattan_distance<cart_lyt>(layout, {1, 1}, {1, 1}) == 0);
        CHECK(manhattan_distance<cart_lyt>(layout, {0, 0}, {0, 1}) == 1);
        CHECK(manhattan_distance<cart_lyt>(layout, {0, 0}, {1, 1}) == 2);
        CHECK(manhattan_distance<cart_lyt>(layout, {1, 2}, {3, 3}) == 3);
        CHECK(manhattan_distance<cart_lyt>(layout, {0, 0}, {4, 4}) == 8);
        CHECK(manhattan_distance<cart_lyt>(layout, {4, 4}, {0, 0}) == 8);

        // ignore z-axis
        CHECK(manhattan_distance<cart_lyt>(layout, {0, 0, 1}, {8, 9, 0}) == 17);
        CHECK(manhattan_distance<cart_lyt>(layout, {0, 0, 1}, {8, 9, 1}) == 17);
    }
    SECTION("Signed Cartesian layout")
    {
        using cart_lyt = cartesian_layout<cube::coord_t>;

        const cart_lyt layout{};

        CHECK(manhattan_distance<cart_lyt>(layout, {0, 0}, {0, 0}) == 0);
        CHECK(manhattan_distance<cart_lyt>(layout, {1, 1}, {1, 1}) == 0);
        CHECK(manhattan_distance<cart_lyt>(layout, {0, 0}, {0, 1}) == 1);
        CHECK(manhattan_distance<cart_lyt>(layout, {0, 0}, {1, 1}) == 2);
        CHECK(manhattan_distance<cart_lyt>(layout, {1, 2}, {3, 3}) == 3);
        CHECK(manhattan_distance<cart_lyt>(layout, {0, 0}, {4, 4}) == 8);
        CHECK(manhattan_distance<cart_lyt>(layout, {4, 4}, {0, 0}) == 8);

        // ignore z-axis
        CHECK(manhattan_distance<cart_lyt>(layout, {0, 0, 1}, {8, 9, 0}) == 17);
        CHECK(manhattan_distance<cart_lyt>(layout, {0, 0, 1}, {8, 9, 1}) == 17);

        // negative coordinates
        CHECK(manhattan_distance<cart_lyt>(layout, {0, 0}, {-1, -1}) == 2);
        CHECK(manhattan_distance<cart_lyt>(layout, {-4, -3}, {1, -1}) == 7);
        CHECK(manhattan_distance<cart_lyt>(layout, {-2, -8}, {-6, -4}) == 8);
    }
}

TEST_CASE("Manhattan distance functor", "[distance]")
{
    SECTION("Unsigned Cartesian layout")
    {
        using cart_lyt = cartesian_layout<offset::ucoord_t>;

        const cart_lyt layout{};

        const manhattan_distance_functor<cart_lyt> distance{};

        CHECK(distance(layout, {0, 0}, {0, 0}) == 0);
        CHECK(distance(layout, {1, 1}, {1, 1}) == 0);
        CHECK(distance(layout, {0, 0}, {0, 1}) == 1);
        CHECK(distance(layout, {0, 0}, {1, 1}) == 2);
        CHECK(distance(layout, {1, 2}, {3, 3}) == 3);
        CHECK(distance(layout, {0, 0}, {4, 4}) == 8);
        CHECK(distance(layout, {4, 4}, {0, 0}) == 8);

        // ignore z-axis
        CHECK(distance(layout, {0, 0, 1}, {8, 9, 0}) == 17);
        CHECK(distance(layout, {0, 0, 1}, {8, 9, 1}) == 17);
    }
    SECTION("Signed Cartesian layout")
    {
        using cart_lyt = cartesian_layout<cube::coord_t>;

        const cart_lyt layout{};

        const manhattan_distance_functor<cart_lyt> distance{};

        CHECK(distance(layout, {0, 0}, {0, 0}) == 0);
        CHECK(distance(layout, {1, 1}, {1, 1}) == 0);
        CHECK(distance(layout, {0, 0}, {0, 1}) == 1);
        CHECK(distance(layout, {0, 0}, {1, 1}) == 2);
        CHECK(distance(layout, {1, 2}, {3, 3}) == 3);
        CHECK(distance(layout, {0, 0}, {4, 4}) == 8);
        CHECK(distance(layout, {4, 4}, {0, 0}) == 8);

        // ignore z-axis
        CHECK(distance(layout, {0, 0, 1}, {8, 9, 0}) == 17);
        CHECK(distance(layout, {0, 0, 1}, {8, 9, 1}) == 17);

        // negative coordinates
        CHECK(distance(layout, {0, 0}, {-1, -1}) == 2);
        CHECK(distance(layout, {-4, -3}, {1, -1}) == 7);
        CHECK(distance(layout, {-2, -8}, {-6, -4}) == 8);
    }
}

TEST_CASE("Euclidean distance", "[distance]")
{
    using namespace Catch::Matchers;

    SECTION("Unsigned Cartesian layout")
    {
        using cart_lyt = cartesian_layout<offset::ucoord_t>;

        const cart_lyt layout{};

        CHECK_THAT(euclidean_distance<cart_lyt>(layout, {0, 0}, {0, 0}), WithinAbs(0.0, 0.00001));
        CHECK_THAT(euclidean_distance<cart_lyt>(layout, {1, 1}, {1, 1}), WithinAbs(0.0, 0.00001));
        CHECK_THAT(euclidean_distance<cart_lyt>(layout, {0, 0}, {0, 1}), WithinAbs(1.0, 0.00001));
        CHECK_THAT(euclidean_distance<cart_lyt>(layout, {0, 0}, {1, 1}), WithinAbs(std::sqrt(2), 0.00001));
        CHECK_THAT(euclidean_distance<cart_lyt>(layout, {9, 1}, {6, 2}), WithinAbs(std::sqrt(10), 0.00001));
        CHECK_THAT(euclidean_distance<cart_lyt>(layout, {6, 2}, {0, 4}), WithinAbs(std::sqrt(40), 0.00001));
        CHECK_THAT(euclidean_distance<cart_lyt>(layout, {0, 4}, {9, 1}), WithinAbs(std::sqrt(90), 0.00001));

        // ignore z-axis
        CHECK_THAT(euclidean_distance<cart_lyt>(layout, {6, 2, 1}, {0, 4, 0}), WithinAbs(std::sqrt(40), 0.00001));
        CHECK_THAT(euclidean_distance<cart_lyt>(layout, {6, 2, 0}, {0, 4, 1}), WithinAbs(std::sqrt(40), 0.00001));
        CHECK_THAT(euclidean_distance<cart_lyt>(layout, {0, 4, 1}, {9, 1, 1}), WithinAbs(std::sqrt(90), 0.00001));
    }
    SECTION("Signed Cartesian layout")
    {
        using cart_lyt = cartesian_layout<cube::coord_t>;

        const cart_lyt layout{};

        CHECK_THAT(euclidean_distance<cart_lyt>(layout, {0, 0}, {0, 0}), WithinAbs(0.0, 0.00001));
        CHECK_THAT(euclidean_distance<cart_lyt>(layout, {1, 1}, {1, 1}), WithinAbs(0.0, 0.00001));
        CHECK_THAT(euclidean_distance<cart_lyt>(layout, {0, 0}, {0, 1}), WithinAbs(1.0, 0.00001));
        CHECK_THAT(euclidean_distance<cart_lyt>(layout, {0, 0}, {1, 1}), WithinAbs(std::sqrt(2), 0.00001));
        CHECK_THAT(euclidean_distance<cart_lyt>(layout, {9, 1}, {6, 2}), WithinAbs(std::sqrt(10), 0.00001));
        CHECK_THAT(euclidean_distance<cart_lyt>(layout, {6, 2}, {0, 4}), WithinAbs(std::sqrt(40), 0.00001));
        CHECK_THAT(euclidean_distance<cart_lyt>(layout, {0, 4}, {9, 1}), WithinAbs(std::sqrt(90), 0.00001));

        // ignore z-axis
        CHECK_THAT(euclidean_distance<cart_lyt>(layout, {6, 2, 1}, {0, 4, 0}), WithinAbs(std::sqrt(40), 0.00001));
        CHECK_THAT(euclidean_distance<cart_lyt>(layout, {6, 2, 0}, {0, 4, 1}), WithinAbs(std::sqrt(40), 0.00001));
        CHECK_THAT(euclidean_distance<cart_lyt>(layout, {0, 4, 1}, {9, 1, 1}), WithinAbs(std::sqrt(90), 0.00001));

        // negative coordinates
        CHECK_THAT(euclidean_distance<cart_lyt>(layout, {0, 0}, {-1, -1}), WithinAbs(std::sqrt(2), 0.00001));
        CHECK_THAT(euclidean_distance<cart_lyt>(layout, {-4, -3}, {1, -1}), WithinAbs(std::sqrt(29), 0.00001));
        CHECK_THAT(euclidean_distance<cart_lyt>(layout, {-2, -8}, {-6, -4}), WithinAbs(std::sqrt(32), 0.00001));
    }
}

TEST_CASE("Euclidean distance functor", "[distance]")
{
    using namespace Catch::Matchers;

    SECTION("Unsigned Cartesian layout")
    {
        using cart_lyt = cartesian_layout<offset::ucoord_t>;

        const cart_lyt layout{};

        const euclidean_distance_functor<cart_lyt> distance{};

        CHECK_THAT(distance(layout, {0, 0}, {0, 0}), WithinAbs(0.0, 0.00001));
        CHECK_THAT(distance(layout, {1, 1}, {1, 1}), WithinAbs(0.0, 0.00001));
        CHECK_THAT(distance(layout, {0, 0}, {0, 1}), WithinAbs(1.0, 0.00001));
        CHECK_THAT(distance(layout, {0, 0}, {1, 1}), WithinAbs(std::sqrt(2), 0.00001));
        CHECK_THAT(distance(layout, {9, 1}, {6, 2}), WithinAbs(std::sqrt(10), 0.00001));
        CHECK_THAT(distance(layout, {6, 2}, {0, 4}), WithinAbs(std::sqrt(40), 0.00001));
        CHECK_THAT(distance(layout, {0, 4}, {9, 1}), WithinAbs(std::sqrt(90), 0.00001));

        // ignore z-axis
        CHECK_THAT(distance(layout, {6, 2, 1}, {0, 4, 0}), WithinAbs(std::sqrt(40), 0.00001));
        CHECK_THAT(distance(layout, {6, 2, 0}, {0, 4, 1}), WithinAbs(std::sqrt(40), 0.00001));
        CHECK_THAT(distance(layout, {0, 4, 1}, {9, 1, 1}), WithinAbs(std::sqrt(90), 0.00001));
    }
    SECTION("Signed Cartesian layout")
    {
        using cart_lyt = cartesian_layout<cube::coord_t>;

        const cart_lyt layout{};

        const euclidean_distance_functor<cart_lyt> distance{};

        CHECK_THAT(distance(layout, {0, 0}, {0, 0}), WithinAbs(0.0, 0.00001));
        CHECK_THAT(distance(layout, {1, 1}, {1, 1}), WithinAbs(0.0, 0.00001));
        CHECK_THAT(distance(layout, {0, 0}, {0, 1}), WithinAbs(1.0, 0.00001));
        CHECK_THAT(distance(layout, {0, 0}, {1, 1}), WithinAbs(std::sqrt(2), 0.00001));
        CHECK_THAT(distance(layout, {9, 1}, {6, 2}), WithinAbs(std::sqrt(10), 0.00001));
        CHECK_THAT(distance(layout, {6, 2}, {0, 4}), WithinAbs(std::sqrt(40), 0.00001));
        CHECK_THAT(distance(layout, {0, 4}, {9, 1}), WithinAbs(std::sqrt(90), 0.00001));

        // ignore z-axis
        CHECK_THAT(distance(layout, {6, 2, 1}, {0, 4, 0}), WithinAbs(std::sqrt(40), 0.00001));
        CHECK_THAT(distance(layout, {6, 2, 0}, {0, 4, 1}), WithinAbs(std::sqrt(40), 0.00001));
        CHECK_THAT(distance(layout, {0, 4, 1}, {9, 1, 1}), WithinAbs(std::sqrt(90), 0.00001));

        // negative coordinates
        CHECK_THAT(distance(layout, {0, 0}, {-1, -1}), WithinAbs(std::sqrt(2), 0.00001));
        CHECK_THAT(distance(layout, {-4, -3}, {1, -1}), WithinAbs(std::sqrt(29), 0.00001));
        CHECK_THAT(distance(layout, {-2, -8}, {-6, -4}), WithinAbs(std::sqrt(32), 0.00001));
    }
}

TEST_CASE("2DDWave distance", "[distance]")
{
    SECTION("Unsigned Cartesian layout")
    {
        using cart_lyt = cartesian_layout<offset::ucoord_t>;

        const cart_lyt layout{};

        CHECK(twoddwave_distance<cart_lyt>(layout, {0, 0}, {0, 0}) == 0);
        CHECK(twoddwave_distance<cart_lyt>(layout, {1, 1}, {1, 1}) == 0);
        CHECK(twoddwave_distance<cart_lyt>(layout, {0, 0}, {0, 1}) == 1);
        CHECK(twoddwave_distance<cart_lyt>(layout, {0, 0}, {1, 1}) == 2);
        CHECK(twoddwave_distance<cart_lyt>(layout, {1, 2}, {3, 3}) == 3);
        CHECK(twoddwave_distance<cart_lyt>(layout, {0, 0}, {4, 4}) == 8);
        CHECK(twoddwave_distance<cart_lyt>(layout, {4, 4}, {0, 0}) == std::numeric_limits<uint32_t>::max());
        CHECK(twoddwave_distance<cart_lyt>(layout, {2, 1}, {0, 2}) == std::numeric_limits<uint32_t>::max());
        CHECK(twoddwave_distance<cart_lyt>(layout, {1, 0}, {0, 1}) == std::numeric_limits<uint32_t>::max());

        // ignore z-axis
        CHECK(twoddwave_distance<cart_lyt>(layout, {0, 0, 1}, {8, 9, 0}) == 17);
        CHECK(twoddwave_distance<cart_lyt>(layout, {0, 0, 1}, {8, 9, 1}) == 17);
    }
    SECTION("Signed Cartesian layout")
    {
        using cart_lyt = cartesian_layout<cube::coord_t>;

        const cart_lyt layout{};

        CHECK(twoddwave_distance<cart_lyt>(layout, {0, 0}, {0, 0}) == 0);
        CHECK(twoddwave_distance<cart_lyt>(layout, {1, 1}, {1, 1}) == 0);
        CHECK(twoddwave_distance<cart_lyt>(layout, {0, 0}, {0, 1}) == 1);
        CHECK(twoddwave_distance<cart_lyt>(layout, {0, 0}, {1, 1}) == 2);
        CHECK(twoddwave_distance<cart_lyt>(layout, {1, 2}, {3, 3}) == 3);
        CHECK(twoddwave_distance<cart_lyt>(layout, {0, 0}, {4, 4}) == 8);
        CHECK(twoddwave_distance<cart_lyt>(layout, {4, 4}, {0, 0}) == std::numeric_limits<uint32_t>::max());
        CHECK(twoddwave_distance<cart_lyt>(layout, {2, 1}, {0, 2}) == std::numeric_limits<uint32_t>::max());
        CHECK(twoddwave_distance<cart_lyt>(layout, {1, 0}, {0, 1}) == std::numeric_limits<uint32_t>::max());

        // ignore z-axis
        CHECK(twoddwave_distance<cart_lyt>(layout, {0, 0, 1}, {8, 9, 0}) == 17);
        CHECK(twoddwave_distance<cart_lyt>(layout, {0, 0, 1}, {8, 9, 1}) == 17);

        // negative coordinates
        CHECK(twoddwave_distance<cart_lyt>(layout, {0, 0}, {-1, -1}) == std::numeric_limits<uint32_t>::max());
        CHECK(twoddwave_distance<cart_lyt>(layout, {-2, -8}, {-6, -4}) == std::numeric_limits<uint32_t>::max());
        CHECK(twoddwave_distance<cart_lyt>(layout, {-6, -4}, {-2, -8}) == std::numeric_limits<uint32_t>::max());
        CHECK(twoddwave_distance<cart_lyt>(layout, {-4, -3}, {1, -1}) == 7);
    }
}

TEST_CASE("2DDWave distance functor", "[distance]")
{
    SECTION("Unsigned Cartesian layout")
    {
        using cart_lyt = cartesian_layout<offset::ucoord_t>;

        const cart_lyt layout{};

        const twoddwave_distance_functor<cart_lyt> distance{};

        CHECK(distance(layout, {0, 0}, {0, 0}) == 0);
        CHECK(distance(layout, {1, 1}, {1, 1}) == 0);
        CHECK(distance(layout, {0, 0}, {0, 1}) == 1);
        CHECK(distance(layout, {0, 0}, {1, 1}) == 2);
        CHECK(distance(layout, {1, 2}, {3, 3}) == 3);
        CHECK(distance(layout, {0, 0}, {4, 4}) == 8);
        CHECK(distance(layout, {4, 4}, {0, 0}) == std::numeric_limits<uint32_t>::max());

        // ignore z-axis
        CHECK(distance(layout, {0, 0, 1}, {8, 9, 0}) == 17);
        CHECK(distance(layout, {0, 0, 1}, {8, 9, 1}) == 17);
    }
    SECTION("Signed Cartesian layout")
    {
        using cart_lyt = cartesian_layout<cube::coord_t>;

        const cart_lyt layout{};

        const twoddwave_distance_functor<cart_lyt> distance{};

        CHECK(distance(layout, {0, 0}, {0, 0}) == 0);
        CHECK(distance(layout, {1, 1}, {1, 1}) == 0);
        CHECK(distance(layout, {0, 0}, {0, 1}) == 1);
        CHECK(distance(layout, {0, 0}, {1, 1}) == 2);
        CHECK(distance(layout, {1, 2}, {3, 3}) == 3);
        CHECK(distance(layout, {0, 0}, {4, 4}) == 8);
        CHECK(distance(layout, {4, 4}, {0, 0}) == std::numeric_limits<uint32_t>::max());

        // ignore z-axis
        CHECK(distance(layout, {0, 0, 1}, {8, 9, 0}) == 17);
        CHECK(distance(layout, {0, 0, 1}, {8, 9, 1}) == 17);

        // negative coordinates
        CHECK(distance(layout, {0, 0}, {-1, -1}) == std::numeric_limits<uint32_t>::max());
        CHECK(distance(layout, {-2, -8}, {-6, -4}) == std::numeric_limits<uint32_t>::max());
        CHECK(distance(layout, {-6, -4}, {-2, -8}) == std::numeric_limits<uint32_t>::max());
        CHECK(distance(layout, {-4, -3}, {1, -1}) == 7);
    }
}

TEST_CASE("A* distance", "[distance]")
{
    SECTION("Unsigned Cartesian layout")
    {
        using lyt = cartesian_layout<offset::ucoord_t>;

        SECTION("coordinate path distance")
        {
            const lyt layout{{9, 4, 1}};

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
            using clk_lyt = clocked_layout<lyt>;

            SECTION("2DDWave")
            {
                const clk_lyt layout{{9, 4, 1}, twoddwave_clocking<clk_lyt>()};

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

TEST_CASE("a_star distance functor", "[distance]")
{
    SECTION("Unsigned Cartesian layout")
    {
        using lyt = cartesian_layout<offset::ucoord_t>;

        SECTION("coordinate path distance")
        {
            const lyt layout{{9, 4, 1}};

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
            using clk_lyt = clocked_layout<lyt>;

            SECTION("2DDWave")
            {
                const clk_lyt layout{{9, 4, 1}, twoddwave_clocking<clk_lyt>()};

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
