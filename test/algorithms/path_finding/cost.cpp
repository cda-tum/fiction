//
// Created by marcel on 27.05.22.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/path_finding/cost.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/coordinates.hpp>

#include <cmath>

using namespace fiction;

TEST_CASE("Unit cost", "[cost]")
{
    SECTION("Unsigned Cartesian layout")
    {
        using layout = cartesian_layout<offset::ucoord_t>;

        CHECK(unit_cost<layout>({0, 0}, {0, 0}) == 1);
        CHECK(unit_cost<layout>({1, 1}, {1, 1}) == 1);
        CHECK(unit_cost<layout>({0, 0}, {0, 1}) == 1);
        CHECK(unit_cost<layout>({0, 0}, {1, 1}) == 1);
        CHECK(unit_cost<layout>({1, 2}, {3, 3}) == 1);
        CHECK(unit_cost<layout>({0, 0}, {4, 4}) == 1);
        CHECK(unit_cost<layout>({4, 4}, {0, 0}) == 1);

        // ignore z-axis
        CHECK(unit_cost<layout>({0, 0, 1}, {8, 9, 0}) == 1);
        CHECK(unit_cost<layout>({0, 0, 1}, {8, 9, 1}) == 1);
    }
    SECTION("Signed Cartesian layout")
    {
        using layout = cartesian_layout<cube::coord_t>;

        CHECK(unit_cost<layout>({0, 0}, {0, 0}) == 1);
        CHECK(unit_cost<layout>({1, 1}, {1, 1}) == 1);
        CHECK(unit_cost<layout>({0, 0}, {0, 1}) == 1);
        CHECK(unit_cost<layout>({0, 0}, {1, 1}) == 1);
        CHECK(unit_cost<layout>({1, 2}, {3, 3}) == 1);
        CHECK(unit_cost<layout>({0, 0}, {4, 4}) == 1);
        CHECK(unit_cost<layout>({4, 4}, {0, 0}) == 1);

        // ignore z-axis
        CHECK(unit_cost<layout>({0, 0, 1}, {8, 9, 0}) == 1);
        CHECK(unit_cost<layout>({0, 0, 1}, {8, 9, 1}) == 1);

        // negative coordinates
        CHECK(unit_cost<layout>({0, 0}, {-1, -1}) == 1);
        CHECK(unit_cost<layout>({-4, -3}, {1, -1}) == 1);
        CHECK(unit_cost<layout>({-2, -8}, {-6, -4}) == 1);
    }
}

TEST_CASE("Unit cost functor", "[cost]")
{
    SECTION("Unsigned Cartesian layout")
    {
        using layout = cartesian_layout<offset::ucoord_t>;

        unit_cost_functor<layout> cost{};

        CHECK(cost({0, 0}, {0, 0}) == 1);
        CHECK(cost({1, 1}, {1, 1}) == 1);
        CHECK(cost({0, 0}, {0, 1}) == 1);
        CHECK(cost({0, 0}, {1, 1}) == 1);
        CHECK(cost({1, 2}, {3, 3}) == 1);
        CHECK(cost({0, 0}, {4, 4}) == 1);
        CHECK(cost({4, 4}, {0, 0}) == 1);

        // ignore z-axis
        CHECK(cost({0, 0, 1}, {8, 9, 0}) == 1);
        CHECK(cost({0, 0, 1}, {8, 9, 1}) == 1);
    }
    SECTION("Signed Cartesian layout")
    {
        using layout = cartesian_layout<cube::coord_t>;

        unit_cost_functor<layout> cost{};

        CHECK(cost({0, 0}, {0, 0}) == 1);
        CHECK(cost({1, 1}, {1, 1}) == 1);
        CHECK(cost({0, 0}, {0, 1}) == 1);
        CHECK(cost({0, 0}, {1, 1}) == 1);
        CHECK(cost({1, 2}, {3, 3}) == 1);
        CHECK(cost({0, 0}, {4, 4}) == 1);
        CHECK(cost({4, 4}, {0, 0}) == 1);

        // ignore z-axis
        CHECK(cost({0, 0, 1}, {8, 9, 0}) == 1);
        CHECK(cost({0, 0, 1}, {8, 9, 1}) == 1);

        // negative coordinates
        CHECK(cost({0, 0}, {-1, -1}) == 1);
        CHECK(cost({-4, -3}, {1, -1}) == 1);
        CHECK(cost({-2, -8}, {-6, -4}) == 1);
    }
}

TEST_CASE("Random cost", "[distance]")
{
    SECTION("Unsigned Cartesian layout")
    {
        using layout = cartesian_layout<offset::ucoord_t>;

        auto r = random_cost<layout>({0, 0}, {0, 0});
        CHECK((r >= 0 && r <= 1));
        r = random_cost<layout>({1, 1}, {1, 1});
        CHECK((r >= 0 && r <= 1));
        r = random_cost<layout>({0, 0}, {0, 1});
        CHECK((r >= 0 && r <= 1));
        r = random_cost<layout>({0, 0}, {1, 1});
        CHECK((r >= 0 && r <= 1));
        r = random_cost<layout>({9, 1}, {6, 2});
        CHECK((r >= 0 && r <= 1));
        r = random_cost<layout>({6, 2}, {0, 4});
        CHECK((r >= 0 && r <= 1));
        r = random_cost<layout>({0, 4}, {9, 1});
        CHECK((r >= 0 && r <= 1));

        // ignore z-axis
        r = random_cost<layout>({6, 2, 1}, {0, 4, 0});
        CHECK((r >= 0 && r <= 1));
        r = random_cost<layout>({6, 2, 0}, {0, 4, 1});
        CHECK((r >= 0 && r <= 1));
        r = random_cost<layout>({0, 4, 1}, {9, 1, 1});
        CHECK((r >= 0 && r <= 1));
    }
    SECTION("Signed Cartesian layout")
    {
        using layout = cartesian_layout<cube::coord_t>;

        auto r = random_cost<layout>({0, 0}, {0, 0});
        CHECK((r >= 0 && r <= 1));
        r = random_cost<layout>({1, 1}, {1, 1});
        CHECK((r >= 0 && r <= 1));
        r = random_cost<layout>({0, 0}, {0, 1});
        CHECK((r >= 0 && r <= 1));
        r = random_cost<layout>({0, 0}, {1, 1});
        CHECK((r >= 0 && r <= 1));
        r = random_cost<layout>({9, 1}, {6, 2});
        CHECK((r >= 0 && r <= 1));
        r = random_cost<layout>({6, 2}, {0, 4});
        CHECK((r >= 0 && r <= 1));
        r = random_cost<layout>({0, 4}, {9, 1});
        CHECK((r >= 0 && r <= 1));

        // ignore z-axis
        r = random_cost<layout>({6, 2, 1}, {0, 4, 0});
        CHECK((r >= 0 && r <= 1));
        r = random_cost<layout>({6, 2, 0}, {0, 4, 1});
        CHECK((r >= 0 && r <= 1));
        r = random_cost<layout>({0, 4, 1}, {9, 1, 1});
        CHECK((r >= 0 && r <= 1));

        // negative coordinates
        r = random_cost<layout>({0, 0}, {-1, -1});
        CHECK((r >= 0 && r <= 1));
        r = random_cost<layout>({-4, -3}, {1, -1});
        CHECK((r >= 0 && r <= 1));
        r = random_cost<layout>({-2, -8}, {-6, -4});
        CHECK((r >= 0 && r <= 1));
    }
}

TEST_CASE("Random cost functor", "[distance]")
{
    SECTION("Unsigned Cartesian layout")
    {
        using layout = cartesian_layout<offset::ucoord_t>;

        random_cost_functor<layout> cost{};

        auto r = cost({0, 0}, {0, 0});
        CHECK((r >= 0 && r <= 1));
        r = cost({1, 1}, {1, 1});
        CHECK((r >= 0 && r <= 1));
        r = cost({0, 0}, {0, 1});
        CHECK((r >= 0 && r <= 1));
        r = cost({0, 0}, {1, 1});
        CHECK((r >= 0 && r <= 1));
        r = cost({9, 1}, {6, 2});
        CHECK((r >= 0 && r <= 1));
        r = cost({6, 2}, {0, 4});
        CHECK((r >= 0 && r <= 1));
        r = cost({0, 4}, {9, 1});
        CHECK((r >= 0 && r <= 1));

        // ignore z-axis
        r = cost({6, 2, 1}, {0, 4, 0});
        CHECK((r >= 0 && r <= 1));
        r = cost({6, 2, 0}, {0, 4, 1});
        CHECK((r >= 0 && r <= 1));
        r = cost({0, 4, 1}, {9, 1, 1});
        CHECK((r >= 0 && r <= 1));
    }
    SECTION("Signed Cartesian layout")
    {
        using layout = cartesian_layout<cube::coord_t>;

        random_cost_functor<layout> cost{};

        auto r = cost({0, 0}, {0, 0});
        CHECK((r >= 0 && r <= 1));
        r = cost({1, 1}, {1, 1});
        CHECK((r >= 0 && r <= 1));
        r = cost({0, 0}, {0, 1});
        CHECK((r >= 0 && r <= 1));
        r = cost({0, 0}, {1, 1});
        CHECK((r >= 0 && r <= 1));
        r = cost({9, 1}, {6, 2});
        CHECK((r >= 0 && r <= 1));
        r = cost({6, 2}, {0, 4});
        CHECK((r >= 0 && r <= 1));
        r = cost({0, 4}, {9, 1});
        CHECK((r >= 0 && r <= 1));

        // ignore z-axis
        r = cost({6, 2, 1}, {0, 4, 0});
        CHECK((r >= 0 && r <= 1));
        r = cost({6, 2, 0}, {0, 4, 1});
        CHECK((r >= 0 && r <= 1));
        r = cost({0, 4, 1}, {9, 1, 1});
        CHECK((r >= 0 && r <= 1));

        // negative coordinates
        r = cost({0, 0}, {-1, -1});
        CHECK((r >= 0 && r <= 1));
        r = cost({-4, -3}, {1, -1});
        CHECK((r >= 0 && r <= 1));
        r = cost({-2, -8}, {-6, -4});
        CHECK((r >= 0 && r <= 1));
    }
}
