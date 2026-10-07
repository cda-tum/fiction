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
 * @brief Tests for `fiction/technology/inml/layout.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/technology/inml/layout.hpp>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::inml;

TEST_CASE("iNML layouts hold planar, clocked magnets", "[inml-layout]")
{
    layout lyt{{7, 7, 1}, clocking::twoddwave(clocking::num_clks::THREE), "inverter", 4, 4};

    CHECK(lyt.z() == 0);
    CHECK(lyt.num_clocks() == 3);
    CHECK(lyt.get_tile_size_x() == 4);

    lyt.assign_cell_type({0, 1}, magnet_type::INPUT);
    lyt.assign_cell_type({1, 1}, magnet_type::INVERTER_MAGNET);
    lyt.assign_cell_type({5, 1}, magnet_type::OUTPUT);
    lyt.assign_cell_name({0, 1}, "a");

    CHECK(lyt.num_cells() == 3);
    CHECK(lyt.get_cell_type({1, 1}) == magnet_type::INVERTER_MAGNET);
    CHECK(lyt.get_cell_name({0, 1}) == "a");
    CHECK(lyt.get_clock_number({1, 1}) == 0);
    CHECK(lyt.get_clock_number({5, 1}) == 1);

    auto copy = lyt;
    CHECK(copy == lyt);
    copy.assign_cell_type({1, 1}, magnet_type::NORMAL);
    CHECK(lyt.get_cell_type({1, 1}) == magnet_type::INVERTER_MAGNET);
}
