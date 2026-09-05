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
 * @brief Tests for `fiction/technology/sidb/io/read_surface_defects.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/technology/sidb/io/read_surface_defects.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/technology.hpp>
#include <fiction/types.hpp>

#include <fstream>
#include <initializer_list>
#include <sstream>
#include <string>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::io;
using namespace fiction::sidb::model;

TEST_CASE("Read surface defects into an sidb::layout", "[read-sidb-surface-defects]")
{
    SECTION("empty surfaces")
    {
        for (const auto* const surface : {"[]", "[[]]", "[[][][]]"})
        {
            std::istringstream surface_stream{surface};

            const auto lyt = read_surface_defects(surface_stream);

            CHECK(lyt.is_empty());
            CHECK(lyt.num_defects() == 0);
        }
    }
    SECTION("defective surface")
    {
        static constexpr const char* sidb_surface = "[[0 1 2 3]"
                                                    " [4 5 6 7]"
                                                    " [8 9 10 0]]";

        std::istringstream surface_stream{sidb_surface};

        const auto lyt = read_surface_defects(surface_stream, "surface");

        CHECK(lyt.get_layout_name() == "surface");
        CHECK(lyt.is_empty());
        CHECK(lyt.num_defects() == 10);

        CHECK(lyt.get_defect({0, 0, 0}).type == defect_type::NONE);
        CHECK(lyt.get_defect({1, 0, 0}).type == defect_type::DB);
        CHECK(lyt.get_defect({2, 0, 0}).type == defect_type::SI_VACANCY);
        CHECK(lyt.get_defect({3, 0, 0}).type == defect_type::DIHYDRIDE_PAIR);
        CHECK(lyt.get_defect({0, 0, 1}).type == defect_type::SINGLE_DIHYDRIDE);
        CHECK(lyt.get_defect({1, 0, 1}).type == defect_type::ONE_BY_ONE);
        CHECK(lyt.get_defect({2, 0, 1}).type == defect_type::THREE_BY_ONE);
        CHECK(lyt.get_defect({3, 0, 1}).type == defect_type::SILOXANE);
        CHECK(lyt.get_defect({0, 1, 0}).type == defect_type::RAISED_SI);
        CHECK(lyt.get_defect({1, 1, 0}).type == defect_type::ETCH_PIT);
        CHECK(lyt.get_defect({2, 1, 0}).type == defect_type::MISSING_DIMER);
        CHECK(lyt.get_defect({3, 1, 0}).type == defect_type::NONE);
    }
    SECTION("non-defective surface")
    {
        std::istringstream surface_stream{"[[0 0 0 0] [0 0 0 0] [0 0 0 0]]"};
        const auto         lyt = read_surface_defects(surface_stream);
        CHECK(lyt.is_empty());
        CHECK(lyt.num_defects() == 0);
    }
    SECTION("lines with negative numbers or non-numeral characters are ignored")
    {
        for (const auto* const surface : {"[[0 1 0 0] [0 -5 0 0] [0 1 0 0]]", "[[0 1 0 0] [0 Z 0 0] [0 1 0 0]]"})
        {
            std::istringstream surface_stream{surface};
            const auto         lyt = read_surface_defects(surface_stream);
            // the ignored line does not count as a row, so the third line lands on row 1
            CHECK(lyt.num_defects() == 2);
            CHECK(lyt.get_defect(site_at_row(1, 0)).type == defect_type::DB);
            CHECK(lyt.get_defect(site_at_row(1, 1)).type == defect_type::DB);
        }
    }
    SECTION("exceptions")
    {
        std::istringstream unsupported{"[[0 1 2 3] [4 5 6 7] [8 9 10 11]]"};
        CHECK_THROWS_AS(read_surface_defects(unsupported), unsupported_defect_index_exception);

        std::istringstream oversized{"[[999999999999999999999999]]"};
        CHECK_THROWS_AS(read_surface_defects(oversized), unsupported_defect_index_exception);

        std::istringstream missing{"[[0 1 2 3] [4 5 6] [8 9 10 0]]"};
        CHECK_THROWS_AS(read_surface_defects(missing), missing_position_exception);

        CHECK_THROWS_AS(read_surface_defects("/this/file/does/not/exist.txt"), std::ifstream::failure);
    }
}
