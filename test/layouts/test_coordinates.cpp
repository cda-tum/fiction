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
 * @brief Tests for `fiction/layouts/coordinates.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 * @author Willem Lambooy (wlambooy)
 */

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/traits.hpp>

#include <fmt/format.h>

#include <map>
#include <sstream>
#include <vector>

using namespace fiction;
using namespace fiction::layouts;

#pragma GCC diagnostic push
#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wuseless-cast"
#endif
#pragma GCC diagnostic ignored "-Wconversion"

TEST_CASE("Unsigned offset coordinates", "[coordinates]")
{
    using coordinate = coords::offset;

    auto td = coordinate{};
    CHECK(td.is_dead());

    auto t0 = coordinate{0, 0, 0};
    CHECK(!t0.is_dead());

    CHECK(t0 != td);

    auto t1 = coordinate{1, 2, 0};
    auto t2 = coordinate{1, 2};

    CHECK(t0 < t1);
    CHECK(t1 > t0);
    CHECK(t1 >= t0);
    CHECK(t0 <= t1);
    CHECK(t1 == t2);
    CHECK(t2 == t1);

    t1.z += uint64_t{4ul};

    CHECK(t1 == t2);

    t1.y += uint64_t{2147483648ul};

    CHECK(t1 == t2);

    t1.x += uint64_t{2147483648ul};

    CHECK(t1 == t2);

    t1.x++;

    CHECK(t1 != t2);
    CHECK(t1 > t2);
    CHECK(t1 >= t2);
    CHECK(t2 < t1);
    CHECK(t2 <= t1);

    auto t3 = coordinate{0, 0, 1};

    CHECK(t1 < t3);
    CHECK(t2 < t3);

    const std::map<uint64_t, coordinate> coordinate_repr{
        {0x8000000000000000, coordinate{}},        {0x0000000000000000, coordinate{0, 0, 0}},
        {0x4000000000000000, coordinate{0, 0, 1}}, {0x4000000080000001, coordinate{1, 1, 1}},
        {0x0000000000000002, coordinate{2, 0, 0}}, {0x3fffffffffffffff, coordinate{2147483647, 2147483647, 0}}};

    for (auto [repr, coord] : coordinate_repr)
    {
        CHECK(static_cast<coordinate>(repr) == coord);
        CHECK(repr == static_cast<uint64_t>(coord));
        CHECK(coordinate{repr} == coord);
        CHECK(coordinate{coord} == coord);
        CHECK(coordinate{static_cast<uint64_t>(coord)} == coord);
    }

    std::ostringstream os{};
    os << coordinate{3, 2, 1};
    CHECK(os.str() == "(3,2,1)");
}

TEST_CASE("Offset to cube coordinate conversion", "[coordinates]")
{
    auto t = coords::offset{};
    CHECK(t.is_dead());
    auto fiction_d = coords::to_cube(t);
    CHECK(fiction_d.is_dead());

    auto t0        = coords::offset{0, 0, 0};
    auto fiction_0 = coords::to_cube(t0);
    CHECK(!fiction_0.is_dead());

    auto t1      = coords::offset{1, 3, 1};
    auto t1_cube = coords::to_cube(t1);
    CHECK(t1_cube.x == t1.x);
    CHECK(t1_cube.y == 3);

    auto t2      = coords::offset{1, 2};
    auto t2_cube = coords::to_cube(t2);
    CHECK(t2_cube.x == t2.x);
    CHECK(t2_cube.y == 2);
    CHECK(t2_cube.z == 0);
}

TEMPLATE_TEST_CASE("Coordinate iteration", "[coordinates]", coords::offset, coords::cube)
{
    using lyt_t = cartesian_layout<TestType>;

    std::vector<TestType> coord_vector{};
    coord_vector.reserve(7);

    const lyt_t lyt{{1, 1, 1}};

    const auto fill_coord_vector = [&v = coord_vector](const auto& c) { v.emplace_back(c); };

    SECTION("With bounds")
    {
        lyt.foreach_coordinate(fill_coord_vector, {1, 0, 0}, {1, 1, 1});

        REQUIRE(coord_vector.size() == 6);

        CHECK(coord_vector[0] == TestType{1, 0, 0});

        CHECK(coord_vector[1] == TestType{0, 1, 0});
        CHECK(coord_vector[2] == TestType{1, 1, 0});
        CHECK(coord_vector[3] == TestType{0, 0, 1});
        CHECK(coord_vector[4] == TestType{1, 0, 1});

        CHECK(coord_vector[5] == TestType{0, 1, 1});
    }
    SECTION("Without bounds")
    {
        coord_vector.clear();
        coord_vector.reserve(8);

        lyt.foreach_coordinate(fill_coord_vector);

        CHECK(coord_vector.size() == 8);

        CHECK(coord_vector.front().str() == fmt::format("{}", TestType{0, 0, 0}));
        CHECK(coord_vector.back().str() == fmt::format("{}", TestType{1, 1, 1}));
    }
    SECTION("With non-dead out of bounds end bound")
    {
        std::vector<TestType> good_bound_coord_vector{};

        const auto fill_good_bound_coord_vector = [&v = good_bound_coord_vector](const auto& c) { v.emplace_back(c); };

        const auto test_bounds_equal = [&](const auto& c_lyt, const TestType& bad_bound, const TestType& good_bound)
        {
            coord_vector.clear();
            coord_vector.reserve(8);

            good_bound_coord_vector.clear();
            good_bound_coord_vector.reserve(8);

            c_lyt.foreach_coordinate(fill_coord_vector, {}, bad_bound);
            c_lyt.foreach_coordinate(fill_good_bound_coord_vector, {}, good_bound);

            CHECK(coord_vector.size() == good_bound_coord_vector.size());
            CHECK(coord_vector.back() == good_bound_coord_vector.back());
        };

        test_bounds_equal(lyt, {9, 9, 9}, {});
        test_bounds_equal(lyt, {0, 2, 1}, {});

        if constexpr (std::is_same_v<TestType, coords::cube>)
        {
            test_bounds_equal(lyt, {0, 0, 9}, {});
        }

        test_bounds_equal(lyt, {2, 0, 0}, {0, 1, 0});
        test_bounds_equal(lyt, {2, 0, 1}, {0, 1, 1});
        test_bounds_equal(lyt, {2, 1, 0}, {0, 0, 1});
        test_bounds_equal(lyt, {0, 2, 0}, {0, 0, 1});

        test_bounds_equal(lyt_t{aspect_ratio<lyt_t>{0, 1, 0}}, {0, 1, 1}, {});

        test_bounds_equal(lyt_t{aspect_ratio<lyt_t>{0, 0, 0}}, {9, 9, 9}, {});
    }
}

TEST_CASE("Computing area and volume of offset coordinates", "[coordinates]")
{
    CHECK(coords::area_of(coords::offset{1, 1, 1}) == 4);
    CHECK(coords::volume_of(coords::offset{1, 1, 1}) == 8);
}

TEST_CASE("Computing area and volume of cube coordinates", "[coordinates]")
{
    CHECK(coords::area_of(coords::cube{1, 1, 1}) == 4);
    CHECK(coords::area_of(coords::cube{-1, -1, -1}) == 4);

    CHECK(coords::volume_of(coords::cube{-1, -1, -1}) == 8);
    CHECK(coords::volume_of(coords::cube{1, 1, 1}) == 8);
}

TEST_CASE("Addition / subtraction of cube coordinates", "[coordinates]")
{
    using coord = coords::cube;

    CHECK(coord{-4, 4, -43} + coord{1, -7, 27} == coord{-3, -3, -16});
    CHECK(coord{-4, 4, 42} - coord{1, -7, 24} == coord{-5, 11, 18});
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
