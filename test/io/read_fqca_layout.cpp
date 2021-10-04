//
// Created by marcel on 04.10.21.
//

#include "catch.hpp"

#include <fiction/io/print_layout.hpp>
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
    using qca_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<coord_t>>>;

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

    const auto layout = read_fqca_layout<qca_layout>(layout_stream);

    CHECK(layout.x() == 0);
    CHECK(layout.y() == 0);
    CHECK(layout.area() == 1);
    CHECK(layout.is_empty_cell({0, 0}));
}

TEST_CASE("Read single-layer AND gate", "[fqca]")
{
    using qca_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<coord_t>>>;

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

    const auto layout = read_fqca_layout<qca_layout>(layout_stream);

    print_cell_level_layout(std::cout, layout);

    CHECK(layout.x() == 4);
    CHECK(layout.y() == 4);
    CHECK(layout.area() == 25);
    CHECK(layout.get_layout_name() == "AND");

    CHECK(layout.get_cell_type({0, 2}) == qca_technology::cell_type::INPUT);
    CHECK(layout.get_cell_type({2, 4}) == qca_technology::cell_type::INPUT);
    CHECK(layout.get_cell_type({2, 0}) == qca_technology::cell_type::CONST_0);
    CHECK(layout.get_cell_type({2, 1}) == qca_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({2, 2}) == qca_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({2, 3}) == qca_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({1, 2}) == qca_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({3, 2}) == qca_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({4, 2}) == qca_technology::cell_type::OUTPUT);

    CHECK(layout.get_clock_number({0, 2}) == 0);
    CHECK(layout.get_clock_number({2, 4}) == 0);
    CHECK(layout.get_clock_number({2, 0}) == 0);
    CHECK(layout.get_clock_number({2, 1}) == 0);
    CHECK(layout.get_clock_number({2, 2}) == 0);
    CHECK(layout.get_clock_number({2, 3}) == 0);
    CHECK(layout.get_clock_number({1, 2}) == 0);
    CHECK(layout.get_clock_number({3, 2}) == 0);
    CHECK(layout.get_clock_number({4, 2}) == 1);

    CHECK(layout.get_cell_name({0, 2}) == "a");
    CHECK(layout.get_cell_name({2, 4}) == "b");
    CHECK(layout.get_cell_name({4, 2}) == "f");
}
