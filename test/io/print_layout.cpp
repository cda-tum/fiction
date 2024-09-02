//
// Created by marcel on 19.05.21.
//

#include <catch2/catch_test_macros.hpp>

#include "fiction/layouts/cell_level_layout.hpp"
#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/layouts/coordinates.hpp"
#include "fiction/technology/sidb_defect_surface.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/utils/layout_utils.hpp"
#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/physical_design/apply_gate_library.hpp>
#include <fiction/io/print_layout.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/technology/sidb_bestagon_library.hpp>
#include <fiction/technology/sidb_lattice.hpp>
#include <fiction/technology/sidb_lattice_orientations.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/layout_utils.hpp>

#include <sstream>

using namespace fiction;

TEST_CASE("Print empty gate-level layout", "[print-gate-level-layout]")
{
    using gate_layout = gate_level_layout<clocked_layout<tile_based_layout<cartesian_layout<offset::ucoord_t>>>>;

    const gate_layout layout{gate_layout::aspect_ratio{2, 2}, open_clocking<gate_layout>(num_clks::FOUR)};

    constexpr const char* layout_print = "[i] empty layout\n";

    std::stringstream print_stream{};

    print_gate_level_layout(print_stream, layout, false, false);

    CHECK(print_stream.str() == layout_print);

    print_stream = {};

    print_layout(layout, print_stream);

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

TEST_CASE("Print empty cell-level layout", "[print-cell-level-layout]")
{
    using cell_layout = fiction::cell_level_layout<fiction::qca_technology,
                                                   fiction::clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    const cell_layout layout{cell_layout::aspect_ratio{2, 2}, "Empty"};

    constexpr const char* layout_print = "[i] empty layout\n";

    std::stringstream print_stream{};

    print_cell_level_layout(print_stream, layout, false, false);

    CHECK(print_stream.str() == layout_print);

    print_stream = {};

    print_layout(layout, print_stream);

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

TEST_CASE("Print empty charge layout", "[print-charge-layout]")
{
    const sidb_cell_clk_lyt_siqad     lyt{{2, 2}, "Empty"};
    const sidb_100_cell_clk_lyt_siqad lat{lyt};

    const charge_distribution_surface layout{lat};

    constexpr const char* layout_print = "[i] empty layout\n";

    std::stringstream print_stream{};

    print_sidb_layout(print_stream, layout, false);

    CHECK(print_stream.str() == layout_print);

    print_stream = {};

    print_layout(layout, print_stream);

    CHECK(print_stream.str() == layout_print);
}

TEST_CASE("layout which is sidb_defect_surface and charge distribution surface but empty", "[print-charge-layout]")
{
    const sidb_defect_surface<sidb_cell_clk_lyt_siqad> sidb_layout{{2, 2}};

    const sidb_lattice<sidb_100_lattice, sidb_defect_surface<sidb_cell_clk_lyt_siqad>> lat{sidb_layout};

    const charge_distribution_surface layout{lat};

    constexpr const char* layout_print = "[i] empty layout\n";

    std::stringstream print_stream{};

    print_sidb_layout(print_stream, layout, false);

    CHECK(print_stream.str() == layout_print);

    print_stream = {};

    print_layout(layout, print_stream);

    CHECK(print_stream.str() == layout_print);
}

TEST_CASE("Print Bestagon OR-gate without defect", "[print-charge-layout]")
{
    using hex_gate_lyt = hex_odd_row_gate_clk_lyt;

    hex_gate_lyt layout{aspect_ratio<hex_gate_lyt>{0, 0}};

    layout.create_or({}, {}, {0, 0});

    const auto lyt =
        convert_layout_to_siqad_coordinates(apply_gate_library<sidb_100_cell_clk_lyt, sidb_bestagon_library>(layout));

    charge_distribution_surface cl{lyt, sidb_simulation_parameters{3, -0.32}, sidb_charge_state::NEGATIVE};

    cl.assign_charge_state({16, 3, 0}, sidb_charge_state::NEUTRAL);
    cl.assign_charge_state({42, 3, 0}, sidb_charge_state::NEGATIVE);
    cl.assign_charge_state({18, 4, 0}, sidb_charge_state::NEGATIVE);
    cl.assign_charge_state({40, 4, 0}, sidb_charge_state::NEGATIVE);
    cl.assign_charge_state({22, 5, 0}, sidb_charge_state::NEGATIVE);
    cl.assign_charge_state({36, 5, 0}, sidb_charge_state::NEUTRAL);
    cl.assign_charge_state({24, 6, 0}, sidb_charge_state::NEGATIVE);
    cl.assign_charge_state({34, 6, 0}, sidb_charge_state::POSITIVE);
    cl.assign_charge_state({27, 9, 0}, sidb_charge_state::NEGATIVE);
    cl.assign_charge_state({32, 10, 0}, sidb_charge_state::POSITIVE);
    cl.assign_charge_state({27, 10, 1}, sidb_charge_state::NEGATIVE);
    cl.assign_charge_state({29, 11, 1}, sidb_charge_state::NEGATIVE);
    cl.assign_charge_state({29, 14, 0}, sidb_charge_state::NEGATIVE);
    cl.assign_charge_state({30, 15, 0}, sidb_charge_state::NEGATIVE);
    cl.assign_charge_state({34, 16, 0}, sidb_charge_state::POSITIVE);
    cl.assign_charge_state({36, 17, 0}, sidb_charge_state::NEUTRAL);
    cl.assign_charge_state({40, 18, 0}, sidb_charge_state::NEGATIVE);
    cl.assign_charge_state({42, 19, 0}, sidb_charge_state::NEGATIVE);

    std::stringstream print_stream{};

    SECTION("Cropped")
    {
        print_sidb_layout(print_stream, cl, false, true, true);

        constexpr const char* layout_print =
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ◯  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ◯  ·  ·  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ◯  ·  ·  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n";

        CHECK(layout_print == print_stream.str());
    }

    SECTION("No crop")
    {
        print_stream.clear();

        print_sidb_layout(print_stream, cl, false);

        constexpr const char* layout_print =
            " ◯  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ● \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ◯  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ◯  ·  ·  ·  ·  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  · \n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
            "\n"
            " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ● \n";
        CHECK(layout_print == print_stream.str());
    }
}

TEST_CASE("Print Bestagon OR-gate with defect", "[print-charge-layout]")
{
    using hex_gate_lyt = hex_odd_row_gate_clk_lyt;

    hex_gate_lyt layout{aspect_ratio<hex_gate_lyt>{0, 0}};

    layout.create_or({}, {}, {0, 0});

    const auto lyt = sidb_defect_surface{
        convert_layout_to_siqad_coordinates(apply_gate_library<sidb_100_cell_clk_lyt, sidb_bestagon_library>(layout))};

    charge_distribution_surface cl{lyt, sidb_simulation_parameters{3, -0.32}, sidb_charge_state::NEGATIVE};

    cl.assign_sidb_defect({18, 3, 0}, sidb_defect{sidb_defect_type::UNKNOWN, 1});
    cl.assign_sidb_defect({44, 2, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1});
    cl.assign_sidb_defect({40, 5, 1}, sidb_defect{sidb_defect_type::UNKNOWN, 0});
    cl.assign_sidb_defect({42, 20, 0}, sidb_defect{sidb_defect_type::UNKNOWN, 1});

    cl.assign_charge_state({42, 3, 0}, sidb_charge_state::NEGATIVE);

    cl.assign_charge_state({16, 3, 0}, sidb_charge_state::NEUTRAL);
    cl.assign_charge_state({42, 3, 0}, sidb_charge_state::NEGATIVE);
    cl.assign_charge_state({18, 4, 0}, sidb_charge_state::NEGATIVE);
    cl.assign_charge_state({40, 4, 0}, sidb_charge_state::NEGATIVE);
    cl.assign_charge_state({22, 5, 0}, sidb_charge_state::NEGATIVE);
    cl.assign_charge_state({36, 5, 0}, sidb_charge_state::NEUTRAL);
    cl.assign_charge_state({24, 6, 0}, sidb_charge_state::NEGATIVE);
    cl.assign_charge_state({34, 6, 0}, sidb_charge_state::POSITIVE);
    cl.assign_charge_state({27, 9, 0}, sidb_charge_state::NEGATIVE);
    cl.assign_charge_state({32, 10, 0}, sidb_charge_state::POSITIVE);
    cl.assign_charge_state({27, 10, 1}, sidb_charge_state::NEGATIVE);
    cl.assign_charge_state({29, 11, 1}, sidb_charge_state::NEGATIVE);
    cl.assign_charge_state({29, 14, 0}, sidb_charge_state::NEGATIVE);
    cl.assign_charge_state({30, 15, 0}, sidb_charge_state::NEGATIVE);
    cl.assign_charge_state({34, 16, 0}, sidb_charge_state::POSITIVE);
    cl.assign_charge_state({36, 17, 0}, sidb_charge_state::NEUTRAL);
    cl.assign_charge_state({40, 18, 0}, sidb_charge_state::NEGATIVE);
    cl.assign_charge_state({42, 19, 0}, sidb_charge_state::NEGATIVE);

    std::stringstream print_stream{};

    print_sidb_layout(print_stream, cl, false, true, true);

    constexpr const char* layout_print =
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ⊟  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ◯  ·  ⊞  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ◯  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ⊡  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ◯  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ●  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ⊞  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n";

    CHECK(layout_print == print_stream.str());
}

TEST_CASE("Print layout without charges but defects", "[print-charge-layout]")
{
    const sidb_100_cell_clk_lyt layout{};

    sidb_defect_surface<sidb_cell_clk_lyt_siqad> cl{convert_layout_to_siqad_coordinates(layout)};

    cl.assign_cell_type({0, 0, 0}, sidb_cell_clk_lyt_siqad::technology::NORMAL);
    cl.assign_cell_type({1, 0, 1}, sidb_cell_clk_lyt_siqad::technology::NORMAL);
    cl.assign_cell_type({4, 0, 1}, sidb_cell_clk_lyt_siqad::technology::NORMAL);

    cl.assign_sidb_defect({0, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, 1});
    cl.assign_sidb_defect({1, 0, 1}, sidb_defect{sidb_defect_type::UNKNOWN, 0});
    cl.assign_sidb_defect({4, 0, 1}, sidb_defect{sidb_defect_type::UNKNOWN, -1});

    std::stringstream print_stream{};

    SECTION("crop_layout option activated")
    {
        print_sidb_layout(print_stream,
                          sidb_lattice<sidb_100_lattice, sidb_defect_surface<sidb_cell_clk_lyt_siqad>>{cl}, false, true,
                          true);

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
        print_sidb_layout(print_stream,
                          sidb_lattice<sidb_100_lattice, sidb_defect_surface<sidb_cell_clk_lyt_siqad>>{cl}, false,
                          false, true);

        constexpr const char* layout_print = " ⊞  ·  ·  ·  · \n"
                                             " ·  ⊡  ·  ·  ⊟ \n";

        CHECK(layout_print == print_stream.str());
    }
}

TEST_CASE("Print Bestagon OR-gate", "[print-charge-layout]")
{
    using hex_gate_lyt = hex_odd_row_gate_clk_lyt;

    hex_gate_lyt layout{aspect_ratio<hex_gate_lyt>{0, 0}};

    constexpr const char* layout_print =
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ◯  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ◯  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ◯  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ◯  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ◯  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ◯  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ◯  ·  ·  ·  ·  ·  ·  ·  ·  ·  ◯  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ◯  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ◯  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ◯  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ◯  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ◯  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ◯  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ◯  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ◯  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ◯  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ◯  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        "\n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
        " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n";

    SECTION("No lattice orientation is provided, SiDB cell-level layout is used as input")
    {
        layout.create_or({}, {}, {0, 0});

        const auto cell_layout_or       = apply_gate_library<sidb_cell_clk_lyt, sidb_bestagon_library>(layout);
        const auto cell_layout_or_siqad = convert_layout_to_siqad_coordinates(cell_layout_or);

        std::stringstream print_stream{};

        print_sidb_layout(print_stream, cell_layout_or_siqad, false, true, true);
        CHECK(layout_print == print_stream.str());
    }

    SECTION("Lattice orientation is provided, sidb lattice is used as input")
    {
        layout.create_or({}, {}, {0, 0});

        const auto cell_layout_or       = apply_gate_library<sidb_100_cell_clk_lyt, sidb_bestagon_library>(layout);
        const auto cell_layout_or_siqad = convert_layout_to_siqad_coordinates(cell_layout_or);

        std::stringstream print_stream{};

        print_sidb_layout(print_stream, cell_layout_or_siqad, false, true, true);
        CHECK(layout_print == print_stream.str());
    }
}

TEST_CASE("Print H-Si 111 surface with six cells, defined with siqad::coord_t", "[print-charge-layout]")
{
    sidb_cell_clk_lyt_siqad lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
    lyt.assign_cell_type({1, 1, 1}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
    lyt.assign_cell_type({2, 2, 1}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);

    lyt.assign_cell_type({8, 0, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
    lyt.assign_cell_type({6, 1, 1}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
    lyt.assign_cell_type({5, 2, 1}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);

    SECTION("Print the cell-level layout")
    {
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

        const sidb_lattice<sidb_111_lattice, decltype(lyt)> lattice_lyt{lyt};

        std::stringstream print_stream{};

        print_sidb_layout(print_stream, lattice_lyt, false, true, true);
        CHECK(layout_print == print_stream.str());
    }

    SECTION("Print charge distribution cell-level layout")
    {
        constexpr const char* layout_print = " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
                                             "  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
                                             " ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ●  ·  · \n"
                                             "  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
                                             " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
                                             "  ·  ·  ·  ●  ·  ·  ·  ·  ●  ·  ·  ·  · \n"
                                             " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
                                             "  ·  ·  ·  ·  ●  ·  ·  ●  ·  ·  ·  ·  · \n"
                                             " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
                                             "  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
                                             "\n"
                                             "\n";

        const sidb_lattice<sidb_111_lattice, decltype(lyt)> lattice_lyt{lyt};

        std::stringstream print_stream{};

        const auto cds = charge_distribution_surface{lattice_lyt};

        print_sidb_layout(print_stream, cds, false, true, true);
        CHECK(layout_print == print_stream.str());
    }
}

TEST_CASE("Print H-Si 111 surface with six cells, defined with offset::ucoord_t coordinates", "[print-charge-layout]")
{
    sidb_cell_clk_lyt lyt{};

    lyt.assign_cell_type({0, 0}, sidb_cell_clk_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({1, 3}, sidb_cell_clk_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({2, 5}, sidb_cell_clk_lyt::cell_type::NORMAL);

    lyt.assign_cell_type({8, 0}, sidb_cell_clk_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({6, 3}, sidb_cell_clk_lyt::cell_type::NORMAL);
    lyt.assign_cell_type({5, 5}, sidb_cell_clk_lyt::cell_type::NORMAL);

    SECTION("Print the cell-level layout")
    {
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

        const sidb_lattice<sidb_111_lattice, decltype(lyt)> lattice_lyt{lyt};

        std::stringstream print_stream{};

        print_sidb_layout(print_stream, lattice_lyt, false, true);
        CHECK(layout_print == print_stream.str());
    }

    SECTION("Print charge distribution cell-level layout")
    {
        constexpr const char* layout_print = " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
                                             "  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
                                             " ·  ·  ●  ·  ·  ·  ·  ·  ·  ·  ●  ·  · \n"
                                             "  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
                                             " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
                                             "  ·  ·  ·  ●  ·  ·  ·  ·  ●  ·  ·  ·  · \n"
                                             " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
                                             "  ·  ·  ·  ·  ●  ·  ·  ●  ·  ·  ·  ·  · \n"
                                             " ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
                                             "  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  ·  · \n"
                                             "\n"
                                             "\n";

        const sidb_lattice<sidb_111_lattice, decltype(lyt)> lattice_lyt{lyt};

        std::stringstream print_stream{};

        const auto cds = charge_distribution_surface{lattice_lyt};

        print_sidb_layout(print_stream, cds, false, true);
        CHECK(layout_print == print_stream.str());
    }
}
