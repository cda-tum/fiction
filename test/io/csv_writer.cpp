//
// Created by marcel on 02.08.23.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/io/csv_writer.hpp>

#include <sstream>
#include <string>
#include <string_view>

using namespace fiction;

TEST_CASE("CSV writer", "[csv-writer]")
{
    std::stringstream ss{};

    csv_writer writer(ss);

    SECTION("writing single integer value")
    {
        writer.write_line(1);
        CHECK(ss.str() == "1\n");
    }

    SECTION("writing single string value")
    {
        writer.write_line("fiction");
        CHECK(ss.str() == "fiction\n");
    }

    SECTION("writing single double value")
    {
        writer.write_line(3.14159);
        CHECK(ss.str() == "3.14159\n");
    }

    SECTION("writing multiple integer values")
    {
        writer.write_line(1, 2, 3, 4, 5);
        CHECK(ss.str() == "1,2,3,4,5\n");
    }

    SECTION("writing multiple string values")
    {
        writer.write_line("a", "b", "c");
        CHECK(ss.str() == "a,b,c\n");
    }

    SECTION("writing mixed type values")
    {
        writer.write_line("mixed", 1234, 42.0);
        CHECK(ss.str() == "mixed,1234,42\n");
    }

    SECTION("writing with empty values")
    {
        writer.write_line("");
        CHECK(ss.str() == "\n");
        writer.write_line(1234, "", 42.0);
        CHECK(ss.str() == "\n1234,,42\n");
    }
    SECTION("writing multiple lines")
    {
        writer.write_line("line1", 1234, 42.0);
        writer.write_line("line2", 5678, 43.0);
        CHECK(ss.str() == "line1,1234,42\nline2,5678,43\n");

        writer.write_line(1, 2, 3);
        writer.write_line(4, 5, 6);
        CHECK(ss.str() == "line1,1234,42\nline2,5678,43\n1,2,3\n4,5,6\n");

        writer.write_line("a", "b", "c");
        writer.write_line("d", "e", "f");
        CHECK(ss.str() == "line1,1234,42\nline2,5678,43\n1,2,3\n4,5,6\n"
                          "a,b,c\nd,e,f\n");
    }
}
