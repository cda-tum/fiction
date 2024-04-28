//
// Created by marcel on 15.09.21.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/traits.hpp>

#include <iostream>
#include <set>

using namespace fiction;

template <typename Lyt>
void check_common_traits()
{
    CHECK(has_north_v<Lyt>);
    CHECK(has_east_v<Lyt>);
    CHECK(has_south_v<Lyt>);
    CHECK(has_west_v<Lyt>);
    CHECK(has_cardinal_operations_v<Lyt>);
    CHECK(has_north_east_v<Lyt>);
    CHECK(has_south_east_v<Lyt>);
    CHECK(has_south_west_v<Lyt>);
    CHECK(has_north_west_v<Lyt>);
    CHECK(has_ordinal_operations_v<Lyt>);
    CHECK(has_above_v<Lyt>);
    CHECK(has_below_v<Lyt>);
    CHECK(has_elevation_operations_v<Lyt>);
    CHECK(is_coordinate_layout_v<Lyt>);
    CHECK(!is_tile_based_layout_v<Lyt>);
    CHECK(!is_cartesian_layout_v<Lyt>);
    CHECK(is_hexagonal_layout_v<Lyt>);

    CHECK(has_foreach_coordinate_v<Lyt>);
    CHECK(has_foreach_adjacent_coordinate_v<Lyt>);
    CHECK(has_foreach_adjacent_opposite_coordinates_v<Lyt>);
}

TEST_CASE("Hexagonal layout traits", "[hexagonal-layout]")
{
    SECTION("odd row")
    {
        using layout = hexagonal_layout<offset::ucoord_t, odd_row_hex>;

        CHECK(has_pointy_top_hex_orientation_v<layout>);
        CHECK(!has_flat_top_hex_orientation_v<layout>);
        CHECK(has_odd_row_hex_arrangement_v<layout>);
        CHECK(!has_even_row_hex_arrangement_v<layout>);
        CHECK(!has_odd_column_hex_arrangement_v<layout>);
        CHECK(!has_even_column_hex_arrangement_v<layout>);

        check_common_traits<layout>();
    }
    SECTION("even row")
    {
        using layout = hexagonal_layout<offset::ucoord_t, even_row_hex>;

        CHECK(has_pointy_top_hex_orientation_v<layout>);
        CHECK(!has_flat_top_hex_orientation_v<layout>);
        CHECK(!has_odd_row_hex_arrangement_v<layout>);
        CHECK(has_even_row_hex_arrangement_v<layout>);
        CHECK(!has_odd_column_hex_arrangement_v<layout>);
        CHECK(!has_even_column_hex_arrangement_v<layout>);

        check_common_traits<layout>();
    }
    SECTION("odd column")
    {
        using layout = hexagonal_layout<offset::ucoord_t, odd_column_hex>;

        CHECK(!has_pointy_top_hex_orientation_v<layout>);
        CHECK(has_flat_top_hex_orientation_v<layout>);
        CHECK(!has_odd_row_hex_arrangement_v<layout>);
        CHECK(!has_even_row_hex_arrangement_v<layout>);
        CHECK(has_odd_column_hex_arrangement_v<layout>);
        CHECK(!has_even_column_hex_arrangement_v<layout>);

        check_common_traits<layout>();
    }
    SECTION("even column")
    {
        using layout = hexagonal_layout<offset::ucoord_t, even_column_hex>;

        CHECK(!has_pointy_top_hex_orientation_v<layout>);
        CHECK(has_flat_top_hex_orientation_v<layout>);
        CHECK(!has_odd_row_hex_arrangement_v<layout>);
        CHECK(!has_even_row_hex_arrangement_v<layout>);
        CHECK(!has_odd_column_hex_arrangement_v<layout>);
        CHECK(has_even_column_hex_arrangement_v<layout>);

        check_common_traits<layout>();
    }
}

TEST_CASE("Deep copy hexagonal layout", "[hexagonal-layout]")
{
    const hexagonal_layout original{{5, 5, 0}};

    auto copy = original.clone();

    copy.resize({10, 10, 1});

    CHECK(original.x() == 5);
    CHECK(original.y() == 5);
    CHECK(original.z() == 0);

    CHECK(copy.x() == 10);
    CHECK(copy.y() == 10);
    CHECK(copy.z() == 1);
}

template <typename Lyt>
void check_identity_conversion()
{
    Lyt layout{aspect_ratio<Lyt>{10, 10}};

    layout.foreach_coordinate([&layout](const auto& coord)
                              { CHECK(layout.to_offset_coordinate(layout.to_cube_coordinate(coord)) == coord); });
}

TEST_CASE("Coordinate creation", "[hexagonal-layout]")
{
    SECTION("odd row")
    {
        using layout = hexagonal_layout<offset::ucoord_t, odd_row_hex>;

        const layout lyt{{3, 3}};

        CHECK(lyt.coord(0, 0, 0) == offset::ucoord_t{0, 0, 0});
        CHECK(lyt.coord(0, 0, 1) == offset::ucoord_t{0, 0, 1});
        CHECK(lyt.coord(1, 0) == offset::ucoord_t{1, 0});
        CHECK(lyt.coord(2, 0) == offset::ucoord_t{2, 0});
        CHECK(lyt.coord(0, 1) == offset::ucoord_t{0, 1});
        CHECK(lyt.coord(1, 1) == offset::ucoord_t{1, 1});
        CHECK(lyt.coord(2, 1) == offset::ucoord_t{2, 1});
    }
    SECTION("even row")
    {
        using layout = hexagonal_layout<offset::ucoord_t, even_row_hex>;

        const layout lyt{{3, 3}};

        CHECK(lyt.coord(0, 0, 0) == offset::ucoord_t{0, 0, 0});
        CHECK(lyt.coord(0, 0, 1) == offset::ucoord_t{0, 0, 1});
        CHECK(lyt.coord(1, 0) == offset::ucoord_t{1, 0});
        CHECK(lyt.coord(2, 0) == offset::ucoord_t{2, 0});
        CHECK(lyt.coord(0, 1) == offset::ucoord_t{0, 1});
        CHECK(lyt.coord(1, 1) == offset::ucoord_t{1, 1});
        CHECK(lyt.coord(2, 1) == offset::ucoord_t{2, 1});
    }
    SECTION("odd column")
    {
        using layout = hexagonal_layout<offset::ucoord_t, odd_column_hex>;

        const layout lyt{{3, 3}};

        CHECK(lyt.coord(0, 0, 0) == offset::ucoord_t{0, 0, 0});
        CHECK(lyt.coord(0, 0, 1) == offset::ucoord_t{0, 0, 1});
        CHECK(lyt.coord(1, 0) == offset::ucoord_t{1, 0});
        CHECK(lyt.coord(2, 0) == offset::ucoord_t{2, 0});
        CHECK(lyt.coord(0, 1) == offset::ucoord_t{0, 1});
        CHECK(lyt.coord(1, 1) == offset::ucoord_t{1, 1});
        CHECK(lyt.coord(2, 1) == offset::ucoord_t{2, 1});
    }
    SECTION("even column")
    {
        using layout = hexagonal_layout<offset::ucoord_t, even_column_hex>;

        const layout lyt{{3, 3}};

        CHECK(lyt.coord(0, 0, 0) == offset::ucoord_t{0, 0, 0});
        CHECK(lyt.coord(0, 0, 1) == offset::ucoord_t{0, 0, 1});
        CHECK(lyt.coord(1, 0) == offset::ucoord_t{1, 0});
        CHECK(lyt.coord(2, 0) == offset::ucoord_t{2, 0});
        CHECK(lyt.coord(0, 1) == offset::ucoord_t{0, 1});
        CHECK(lyt.coord(1, 1) == offset::ucoord_t{1, 1});
        CHECK(lyt.coord(2, 1) == offset::ucoord_t{2, 1});
    }
}

TEST_CASE("Coordinate conversions", "[hexagonal-layout]")
{
    SECTION("odd row")
    {
        using layout = hexagonal_layout<offset::ucoord_t, odd_row_hex>;
        check_identity_conversion<layout>();

        const layout lyt{{3, 3}};

        CHECK(lyt.to_cube_coordinate({0, 0}) == typename layout::cube_coordinate{0, 0, 0});
        CHECK(lyt.to_cube_coordinate({1, 0}) == typename layout::cube_coordinate{+1, -1, 0});
        CHECK(lyt.to_cube_coordinate({2, 0}) == typename layout::cube_coordinate{+2, -2, 0});
        CHECK(lyt.to_cube_coordinate({0, 1}) == typename layout::cube_coordinate{0, -1, +1});
        CHECK(lyt.to_cube_coordinate({1, 1}) == typename layout::cube_coordinate{+1, -2, +1});
        CHECK(lyt.to_cube_coordinate({2, 1}) == typename layout::cube_coordinate{+2, -3, +1});
    }
    SECTION("even row")
    {
        using layout = hexagonal_layout<offset::ucoord_t, even_row_hex>;
        check_identity_conversion<layout>();

        const layout lyt{{3, 3}};

        CHECK(lyt.to_cube_coordinate({0, 0}) == typename layout::cube_coordinate{0, 0, 0});
        CHECK(lyt.to_cube_coordinate({1, 0}) == typename layout::cube_coordinate{+1, -1, 0});
        CHECK(lyt.to_cube_coordinate({2, 0}) == typename layout::cube_coordinate{+2, -2, 0});
        CHECK(lyt.to_cube_coordinate({0, 1}) == typename layout::cube_coordinate{-1, 0, +1});
        CHECK(lyt.to_cube_coordinate({1, 1}) == typename layout::cube_coordinate{0, -1, +1});
        CHECK(lyt.to_cube_coordinate({2, 1}) == typename layout::cube_coordinate{+1, -2, +1});
    }
    SECTION("odd column")
    {
        using layout = hexagonal_layout<offset::ucoord_t, odd_column_hex>;
        check_identity_conversion<layout>();

        const layout lyt{{3, 3}};

        CHECK(lyt.to_cube_coordinate({0, 0}) == typename layout::cube_coordinate{0, 0, 0});
        CHECK(lyt.to_cube_coordinate({1, 0}) == typename layout::cube_coordinate{+1, -1, 0});
        CHECK(lyt.to_cube_coordinate({2, 0}) == typename layout::cube_coordinate{+2, -1, -1});
        CHECK(lyt.to_cube_coordinate({0, 1}) == typename layout::cube_coordinate{0, -1, +1});
        CHECK(lyt.to_cube_coordinate({1, 1}) == typename layout::cube_coordinate{+1, -2, +1});
        CHECK(lyt.to_cube_coordinate({2, 1}) == typename layout::cube_coordinate{+2, -2, 0});
    }
    SECTION("even column")
    {
        using layout = hexagonal_layout<offset::ucoord_t, even_column_hex>;
        check_identity_conversion<layout>();

        const layout lyt{{3, 3}};

        CHECK(lyt.to_cube_coordinate({0, 0}) == typename layout::cube_coordinate{0, 0, 0});
        CHECK(lyt.to_cube_coordinate({1, 0}) == typename layout::cube_coordinate{+1, 0, -1});
        CHECK(lyt.to_cube_coordinate({2, 0}) == typename layout::cube_coordinate{+2, -1, -1});
        CHECK(lyt.to_cube_coordinate({0, 1}) == typename layout::cube_coordinate{0, -1, +1});
        CHECK(lyt.to_cube_coordinate({1, 1}) == typename layout::cube_coordinate{+1, -1, 0});
        CHECK(lyt.to_cube_coordinate({2, 1}) == typename layout::cube_coordinate{+2, -2, 0});
    }
}

template <typename Lyt>
void check_visited_coordinates()
{
    aspect_ratio<Lyt> ar{9, 9, 1};

    Lyt layout{ar};

    std::set<coordinate<Lyt>> visited{};

    const auto check1 = [&visited, &ar, &layout](const auto& c)
    {
        CHECK(c <= ar);

        // all coordinates are within the layout bounds
        CHECK(layout.is_within_bounds(c));

        // no coordinate is visited twice
        CHECK(visited.count(c) == 0);
        visited.insert(c);
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

    aspect_ratio<Lyt> ar_ground{ar.x, ar.y, 0};

    const auto check2 = [&visited, &ar_ground, &layout](const auto& c)
    {
        // iteration stays in ground layer
        CHECK(c.z == 0);
        CHECK(c <= ar_ground);
        CHECK(layout.is_ground_layer(c));

        // all coordinates are within the layout bounds
        CHECK(layout.is_within_bounds(c));

        // no coordinate is visited twice
        CHECK(visited.count(c) == 0);
        visited.insert(c);
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

    coordinate<Lyt> start{2, 2}, stop{5, 4};

    const auto check3 = [&visited, &start, &stop, &layout](const auto& c)
    {
        CHECK(c.z == 0);
        // iteration stays in between the bounds
        CHECK(c >= start);
        CHECK(c < stop);

        // all coordinates are within the layout bounds
        CHECK(layout.is_within_bounds(c));

        // no coordinate is visited twice
        CHECK(visited.count(c) == 0);
        visited.insert(c);
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

TEST_CASE("Hexagonal coordinate iteration", "[hexagonal-layout]")
{
    using odd_row_layout     = hexagonal_layout<offset::ucoord_t, odd_row_hex>;
    using even_row_layout    = hexagonal_layout<offset::ucoord_t, even_row_hex>;
    using odd_column_layout  = hexagonal_layout<offset::ucoord_t, odd_column_hex>;
    using even_column_layout = hexagonal_layout<offset::ucoord_t, even_column_hex>;

    check_visited_coordinates<odd_row_layout>();
    check_visited_coordinates<even_row_layout>();
    check_visited_coordinates<odd_column_layout>();
    check_visited_coordinates<even_column_layout>();
}

TEST_CASE("Cardinal and ordinal operations", "[hexagonal-layout]")
{
    SECTION("odd row")
    {
        using layout = hexagonal_layout<offset::ucoord_t, odd_row_hex>;

        layout lyt{{3, 3, 1}};

        const coordinate<layout> c{2, 2};
        const coordinate<layout> ac{2, 2, 1};

        const coordinate<layout> nc{2, 1};
        const coordinate<layout> nec{2, 1};
        const coordinate<layout> ec{3, 2};
        const coordinate<layout> sec{2, 3};
        const coordinate<layout> sc{2, 3};
        const coordinate<layout> swc{1, 3};
        const coordinate<layout> wc{1, 2};
        const coordinate<layout> nwc{1, 1};

        const coordinate<layout> bnc{2, 0};
        const coordinate<layout> bnec{3, 0};
        const coordinate<layout> bec{3, 2};
        const coordinate<layout> bsec{3, 3};
        const coordinate<layout> bsc{2, 3};
        const coordinate<layout> bswc{0, 3};
        const coordinate<layout> bwc{0, 2};
        const coordinate<layout> bnwc{0, 0};

        CHECK(lyt.is_above(c, ac));
        CHECK(lyt.is_below(ac, c));

        CHECK(lyt.north(c) == nc);
        CHECK(lyt.is_north_of(c, nc));
        CHECK(lyt.is_northwards_of(c, bnc));
        CHECK(lyt.northern_border_of(c) == bnc);
        CHECK(lyt.north_east(c) == nec);
        CHECK(lyt.east(c) == ec);
        CHECK(lyt.is_east_of(c, ec));
        CHECK(lyt.is_eastwards_of(c, bec));
        CHECK(lyt.eastern_border_of(c) == bec);
        CHECK(lyt.south_east(c) == sec);
        CHECK(lyt.south(c) == sc);
        CHECK(lyt.is_south_of(c, sc));
        CHECK(lyt.is_southwards_of(c, bsc));
        CHECK(lyt.southern_border_of(c) == bsc);
        CHECK(lyt.south_west(c) == swc);
        CHECK(lyt.west(c) == wc);
        CHECK(lyt.is_west_of(c, wc));
        CHECK(lyt.is_westwards_of(c, bwc));
        CHECK(lyt.western_border_of(c) == bwc);
        CHECK(lyt.north_west(c) == nwc);

        CHECK(lyt.is_adjacent_of(c, nc));
        CHECK(lyt.is_adjacent_of(c, nec));
        CHECK(lyt.is_adjacent_of(c, ec));
        CHECK(lyt.is_adjacent_of(c, sec));
        CHECK(lyt.is_adjacent_of(c, sc));
        CHECK(lyt.is_adjacent_of(c, swc));
        CHECK(lyt.is_adjacent_of(c, wc));
        CHECK(lyt.is_adjacent_of(c, nwc));

        // edge cases
        CHECK(lyt.north(bnc) == bnc);
        CHECK(lyt.north_east(bnec) == bnec);
        CHECK(lyt.east(bec) == bec);
        CHECK(lyt.south_east(bsec) == bsec);
        CHECK(lyt.south(bsc) == bsc);
        CHECK(lyt.south_west(bswc) == bswc);
        CHECK(lyt.west(bwc) == bwc);
        CHECK(lyt.north_west(bnwc) == bnwc);
    }
    SECTION("even row")
    {
        using layout = hexagonal_layout<offset::ucoord_t, even_row_hex>;

        layout lyt{{3, 3, 1}};

        const coordinate<layout> c{2, 2};
        const coordinate<layout> ac{2, 2, 1};

        const coordinate<layout> nc{2, 1};
        const coordinate<layout> nec{3, 1};
        const coordinate<layout> ec{3, 2};
        const coordinate<layout> sec{3, 3};
        const coordinate<layout> sc{2, 3};
        const coordinate<layout> swc{2, 3};
        const coordinate<layout> wc{1, 2};
        const coordinate<layout> nwc{2, 1};

        const coordinate<layout> bnc{2, 0};
        const coordinate<layout> bnec{3, 0};
        const coordinate<layout> bec{3, 2};
        const coordinate<layout> bsec{3, 3};
        const coordinate<layout> bsc{2, 3};
        const coordinate<layout> bswc{0, 3};
        const coordinate<layout> bwc{0, 2};
        const coordinate<layout> bnwc{0, 0};

        CHECK(lyt.is_above(c, ac));
        CHECK(lyt.is_below(ac, c));

        CHECK(lyt.north(c) == nc);
        CHECK(lyt.is_north_of(c, nc));
        CHECK(lyt.is_northwards_of(c, bnc));
        CHECK(lyt.northern_border_of(c) == bnc);
        CHECK(lyt.north_east(c) == nec);
        CHECK(lyt.east(c) == ec);
        CHECK(lyt.is_east_of(c, ec));
        CHECK(lyt.is_eastwards_of(c, bec));
        CHECK(lyt.eastern_border_of(c) == bec);
        CHECK(lyt.south_east(c) == sec);
        CHECK(lyt.south(c) == sc);
        CHECK(lyt.is_south_of(c, sc));
        CHECK(lyt.is_southwards_of(c, bsc));
        CHECK(lyt.southern_border_of(c) == bsc);
        CHECK(lyt.south_west(c) == swc);
        CHECK(lyt.west(c) == wc);
        CHECK(lyt.is_west_of(c, wc));
        CHECK(lyt.is_westwards_of(c, bwc));
        CHECK(lyt.western_border_of(c) == bwc);
        CHECK(lyt.north_west(c) == nwc);

        CHECK(lyt.is_adjacent_of(c, nc));
        CHECK(lyt.is_adjacent_of(c, nec));
        CHECK(lyt.is_adjacent_of(c, ec));
        CHECK(lyt.is_adjacent_of(c, sec));
        CHECK(lyt.is_adjacent_of(c, sc));
        CHECK(lyt.is_adjacent_of(c, swc));
        CHECK(lyt.is_adjacent_of(c, wc));
        CHECK(lyt.is_adjacent_of(c, nwc));

        // edge cases
        CHECK(lyt.north(bnc) == bnc);
        CHECK(lyt.north_east(bnec) == bnec);
        CHECK(lyt.east(bec) == bec);
        CHECK(lyt.south_east(bsec) == bsec);
        CHECK(lyt.south(bsc) == bsc);
        CHECK(lyt.south_west(bswc) == bswc);
        CHECK(lyt.west(bwc) == bwc);
        CHECK(lyt.north_west(bnwc) == bnwc);
    }
    SECTION("odd column")
    {
        using layout = hexagonal_layout<offset::ucoord_t, odd_column_hex>;

        layout lyt{{3, 3, 1}};

        const coordinate<layout> c{2, 2};
        const coordinate<layout> ac{2, 2, 1};

        const coordinate<layout> nc{2, 1};
        const coordinate<layout> nec{3, 1};
        const coordinate<layout> ec{3, 2};
        const coordinate<layout> sec{3, 2};
        const coordinate<layout> sc{2, 3};
        const coordinate<layout> swc{1, 2};
        const coordinate<layout> wc{1, 2};
        const coordinate<layout> nwc{1, 1};

        const coordinate<layout> bnc{2, 0};
        const coordinate<layout> bnec{3, 0};
        const coordinate<layout> bec{3, 2};
        const coordinate<layout> bsec{3, 3};
        const coordinate<layout> bsc{2, 3};
        const coordinate<layout> bswc{0, 3};
        const coordinate<layout> bwc{0, 2};
        const coordinate<layout> bnwc{0, 0};

        CHECK(lyt.is_above(c, ac));
        CHECK(lyt.is_below(ac, c));

        CHECK(lyt.north(c) == nc);
        CHECK(lyt.is_north_of(c, nc));
        CHECK(lyt.is_northwards_of(c, bnc));
        CHECK(lyt.northern_border_of(c) == bnc);
        CHECK(lyt.north_east(c) == nec);
        CHECK(lyt.east(c) == ec);
        CHECK(lyt.is_east_of(c, ec));
        CHECK(lyt.is_eastwards_of(c, bec));
        CHECK(lyt.eastern_border_of(c) == bec);
        CHECK(lyt.south_east(c) == sec);
        CHECK(lyt.south(c) == sc);
        CHECK(lyt.is_south_of(c, sc));
        CHECK(lyt.is_southwards_of(c, bsc));
        CHECK(lyt.southern_border_of(c) == bsc);
        CHECK(lyt.south_west(c) == swc);
        CHECK(lyt.west(c) == wc);
        CHECK(lyt.is_west_of(c, wc));
        CHECK(lyt.is_westwards_of(c, bwc));
        CHECK(lyt.western_border_of(c) == bwc);
        CHECK(lyt.north_west(c) == nwc);

        CHECK(lyt.is_adjacent_of(c, nc));
        CHECK(lyt.is_adjacent_of(c, nec));
        CHECK(lyt.is_adjacent_of(c, ec));
        CHECK(lyt.is_adjacent_of(c, sec));
        CHECK(lyt.is_adjacent_of(c, sc));
        CHECK(lyt.is_adjacent_of(c, swc));
        CHECK(lyt.is_adjacent_of(c, wc));
        CHECK(lyt.is_adjacent_of(c, nwc));

        // edge cases
        CHECK(lyt.north(bnc) == bnc);
        CHECK(lyt.north_east(bnec) == bnec);
        CHECK(lyt.east(bec) == bec);
        CHECK(lyt.south_east(bsec) == bsec);
        CHECK(lyt.south(bsc) == bsc);
        CHECK(lyt.south_west(bswc) == bswc);
        CHECK(lyt.west(bwc) == bwc);
        CHECK(lyt.north_west(bnwc) == bnwc);
    }
    SECTION("even column")
    {
        using layout = hexagonal_layout<offset::ucoord_t, even_column_hex>;

        layout lyt{{3, 3, 1}};

        const coordinate<layout> c{2, 2};
        const coordinate<layout> ac{2, 2, 1};

        const coordinate<layout> nc{2, 1};
        const coordinate<layout> nec{3, 2};
        const coordinate<layout> ec{3, 2};
        const coordinate<layout> sec{3, 3};
        const coordinate<layout> sc{2, 3};
        const coordinate<layout> swc{1, 3};
        const coordinate<layout> wc{1, 2};
        const coordinate<layout> nwc{1, 2};

        const coordinate<layout> bnc{2, 0};
        const coordinate<layout> bnec{3, 0};
        const coordinate<layout> bec{3, 2};
        const coordinate<layout> bsec{3, 3};
        const coordinate<layout> bsc{2, 3};
        const coordinate<layout> bswc{0, 3};
        const coordinate<layout> bwc{0, 2};
        const coordinate<layout> bnwc{0, 0};

        CHECK(lyt.is_above(c, ac));
        CHECK(lyt.is_below(ac, c));

        CHECK(lyt.north(c) == nc);
        CHECK(lyt.is_north_of(c, nc));
        CHECK(lyt.is_northwards_of(c, bnc));
        CHECK(lyt.northern_border_of(c) == bnc);
        CHECK(lyt.north_east(c) == nec);
        CHECK(lyt.east(c) == ec);
        CHECK(lyt.is_east_of(c, ec));
        CHECK(lyt.is_eastwards_of(c, bec));
        CHECK(lyt.eastern_border_of(c) == bec);
        CHECK(lyt.south_east(c) == sec);
        CHECK(lyt.south(c) == sc);
        CHECK(lyt.is_south_of(c, sc));
        CHECK(lyt.is_southwards_of(c, bsc));
        CHECK(lyt.southern_border_of(c) == bsc);
        CHECK(lyt.south_west(c) == swc);
        CHECK(lyt.west(c) == wc);
        CHECK(lyt.is_west_of(c, wc));
        CHECK(lyt.is_westwards_of(c, bwc));
        CHECK(lyt.western_border_of(c) == bwc);
        CHECK(lyt.north_west(c) == nwc);

        CHECK(lyt.is_adjacent_of(c, nc));
        CHECK(lyt.is_adjacent_of(c, nec));
        CHECK(lyt.is_adjacent_of(c, ec));
        CHECK(lyt.is_adjacent_of(c, sec));
        CHECK(lyt.is_adjacent_of(c, sc));
        CHECK(lyt.is_adjacent_of(c, swc));
        CHECK(lyt.is_adjacent_of(c, wc));
        CHECK(lyt.is_adjacent_of(c, nwc));

        // edge cases
        CHECK(lyt.north(bnc) == bnc);
        CHECK(lyt.north_east(bnec) == bnec);
        CHECK(lyt.east(bec) == bec);
        CHECK(lyt.south_east(bsec) == bsec);
        CHECK(lyt.south(bsc) == bsc);
        CHECK(lyt.south_west(bswc) == bswc);
        CHECK(lyt.west(bwc) == bwc);
        CHECK(lyt.north_west(bnwc) == bnwc);
    }
}

TEST_CASE("Coordinate adjacencies", "[hexagonal-layout]")
{
    SECTION("odd row")
    {
        using layout = hexagonal_layout<offset::ucoord_t, odd_row_hex>;

        layout lyt{{2, 2}};

        const auto adj00_v = lyt.adjacent_coordinates({0, 0});
        const auto adj00_s = std::set<coordinate<layout>>{adj00_v.cbegin(), adj00_v.cend()};

        CHECK(std::set<coordinate<layout>>{{{0, 1}, {1, 0}}} == adj00_s);

        const auto adj01_v = lyt.adjacent_coordinates({0, 1});
        const auto adj01_s = std::set<coordinate<layout>>{adj01_v.cbegin(), adj01_v.cend()};

        CHECK(std::set<coordinate<layout>>{{{0, 0}, {1, 0}, {1, 1}, {0, 2}, {1, 2}}} == adj01_s);

        const auto adj22_v = lyt.adjacent_coordinates({2, 2});
        const auto adj22_s = std::set<coordinate<layout>>{adj22_v.cbegin(), adj22_v.cend()};

        CHECK(std::set<coordinate<layout>>{{{1, 2}, {1, 1}, {2, 1}}} == adj22_s);
    }
    SECTION("even row")
    {
        using layout = hexagonal_layout<offset::ucoord_t, even_row_hex>;

        layout lyt{{2, 2}};

        const auto adj00_v = lyt.adjacent_coordinates({0, 0});
        const auto adj00_s = std::set<coordinate<layout>>{adj00_v.cbegin(), adj00_v.cend()};

        CHECK(std::set<coordinate<layout>>{{{0, 1}, {1, 0}, {1, 1}}} == adj00_s);

        const auto adj01_v = lyt.adjacent_coordinates({0, 1});
        const auto adj01_s = std::set<coordinate<layout>>{adj01_v.cbegin(), adj01_v.cend()};

        CHECK(std::set<coordinate<layout>>{{{0, 0}, {1, 1}, {0, 2}}} == adj01_s);

        const auto adj22_v = lyt.adjacent_coordinates({2, 2});
        const auto adj22_s = std::set<coordinate<layout>>{adj22_v.cbegin(), adj22_v.cend()};

        CHECK(std::set<coordinate<layout>>{{{1, 2}, {2, 1}}} == adj22_s);
    }
    SECTION("odd column")
    {
        using layout = hexagonal_layout<offset::ucoord_t, odd_column_hex>;

        layout lyt{{2, 2}};

        const auto adj00_v = lyt.adjacent_coordinates({0, 0});
        const auto adj00_s = std::set<coordinate<layout>>{adj00_v.cbegin(), adj00_v.cend()};

        CHECK(std::set<coordinate<layout>>{{{0, 1}, {1, 0}}} == adj00_s);

        const auto adj01_v = lyt.adjacent_coordinates({0, 1});
        const auto adj01_s = std::set<coordinate<layout>>{adj01_v.cbegin(), adj01_v.cend()};

        CHECK(std::set<coordinate<layout>>{{{0, 0}, {1, 0}, {1, 1}, {0, 2}}} == adj01_s);

        const auto adj22_v = lyt.adjacent_coordinates({2, 2});
        const auto adj22_s = std::set<coordinate<layout>>{adj22_v.cbegin(), adj22_v.cend()};

        CHECK(std::set<coordinate<layout>>{{{1, 2}, {1, 1}, {2, 1}}} == adj22_s);
    }
    SECTION("even column")
    {
        using layout = hexagonal_layout<offset::ucoord_t, even_column_hex>;

        layout lyt{{2, 2}};

        const auto adj00_v = lyt.adjacent_coordinates({0, 0});
        const auto adj00_s = std::set<coordinate<layout>>{adj00_v.cbegin(), adj00_v.cend()};

        CHECK(std::set<coordinate<layout>>{{{0, 1}, {1, 0}, {1, 1}}} == adj00_s);

        const auto adj01_v = lyt.adjacent_coordinates({0, 1});
        const auto adj01_s = std::set<coordinate<layout>>{adj01_v.cbegin(), adj01_v.cend()};

        CHECK(std::set<coordinate<layout>>{{{0, 0}, {1, 1}, {0, 2}, {1, 2}}} == adj01_s);

        const auto adj22_v = lyt.adjacent_coordinates({2, 2});
        const auto adj22_s = std::set<coordinate<layout>>{adj22_v.cbegin(), adj22_v.cend()};

        CHECK(std::set<coordinate<layout>>{{{1, 2}, {2, 1}}} == adj22_s);
    }
}
