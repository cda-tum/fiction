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
        using layout = cartesian_layout<offset::ucoord_t>;

        CHECK(manhattan_distance<layout>({0, 0}, {0, 0}) == 0);
        CHECK(manhattan_distance<layout>({1, 1}, {1, 1}) == 0);
        CHECK(manhattan_distance<layout>({0, 0}, {0, 1}) == 1);
        CHECK(manhattan_distance<layout>({0, 0}, {1, 1}) == 2);
        CHECK(manhattan_distance<layout>({1, 2}, {3, 3}) == 3);
        CHECK(manhattan_distance<layout>({0, 0}, {4, 4}) == 8);
        CHECK(manhattan_distance<layout>({4, 4}, {0, 0}) == 8);

        // ignore z-axis
        CHECK(manhattan_distance<layout>({0, 0, 1}, {8, 9, 0}) == 17);
        CHECK(manhattan_distance<layout>({0, 0, 1}, {8, 9, 1}) == 17);
    }
    SECTION("Signed Cartesian layout")
    {
        using layout = cartesian_layout<cube::coord_t>;

        CHECK(manhattan_distance<layout>({0, 0}, {0, 0}) == 0);
        CHECK(manhattan_distance<layout>({1, 1}, {1, 1}) == 0);
        CHECK(manhattan_distance<layout>({0, 0}, {0, 1}) == 1);
        CHECK(manhattan_distance<layout>({0, 0}, {1, 1}) == 2);
        CHECK(manhattan_distance<layout>({1, 2}, {3, 3}) == 3);
        CHECK(manhattan_distance<layout>({0, 0}, {4, 4}) == 8);
        CHECK(manhattan_distance<layout>({4, 4}, {0, 0}) == 8);

        // ignore z-axis
        CHECK(manhattan_distance<layout>({0, 0, 1}, {8, 9, 0}) == 17);
        CHECK(manhattan_distance<layout>({0, 0, 1}, {8, 9, 1}) == 17);

        // negative coordinates
        CHECK(manhattan_distance<layout>({0, 0}, {-1, -1}) == 2);
        CHECK(manhattan_distance<layout>({-4, -3}, {1, -1}) == 7);
        CHECK(manhattan_distance<layout>({-2, -8}, {-6, -4}) == 8);
    }
}

TEST_CASE("Euclidean distance", "[distance]")
{
    SECTION("Unsigned Cartesian layout")
    {
        using layout = cartesian_layout<offset::ucoord_t>;

        CHECK(euclidean_distance<layout>({0, 0}, {0, 0}) == 0.0);
        CHECK(euclidean_distance<layout>({1, 1}, {1, 1}) == 0.0);
        CHECK(euclidean_distance<layout>({0, 0}, {0, 1}) == 1.0);
        CHECK(euclidean_distance<layout>({0, 0}, {1, 1}) == std::sqrt(2));
        CHECK(euclidean_distance<layout>({9, 1}, {6, 2}) == std::sqrt(10));
        CHECK(euclidean_distance<layout>({6, 2}, {0, 4}) == std::sqrt(40));
        CHECK(euclidean_distance<layout>({0, 4}, {9, 1}) == std::sqrt(90));

        // ignore z-axis
        CHECK(euclidean_distance<layout>({6, 2, 1}, {0, 4, 0}) == std::sqrt(40));
        CHECK(euclidean_distance<layout>({6, 2, 0}, {0, 4, 1}) == std::sqrt(40));
        CHECK(euclidean_distance<layout>({0, 4, 1}, {9, 1, 1}) == std::sqrt(90));
    }
    SECTION("Signed Cartesian layout")
    {
        using layout = cartesian_layout<cube::coord_t>;

        CHECK(euclidean_distance<layout>({0, 0}, {0, 0}) == 0.0);
        CHECK(euclidean_distance<layout>({1, 1}, {1, 1}) == 0.0);
        CHECK(euclidean_distance<layout>({0, 0}, {0, 1}) == 1.0);
        CHECK(euclidean_distance<layout>({0, 0}, {1, 1}) == std::sqrt(2));
        CHECK(euclidean_distance<layout>({9, 1}, {6, 2}) == std::sqrt(10));
        CHECK(euclidean_distance<layout>({6, 2}, {0, 4}) == std::sqrt(40));
        CHECK(euclidean_distance<layout>({0, 4}, {9, 1}) == std::sqrt(90));

        // ignore z-axis
        CHECK(euclidean_distance<layout>({6, 2, 1}, {0, 4, 0}) == std::sqrt(40));
        CHECK(euclidean_distance<layout>({6, 2, 0}, {0, 4, 1}) == std::sqrt(40));
        CHECK(euclidean_distance<layout>({0, 4, 1}, {9, 1, 1}) == std::sqrt(90));

        // negative coordinates
        CHECK(euclidean_distance<layout>({0, 0}, {-1, -1}) == std::sqrt(2));
        CHECK(euclidean_distance<layout>({-4, -3}, {1, -1}) == std::sqrt(29));
        CHECK(euclidean_distance<layout>({-2, -8}, {-6, -4}) == std::sqrt(32));
    }
}
