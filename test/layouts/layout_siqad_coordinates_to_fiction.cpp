//
// Created by Jan Drewniok on 12.02.23.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/layout_siqad_coordinates_to_fiction.hpp>
#include <fiction/technology/cell_technologies.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE("Cell-level layout traits", "[layout_siqad_coordinates_to_fiction]", sidb_cell_clk_lyt_siqad)
{
    SECTION("empty layout")
    {
        TestType lyt{};
        auto     lyt_transformed = lyt_coordinates_to_fiction<TestType>(lyt);
        CHECK(lyt_transformed.is_empty());
    }

    SECTION("layout with one cell")
    {
        TestType lyt{};
        lyt.assign_cell_type({5, 3}, TestType::cell_type::NORMAL);
        auto lyt_transformed = lyt_coordinates_to_fiction<TestType>(lyt);
        CHECK(lyt_transformed.num_cells() == 1);
        CHECK(lyt_transformed.get_cell_type({5, 6}) == TestType::cell_type::NORMAL);
    }

    SECTION("layout with three cells")
    {
        TestType lyt{};
        lyt.assign_cell_type({5, 3}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 1}, TestType::cell_type::NORMAL);
        auto lyt_transformed = lyt_coordinates_to_fiction<TestType>(lyt);
        CHECK(lyt_transformed.num_cells() == 3);
        CHECK(lyt_transformed.get_cell_type({5, 6}) == TestType::cell_type::NORMAL);
        CHECK(lyt_transformed.get_cell_type({5, 2}) == TestType::cell_type::NORMAL);
        CHECK(lyt_transformed.get_cell_type({0, 0}) == TestType::cell_type::NORMAL);
    }
}