//
// Created by marcel on 15.09.21.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/shifted_cartesian_layout.hpp>
#include <fiction/traits.hpp>

#include <iostream>

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
    CHECK(!is_hexagonal_layout_v<Lyt>);
    CHECK(is_shifted_cartesian_layout_v<Lyt>);

    CHECK(has_foreach_coordinate_v<Lyt>);
    CHECK(has_foreach_adjacent_coordinate_v<Lyt>);
    CHECK(has_foreach_adjacent_opposite_coordinates_v<Lyt>);
}

// traits is the only thing that needs to be checked because for all other purposes,
// shifted_cartesian_layout is a hexagonal_layout
TEST_CASE("Shifted Cartesian layout traits", "[shifted-cartesian-layout]")
{
    SECTION("odd row")
    {
        using layout = shifted_cartesian_layout<offset::ucoord_t, odd_row_cartesian>;

        CHECK(has_horizontally_shifted_cartesian_orientation_v<layout>);
        CHECK(!has_vertically_shifted_cartesian_orientation_v<layout>);
        CHECK(has_odd_row_cartesian_arrangement_v<layout>);
        CHECK(!has_even_row_cartesian_arrangement_v<layout>);
        CHECK(!has_odd_column_cartesian_arrangement_v<layout>);
        CHECK(!has_even_column_cartesian_arrangement_v<layout>);

        check_common_traits<layout>();
    }
    SECTION("even row")
    {
        using layout = shifted_cartesian_layout<offset::ucoord_t, even_row_cartesian>;

        CHECK(has_horizontally_shifted_cartesian_orientation_v<layout>);
        CHECK(!has_vertically_shifted_cartesian_orientation_v<layout>);
        CHECK(!has_odd_row_cartesian_arrangement_v<layout>);
        CHECK(has_even_row_cartesian_arrangement_v<layout>);
        CHECK(!has_odd_column_cartesian_arrangement_v<layout>);
        CHECK(!has_even_column_cartesian_arrangement_v<layout>);

        check_common_traits<layout>();
    }
    SECTION("odd column")
    {
        using layout = shifted_cartesian_layout<offset::ucoord_t, odd_column_cartesian>;

        CHECK(!has_horizontally_shifted_cartesian_orientation_v<layout>);
        CHECK(has_vertically_shifted_cartesian_orientation_v<layout>);
        CHECK(!has_odd_row_cartesian_arrangement_v<layout>);
        CHECK(!has_even_row_cartesian_arrangement_v<layout>);
        CHECK(has_odd_column_cartesian_arrangement_v<layout>);
        CHECK(!has_even_column_cartesian_arrangement_v<layout>);

        check_common_traits<layout>();
    }
    SECTION("even column")
    {
        using layout = shifted_cartesian_layout<offset::ucoord_t, even_column_cartesian>;

        CHECK(!has_horizontally_shifted_cartesian_orientation_v<layout>);
        CHECK(has_vertically_shifted_cartesian_orientation_v<layout>);
        CHECK(!has_odd_row_cartesian_arrangement_v<layout>);
        CHECK(!has_even_row_cartesian_arrangement_v<layout>);
        CHECK(!has_odd_column_cartesian_arrangement_v<layout>);
        CHECK(has_even_column_cartesian_arrangement_v<layout>);

        check_common_traits<layout>();
    }
}

TEST_CASE("Deep copy shifted Cartesian layout", "[shifted-cartesian-layout]")
{
    const shifted_cartesian_layout original{{5, 5, 0}};

    auto copy = original.clone();

    copy.resize(aspect_ratio<coordinate<decltype(original)>>{10, 10, 1});

    CHECK(original.x() == 5);
    CHECK(original.y() == 5);
    CHECK(original.z() == 0);

    CHECK(copy.x() == 10);
    CHECK(copy.y() == 10);
    CHECK(copy.z() == 1);
}
