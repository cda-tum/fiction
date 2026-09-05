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
#include <fiction/technology/sidb/model/defect.hpp>
#include <fiction/technology/sidb/surfaces/defect_surface.hpp>
#include <fiction/technology/sidb/technology.hpp>
#include <fiction/types.hpp>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::sidb;
using namespace fiction::sidb::model;

TEST_CASE("Coordinate to site", "[cell-level-layout-conversion]")
{
    CHECK(to_lattice_site(coords::offset{3, 0}) == lattice_site{3, 0, 0});
    CHECK(to_lattice_site(coords::offset{3, 1}) == lattice_site{3, 0, 1});
    CHECK(to_lattice_site(coords::offset{3, 7}) == lattice_site{3, 3, 1});
    CHECK(to_lattice_site(coords::cube{-3, -1}) == lattice_site{-3, -1, 1});
    CHECK(to_lattice_site(coords::cube{-3, -2}) == lattice_site{-3, -1, 0});
    CHECK(to_lattice_site(coords::siqad{4, 5, 1}) == lattice_site{4, 5, 1});

    CHECK(to_cube(lattice_site{3, 3, 1}) == coords::cube{3, 7});
    CHECK(to_cube(lattice_site{-3, -1, 1}) == coords::cube{-3, -1});
    CHECK(to_lattice_site(to_cube(lattice_site{2, -4, 0})) == lattice_site{2, -4, 0});
    constexpr auto min_coordinate = std::numeric_limits<int32_t>::min();
    constexpr auto max_coordinate = std::numeric_limits<int32_t>::max();
    CHECK(to_cube(site_at_row(0, min_coordinate)) == coords::cube{0, min_coordinate});
    CHECK(to_cube(site_at_row(0, max_coordinate)) == coords::cube{0, max_coordinate});
    CHECK_THROWS_AS(to_cube(lattice_site{0, min_coordinate, 0}), std::out_of_range);
    CHECK_THROWS_AS(to_cube(lattice_site{0, max_coordinate, 1}), std::out_of_range);
}

TEST_CASE("Lattice of a layout type", "[cell-level-layout-conversion]")
{
    CHECK(lattice_of<sidb_cell_clk_lyt>() == lattice::si_100_2x1());
    CHECK(lattice_of<sidb_100_cell_clk_lyt_cube>() == lattice::si_100_2x1());
    CHECK(lattice_of<sidb_111_cell_clk_lyt_siqad>() == lattice::si_111_1x1());
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
        CHECK(converted.num_cells() == 3);
        CHECK(converted.get_cell_type({0, 0, 0}) == sidb_technology::cell_type::INPUT);
        CHECK(converted.get_cell_type({2, 1, 1}) == sidb_technology::cell_type::NORMAL);
        CHECK(converted.get_cell_type({4, 2, 0}) == sidb_technology::cell_type::OUTPUT);
        CHECK(converted.num_defects() == 0);
    }
    SECTION("SiQAD coordinates on the 111 lattice")
    {
        sidb_111_cell_clk_lyt_siqad lyt{};
        lyt.assign_cell_type({1, 2, 1}, sidb_technology::cell_type::NORMAL);

        const auto converted = to_sidb_layout(lyt);

        CHECK(converted.get_lattice() == lattice::si_111_1x1());
        CHECK(converted.sidbs() == std::vector<lattice_site>{{1, 2, 1}});
    }
    SECTION("explicit lattice")
    {
        sidb_cell_clk_lyt_cube lyt{};
        lyt.assign_cell_type({1, 3}, sidb_technology::cell_type::NORMAL);

        const auto converted = to_sidb_layout(lyt, lattice::si_111_1x1());

        CHECK(converted.get_lattice() == lattice::si_111_1x1());
        CHECK(converted.sidbs() == std::vector<lattice_site>{{1, 1, 1}});
    }
    SECTION("defects carry over")
    {
        surfaces::defect_surface<sidb_cell_clk_lyt_cube> lyt{};
        lyt.assign_cell_type({1, 1}, sidb_technology::cell_type::NORMAL);
        lyt.assign_defect({0, 2}, defect{defect_type::SI_VACANCY, -1, 5.6, 5.0});

        const auto converted = to_sidb_layout(lyt);

        CHECK(converted.num_cells() == 1);
        CHECK(converted.num_defects() == 1);
        CHECK(converted.get_defect({0, 1, 0}) == defect{defect_type::SI_VACANCY, -1, 5.6, 5.0});
    }
}
