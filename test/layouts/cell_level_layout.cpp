//
// Created by marcel on 24.06.21.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>

#include <string>

using namespace fiction;

TEMPLATE_TEST_CASE("Cell-level layout traits", "[cell-level-layout]", qca_cell_clk_lyt, stacked_qca_cell_clk_lyt,
                   inml_cell_clk_lyt, sidb_100_cell_clk_lyt, sidb_111_cell_clk_lyt, sidb_111_cell_clk_lyt_siqad,
                   sidb_111_cell_clk_lyt_cube)
{
    CHECK(is_cell_level_layout_v<TestType>);
    CHECK(has_foreach_cell_v<TestType>);
    CHECK(has_is_empty_cell_v<TestType>);
    CHECK(has_is_empty_v<TestType>);
    CHECK(has_get_layout_name_v<TestType>);
    CHECK(has_set_layout_name_v<TestType>);
}

TEST_CASE("Deep copy cell-level layout", "[cell-level-layout]")
{
    using cell_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    cell_layout original{{5, 5, 0}, twoddwave_clocking<cell_layout>(), "Original", 2, 2};
    original.assign_cell_type({0, 2}, qca_technology::cell_type::NORMAL);
    original.assign_cell_type({2, 4}, qca_technology::cell_type::NORMAL);

    auto copy = original.clone();

    copy.resize({10, 10, 1});
    copy.replace_clocking_scheme(use_clocking<cell_layout>());
    copy.set_layout_name("Copy");
    copy.assign_cell_type({0, 2}, qca_technology::cell_type::INPUT);
    copy.assign_cell_type({2, 4}, qca_technology::cell_type::INPUT);

    CHECK(original.x() == 5);
    CHECK(original.y() == 5);
    CHECK(original.z() == 0);
    CHECK(original.is_clocking_scheme(clock_name::TWODDWAVE));
    CHECK(original.get_layout_name() == "Original");
    CHECK(original.get_cell_type({0, 2}) == qca_technology::cell_type::NORMAL);
    CHECK(original.get_cell_type({2, 4}) == qca_technology::cell_type::NORMAL);

    CHECK(copy.x() == 10);
    CHECK(copy.y() == 10);
    CHECK(copy.z() == 1);
    CHECK(copy.is_clocking_scheme(clock_name::USE));
    CHECK(copy.get_layout_name() == "Copy");
    CHECK(copy.get_cell_type({0, 2}) == qca_technology::cell_type::INPUT);
    CHECK(copy.get_cell_type({2, 4}) == qca_technology::cell_type::INPUT);
}

TEST_CASE("Cell technology", "[cell-level-layout]")
{
    SECTION("QCA")
    {
        CHECK(qca_technology::is_empty_cell(qca_technology::cell_type::EMPTY));
        CHECK(qca_technology::is_normal_cell(qca_technology::cell_type::NORMAL));
        CHECK(qca_technology::is_input_cell(qca_technology::cell_type::INPUT));
        CHECK(qca_technology::is_output_cell(qca_technology::cell_type::OUTPUT));
        CHECK(qca_technology::is_const_0_cell(qca_technology::cell_type::CONST_0));
        CHECK(qca_technology::is_const_1_cell(qca_technology::cell_type::CONST_1));
        CHECK(qca_technology::is_constant_cell(qca_technology::cell_type::CONST_0));
        CHECK(qca_technology::is_constant_cell(qca_technology::cell_type::CONST_1));

        CHECK(qca_technology::is_normal_cell_mode(qca_technology::cell_mode::NORMAL));
        CHECK(qca_technology::is_rotated_cell_mode(qca_technology::cell_mode::ROTATED));
        CHECK(qca_technology::is_vertical_cell_mode(qca_technology::cell_mode::VERTICAL));
        CHECK(qca_technology::is_crossover_cell_mode(qca_technology::cell_mode::CROSSOVER));

        CHECK(tech_impl_name<qca_technology> == std::string{"QCA"});

        CHECK(has_qca_technology_v<qca_cell_clk_lyt>);
        CHECK(has_qca_technology_v<stacked_qca_cell_clk_lyt>);
    }
    SECTION("iNML")
    {
        CHECK(inml_technology::is_empty_cell(inml_technology::cell_type::EMPTY));
        CHECK(inml_technology::is_normal_cell(inml_technology::cell_type::NORMAL));
        CHECK(inml_technology::is_input_cell(inml_technology::cell_type::INPUT));
        CHECK(inml_technology::is_output_cell(inml_technology::cell_type::OUTPUT));
        CHECK(inml_technology::is_slanted_edge_up_magnet(inml_technology::cell_type::SLANTED_EDGE_UP_MAGNET));
        CHECK(inml_technology::is_slanted_edge_down_magnet(inml_technology::cell_type::SLANTED_EDGE_DOWN_MAGNET));
        CHECK(inml_technology::is_slanted_edge_magnet(inml_technology::cell_type::SLANTED_EDGE_UP_MAGNET));
        CHECK(inml_technology::is_slanted_edge_magnet(inml_technology::cell_type::SLANTED_EDGE_DOWN_MAGNET));
        CHECK(inml_technology::is_inverter_magnet(inml_technology::cell_type::INVERTER_MAGNET));
        CHECK(inml_technology::is_crosswire_magnet(inml_technology::cell_type::CROSSWIRE_MAGNET));
        CHECK(inml_technology::is_fanout_coupler_magnet(inml_technology::cell_type::FANOUT_COUPLER_MAGNET));

        CHECK(inml_technology::is_normal_cell_mode(inml_technology::cell_mode{}));

        CHECK(tech_impl_name<inml_technology> == std::string{"iNML"});

        CHECK(has_inml_technology_v<inml_cell_clk_lyt>);
    }
    SECTION("SiDB")
    {
        CHECK(sidb_technology::is_empty_cell(sidb_technology::cell_type::EMPTY));
        CHECK(sidb_technology::is_normal_cell(sidb_technology::cell_type::NORMAL));
        CHECK(sidb_technology::is_input_cell(sidb_technology::cell_type::INPUT));
        CHECK(sidb_technology::is_output_cell(sidb_technology::cell_type::OUTPUT));
        CHECK(sidb_technology::is_logic_cell(sidb_technology::cell_type::LOGIC));

        CHECK(sidb_technology::is_normal_cell_mode(sidb_technology::cell_mode{}));

        CHECK(tech_impl_name<sidb_technology> == std::string{"SiDB"});

        CHECK(has_sidb_technology_v<sidb_100_cell_clk_lyt>);
        CHECK(has_sidb_technology_v<sidb_111_cell_clk_lyt>);
    }
}

TEST_CASE("Cell type assignment", "[cell-level-layout]")
{
    using cell_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    REQUIRE(has_get_layout_name_v<cell_layout>);
    REQUIRE(has_set_layout_name_v<cell_layout>);

    cell_layout layout{cell_layout::aspect_ratio{4, 4}, "AND"};

    CHECK(layout.is_empty());

    layout.assign_cell_type({0, 2}, qca_technology::cell_type::INPUT);
    layout.assign_cell_type({2, 4}, qca_technology::cell_type::INPUT);
    layout.assign_cell_type({2, 0}, qca_technology::cell_type::CONST_0);
    layout.assign_cell_type({2, 1}, qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 2}, qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 3}, qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({1, 2}, qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({3, 2}, qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({4, 2}, qca_technology::cell_type::OUTPUT);

    CHECK(!layout.is_empty());

    layout.assign_cell_name({0, 2}, "a");
    layout.assign_cell_name({2, 4}, "b");
    layout.assign_cell_name({4, 2}, "f");

    CHECK(layout.get_layout_name() == "AND");
    CHECK(layout.get_cell_name({0, 2}) == "a");
    CHECK(layout.get_cell_name({2, 4}) == "b");
    CHECK(layout.get_cell_name({4, 2}) == "f");

    CHECK(layout.num_cells() == 9);
    CHECK(layout.num_pis() == 2);
    CHECK(layout.num_pos() == 1);

    CHECK(layout.is_pi({0, 2}));
    CHECK(layout.is_pi({2, 4}));
    CHECK(layout.is_po({4, 2}));

    CHECK(layout.get_cell_type({2, 0}) == qca_technology::cell_type::CONST_0);
    CHECK(layout.get_cell_type({2, 4}) == qca_technology::cell_type::INPUT);
    CHECK(layout.get_cell_type({0, 2}) == qca_technology::cell_type::INPUT);
    CHECK(layout.get_cell_type({2, 1}) == qca_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({2, 2}) == qca_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({2, 3}) == qca_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({1, 2}) == qca_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({3, 2}) == qca_technology::cell_type::NORMAL);
    CHECK(layout.get_cell_type({4, 2}) == qca_technology::cell_type::OUTPUT);

    CHECK(layout.is_empty_cell({0, 0}));
    CHECK(layout.is_empty_cell({0, 1}));
    CHECK(layout.is_empty_cell({1, 0}));
    CHECK(layout.is_empty_cell({1, 1}));
    CHECK(layout.is_empty_cell({3, 0}));
    CHECK(layout.is_empty_cell({3, 1}));
    CHECK(layout.is_empty_cell({4, 0}));
    CHECK(layout.is_empty_cell({4, 1}));
    CHECK(layout.is_empty_cell({0, 3}));
    CHECK(layout.is_empty_cell({1, 3}));
    CHECK(layout.is_empty_cell({0, 4}));
    CHECK(layout.is_empty_cell({1, 0}));
    CHECK(layout.is_empty_cell({3, 3}));
    CHECK(layout.is_empty_cell({3, 4}));
    CHECK(layout.is_empty_cell({4, 3}));
    CHECK(layout.is_empty_cell({4, 4}));

    CHECK(!layout.is_empty_cell({2, 0}));
    CHECK(!layout.is_empty_cell({2, 4}));
    CHECK(!layout.is_empty_cell({0, 2}));
    CHECK(!layout.is_empty_cell({2, 1}));
    CHECK(!layout.is_empty_cell({2, 2}));
    CHECK(!layout.is_empty_cell({2, 3}));
    CHECK(!layout.is_empty_cell({1, 2}));
    CHECK(!layout.is_empty_cell({3, 2}));
    CHECK(!layout.is_empty_cell({4, 2}));

    // remove cells by assigning them the empty cell type
    layout.assign_cell_type({0, 2}, qca_technology::cell_type::EMPTY);
    layout.assign_cell_type({2, 0}, qca_technology::cell_type::EMPTY);
    layout.assign_cell_type({2, 1}, qca_technology::cell_type::EMPTY);
    layout.assign_cell_type({4, 2}, qca_technology::cell_type::EMPTY);

    CHECK(layout.is_empty_cell({0, 2}));
    CHECK(layout.is_empty_cell({2, 0}));
    CHECK(layout.is_empty_cell({2, 1}));
    CHECK(layout.is_empty_cell({4, 2}));

    // this should also have reduced the number of cells, PIs, and POs
    CHECK(layout.num_cells() == 5);
    CHECK(layout.num_pis() == 1);
    CHECK(layout.num_pos() == 0);

    // remove cell names by assigning them the empty name
    layout.assign_cell_name({2, 4}, "");
    CHECK(layout.get_cell_name({2, 4}).empty());
}

TEST_CASE("Cell mode assignment", "[cell-level-layout]")
{
    using cell_layout = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    cell_layout layout{cell_layout::aspect_ratio{4, 4, 1}, "Crossover"};

    layout.assign_cell_type({0, 2, 0}, qca_technology::cell_type::INPUT);
    layout.assign_cell_type({2, 0, 0}, qca_technology::cell_type::INPUT);
    layout.assign_cell_type({1, 2, 0}, qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 2, 0}, qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({3, 2, 0}, qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 1, 1}, qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 2, 1}, qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 3, 1}, qca_technology::cell_type::NORMAL);
    layout.assign_cell_type({2, 4, 0}, qca_technology::cell_type::OUTPUT);
    layout.assign_cell_type({4, 2, 0}, qca_technology::cell_type::OUTPUT);

    layout.assign_cell_mode({2, 1, 1}, qca_technology::cell_mode::CROSSOVER);
    layout.assign_cell_mode({2, 2, 1}, qca_technology::cell_mode::CROSSOVER);
    layout.assign_cell_mode({2, 3, 1}, qca_technology::cell_mode::CROSSOVER);

    layout.assign_cell_name({0, 2}, "a");
    layout.assign_cell_name({2, 0}, "b");
    layout.assign_cell_name({4, 2}, "a'");
    layout.assign_cell_name({2, 4}, "b'");

    CHECK(layout.get_layout_name() == "Crossover");
    CHECK(layout.get_cell_name({0, 2}) == "a");
    CHECK(layout.get_cell_name({2, 0}) == "b");
    CHECK(layout.get_cell_name({4, 2}) == "a'");
    CHECK(layout.get_cell_name({2, 4}) == "b'");

    CHECK(layout.num_cells() == 10);
    CHECK(layout.num_pis() == 2);
    CHECK(layout.num_pos() == 2);

    CHECK(layout.is_pi({0, 2}));
    CHECK(layout.is_pi({2, 0}));
    CHECK(layout.is_po({2, 4}));
    CHECK(layout.is_po({4, 2}));

    CHECK(layout.get_cell_mode({0, 2, 0}) == qca_technology::cell_mode::NORMAL);
    CHECK(layout.get_cell_mode({2, 0, 0}) == qca_technology::cell_mode::NORMAL);
    CHECK(layout.get_cell_mode({1, 2, 0}) == qca_technology::cell_mode::NORMAL);
    CHECK(layout.get_cell_mode({2, 2, 0}) == qca_technology::cell_mode::NORMAL);
    CHECK(layout.get_cell_mode({3, 2, 0}) == qca_technology::cell_mode::NORMAL);
    CHECK(layout.get_cell_mode({2, 4, 0}) == qca_technology::cell_mode::NORMAL);
    CHECK(layout.get_cell_mode({4, 2, 0}) == qca_technology::cell_mode::NORMAL);

    CHECK(layout.get_cell_mode({2, 1, 1}) == qca_technology::cell_mode::CROSSOVER);
    CHECK(layout.get_cell_mode({2, 2, 1}) == qca_technology::cell_mode::CROSSOVER);
    CHECK(layout.get_cell_mode({2, 3, 1}) == qca_technology::cell_mode::CROSSOVER);

    // remove cell modes by assigning the normal mode
    layout.assign_cell_mode({2, 1, 1}, qca_technology::cell_mode::NORMAL);
    layout.assign_cell_mode({2, 2, 1}, qca_technology::cell_mode::NORMAL);
    layout.assign_cell_mode({2, 3, 1}, qca_technology::cell_mode::NORMAL);

    CHECK(layout.get_cell_mode({2, 1, 1}) == qca_technology::cell_mode::NORMAL);
    CHECK(layout.get_cell_mode({2, 2, 1}) == qca_technology::cell_mode::NORMAL);
    CHECK(layout.get_cell_mode({2, 3, 1}) == qca_technology::cell_mode::NORMAL);
}

TEST_CASE("Clock zone assignment to cells", "[cell-level-layout]")
{
    using clk_cell_lyt = cell_level_layout<qca_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>;

    clk_cell_lyt layout{clk_cell_lyt::aspect_ratio{4, 4, 0}, twoddwave_clocking<clk_cell_lyt>(), "Lyt", 2, 2};

    CHECK(layout.get_clock_number({0, 0}) == 0);
    CHECK(layout.get_clock_number({0, 1}) == 0);
    CHECK(layout.get_clock_number({1, 0}) == 0);
    CHECK(layout.get_clock_number({1, 1}) == 0);
    CHECK(layout.get_clock_number({2, 0}) == 1);
    CHECK(layout.get_clock_number({2, 1}) == 1);
    CHECK(layout.get_clock_number({3, 0}) == 1);
    CHECK(layout.get_clock_number({3, 1}) == 1);
    CHECK(layout.get_clock_number({0, 2}) == 1);
    CHECK(layout.get_clock_number({0, 3}) == 1);
    CHECK(layout.get_clock_number({1, 2}) == 1);
    CHECK(layout.get_clock_number({1, 3}) == 1);
    CHECK(layout.get_clock_number({2, 2}) == 2);
    CHECK(layout.get_clock_number({2, 3}) == 2);
    CHECK(layout.get_clock_number({3, 2}) == 2);
    CHECK(layout.get_clock_number({3, 3}) == 2);
    CHECK(layout.get_clock_number({4, 0}) == 2);
    CHECK(layout.get_clock_number({4, 1}) == 2);
    CHECK(layout.get_clock_number({4, 2}) == 3);
    CHECK(layout.get_clock_number({4, 3}) == 3);
    CHECK(layout.get_clock_number({0, 4}) == 2);
    CHECK(layout.get_clock_number({1, 4}) == 2);
    CHECK(layout.get_clock_number({2, 4}) == 3);
    CHECK(layout.get_clock_number({3, 4}) == 3);
    CHECK(layout.get_clock_number({4, 4}) == 0);
}
