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
 * @brief Tests for `fiction/layouts/shifted_cartesian_layout.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <fiction/layouts/arrangement.hpp>
#include <fiction/layouts/shifted_cartesian_layout.hpp>
#include <fiction/traits.hpp>

using namespace fiction;
using namespace fiction::layouts;

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
    CHECK(!is_gate_level_layout_v<Lyt>);
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
    check_common_traits<shifted_cartesian_layout<>>();
}

TEST_CASE("Shifted Cartesian layout arrangement", "[shifted-cartesian-layout]")
{
    const auto a =
        GENERATE(arrangement::ODD_ROW, arrangement::EVEN_ROW, arrangement::ODD_COLUMN, arrangement::EVEN_COLUMN);

    const shifted_cartesian_layout<> lyt{a, {3, 3}};

    CHECK(lyt.get_arrangement() == a);
    CHECK(lyt.clone().get_arrangement() == a);
}

TEST_CASE("Deep copy shifted Cartesian layout", "[shifted-cartesian-layout]")
{
    const shifted_cartesian_layout<> original{arrangement::EVEN_ROW, {5, 5, 0}};

    auto copy = original.clone();

    copy.resize({10, 10, 1});

    CHECK(original.x() == 5);
    CHECK(original.y() == 5);
    CHECK(original.z() == 0);

    CHECK(copy.x() == 10);
    CHECK(copy.y() == 10);
    CHECK(copy.z() == 1);
}
