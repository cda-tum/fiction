//
// Created by marcel on 04.10.21.
//

#include "catch.hpp"

#include <fiction/io/read_fqca_layout.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>

#include <sstream>
#include <string>

using namespace fiction;

TEST_CASE("Read empty layout", "[fqca]")
{
    static constexpr const char* fqca_layout = "[ empty layout ]\n"
                                               "\n"
                                               "= = = \n"
                                               "      \n"
                                               "      \n"
                                               "      \n"
                                               "= = = \n"
                                               "      \n"
                                               "      \n"
                                               "      \n"
                                               "= = = \n"
                                               "      \n"
                                               "      \n"
                                               "      \n"
                                               "= = = \n"
                                               "\n"
                                               "\n"
                                               "$\n"
                                               "\n";

    std::istringstream layout_stream{fqca_layout};

    const auto check = [](const auto& lyt)
    {
        CHECK(lyt.x() == 0);
        CHECK(lyt.y() == 0);
        CHECK(lyt.area() == 1);
        CHECK(lyt.is_empty_cell({0, 0}));
    };

    SECTION("Cartesian layout")
    {
        using qca_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<cartesian::ucoord_t>>>;

        check(read_fqca_layout<qca_layout>(layout_stream));
    }
    SECTION("Stacked layout")
    {
        using qca_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<cube::coord_t>>>;

        check(read_fqca_layout<qca_layout>(layout_stream));
    }
}

TEST_CASE("Read single-layer AND gate", "[fqca]")
{
    static constexpr const char* fqca_layout = "[ AND ]\n"
                                               "\n"
                                               "= = = = = \n"
                                               "    -     \n"
                                               "    0     \n"
                                               "a 0 0 0 b \n"
                                               "    0     \n"
                                               "    c     \n"
                                               "= = = = = \n"
                                               "\n"
                                               "\n"
                                               "$\n"
                                               "\n"
                                               "a:\n"
                                               "- input\n"
                                               "- label = \"a\"\n"
                                               "- clock = 0\n"
                                               "\n"
                                               "b:\n"
                                               "- output\n"
                                               "- label = \"f\"\n"
                                               "- clock = 1\n"
                                               "\n"
                                               "c:\n"
                                               "- input\n"
                                               "- label = \"b\"\n"
                                               "- clock = 0\n"
                                               "\n";

    std::istringstream layout_stream{fqca_layout};

    const auto check = [](const auto& lyt)
    {
        CHECK(lyt.x() == 4);
        CHECK(lyt.y() == 4);
        CHECK(lyt.area() == 25);
        //    CHECK(layout.get_layout_name() == "AND");

        CHECK(lyt.get_cell_type({0, 2}) == qca_technology::cell_type::INPUT);
        CHECK(lyt.get_cell_type({2, 4}) == qca_technology::cell_type::INPUT);
        CHECK(lyt.get_cell_type({2, 0}) == qca_technology::cell_type::CONST_0);
        CHECK(lyt.get_cell_type({2, 1}) == qca_technology::cell_type::NORMAL);
        CHECK(lyt.get_cell_type({2, 2}) == qca_technology::cell_type::NORMAL);
        CHECK(lyt.get_cell_type({2, 3}) == qca_technology::cell_type::NORMAL);
        CHECK(lyt.get_cell_type({1, 2}) == qca_technology::cell_type::NORMAL);
        CHECK(lyt.get_cell_type({3, 2}) == qca_technology::cell_type::NORMAL);
        CHECK(lyt.get_cell_type({4, 2}) == qca_technology::cell_type::OUTPUT);

        CHECK(lyt.get_clock_number({0, 2}) == 0);
        CHECK(lyt.get_clock_number({2, 4}) == 0);
        CHECK(lyt.get_clock_number({2, 0}) == 0);
        CHECK(lyt.get_clock_number({2, 1}) == 0);
        CHECK(lyt.get_clock_number({2, 2}) == 0);
        CHECK(lyt.get_clock_number({2, 3}) == 0);
        CHECK(lyt.get_clock_number({1, 2}) == 0);
        CHECK(lyt.get_clock_number({3, 2}) == 0);
        CHECK(lyt.get_clock_number({4, 2}) == 1);

        CHECK(lyt.get_cell_name({0, 2}) == "a");
        CHECK(lyt.get_cell_name({2, 4}) == "b");
        CHECK(lyt.get_cell_name({4, 2}) == "f");
    };

    SECTION("Cartesian layout")
    {
        using qca_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<cartesian::ucoord_t>>>;

        check(read_fqca_layout<qca_layout>(layout_stream));
    }
    SECTION("Stacked layout")
    {
        using qca_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<cube::coord_t>>>;

        check(read_fqca_layout<qca_layout>(layout_stream));
    }
}

TEST_CASE("Parsing of unsupported features", "[fqca]")
{
    static constexpr const char* fqca_layout = "[ AND ]\n"
                                               "\n"
                                               "= = = = = \n"
                                               "    -     \n"
                                               "    0     \n"
                                               "a 0 0 0 b \n"
                                               "    0     \n"
                                               "    c     \n"
                                               "= = = = = \n"
                                               "\n"
                                               "\n"
                                               "$\n"
                                               "\n"
                                               "a:\n"
                                               "- input\n"
                                               "- label = \"a\"\n"
                                               "- clock = 0\n"
                                               "- propagate\n"  // 'propagate' is not supported
                                               "\n"
                                               "b:\n"
                                               "- output\n"
                                               "- label = \"f\"\n"
                                               "- clock = 1\n"
                                               "- number = 5\n"  // 'number' is not supported
                                               "\n"
                                               "c:\n"
                                               "- input\n"
                                               "- label = \"b\"\n"
                                               "- clock = 0\n"
                                               "- offset = (0, -0.5, 2)\n"  // 'offset' is not supported
                                               "\n";

    std::istringstream layout_stream{fqca_layout};

    const auto check = [](const auto& lyt)
    {
        CHECK(lyt.x() == 4);
        CHECK(lyt.y() == 4);
        CHECK(lyt.area() == 25);
        //    CHECK(layout.get_layout_name() == "AND");

        CHECK(lyt.get_cell_type({0, 2}) == qca_technology::cell_type::INPUT);
        CHECK(lyt.get_cell_type({2, 4}) == qca_technology::cell_type::INPUT);
        CHECK(lyt.get_cell_type({2, 0}) == qca_technology::cell_type::CONST_0);
        CHECK(lyt.get_cell_type({2, 1}) == qca_technology::cell_type::NORMAL);
        CHECK(lyt.get_cell_type({2, 2}) == qca_technology::cell_type::NORMAL);
        CHECK(lyt.get_cell_type({2, 3}) == qca_technology::cell_type::NORMAL);
        CHECK(lyt.get_cell_type({1, 2}) == qca_technology::cell_type::NORMAL);
        CHECK(lyt.get_cell_type({3, 2}) == qca_technology::cell_type::NORMAL);
        CHECK(lyt.get_cell_type({4, 2}) == qca_technology::cell_type::OUTPUT);

        CHECK(lyt.get_clock_number({0, 2}) == 0);
        CHECK(lyt.get_clock_number({2, 4}) == 0);
        CHECK(lyt.get_clock_number({2, 0}) == 0);
        CHECK(lyt.get_clock_number({2, 1}) == 0);
        CHECK(lyt.get_clock_number({2, 2}) == 0);
        CHECK(lyt.get_clock_number({2, 3}) == 0);
        CHECK(lyt.get_clock_number({1, 2}) == 0);
        CHECK(lyt.get_clock_number({3, 2}) == 0);
        CHECK(lyt.get_clock_number({4, 2}) == 1);

        CHECK(lyt.get_cell_name({0, 2}) == "a");
        CHECK(lyt.get_cell_name({2, 4}) == "b");
        CHECK(lyt.get_cell_name({4, 2}) == "f");
    };

    SECTION("Cartesian layout")
    {
        using qca_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<cartesian::ucoord_t>>>;

        // despite unsupported features, the parsing should proceed normally
        check(read_fqca_layout<qca_layout>(layout_stream));
    }
    SECTION("Stacked layout")
    {
        using qca_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<cube::coord_t>>>;

        // despite unsupported features, the parsing should proceed normally
        check(read_fqca_layout<qca_layout>(layout_stream));
    }
}

TEST_CASE("Exceptions", "[fqca]")
{
    SECTION("unsupported_character_exception")
    {
        static constexpr const char* fqca_layout =
            "[ AND ]\n"
            "\n"
            "= = = = = \n"
            "    -     \n"
            "    0     \n"
            "_ 0 0 0 b \n"  // cell (0,2) uses an underscore as id, which is not allowed
            "    0     \n"
            "    c     \n"
            "= = = = = \n"
            "\n"
            "\n"
            "$\n"
            "\n"
            "_:\n"
            "- input\n"
            "- label = \"a\"\n"
            "- clock = 0\n"
            "\n"
            "b:\n"
            "- output\n"
            "- label = \"f\"\n"
            "- clock = 1\n"
            "\n"
            "c:\n"
            "- input\n"
            "- label = \"b\"\n"
            "- clock = 0\n"
            "\n";

        std::istringstream layout_stream{fqca_layout};

        SECTION("Cartesian layout")
        {
            using qca_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<cartesian::ucoord_t>>>;
            CHECK_THROWS_AS(read_fqca_layout<qca_layout>(layout_stream), unsupported_character_exception);
        }
        SECTION("Stacked layout")
        {
            using qca_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<cube::coord_t>>>;
            CHECK_THROWS_AS(read_fqca_layout<qca_layout>(layout_stream), unsupported_character_exception);
        }
    }
    SECTION("undefined_cell_label_exception")
    {
        static constexpr const char* fqca_layout = "[ AND ]\n"
                                                   "\n"
                                                   "= = = = = \n"
                                                   "    -     \n"
                                                   "    0     \n"
                                                   "a 0 0 0 b \n"
                                                   "    0     \n"
                                                   "    c     \n"
                                                   "= = = = = \n"
                                                   "\n"
                                                   "\n"
                                                   "$\n"
                                                   "\n"
                                                   "a:\n"
                                                   "- input\n"
                                                   "- label = \"a\"\n"
                                                   "- clock = 0\n"
                                                   "\n"
                                                   "b:\n"
                                                   "- output\n"
                                                   "- label = \"f\"\n"
                                                   "- clock = 1\n"
                                                   "\n"
                                                   "c:\n"
                                                   "- input\n"
                                                   "- label = \"b\"\n"
                                                   "- clock = 0\n"
                                                   "\n"
                                                   "d:\n"  // id 'd' does not appear in the layout definition
                                                   "- label = \"unused\"\n"
                                                   "- clock = 1\n";

        std::istringstream layout_stream{fqca_layout};

        SECTION("Cartesian layout")
        {
            using qca_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<cartesian::ucoord_t>>>;
            CHECK_THROWS_AS(read_fqca_layout<qca_layout>(layout_stream), undefined_cell_label_exception);
        }
        SECTION("Stacked layout")
        {
            using qca_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<cube::coord_t>>>;
            CHECK_THROWS_AS(read_fqca_layout<qca_layout>(layout_stream), undefined_cell_label_exception);
        }
    }
    SECTION("unrecognized_cell_definition_exception")
    {
        static constexpr const char* fqca_layout =
            "[ AND ]\n"
            "\n"
            "= = = = = \n"
            "    -     \n"
            "    0     \n"
            "a 0 0 0 b \n"
            "    0     \n"
            "    c     \n"
            "= = = = = \n"
            "\n"
            "\n"
            "$\n"
            "\n"
            "a:\n"
            "- input\n"
            "- label = \"a\"\n"
            "- clock = 0\n"
            "\n"
            "b:\n"
            "- output\n"
            "- label = \"f\"\n"
            "- clock = 1\n"
            "\n"
            "c:\n"
            "- input\n"
            "- label = \"b\"\n"
            "- clock = 0\n"
            "- unrecognized = 3.14\n"  // cell 'c' uses an unrecognized definition
            "\n";

        std::istringstream layout_stream{fqca_layout};

        SECTION("Cartesian layout")
        {
            using qca_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<cartesian::ucoord_t>>>;
            CHECK_THROWS_AS(read_fqca_layout<qca_layout>(layout_stream), unrecognized_cell_definition_exception);
        }
        SECTION("Stacked layout")
        {
            using qca_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<cube::coord_t>>>;
            CHECK_THROWS_AS(read_fqca_layout<qca_layout>(layout_stream), unrecognized_cell_definition_exception);
        }
    }
}
