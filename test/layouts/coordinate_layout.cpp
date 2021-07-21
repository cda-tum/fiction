//
// Created by marcel on 31.03.21.
//

#include "catch.hpp"

#include <fiction/layouts/coordinate_layout.hpp>

#include <map>
#include <set>
#include <sstream>

using namespace fiction;

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wconversion"
#endif

TEST_CASE("Coordinates", "[coordinate")
{
    auto td = coordinate_layout::coordinate{};
    CHECK(td.is_dead());

    auto t0 = coordinate_layout::coordinate{0, 0, 0};
    CHECK(!t0.is_dead());

    CHECK(t0 != td);

    auto t1 = coordinate_layout::coordinate{1, 2, 0};
    auto t2 = coordinate_layout::coordinate{1, 2};

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

    auto t3 = coordinate_layout::coordinate{0, 0, 1};

    CHECK(t1 < t3);
    CHECK(t2 < t3);

    std::map<uint64_t, coordinate_layout::coordinate> coordinate_repr{
        {0x8000000000000000, coordinate_layout::coordinate{}},
        {0x0000000000000000, coordinate_layout::coordinate{0, 0, 0}},
        {0x4000000000000000, coordinate_layout::coordinate{0, 0, 1}},
        {0x4000000080000001, coordinate_layout::coordinate{1, 1, 1}},
        {0x0000000000000002, coordinate_layout::coordinate{2, 0, 0}},
        {0x3fffffffffffffff, coordinate_layout::coordinate{2147483647, 2147483647, 0}}};

    for (auto [repr, coordinate] : coordinate_repr)
    {
        CHECK(static_cast<coordinate_layout::coordinate>(repr) == coordinate);
        CHECK(repr == static_cast<uint64_t>(coordinate));
        CHECK(coordinate_layout::coordinate{repr} == coordinate);
        CHECK(coordinate_layout::coordinate{coordinate} == coordinate);
        CHECK(coordinate_layout::coordinate{static_cast<uint64_t>(coordinate)} == coordinate);
    }

    std::ostringstream os{};
    os << coordinate_layout::coordinate{3, 2, 1};
    CHECK(os.str() == "(3,2,1)");
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

TEST_CASE("Coordinate iteration", "[coordinate]")
{
    coordinate_layout::aspect_ratio ar{9, 9, 1};

    coordinate_layout layout{ar};

    std::set<coordinate_layout::coordinate> visited{};

    const auto check1 = [&visited, &ar](const auto& t)
    {
        CHECK(t <= ar);

        // no coordinate is visited twice
        CHECK(visited.count(t) == 0);
        visited.insert(t);
    };

    for (auto&& t : layout.coordinates()) { check1(t); }
    CHECK(visited.size() == 200);

    visited.clear();

    layout.foreach_coordinate(check1);
    CHECK(visited.size() == 200);

    visited.clear();

    coordinate_layout::aspect_ratio ar_ground{ar.x, ar.y, 0};

    const auto check2 = [&visited, &ar_ground](const auto& t)
    {
        // iteration stays in ground layer
        CHECK(t.z == 0);
        CHECK(t <= ar_ground);

        // no coordinate is visited twice
        CHECK(visited.count(t) == 0);
        visited.insert(t);
    };

    for (auto&& t : layout.ground_coordinates()) { check2(t); }
    CHECK(visited.size() == 100);

    visited.clear();

    layout.foreach_ground_coordinate(check2);
    CHECK(visited.size() == 100);

    visited.clear();

    coordinate_layout::coordinate start{2, 2}, stop{5, 4};

    const auto check3 = [&visited, &start, &stop](const auto& t)
    {
        CHECK(t.z == 0);
        // iteration stays in between the bounds
        CHECK(t >= start);
        CHECK(t < stop);

        // no coordinate is visited twice
        CHECK(visited.count(t) == 0);
        visited.insert(t);
    };

    for (auto&& t : layout.coordinates(start, stop)) { check3(t); }
    CHECK(visited.size() == 23);

    visited.clear();

    layout.foreach_coordinate(check3, start, stop);
    CHECK(visited.size() == 23);
}

TEST_CASE("Cardinal operations", "[coordinate]")
{
    coordinate_layout::aspect_ratio ar{10, 10, 1};

    coordinate_layout layout{ar};

    const auto check = [&](const auto& t, const auto& at1, const auto& at2, const auto& b, const auto& bt)
    {
        CHECK(!at1.is_dead());
        CHECK(at1 == at2);
        CHECK(layout.is_adjacent_of(t, at1));
        CHECK(layout.is_adjacent_of(at1, t));

        CHECK(layout.is_border(b));
        CHECK(!bt.is_dead());
        CHECK(b == bt);
        CHECK(layout.is_ground_layer(bt));
        CHECK(layout.is_border(bt));
    };

    auto t = coordinate_layout::coordinate{5, 5};

    auto nt  = coordinate_layout::coordinate{5, 4};
    auto bnt = coordinate_layout::coordinate{5, 0};

    check(t, layout.north(t), nt, bnt, layout.north(bnt));
    CHECK(layout.is_north_of(t, nt));
    CHECK(layout.is_northwards_of(t, nt));
    CHECK(layout.is_northwards_of(t, bnt));
    CHECK(layout.is_northern_border(bnt));
    CHECK(layout.northern_border_of(t) == bnt);

    auto et  = coordinate_layout::coordinate{6, 5};
    auto bet = coordinate_layout::coordinate{10, 5};

    check(t, layout.east(t), et, bet, layout.east(bet));
    CHECK(layout.is_east_of(t, et));
    CHECK(layout.is_eastwards_of(t, et));
    CHECK(layout.is_eastwards_of(t, bet));
    CHECK(layout.is_eastern_border(bet));
    CHECK(layout.eastern_border_of(t) == bet);

    auto st  = coordinate_layout::coordinate{5, 6};
    auto bst = coordinate_layout::coordinate{5, 10};

    check(t, layout.south(t), st, bst, layout.south(bst));
    CHECK(layout.is_south_of(t, st));
    CHECK(layout.is_southwards_of(t, st));
    CHECK(layout.is_southwards_of(t, bst));
    CHECK(layout.is_southern_border(bst));
    CHECK(layout.southern_border_of(t) == bst);

    auto wt  = coordinate_layout::coordinate{4, 5};
    auto bwt = coordinate_layout::coordinate{0, 5};

    check(t, layout.west(t), wt, bwt, layout.west(bwt));
    CHECK(layout.is_west_of(t, wt));
    CHECK(layout.is_westwards_of(t, wt));
    CHECK(layout.is_westwards_of(t, bwt));
    CHECK(layout.is_western_border(bwt));
    CHECK(layout.western_border_of(t) == bwt);

    auto at  = coordinate_layout::coordinate{5, 5, 1};
    auto bat = layout.above(at);

    CHECK(!at.is_dead());
    CHECK(layout.is_above_of(t, at));
    CHECK(at == bat);
    CHECK(layout.is_crossing_layer(bat));
    CHECK(!layout.is_border(at));

    // cover corner case
    coordinate_layout planar_layout{{1, 1, 0}};
    auto              dat = planar_layout.above({1, 1, 1});
    CHECK(dat.is_dead());

    auto bt  = layout.below(at);
    auto bbt = layout.below(bt);

    CHECK(!bt.is_dead());
    CHECK(bt == t);
    CHECK(layout.is_below_of(at, bt));
    CHECK(!bbt.is_dead());
    CHECK(bt == bbt);
    CHECK(layout.is_ground_layer(bbt));

    auto s1 = layout.adjacent_coordinates<std::set<coordinate_layout::coordinate>>({5, 5});
    auto s2 = std::set<coordinate_layout::coordinate>{{{4, 5}, {5, 4}, {6, 5}, {5, 6}}};

    CHECK(s1 == s2);
}
