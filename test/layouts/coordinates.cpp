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

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
