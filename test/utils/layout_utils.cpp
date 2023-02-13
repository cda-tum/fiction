//
// Created by marcel on 28.07.22.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/utils/layout_utils.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE("Port directions to coordinates", "[layout-utils]", (cartesian_layout<offset::ucoord_t>),
                   (hexagonal_layout<offset::ucoord_t, odd_row_hex>),
                   (hexagonal_layout<offset::ucoord_t, even_row_hex>),
                   (hexagonal_layout<offset::ucoord_t, odd_column_hex>),
                   (hexagonal_layout<offset::ucoord_t, even_column_hex>))
{
    TestType lyt{{4, 4}};

    lyt.foreach_coordinate(
        [&lyt](const auto& c)
        {
            CHECK(port_direction_to_coordinate(lyt, c, port_direction{port_direction::cardinal::NORTH}) ==
                  lyt.north(c));
            CHECK(port_direction_to_coordinate(lyt, c, port_direction{port_direction::cardinal::NORTH_EAST}) ==
                  lyt.north_east(c));
            CHECK(port_direction_to_coordinate(lyt, c, port_direction{port_direction::cardinal::EAST}) == lyt.east(c));
            CHECK(port_direction_to_coordinate(lyt, c, port_direction{port_direction::cardinal::SOUTH_EAST}) ==
                  lyt.south_east(c));
            CHECK(port_direction_to_coordinate(lyt, c, port_direction{port_direction::cardinal::SOUTH}) ==
                  lyt.south(c));
            CHECK(port_direction_to_coordinate(lyt, c, port_direction{port_direction::cardinal::SOUTH_WEST}) ==
                  lyt.south_west(c));
            CHECK(port_direction_to_coordinate(lyt, c, port_direction{port_direction::cardinal::WEST}) == lyt.west(c));
            CHECK(port_direction_to_coordinate(lyt, c, port_direction{port_direction::cardinal::NORTH_WEST}) ==
                  lyt.north_west(c));
        });
}

TEMPLATE_TEST_CASE("Cell-level layout trait", "[layout_fiction_coordinates_to_siqad]", sidb_cell_clk_lyt)
{
    SECTION("empty layout")
    {
        TestType lyt{{10, 10}, "test"};
        auto     lyt_transformed = lyt_coordinates_to_siqad<TestType>(lyt);
        CHECK(lyt_transformed.is_empty());
        CHECK(lyt_transformed.get_layout_name() == "test");
    }
    //
    SECTION("layout with one normal and one input cell")
    {
        TestType lyt{};
        lyt.assign_cell_type({5, 3}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 1}, TestType::cell_type::INPUT);
        auto lyt_transformed = lyt_coordinates_to_siqad<TestType>(lyt);
        CHECK(lyt_transformed.num_cells() == 2);
        CHECK(lyt_transformed.get_cell_type({5, 1, 1}) == TestType::cell_type::NORMAL);
        CHECK(lyt_transformed.get_cell_type({5, 0, 1}) == TestType::cell_type::INPUT);
    }

    SECTION("layout with three cells")
    {
        TestType lyt{};
        lyt.assign_cell_type({0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 3}, TestType::cell_type::INPUT);
        lyt.assign_cell_type({5, 1}, TestType::cell_type::OUTPUT);
        lyt.assign_cell_name({0, 0}, "normal cell");
        lyt.assign_cell_name({5, 3}, "input cell");
        lyt.assign_cell_name({5, 1}, "output cell");
        auto lyt_transformed = lyt_coordinates_to_siqad<TestType>(lyt);
        CHECK(lyt_transformed.num_cells() == 3);
        CHECK(lyt_transformed.get_cell_type({0, 0, 0}) == TestType::cell_type::NORMAL);
        CHECK(lyt_transformed.get_cell_type({5, 1, 1}) == TestType::cell_type::INPUT);
        CHECK(lyt_transformed.get_cell_type({5, 0, 1}) == TestType::cell_type::OUTPUT);
        CHECK(lyt_transformed.get_cell_name({0, 0, 0}) == "normal cell");
        CHECK(lyt_transformed.get_cell_name({5, 1, 1}) == "input cell");
        CHECK(lyt_transformed.get_cell_name({5, 0, 1}) == "output cell");
    }
}

TEMPLATE_TEST_CASE("Cell-level layout traits", "[layout_siqad_coordinates_to_fiction]", sidb_cell_clk_lyt_siqad)
{
    SECTION("empty layout")
    {
        TestType lyt{{}, "layout based on siqad coordinates"};
        auto     lyt_transformed = lyt_coordinates_to_fiction<TestType>(lyt);
        CHECK(lyt_transformed.is_empty());
        CHECK(lyt_transformed.get_layout_name() == "layout based on siqad coordinates");
    }

    SECTION("layout with one normal and one input cell")
    {
        TestType lyt{};
        lyt.assign_cell_type({5, 3}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 1}, TestType::cell_type::INPUT);
        auto lyt_transformed = lyt_coordinates_to_fiction<TestType>(lyt);
        CHECK(lyt_transformed.num_cells() == 2);
        CHECK(lyt_transformed.get_cell_type({5, 6}) == TestType::cell_type::NORMAL);
        CHECK(lyt_transformed.get_cell_type({5, 2}) == TestType::cell_type::INPUT);
    }

    SECTION("layout with three cells")
    {
        TestType lyt{};
        lyt.assign_cell_type({5, 3}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({0, 0}, TestType::cell_type::INPUT);
        lyt.assign_cell_type({5, 1}, TestType::cell_type::OUTPUT);
        lyt.assign_cell_name({5, 3}, "normal cell");
        lyt.assign_cell_name({0, 0}, "input cell");
        lyt.assign_cell_name({5, 1}, "output cell");
        auto lyt_transformed = lyt_coordinates_to_fiction<TestType>(lyt);
        CHECK(lyt_transformed.num_cells() == 3);
        CHECK(lyt_transformed.get_cell_type({5, 6}) == TestType::cell_type::NORMAL);
        CHECK(lyt_transformed.get_cell_type({0, 0}) == TestType::cell_type::INPUT);
        CHECK(lyt_transformed.get_cell_type({5, 2}) == TestType::cell_type::OUTPUT);
        CHECK(lyt_transformed.get_cell_name({5, 6}) == "normal cell");
        CHECK(lyt_transformed.get_cell_name({0, 0}) == "input cell");
        CHECK(lyt_transformed.get_cell_name({5, 2}) == "output cell");
    }
}
