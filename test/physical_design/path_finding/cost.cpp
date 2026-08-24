//
// Created by marcel on 27.05.22.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/physical_design/path_finding/cost.hpp>

#include <cmath>

using namespace fiction;

TEST_CASE("Unit cost", "[cost]")
{
    SECTION("Unsigned Cartesian layout")
    {
        using layout = layouts::cartesian_layout<layouts::coords::offset>;

        CHECK(physical_design::path_finding::unit_cost<layout>({0, 0}, {0, 0}) == 1);
        CHECK(physical_design::path_finding::unit_cost<layout>({1, 1}, {1, 1}) == 1);
        CHECK(physical_design::path_finding::unit_cost<layout>({0, 0}, {0, 1}) == 1);
        CHECK(physical_design::path_finding::unit_cost<layout>({0, 0}, {1, 1}) == 1);
        CHECK(physical_design::path_finding::unit_cost<layout>({1, 2}, {3, 3}) == 1);
        CHECK(physical_design::path_finding::unit_cost<layout>({0, 0}, {4, 4}) == 1);
        CHECK(physical_design::path_finding::unit_cost<layout>({4, 4}, {0, 0}) == 1);

        // ignore z-axis
        CHECK(physical_design::path_finding::unit_cost<layout>({0, 0, 1}, {8, 9, 0}) == 1);
        CHECK(physical_design::path_finding::unit_cost<layout>({0, 0, 1}, {8, 9, 1}) == 1);
    }
    SECTION("Signed Cartesian layout")
    {
        using layout = layouts::cartesian_layout<layouts::coords::cube>;

        CHECK(physical_design::path_finding::unit_cost<layout>({0, 0}, {0, 0}) == 1);
        CHECK(physical_design::path_finding::unit_cost<layout>({1, 1}, {1, 1}) == 1);
        CHECK(physical_design::path_finding::unit_cost<layout>({0, 0}, {0, 1}) == 1);
        CHECK(physical_design::path_finding::unit_cost<layout>({0, 0}, {1, 1}) == 1);
        CHECK(physical_design::path_finding::unit_cost<layout>({1, 2}, {3, 3}) == 1);
        CHECK(physical_design::path_finding::unit_cost<layout>({0, 0}, {4, 4}) == 1);
        CHECK(physical_design::path_finding::unit_cost<layout>({4, 4}, {0, 0}) == 1);

        // ignore z-axis
        CHECK(physical_design::path_finding::unit_cost<layout>({0, 0, 1}, {8, 9, 0}) == 1);
        CHECK(physical_design::path_finding::unit_cost<layout>({0, 0, 1}, {8, 9, 1}) == 1);

        // negative coordinates
        CHECK(physical_design::path_finding::unit_cost<layout>({0, 0}, {-1, -1}) == 1);
        CHECK(physical_design::path_finding::unit_cost<layout>({-4, -3}, {1, -1}) == 1);
        CHECK(physical_design::path_finding::unit_cost<layout>({-2, -8}, {-6, -4}) == 1);
    }
}

TEST_CASE("Unit cost functor", "[cost]")
{
    SECTION("Unsigned Cartesian layout")
    {
        using layout = layouts::cartesian_layout<layouts::coords::offset>;

        physical_design::path_finding::unit_cost_functor<layout> cost{};

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
        using layout = layouts::cartesian_layout<layouts::coords::cube>;

        physical_design::path_finding::unit_cost_functor<layout> cost{};

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
        using layout = layouts::cartesian_layout<layouts::coords::offset>;

        auto r = physical_design::path_finding::random_cost<layout>({0, 0}, {0, 0});
        CHECK((r >= 0 && r <= 1));
        r = physical_design::path_finding::random_cost<layout>({1, 1}, {1, 1});
        CHECK((r >= 0 && r <= 1));
        r = physical_design::path_finding::random_cost<layout>({0, 0}, {0, 1});
        CHECK((r >= 0 && r <= 1));
        r = physical_design::path_finding::random_cost<layout>({0, 0}, {1, 1});
        CHECK((r >= 0 && r <= 1));
        r = physical_design::path_finding::random_cost<layout>({9, 1}, {6, 2});
        CHECK((r >= 0 && r <= 1));
        r = physical_design::path_finding::random_cost<layout>({6, 2}, {0, 4});
        CHECK((r >= 0 && r <= 1));
        r = physical_design::path_finding::random_cost<layout>({0, 4}, {9, 1});
        CHECK((r >= 0 && r <= 1));

        // ignore z-axis
        r = physical_design::path_finding::random_cost<layout>({6, 2, 1}, {0, 4, 0});
        CHECK((r >= 0 && r <= 1));
        r = physical_design::path_finding::random_cost<layout>({6, 2, 0}, {0, 4, 1});
        CHECK((r >= 0 && r <= 1));
        r = physical_design::path_finding::random_cost<layout>({0, 4, 1}, {9, 1, 1});
        CHECK((r >= 0 && r <= 1));
    }
    SECTION("Signed Cartesian layout")
    {
        using layout = layouts::cartesian_layout<layouts::coords::cube>;

        auto r = physical_design::path_finding::random_cost<layout>({0, 0}, {0, 0});
        CHECK((r >= 0 && r <= 1));
        r = physical_design::path_finding::random_cost<layout>({1, 1}, {1, 1});
        CHECK((r >= 0 && r <= 1));
        r = physical_design::path_finding::random_cost<layout>({0, 0}, {0, 1});
        CHECK((r >= 0 && r <= 1));
        r = physical_design::path_finding::random_cost<layout>({0, 0}, {1, 1});
        CHECK((r >= 0 && r <= 1));
        r = physical_design::path_finding::random_cost<layout>({9, 1}, {6, 2});
        CHECK((r >= 0 && r <= 1));
        r = physical_design::path_finding::random_cost<layout>({6, 2}, {0, 4});
        CHECK((r >= 0 && r <= 1));
        r = physical_design::path_finding::random_cost<layout>({0, 4}, {9, 1});
        CHECK((r >= 0 && r <= 1));

        // ignore z-axis
        r = physical_design::path_finding::random_cost<layout>({6, 2, 1}, {0, 4, 0});
        CHECK((r >= 0 && r <= 1));
        r = physical_design::path_finding::random_cost<layout>({6, 2, 0}, {0, 4, 1});
        CHECK((r >= 0 && r <= 1));
        r = physical_design::path_finding::random_cost<layout>({0, 4, 1}, {9, 1, 1});
        CHECK((r >= 0 && r <= 1));

        // negative coordinates
        r = physical_design::path_finding::random_cost<layout>({0, 0}, {-1, -1});
        CHECK((r >= 0 && r <= 1));
        r = physical_design::path_finding::random_cost<layout>({-4, -3}, {1, -1});
        CHECK((r >= 0 && r <= 1));
        r = physical_design::path_finding::random_cost<layout>({-2, -8}, {-6, -4});
        CHECK((r >= 0 && r <= 1));
    }
}

TEST_CASE("Random cost functor", "[distance]")
{
    SECTION("Unsigned Cartesian layout")
    {
        using layout = layouts::cartesian_layout<layouts::coords::offset>;

        physical_design::path_finding::random_cost_functor<layout> cost{};

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
        using layout = layouts::cartesian_layout<layouts::coords::cube>;

        physical_design::path_finding::random_cost_functor<layout> cost{};

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
