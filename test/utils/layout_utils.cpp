//
// Created by marcel on 28.07.22.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/hexagonal_layout.hpp>
#include <fiction/utils/layout_utils.hpp>

#include <cstdint>

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
