//
// Created by marcel on 15.09.21.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/coordinates.hpp>

#include <map>
#include <set>
#include <sstream>

using namespace fiction;

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wconversion"
#endif

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

    std::map<uint64_t, coordinate> coordinate_repr{
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

TEST_CASE("siqad coordinate conversion", "[coordinates]")
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

    auto t4_fiction = coordinate_fiction{-1, -2};
    auto t4_siqad   = siqad::to_siqad_coord<coordinate_fiction>(t4_fiction);
    CHECK(t4_siqad.x == t4_fiction.x);
    CHECK(t4_siqad.y == -1);
    CHECK(t4_siqad.z == 0);

    auto t5_siqad   = coordinate{-1, -2, 1};
    auto t5_fiction = siqad::to_fiction_coord<coordinate_fiction>(t5_siqad);
    CHECK(t5_fiction.x == -1);
    CHECK(t5_fiction.y == -3);
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
