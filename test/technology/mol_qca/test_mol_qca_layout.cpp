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

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <fiction/technology/mol_qca/layout.hpp>

using namespace fiction;
using namespace fiction::mol_qca;

TEST_CASE("molQCA cell types carry their clock phase", "[mol-qca-layout]")
{
    const auto [type, phase, normal, constant] = GENERATE(Catch::Generators::table<cell_type, unsigned, bool, bool>({
        {cell_type::EMPTY, 0, false, false},
        {cell_type::NORMAL1, 0, true, false},
        {cell_type::NORMAL2, 1, true, false},
        {cell_type::NORMAL3, 2, true, false},
        {cell_type::NORMAL4, 3, true, false},
        {cell_type::INPUT, 0, false, false},
        {cell_type::OUTPUT, 0, false, false},
        {cell_type::CONST_0, 0, false, true},
        {cell_type::CONST_1, 0, false, true},
    }));
    CAPTURE(static_cast<char>(type));

    CHECK(clock_number(type) == phase);
    CHECK(is_normal(type) == normal);
    CHECK(is_constant(type) == constant);
}

TEST_CASE("molQCA layouts are planar", "[mol-qca-layout]")
{
    layout lyt{{9, 9, 1}, "wire"};

    CHECK(lyt.layers() == 1);
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
