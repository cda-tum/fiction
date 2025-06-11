//
// Created by marcel on 31.03.21.
//

#include <catch2/catch_template_test_macros.hpp>

#include "catch2/catch_test_macros.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/traits.hpp>

#include <cstdlib>
#include <set>

using namespace fiction;

TEMPLATE_TEST_CASE("Cartesian layout traits", "[cartesian-layout]", offset::ucoord_t, cube::coord_t)
{
    using Layout = cartesian_layout<TestType>;

    CHECK(has_north_v<Layout>);
    CHECK(has_east_v<Layout>);
    CHECK(has_south_v<Layout>);
    CHECK(has_west_v<Layout>);
    CHECK(has_cardinal_operations_v<Layout>);
    CHECK(has_north_east_v<Layout>);
    CHECK(has_south_east_v<Layout>);
    CHECK(has_south_west_v<Layout>);
    CHECK(has_north_west_v<Layout>);
    CHECK(has_ordinal_operations_v<Layout>);
    CHECK(has_above_v<Layout>);
    CHECK(has_below_v<Layout>);
    CHECK(has_elevation_operations_v<Layout>);
    CHECK(is_coordinate_layout_v<Layout>);
    CHECK(is_cartesian_layout_v<Layout>);
    CHECK(!is_tile_based_layout_v<Layout>);
    CHECK(!is_hexagonal_layout_v<Layout>);

    CHECK(has_foreach_coordinate_v<Layout>);
    CHECK(has_foreach_adjacent_coordinate_v<Layout>);
    CHECK(has_foreach_adjacent_opposite_coordinates_v<Layout>);
}

TEMPLATE_TEST_CASE("Coordinate creation", "[cartesian-layout]", offset::ucoord_t, cube::coord_t)
{
    using Layout = cartesian_layout<TestType>;
    using coord  = typename Layout::coordinate;

    // create a layout with max=(3,3)
    const Layout lyt{{3, 3}};

    // checks (same logic for offset/cube)
    CHECK(lyt.coord(0, 0, 0) == coord{0, 0, 0});
    CHECK(lyt.coord(0, 0, 1) == coord{0, 0, 1});
    CHECK(lyt.coord(1, 0) == coord{1, 0});
    CHECK(lyt.coord(2, 0) == coord{2, 0});
    CHECK(lyt.coord(0, 1) == coord{0, 1});
    CHECK(lyt.coord(1, 1) == coord{1, 1});
    CHECK(lyt.coord(2, 1) == coord{2, 1});
}

TEMPLATE_TEST_CASE("Deep copy Cartesian layout", "[cartesian-layout]", offset::ucoord_t, cube::coord_t)
{
    using Layout = cartesian_layout<TestType>;
    using ar_t   = typename Layout::aspect_ratio_type;

    // original layout with max=(5,5,0)
    const Layout original{{5, 5, 0}};

    // clone
    auto copy = original.clone();

    // resize copy to (10,10,1)
    copy.resize(ar_t{10, 10, 1});

    // original is still (5,5,0)
    CHECK(original.x() == 5);
    CHECK(original.y() == 5);
    CHECK(original.z() == 0);

    // copy changed
    CHECK(copy.x() == 10);
    CHECK(copy.y() == 10);
    CHECK(copy.z() == 1);
}

TEMPLATE_TEST_CASE("Cartesian coordinate iteration", "[cartesian-layout]", offset::ucoord_t, cube::coord_t)
{
    using Layout = cartesian_layout<TestType>;
    using coord  = typename Layout::coordinate;
    using ar_t   = typename Layout::aspect_ratio_type;

    ar_t   ar{9, 9, 1};
    Layout layout{ar};

    std::set<coord> visited{};

    const auto check1 = [&visited, &ar, &layout](const auto& t)
    {
        CHECK(t <= ar.max);

        // all coordinates are within the layout bounds
        CHECK(layout.is_within_bounds(t));

        // no coordinate is visited twice
        CHECK(visited.count(t) == 0);
        visited.insert(t);
    };

    for (auto&& t : layout.coordinates())
    {
        check1(t);
    }
    CHECK(visited.size() == 200);

    visited.clear();

    layout.foreach_coordinate(check1);
    CHECK(visited.size() == 200);

    visited.clear();

    // ground coordinates => e.g., z=0
    ar_t       ar_ground{ar.x(), ar.y(), 0};
    const auto check2 = [&visited, &ar_ground, &layout](const auto& t)
    {
        CHECK(t.z == 0);
        CHECK(t <= ar_ground.max);

        // all coordinates are within the layout bounds
        CHECK(layout.is_within_bounds(t));

        // no coordinate is visited twice
        CHECK(visited.count(t) == 0);
        visited.insert(t);
    };

    for (auto&& t : layout.ground_coordinates())
    {
        check2(t);
    }
    CHECK(visited.size() == 100);

    visited.clear();

    layout.foreach_ground_coordinate(check2);
    CHECK(visited.size() == 100);

    visited.clear();

    // partial iteration from start=(2,2,0) to stop=(5,4,0)
    coord start{2, 2}, stop{5, 4};

    const auto check3 = [&visited, &start, &stop, &layout](const auto& t)
    {
        CHECK(t.z == 0);
        // iteration stays in between the bounds
        CHECK(t >= start);
        CHECK(t < stop);

        // all coordinates are within the layout bounds
        CHECK(layout.is_within_bounds(t));

        // no coordinate is visited twice
        CHECK(visited.count(t) == 0);
        visited.insert(t);
    };

    for (auto&& t : layout.coordinates(start, stop))
    {
        check3(t);
    }
    CHECK(visited.size() == 23);

    visited.clear();

    layout.foreach_coordinate(check3, start, stop);
    CHECK(visited.size() == 23);
}

TEMPLATE_TEST_CASE("Cartesian coordinate iteration with non-zero minimum", "[cartesian-layout]", offset::ucoord_t,
                   cube::coord_t)
{
    using Layout = cartesian_layout<TestType>;
    using coord  = typename Layout::coordinate;
    using ar_t   = typename Layout::aspect_ratio_type;

    // offset => min=(1,1,0), max=(9,9,1)
    // cube   => min=(-9,-9,0), max=(9,9,1)
    ar_t ar;
    if constexpr (std::is_same_v<coord, offset::ucoord_t>)
    {
        ar = ar_t{{1, 1, 0}, {9, 9, 1}};
    }
    else
    {
        ar = ar_t{{-9, -9, 0}, {9, 9, 1}};
    }

    Layout layout{ar};

    // area check:
    // offset => from x=1..9 => 9 steps, y=1..9 => 9 steps => area=81 => volume=162
    // cube   => from x=-9..9 => 19 steps, y=-9..9 => 19 steps => area=361 => volume=722
    if constexpr (std::is_same_v<coord, offset::ucoord_t>)
    {
        CHECK(layout.area() == 81);
        CHECK(layout.volume() == 162);
    }
    else
    {
        CHECK(layout.area() == 361);
        CHECK(layout.volume() == 722);
    }

    std::set<coord> visited{};

    // Check all coordinates
    const auto check1 = [&visited, &ar, &layout](const auto& t)
    {
        CHECK(t <= ar.max);

        // all coordinates are within the layout bounds
        CHECK(layout.is_within_bounds(t));

        // no coordinate is visited twice
        CHECK(visited.count(t) == 0);
        visited.insert(t);
    };

    for (auto&& c : layout.coordinates())
    {
        check1(c);
    }

    if constexpr (std::is_same_v<coord, offset::ucoord_t>)
    {
        CHECK(visited.size() == 162);
    }
    else
    {
        CHECK(visited.size() == 722);
    }

    visited.clear();

    layout.foreach_coordinate(check1);
    if constexpr (std::is_same_v<coord, offset::ucoord_t>)
    {
        CHECK(visited.size() == 162);
    }
    else
    {
        CHECK(visited.size() == 722);
    }

    visited.clear();

    // ground coords => z=0
    ar_t ar_ground{ar.x(), ar.y(), 0};

    const auto check2 = [&visited, &ar_ground, &layout](const auto& t)
    {
        CHECK(t.z == 0);
        CHECK(t <= ar_ground.max);

        // all coordinates are within the layout bounds
        CHECK(layout.is_within_bounds(t));

        // no coordinate is visited twice
        CHECK(visited.count(t) == 0);
        visited.insert(t);
    };

    for (auto&& c : layout.ground_coordinates())
    {
        check2(c);
    }

    if constexpr (std::is_same_v<coord, offset::ucoord_t>)
    {
        CHECK(visited.size() == 81);
    }
    else
    {
        CHECK(visited.size() == 361);
    }

    visited.clear();

    layout.foreach_ground_coordinate(check2);
    if constexpr (std::is_same_v<coord, offset::ucoord_t>)
    {
        CHECK(visited.size() == 81);
    }
    else
    {
        CHECK(visited.size() == 361);
    }

    visited.clear();

    // partial iteration from start=(2,2,0) to stop=(5,4,0)
    coord start{2, 2}, stop{5, 4};

    const auto check3 = [&visited, &start, &stop, &layout](const auto& t)
    {
        CHECK(t.z == 0);
        // iteration stays in between the bounds
        CHECK(t >= start);
        CHECK(t < stop);

        // all coordinates are within the layout bounds
        CHECK(layout.is_within_bounds(t));

        // no coordinate is visited twice
        CHECK(visited.count(t) == 0);
        visited.insert(t);
    };

    for (auto&& c : layout.coordinates(start, stop))
    {
        check3(c);
    }

    if constexpr (std::is_same_v<coord, offset::ucoord_t>)
    {
        CHECK(visited.size() == 21);
    }
    else
    {
        CHECK(visited.size() == 41);
    }

    visited.clear();

    layout.foreach_coordinate(check3, start, stop);
    if constexpr (std::is_same_v<coord, offset::ucoord_t>)
    {
        CHECK(visited.size() == 21);
    }
    else
    {
        CHECK(visited.size() == 41);
    }
}

TEMPLATE_TEST_CASE("Cartesian cardinal operations", "[cartesian-layout]", offset::ucoord_t, cube::coord_t)
{
    using Layout = cartesian_layout<TestType>;
    using coord  = typename Layout::coordinate;
    using ar_t   = typename Layout::aspect_ratio_type;

    const ar_t ar{10, 10, 1};
    Layout     layout{ar};

    // A small helper lambda that runs repeated checks on 'adjacent' coordinates
    const auto check =
        [&](const auto& t, const auto& at1, const auto& at2, const auto& border_c, const auto& border_twice)
    {
        // at1 should not be dead
        CHECK(!at1.is_dead());
        // at1 == at2
        CHECK(at1 == at2);

        // t <-> at1 are adjacent
        CHECK(layout.is_adjacent_of(t, at1));
        CHECK(layout.is_adjacent_of(at1, t));

        // also adjacent in "elevation" sense
        CHECK(layout.is_adjacent_elevation_of(t, at1));
        CHECK(layout.is_adjacent_elevation_of(at1, t));

        // border_c is at some border
        CHECK(layout.is_at_any_border(border_c));

        // border_twice is not dead
        CHECK(!border_twice.is_dead());
        // border_c == border_twice
        CHECK(border_c == border_twice);

        // border_twice is a ground coordinate
        CHECK(layout.is_ground_layer(border_twice));

        // also at some border
        CHECK(layout.is_at_any_border(border_twice));
    };

    const coord t{5, 5, 0};

    auto        nt  = layout.north(t);
    auto        net = layout.north_east(t);
    const coord bnt{5, 0, 0};

    check(t, nt, layout.north(t), bnt, layout.north(bnt));
    CHECK(layout.is_north_of(t, nt));
    CHECK(layout.is_northwards_of(t, nt));
    CHECK(layout.is_northwards_of(t, bnt));
    CHECK(layout.is_at_northern_border(bnt));
    CHECK(layout.northern_border_of(t) == bnt);
    CHECK(layout.north(bnt) == bnt);
    CHECK(net == layout.north_east(t));
    CHECK(layout.north_east(bnt) == bnt);

    const auto  et  = layout.east(t);
    const auto  set = layout.south_east(t);
    const coord bet{10, 5, 0};

    check(t, et, layout.east(t), bet, layout.east(bet));
    CHECK(layout.is_east_of(t, et));
    CHECK(layout.is_eastwards_of(t, et));
    CHECK(layout.is_eastwards_of(t, bet));
    CHECK(layout.is_at_eastern_border(bet));
    CHECK(layout.eastern_border_of(t) == bet);
    CHECK(layout.east(bet) == bet);
    CHECK(layout.south_east(t) == set);
    CHECK(layout.south_east(bet) == bet);

    const auto  st  = layout.south(t);
    const auto  swt = layout.south_west(t);
    const coord bst{5, 10, 0};

    check(t, st, layout.south(t), bst, layout.south(bst));
    CHECK(layout.is_south_of(t, st));
    CHECK(layout.is_southwards_of(t, st));
    CHECK(layout.is_southwards_of(t, bst));
    CHECK(layout.is_at_southern_border(bst));
    CHECK(layout.southern_border_of(t) == bst);
    CHECK(layout.south(bst) == bst);
    CHECK(layout.south_west(t) == swt);
    CHECK(layout.south_west(bst) == bst);

    const auto  wt  = layout.west(t);
    const auto  nwt = layout.north_west(t);
    const coord bwt{0, 5, 0};

    check(t, wt, layout.west(t), bwt, layout.west(bwt));
    CHECK(layout.is_west_of(t, wt));
    CHECK(layout.is_westwards_of(t, wt));
    CHECK(layout.is_westwards_of(t, bwt));
    CHECK(layout.is_at_western_border(bwt));
    CHECK(layout.western_border_of(t) == bwt);
    CHECK(layout.west(bwt) == bwt);
    CHECK(layout.north_west(t) == nwt);
    CHECK(layout.north_west(bwt) == bwt);

    const coord at{5, 5, 1};
    const auto  bat = layout.above(at);

    CHECK(!at.is_dead());
    CHECK(layout.is_above(t, at));
    CHECK(at == bat);
    CHECK(layout.is_crossing_layer(bat));
    CHECK(!layout.is_at_any_border(at));

    // corner case: a planar layout with z=0
    {
        const Layout planar_layout{ar_t{1, 1, 0}};
        auto         dat = planar_layout.above(coord{1, 1, 1});
        CHECK(dat.is_dead());  // out of range => dead
    }

    const auto bt  = layout.below(at);
    const auto bbt = layout.below(bt);

    CHECK(!bt.is_dead());
    CHECK(bt == t);
    CHECK(layout.is_below(at, bt));
    CHECK(!bbt.is_dead());
    CHECK(bt == bbt);
    CHECK(layout.is_ground_layer(bbt));

    const auto      v1 = layout.adjacent_coordinates({5, 5, 0});
    std::set<coord> s1(v1.begin(), v1.end());

    std::set<coord> s2{{4, 5, 0}, {5, 4, 0}, {6, 5, 0}, {5, 6, 0}};
    CHECK(s1 == s2);

    layout.foreach_adjacent_coordinate({5, 5, 0}, [&](const auto& adj) { CHECK(s2.count(adj) == 1); });
}

TEMPLATE_TEST_CASE("Cartesian cardinal operations with non-zero minimum", "[cartesian-layout]", offset::ucoord_t,
                   cube::coord_t)
{
    using Layout = cartesian_layout<TestType>;
    using coord  = typename Layout::coordinate;
    using ar_t   = typename Layout::aspect_ratio_type;

    ar_t ar;
    if constexpr (std::is_same_v<coord, offset::ucoord_t>)
    {
        ar = ar_t{{1, 1, 0}, {9, 9, 1}};
    }
    else
    {
        ar = ar_t{{-9, -9, 0}, {9, 9, 1}};
    }

    Layout layout{ar};

    const auto  mid_x = static_cast<decltype(coord::x)>((layout.x_min() + layout.x()) / 2);
    const auto  mid_y = static_cast<decltype(coord::y)>((layout.y_min() + layout.y()) / 2);
    const coord t{mid_x, mid_y, 0};

    auto cardinal_check = [&](const auto& reference, const auto& adjacent1, const auto& adjacent2, const auto& border_c,
                              const auto& border_again)
    {
        // adjacent1 should not be dead
        CHECK(!adjacent1.is_dead());
        // adjacent1 == adjacent2
        CHECK(adjacent1 == adjacent2);

        // reference <-> adjacent are adjacent
        CHECK(layout.is_adjacent_of(reference, adjacent1));
        CHECK(layout.is_adjacent_of(adjacent1, reference));

        // also adjacent in elevation sense
        CHECK(layout.is_adjacent_elevation_of(reference, adjacent1));
        CHECK(layout.is_adjacent_elevation_of(adjacent1, reference));

        // border_c is at some layout border
        CHECK(layout.is_at_any_border(border_c));

        // border_again is not dead
        CHECK(!border_again.is_dead());
        // border_c == border_again
        CHECK(border_c == border_again);

        // border_again is "ground layer"
        CHECK(layout.is_ground_layer(border_again));

        // also at a border
        CHECK(layout.is_at_any_border(border_again));
    };

    const auto nt  = layout.north(t);
    const auto net = layout.north_east(t);

    const coord nb{t.x, layout.y_min(), t.z};

    cardinal_check(t,                // reference
                   nt,               // adjacent1
                   layout.north(t),  // adjacent2
                   nb,               // border_c
                   layout.north(nb)  // border_again
    );

    if (nt != t)
    {
        CHECK(layout.is_north_of(t, nt));
        CHECK(layout.is_northwards_of(t, nt));
        CHECK(layout.is_northwards_of(t, nb));
        CHECK(layout.is_at_northern_border(nb));
        CHECK(layout.northern_border_of(t) == nb);
        CHECK(layout.north(nb) == nb);
        CHECK(net == layout.north_east(t));
        CHECK(layout.north_east(nb) == nb);
    }

    const auto  et  = layout.east(t);
    const auto  set = layout.south_east(t);
    const coord eb{layout.x(), t.y, t.z};

    cardinal_check(t, et, layout.east(t), eb, layout.east(eb));

    if (et != t)
    {
        CHECK(layout.is_east_of(t, et));
        CHECK(layout.is_eastwards_of(t, et));
        CHECK(layout.is_eastwards_of(t, eb));
        CHECK(layout.is_at_eastern_border(eb));
        CHECK(layout.eastern_border_of(t) == eb);
        CHECK(layout.east(eb) == eb);
        CHECK(layout.south_east(t) == set);
        CHECK(layout.south_east(eb) == eb);
    }

    const auto  st  = layout.south(t);
    const auto  swt = layout.south_west(t);
    const coord sb{t.x, layout.y(), t.z};

    cardinal_check(t, st, layout.south(t), sb, layout.south(sb));

    if (st != t)
    {
        CHECK(layout.is_south_of(t, st));
        CHECK(layout.is_southwards_of(t, st));
        CHECK(layout.is_southwards_of(t, sb));
        CHECK(layout.is_at_southern_border(sb));
        CHECK(layout.southern_border_of(t) == sb);
        CHECK(layout.south(sb) == sb);
        CHECK(layout.south_west(t) == swt);
        CHECK(layout.south_west(sb) == sb);
    }

    const auto  wt  = layout.west(t);
    const auto  nwt = layout.north_west(t);
    const coord wb{layout.x_min(), t.y, t.z};

    cardinal_check(t, wt, layout.west(t), wb, layout.west(wb));

    if (wt != t)
    {
        CHECK(layout.is_west_of(t, wt));
        CHECK(layout.is_westwards_of(t, wt));
        CHECK(layout.is_westwards_of(t, wb));
        CHECK(layout.is_at_western_border(wb));
        CHECK(layout.western_border_of(t) == wb);
        CHECK(layout.west(wb) == wb);
        CHECK(layout.north_west(t) == nwt);
        CHECK(layout.north_west(wb) == wb);
    }

    const coord t_above = layout.above(t);
    if (t_above != t)
    {
        CHECK(!t_above.is_dead());
        CHECK(layout.is_above(t, t_above));

        const auto t2 = layout.above(t_above);
        if (t2 != t_above)
        {
            CHECK(t2.is_dead());
        }
    }

    const coord t_below = layout.below(t_above);
    if (t_below != t_above)
    {
        CHECK(!t_below.is_dead());
        CHECK(layout.is_below(t_above, t_below));
    }

    const auto            adj_vec = layout.adjacent_coordinates(t);
    const std::set<coord> adj_set(adj_vec.begin(), adj_vec.end());

    for (const auto& a : adj_set)
    {
        REQUIRE(!a.is_dead());

        const auto dx = static_cast<int>(a.x) - static_cast<int>(t.x);
        const auto dy = static_cast<int>(a.y) - static_cast<int>(t.y);
        const auto dz = static_cast<int>(a.z) - static_cast<int>(t.z);

        CHECK(((abs(dx) == 1 && dy == 0 && dz == 0) || (abs(dy) == 1 && dx == 0 && dz == 0)));
    }

    layout.foreach_adjacent_coordinate(
        t,
        [&](const coord& ac)
        {
            const auto dx = static_cast<int>(ac.x) - static_cast<int>(t.x);
            const auto dy = static_cast<int>(ac.y) - static_cast<int>(t.y);
            const auto dz = static_cast<int>(ac.z) - static_cast<int>(t.z);

            CHECK(((abs(dx) == 1 && dy == 0 && dz == 0) || (abs(dy) == 1 && dx == 0 && dz == 0)));
        });
}

TEST_CASE("Cartesian layouts with SiQAD coordinates must have a z dimension of 1")
{
    using lyt = cartesian_layout<siqad::coord_t>;

    CHECK(lyt{aspect_ratio_t<lyt>{0, 0}}.z() == 1);
    CHECK(lyt{aspect_ratio_t<lyt>{9, 9}}.z() == 1);
    CHECK(lyt{aspect_ratio_t<lyt>{42, 42, 1}}.z() == 1);
}
