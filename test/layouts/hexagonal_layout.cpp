//
// Created by marcel on 15.09.21.
//

#include "catch.hpp"

#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/traits.hpp>

using namespace fiction;

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wconversion"
#endif

TEST_CASE("Traits", "[hexagonal]")
{
    using layout = hexagonal_layout<cartesian::ucoord_t, even_column>;

//    CHECK(has_north_v<layout>);
//    CHECK(has_east_v<layout>);
//    CHECK(has_south_v<layout>);
//    CHECK(has_west_v<layout>);
//    CHECK(has_cardinal_checks_v<layout>);
//    CHECK(has_above_v<layout>);
//    CHECK(has_below_v<layout>);
//    CHECK(has_elevation_checks_v<layout>);
//    CHECK(is_coordinate_layout_v<layout>);
//
//    CHECK(has_foreach_coordinate_v<layout>);
//    CHECK(has_foreach_adjacent_coordinate_v<layout>);
}
