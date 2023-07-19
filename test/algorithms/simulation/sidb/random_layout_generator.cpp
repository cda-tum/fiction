//
// Created by Jan Drewniok on 04.05.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/path_finding/distance.hpp>
#include <fiction/algorithms/simulation/sidb/random_layout_generator.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>

#include <sstream>
#include <vector>

using namespace fiction;

TEST_CASE("Random cube::coord_t layout generation", "[generate_random_layout]")
{
    using cube_layout = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<cube::coord_t>>>;

    SECTION("empty parameters")
    {
        const generate_random_layout_params<cube_layout> params{};

        const auto lyt = generate_random_layout<cube_layout>(cube_layout{}, params);

        CHECK(lyt.num_cells() == 0);
        CHECK(lyt.x() == 0);
        CHECK(lyt.y() == 0);
    }
}
//
//    SECTION("given corner coordinates, wrong order")
//    {
//        const generate_random_layout_params<cube_layout> params{{{5, 7, 2}, {-10, -10, 0}}};
//
//        const auto result_lyt = generate_random_layout<cube_layout>(cube_layout{}, params);
//
//        CHECK(result_lyt.num_cells() == 0);
//        result_lyt.foreach_cell(
//            [](const auto& cell)
//            {
//                CHECK(cell.x == 0);
//                CHECK(cell.y == 0);
//                CHECK(cell.z == 0);
//            });
//    }
//
//    SECTION("given corner coordinates")
//    {
//        const generate_random_layout_params<cube_layout> params{{{-10, -10, 0}, {5, 7, 2}}};
//
//        const auto result_lyt = generate_random_layout<cube_layout>(cube_layout{}, params);
//
//        CHECK(result_lyt.num_cells() == 0);
//        result_lyt.foreach_cell(
//            [](const auto& cell)
//            {
//                CHECK(cell.x == 0);
//                CHECK(cell.y == 0);
//                CHECK(cell.z == 0);
//            });
//    }
//
//    SECTION("given two identical coordinates")
//    {
//        const generate_random_layout_params<cube_layout> params{{{-10, -10, 1}, {-10, -10, 1}}, 1};
//
//        const auto result_lyt = generate_random_layout<cube_layout>(cube_layout{}, params);
//
//        CHECK(result_lyt.num_cells() == 1);
//        result_lyt.foreach_cell(
//            [](const auto& cell)
//            {
//                CHECK(cell.x == -10);
//                CHECK(cell.y == -10);
//                CHECK(cell.z == 1);
//            });
//    }
//
//    SECTION("given corner coordinates and number of placed SiDBs")
//    {
//        const generate_random_layout_params<cube_layout> params{{{-10, -10, 0}, {5, 7, 1}}, 10};
//
//        const auto result_lyt = generate_random_layout<cube_layout>(cube_layout{}, params);
//
//        CHECK(result_lyt.num_cells() == 10);
//        result_lyt.foreach_cell(
//            [&](const auto& cell)
//            {
//                CHECK(cell.x < 6);
//                CHECK(cell.x > -11);
//                CHECK(cell.y < 8);
//                CHECK(cell.y > -11);
//                CHECK(cell.z < 21);
//                CHECK(cell.z > -1);
//            });
//    }
//
//    SECTION("given corner coordinates and number of placed SiDBs, and forbid positive charges")
//    {
//        const generate_random_layout_params<cube_layout> params{{{0, 0, 0}, {90, 90, 0}},
//                                                                100,
//                                                                positive_charges::ALLOWED};
//
//        const auto result_lyt = generate_random_layout<cube_layout>(cube_layout{}, params);
//
//        CHECK(result_lyt.num_cells() == 100);
//        result_lyt.foreach_cell(
//            [](const auto& cell)
//            {
//                CHECK(cell.x < 91);
//                CHECK(cell.y < 91);
//            });
//    }
//
//    SECTION("given corner coordinates and number of placed SiDBs, and allow positive charges")
//    {
//        const generate_random_layout_params<cube_layout> params{{{0, 0, 0}, {90, 90, 0}},
//                                                                100,
//                                                                positive_charges::FORBIDDEN};
//
//        const auto result_lyt = generate_random_layout<cube_layout>(cube_layout{}, params);
//
//        CHECK(result_lyt.num_cells() == 100);
//        result_lyt.foreach_cell(
//            [](const auto& cell)
//            {
//                CHECK(cell.x < 91);
//                CHECK(cell.y < 91);
//            });
//        // check if all cells are not closer than two cells (Euclidean distance).
//        result_lyt.foreach_cell(
//            [&result_lyt](const auto& cell_one)
//            {
//                result_lyt.foreach_cell(
//                    [&cell_one, &result_lyt](const auto& cell_two)
//                    {
//                        if (cell_one != cell_two)
//                        {
//                            CHECK(euclidean_distance<cube_layout>(result_lyt, cell_one, cell_two) >= 2);
//                        }
//                    });
//            });
//    }
//
//    SECTION("given previous layouts")
//    {
//        const generate_random_layout_params<cube_layout> params{
//            {{-5, -2}, {9, 9}}, 10, positive_charges::FORBIDDEN, 2, static_cast<uint64_t>(10E6), 3};
//        const auto result_lyts = generate_multiple_random_layouts<cube_layout>(cube_layout{}, params);
//        CHECK(result_lyts.size() == 3);
//
//        for (const auto& lyt : result_lyts)
//        {
//            lyt.foreach_cell(
//                [](const auto& cell)
//                {
//                    CHECK(cell.x < 10);
//                    CHECK(cell.x > -6);
//                    CHECK(cell.y < 10);
//                    CHECK(cell.y > -3);
//                });
//        }
//    }
//
//    SECTION("Check uniqueness of two layouts")
//    {
//        const generate_random_layout_params<cube_layout> params{
//            {{0, 0}, {9, 9}}, 10, positive_charges::FORBIDDEN, 2, static_cast<uint64_t>(10E6), 2};
//        const auto result_lyts = generate_multiple_random_layouts<cube_layout>(cube_layout{}, params);
//        REQUIRE(result_lyts.size() == 2);
//
//        const auto& first_lyt  = result_lyts.front();
//        const auto& second_lyt = result_lyts.back();
//
//        uint64_t counter_different_cell = 0;
//        first_lyt.foreach_cell(
//            [&second_lyt, &counter_different_cell](const auto& cell_first)
//            {
//                second_lyt.foreach_cell(
//                    [&cell_first, &counter_different_cell](const auto& cell_second)
//                    {
//                        if (cell_first != cell_second)
//                        {
//                            counter_different_cell += 1;
//                        };
//                    });
//            });
//        CHECK(counter_different_cell != 0);
//    }
//}
//
// TEST_CASE("Random offset::ucoord_t layout generation", "[generate_random_layout]")
//{
//    SECTION("empty parameters")
//    {
//        const generate_random_layout_params<sidb_cell_clk_lyt> params{};
//
//        const auto lyt = generate_random_layout<sidb_cell_clk_lyt>(sidb_cell_clk_lyt{}, params);
//
//        CHECK(lyt.num_cells() == 0);
//        CHECK(lyt.x() == 0);
//        CHECK(lyt.y() == 0);
//    }
//
//    SECTION("given corner coordinates")
//    {
//        const generate_random_layout_params<sidb_cell_clk_lyt> params{{{1, 1, 0}, {5, 7, 2}}};
//
//        const auto result_lyt = generate_random_layout<sidb_cell_clk_lyt>(sidb_cell_clk_lyt{}, params);
//
//        CHECK(result_lyt.num_cells() == 0);
//        result_lyt.foreach_cell(
//            [&](const auto& cell)
//            {
//                CHECK(cell.x == 0);
//                CHECK(cell.y == 0);
//                CHECK(cell.z == 0);
//            });
//    }
//
//    SECTION("given two identical coordinates")
//    {
//        const generate_random_layout_params<sidb_cell_clk_lyt> params{{{5, 5, 1}, {5, 5, 1}}, 1};
//
//        const auto result_lyt = generate_random_layout<sidb_cell_clk_lyt>(sidb_cell_clk_lyt{}, params);
//
//        CHECK(result_lyt.num_cells() == 1);
//        result_lyt.foreach_cell(
//            [](const auto& cell)
//            {
//                CHECK(cell.x == 5);
//                CHECK(cell.y == 5);
//                CHECK(cell.z == 1);
//            });
//    }
//
//    SECTION("given corner coordinates and number of placed SiDBs")
//    {
//        const generate_random_layout_params<sidb_cell_clk_lyt> params{{{1, 1, 0}, {50, 7, 1}}, 10};
//
//        const auto result_lyt = generate_random_layout<sidb_cell_clk_lyt>(sidb_cell_clk_lyt{}, params);
//
//        CHECK(result_lyt.num_cells() == 10);
//        result_lyt.foreach_cell(
//            [](const auto& cell)
//            {
//                CHECK(cell.x < 51);
//                CHECK(cell.x > 0);
//                CHECK(cell.y < 8);
//                CHECK(cell.y > 0);
//                CHECK(cell.z < 21);
//                CHECK(cell.z >= 0);
//            });
//    }
//
//    SECTION("given corner coordinates and number of placed SiDBs, and forbid positive charges")
//    {
//        const generate_random_layout_params<sidb_cell_clk_lyt> params{{{0, 0, 0}, {90, 90, 0}},
//                                                                      100,
//                                                                      positive_charges::ALLOWED};
//
//        const auto result_lyt = generate_random_layout<sidb_cell_clk_lyt>(sidb_cell_clk_lyt{}, params);
//
//        CHECK(result_lyt.num_cells() == 100);
//        result_lyt.foreach_cell(
//            [](const auto& cell)
//            {
//                CHECK(cell.x < 91);
//                CHECK(cell.y < 91);
//            });
//    }
//
//    SECTION("given corner coordinates and number of placed SiDBs, and allow positive charges")
//    {
//        const generate_random_layout_params<sidb_cell_clk_lyt> params{{{0, 0, 0}, {90, 90, 0}},
//                                                                      100,
//                                                                      positive_charges::FORBIDDEN};
//
//        const auto result_lyt = generate_random_layout<sidb_cell_clk_lyt>(sidb_cell_clk_lyt{}, params);
//
//        CHECK(result_lyt.num_cells() == 100);
//        result_lyt.foreach_cell(
//            [](const auto& cell)
//            {
//                CHECK(cell.x < 91);
//                CHECK(cell.y < 91);
//            });
//        // check if all cells are not closer than two cells (Euclidean distance).
//        result_lyt.foreach_cell(
//            [&result_lyt](const auto& cell_one)
//            {
//                result_lyt.foreach_cell(
//                    [&cell_one, &result_lyt](const auto& cell_two)
//                    {
//                        if (cell_one != cell_two)
//                        {
//                            CHECK(euclidean_distance<sidb_cell_clk_lyt>(result_lyt, cell_one, cell_two) >= 2);
//                        }
//                    });
//            });
//    }
//
//    SECTION("given previous layouts")
//    {
//        const generate_random_layout_params<sidb_cell_clk_lyt> params{
//            {{0, 0}, {9, 9, 2}}, 10, positive_charges::FORBIDDEN, 2, static_cast<uint64_t>(10E6), 3};
//        const auto result_lyts = generate_multiple_random_layouts<sidb_cell_clk_lyt>(sidb_cell_clk_lyt{}, params);
//        CHECK(result_lyts.size() == 3);
//
//        for (const auto& lyt : result_lyts)
//        {
//            lyt.foreach_cell(
//                [](const auto& cell)
//                {
//                    CHECK(cell.x < 10);
//                    CHECK(cell.y < 10);
//                });
//        }
//    }
//
//    SECTION("Check uniqueness of two layouts")
//    {
//        const generate_random_layout_params<sidb_cell_clk_lyt> params{
//            {{0, 0}, {9, 9}}, 10, positive_charges::FORBIDDEN, 2, static_cast<uint64_t>(10E6), 2};
//        const auto result_lyts = generate_multiple_random_layouts<sidb_cell_clk_lyt>(sidb_cell_clk_lyt{}, params);
//        REQUIRE(result_lyts.size() == 2);
//
//        const auto& first_lyt  = result_lyts.front();
//        const auto& second_lyt = result_lyts.back();
//
//        uint64_t counter_different_cell = 0;
//        first_lyt.foreach_cell(
//            [&second_lyt, &counter_different_cell](const auto& cell_first)
//            {
//                second_lyt.foreach_cell(
//                    [&cell_first, &counter_different_cell](const auto& cell_second)
//                    {
//                        if (cell_first != cell_second)
//                        {
//                            counter_different_cell += 1;
//                        };
//                    });
//            });
//        CHECK(counter_different_cell != 0);
//    }
//
//    SECTION("Check correct use of skeleton layout when generating only one random layout")
//    {
//        const generate_random_layout_params<sidb_cell_clk_lyt> params{{{0, 0}, {9, 9}}, 10};
//        sidb_cell_clk_lyt                                      skeleton_layout{};
//        skeleton_layout.assign_cell_type({0, 0}, sidb_cell_clk_lyt::technology::NORMAL);
//        skeleton_layout.assign_cell_type({9, 1}, sidb_cell_clk_lyt::technology::NORMAL);
//        skeleton_layout.assign_cell_type({5, 0}, sidb_cell_clk_lyt::technology::NORMAL);
//        const auto result_lyt = generate_random_layout<sidb_cell_clk_lyt>(skeleton_layout, params);
//
//        CHECK(result_lyt.num_cells() == 13);
//    }
//
//    SECTION("Check correct use of skeleton layout when generating multiple random layouts")
//    {
//        const generate_random_layout_params<sidb_cell_clk_lyt> params{
//            {{0, 0}, {9, 9}}, 10, positive_charges::FORBIDDEN, 2, static_cast<uint64_t>(10E6), 2};
//        sidb_cell_clk_lyt skeleton_layout{};
//        skeleton_layout.assign_cell_type({0, 0}, sidb_cell_clk_lyt::technology::NORMAL);
//        skeleton_layout.assign_cell_type({3, 0}, sidb_cell_clk_lyt::technology::NORMAL);
//        skeleton_layout.assign_cell_type({9, 1}, sidb_cell_clk_lyt::technology::NORMAL);
//        const auto result_lyts = generate_multiple_random_layouts<sidb_cell_clk_lyt>(skeleton_layout, params);
//
//        REQUIRE(result_lyts.size() == 2);
//        CHECK(result_lyts.front().num_cells() == 13);
//        CHECK(result_lyts.back().num_cells() == 13);
//    }
//}
//
// TEST_CASE("Random siqad::coord_t layout generation", "[generate_random_layout]")
//{
//    SECTION("empty parameters")
//    {
//        const generate_random_layout_params<sidb_cell_clk_lyt> params{};
//
//        const auto lyt = generate_random_layout<sidb_cell_clk_lyt>(sidb_cell_clk_lyt{}, params);
//
//        CHECK(lyt.num_cells() == 0);
//        CHECK(lyt.x() == 0);
//        CHECK(lyt.y() == 0);
//    }
//
//    SECTION("given two identical coordinates")
//    {
//        const generate_random_layout_params<sidb_cell_clk_lyt> params{{{5, 5, 1}, {5, 5, 1}}, 1};
//
//        const auto result_lyt = generate_random_layout<sidb_cell_clk_lyt>(sidb_cell_clk_lyt{}, params);
//
//        CHECK(result_lyt.num_cells() == 1);
//        result_lyt.foreach_cell(
//            [](const auto& cell)
//            {
//                CHECK(cell.x == 5);
//                CHECK(cell.y == 5);
//                CHECK(cell.z == 1);
//            });
//    }
//
//    SECTION("given corner coordinates")
//    {
//        const generate_random_layout_params<sidb_cell_clk_lyt> params{{{1, 1, 0}, {5, 7, 1}}};
//
//        const auto result_lyt = generate_random_layout<sidb_cell_clk_lyt>(sidb_cell_clk_lyt{}, params);
//
//        CHECK(result_lyt.num_cells() == 0);
//        result_lyt.foreach_cell(
//            [&](const auto& cell)
//            {
//                CHECK(cell.x == 0);
//                CHECK(cell.y == 0);
//                CHECK(cell.z == 0);
//            });
//    }
//
//    SECTION("given corner coordinates and number of placed SiDBs")
//    {
//        const generate_random_layout_params<sidb_cell_clk_lyt> params{{{1, 1, 0}, {50, 7, 1}}, 10};
//
//        const auto result_lyt = generate_random_layout<sidb_cell_clk_lyt>(sidb_cell_clk_lyt{}, params);
//
//        CHECK(result_lyt.num_cells() == 10);
//        result_lyt.foreach_cell(
//            [](const auto& cell)
//            {
//                CHECK(cell.x < 51);
//                CHECK(cell.x > 0);
//                CHECK(cell.y < 8);
//                CHECK(cell.y > 0);
//                CHECK(cell.z < 21);
//                CHECK(cell.z >= 0);
//            });
//    }
//
//    SECTION("given corner coordinates and number of placed SiDBs, and forbid positive charges")
//    {
//        const generate_random_layout_params<sidb_cell_clk_lyt> params{{{0, 0, 0}, {90, 90, 0}},
//                                                                      100,
//                                                                      positive_charges::ALLOWED};
//
//        const auto result_lyt = generate_random_layout<sidb_cell_clk_lyt>(sidb_cell_clk_lyt{}, params);
//
//        CHECK(result_lyt.num_cells() == 100);
//        result_lyt.foreach_cell(
//            [](const auto& cell)
//            {
//                CHECK(cell.x < 91);
//                CHECK(cell.y < 91);
//            });
//    }
//
//    SECTION("given corner coordinates and number of placed SiDBs, and allow positive charges")
//    {
//        const generate_random_layout_params<sidb_cell_clk_lyt> params{{{0, 0, 0}, {90, 90, 0}},
//                                                                      100,
//                                                                      positive_charges::FORBIDDEN};
//
//        const auto result_lyt = generate_random_layout<sidb_cell_clk_lyt>(sidb_cell_clk_lyt{}, params);
//
//        CHECK(result_lyt.num_cells() == 100);
//        result_lyt.foreach_cell(
//            [](const auto& cell)
//            {
//                CHECK(cell.x < 91);
//                CHECK(cell.y < 91);
//                CHECK(cell.z == 0);
//            });
//        // check if all cells are not closer than two cells (Euclidean distance).
//        result_lyt.foreach_cell(
//            [&result_lyt](const auto& cell_one)
//            {
//                result_lyt.foreach_cell(
//                    [&cell_one, &result_lyt](const auto& cell_two)
//                    {
//                        if (cell_one != cell_two)
//                        {
//                            CHECK(euclidean_distance<sidb_cell_clk_lyt>(result_lyt, cell_one, cell_two) >= 2);
//                        }
//                    });
//            });
//    }
//
//    SECTION("given previous layouts")
//    {
//        const generate_random_layout_params<sidb_cell_clk_lyt> params{
//            {{0, 0, 1}, {9, 9, 1}}, 10, positive_charges::FORBIDDEN, 2, static_cast<uint64_t>(10E6), 3};
//        const auto result_lyts = generate_multiple_random_layouts<sidb_cell_clk_lyt>(sidb_cell_clk_lyt{}, params);
//        CHECK(result_lyts.size() == 3);
//
//        for (const auto& lyt : result_lyts)
//        {
//            lyt.foreach_cell(
//                [](const auto& cell)
//                {
//                    CHECK(cell.x < 10);
//                    CHECK(cell.x >= 0);
//                    CHECK(cell.y < 10);
//                    CHECK(cell.y >= 0);
//                    CHECK(cell.z == 1);
//                });
//        }
//    }
//
//    SECTION("Check uniqueness of two layouts")
//    {
//        const generate_random_layout_params<sidb_cell_clk_lyt> params{
//            {{0, 0, 1}, {9, 9, 0}}, 10, positive_charges::FORBIDDEN, 2, static_cast<uint64_t>(10E6), 2};
//        const auto result_lyts = generate_multiple_random_layouts<sidb_cell_clk_lyt>(sidb_cell_clk_lyt{}, params);
//        REQUIRE(result_lyts.size() == 2);
//
//        const auto& first_lyt  = result_lyts.front();
//        const auto& second_lyt = result_lyts.back();
//
//        first_lyt.foreach_cell(
//            [&second_lyt](const auto& cell_first)
//            { second_lyt.foreach_cell([&cell_first](const auto& cell_second) { CHECK(cell_first != cell_second); });
//            });
//    }
//}
