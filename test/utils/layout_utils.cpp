//
// Created by marcel on 28.07.22.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/types.hpp>
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

TEMPLATE_TEST_CASE("siqad layout is normalized, shifted to positive coordinates", "[layout-utils]",
                   sidb_cell_clk_lyt_siqad)
{
    SECTION("empty layout")
    {
        TestType lyt{{}, "layout based on siqad coordinates"};

        lyt.assign_cell_type({-5, -1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 1}, TestType::cell_type::NORMAL);

        auto lyt_transformed = normalize_layout_coordinates<TestType>(lyt);

        CHECK(lyt_transformed.get_layout_name() == "layout based on siqad coordinates");
        CHECK(lyt_transformed.get_cell_type({0, 0}) == TestType::cell_type::NORMAL);
        CHECK(lyt_transformed.get_cell_type({10, 2}) == TestType::cell_type::NORMAL);
        CHECK(lyt_transformed.get_cell_type({-5, -1}) == TestType::cell_type::EMPTY);
    }
}

template <typename T1, typename T2>
inline auto area_with_padding(const uint64_t& area, const T1& x, const T2& y) noexcept
{
    return area + (static_cast<uint64_t>(x) + 1) * ((static_cast<uint64_t>(y) + 1) % 2ul);
}

TEMPLATE_TEST_CASE("Convert offset::ucoord_t layout to SiQAD coordinate layout", "[layout-utils]", sidb_cell_clk_lyt)
{
    SECTION("empty layout")
    {
        const auto x = 10, y = 10;

        TestType lyt{{x, y}, "test"};

        auto lyt_transformed = convert_to_siqad_coordinates<TestType>(lyt);

        CHECK(lyt_transformed.is_empty());
        CHECK(lyt_transformed.area() == area_with_padding(lyt.area(), x, y));
        CHECK(lyt_transformed.get_layout_name() == lyt.get_layout_name());
    }

    SECTION("layout with one normal and one input cell")
    {
        const auto x = 5, y = 3;

        TestType lyt{{x, y}};

        lyt.assign_cell_type({5, 3}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 1}, TestType::cell_type::INPUT);

        auto lyt_transformed = convert_to_siqad_coordinates<TestType>(lyt);

        CHECK(lyt_transformed.num_cells() == 2);
        CHECK(lyt_transformed.area() == area_with_padding(lyt.area(), x, y));
        CHECK(lyt_transformed.get_cell_type({5, 1, 1}) == TestType::cell_type::NORMAL);
        CHECK(lyt_transformed.get_cell_type({5, 0, 1}) == TestType::cell_type::INPUT);
    }

    SECTION("layout with three cells")
    {
        const auto x = 5, y = 3;

        TestType lyt{{x, y}};

        lyt.assign_cell_type({0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 3}, TestType::cell_type::INPUT);
        lyt.assign_cell_type({5, 1}, TestType::cell_type::OUTPUT);
        lyt.assign_cell_name({0, 0}, "normal cell");
        lyt.assign_cell_name({5, 3}, "input cell");
        lyt.assign_cell_name({5, 1}, "output cell");

        auto lyt_transformed = convert_to_siqad_coordinates<TestType>(lyt);

        CHECK(lyt_transformed.num_cells() == 3);
        CHECK(lyt_transformed.area() == area_with_padding(lyt.area(), x, y));
        CHECK(lyt_transformed.get_cell_type({0, 0, 0}) == TestType::cell_type::NORMAL);
        CHECK(lyt_transformed.get_cell_type({5, 1, 1}) == TestType::cell_type::INPUT);
        CHECK(lyt_transformed.get_cell_type({5, 0, 1}) == TestType::cell_type::OUTPUT);
        CHECK(lyt_transformed.get_cell_name({0, 0, 0}) == "normal cell");
        CHECK(lyt_transformed.get_cell_name({5, 1, 1}) == "input cell");
        CHECK(lyt_transformed.get_cell_name({5, 0, 1}) == "output cell");
    }
}

TEMPLATE_TEST_CASE("Convert SiQAD layout to offset::ucoord_t coordinate layout", "[layout-utils]", sidb_cell_clk_lyt)
{
    SECTION("empty layout")
    {
        const sidb_cell_clk_lyt_siqad lyt{{}, "layout based on siqad coordinates"};

        auto lyt_transformed = convert_to_fiction_coordinates<TestType>(lyt);

        CHECK(lyt_transformed.is_empty());
        CHECK(lyt_transformed.area() == lyt.area());
        CHECK(lyt_transformed.get_layout_name() == lyt.get_layout_name());
    }

    SECTION("layout with one normal and one input cell")
    {
        sidb_cell_clk_lyt_siqad lyt{{5, 3}};

        lyt.assign_cell_type({5, 3}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({-5, -1}, TestType::cell_type::INPUT);
        CHECK(lyt.x() == 5);
        CHECK(lyt.y() == 3);

        auto lyt_transformed = convert_to_fiction_coordinates<TestType>(lyt);

        CHECK(lyt_transformed.x() == 10);
        CHECK(lyt_transformed.y() == 9);
        CHECK(lyt_transformed.num_cells() == 2);
        CHECK(lyt_transformed.get_cell_type({10, 8}) == TestType::cell_type::NORMAL);
        CHECK(lyt_transformed.get_cell_type({0, 0}) == TestType::cell_type::INPUT);
    }

    SECTION("layout with three cells")
    {
        sidb_cell_clk_lyt_siqad lyt{{5, 3}};

        lyt.assign_cell_type({5, 3}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({0, 0}, TestType::cell_type::INPUT);
        lyt.assign_cell_type({5, 1}, TestType::cell_type::OUTPUT);
        lyt.assign_cell_name({5, 3}, "normal cell");
        lyt.assign_cell_name({0, 0}, "input cell");
        lyt.assign_cell_name({5, 1}, "output cell");

        auto lyt_transformed = convert_to_fiction_coordinates<TestType>(lyt);

        CHECK(lyt_transformed.num_cells() == 3);
        CHECK(lyt_transformed.area() == lyt.area());
        CHECK(lyt_transformed.get_cell_type({5, 6}) == TestType::cell_type::NORMAL);
        CHECK(lyt_transformed.get_cell_type({0, 0}) == TestType::cell_type::INPUT);
        CHECK(lyt_transformed.get_cell_type({5, 2}) == TestType::cell_type::OUTPUT);
        CHECK(lyt_transformed.get_cell_name({5, 6}) == "normal cell");
        CHECK(lyt_transformed.get_cell_name({0, 0}) == "input cell");
        CHECK(lyt_transformed.get_cell_name({5, 2}) == "output cell");
    }
}

TEMPLATE_TEST_CASE("Convert SiQAD layout to cube::coord_t coordinate layout", "[layout-utils]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<cube::coord_t>>>))
{
    SECTION("empty layout")
    {
        const sidb_cell_clk_lyt_siqad lyt{{}, "layout based on siqad coordinates"};

        auto lyt_transformed = convert_to_fiction_coordinates<TestType>(lyt);

        CHECK(lyt_transformed.is_empty());
        CHECK(lyt_transformed.area() == lyt.area());
        CHECK(lyt_transformed.get_layout_name() == lyt.get_layout_name());
    }

    SECTION("layout with one normal and one input cell")
    {
        sidb_cell_clk_lyt_siqad lyt{{5, 1}};

        lyt.assign_cell_type({5, -1, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 1, 0}, TestType::cell_type::INPUT);

        auto lyt_transformed = convert_to_fiction_coordinates<TestType>(lyt);

        CHECK(lyt_transformed.num_cells() == 2);
        CHECK(lyt_transformed.area() == lyt.area());
        CHECK(lyt_transformed.get_cell_type({5, -1}) == TestType::cell_type::NORMAL);
        CHECK(lyt_transformed.get_cell_type({5, 2, 0}) == TestType::cell_type::INPUT);
    }

    SECTION("layout with three cells")
    {
        sidb_cell_clk_lyt_siqad lyt{{5, 3}};

        lyt.assign_cell_type({5, -3}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({0, 0}, TestType::cell_type::INPUT);
        lyt.assign_cell_type({5, 3}, TestType::cell_type::OUTPUT);
        lyt.assign_cell_name({5, -3}, "normal cell");
        lyt.assign_cell_name({0, 0}, "input cell");
        lyt.assign_cell_name({5, 3}, "output cell");

        auto lyt_transformed = convert_to_fiction_coordinates<TestType>(lyt);

        CHECK(lyt_transformed.num_cells() == 3);
        CHECK(lyt_transformed.area() == lyt.area());
        CHECK(lyt_transformed.get_cell_type({5, -6}) == TestType::cell_type::NORMAL);
        CHECK(lyt_transformed.get_cell_type({0, 0}) == TestType::cell_type::INPUT);
        CHECK(lyt_transformed.get_cell_type({5, 6}) == TestType::cell_type::OUTPUT);
        CHECK(lyt_transformed.get_cell_name({5, -6}) == "normal cell");
        CHECK(lyt_transformed.get_cell_name({0, 0}) == "input cell");
        CHECK(lyt_transformed.get_cell_name({5, 6}) == "output cell");
    }
}

TEST_CASE("Generate random offset::ucoord_t coordinate", "[layout-utils]")
{
    SECTION("two identical cells as input")
    {
        const auto randomly_generated_coordinate = random_coordinate<offset::ucoord_t>({0, 0, 0}, {0, 0, 0});
        CHECK(randomly_generated_coordinate.x == 0);
        CHECK(randomly_generated_coordinate.y == 0);
        CHECK(randomly_generated_coordinate.z == 0);

        const auto randomly_generated_coordinate_second = random_coordinate<offset::ucoord_t>({1, 0, 0}, {1, 0, 0});
        CHECK(randomly_generated_coordinate_second.x == 1);
        CHECK(randomly_generated_coordinate_second.y == 0);
        CHECK(randomly_generated_coordinate_second.z == 0);
    }

    SECTION("two unidentical cells as input, correct order")
    {
        const auto randomly_generated_coordinate_second = random_coordinate<offset::ucoord_t>({1, 1, 1}, {5, 2, 3});
        CHECK(randomly_generated_coordinate_second.x >= 1);
        CHECK(randomly_generated_coordinate_second.x <= 5);
        CHECK(randomly_generated_coordinate_second.y <= 2);
        CHECK(randomly_generated_coordinate_second.y >= 0);
        CHECK(randomly_generated_coordinate_second.z <= 3);
        CHECK(randomly_generated_coordinate_second.z >= 1);
    }

    SECTION("two unidentical cells as input, switched correct order")
    {
        const auto randomly_generated_coordinate = random_coordinate<offset::ucoord_t>({5, 2, 3}, {1, 1, 1});
        CHECK(randomly_generated_coordinate.x >= 1);
        CHECK(randomly_generated_coordinate.x <= 5);
        CHECK(randomly_generated_coordinate.y <= 2);
        CHECK(randomly_generated_coordinate.y >= 0);
        CHECK(randomly_generated_coordinate.z <= 3);
        CHECK(randomly_generated_coordinate.z >= 1);
    }
}

TEST_CASE("Generate random cube::coord_t coordinate", "[layout-utils]")
{
    SECTION("two identical cells as input")
    {
        const auto randomly_generated_coordinate = random_coordinate<cube::coord_t>({-10, -5, 0}, {-10, -5, 0});
        CHECK(randomly_generated_coordinate.x == -10);
        CHECK(randomly_generated_coordinate.y == -5);
        CHECK(randomly_generated_coordinate.z == 0);

        const auto randomly_generated_coordinate_second = random_coordinate<cube::coord_t>({1, 0, 0}, {1, 0, 0});
        CHECK(randomly_generated_coordinate_second.x == 1);
        CHECK(randomly_generated_coordinate_second.y == 0);
        CHECK(randomly_generated_coordinate_second.z == 0);
    }

    SECTION("two unidentical cells as input, correct order")
    {
        const auto randomly_generated_coordinate = random_coordinate<cube::coord_t>({-10, -1, 3}, {-10, -1, 6});
        CHECK(randomly_generated_coordinate.x == -10);
        CHECK(randomly_generated_coordinate.y == -1);
        CHECK(randomly_generated_coordinate.z >= 3);
        CHECK(randomly_generated_coordinate.z <= 6);
    }

    SECTION("two unidentical cells as input, switched correct order")
    {
        const auto randomly_generated_coordinate = random_coordinate<cube::coord_t>({-10, -1, 6}, {-10, -1, 3});
        CHECK(randomly_generated_coordinate.x == -10);
        CHECK(randomly_generated_coordinate.y == -1);
        CHECK(randomly_generated_coordinate.z >= 3);
        CHECK(randomly_generated_coordinate.z <= 6);
    }
}

TEST_CASE("Generate random siqad::coord_t coordinate", "[layout-utils]")
{
    SECTION("two identical cells as input")
    {
        const auto randomly_generated_coordinate = random_coordinate<siqad::coord_t>({-10, -5, 0}, {-10, -5, 0});
        CHECK(randomly_generated_coordinate.x == -10);
        CHECK(randomly_generated_coordinate.y == -5);
        CHECK(randomly_generated_coordinate.z == 0);

        const auto randomly_generated_coordinate_second = random_coordinate<siqad::coord_t>({1, 0, 0}, {1, 0, 0});
        CHECK(randomly_generated_coordinate_second.x == 1);
        CHECK(randomly_generated_coordinate_second.y == 0);
        CHECK(randomly_generated_coordinate_second.z == 0);
    }

    SECTION("two unidentical cells as input, correct order")
    {
        const auto randomly_generated_coordinate = random_coordinate<siqad::coord_t>({-10, -1, 0}, {-10, -1, 1});
        CHECK(randomly_generated_coordinate.x == -10);
        CHECK(randomly_generated_coordinate.y == -1);
        CHECK(randomly_generated_coordinate.z <= 1);
    }

    SECTION("two unidentical cells as input, switched correct order")
    {
        const auto randomly_generated_coordinate = random_coordinate<siqad::coord_t>({-10, -1, 1}, {-10, -1, 0});
        CHECK(randomly_generated_coordinate.x == -10);
        CHECK(randomly_generated_coordinate.y == -1);
        CHECK(randomly_generated_coordinate.z <= 1);
    }
}

TEST_CASE("Generate all cells in area spanned by two cells", "[layout-utils]")
{
    SECTION("two identical cells")
    {
        const auto all_area_cells = all_sidbs_in_spanned_area({-10, -5, 0}, {-10, -5, 0});
        REQUIRE(all_area_cells.size() == 1);
        const auto first_cell = all_area_cells.front();
        CHECK(first_cell.x == -10);
        CHECK(first_cell.y == -5);
        CHECK(first_cell.z == 0);
    }

    SECTION("two cells at the same y and z coordinate ")
    {
        const auto all_area_cells = all_sidbs_in_spanned_area({-10, -5, 0}, {10, -5, 0});
        REQUIRE(all_area_cells.size() == 21);
        const auto first_cell = all_area_cells.front();
        CHECK(first_cell.x == -10);
        CHECK(first_cell.y == -5);
        CHECK(first_cell.z == 0);

        const auto final_cell = all_area_cells.back();
        CHECK(final_cell.x == 10);
        CHECK(final_cell.y == -5);
        CHECK(final_cell.z == 0);
    }

    SECTION("two cells at the same y coordinate ")
    {
        const auto all_area_cells = all_sidbs_in_spanned_area({-10, 5, 0}, {10, 5, 1});
        REQUIRE(all_area_cells.size() == 42);
        const auto first_cell = all_area_cells.front();
        CHECK(first_cell.x == -10);
        CHECK(first_cell.y == 5);
        CHECK(first_cell.z == 0);

        const auto final_cell = all_area_cells.back();
        CHECK(final_cell.x == 10);
        CHECK(final_cell.y == 5);
        CHECK(final_cell.z == 1);
    }

    SECTION("two cells at the same x coordinate ")
    {
        const auto all_area_cells = all_sidbs_in_spanned_area({10, 2, 0}, {10, 5, 1});
        REQUIRE(all_area_cells.size() == 8);
        const auto first_cell = all_area_cells.front();
        CHECK(first_cell.x == 10);
        CHECK(first_cell.y == 2);
        CHECK(first_cell.z == 0);

        const auto final_cell = all_area_cells.back();
        CHECK(final_cell.x == 10);
        CHECK(final_cell.y == 5);
        CHECK(final_cell.z == 1);
    }
}
