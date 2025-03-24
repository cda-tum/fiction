//
// Created by marcel on 15.09.21.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/traits.hpp>

#include <fmt/format.h>

#include <limits>
#include <map>
#include <sstream>
#include <vector>

using namespace fiction;

#pragma GCC diagnostic push
#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wuseless-cast"
#endif
#pragma GCC diagnostic ignored "-Wconversion"

TEST_CASE("Unsigned offset coordinates", "[coordinates]")
{
    using coordinate = offset::ucoord_t;

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

TEST_CASE("SiQAD coordinate conversion", "[coordinates]")
{
    using coordinate         = siqad::coord_t;
    using coordinate_fiction = cube::coord_t;

    auto t = coordinate{};
    CHECK(t.is_dead());
    auto fiction_d = siqad::to_fiction_coord<coordinate_fiction>(t);
    CHECK(fiction_d.is_dead());

    auto t0        = coordinate{0, 0, 0};
    auto fiction_0 = siqad::to_fiction_coord<coordinate_fiction>(t0);
    CHECK(!fiction_0.is_dead());

    auto t1         = coordinate{1, 3, 1};
    auto t1_fiction = siqad::to_fiction_coord<coordinate_fiction>(t1);
    CHECK(t1_fiction.x == t1.x);
    CHECK(t1_fiction.y == 7);
    auto t2 = siqad::to_siqad_coord<coordinate_fiction>(t1_fiction);
    CHECK(t1 == t2);

    auto t3_fiction = coordinate_fiction{1, 2};
    auto t3_siqad   = siqad::to_siqad_coord<coordinate_fiction>(t3_fiction);
    CHECK(t3_siqad.x == t3_fiction.x);
    CHECK(t3_siqad.y == 1);
    CHECK(t3_siqad.z == 0);
    CHECK(t3_fiction == siqad::to_fiction_coord<coordinate_fiction>(t3_siqad));

    auto t4_fiction = coordinate_fiction{-1, -2};
    auto t4_siqad   = siqad::to_siqad_coord<coordinate_fiction>(t4_fiction);
    CHECK(t4_siqad.x == t4_fiction.x);
    CHECK(t4_siqad.y == -1);
    CHECK(t4_siqad.z == 0);
    CHECK(t4_fiction == siqad::to_fiction_coord<coordinate_fiction>(t4_siqad));

    auto t5_siqad   = coordinate{-1, -2, 1};
    auto t5_fiction = siqad::to_fiction_coord<coordinate_fiction>(t5_siqad);
    CHECK(t5_fiction.x == -1);
    CHECK(t5_fiction.y == -3);
    CHECK(t5_fiction == siqad::to_fiction_coord<coordinate_fiction>(t5_siqad));

    auto t6_fiction = coordinate_fiction{-1, -1};
    auto t6_siqad   = siqad::to_siqad_coord<coordinate_fiction>(t6_fiction);
    CHECK(t6_siqad.x == t6_fiction.x);
    CHECK(t6_siqad.y == -1);
    CHECK(t6_siqad.z == 1);
    CHECK(t6_fiction == siqad::to_fiction_coord<coordinate_fiction>(t6_siqad));

    auto t7_fiction = coordinate_fiction{-1, -3};
    auto t7_siqad   = siqad::to_siqad_coord<coordinate_fiction>(t7_fiction);
    CHECK(t7_siqad.x == t7_fiction.x);
    CHECK(t7_siqad.y == -2);
    CHECK(t7_siqad.z == 1);
    CHECK(t7_fiction == siqad::to_fiction_coord<coordinate_fiction>(t7_siqad));

    auto t8_fiction = coordinate_fiction{-1, -4};
    auto t8_siqad   = siqad::to_siqad_coord<coordinate_fiction>(t8_fiction);
    CHECK(t8_siqad.x == t8_fiction.x);
    CHECK(t8_siqad.y == -2);
    CHECK(t8_siqad.z == 0);
    CHECK(t8_fiction == siqad::to_fiction_coord<coordinate_fiction>(t8_siqad));

    // Test for overflow scenario
    auto t9        = coordinate{1, (std::numeric_limits<int32_t>::max() - 1) / 2, 1};
    auto fiction_9 = siqad::to_fiction_coord<coordinate_fiction>(t9);
    CHECK(fiction_9.x == t9.x);
    CHECK(fiction_9.y == std::numeric_limits<int32_t>::max());

    // Test for underflow scenario
    auto t10        = coordinate{1, (std::numeric_limits<int32_t>::min() + 1) / 2};
    auto fiction_10 = siqad::to_fiction_coord<coordinate_fiction>(t10);
    CHECK(fiction_10.x == t10.x);
    CHECK(fiction_10.y == std::numeric_limits<int32_t>::min() + 2);
}

TEST_CASE("Offset to cube coordinate conversion", "[coordinates]")
{
    using offset = offset::ucoord_t;

    auto t = offset{};
    CHECK(t.is_dead());
    auto fiction_d = offset_to_cube_coord(t);
    CHECK(fiction_d.is_dead());

    auto t0        = offset{0, 0, 0};
    auto fiction_0 = offset_to_cube_coord(t0);
    CHECK(!fiction_0.is_dead());

    auto t1      = offset{1, 3, 1};
    auto t1_cube = offset_to_cube_coord(t1);
    CHECK(t1_cube.x == t1.x);
    CHECK(t1_cube.y == 3);

    auto t2      = offset{1, 2};
    auto t2_cube = offset_to_cube_coord(t2);
    CHECK(t2_cube.x == t2.x);
    CHECK(t2_cube.y == 2);
    CHECK(t2_cube.z == 0);
}

TEMPLATE_TEST_CASE("Coordinate iteration", "[coordinates]", offset::ucoord_t, cube::coord_t, siqad::coord_t)
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

        if constexpr (std::is_same_v<TestType, siqad::coord_t>)
        {
            CHECK(coord_vector[1] == TestType{0, 0, 1});
            CHECK(coord_vector[2] == TestType{1, 0, 1});
            CHECK(coord_vector[3] == TestType{0, 1, 0});
            CHECK(coord_vector[4] == TestType{1, 1, 0});
        }
        else
        {
            CHECK(coord_vector[1] == TestType{0, 1, 0});
            CHECK(coord_vector[2] == TestType{1, 1, 0});
            CHECK(coord_vector[3] == TestType{0, 0, 1});
            CHECK(coord_vector[4] == TestType{1, 0, 1});
        }

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

        if constexpr (std::is_same_v<TestType, cube::coord_t>)
        {
            test_bounds_equal(lyt, {0, 0, 9}, {});
        }

        if constexpr (std::is_same_v<TestType, siqad::coord_t>)
        {
            test_bounds_equal(lyt, {2, 0, 0}, {0, 0, 1});
            test_bounds_equal(lyt, {2, 0, 1}, {0, 1, 0});
            test_bounds_equal(lyt, {2, 1, 0}, {0, 1, 1});
            test_bounds_equal(lyt, {0, 2, 0}, {});

            using h_lyt = hexagonal_layout<TestType, even_row_hex>;

            test_bounds_equal(h_lyt{aspect_ratio_type_t<h_lyt>{0, 1, 0}}, {0, 1, 1}, {});
        }
        else
        {
            test_bounds_equal(lyt, {2, 0, 0}, {0, 1, 0});
            test_bounds_equal(lyt, {2, 0, 1}, {0, 1, 1});
            test_bounds_equal(lyt, {2, 1, 0}, {0, 0, 1});
            test_bounds_equal(lyt, {0, 2, 0}, {0, 0, 1});

            test_bounds_equal(lyt_t{aspect_ratio_type_t<lyt_t>{0, 1, 0}}, {0, 1, 1}, {});
        }

        test_bounds_equal(lyt_t{aspect_ratio_type_t<lyt_t>{0, 0, 0}}, {9, 9, 9}, {});
    }
}

TEST_CASE("Computing area and volume of offset coordinates", "[coordinates]")
{
    CHECK(aspect_ratio(offset::ucoord_t{1, 1, 1}).area() == 4);
    CHECK(aspect_ratio(offset::ucoord_t{1, 1}).area() == 4);
    CHECK(aspect_ratio(offset::ucoord_t{1, 1, 0}, offset::ucoord_t{2, 2, 1}).area() == 4);

    CHECK(aspect_ratio(offset::ucoord_t{1, 1, 1}).volume() == 8);
    CHECK(aspect_ratio(offset::ucoord_t{1, 1}).volume() == 4);
    CHECK(aspect_ratio(offset::ucoord_t{1, 1, 0}, offset::ucoord_t{2, 2, 1}).volume() == 8);
}

TEST_CASE("Computing area and volume of cube coordinates", "[coordinates]")
{
    CHECK(aspect_ratio(cube::coord_t{1, 1, 1}).area() == 4);
    CHECK(aspect_ratio(cube::coord_t{-1, -1, -1}).area() == 4);
    CHECK(aspect_ratio(cube::coord_t{-1, -1, 0}, cube::coord_t{1, 1, 1}).area() == 9);

    CHECK(aspect_ratio(cube::coord_t{-1, -1, -1}).volume() == 8);
    CHECK(aspect_ratio(cube::coord_t{1, 1, 1}).volume() == 8);
    CHECK(aspect_ratio(cube::coord_t{-1, -1, 0}, cube::coord_t{1, 1, 1}).volume() == 18);
}

TEST_CASE("Computing area and volume of SiQAD coordinates", "[coordinates]")
{
    CHECK(aspect_ratio(siqad::coord_t{1, 1, 1}).area() == 8);
    CHECK(aspect_ratio(siqad::coord_t{-1, -1, 1}).area() == 8);
    CHECK(aspect_ratio(siqad::coord_t{-1, -1, 0}, siqad::coord_t{1, 1, 1}).area() == 18);

    CHECK(aspect_ratio(siqad::coord_t{1, 1, 1}).volume() == 8);
    CHECK(aspect_ratio(siqad::coord_t{-1, -1, 1}).volume() == 8);
    CHECK(aspect_ratio(siqad::coord_t{-1, -1, 0}, siqad::coord_t{1, 1, 1}).volume() == 18);
}

TEST_CASE("Addition / subtraction of SiQAD coordinates", "[coordinates]")
{
    using coord = siqad::coord_t;

    CHECK(coord{-4, 4, 1} + coord{1, -7, 1} == coord{-3, -2, 0});
    CHECK(coord{-4, 4, 1} + coord{1, -7, 0} == coord{-3, -3, 1});

    CHECK(coord{-4, 4, 0} - coord{1, -7, 1} == coord{-5, 10, 1});
    CHECK(coord{-4, 4, 1} - coord{1, -7, 1} == coord{-5, 11, 0});
}

TEST_CASE("Addition / subtraction of cube coordinates", "[coordinates]")
{
    using coord = cube::coord_t;

    CHECK(coord{-4, 4, -43} + coord{1, -7, 27} == coord{-3, -3, -16});
    CHECK(coord{-4, 4, 42} - coord{1, -7, 24} == coord{-5, 11, 18});
}

TEMPLATE_TEST_CASE("aspect_ratio constructor and operator checks", "[coordinates]", offset::ucoord_t, cube::coord_t,
                   siqad::coord_t)
{
    using coord_t      = TestType;
    using aspect_ratio = aspect_ratio<coord_t>;

    SECTION("Default constructor: min == max == (0,0,0)")
    {
        aspect_ratio ar;
        CHECK(ar.min.x == 0);
        CHECK(ar.min.y == 0);
        CHECK(ar.min.z == 0);

        CHECK(ar.max.x == 0);
        CHECK(ar.max.y == 0);
        CHECK(ar.max.z == 0);

        CHECK(ar.x_min() == 0);
        CHECK(ar.y_min() == 0);
        CHECK(ar.z_min() == 0);

        CHECK(ar.x() == 0);
        CHECK(ar.y() == 0);
        CHECK(ar.z() == 0);

        CHECK(ar.x_size() == 0);
        CHECK(ar.y_size() == 0);
        CHECK(ar.z_size() == 0);
    }

    SECTION("Single-coordinate constructor (0,0,0) -> e")
    {
        coord_t      e{1, 2, 1};
        aspect_ratio ar(e);

        // min should still be (0,0,0)
        CHECK(ar.min.x == 0);
        CHECK(ar.min.y == 0);
        CHECK(ar.min.z == 0);

        // max should be e
        CHECK(ar.max.x == e.x);
        CHECK(ar.max.y == e.y);
        CHECK(ar.max.z == e.z);
    }

    SECTION("Min/Max constructor")
    {
        // use different values for min and max
        coord_t mn{-1, -2, -3};
        coord_t mx{4, 5, 1};

        // if the coordinate type is unsigned (offset::ucoord_t),
        // negative initialization effectively becomes 0.
        if constexpr (std::is_same_v<coord_t, offset::ucoord_t>)
        {
            mn = {0, 0, 0};
            mx = {4, 5, 1};
        }

        aspect_ratio ar(mn, mx);

        CHECK(ar.min.x == mn.x);
        CHECK(ar.min.y == mn.y);
        CHECK(ar.min.z == mn.z);

        CHECK(ar.max.x == mx.x);
        CHECK(ar.max.y == mx.y);
        CHECK(ar.max.z == mx.z);

        // check dimension sizes
        auto expected_x_size = (mx.x >= mn.x) ? static_cast<decltype(ar.x_size())>(mx.x - mn.x) :
                                                static_cast<decltype(ar.x_size())>(mn.x - mx.x);
        auto expected_y_size = (mx.y >= mn.y) ? static_cast<decltype(ar.y_size())>(mx.y - mn.y) :
                                                static_cast<decltype(ar.y_size())>(mn.y - mx.y);
        auto expected_z_size = (mx.z >= mn.z) ? static_cast<decltype(ar.z_size())>(mx.z - mn.z) :
                                                static_cast<decltype(ar.y_size())>(mn.z - mx.z);

        CHECK(ar.x_size() == expected_x_size);
        CHECK(ar.y_size() == expected_y_size);
        CHECK(ar.z_size() == expected_z_size);
    }

    SECTION("Templated constructor (3D)")
    {
        // construct from integral x,y,z
        aspect_ratio ar(10, 20, 1);

        CHECK(ar.min.x == 0);
        CHECK(ar.min.y == 0);
        CHECK(ar.min.z == 0);

        CHECK(ar.max.x == 10);
        CHECK(ar.max.y == 20);
        CHECK(ar.max.z == 1);
    }

    SECTION("Templated constructor (2D)")
    {
        aspect_ratio ar(10, 20);

        CHECK(ar.min.x == 0);
        CHECK(ar.min.y == 0);
        CHECK(ar.min.z == 0);

        CHECK(ar.max.x == 10);
        CHECK(ar.max.y == 20);
        CHECK(ar.max.z == 0);  // z should be 0 in 2D
    }

    SECTION("Equality and inequality")
    {
        aspect_ratio ar1(5, 6, 1);
        aspect_ratio ar2(5, 6, 1);
        aspect_ratio ar3(1, 2, 1);

        CHECK(ar1 == ar2);
        CHECK(ar1 != ar3);
        CHECK_FALSE(ar1 == ar3);
        CHECK_FALSE(ar1 != ar2);
    }
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
