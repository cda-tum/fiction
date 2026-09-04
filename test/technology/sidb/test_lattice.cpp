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
 * @brief Tests for `fiction/technology/sidb/lattice.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/technology/sidb/lattice.hpp>

#include <fmt/format.h>

#include <algorithm>
#include <cstdint>
#include <limits>
#include <random>
#include <stdexcept>
#include <unordered_set>
#include <vector>

using namespace fiction;
using namespace fiction::sidb;

TEST_CASE("Lattice sites", "[lattice]")
{
    SECTION("construction")
    {
        constexpr lattice_site origin{};
        CHECK(origin.x == 0);
        CHECK(origin.y == 0);
        CHECK(origin.z == 0);

        constexpr lattice_site two_arg{3, 4};
        CHECK(two_arg.x == 3);
        CHECK(two_arg.y == 4);
        CHECK(two_arg.z == 0);

        constexpr lattice_site three_arg{-3, -4, 1};
        CHECK(three_arg.x == -3);
        CHECK(three_arg.y == -4);
        CHECK(three_arg.z == 1);

        const uint64_t     ux = 5;
        const int64_t      iy = -7;
        const lattice_site mixed{ux, iy, 1u};
        CHECK(mixed == lattice_site{5, -7, 1});
    }
    SECTION("construction rejects unrepresentable coordinates")
    {
        constexpr int64_t      min_coordinate = std::numeric_limits<int32_t>::min();
        constexpr int64_t      max_coordinate = std::numeric_limits<int32_t>::max();
        constexpr lattice_site boundary{min_coordinate, max_coordinate, 1};
        CHECK(boundary.x == min_coordinate);
        CHECK(boundary.y == max_coordinate);
        CHECK(lattice_site{false, char{1}} == lattice_site{0, 1});

        for (const auto coordinate : {min_coordinate - 1, max_coordinate + 1})
        {
            CHECK_THROWS_AS((lattice_site{coordinate, 0}), std::out_of_range);
            CHECK_THROWS_AS((lattice_site{0, coordinate}), std::out_of_range);
            CHECK_THROWS_AS((lattice_site{coordinate, 0, 1}), std::out_of_range);
            CHECK_THROWS_AS((lattice_site{0, coordinate, 1}), std::out_of_range);
        }
        constexpr auto max_unsigned = std::numeric_limits<uint64_t>::max();
        CHECK_THROWS_AS((lattice_site{max_unsigned, 0}), std::out_of_range);
        CHECK_THROWS_AS((lattice_site{0, max_unsigned}), std::out_of_range);
        CHECK_THROWS_AS((lattice_site{max_unsigned, 0, 1}), std::out_of_range);
        CHECK_THROWS_AS((lattice_site{0, max_unsigned, 1}), std::out_of_range);
    }
    SECTION("raster order")
    {
        CHECK(lattice_site{5, 0, 0} < lattice_site{0, 1, 0});
        CHECK(lattice_site{5, 0, 0} < lattice_site{0, 0, 1});
        CHECK(lattice_site{0, 0, 1} < lattice_site{1, 0, 1});
        CHECK(lattice_site{0, 0, 0} == lattice_site{0, 0, 0});
        CHECK(lattice_site{0, 0, 0} != lattice_site{0, 0, 1});

        std::vector<lattice_site> sites{{1, 0, 1}, {0, 1, 0}, {1, 0, 0}, {0, 0, 0}};
        std::ranges::sort(sites);
        CHECK(sites == std::vector<lattice_site>{{0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 1, 0}});
    }
    SECTION("arithmetic carries the basis index")
    {
        CHECK(lattice_site{1, 1, 0} + lattice_site{2, 3, 1} == lattice_site{3, 4, 1});
        CHECK(lattice_site{1, 1, 1} + lattice_site{2, 3, 1} == lattice_site{3, 5, 0});
        CHECK(lattice_site{3, 4, 1} - lattice_site{2, 3, 1} == lattice_site{1, 1, 0});
        CHECK(lattice_site{3, 4, 0} - lattice_site{2, 3, 1} == lattice_site{1, 0, 1});
        CHECK(lattice_site{0, 0, 0} - lattice_site{2, 1} == lattice_site{-2, -1, 0});
    }
    SECTION("arithmetic rejects unrepresentable sites")
    {
        constexpr auto min_coordinate = std::numeric_limits<int32_t>::min();
        constexpr auto max_coordinate = std::numeric_limits<int32_t>::max();
        CHECK_THROWS_AS((lattice_site{max_coordinate, 0} + lattice_site{1, 0}), std::out_of_range);
        CHECK_THROWS_AS((lattice_site{min_coordinate, 0} + lattice_site{-1, 0}), std::out_of_range);
        CHECK_THROWS_AS((lattice_site{0, max_coordinate} + lattice_site{0, 1}), std::out_of_range);
        CHECK_THROWS_AS((lattice_site{0, min_coordinate} + lattice_site{0, -1}), std::out_of_range);
        CHECK_THROWS_AS((lattice_site{0, max_coordinate, 1} + lattice_site{0, 0, 1}), std::out_of_range);
        CHECK_THROWS_AS((lattice_site{max_coordinate, 0} - lattice_site{-1, 0}), std::out_of_range);
        CHECK_THROWS_AS((lattice_site{min_coordinate, 0} - lattice_site{1, 0}), std::out_of_range);
        CHECK_THROWS_AS((lattice_site{0, max_coordinate} - lattice_site{0, -1}), std::out_of_range);
        CHECK_THROWS_AS((lattice_site{0, min_coordinate} - lattice_site{0, 1}), std::out_of_range);
        CHECK_THROWS_AS((lattice_site{0, min_coordinate, 0} - lattice_site{0, 0, 1}), std::out_of_range);
        CHECK(lattice_site{0, min_coordinate, 1} + lattice_site{0, -1, 1} == lattice_site{0, min_coordinate, 0});
        CHECK(lattice_site{0, max_coordinate, 0} - lattice_site{0, -1, 1} == lattice_site{0, max_coordinate, 1});
    }
    SECTION("rows")
    {
        CHECK(row_of({4, 0, 0}) == 0);
        CHECK(row_of({4, 0, 1}) == 1);
        CHECK(row_of({4, 3, 0}) == 6);
        CHECK(row_of({4, 3, 1}) == 7);
        CHECK(row_of({4, -1, 1}) == -1);
        CHECK(row_of({4, -1, 0}) == -2);

        for (int32_t row = -7; row <= 7; ++row)
        {
            const auto s = site_at_row(2, row);
            CHECK(s.x == 2);
            CHECK(s.z <= 1);
            CHECK(row_of(s) == row);
        }

        CHECK(site_at_row(0, -1) == lattice_site{0, -1, 1});
        CHECK(site_at_row(0, -2) == lattice_site{0, -1, 0});

        constexpr auto min_row = std::numeric_limits<int32_t>::min();
        CHECK(site_at_row(0, min_row) == lattice_site{0, min_row / 2, 0});
        CHECK(site_at_row(0, min_row + 1) == lattice_site{0, min_row / 2, 1});

        constexpr auto max_coordinate = std::numeric_limits<int32_t>::max();
        CHECK(row_of({0, min_row, 0}) == int64_t{2} * min_row);
        CHECK(row_of({0, max_coordinate, 1}) == (int64_t{2} * max_coordinate) + 1);
        CHECK(site_at_row(0, row_of({0, min_row, 0})) == lattice_site{0, min_row, 0});
        CHECK(site_at_row(0, row_of({0, max_coordinate, 1})) == lattice_site{0, max_coordinate, 1});
        CHECK_THROWS_AS(site_at_row(0, row_of({0, min_row, 0}) - 1), std::out_of_range);
        CHECK_THROWS_AS(site_at_row(0, row_of({0, max_coordinate, 1}) + 1), std::out_of_range);
    }
    SECTION("string and format")
    {
        CHECK(lattice_site{1, -2, 1}.str() == "(1,-2,1)");
        CHECK(fmt::format("{}", lattice_site{1, -2, 1}) == "(1,-2,1)");
    }
    SECTION("hash")
    {
        const std::unordered_set<lattice_site> set{{0, 0, 0}, {0, 0, 1}, {0, 0, 0}, {-1, 0, 0}};
        CHECK(set.size() == 3);
    }
}

TEST_CASE("Sites in an area", "[lattice]")
{
    SECTION("raster order, both corners included")
    {
        const auto sites = sites_in_area({0, 0, 0}, {1, 1, 0});
        CHECK(sites == std::vector<lattice_site>{{0, 0, 0}, {1, 0, 0}, {0, 0, 1}, {1, 0, 1}, {0, 1, 0}, {1, 1, 0}});
    }
    SECTION("corners in any order")
    {
        CHECK(sites_in_area({1, 1, 0}, {0, 0, 0}) == sites_in_area({0, 0, 0}, {1, 1, 0}));
    }
    SECTION("single site")
    {
        CHECK(sites_in_area({2, 3, 1}, {2, 3, 1}) == std::vector<lattice_site>{{2, 3, 1}});
    }
    SECTION("negative sites")
    {
        const auto sites = sites_in_area({-1, -1, 1}, {0, 0, 0});
        CHECK(sites == std::vector<lattice_site>{{-1, -1, 1}, {0, -1, 1}, {-1, 0, 0}, {0, 0, 0}});
    }
    SECTION("random site stays inside")
    {
        // NOLINTNEXTLINE(cert-msc32-c, cert-msc51-cpp): a fixed seed keeps the random-site test reproducible.
        std::mt19937_64 rng{42};

        const lattice_site nw{-2, -1, 1};
        const lattice_site se{3, 2, 0};

        for (auto i = 0; i < 200; ++i)
        {
            const auto s = random_site_in_area(nw, se, rng);
            CHECK(s.x >= nw.x);
            CHECK(s.x <= se.x);
            CHECK(row_of(s) >= row_of(nw));
            CHECK(row_of(s) <= row_of(se));
        }
    }
    SECTION("coordinate limits")
    {
        constexpr auto     min_coordinate = std::numeric_limits<int32_t>::min();
        constexpr auto     max_coordinate = std::numeric_limits<int32_t>::max();
        const lattice_site first{max_coordinate - 1, max_coordinate, 0};
        const lattice_site last{max_coordinate, max_coordinate, 1};
        CHECK(sites_in_area(first, last) == std::vector<lattice_site>{first,
                                                                      {max_coordinate, max_coordinate, 0},
                                                                      {max_coordinate - 1, max_coordinate, 1},
                                                                      last});
        CHECK_THROWS_AS(sites_in_area({min_coordinate, min_coordinate, 0}, last), std::length_error);

        // NOLINTNEXTLINE(cert-msc32-c, cert-msc51-cpp): a fixed seed keeps the random-site test reproducible.
        std::mt19937_64 rng{42};
        CHECK(random_site_in_area(last, last, rng) == last);
    }
}

TEST_CASE("H-Si(100)-2x1 positions", "[lattice]")
{
    using namespace Catch::Matchers;

    const auto lat = lattice::si_100_2x1();

    CHECK(lat.name == "Si(100) 2x1");

    const auto check = [&lat](const lattice_site& s, const double x, const double y)
    {
        const auto [px, py] = lat.nm_position(s);
        CHECK_THAT(px, WithinAbs(x, 1E-5));
        CHECK_THAT(py, WithinAbs(y, 1E-5));
    };

    check({0, 0, 0}, 0.0, 0.0);
    check({1, 0, 0}, 0.384, 0.0);
    check({0, 1, 0}, 0.0, 0.768);
    check({0, 8, 1}, 0.0, 6.369);
    check({1, 1, 1}, 0.384, 0.993);
    check({1, 10, 1}, 0.384, 7.905);
    check({-1, 0, 0}, -0.384, 0.0);
    check({0, -1, 0}, 0.0, -0.768);
    check({-5, -5, 0}, -1.92, -3.84);

    CHECK_THAT(lat.nm_distance({0, 0, 0}, {0, 0, 0}), WithinAbs(0.0, 1E-9));
    CHECK_THAT(lat.nm_distance({0, 0, 0}, {1, 0, 0}), WithinAbs(0.384, 1E-5));
    CHECK_THAT(lat.nm_distance({0, 0, 0}, {0, 0, 1}), WithinAbs(0.225, 1E-5));
    CHECK_THAT(lat.nm_distance({0, 0, 0}, {1, 1, 1}), WithinAbs(1.06467, 1E-5));
    CHECK_THAT(lat.nm_distance({1, 1, 1}, {0, 0, 0}), WithinAbs(1.06467, 1E-5));
}

TEST_CASE("H-Si(111)-1x1 positions", "[lattice]")
{
    using namespace Catch::Matchers;

    const auto lat = lattice::si_111_1x1();

    CHECK(lat.name == "Si(111) 1x1");

    const auto check = [&lat](const lattice_site& s, const double x, const double y)
    {
        const auto [px, py] = lat.nm_position(s);
        CHECK_THAT(px, WithinAbs(x, 1E-5));
        CHECK_THAT(py, WithinAbs(y, 1E-5));
    };

    check({0, 0, 0}, 0.0, 0.0);
    check({0, 0, 1}, 0.33255, 0.192);
    check({0, -1, 0}, 0.0, -0.384);
    check({-5, -5, 0}, -3.325, -1.92);
    check({-1, 0, 0}, -0.665, 0.0);
    check({1, 1, 1}, 0.99755, 0.576);

    // nearest neighbors on the hexagonal surface are 3.84 Å apart
    CHECK_THAT(lat.nm_distance({0, 0, 0}, {0, 0, 1}), WithinAbs(0.384, 1E-5));
    CHECK_THAT(lat.nm_distance({0, 0, 0}, {0, 1, 0}), WithinAbs(0.384, 1E-5));
}

TEST_CASE("Custom lattice", "[lattice]")
{
    using namespace Catch::Matchers;

    const lattice square{.name = "square", .a1 = {5.0, 0.0}, .a2 = {0.0, 5.0}, .basis = {{{0.0, 0.0}, {2.5, 2.5}}}};

    const auto [x, y] = square.nm_position({2, 3, 1});
    CHECK_THAT(x, WithinAbs(1.25, 1E-9));
    CHECK_THAT(y, WithinAbs(1.75, 1E-9));

    CHECK(square != lattice::si_100_2x1());
    CHECK(lattice::si_100_2x1() == lattice::si_100_2x1());
}
