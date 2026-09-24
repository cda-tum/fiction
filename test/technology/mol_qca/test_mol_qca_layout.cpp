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
 * @brief Tests for `fiction/technology/mol_qca/layout.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/technology/mol_qca/layout.hpp>

using namespace fiction;
using namespace fiction::mol_qca;

TEST_CASE("molQCA cell types carry their clock phase", "[mol-qca-layout]")
{
    CHECK(clock_number(cell_type::NORMAL1) == 0);
    CHECK(clock_number(cell_type::NORMAL2) == 1);
    CHECK(clock_number(cell_type::NORMAL3) == 2);
    CHECK(clock_number(cell_type::NORMAL4) == 3);
    CHECK(clock_number(cell_type::INPUT) == 0);

    CHECK(is_normal(cell_type::NORMAL3));
    CHECK(!is_normal(cell_type::OUTPUT));
    CHECK(is_constant(cell_type::CONST_1));
    CHECK(!is_constant(cell_type::NORMAL1));
}

TEST_CASE("molQCA layouts are planar", "[mol-qca-layout]")
{
    layout lyt{{9, 9, 1}, "wire"};

    CHECK(lyt.z() == 0);
    CHECK(lyt.get_layout_name() == "wire");

    lyt.assign_cell_type({0, 4}, cell_type::INPUT);
    lyt.assign_cell_type({1, 4}, cell_type::NORMAL2);
    lyt.assign_cell_type({2, 4}, cell_type::OUTPUT);

    CHECK(lyt.num_cells() == 3);
    CHECK(lyt.num_pis() == 1);
    CHECK(lyt.num_pos() == 1);
    CHECK(clock_number(lyt.get_cell_type({1, 4})) == 1);

    auto copy = lyt;
    copy.assign_cell_type({1, 4}, cell_type::EMPTY);
    CHECK(lyt.get_cell_type({1, 4}) == cell_type::NORMAL2);
    CHECK(copy != lyt);
}
