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
 * @brief Tests for `fiction/technology/sidb/cell_level_layout_conversion.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/sidb/cell_level_layout_conversion.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/technology.hpp>
#include <fiction/types.hpp>

#include <vector>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::sidb;

TEST_CASE("Coordinate to site", "[cell-level-layout-conversion]")
{
    CHECK(to_lattice_site(coords::offset{3, 0}) == lattice_site{3, 0, 0});
    CHECK(to_lattice_site(coords::offset{3, 1}) == lattice_site{3, 0, 1});
    CHECK(to_lattice_site(coords::offset{3, 7}) == lattice_site{3, 3, 1});
    CHECK(to_lattice_site(coords::cube{-3, -1}) == lattice_site{-3, -1, 1});
    CHECK(to_lattice_site(coords::cube{-3, -2}) == lattice_site{-3, -1, 0});
}

TEST_CASE("Cell-level layout to SiDB layout", "[cell-level-layout-conversion]")
{
    SECTION("offset coordinates")
    {
        sidb_cell_clk_lyt lyt{{5, 5}, "gate"};
        lyt.assign_cell_type({0, 0}, sidb_technology::cell_type::INPUT);
        lyt.assign_cell_type({2, 3}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({4, 4}, sidb_technology::cell_type::OUTPUT);

        const auto converted = to_sidb_layout(lyt);

        CHECK(converted.get_lattice() == lattice::si_100_2x1());
        CHECK(converted.get_layout_name() == "gate");
        CHECK(converted.num_dots() == 3);
        CHECK(converted.get_dot_tag({0, 0, 0}) == sidb_technology::cell_type::INPUT);
        CHECK(converted.get_dot_tag({2, 1, 1}) == sidb_technology::cell_type::NORMAL);
        CHECK(converted.get_dot_tag({4, 2, 0}) == sidb_technology::cell_type::OUTPUT);
        CHECK(converted.num_defects() == 0);
    }
    SECTION("explicit lattice")
    {
        sidb_cell_clk_lyt_cube lyt{};
        lyt.assign_cell_type({1, 3}, sidb_technology::cell_type::NORMAL);

        const auto converted = to_sidb_layout(lyt, lattice::si_111_1x1());

        CHECK(converted.get_lattice() == lattice::si_111_1x1());
        CHECK(converted.sidbs() == std::vector<lattice_site>{{1, 1, 1}});
    }
}
