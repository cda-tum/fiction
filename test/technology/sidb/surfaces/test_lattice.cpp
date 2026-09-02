/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

//
// Created by Jan Drewniok on 12.02.24.
//

#include "catch2/catch_test_macros.hpp"

#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/technology/sidb/surfaces/lattice.hpp>
#include <fiction/technology/sidb/surfaces/lattice_orientations.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEST_CASE("layout traits", "[sidb-lattice]")
{
    CHECK(!is_sidb_lattice_v<sidb_cell_clk_lyt>);
    CHECK(is_cell_level_layout_v<sidb_cell_clk_lyt>);
    CHECK(!is_sidb_lattice_111_v<sidb_cell_clk_lyt>);
    CHECK(!is_sidb_lattice_100_v<sidb_cell_clk_lyt>);

    using lattice_layout_100 = sidb::surfaces::lattice<sidb::surfaces::lattice_100, sidb_cell_clk_lyt>;
    CHECK(is_sidb_lattice_v<lattice_layout_100>);
    CHECK(is_cell_level_layout_v<lattice_layout_100>);
    CHECK(is_sidb_lattice_100_v<lattice_layout_100>);

    using lattice_layout_111 = sidb::surfaces::lattice<sidb::surfaces::lattice_111, sidb_cell_clk_lyt>;
    CHECK(is_sidb_lattice_v<lattice_layout_111>);
    CHECK(is_cell_level_layout_v<lattice_layout_111>);
    CHECK(is_sidb_lattice_111_v<lattice_layout_111>);
}

TEST_CASE("use different lattice orientations", "[sidb-lattice]")
{
    sidb::surfaces::lattice<sidb::surfaces::lattice_100, sidb_cell_clk_lyt> lyt_100{};
    lyt_100.assign_cell_type({0, 0}, sidb_100_cell_clk_lyt::cell_type::NORMAL);
    lyt_100.assign_cell_type({3, 0}, sidb_100_cell_clk_lyt::cell_type::NORMAL);
    lyt_100.assign_cell_type({5, 0}, sidb_100_cell_clk_lyt::cell_type::NORMAL);
    CHECK(lyt_100.num_cells() == 3);

    using lattice_layout_111 = sidb::surfaces::lattice<sidb::surfaces::lattice_100, sidb_cell_clk_lyt>;
    lattice_layout_111 lyt_111{};
    lyt_111.assign_cell_type({0, 0}, sidb_100_cell_clk_lyt::cell_type::NORMAL);
    lyt_111.assign_cell_type({3, 0}, sidb_100_cell_clk_lyt::cell_type::NORMAL);
    lyt_111.assign_cell_type({5, 0}, sidb_100_cell_clk_lyt::cell_type::NORMAL);
    CHECK(lyt_111.num_cells() == 3);
}
