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

#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/layout_base.hpp>
#include <fiction/traits.hpp>

#include <fmt/format.h>

#include <cstdint>
#include <map>
#include <sstream>
#include <vector>

using namespace fiction;
using namespace fiction::layouts;

#pragma GCC diagnostic push
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wuseless-cast"
#endif
#pragma GCC diagnostic ignored "-Wconversion"

TEST_CASE("Signed offset coordinates", "[coordinates]")
{
    using coordinate = layout_base::coordinate;

    auto td = coordinate{};
    CHECK(!td.is_valid());

    auto t0 = coordinate{0, 0, 0};
    CHECK(t0.is_valid());

    CHECK(t0 != td);
    CHECK(td == coordinate{});

    auto t1 = coordinate{1, 2, 0};
    auto t2 = coordinate{1, 2};

    CHECK(t0 < t1);
    CHECK(t1 > t0);
    CHECK(t1 >= t0);
    CHECK(t0 <= t1);
    CHECK(t1 == t2);
    CHECK(t2 == t1);

    t1.x++;

    CHECK(t1 != t2);
    CHECK(t1 > t2);
    CHECK(t1 >= t2);
    CHECK(t2 < t1);
    CHECK(t2 <= t1);

    auto t3 = coordinate{0, 0, 1};

    CHECK(t1 < t3);
    CHECK(t2 < t3);

    SECTION("Negative axes")
    {
        const coordinate n{-1, -2, 0};

        CHECK(n.is_valid());
        CHECK(n.x == -1);
        CHECK(n.y == -2);
        CHECK(n < t0);
        CHECK(coordinate{-3, 0, 0} < coordinate{-2, 0, 0});
        CHECK(coordinate{5, -1, 0} < coordinate{-5, 0, 0});
        CHECK(coordinate{5, 5, -1} < coordinate{-5, -5, 0});
    }
    SECTION("Signal encoding")
    {
        const std::map<uint64_t, coordinate> coordinate_repr{
            {0x8000000000000000, coordinate{}},          {0x0000000000000000, coordinate{0, 0, 0}},
            {0x4000000000000000, coordinate{0, 0, 1}},   {0x4000000080000001, coordinate{1, 1, 1}},
            {0x0000000000000002, coordinate{2, 0, 0}},   {0x1fffffffbfffffff, coordinate{1073741823, 1073741823, 0}},
            {0x3fffffffffffffff, coordinate{-1, -1, 0}}, {0x5fffffffc0000000, coordinate{-1073741824, 1073741823, 1}}};

        for (auto [repr, coord] : coordinate_repr)
        {
            CHECK(static_cast<coordinate>(repr) == coord);
            CHECK(repr == static_cast<uint64_t>(coord));
            CHECK(coordinate{repr} == coord);
            CHECK(coordinate{coord} == coord);
            CHECK(coordinate{static_cast<uint64_t>(coord)} == coord);
        }

        // the invalid coordinate ignores all further bits of its encoding
        CHECK(coordinate{0xffffffffffffffff} == coordinate{});
    }
    SECTION("Range of the signal encoding")
    {
        CHECK(coordinate{0, 0, 0}.fits_signal());
        CHECK(coordinate{1073741823, 1073741823, 1}.fits_signal());
        CHECK(coordinate{-1073741824, -1073741824, 0}.fits_signal());

        CHECK(!coordinate{1073741824, 0, 0}.fits_signal());
        CHECK(!coordinate{0, 1073741824, 0}.fits_signal());
        CHECK(!coordinate{-1073741825, 0, 0}.fits_signal());
        CHECK(!coordinate{0, 0, 2}.fits_signal());
        CHECK(!coordinate{0, 0, -1}.fits_signal());
    }
    SECTION("Hash")
    {
        CHECK(std::hash<coordinate>{}(coordinate{5, 7, 1}) == std::hash<uint64_t>{}(0x4000000380000005));
        CHECK(std::hash<coordinate>{}(coordinate{}) == std::hash<uint64_t>{}(0x8000000000000000));
    }

    std::ostringstream os{};
    os << coordinate{3, 2, 1};
    CHECK(os.str() == "(3,2,1)");
    CHECK(coordinate{-3, 2, 1}.str() == "(-3,2,1)");
}

TEST_CASE("Coordinate iteration", "[coordinates]")
{
    using coord_t = layout_base::coordinate;
    using lyt_t   = cartesian_layout;

    std::vector<coord_t> coord_vector{};
    coord_vector.reserve(7);

    const lyt_t lyt{{1, 1, 1}};

    const auto fill_coord_vector = [&v = coord_vector](const auto& c) { v.emplace_back(c); };

    SECTION("With bounds")
    {
        lyt.foreach_coordinate(fill_coord_vector, {1, 0, 0}, {1, 1, 1});

        REQUIRE(coord_vector.size() == 6);

        CHECK(coord_vector[0] == coord_t{1, 0, 0});

        CHECK(coord_vector[1] == coord_t{0, 1, 0});
        CHECK(coord_vector[2] == coord_t{1, 1, 0});
        CHECK(coord_vector[3] == coord_t{0, 0, 1});
        CHECK(coord_vector[4] == coord_t{1, 0, 1});

        CHECK(coord_vector[5] == coord_t{0, 1, 1});
    }
    SECTION("Without bounds")
    {
        coord_vector.clear();
        coord_vector.reserve(8);

        lyt.foreach_coordinate(fill_coord_vector);

        CHECK(coord_vector.size() == 8);

        CHECK(coord_vector.front().str() == fmt::format("{}", coord_t{0, 0, 0}));
        CHECK(coord_vector.back().str() == fmt::format("{}", coord_t{1, 1, 1}));
    }
    SECTION("With non-dead out of bounds end bound")
    {
        std::vector<coord_t> good_bound_coord_vector{};

        const auto fill_good_bound_coord_vector = [&v = good_bound_coord_vector](const auto& c) { v.emplace_back(c); };

        const auto test_bounds_equal = [&](const auto& c_lyt, const coord_t& bad_bound, const coord_t& good_bound)
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

        test_bounds_equal(lyt, {0, 0, 9}, {});

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
    CHECK(area_of(layout_base::coordinate{1, 1, 1}) == 4);
    CHECK(volume_of(layout_base::coordinate{1, 1, 1}) == 8);

    CHECK(area_of(layout_base::coordinate{-1, -1, -1}) == 4);
    CHECK(volume_of(layout_base::coordinate{-1, -1, -1}) == 8);
}

#pragma GCC diagnostic pop
