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
 * @brief Tests for `fiction/technology/qca/layout.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/qca/layout.hpp>

#include <stdexcept>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::qca;

TEST_CASE("QCA cell types", "[qca-layout]")
{
    CHECK(is_constant(cell_type::CONST_0));
    CHECK(is_constant(cell_type::CONST_1));
    CHECK(!is_constant(cell_type::NORMAL));
    CHECK(!is_constant(cell_type::INPUT));
}

TEST_CASE("QCA layout construction", "[qca-layout]")
{
    CHECK_THROWS_AS((layout{{2, 2}, "", 0, 1}), std::invalid_argument);
    CHECK_THROWS_AS((layout{{2, 2}, clocking::twoddwave<layout>(), "", 1, 0}), std::invalid_argument);

    const layout lyt{{4, 4, 1}, clocking::use<layout>(), "crossing", 5, 5};

    CHECK(lyt.x() == 4);
    CHECK(lyt.z() == 1);
    CHECK(lyt.get_layout_name() == "crossing");
    CHECK(lyt.is_clocking_scheme(clocking::USE_NAME));
    CHECK(lyt.get_tile_size_x() == 5);
    CHECK(lyt.get_tile_size_y() == 5);
}

TEST_CASE("QCA cell modes on the crossing layer", "[qca-layout]")
{
    layout lyt{{4, 4, 1}, "crossover"};

    lyt.assign_cell_type({2, 2, 0}, cell_type::NORMAL);
    lyt.assign_cell_type({2, 1, 1}, cell_type::NORMAL);
    lyt.assign_cell_type({2, 2, 1}, cell_type::NORMAL);

    lyt.assign_cell_mode({2, 1, 1}, cell_mode::VERTICAL);
    lyt.assign_cell_mode({2, 2, 1}, cell_mode::CROSSOVER);

    CHECK(lyt.is_crossing_layer({2, 2, 1}));
    CHECK(lyt.num_cells() == 3);
    CHECK(lyt.get_cell_mode({2, 2, 0}) == cell_mode::NORMAL);
    CHECK(lyt.get_cell_mode({2, 1, 1}) == cell_mode::VERTICAL);
    CHECK(lyt.get_cell_mode({2, 2, 1}) == cell_mode::CROSSOVER);

    SECTION("normal mode removes a stored mode")
    {
        lyt.assign_cell_mode({2, 1, 1}, cell_mode::NORMAL);
        CHECK(lyt.get_cell_mode({2, 1, 1}) == cell_mode::NORMAL);
    }
    SECTION("removing a cell removes its mode")
    {
        lyt.assign_cell_type({2, 2, 1}, cell_type::EMPTY);
        lyt.assign_cell_type({2, 2, 1}, cell_type::NORMAL);
        CHECK(lyt.get_cell_mode({2, 2, 1}) == cell_mode::NORMAL);
    }
}

TEST_CASE("QCA synchronization elements belong to clock zones", "[qca-layout]")
{
    layout original{{5, 5, 1}, clocking::twoddwave<layout>(), "cells", 2, 2};

    original.assign_synchronization_element({1, 1}, 2);

    CHECK(original.num_se() == 1);
    CHECK(original.get_synchronization_element({2, 2}) == 2);
    CHECK(original.get_synchronization_element({3, 3}) == 2);
    CHECK(original.is_synchronization_element({3, 2, 1}));
    CHECK(original.get_synchronization_element({4, 4}) == 0);
    CHECK(!original.is_synchronization_element({1, 1}));

    SECTION("copies are independent")
    {
        auto copy = original;
        CHECK(copy == original);

        copy.assign_synchronization_element({1, 1}, 0);
        copy.assign_clock_number({1, 1}, 0);
        copy.assign_cell_type({0, 0}, cell_type::NORMAL);
        copy.assign_cell_mode({0, 0}, cell_mode::ROTATED);

        CHECK(copy.num_se() == 0);
        CHECK(original.get_synchronization_element({3, 3}) == 2);
        CHECK(original.get_clock_number({3, 3}) == 2);
        CHECK(original.is_empty());
        CHECK(copy != original);
    }
}
