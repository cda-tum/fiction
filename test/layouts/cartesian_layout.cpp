//
// Created by marcel on 31.03.21.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/traits.hpp>

#include <set>

using namespace fiction;

TEST_CASE("Cartesian layout traits", "[cartesian-layout]")
{
    using layout = cartesian_layout<offset::ucoord_t>;

    CHECK(has_north_v<layout>);
    CHECK(has_east_v<layout>);
    CHECK(has_south_v<layout>);
    CHECK(has_west_v<layout>);
    CHECK(has_cardinal_operations_v<layout>);
    CHECK(has_north_east_v<layout>);
    CHECK(has_south_east_v<layout>);
    CHECK(has_south_west_v<layout>);
    CHECK(has_north_west_v<layout>);
    CHECK(has_ordinal_operations_v<layout>);
    CHECK(has_above_v<layout>);
    CHECK(has_below_v<layout>);
    CHECK(has_elevation_operations_v<layout>);
    CHECK(is_coordinate_layout_v<layout>);
    CHECK(is_cartesian_layout_v<layout>);
    CHECK(!is_tile_based_layout_v<layout>);
    CHECK(!is_hexagonal_layout_v<layout>);

    CHECK(has_foreach_coordinate_v<layout>);
    CHECK(has_foreach_adjacent_coordinate_v<layout>);
    CHECK(has_foreach_adjacent_opposite_coordinates_v<layout>);
}

TEST_CASE("Coordinate creation", "[cartesian-layout]")
{
    using layout = cartesian_layout<offset::ucoord_t>;

    const layout lyt{{3, 3}};

    CHECK(lyt.coord(0, 0, 0) == offset::ucoord_t{0, 0, 0});
    CHECK(lyt.coord(0, 0, 1) == offset::ucoord_t{0, 0, 1});
    CHECK(lyt.coord(1, 0) == offset::ucoord_t{1, 0});
    CHECK(lyt.coord(2, 0) == offset::ucoord_t{2, 0});
    CHECK(lyt.coord(0, 1) == offset::ucoord_t{0, 1});
    CHECK(lyt.coord(1, 1) == offset::ucoord_t{1, 1});
    CHECK(lyt.coord(2, 1) == offset::ucoord_t{2, 1});
}

TEST_CASE("Deep copy Cartesian layout", "[cartesian-layout]")
{
    const cartesian_layout original{{5, 5, 0}};

    auto copy = original.clone();

    copy.resize(aspect_ratio_t<cartesian_layout<offset::ucoord_t>>{10, 10, 1});

    CHECK(original.x() == 5);
    CHECK(original.y() == 5);
    CHECK(original.z() == 0);

    CHECK(copy.x() == 10);
    CHECK(copy.y() == 10);
    CHECK(copy.z() == 1);
}

TEST_CASE("Cartesian coordinate iteration", "[cartesian-layout]")
{
    cartesian_layout<offset::ucoord_t>::aspect_ratio ar{9, 9, 1};

    cartesian_layout layout{ar};

    std::set<cartesian_layout<offset::ucoord_t>::coordinate> visited{};

    const auto check1 = [&visited, &ar, &layout](const auto& t)
    {
        CHECK(t <= ar.end);

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

    cartesian_layout<offset::ucoord_t>::aspect_ratio ar_ground{ar.x(), ar.y(), 0};

    const auto check2 = [&visited, &ar_ground, &layout](const auto& t)
    {
        // iteration stays in ground layer
        CHECK(t.z == 0);
        CHECK(t <= ar_ground.end);

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

    cartesian_layout<offset::ucoord_t>::coordinate start{2, 2}, stop{5, 4};

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

TEST_CASE("Cartesian cardinal operations", "[cartesian-layout]")
{
    const cartesian_layout<offset::ucoord_t>::aspect_ratio ar{10, 10, 1};

    cartesian_layout layout{ar};

    const auto check = [&](const auto& t, const auto& at1, const auto& at2, const auto& b, const auto& bt)
    {
        CHECK(!at1.is_dead());
        CHECK(at1 == at2);
        CHECK(layout.is_adjacent_of(t, at1));
        CHECK(layout.is_adjacent_of(at1, t));
        CHECK(layout.is_adjacent_elevation_of(t, at1));
        CHECK(layout.is_adjacent_elevation_of(at1, t));

        CHECK(layout.is_at_any_border(b));
        CHECK(!bt.is_dead());
        CHECK(b == bt);
        CHECK(layout.is_ground_layer(bt));
        CHECK(layout.is_at_any_border(bt));
    };

    auto t = cartesian_layout<offset::ucoord_t>::coordinate{5, 5};

    auto nt  = cartesian_layout<offset::ucoord_t>::coordinate{5, 4};
    auto net = cartesian_layout<offset::ucoord_t>::coordinate{6, 4};
    auto bnt = cartesian_layout<offset::ucoord_t>::coordinate{5, 0};

    check(t, layout.north(t), nt, bnt, layout.north(bnt));
    CHECK(layout.is_north_of(t, nt));
    CHECK(layout.is_northwards_of(t, nt));
    CHECK(layout.is_northwards_of(t, bnt));
    CHECK(layout.is_at_northern_border(bnt));
    CHECK(layout.northern_border_of(t) == bnt);
    CHECK(layout.north(bnt) == bnt);
    CHECK(layout.north_east(t) == net);
    CHECK(layout.north_east(bnt) == bnt);

    auto et  = cartesian_layout<offset::ucoord_t>::coordinate{6, 5};
    auto set = cartesian_layout<offset::ucoord_t>::coordinate{6, 6};
    auto bet = cartesian_layout<offset::ucoord_t>::coordinate{10, 5};

    check(t, layout.east(t), et, bet, layout.east(bet));
    CHECK(layout.is_east_of(t, et));
    CHECK(layout.is_eastwards_of(t, et));
    CHECK(layout.is_eastwards_of(t, bet));
    CHECK(layout.is_at_eastern_border(bet));
    CHECK(layout.eastern_border_of(t) == bet);
    CHECK(layout.east(bet) == bet);
    CHECK(layout.south_east(t) == set);
    CHECK(layout.south_east(bet) == bet);

    auto st  = cartesian_layout<offset::ucoord_t>::coordinate{5, 6};
    auto swt = cartesian_layout<offset::ucoord_t>::coordinate{4, 6};
    auto bst = cartesian_layout<offset::ucoord_t>::coordinate{5, 10};

    check(t, layout.south(t), st, bst, layout.south(bst));
    CHECK(layout.is_south_of(t, st));
    CHECK(layout.is_southwards_of(t, st));
    CHECK(layout.is_southwards_of(t, bst));
    CHECK(layout.is_at_southern_border(bst));
    CHECK(layout.southern_border_of(t) == bst);
    CHECK(layout.south(bst) == bst);
    CHECK(layout.south_west(t) == swt);
    CHECK(layout.south_west(bst) == bst);

    auto wt  = cartesian_layout<offset::ucoord_t>::coordinate{4, 5};
    auto nwt = cartesian_layout<offset::ucoord_t>::coordinate{4, 4};
    auto bwt = cartesian_layout<offset::ucoord_t>::coordinate{0, 5};

    check(t, layout.west(t), wt, bwt, layout.west(bwt));
    CHECK(layout.is_west_of(t, wt));
    CHECK(layout.is_westwards_of(t, wt));
    CHECK(layout.is_westwards_of(t, bwt));
    CHECK(layout.is_at_western_border(bwt));
    CHECK(layout.western_border_of(t) == bwt);
    CHECK(layout.west(bwt) == bwt);
    CHECK(layout.north_west(t) == nwt);
    CHECK(layout.north_west(bwt) == bwt);

    auto at  = cartesian_layout<offset::ucoord_t>::coordinate{5, 5, 1};
    auto bat = layout.above(at);

    CHECK(!at.is_dead());
    CHECK(layout.is_above(t, at));
    CHECK(at == bat);
    CHECK(layout.is_crossing_layer(bat));
    CHECK(!layout.is_at_any_border(at));

    // cover corner case
    const cartesian_layout<offset::ucoord_t> planar_layout{{1, 1, 0}};

    auto dat = planar_layout.above({1, 1, 1});
    CHECK(dat.is_dead());

    auto bt  = layout.below(at);
    auto bbt = layout.below(bt);

    CHECK(!bt.is_dead());
    CHECK(bt == t);
    CHECK(layout.is_below(at, bt));
    CHECK(!bbt.is_dead());
    CHECK(bt == bbt);
    CHECK(layout.is_ground_layer(bbt));

    const auto v1 = layout.adjacent_coordinates({5, 5});
    const auto s1 = std::set<cartesian_layout<offset::ucoord_t>::coordinate>{v1.cbegin(), v1.cend()};
    const auto s2 = std::set<cartesian_layout<offset::ucoord_t>::coordinate>{{{4, 5}, {5, 4}, {6, 5}, {5, 6}}};

    CHECK(s1 == s2);

    layout.foreach_adjacent_coordinate(
        {5, 5},
        [](const auto& adj)
        {
            CHECK(
                std::set<cartesian_layout<offset::ucoord_t>::coordinate>{{{4, 5}, {5, 4}, {6, 5}, {5, 6}}}.count(adj));
        });
}

TEST_CASE("Cartesian layouts with SiQAD coordinates must have a z dimension of 1")
{
    using lyt = cartesian_layout<siqad::coord_t>;

    CHECK(lyt{aspect_ratio_t<lyt>{0, 0}}.z() == 1);
    CHECK(lyt{aspect_ratio_t<lyt>{9, 9}}.z() == 1);
    CHECK(lyt{aspect_ratio_t<lyt>{42, 42, 1}}.z() == 1);
}
