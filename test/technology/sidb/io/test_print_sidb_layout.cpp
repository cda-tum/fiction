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
 * @brief Tests for `fiction/technology/sidb/io/print_sidb_layout.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/io/print_sidb_layout.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/charge_state.hpp>
#include <fiction/technology/sidb/model/defect.hpp>
#include <fiction/technology/sidb/technology.hpp>

#include <cstdint>
#include <limits>
#include <sstream>
#include <stdexcept>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::io;
using namespace fiction::sidb::model;

TEST_CASE("Print empty SiDB layout", "[print-sidb-layout]")
{
    std::stringstream print_stream{};

    print_sidb_layout(print_stream, layout{});

    CHECK(print_stream.str() == "[i] empty layout\n");
}

TEST_CASE("Print SiDBs and defects on the H-Si(100)-2x1 surface", "[print-sidb-layout]")
{
    layout lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({1, 0, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({4, 0, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_defect({0, 0, 0}, defect{defect_type::UNKNOWN, 1});
    lyt.assign_defect({1, 0, 1}, defect{defect_type::UNKNOWN, 0});
    lyt.assign_defect({4, 0, 1}, defect{defect_type::UNKNOWN, -1});

    std::stringstream print_stream{};

    SECTION("crop_layout option activated")
    {
        print_sidb_layout(print_stream, lyt, false, true, true);

        constexpr const char* layout_print = " ·  ·  ·  ·  ·  ·  ·  ·  · \n"
                                             " ·  ·  ·  ·  ·  ·  ·  ·  · \n"
                                             "\n"
                                             " ·  ·  ⊞  ·  ·  ·  ·  ·  · \n"
                                             " ·  ·  ·  ⊡  ·  ·  ⊟  ·  · \n"
                                             "\n"
                                             " ·  ·  ·  ·  ·  ·  ·  ·  · \n"
                                             " ·  ·  ·  ·  ·  ·  ·  ·  · \n";

        CHECK(layout_print == print_stream.str());
    }
    SECTION("crop_layout option deactivated")
    {
        print_sidb_layout(print_stream, lyt, false, false, true);

        constexpr const char* layout_print = " ⊞  ·  ·  ·  · \n"
                                             " ·  ⊡  ·  ·  ⊟ \n";

        CHECK(layout_print == print_stream.str());
    }
    SECTION("inputs, outputs, and hidden lattice")
    {
        layout gate{};
        gate.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
        gate.assign_cell_type({2, 0, 0}, sidb_technology::cell_type::NORMAL);
        gate.assign_cell_type({4, 0, 0}, sidb_technology::cell_type::OUTPUT);

        print_sidb_layout(print_stream, gate, false, false, false);

        CHECK(print_stream.str() == " ◯    ◯    ◯ \n");
    }
}

TEST_CASE("Print six SiDBs on the H-Si(111)-1x1 surface", "[print-sidb-layout]")
{
    layout lyt{lattice::si_111_1x1()};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({1, 1, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, 2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({8, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 1, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({5, 2, 1}, sidb_technology::cell_type::NORMAL);

    constexpr const char* layout_print = " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
                                         "  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
                                         " ·  ·  ◯  ·  ·  ·  ·  ·  ·  ·  ◯  ·  · \n"
                                         "  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
                                         " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
                                         "  ·  ·  ·  ◯  ·  ·  ·  ·  ◯  ·  ·  ·  · \n"
                                         " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
                                         "  ·  ·  ·  ·  ◯  ·  ·  ◯  ·  ·  ·  ·  · \n"
                                         " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
                                         "  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
                                         "\n"
                                         "\n";

    std::stringstream print_stream{};

    print_sidb_layout(print_stream, lyt, false, true, true);

    CHECK(layout_print == print_stream.str());
}

TEST_CASE("Text padding rejects unrepresentable lattice sites", "[print-sidb-layout]")
{
    for (const auto coordinate : {std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max()})
    {
        layout lyt{};
        lyt.assign_cell_type({coordinate, coordinate, 0}, sidb_technology::cell_type::NORMAL);
        std::stringstream os{};
        CHECK_NOTHROW(print_sidb_layout(os, lyt, false, false));
        os.str("");
        CHECK_THROWS_AS(print_sidb_layout(os, lyt, false, true), std::out_of_range);
        CHECK(os.str().empty());
    }
}

TEST_CASE("Print SiDBs with a charge distribution", "[print-sidb-layout]")
{
    layout lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({4, 0, 1}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_defect({6, 0, 1}, defect{defect_type::UNKNOWN, -1});

    charge_distribution cd{lyt, charge_state::NEGATIVE};
    cd.assign_charge_state({2, 0, 0}, charge_state::NEUTRAL);
    cd.assign_charge_state({4, 0, 1}, charge_state::POSITIVE);

    std::stringstream print_stream{};

    print_sidb_layout(print_stream, lyt, cd, false, false, true);

    constexpr const char* layout_print = " ●  ·  ◯  ·  ·  ·  · \n"
                                         " ·  ·  ·  ·  ●  ·  ⊟ \n";

    CHECK(layout_print == print_stream.str());
}
