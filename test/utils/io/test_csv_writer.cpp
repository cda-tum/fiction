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
 * @brief Tests for `fiction/utils/io/csv_writer.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <fiction/utils/io/csv_writer.hpp>

#include <sstream>

using namespace fiction;
using namespace fiction::utils::io;

TEST_CASE("CSV writer", "[csv-writer]")
{
    std::stringstream ss{};

    csv_writer writer(ss);

    SECTION("writing no values")
    {
        writer.write_line();
        CHECK_THAT(ss.str(), Catch::Matchers::Equals("\n"));
    }

    SECTION("writing single integer value")
    {
        writer.write_line(1);
        CHECK_THAT(ss.str(), Catch::Matchers::Equals("1\n"));
    }

    SECTION("writing single string value")
    {
        writer.write_line("fiction");
        CHECK_THAT(ss.str(), Catch::Matchers::Equals("fiction\n"));
    }

    SECTION("writing single double value")
    {
        // NOLINTNEXTLINE(modernize-use-std-numbers): the decimal input is part of the exact serialization test.
        writer.write_line(3.14159);
        CHECK_THAT(ss.str(), Catch::Matchers::Equals("3.14159\n"));
    }

    SECTION("writing multiple integer values")
    {
        writer.write_line(1, 2, 3, 4, 5);
        CHECK_THAT(ss.str(), Catch::Matchers::Equals("1,2,3,4,5\n"));
    }

    SECTION("writing multiple string values")
    {
        writer.write_line("a", "b", "c");
        CHECK_THAT(ss.str(), Catch::Matchers::Equals("a,b,c\n"));
    }

    SECTION("writing values without escaping")
    {
        writer.write_line("a,b", "\"quoted\"", "first\nsecond");
        CHECK(ss.str() == "a,b,\"quoted\",first\nsecond\n");
    }

    SECTION("writing mixed type values")
    {
        writer.write_line("mixed", 1234, 42.0);
        CHECK_THAT(ss.str(), Catch::Matchers::Equals("mixed,1234,42\n"));
    }

    SECTION("writing with empty values")
    {
        writer.write_line("");
        CHECK_THAT(ss.str(), Catch::Matchers::Equals("\n"));
        writer.write_line(1234, "", 42.0);
        CHECK_THAT(ss.str(), Catch::Matchers::Equals("\n1234,,42\n"));
    }
    SECTION("writing multiple lines")
    {
        writer.write_line("line1", 1234, 42.0);
        writer.write_line("line2", 5678, 43.0);
        CHECK_THAT(ss.str(), Catch::Matchers::Equals("line1,1234,42\nline2,5678,43\n"));

        writer.write_line(1, 2, 3);
        writer.write_line(4, 5, 6);
        CHECK_THAT(ss.str(), Catch::Matchers::Equals("line1,1234,42\nline2,5678,43\n1,2,3\n4,5,6\n"));

        writer.write_line("a", "b", "c");
        writer.write_line("d", "e", "f");
        CHECK_THAT(ss.str(), Catch::Matchers::Equals("line1,1234,42\nline2,5678,43\n1,2,3\n4,5,6\n"
                                                     "a,b,c\nd,e,f\n"));
    }
}
