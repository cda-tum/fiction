//
// Created by marcel on 17.02.22.
//

#include "catch.hpp"

#include <fiction/algorithms/path_finding/distance.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/coordinates.hpp>

#include <cmath>

using namespace fiction;

TEST_CASE("Manhattan distance", "[distance]")
{
    SECTION("Unsigned Cartesian layout")
    {
        using cart_lyt = cartesian_layout<offset::ucoord_t>;

        cart_lyt layout{};

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

        cart_lyt layout{};

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

        cart_lyt layout{};

        manhattan_distance_functor<cart_lyt> distance{};

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

        cart_lyt layout{};

        manhattan_distance_functor<cart_lyt> distance{};

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
    SECTION("Unsigned Cartesian layout")
    {
        using cart_lyt = cartesian_layout<offset::ucoord_t>;

        cart_lyt layout{};

        CHECK(euclidean_distance<cart_lyt>(layout, {0, 0}, {0, 0}) == 0.0);
        CHECK(euclidean_distance<cart_lyt>(layout, {1, 1}, {1, 1}) == 0.0);
        CHECK(euclidean_distance<cart_lyt>(layout, {0, 0}, {0, 1}) == 1.0);
        CHECK(euclidean_distance<cart_lyt>(layout, {0, 0}, {1, 1}) == std::sqrt(2));
        CHECK(euclidean_distance<cart_lyt>(layout, {9, 1}, {6, 2}) == std::sqrt(10));
        CHECK(euclidean_distance<cart_lyt>(layout, {6, 2}, {0, 4}) == std::sqrt(40));
        CHECK(euclidean_distance<cart_lyt>(layout, {0, 4}, {9, 1}) == std::sqrt(90));

        // ignore z-axis
        CHECK(euclidean_distance<cart_lyt>(layout, {6, 2, 1}, {0, 4, 0}) == std::sqrt(40));
        CHECK(euclidean_distance<cart_lyt>(layout, {6, 2, 0}, {0, 4, 1}) == std::sqrt(40));
        CHECK(euclidean_distance<cart_lyt>(layout, {0, 4, 1}, {9, 1, 1}) == std::sqrt(90));
    }
    SECTION("Signed Cartesian layout")
    {
        using cart_lyt = cartesian_layout<cube::coord_t>;

        cart_lyt layout{};

        CHECK(euclidean_distance<cart_lyt>(layout, {0, 0}, {0, 0}) == 0.0);
        CHECK(euclidean_distance<cart_lyt>(layout, {1, 1}, {1, 1}) == 0.0);
        CHECK(euclidean_distance<cart_lyt>(layout, {0, 0}, {0, 1}) == 1.0);
        CHECK(euclidean_distance<cart_lyt>(layout, {0, 0}, {1, 1}) == std::sqrt(2));
        CHECK(euclidean_distance<cart_lyt>(layout, {9, 1}, {6, 2}) == std::sqrt(10));
        CHECK(euclidean_distance<cart_lyt>(layout, {6, 2}, {0, 4}) == std::sqrt(40));
        CHECK(euclidean_distance<cart_lyt>(layout, {0, 4}, {9, 1}) == std::sqrt(90));

        // ignore z-axis
        CHECK(euclidean_distance<cart_lyt>(layout, {6, 2, 1}, {0, 4, 0}) == std::sqrt(40));
        CHECK(euclidean_distance<cart_lyt>(layout, {6, 2, 0}, {0, 4, 1}) == std::sqrt(40));
        CHECK(euclidean_distance<cart_lyt>(layout, {0, 4, 1}, {9, 1, 1}) == std::sqrt(90));

        // negative coordinates
        CHECK(euclidean_distance<cart_lyt>(layout, {0, 0}, {-1, -1}) == std::sqrt(2));
        CHECK(euclidean_distance<cart_lyt>(layout, {-4, -3}, {1, -1}) == std::sqrt(29));
        CHECK(euclidean_distance<cart_lyt>(layout, {-2, -8}, {-6, -4}) == std::sqrt(32));
    }
}

TEST_CASE("Euclidean distance functor", "[distance]")
{
    SECTION("Unsigned Cartesian layout")
    {
        using cart_lyt = cartesian_layout<offset::ucoord_t>;

        cart_lyt layout{};

        euclidean_distance_functor<cart_lyt> distance{};

        CHECK(distance(layout, {0, 0}, {0, 0}) == 0.0);
        CHECK(distance(layout, {1, 1}, {1, 1}) == 0.0);
        CHECK(distance(layout, {0, 0}, {0, 1}) == 1.0);
        CHECK(distance(layout, {0, 0}, {1, 1}) == std::sqrt(2));
        CHECK(distance(layout, {9, 1}, {6, 2}) == std::sqrt(10));
        CHECK(distance(layout, {6, 2}, {0, 4}) == std::sqrt(40));
        CHECK(distance(layout, {0, 4}, {9, 1}) == std::sqrt(90));

        // ignore z-axis
        CHECK(distance(layout, {6, 2, 1}, {0, 4, 0}) == std::sqrt(40));
        CHECK(distance(layout, {6, 2, 0}, {0, 4, 1}) == std::sqrt(40));
        CHECK(distance(layout, {0, 4, 1}, {9, 1, 1}) == std::sqrt(90));
    }
    SECTION("Signed Cartesian layout")
    {
        using cart_lyt = cartesian_layout<cube::coord_t>;

        cart_lyt layout{};

        euclidean_distance_functor<cart_lyt> distance{};

        CHECK(distance(layout, {0, 0}, {0, 0}) == 0.0);
        CHECK(distance(layout, {1, 1}, {1, 1}) == 0.0);
        CHECK(distance(layout, {0, 0}, {0, 1}) == 1.0);
        CHECK(distance(layout, {0, 0}, {1, 1}) == std::sqrt(2));
        CHECK(distance(layout, {9, 1}, {6, 2}) == std::sqrt(10));
        CHECK(distance(layout, {6, 2}, {0, 4}) == std::sqrt(40));
        CHECK(distance(layout, {0, 4}, {9, 1}) == std::sqrt(90));

        // ignore z-axis
        CHECK(distance(layout, {6, 2, 1}, {0, 4, 0}) == std::sqrt(40));
        CHECK(distance(layout, {6, 2, 0}, {0, 4, 1}) == std::sqrt(40));
        CHECK(distance(layout, {0, 4, 1}, {9, 1, 1}) == std::sqrt(90));

        // negative coordinates
        CHECK(distance(layout, {0, 0}, {-1, -1}) == std::sqrt(2));
        CHECK(distance(layout, {-4, -3}, {1, -1}) == std::sqrt(29));
        CHECK(distance(layout, {-2, -8}, {-6, -4}) == std::sqrt(32));
    }
}
