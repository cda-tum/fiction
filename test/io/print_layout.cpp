//
// Created by marcel on 19.05.21.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/io/print_layout.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>

#include <sstream>

using namespace fiction;

TEST_CASE("Print empty gate-level layout", "[print-gate-level-layout]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    gate_layout layout{gate_layout::aspect_ratio{2, 2}, open_clocking<gate_layout>(num_clks::FOUR)};

    constexpr const char* layout_print = "[i] empty layout";

    std::stringstream print_stream{};

    print_gate_level_layout(print_stream, layout, false, false);

    CHECK(print_stream.str() == layout_print);
}

TEST_CASE("Print simple gate-level layout", "[print-gate-level-layout]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    gate_layout layout{gate_layout::aspect_ratio{3, 1, 0}, open_clocking<gate_layout>(num_clks::FOUR)};

    const auto x1 = layout.create_pi("x1", {2, 0});
    const auto x2 = layout.create_pi("x2", {1, 1});
    const auto a1 = layout.create_and(x1, x2, {1, 0});
    const auto n1 = layout.create_not(x2, {2, 1});
    layout.create_po(a1, "f1", {0, 0});
    layout.create_po(n1, "f2", {3, 1});

    SECTION("No clocking")
    {
        constexpr const char* layout_print = "        \n"
                                             "O & I ▢ \n"
                                             "        \n"
                                             "▢ I ¬ O \n"
                                             "\n";

        std::stringstream print_stream{};

        print_gate_level_layout(print_stream, layout, false, false);

        CHECK(print_stream.str() == layout_print);
    }

    SECTION("Irregular clocking")
    {
        layout.assign_clock_number({2, 0}, static_cast<typename gate_layout::clock_number_t>(0));
        layout.assign_clock_number({1, 0}, static_cast<typename gate_layout::clock_number_t>(1));
        layout.assign_clock_number({0, 0}, static_cast<typename gate_layout::clock_number_t>(2));
        layout.assign_clock_number({1, 1}, static_cast<typename gate_layout::clock_number_t>(0));
        layout.assign_clock_number({2, 1}, static_cast<typename gate_layout::clock_number_t>(1));
        layout.assign_clock_number({3, 1}, static_cast<typename gate_layout::clock_number_t>(2));

        constexpr const char* layout_print = "        \n"
                                             "O←&←I ▢ \n"
                                             "  ↑     \n"
                                             "▢ F→¬→O \n"
                                             "\n";

        std::stringstream print_stream{};

        print_gate_level_layout(print_stream, layout, false, false);

        CHECK(print_stream.str() == layout_print);
    }
}

TEST_CASE("Print crossing gate-level layout", "[print-gate-level-layout]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    auto layout = blueprints::crossing_layout<gate_layout>();

    constexpr const char* layout_print = "        \n"
                                         "▢ I I ▢ \n"
                                         "  ↓ ↓   \n"
                                         "I→&→+→O \n"
                                         "    ↓   \n"
                                         "I→=→&→O \n"
                                         "\n";

    std::stringstream print_stream{};

    print_gate_level_layout(print_stream, layout, false, false);

    CHECK(print_stream.str() == layout_print);
}

TEST_CASE("Print empty cell-level layout", "[print-gate-level-layout]")
{
    using cell_layout = fiction::cell_level_layout<fiction::qca_technology,
                                                   fiction::clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    cell_layout layout{cell_layout::aspect_ratio{2, 2}, "Empty"};

    constexpr const char* layout_print = "[i] empty layout";

    std::stringstream print_stream{};

    print_cell_level_layout(print_stream, layout, false, false);

    CHECK(print_stream.str() == layout_print);
}

TEST_CASE("Print AND gate cell-level layout", "[print-cell-level-layout]")
{
    using cell_layout = fiction::cell_level_layout<fiction::qca_technology,
                                                   fiction::clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    cell_layout layout{cell_layout::aspect_ratio{4, 4}, "AND"};

    layout.assign_cell_type({0, 2}, fiction::qca_technology::cell_type::INPUT);
    layout.assign_cell_type({2, 4}, fiction::qca_technology::cell_type::INPUT);
    layout.assign_cell_type({2, 0}, fiction::qca_technology::cell_type::CONST_0);
    layout.assign_cell_type({2, 1}, fiction::qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 2}, fiction::qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 3}, fiction::qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({1, 2}, fiction::qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({3, 2}, fiction::qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({4, 2}, fiction::qca_technology::cell_type::OUTPUT);

    layout.assign_cell_name({0, 2}, "a");
    layout.assign_cell_name({2, 4}, "b");
    layout.assign_cell_name({4, 2}, "f");

    constexpr const char* layout_print = "  0  \n"
                                         "  ▢  \n"
                                         "i▢▢▢o\n"
                                         "  ▢  \n"
                                         "  i  \n"
                                         "\n";

    std::stringstream print_stream{};

    print_cell_level_layout(print_stream, layout, false, false);

    CHECK(print_stream.str() == layout_print);
}

TEST_CASE("Print wire crossing cell-level layout", "[print-cell-level-layout]")
{
    using cell_layout = fiction::cell_level_layout<fiction::qca_technology,
                                                   fiction::clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    cell_layout layout{cell_layout::aspect_ratio{4, 4, 1}, "Crossover"};

    layout.assign_cell_type({0, 2, 0}, fiction::qca_technology::cell_type::INPUT);
    layout.assign_cell_type({2, 0, 0}, fiction::qca_technology::cell_type::INPUT);
    layout.assign_cell_type({1, 2, 0}, fiction::qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 2, 0}, fiction::qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({3, 2, 0}, fiction::qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 1, 1}, fiction::qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 2, 1}, fiction::qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 3, 1}, fiction::qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 4, 0}, fiction::qca_technology::cell_type::OUTPUT);
    layout.assign_cell_type({4, 2, 0}, fiction::qca_technology::cell_type::OUTPUT);

    layout.assign_cell_mode({2, 1, 1}, fiction::qca_technology::cell_mode::CROSSOVER);
    layout.assign_cell_mode({2, 2, 1}, fiction::qca_technology::cell_mode::CROSSOVER);
    layout.assign_cell_mode({2, 3, 1}, fiction::qca_technology::cell_mode::CROSSOVER);

    layout.assign_cell_name({0, 2}, "a");
    layout.assign_cell_name({2, 0}, "b");
    layout.assign_cell_name({4, 2}, "a'");
    layout.assign_cell_name({2, 4}, "b'");

    constexpr const char* layout_print = "  i  \n"
                                         "  x  \n"
                                         "i▢x▢o\n"
                                         "  x  \n"
                                         "  o  \n"
                                         "\n";

    std::stringstream print_stream{};

    print_cell_level_layout(print_stream, layout, false, false);

    CHECK(print_stream.str() == layout_print);
}
